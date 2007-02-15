#ifndef _HARDWARE_BREAKPOINT_H

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

inline void SET_WRITE_BREAKPOINT(unsigned int addr, unsigned int addrmask, unsigned int data, unsigned int datamask)
{
	unsigned int bpc = DWE | (DXE | DKE | DSE | DUE);
	if (datamask != 0)
		bpc |= DVE;
	asm __volatile__ ("
		.set noreorder
		mtdab %0
		mtdabm %1
		mtdvb %2
		mtdvbm %3
		mtbpc %4
		sync.p
		.set reorder
		" : : "r" (addr), "r" (addrmask), "r" (data), "r" (datamask), "r" (bpc)
	);
}

inline void CLEAR_WRITE_BREAKPOINT()
{
	unsigned int bpc = BED;
	asm __volatile__ ("
		.set noreorder
		mtbpc %0
		sync.p
		.set reorder
		" : : "r" (bpc)
	);
}

#endif //_HARDWARE_BREAKPOINT_H
