/* ContGCN.s Context switching assembly Code for the GameCube
// Written by ATB
// Custom Build Settings 
// Comands: $(CWFOLDER)\PowerPC_EABI_Tools\Command_Line_tools\mwasmeppc.exe -g -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s
// Output:  $(IntDir)\$(InputName).obj
*/

U32	 =	4
U64 =	8

c_lr = (1 * U32)
c_cr = (2 * U32)

c_fp14 = (c_cr + ( 1 * U64))
c_fp15 = (c_cr + ( 2 * U64))
c_fp16 = (c_cr + ( 3 * U64))
c_fp17 = (c_cr + ( 4 * U64))
c_fp18 = (c_cr + ( 5 * U64))
c_fp19 = (c_cr + ( 6 * U64))
c_fp20 = (c_cr + ( 7 * U64))
c_fp21 = (c_cr + ( 8 * U64))
c_fp22 = (c_cr + ( 9 * U64))
c_fp23 = (c_cr + (10 * U64))
c_fp24 = (c_cr + (11 * U64))
c_fp25 = (c_cr + (12 * U64))
c_fp26 = (c_cr + (13 * U64))
c_fp27 = (c_cr + (14 * U64))
c_fp28 = (c_cr + (15 * U64))
c_fp29 = (c_cr + (16 * U64))
c_fp30 = (c_cr + (17 * U64))
c_fp31 = (c_cr + (18 * U64))

c_r14 = (c_fp31 + ( 1 * U32))
c_r15 = (c_fp31 + ( 2 * U32))
c_r16 = (c_fp31 + ( 3 * U32))
c_r17 = (c_fp31 + ( 4 * U32))
c_r18 = (c_fp31 + ( 5 * U32))
c_r19 = (c_fp31 + ( 6 * U32))
c_r20 = (c_fp31 + ( 7 * U32))
c_r21 = (c_fp31 + ( 8 * U32))
c_r22 = (c_fp31 + ( 9 * U32))
c_r23 = (c_fp31 + (10 * U32))
c_r24 = (c_fp31 + (11 * U32))
c_r25 = (c_fp31 + (12 * U32))
c_r26 = (c_fp31 + (13 * U32))
c_r27 = (c_fp31 + (14 * U32))
c_r28 = (c_fp31 + (15 * U32))
c_r29 = (c_fp31 + (16 * U32))
c_r30 = (c_fp31 + (17 * U32))
c_r31 = (c_fp31 + (18 * U32))

c_total = (c_r31 + U64)

STACK_PTR_OFFSET	= 4


.text
.global	ctxSwitchToTask
.global	ctxKillSwitch

/************************************************************
/* r3 = pNewProc, r4 = pSaveProc
************************************************************/
ctxSwitchToTask:
// move our stack pointer down past save area
	addi	r1, r1, -(c_total)

// save the non-volatile floating point registers
	stfd	f14, c_fp14(r1)
	stfd	f15, c_fp15(r1)
	stfd	f16, c_fp16(r1)
	stfd	f17, c_fp17(r1)
	stfd	f18, c_fp18(r1)
	stfd	f19, c_fp19(r1)
	stfd	f20, c_fp20(r1)
	stfd	f21, c_fp21(r1)
	stfd	f22, c_fp22(r1)
	stfd	f23, c_fp23(r1)
	stfd	f24, c_fp24(r1)
	stfd	f25, c_fp25(r1)
	stfd	f26, c_fp26(r1)
	stfd	f27, c_fp27(r1)
	stfd	f28, c_fp28(r1)
	stfd	f29, c_fp29(r1)
	stfd	f30, c_fp30(r1)
	stfd	f31, c_fp31(r1)

// save the non-volatile gp registers
	stw		r14, c_r14(r1)
	stw		r15, c_r15(r1)
	stw		r16, c_r16(r1)
	stw		r17, c_r17(r1)
	stw		r18, c_r18(r1)
	stw		r19, c_r19(r1)
	stw		r20, c_r20(r1)
	stw		r21, c_r21(r1)
	stw		r22, c_r22(r1)
	stw		r23, c_r23(r1)
	stw		r24, c_r24(r1)
	stw		r25, c_r25(r1)
	stw		r26, c_r26(r1)
	stw		r27, c_r27(r1)
	stw		r28, c_r28(r1)
	stw		r29, c_r29(r1)
	stw		r30, c_r30(r1)
	stw		r31, c_r31(r1)

// save link register
	mflr	r0
	stw		r0, c_lr(r1)

// save condition register
	mfcr	r0
	stw		r0, c_cr(r1)

// save off our stack pointer
	stw		r1, STACK_PTR_OFFSET(r4)

ctxKillSwitch:
// load our stack pointer
	lwz		r1, STACK_PTR_OFFSET(r3)

// load the non-volatile floating point registers
	lfd	f14, c_fp14(r1)
	lfd	f15, c_fp15(r1)
	lfd	f16, c_fp16(r1)
	lfd	f17, c_fp17(r1)
	lfd	f18, c_fp18(r1)
	lfd	f19, c_fp19(r1)
	lfd	f20, c_fp20(r1)
	lfd	f21, c_fp21(r1)
	lfd	f22, c_fp22(r1)
	lfd	f23, c_fp23(r1)
	lfd	f24, c_fp24(r1)
	lfd	f25, c_fp25(r1)
	lfd	f26, c_fp26(r1)
	lfd	f27, c_fp27(r1)
	lfd	f28, c_fp28(r1)
	lfd	f29, c_fp29(r1)
	lfd	f30, c_fp30(r1)
	lfd	f31, c_fp31(r1)

// load the non-volatile gp registers
	lwz		r14, c_r14(r1)
	lwz		r15, c_r15(r1)
	lwz		r16, c_r16(r1)
	lwz		r17, c_r17(r1)
	lwz		r18, c_r18(r1)
	lwz		r19, c_r19(r1)
	lwz		r20, c_r20(r1)
	lwz		r21, c_r21(r1)
	lwz		r22, c_r22(r1)
	lwz		r23, c_r23(r1)
	lwz		r24, c_r24(r1)
	lwz		r25, c_r25(r1)
	lwz		r26, c_r26(r1)
	lwz		r27, c_r27(r1)
	lwz		r28, c_r28(r1)
	lwz		r29, c_r29(r1)
	lwz		r30, c_r30(r1)
	lwz		r31, c_r31(r1)

// load link register
	lwz		r0, c_lr(r1)
	mtlr	r0

// load condition register
	lwz		r0, c_cr(r1)
	mtcr	r0

// move our stack pointer up past save area
	addi	r1, r1, c_total

	blr




/************************************************************
/* r3 = pProc
************************************************************/


