/*============================================================================
**
** HardwareBreakAsm_ps2.c - Hardware break asm support routines
**
** See HardwareBreak_ps2.cpp for implementation description.
**
**============================================================================*/

	.set noat
	
/*============================================================================*/
/* variables shared with the C code */
/*============================================================================*/

	.section .bss
	
	.align 6

/*	
	.global HBP_LocalStack
	.type HBP_LocalStack,@object
	.equ  STACK_SIZE, (16 * 32)
	.size HBP_LocalStack,STACK_SIZE
HBP_LocalStack:
	.space STACK_SIZE
*/
	
	.global HBP_PatchAddr1
	.type HBP_PatchAddr1,@object
	.size HBP_PatchAddr1,4
HBP_PatchAddr1:
	.space 4
	
	.global HBP_PatchValue1
	.type HBP_PatchValue1,@object
	.size HBP_PatchValue1,4
HBP_PatchValue1:
	.space 4
	
	.global HBP_PatchAddr2
	.type HBP_PatchAddr2,@object
	.size HBP_PatchAddr2,4
HBP_PatchAddr2:
	.space 4
	
	.global HBP_PatchValue2
	.type HBP_PatchValue2,@object
	.size HBP_PatchValue2,4
HBP_PatchValue2:
	.space 4
	
	.global HBP_Control
	.type HBP_Control,@object
	.size HBP_Control,4
HBP_Control:
	.space 4

	.global HBP_Address
	.type HBP_Address,@object
	.size HBP_Address,4
HBP_Address:
	.space 4

	.global HBP_AddressMask
	.type HBP_AddressMask,@object
	.size HBP_AddressMask,4
HBP_AddressMask:
	.space 4

	.global HBP_Data
	.type HBP_Data,@object
	.size HBP_Data,4
HBP_Data:
	.space 4

	.global HBP_DataMask
	.type HBP_DataMask,@object
	.size HBP_DataMask,4
HBP_DataMask:
	.space 4

	.global HBP_Start
	.type HBP_Start,@object
	.size HBP_Start,4
HBP_Start:
	.space 4

	.global HBP_End
	.type HBP_End,@object
	.size HBP_End,4
HBP_End:
	.space 4

	.global HBP_Start2
	.type HBP_Start2,@object
	.size HBP_Start2,4
HBP_Start2:
	.space 4

	.global HBP_End2
	.type HBP_End2,@object
	.size HBP_End2,4
HBP_End2:
	.space 4

	.global HBP_Start3
	.type HBP_Start3,@object
	.size HBP_Start3,4
HBP_Start3:
	.space 4

	.global HBP_End3
	.type HBP_End3,@object
	.size HBP_End3,4
HBP_End3:
	.space 4

	.global HBP_Start4
	.type HBP_Start4,@object
	.size HBP_Start4,4
HBP_Start4:
	.space 4

	.global HBP_End4
	.type HBP_End4,@object
	.size HBP_End4,4
HBP_End4:
	.space 4

	.global HBP_Start5
	.type HBP_Start5,@object
	.size HBP_Start5,4
HBP_Start5:
	.space 4

	.global HBP_End5
	.type HBP_End5,@object
	.size HBP_End5,4
HBP_End5:
	.space 4

	.global HBP_Start6
	.type HBP_Start6,@object
	.size HBP_Start6,4
HBP_Start6:
	.space 4

	.global HBP_End6
	.type HBP_End6,@object
	.size HBP_End6,4
HBP_End6:
	.space 4

	.global HBP_HandlerPtr
	.type HBP_HandlerPtr,@object
	.size HBP_HandlerPtr,4
HBP_HandlerPtr:
	.space 4
	
	.global HBP_ServiceComplete
	.type HBP_ServiceComplete,@object
	.size HBP_ServiceComplete,4
HBP_ServiceComplete:
	.space 4
	
	.section .bss
	
	.align 8
	.globl OriginalVDebugExceptionEntry
	.size OriginalVDebugExceptionEntry,128
OriginalVDebugExceptionEntry:
	.space 128

	.align 8
	.globl OriginalVCommonExceptionEntry
	.size OriginalVCommonExceptionEntry,128
OriginalVCommonExceptionEntry:
	.space 128
	
	.section .spad
	
	.align 4
	.globl SpadDmaBuffer
	.size SpadDmaBuffer,128
SpadDmaBuffer:
	.space 16 * 3


/*============================================================================*/
/* code */
/*============================================================================*/

	.section .text

.extern HBP_Registers

/*============================================================================*/

