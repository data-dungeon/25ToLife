/* context.cpp */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"





#ifdef WIN32
#pragma optimize( "", off )
#endif


#ifdef PS2

#define	U128			16
#define	U32			4

#define	c_ra 			0 * U128
#define	c_fp 			1 * U128

#define	c_s0			( c_fp + ( 1 * U128))
#define	c_s1			( c_fp + ( 2 * U128))
#define	c_s2			( c_fp + ( 3 * U128))
#define	c_s3			( c_fp + ( 4 * U128))
#define	c_s4			( c_fp + ( 5 * U128))
#define	c_s5			( c_fp + ( 6 * U128))
#define	c_s6			( c_fp + ( 7 * U128))
#define	c_s7			( c_fp + ( 8 * U128))

#define c_vfBase 		(c_s7 + (1 * U128))
		  
#define  c_vf1		   (c_vfBase + ( 0 * U128))
#define  c_vf2   		(c_vfBase + ( 1 * U128))
#define  c_vf3   		(c_vfBase + ( 2 * U128))
#define  c_vf4   		(c_vfBase + ( 3 * U128))
#define  c_vf5   		(c_vfBase + ( 4 * U128))
#define  c_vf6   		(c_vfBase + ( 5 * U128))
#define  c_vf7   		(c_vfBase + ( 6 * U128))
#define  c_vf8   		(c_vfBase + ( 7 * U128))
#define  c_vf9   		(c_vfBase + ( 8 * U128))
#define  c_vf10  		(c_vfBase + ( 9 * U128))
#define  c_vf11  		(c_vfBase + ( 10 * U128))
#define  c_vf12  		(c_vfBase + ( 11 * U128))
#define  c_vf13  		(c_vfBase + ( 12 * U128))
#define  c_vf14 		(c_vfBase + ( 13 * U128))
#define  c_vf15  		(c_vfBase + ( 14 * U128))

#define  c_vfacc		(c_vfBase + ( 15 * U128))
#define  c_vfstat1	(c_vfBase + ( 16 * U128))
#define  c_vfstat2	(c_vfBase + ( 17 * U128))

#define c_viBase		(c_vfstat2 + ( 1 * U128))
#define c_vi1			(c_viBase + ( 0 * U32))
#define c_vi2			(c_viBase + ( 1 * U32))
#define c_vi3			(c_viBase + ( 2 * U32))
#define c_vi4			(c_viBase + ( 3 * U32))
#define c_vi5			(c_viBase + ( 4 * U32))
#define c_vi6			(c_viBase + ( 5 * U32))
#define c_vi7			(c_viBase + ( 6 * U32))
#define c_vi8			(c_viBase + ( 7 * U32))
#define c_vi9			(c_viBase + ( 8 * U32))
#define c_vi10			(c_viBase + ( 9 * U32))
#define c_vi11			(c_viBase + ( 10 * U32))
#define c_vi12			(c_viBase + ( 11 * U32))
#define c_vi13			(c_viBase + ( 12 * U32))
#define c_vi14			(c_viBase + ( 13 * U32))
#define c_vi15			(c_viBase + ( 14 * U32))

#define  c_fBase		(c_viBase + ( 15 * U32))


#define	c_f0			(c_fBase + ( 0 * U32))
#define	c_f1			(c_fBase + ( 1 * U32))
#define	c_f2			(c_fBase + ( 2 * U32))
#define	c_f3			(c_fBase + ( 3 * U32))
#define	c_f4			(c_fBase + ( 4 * U32))
#define	c_f5			(c_fBase + ( 5 * U32))
#define	c_f6			(c_fBase + ( 6 * U32))
#define	c_f7			(c_fBase + ( 7 * U32))
#define	c_f8			(c_fBase + ( 8 * U32))
#define	c_f9			(c_fBase + ( 9 * U32))
#define	c_f10			(c_fBase + (10 * U32))
#define	c_f11			(c_fBase + (11 * U32))
#define	c_f12			(c_fBase + (12 * U32))
#define	c_f13			(c_fBase + (13 * U32))
#define	c_f14			(c_fBase + (14 * U32))
#define	c_f15			(c_fBase + (15 * U32))
#define	c_f16			(c_fBase + (16 * U32))
#define	c_f17			(c_fBase + (17 * U32))
#define	c_f18			(c_fBase + (18 * U32))
#define	c_f19			(c_fBase + (19 * U32))
#define	c_f20			(c_fBase + (20 * U32))
#define	c_f21			(c_fBase + (21 * U32))
#define	c_f22			(c_fBase + (22 * U32))
#define	c_f23			(c_fBase + (23 * U32))
#define	c_f24			(c_fBase + (24 * U32))
#define	c_f25			(c_fBase + (25 * U32))
#define	c_f26			(c_fBase + (26 * U32))
#define	c_f27			(c_fBase + (27 * U32))
#define	c_f28			(c_fBase + (28 * U32))
#define	c_f29			(c_fBase + (29 * U32))
#define	c_f30			(c_fBase + (30 * U32))
#define	c_f31			(c_fBase + (31 * U32))

#define	c_cr0			(c_f31 + (1 * U32))
#define	c_cr31		(c_f31 + (2 * U32))

/* keep stack 16-byte aligned */
#define	c_total		(c_cr31 + (2 * U32) + 8)

#endif

#ifdef GCN
#define U32		1
#define U64		2

#define c_lr		(1 * U32)
#define c_cr		(2 * U32)

