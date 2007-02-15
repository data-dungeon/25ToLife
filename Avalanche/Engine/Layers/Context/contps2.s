/* ContPs2.s Context switching assembly Code for the PS2
// Written by JTM
// Custom Build Settings 
// Comands: $(CWFOLDER)\PS2_Tools\Command_Line_tools\asm_r5900_elf.exe -endian little -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s
// Output:  $(IntDir)\$(InputName).obj
*/

.ifndef s8
/* definitions for SN Systems assembler */

	.equr zero,	$0	/* wired zero */
	.equr AT,	$at	/* assembler temp */
	.equr v0,	$2	/* return value */
	.equr v1,	$3
	.equr a0,	$4	/* argument registers */
	.equr a1,	$5
	.equr a2,	$6
	.equr a3,	$7
	.equr t0,	$8	/* caller saved */
	.equr t1,	$9
	.equr t2,	$10
	.equr t3,	$11
	.equr t4,	$12
	.equr t5,	$13
	.equr t6,	$14
	.equr t7,	$15
	.equr s0,	$16	/* callee saved */
	.equr s1,	$17
	.equr s2,	$18
	.equr s3,	$19
	.equr s4,	$20
	.equr s5,	$21
	.equr s6,	$22
	.equr s7,	$23
	.equr t8,	$24	/* code generator */
	.equr t9,	$25
	.equr k0,	$26	/* kernel temporary */
	.equr k1,	$27
	.equr gp,	$28	/* global pointer */
	.equr sp,	$29	/* stack pointer */
	.equr fp,	$30	/* frame pointer */
	.equr ra,	$31	/* return address */
	
	.equr f0,	$f0
	.equr f1,	$f1
	.equr f2,	$f2
	.equr f3,	$f3
	.equr f4,	$f4
	.equr f5,	$f5
	.equr f6,	$f6
	.equr f7,	$f7
	.equr f8,	$f8
	.equr f9,	$f9
	.equr f10,	$f10
	.equr f11,	$f11
	.equr f12,	$f12
	.equr f13,	$f13
	.equr f14,	$f14
	.equr f15,	$f15
	.equr f16,	$f16
	.equr f17,	$f17
	.equr f18,	$f18
	.equr f19,	$f19
	.equr f20,	$f20
	.equr f21,	$f21
	.equr f22,	$f22
	.equr f23,	$f23
	.equr f24,	$f24
	.equr f25,	$f25
	.equr f26,	$f26
	.equr f27,	$f27
	.equr f28,	$f28
	.equr f29,	$f29
	.equr f30,	$f30
	.equr f31,	$f31
.endif

U128		=	16
U32		=	4

/* mips registers */

c_ra =	0 * U128
c_fp =	1 * U128

c_s0 =	( c_fp + ( 1 * U128))
c_s1 =	( c_fp + ( 2 * U128))
c_s2 =	( c_fp + ( 3 * U128))
c_s3 =	( c_fp + ( 4 * U128))
c_s4 =	( c_fp + ( 5 * U128))
c_s5 =	( c_fp + ( 6 * U128))
c_s6 =	( c_fp + ( 7 * U128))
c_s7 =	( c_fp + ( 8 * U128))

/* vu0 floating point registers */

c_vfBase = (c_s7 + (1 * U128))
		  
c_vf1 =  (c_vfBase + ( 0 * U128))
c_vf2 =  (c_vfBase + ( 1 * U128))
c_vf3 =  (c_vfBase + ( 2 * U128))
c_vf4 =  (c_vfBase + ( 3 * U128))
c_vf5 =  (c_vfBase + ( 4 * U128))
c_vf6 =  (c_vfBase + ( 5 * U128))
c_vf7 =  (c_vfBase + ( 6 * U128))
c_vf8 =  (c_vfBase + ( 7 * U128))
c_vf9 =  (c_vfBase + ( 8 * U128))
c_vf10 =  (c_vfBase + ( 9 * U128))
c_vf11 =  (c_vfBase + ( 10 * U128))
c_vf12 =  (c_vfBase + ( 11 * U128))
c_vf13 =  (c_vfBase + ( 12 * U128))
c_vf14 =  (c_vfBase + ( 13 * U128))
c_vf15 =  (c_vfBase + ( 14 * U128))

