//============================================================================
//=
//= HardwareBreak_ps2.cpp - Support for hardware breakpoints
//=
//= This routine uses the PS2 hardware breakpoint registers on the EE to
//= trap reads/writes to RAM in a given range.
//=
//= The hardware registers only allow protecting a power of 2 address range
//= and length.  To get around this limitation and allow us to watch
//= any address and length, we use a hardware and software implementation.
//=
//= Note: The debugger wants to clear the hardware break regs the first time
//=       it stops, so you must "run to main" and not "execute" from the
//=       debugger otherwise it won't run correctly.
//=
//= Internal description:
//=
//=    When HardwareBreak::Enable() is called, we set up a hardware break
//=    on the tightest power-of-2 region that contains the block in question.
//=
//=    We install a handler (the EE V-Debug vector) that gets called when
//=    anything in that range is read/written to.
//=
//=    That handler looks at the address that is being accessed and will
//=    break if it is really in the range we are watching.  Otherwise
//=    it will continue normally.
//=    
//=    To continue normally, we have to disable the write breakpoint
//=    temporarily, otherwise we'd have an infinite loop.  So we disable it
//=    and overwrite the next instruction with "break" (or the next 2 possible
//=    instructions if we're in a branch delay slot.).
//=
//=    We install a handler (the EE V-Common vector) that gets called when
//=    the break instruction is hit.  At which time we replace the original
//=    instructions, re-enable the hardware break registers, and resume.
//=
//=    Careful attention must be paid to cache usage.  The EE-VDebug handler
//=    is a level-2 exception and runs in kernel mode.  Kernel mode is
//=    "unmapped", which means no TLB and more importantly no cache.
//=    Thus all variables that it reads and writes could be seen incorrectly
//=    outside the handler.  This is why all the usages of UNCACHED
//=    to access the HBP_* vars in all the routines (except
//=    HardwareBreak::Handler itself).
//=
//============================================================================

#include <eekernel.h>
#include <eeregs.h>
#include "HardwareBreak.h"
#include "Layers/exception/exception.h"

#define UNCACHED(x) ((int*)(((int)(x)) | 0x20000000))
#define PHYSICAL(x) ((u32)(x) & 0x0fffffff)

struct Registers
{
	u128 r[32];
	u128 hilo[2];
}; 

//============================================================================

// Global variables that aren't in the namespace because the asm file
// needs to reference them.

extern "C" {
	extern int VDebugExceptionEntry[];
	extern int VCommonExceptionEntry[];
	extern int OriginalVDebugExceptionEntry[];
	extern int OriginalVCommonExceptionEntry[];
	extern int SpadDmaBuffer[];
	extern u32 HBP_Control;
	extern u32 HBP_Address;
	extern u32 HBP_AddressMask;
	extern u32 HBP_Data;
	extern u32 HBP_DataMask;
	extern u32 HBP_Start;
	extern u32 HBP_End;
	extern u32 HBP_Start2;
	extern u32 HBP_End2;
	extern u32 HBP_Start3;
	extern u32 HBP_End3;
	extern u32 HBP_Start4;
	extern u32 HBP_End4;
	extern u32 HBP_Start5;
	extern u32 HBP_End5;
	extern u32 HBP_Start6;
	extern u32 HBP_End6;
	extern u32 HBP_HandlerPtr;
	extern u32 HBP_PatchAddr1;
	extern u32 HBP_PatchAddr2;
	extern u32 HBP_PatchValue1;
	extern u32 HBP_PatchValue2;
};

Registers	 HBP_Registers;

//============================================================================

namespace HardwareBreak
{
	void PatchVector(u32 dest, u32 src);
	void InvalidICache(void* src, u_int qwc);
	u32 GetCop0Status();
	void SetCop0Status(u32 value);
	bool Handler();
	u32 ReadWriteAddress(u32 epc);
	u32 BranchTarget(u32 epc);

	bool s_initialized = false;
	TrapFunc s_trapHandler = 0;
};

//============================================================================

// from EE core users manual version 5.0, page 80:
#define IAB (1<<0)	// Instruction address breakpoint
#define DRB (1<<1)	// Data read breakpoint establishment flag
#define DWB (1<<2)	// Data write breakpoint establishment flag
#define BED (1<<15)	// Breakpoint exception disable
#define DTE (1<<16)	// Data breakpoint - trigger generation enable
#define ITE (1<<17)	// Instruction address breakpoint - trigger generation enable
#define DXE (1<<18)	// EXL mode enable
#define DKE (1<<19)	// Kernel mode enable
#define DSE (1<<20)	// Supervisor mode enable
#define DUE (1<<21)	// User mode enable
#define DVE (1<<28)	// Value enable
#define DWE (1<<29)	// Write enable
#define DRE (1<<30)	// Read enable
#define IAE (1<<31)	// Instruction address enable