/* must be kept < 3 QW because thats how the caller expects it. */ 
.align 8
.globl VDebugExceptionEntry
.ent VDebugExceptionEntry
VDebugExceptionEntry:
.set noreorder
		nop
		nop
		j		VDebugExceptionHandler
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
.set reorder
.end VDebugExceptionEntry

/*============================================================================*/

/* must be kept < 3 QW because thats how the caller expects it. */ 
.align 8
.globl VCommonExceptionEntry
.ent VCommonExceptionEntry
VCommonExceptionEntry:
.set noreorder
		nop
		nop
		j		VCommonExceptionHandler
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
.set reorder
.end VCommonExceptionEntry

/*============================================================================
** Handler for VDebug hardware breakpiont exceptions
**============================================================================*/

.globl VDebugExceptionHandler
.ent VDebugExceptionHandler
VDebugExceptionHandler:
.set noreorder

		/* store off $k1, just in case.. */
		lui		$k0, %hi(HBP_Registers+0x1b0)
		sq		$k1, %lo(HBP_Registers+0x1b0)($k0)

		/* save regs */
		la		$k0, HBP_Registers 
		sq		$1, 0x010($k0)
		sq		$2, 0x020($k0)
		sq		$3, 0x030($k0)
		sq		$4, 0x040($k0)
		sq		$5, 0x050($k0)
		sq		$6, 0x060($k0)
		sq		$7, 0x070($k0)
		sq		$8, 0x080($k0)
		sq		$9, 0x090($k0)
		sq		$10, 0x0a0($k0)
		sq		$11, 0x0b0($k0)
		sq		$12, 0x0c0($k0)
		sq		$13, 0x0d0($k0)
		sq		$14, 0x0e0($k0)
		sq		$15, 0x0f0($k0)
		sq		$16, 0x100($k0)
		sq		$17, 0x110($k0)
		sq		$18, 0x120($k0)
		sq		$19, 0x130($k0)
		sq		$20, 0x140($k0)
		sq		$21, 0x150($k0)
		sq		$22, 0x160($k0)
		sq		$23, 0x170($k0)
		sq		$24, 0x180($k0)
		sq		$25, 0x190($k0)
/*		sq		$26, 0x1a0($k0)  */  /*k0*/
/*		sq		$27, 0x1b0($k0)  */  /*k1*/
		sq		$28, 0x1c0($k0)
		sq		$29, 0x1d0($k0)
		sq		$30, 0x1e0($k0)
		sq		$31, 0x1f0($k0)
		
		pmfhi	$2
		sq		$2, 0x200($k0)
		pmflo	$2
		sq		$2, 0x210($k0)
		
		/* use local stack */
		
/*		la		$sp, HBP_LocalStack + STACK_SIZE */
		
		/* call handler */
		
		lui		$k0, %hi(HBP_HandlerPtr)
		lw		$k0, %lo(HBP_HandlerPtr)($k0)
		jalr	$k0
		nop
		lui		$k0, %hi(HBP_ServiceComplete)
		sw		$v0, %lo(HBP_ServiceComplete)($k0)
		
		/* restore regs */
		
		la		$k0, HBP_Registers
		
		lq		$2, 0x200($k0)
		pmthi	$2
		lq		$2, 0x210($k0)
		pmtlo	$2
		
		lq		$1, 0x010($k0)
		lq		$2, 0x020($k0)
		lq		$3, 0x030($k0)
		lq		$4, 0x040($k0)
		lq		$5, 0x050($k0)
		lq		$6, 0x060($k0)
		lq		$7, 0x070($k0)
		lq		$8, 0x080($k0)
		lq		$9, 0x090($k0)
		lq		$10, 0x0a0($k0)
		lq		$11, 0x0b0($k0)
		lq		$12, 0x0c0($k0)
		lq		$13, 0x0d0($k0)
		lq		$14, 0x0e0($k0)
		lq		$15, 0x0f0($k0)
		lq		$16, 0x100($k0)
		lq		$17, 0x110($k0)
		lq		$18, 0x120($k0)
		lq		$19, 0x130($k0)
		lq		$20, 0x140($k0)
		lq		$21, 0x150($k0)
		lq		$22, 0x160($k0)
		lq		$23, 0x170($k0)
		lq		$24, 0x180($k0)
		lq		$25, 0x190($k0)