/* vu0 status registers */

c_vfAcc  = (c_vfBase + (15 * U128))
c_vfStat = (c_vfAcc  + (1 * U128))
c_vfMAC  = (c_vfStat + (1 * U32))
c_vfCLIP = (c_vfMAC  + (1 * U32))
c_vfR	   = (c_vfCLIP + (1 * U32))
c_vfI	   = (c_vfR    + (1 * U32))
c_vfQ	   = (c_vfI    + (1 * U32))
c_vfFBRST= (c_vfQ    + (1 * U32))
c_vfUNUSED=(c_vfFBRST+ (1 * U32))		  		  

/* vu0 interger registers */

c_viBase = (c_vfUNUSED + (1 * U32))
c_vi1 = (c_viBase + (0 * U32))
c_vi2 = (c_viBase + (1 * U32))
c_vi3 = (c_viBase + (2 * U32))
c_vi4 = (c_viBase + (3 * U32))
c_vi5 = (c_viBase + (4 * U32))
c_vi6 = (c_viBase + (5 * U32))
c_vi7 = (c_viBase + (6 * U32))
c_vi8 = (c_viBase + (7 * U32))
c_vi9 = (c_viBase + (8 * U32))
c_vi10 = (c_viBase + (9 * U32))
c_vi11 = (c_viBase + (10 * U32))
c_vi12 = (c_viBase + (11 * U32))
c_vi13 = (c_viBase + (12 * U32))
c_vi14 = (c_viBase + (13 * U32))
c_vi15 = (c_viBase + (14 * U32))
		  
/* fpu registers */

c_fBase = (c_viBase + ( 15 * U32))

c_f0  =	(c_fBase + ( 0 * U32))
c_f1  =	(c_fBase + ( 1 * U32))
c_f2  =	(c_fBase + ( 2 * U32))
c_f3  =	(c_fBase + ( 3 * U32))
c_f4  =	(c_fBase + ( 4 * U32))
c_f5  =	(c_fBase + ( 5 * U32))
c_f6  =	(c_fBase + ( 6 * U32))
c_f7  =	(c_fBase + ( 7 * U32))
c_f8  =	(c_fBase + ( 8 * U32))
c_f9  =	(c_fBase + ( 9 * U32))
c_f10 =	(c_fBase + (10 * U32))
c_f11 =	(c_fBase + (11 * U32))
c_f12 =	(c_fBase + (12 * U32))
c_f13 =	(c_fBase + (13 * U32))
c_f14 =	(c_fBase + (14 * U32))
c_f15 =	(c_fBase + (15 * U32))
c_f16 =	(c_fBase + (16 * U32))
c_f17 =	(c_fBase + (17 * U32))
c_f18 =	(c_fBase + (18 * U32))
c_f19 =	(c_fBase + (19 * U32))
c_f20 =	(c_fBase + (20 * U32))
c_f21 =	(c_fBase + (21 * U32))
c_f22 =	(c_fBase + (22 * U32))
c_f23 =	(c_fBase + (23 * U32))
c_f24 =	(c_fBase + (24 * U32))
c_f25 =	(c_fBase + (25 * U32))
c_f26 =	(c_fBase + (26 * U32))
c_f27 =	(c_fBase + (27 * U32))
c_f28 =	(c_fBase + (28 * U32))
c_f29 =	(c_fBase + (29 * U32))
c_f30 =	(c_fBase + (30 * U32))
c_f31 =	(c_fBase + (31 * U32))

/* fpu status registers */

c_cr0  =	(c_f31 + (1 * U32))
c_cr31 = (c_f31 + (2 * U32))