#define c_fp14		(c_cr + ( 1 * U64))
#define c_fp15		(c_cr + ( 2 * U64))
#define c_fp16		(c_cr + ( 3 * U64))
#define c_fp17		(c_cr + ( 4 * U64))
#define c_fp18		(c_cr + ( 5 * U64))
#define c_fp19		(c_cr + ( 6 * U64))
#define c_fp20		(c_cr + ( 7 * U64))
#define c_fp21		(c_cr + ( 8 * U64))
#define c_fp22		(c_cr + ( 9 * U64))
#define c_fp23		(c_cr + (10 * U64))
#define c_fp24		(c_cr + (11 * U64))
#define c_fp25		(c_cr + (12 * U64))
#define c_fp26		(c_cr + (13 * U64))
#define c_fp27		(c_cr + (14 * U64))
#define c_fp28		(c_cr + (15 * U64))
#define c_fp29		(c_cr + (16 * U64))
#define c_fp30		(c_cr + (17 * U64))
#define c_fp31		(c_cr + (18 * U64))

#define c_r14		(c_fp31 + ( 1 * U32))
#define c_r15		(c_fp31 + ( 2 * U32))
#define c_r16		(c_fp31 + ( 3 * U32))
#define c_r17		(c_fp31 + ( 4 * U32))
#define c_r18		(c_fp31 + ( 5 * U32))
#define c_r19		(c_fp31 + ( 6 * U32))
#define c_r20		(c_fp31 + ( 7 * U32))
#define c_r21		(c_fp31 + ( 8 * U32))
#define c_r22		(c_fp31 + ( 9 * U32))
#define c_r23		(c_fp31 + (10 * U32))
#define c_r24		(c_fp31 + (11 * U32))
#define c_r25		(c_fp31 + (12 * U32))
#define c_r26		(c_fp31 + (13 * U32))
#define c_r27		(c_fp31 + (14 * U32))
#define c_r28		(c_fp31 + (15 * U32))
#define c_r29		(c_fp31 + (16 * U32))
#define c_r30		(c_fp31 + (17 * U32))
#define c_r31		(c_fp31 + (18 * U32))

#define c_total	(c_r31 + U64)

#endif


extern "C"
{
	void ctxSwitchToTask( ts_Context *proc, ts_Context *procSave);
}

void ctxInit( ts_Context *pNewContext, void (*pWakeAddr)(void), void *pLocalData, s32 s32StackSize, s32 s32LocalDataSize )
{
#ifdef WIN32
	u32 tempFd;
	
	
	s32 s32TopOfStack = (s32LocalDataSize + s32StackSize)/sizeof(s32);
	u32 *pStack = (u32 *) pLocalData;

  	_asm
	{
		pushfd
		pop		eax
		mov		tempFd, eax
	}


	// Positions -1 &  -2  would be occupied with the two stack parameters when calling ctxSwitchToTask
	// Position  -3 return address
	// Position  -4 Stack base
	// Positions -5 to -12 Are slots for restoring pushed registers.
	pStack[s32TopOfStack - 3] = (u32)(pWakeAddr);
	pStack[s32TopOfStack - 4] = (u32)(pStack + (s32TopOfStack - 3));
	pStack[s32TopOfStack - 5] = tempFd;


	// Point the stack base at the position of the stack holding the stack base(line above).
	pNewContext->pStackBase = (unsigned char *)&pStack[s32TopOfStack - 4];
	pNewContext->pStack		= (unsigned char *)&pStack[s32TopOfStack - 12];
#endif

#ifdef PS2
	s32					s32TopOfStack;

	unsigned char		*pStack = (unsigned char *)pLocalData;

	u32					cr;

	s32TopOfStack = s32LocalDataSize + s32StackSize;

	//keep top of stack 16 byte aligned
	s32TopOfStack &= ~15;

	s32TopOfStack -= 16;			//fudge just to be sure

	pNewContext->pStackBase = pStack + s32TopOfStack;

	pNewContext->pStack = pNewContext->pStackBase - c_total;

	*( (u32 *)( pNewContext->pStack + c_fp)) = (u32)pNewContext->pStackBase;
	*( (u32 *)( pNewContext->pStack + c_ra)) = (u32)pWakeAddr;
#endif

#ifdef GCN
	s32					s32TopOfStack;

	unsigned char		*pStack = (unsigned char *)pLocalData;

	u32					*pu32Stack;

	u32					cr;

	s32TopOfStack = s32LocalDataSize + s32StackSize;
	s32TopOfStack &= ~7;

	s32TopOfStack -= 16;			//fudge just to be sure

	pNewContext->pStackBase = pStack + s32TopOfStack;
	pNewContext->pStack = pStack + s32TopOfStack;

	pu32Stack = (u32 *)pNewContext->pStack;

	pu32Stack -= c_total;
	pNewContext->pStack = (unsigned char *)pu32Stack;

	asm __volatile__
	("
		mfcr	r0
		stw	r0, cr
	");

	pu32Stack[c_lr] = (u32)pWakeAddr;
	pu32Stack[c_cr] = cr;

#endif
}

void ctxSwitch( ts_Context *pNewContext, ts_Context *pSaveContext)
{
#ifndef _XBOX
		ctxSwitchToTask( pNewContext, pSaveContext );
#endif
}

#ifndef GCN
#ifndef PS2
#ifndef WIN32


void ctxKillSwitch( ts_Context *proc )
{
	return;
}


void ctxSwitchToTask( ts_Context *proc, ts_Context *procSave )
{
	return;
}
#endif
#endif
#endif