/*		lq		$26, 0x1a0($k0)  */ /*k0*/
/*		lq		$27, 0x1b0($k0)  */ /*k1*/
		lq		$28, 0x1c0($k0)
		lq		$29, 0x1d0($k0)
		lq		$30, 0x1e0($k0)
		lq		$31, 0x1f0($k0)
		
		/* if service wasn't handled by our callback, jump to the original
		** exception vector. */
		
		lui		$k0, %hi(HBP_ServiceComplete)
		lw		$k0, %lo(HBP_ServiceComplete)($k0)
		beq		$k0, $0, UseOriginalHandler
		nop

		/* disable breakpoint exceptions.  they'll get turned back on by the
		** following handler when the code executes the "break" instructions
		** the C++ side installed. */
		
		sync.p
		sync.l
		li		$k1, 0x8000	/* BED */
		mtbpc	$k1
		sync.p
		
		lui		$k0, %hi(HBP_Registers+0x1b0)
		lq		$k1, %lo(HBP_Registers+0x1b0)($k0)         
		 
		eret
		nop
		
UseOriginalHandler:
		lui		$k0, %hi(HBP_Registers+0x1b0)
		lq		$k1, %lo(HBP_Registers+0x1b0)($k0)         
		j		OriginalVDebugExceptionEntry
		nop

.set reorder
.end VDebugExceptionHandler

/*============================================================================
** V-Common exception handler (for repatching instructions and reenabling
** the hardware breakpoint
**============================================================================*/

.globl VCommonExceptionHandler
.ent VCommonExceptionHandler
VCommonExceptionHandler:
.set noreorder

		/* did we come here because of the hardware break? */
		lui		$k0, %hi(HBP_PatchAddr1+0x20000000)
		lw		$k0, %lo(HBP_PatchAddr1+0x20000000)($k0)
		bne		$0, $k0, DoPatch
		nop
		
		j		OriginalVCommonExceptionEntry
		nop
		
DoPatch:
		/* save k1 and v0 */		
		lui		$k0, %hi(HBP_Registers+0x1b0+0x20000000)
		sq		$k1, %lo(HBP_Registers+0x1b0+0x20000000)($k0)
		sq		$2,  %lo(HBP_Registers+0x20+0x20000000)($k0)
		
		/* replace "break" with original instruction */		
		lui		$k0, %hi(HBP_PatchAddr1+0x20000000)
		lw		$k1, %lo(HBP_PatchAddr1+0x20000000)($k0)
		sw		$0,  %lo(HBP_PatchAddr1+0x20000000)($k0)
		lui		$k0, %hi(HBP_PatchValue1+0x20000000)
		lw		$k0, %lo(HBP_PatchValue1+0x20000000)($k0)
		cache	0xb, 0($k1)
		lui		$2, 0x2000
		or		$k1, $k1, $2
		sw		$k0, 0($k1)
		sync.l
		sync.p
		
		/* replace "break" with original instruction */		
		lui		$k0, %hi(HBP_PatchAddr2+0x20000000)
		lw		$k1, %lo(HBP_PatchAddr2+0x20000000)($k0)
		beq		$0, $k1, SkipPatch2
		nop
		sw		$0,  %lo(HBP_PatchAddr2+0x20000000)($k0)
		lui		$k0, %hi(HBP_PatchValue2+0x20000000)
		lw		$k0, %lo(HBP_PatchValue2+0x20000000)($k0)
		cache	0xb, 0($k1)
		lui		$2, 0x2000
		or		$k1, $k1, $2
		sw		$k0, 0($k1)
		sync.l
		sync.p
SkipPatch2:

		/* reinstate hardware breakpoint */
		sync.p
		sync.l
		lui		$k0, %hi(HBP_Address+0x20000000)
		lw		$k0, %lo(HBP_Address+0x20000000)($k0)
		mtdab	$k0
		lui		$k0, %hi(HBP_AddressMask+0x20000000)
		lw		$k0, %lo(HBP_AddressMask+0x20000000)($k0)
		mtdabm	$k0
		lui		$k0, %hi(HBP_Data+0x20000000)
		lw		$k0, %lo(HBP_Data+0x20000000)($k0)
		mtdvb	$k0
		lui		$k0, %hi(HBP_DataMask+0x20000000)
		lw		$k0, %lo(HBP_DataMask+0x20000000)($k0)
		mtdvbm	$k0
		lui		$k0, %hi(HBP_Control+0x20000000)
		lw		$k0, %lo(HBP_Control+0x20000000)($k0)
		mtbpc	$k0
		sync.p
		
		/* restore k1 and v0 */
		lui		$k0, %hi(HBP_Registers+0x1b0+0x20000000)
		lq		$k1, %lo(HBP_Registers+0x1b0+0x20000000)($k0)
		lq		$2,  %lo(HBP_Registers+0x20+0x20000000)($k0)
		
		eret
		nop
		 
.set reorder
.end VCommonExceptionHandler

/*============================================================================*/