/* keep stack 16-byte aligned */
c_total = ( c_cr31 + ( 2 * U32) + 8)

/* macros for saving/restoring registers, so they can be used by multiple routines */

.macro SAVE_REGISTERS

	sq ra, c_ra(sp)
	sq $30, c_fp(sp)
	sq s0, c_s0(sp)
	sq s1, c_s1(sp)
	sq s2, c_s2(sp)
	sq s3, c_s3(sp)
	sq s4, c_s4(sp)
	sq s5, c_s5(sp)
	sq s6, c_s6(sp)
	sq s7, c_s7(sp)

	sqc2 vf1, c_vf1(sp)
	sqc2 vf2, c_vf2(sp)
	sqc2 vf3, c_vf3(sp)
	sqc2 vf4, c_vf4(sp)
	sqc2 vf5, c_vf5(sp)
	sqc2 vf6, c_vf6(sp)
	sqc2 vf7, c_vf7(sp)
	sqc2 vf8, c_vf8(sp)
	sqc2 vf9, c_vf9(sp)
	sqc2 vf10, c_vf10(sp)
	sqc2 vf11, c_vf11(sp)
	sqc2 vf12, c_vf12(sp)
	sqc2 vf13, c_vf13(sp)
	sqc2 vf14, c_vf14(sp)
	sqc2 vf15, c_vf15(sp)
	  
	VMADDx.xyzw vf1, vf0, vf0
	sqc2 vf1, c_vfAcc(sp)

	cfc2 t0, vi16
	sw   t0, c_vfStat(sp)
	cfc2 t0, vi17
	sw	  t0, c_vfMAC(sp)
	cfc2 t0, vi18
	sw   t0, c_vfCLIP(sp)
	cfc2 t0, vi20
	sw   t0, c_vfR(sp)
	cfc2 t0, vi21
	sw   t0, c_vfI(sp)
	cfc2 t0, vi22
	sw   t0, c_vfQ(sp)
	cfc2 t0, vi28
	sw   t0, c_vfFBRST(sp)

	cfc2 t0, vi1
	sw   t0, c_vi1(sp)
	cfc2 t0, vi2
	sw	  t0, c_vi2(sp)
	cfc2 t0, vi3
	sw	  t0, c_vi3(sp)
	cfc2 t0, vi4
	sw	  t0, c_vi4(sp)
	cfc2 t0, vi5
	sw	  t0, c_vi5(sp)
	cfc2 t0, vi6
	sw	  t0, c_vi6(sp)
	cfc2 t0, vi7
	sw	  t0, c_vi7(sp)
	cfc2 t0, vi8
	sw	  t0, c_vi8(sp)
	cfc2 t0, vi9
	sw	  t0, c_vi9(sp)
	cfc2 t0, vi10
	sw	  t0, c_vi10(sp)
	cfc2 t0, vi11
	sw	  t0, c_vi11(sp)
	cfc2 t0, vi12
	sw	  t0, c_vi12(sp)
	cfc2 t0, vi13
	sw	  t0, c_vi13(sp)
	cfc2 t0, vi14
	sw	  t0, c_vi14(sp)
	cfc2 t0, vi15
	sw	  t0, c_vi15(sp)
	
	swc1 $f0, c_f0(sp)
	swc1 $f1, c_f1(sp)
	swc1 $f2, c_f2(sp)
	swc1 $f3, c_f3(sp)
	swc1 $f4, c_f4(sp)
	swc1 $f5, c_f5(sp)
	swc1 $f6, c_f6(sp)
	swc1 $f7, c_f7(sp)
	swc1 $f8, c_f8(sp)
	swc1 $f9, c_f9(sp)
	swc1 $f10, c_f10(sp)
	swc1 $f11, c_f11(sp)
	swc1 $f12, c_f12(sp)
	swc1 $f13, c_f13(sp)
	swc1 $f14, c_f14(sp)
	swc1 $f15, c_f15(sp)
	swc1 $f16, c_f16(sp)
	swc1 $f17, c_f17(sp)
	swc1 $f18, c_f18(sp)
	swc1 $f19, c_f19(sp)
	swc1 $f20, c_f20(sp)
	swc1 $f21, c_f21(sp)
	swc1 $f22, c_f22(sp)
	swc1 $f23, c_f23(sp)
	swc1 $f24, c_f24(sp)
	swc1 $f25, c_f25(sp)
	swc1 $f26, c_f26(sp)
	swc1 $f27, c_f27(sp)
	swc1 $f28, c_f28(sp)
	swc1 $f29, c_f29(sp)
	swc1 $f30, c_f30(sp)
	swc1 $f31, c_f31(sp)
	cfc1 t0, $0
	sw	  t0,  c_cr0(sp)
	cfc1 t0, $31
	sw	  t0,  c_cr31(sp)