//============================================================================

void HardwareBreak::Initialize()
{
	if (!s_initialized)
	{
		s_initialized = true;

		DI();

		Disable();

		*UNCACHED(&HBP_HandlerPtr) = (u32)HardwareBreak::Handler;

		PatchVector((u32)&OriginalVDebugExceptionEntry[0], 0x00000100);
		PatchVector(0x00000100, (u32)&VDebugExceptionEntry[0]);

		PatchVector((u32)&OriginalVCommonExceptionEntry[0], 0x00000180);
		PatchVector(0x00000180, (u32)&VCommonExceptionEntry[0]);

		asm("ei");
	}
}

//============================================================================

void HardwareBreak::SetTrapHandler(TrapFunc func)
{
	*UNCACHED(&s_trapHandler) = (u32)func;
}

//============================================================================

void HardwareBreak::Enable(bool write, bool read, u32 address, u32 length)
{
	Enable(write, read, address, length, 0, 0);
}

//============================================================================

void HardwareBreak::Enable(bool write, bool read, u32 address, u32 length, u32 data, u32 datamask)
{
	if (!s_initialized)
		Initialize();

	// force the address into physical range
	address = PHYSICAL(address);
	u32 end = address + length - 1;

	// plug in to one of 6 spots
	if      (*UNCACHED(&HBP_Start) == 0)	{ *UNCACHED(&HBP_Start)  = address; *UNCACHED(&HBP_End)  = end; }
	else if (*UNCACHED(&HBP_Start2) == 0)	{ *UNCACHED(&HBP_Start2) = address; *UNCACHED(&HBP_End2) = end; }
	else if (*UNCACHED(&HBP_Start3) == 0)	{ *UNCACHED(&HBP_Start3) = address; *UNCACHED(&HBP_End3) = end; }
	else if (*UNCACHED(&HBP_Start4) == 0)	{ *UNCACHED(&HBP_Start4) = address; *UNCACHED(&HBP_End4) = end; }
	else if (*UNCACHED(&HBP_Start5) == 0)	{ *UNCACHED(&HBP_Start5) = address; *UNCACHED(&HBP_End5) = end; }
	else if (*UNCACHED(&HBP_Start6) == 0)	{ *UNCACHED(&HBP_Start6) = address; *UNCACHED(&HBP_End6) = end; }

	// figure out min and max address to use for hardware mask
	u32 min = ~0;
	u32 max = 0;
	if (*UNCACHED(&HBP_Start)  != 0 && *UNCACHED(&HBP_Start)  < min) { min = *UNCACHED(&HBP_Start);  }
	if (*UNCACHED(&HBP_Start2) != 0 && *UNCACHED(&HBP_Start2) < min) { min = *UNCACHED(&HBP_Start2); }
	if (*UNCACHED(&HBP_Start3) != 0 && *UNCACHED(&HBP_Start3) < min) { min = *UNCACHED(&HBP_Start3); }
	if (*UNCACHED(&HBP_Start4) != 0 && *UNCACHED(&HBP_Start4) < min) { min = *UNCACHED(&HBP_Start4); }
	if (*UNCACHED(&HBP_Start5) != 0 && *UNCACHED(&HBP_Start5) < min) { min = *UNCACHED(&HBP_Start5); }
	if (*UNCACHED(&HBP_Start6) != 0 && *UNCACHED(&HBP_Start6) < min) { min = *UNCACHED(&HBP_Start6); }
	if (*UNCACHED(&HBP_End)  != 0 && *UNCACHED(&HBP_End)  > max) { max = *UNCACHED(&HBP_End);  }
	if (*UNCACHED(&HBP_End2) != 0 && *UNCACHED(&HBP_End2) > max) { max = *UNCACHED(&HBP_End2); }
	if (*UNCACHED(&HBP_End3) != 0 && *UNCACHED(&HBP_End3) > max) { max = *UNCACHED(&HBP_End3); }
	if (*UNCACHED(&HBP_End4) != 0 && *UNCACHED(&HBP_End4) > max) { max = *UNCACHED(&HBP_End4); }
	if (*UNCACHED(&HBP_End5) != 0 && *UNCACHED(&HBP_End5) > max) { max = *UNCACHED(&HBP_End5); }
	if (*UNCACHED(&HBP_End6) != 0 && *UNCACHED(&HBP_End6) > max) { max = *UNCACHED(&HBP_End6); }

	// get a mask that covers min..max
	u32 ignore = 0;
	while ((min | ignore) < max)
		ignore = (ignore << 1) | 1;

	// setup control word
	u32 control = (DSE | DUE);	// DKE, DXE off
	if (write)    control |= DWE;
	if (read)     control |= DRE;
	if (datamask) control |= DVE;

	DI();

	// disable first
	u32 disable = BED;
	asm __volatile__ ("
		.set noreorder
		sync.p
		sync.l
		mtbpc %0
		sync.p
		.set reorder
		" : : "r" (disable));

	// set it up
	u32 addrmask = PHYSICAL(~ignore);
	u32 addrbase = min & addrmask;
	asm __volatile__ ("
		.set noreorder
		mtdab %0
		mtdabm %1
		mtdvb %2
		mtdvbm %3
		mtbpc %4
		sync.p
		.set reorder
		" : : "r" (addrbase), "r" (addrmask), "r" (data), "r" (datamask), "r" (control));

	// remember the details
	*UNCACHED(&HBP_Address)     = addrbase;
	*UNCACHED(&HBP_AddressMask) = addrmask;
	*UNCACHED(&HBP_Data)        = data;
	*UNCACHED(&HBP_DataMask)    = datamask;
	*UNCACHED(&HBP_Control)     = control;

	asm("ei");
}

//============================================================================

void HardwareBreak::Disable()
{
	u32 control = BED;
	asm __volatile__ ("
		.set noreorder
		sync.p
		sync.l
		mtbpc %0
		sync.p
		.set reorder
		" : : "r" (control));

	*UNCACHED(&HBP_Start)       = 0;
	*UNCACHED(&HBP_End)         = 0;
	*UNCACHED(&HBP_Start2)      = 0;
	*UNCACHED(&HBP_End2)        = 0;
	*UNCACHED(&HBP_Start3)      = 0;
	*UNCACHED(&HBP_End3)        = 0;
	*UNCACHED(&HBP_Start4)      = 0;
	*UNCACHED(&HBP_End4)        = 0;
	*UNCACHED(&HBP_Start5)      = 0;
	*UNCACHED(&HBP_End5)        = 0;
	*UNCACHED(&HBP_Start6)      = 0;
	*UNCACHED(&HBP_End6)        = 0;

	*UNCACHED(&HBP_Address)     = 0;
	*UNCACHED(&HBP_AddressMask) = 0;
	*UNCACHED(&HBP_Data)        = 0;
	*UNCACHED(&HBP_DataMask)    = 0;
	*UNCACHED(&HBP_Control)     = control;
}

//============================================================================

u32 HardwareBreak::ReadWriteAddress(u32 epc)
{
	// all read/writes share this format: [op:6] [base:5] [rt:5] [offset:16]
	u32 opcode = *(u32*)epc;
	u32 offset = (int)(short)(opcode & 0xffff);
	u32 base   = (opcode >> 21) & 0x1f;
	u32 addr   = *(u32*)&HBP_Registers.r[base] + offset;
	return addr;

	// lb		100000 [base:5] [rt:5] [offset:16]
	// lbu	100100 [base:5] [rt:5] [offset:16]
	// ld		110111 [base:5] [rt:5] [offset:16]
	// ldl	011010 [base:5] [rt:5] [offset:16]
	// ldr	011011 [base:5] [rt:5] [offset:16]
	// lh		100001 [base:5] [rt:5] [offset:16]
	// lhu	100101 [base:5] [rt:5] [offset:16]
	// lw		100011 [base:5] [rt:5] [offset:16]
	// lwl	100010 [base:5] [rt:5] [offset:16]
	// lwr	100110 [base:5] [rt:5] [offset:16]
	// lwu	100111 [base:5] [rt:5] [offset:16]
	// lq		011110 [base:5] [rt:5] [offset:16]
	// lwc1	110001 [base:5] [rt:5] [offset:16]
	// sb		101000 [base:5] [rt:5] [offset:16]
	// sd		111111 [base:5] [rt:5] [offset:16]
	// sdl	101100 [base:5] [rt:5] [offset:16]
	// sdr	101101 [base:5] [rt:5] [offset:16]
	// sh		101001 [base:5] [rt:5] [offset:16]
	// sw		101011 [base:5] [rt:5] [offset:16]
	// swl	101010 [base:5] [rt:5] [offset:16]
	// swr	101110 [base:5] [rt:5] [offset:16]
	// sq		011111 [base:5] [rt:5] [offset:16]
	// swc1	111001 [base:5] [rt:5] [offset:16]
}

//============================================================================

u32 HardwareBreak::BranchTarget(u32 epc)
{
	u32 opcode = *(u32*)epc;
	switch (opcode & 0xfc000000)
	{
		case 0x00000000:	// jr, jalr
			return HBP_Registers.r[(opcode >> 21) & 0x1f];

		case 0x08000000:	// j
		case 0x0C000000:	// jal
			return (opcode & 0x03ffffff) << 4;

		default:	// must be an offset branch
			int offset = (int)(short)(opcode & 0xffff) * 4;
			return epc + 4 + offset;
	}

	// beq     000100 [rs:5]  [rt:5]  [offset:16]
	// beql    010100 [rs:5]  [rt:5]  [offset:16]
	// bgez    000001 [rs:5]  [00001] [offset:16]
	// bgezal  000001 [rs:5]  [10001] [offset:16]
	// bgezall 000001 [rs:5]  [10011] [offset:16]
	// bgezl   000001 [rs:5]  [00011] [offset:16]
	// bgtz    000111 [rs:5]  [00000] [offset:16]
	// bgtzl   010111 [rs:5]  [00000] [offset:16]
	// blez    000110 [rs:5]  [00000] [offset:16]
	// blezl   010110 [rs:5]  [00000] [offset:16]
	// bltz    000001 [rs:5]  [00000] [offset:16]
	// bltzal  000001 [rs:5]  [10000] [offset:16]
	// bltzall 000001 [rs:5]  [10010] [offset:16]
	// bltzl   000001 [rs:5]  [00010] [offset:16]
	// bne     000101 [rs:5]  [rt:5]  [offset:16]
	// bnel    010101 [rs:5]  [rt:5]  [offset:16]
	// bc0f    010000 [01000] [00000] [offset:16]
	// bc0fl   010000 [01000] [00010] [offset:16]
	// bc0t    010000 [01000] [00001] [offset:16]
	// bc0tl   010000 [01000] [00011] [offset:16]
	// bc1f    010001 [01000] [00000] [offset:16]
	// bc1fl   010001 [01000] [00010] [offset:16]
	// bc1t    010001 [01000] [00001] [offset:16]
	// bc1tl   010001 [01000] [00011] [offset:16]
	// bc2f    010010 [01000] [00000] [offset:16]
	// bc2fl   010010 [01000] [00010] [offset:16]
	// bc2t    010010 [01000] [00001] [offset:16]
	// bc2tl   010010 [01000] [00011] [offset:16]
	// j       000010 [offset:26]
	// jal     000011 [offset:26]
	// jalr    000000 [rs:5] [00000] [rd:5] [00000] [001001]
	// jr      000000 [rs:5] [000 0000 0000 0000] [001000]
}

//============================================================================

bool HardwareBreak::Handler()
{
	// CALLED INSIDE LEVEL 2 EXCEPTION, ALL VALUES ARE NON-CACHED
	u32 cause, epc;
	asm __volatile__ ("
		.set noreorder
		mfc0 %0, $13
		mfc0 %1, $30
		.set reorder
		": "=r" (cause), "=r" (epc));
	bool delayslot = (cause & (1<<30)) != 0;

	// what was the memory address being accessed?
	u32 addr = ReadWriteAddress(delayslot ? epc + 4 : epc);
	addr = PHYSICAL(addr);
	if ((HBP_Start  != 0 && addr >= HBP_Start  && addr <= HBP_End ) ||
		 (HBP_Start2 != 0 && addr >= HBP_Start2 && addr <= HBP_End2) ||
		 (HBP_Start3 != 0 && addr >= HBP_Start3 && addr <= HBP_End3) ||
		 (HBP_Start4 != 0 && addr >= HBP_Start4 && addr <= HBP_End4) ||
		 (HBP_Start5 != 0 && addr >= HBP_Start5 && addr <= HBP_End5) ||
		 (HBP_Start6 != 0 && addr >= HBP_Start6 && addr <= HBP_End6))
	{
		// BREAK!
		if (s_trapHandler)
		{
			static char buf[256];
			sprintf(buf, "PROTECTED MEMORY BEING USED:\n"
								"[%08x..%08x] A=%08x\n", HBP_Start, HBP_End, addr);
			ExceptionHandler::SetExceptionMessage(buf);

			// we can't jump to a blue screen cleanly in a level-2 exception,
			// so fake the status regs so we can.
			u32 status = 0x70030c10;
			u32 disable = BED;
			asm __volatile__ ("
				.set noreorder
				sync.p
				sync.l
				mtbpc %1
				sync.p
				mtc0 %0, $12
				sync.p
				sync.l
				.set reorder
				" : : "r" (status), "r" (disable));

			s_trapHandler(0, cause, epc, addr, 0, &HBP_Registers.r[0]);
		}
		else
		{
			// we didn't service, so use the original vector so the debugger will catch it.
			return false;
		}
	}
	else
	{
		// put "break" instructions in the next instruction(s) so we can
		// reenable the hardware breakpoint after this instruction executes
		u32 addr1 = epc + 4;
		u32 addr2 = 0;
		if (delayslot)
		{
			addr1 += 4;
			addr2 = BranchTarget(epc);
		}

		HBP_PatchAddr1  = addr1;
		HBP_PatchValue1 = *(volatile u32*)addr1;
		*(volatile u32*)addr1 = 0x4d;	// "break 1"
		asm __volatile__ ("
			.set noreorder
			sync.l
			sync.p
			cache 0x7, 0(%0)
			cache 0x7, 1(%0)
			cache 0xc, 0($0)
			.set reorder
			" : : "r" (addr1));

		if (addr2 && addr2 != addr1)
		{
			HBP_PatchAddr2  = addr2;
			HBP_PatchValue2 = *(volatile u32*)addr2;
			*(volatile u32*)addr2 = 0x4d;	// "break 1"
			asm __volatile__ ("
				.set noreorder
				cache 0x7, 0(%0)
				cache 0x7, 1(%0)
				.set reorder
				" : : "r" (addr2));
		}
	}
	return true;
}

//============================================================================

u32 HardwareBreak::GetCop0Status()
{
	u32 ret;
	asm __volatile__ ("
		.set noreorder
		sync.p
		mfc0 %0, $12
		.set reorder
		" : "=r" (ret));
	return ret;
}

//============================================================================

void HardwareBreak::SetCop0Status(u32 value)
{
	asm __volatile__ ("
		.set noreorder
		sync.p
		mtc0 %0, $12
		sync.p
		.set reorder
		" : : "r" (value));
}

//============================================================================

void HardwareBreak::InvalidICache(void* src, u_int qwc)
{
	asm __volatile__("
		sync.l
		sync.p
		cache 0x0c, 0x0($0)
	0:
		cache 0x0b, 0x0(%0)
		addiu %0, %0, 0x40
		nop
		addi %1, %1, -4
		nop
		bgtz %1, 0b
		nop
		sync.p
		sync.l
		" : "+r" (src) : "r" (qwc));
}

//============================================================================

void HardwareBreak::PatchVector(u32 dest, u32 src)
{
	// wait for pending dma's to scratchpad to finish
	while (*D9_CHCR & 0x100);

	// setup dma to scratchpad
	*D9_MADR = src;
	*D9_QWC = 3;
	*D9_SADR = (u32)SpadDmaBuffer & 0xffff;
	*D9_CHCR = 0x100;

	// wait for completion
	while (*D9_CHCR & 0x100);

	// wait for pending dma's from scratchpad to finish
	while (*D8_CHCR & 0x100);

	// setup dma from scratchpad
	*D8_MADR = dest;
	*D8_QWC = 3;
	*D8_SADR = (u32)SpadDmaBuffer & 0xffff;
	*D8_CHCR = 0x100;

	// wait for completion
	while (*D8_CHCR & 0x100);

	// invalidate the cache
	u32 status = GetCop0Status();
	SetCop0Status(status & ~((1<<3) | (1<<4)));
	InvalidICache((void*)(dest | 0x80000000), 8);
	SetCop0Status(status);
}

//============================================================================

void HardwareBreak::FaultTest()
{
	u32 addr = *UNCACHED(&HBP_Start);
	if (s_initialized && addr)
	{
		// read test
		u8 value = *(volatile u8*)addr;

		// write test
		*(volatile u8*)addr = value;
	}
}

//============================================================================