.endm	/* SAVE_REGISTERS */

.macro RESTORE_REGISTERS

	lq ra, c_ra(sp)
	lq $30, c_fp(sp)
	lq s0, c_s0(sp)
	lq s1, c_s1(sp)
	lq s2, c_s2(sp)
	lq s3, c_s3(sp)
	lq s4, c_s4(sp)
	lq s5, c_s5(sp)
	lq s6, c_s6(sp)
	lq s7, c_s7(sp)

	lqc2 vf1, c_vfAcc(sp)
	VADDAx.xyzw ACC, vf1, vf0

	lqc2 vf1, c_vf1(sp)
	lqc2 vf2, c_vf2(sp)
	lqc2 vf3, c_vf3(sp)
	lqc2 vf4, c_vf4(sp)
	lqc2 vf5, c_vf5(sp)
	lqc2 vf6, c_vf6(sp)
	lqc2 vf7, c_vf7(sp)
	lqc2 vf8, c_vf8(sp)
	lqc2 vf9, c_vf9(sp)
	lqc2 vf10, c_vf10(sp)
	lqc2 vf11, c_vf11(sp)
	lqc2 vf12, c_vf12(sp)
	lqc2 vf13, c_vf13(sp)
	lqc2 vf14, c_vf14(sp)
	lqc2 vf15, c_vf15(sp)
	
	lw   t0, c_vfStat(sp)
	ctc2 t0, vi16
	lw	  t0, c_vfMAC(sp)
	ctc2 t0, vi17
	lw   t0, c_vfCLIP(sp)
	ctc2 t0, vi18
	lw   t0, c_vfR(sp)
	ctc2 t0, vi20
	lw   t0, c_vfI(sp)
	ctc2 t0, vi21
	lw   t0, c_vfQ(sp)
	ctc2 t0, vi22
	lw   t0, c_vfFBRST(sp)
	ctc2 t0, vi28

	lw   t0, c_vi1(sp)
	ctc2 t0, vi1
	lw	  t0, c_vi2(sp)
	ctc2 t0, vi2
	lw	  t0, c_vi3(sp)
	ctc2 t0, vi3
	lw	  t0, c_vi4(sp)
	ctc2 t0, vi4
	lw	  t0, c_vi5(sp)
	ctc2 t0, vi5
	lw	  t0, c_vi6(sp)
	ctc2 t0, vi6
	lw	  t0, c_vi7(sp)
	ctc2 t0, vi7
	lw	  t0, c_vi8(sp)
	ctc2 t0, vi8
	lw	  t0, c_vi9(sp)
	ctc2 t0, vi9
	lw	  t0, c_vi10(sp)
	ctc2 t0, vi10
	lw	  t0, c_vi11(sp)
	ctc2 t0, vi11
	lw	  t0, c_vi12(sp)
	ctc2 t0, vi12
	lw	  t0, c_vi13(sp)
	ctc2 t0, vi13
	lw	  t0, c_vi14(sp)
	ctc2 t0, vi14
	lw	  t0, c_vi15(sp)
	ctc2 t0, vi15
	
	lwc1 $f0, c_f0(sp)
	lwc1 $f1, c_f1(sp)
	lwc1 $f2, c_f2(sp)
	lwc1 $f3, c_f3(sp)
	lwc1 $f4, c_f4(sp)
	lwc1 $f5, c_f5(sp)
	lwc1 $f6, c_f6(sp)
	lwc1 $f7, c_f7(sp)
	lwc1 $f8, c_f8(sp)
	lwc1 $f9, c_f9(sp)
	lwc1 $f10, c_f10(sp)
	lwc1 $f11, c_f11(sp)
	lwc1 $f12, c_f12(sp)
	lwc1 $f13, c_f13(sp)
	lwc1 $f14, c_f14(sp)
	lwc1 $f15, c_f15(sp)
	lwc1 $f16, c_f16(sp)
	lwc1 $f17, c_f17(sp)
	lwc1 $f18, c_f18(sp)
	lwc1 $f19, c_f19(sp)
	lwc1 $f20, c_f20(sp)
	lwc1 $f21, c_f21(sp)
	lwc1 $f22, c_f22(sp)
	lwc1 $f23, c_f23(sp)
	lwc1 $f24, c_f24(sp)
	lwc1 $f25, c_f25(sp)
	lwc1 $f26, c_f26(sp)
	lwc1 $f27, c_f27(sp)
	lwc1 $f28, c_f28(sp)
	lwc1 $f29, c_f29(sp)
	lwc1 $f30, c_f30(sp)
	lwc1 $f31, c_f31(sp)

	lw	  t0,  c_cr0(sp)
	ctc1 t0, $0
	lw	  t0,  c_cr31(sp)
	ctc1 t0, $31

.endm	/* RESTORE_REGISTERS */

PSTACKOFFSET		= 4

.text
.globl	ctxSwitchToTask
.globl	ctxKillSwitch
.globl	InterruptSaveRegisters
.globl	InterruptRestoreRegisters

/************************************************************
/* a0 = pProc, a1 = pSaveProc
************************************************************/
.ent 	ctxSwitchToTask
ctxSwitchToTask:

	/* make room on the stack */

	addiu	sp, sp, -(c_total)

	/* save registers */

	SAVE_REGISTERS

	/* switch context */

	sw sp, PSTACKOFFSET(a1)

	lw sp, PSTACKOFFSET(a0)

	/* restore new context's registers */

	RESTORE_REGISTERS

	/* adjust stack */

	addiu	sp, sp, c_total

	/* return */

	j  	ra
	nop

.end ctxSwitchToTask


/************************************************************
/* a0 = pProc
************************************************************/
.ent 	ctxKillSwitch
ctxKillSwitch:

	/* restore stack pointer */

	lw sp, PSTACKOFFSET(a0)

	/* restore new context's registers */

	RESTORE_REGISTERS

	/* adjust stack */

	addiu	sp, sp, c_total

	/* return */

	j  	ra
	nop

.end ctxKillSwitch

/************************************************************
/* push registers onto stack to save them for an interrupt that needs to use them
************************************************************/
.ent 	InterruptSaveRegisters
InterruptSaveRegisters:

	/* make room on the stack */

	addiu	sp, sp, -(c_total)

	/* save registers */

	SAVE_REGISTERS

	/* return */

	j  	ra
	nop

.end InterruptSaveRegisters

/************************************************************
/* pop registers from stack to restore them after an interrupt that needs to use them
************************************************************/
.ent 	InterruptRestoreRegisters
InterruptRestoreRegisters:

	/* save current return address so that after RESTORE_REGISTERS it's correct */

	sq ra, c_ra(sp)
	
	/* restore registers */

	RESTORE_REGISTERS

	/* adjust stack */

	addiu	sp, sp, c_total

	/* return */

	j  	ra
	nop

.end InterruptRestoreRegisters



