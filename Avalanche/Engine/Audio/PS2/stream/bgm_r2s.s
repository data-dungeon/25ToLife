/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.3
 */
/* 
 *                  I/O Proseccor sample program
 *                          Version 1.20
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       ezbgm.irx - bgm_r2s.c
 *                          raw to spu pcm
 *
 *   Version   Date            Design    Log
 *  --------------------------------------------------------------------
 *   1.20      Nov.23.1999     morita    modify for EzBGM
 *   0.01      Nov.18.1999     ishii     optimize for IOP
 */

// Code taken from the ezBGM sample code. Thankyou to morita and ishii.

//	_BgmRaw2Spu ((u_long *)sp,(u_long *)dp, size/1024);

#include <cpureg.h>

#define src  a0
#define dst  a1
#define blk  a2
#define cnt  a3

	.globl	_BgmRaw2Spu
	.ent	_BgmRaw2Spu
_BgmRaw2Spu:
	subu	sp, (8*4)
	sw	s0, 0*4(sp)	; 	sw	s1, 1*4(sp)
	sw	s2, 2*4(sp)	; 	sw	s3, 3*4(sp)
	sw	s4, 4*4(sp)	; 	sw	s5, 5*4(sp)
	sw	s6, 6*4(sp)	; 	sw	s7, 7*4(sp)
	move	v0, zero
	move	v1, zero
	addi	cnt,zero, 256

					.set noreorder
	/* ここから、手で命令のならびを制御。 */
pcm_separate_loop:
	lw	t0, 0*4(src)	/* 1 + 4 clock */
	lw	t1, 1*4(src)	/* 1 + 2 clock */

	/* 7 clock */
	and	s0, t0, 0xffff		/* t0 の下位を s0 へ */
	srl	s1, t0, 16		/* t0 の上位を s1 へ */
	sll	t0, t1, 16		/* t1 の下位を上にシフトし s0 へ */
	or	s0, t0
	srl	t1, t1, 16		/* t1 の上位をマスクして s1 へ */
	sll	t1, t1, 16
	or	s1, t1

	/* 既にキャッシュに読み込まれているはず */
	lw	t2, 2*4(src)	/* 1 clock */
	lw	t3, 3*4(src)	/* 1 clock */
	/* 次のキャッシュラインを読み込むきっかけを作る */
	lw	t4, 4*4(src)	/* 1 + 4 clock */

	/* 7 clock */
	and	s2, t2, 0xffff		/* t2 の下位を s2 へ */
	srl	s3, t2, 16		/* t2 の上位を s3 へ */
	sll	t2, t3, 16		/* t3 の下位を上にシフトし s2 へ */
	or	s2, t2
	srl	t3, t3, 16		/* t3 の上位をマスクして s3 へ */
	sll	t3, t3, 16
	or	s3, t3

	/* 既にキャッシュに読み込まれているはず */
	lw	t5, 5*4(src)	/* 1 clock */
	lw	t6, 6*4(src)	/* 1 clock */
	lw	t7, 7*4(src)	/* 1 clock */
	add	src, 8*4	/* 1 clock */

	/* 7 clock */
	and	s4, t4, 0xffff		/* t4 の下位を s4 へ */
	srl	s5, t4, 16		/* t4 の上位を s5 へ */
	sll	t4, t5, 16		/* t5 の下位を上にシフトし s4 へ */
	or	s4, t4
	srl	t5, t5, 16		/* t5 の上位をマスクして s5 へ */
	sll	t5, t5, 16
	or	s5, t5

	/* store  dst1(s0, s2),  dst2(s1, s3) */
	/* 4 clock */

	sw	s0, 0*4(dst)	;	sw	s2, 1*4(dst)
	sw	s1, (0*4+512)(dst); 	sw	s3, (1*4+512)(dst)
	/* 以下と平行して +2, 4+2 clock で書き込まれるはず */

	/* 7 clock */
	and	s6, t6, 0xffff		/* t6 の下位を s6 へ */
	srl	s7, t6, 16		/* t6 の上位を s7 へ */
	sll	t6, t7, 16		/* t7 の下位を上にシフトし s6 へ */
	or	s6, t6
	srl	t7, t7, 16		/* t7 の上位をマスクして s7 へ */
	sll	t7, t7, 16
	or	s7, t7

	/* store  dst1(s4, s6),  dst2(s5, s7) */
	/* 4 clock */
	sw	s4, 2*4(dst)	; 	sw	s6, 3*4(dst)
	sw	s5, (2*4+512)(dst)	; 	sw	s7, (3*4+512)(dst)
	/* 以下と平行して +2, 4+2 clock で書き込まれるはず */

	add	dst, 4*4		/* 1 clock */


	/* ここまで、手で命令のならびを制御。 */
					.set reorder

	add	v0, 8			/* 1 clock */
	blt	v0, cnt, pcm_separate_loop	/* 1 clock */

	add	dst, 512
	move	v0, zero

	add	v1, 1
	blt	v1, blk, pcm_separate_loop

	lw	s0, 0*4(sp)	; 	lw	s1, 1*4(sp)
	lw	s2, 2*4(sp)	; 	lw	s3, 3*4(sp)
	lw	s4, 4*4(sp)	; 	lw	s5, 5*4(sp)
	lw	s6, 6*4(sp)	; 	lw	s7, 7*4(sp)
	addu	sp, (8*4)
	j	ra
	.end	_BgmRaw2Spu



/************************************************************************/
/** Assembly version of filter routine (original C version in MS_DSP.C **/
/** Thanks to Simon Goodwin for this code **/
/** (Slightly modified by Jason Page to handle being called as a seperate routine rather than inbedded) **/
/************************************************************************/

	.globl	_FastFilter
	.ent	_FastFilter

_FastFilter:

	subu	sp, (8*4)
	sw	s0, 0*4(sp)	; 	sw	s1, 1*4(sp)
	sw	s2, 2*4(sp)	; 	sw	s3, 3*4(sp)
	sw	s4, 4*4(sp)	; 	sw	s5, 5*4(sp)
	sw	s6, 6*4(sp)	; 	sw	s7, 7*4(sp)

	move	    t2, a0		/* LowPass */
	move	    t3, a1		/* BandPass */
	move	    t6, a1		/* JFreq - filter centre frequency */
	move	    t7, a2		/* Data */
	move	    t5, a3		/* Resonance - only needed if Q != 1, see later (bottom 16 bits) */
	move	    t6, a3		/* JFreq - filter centre frequence (top 16 bits) */

	srl		t6, t6, 16		/* shift right 16 */

	and		t5, t5, 0xffff
	and		t6, t6, 0xffff

	addiu	t0, t7, 256*2	    /* Point t0 at end of table so no need to count */

	mult	t6, t3            /* Start to compute (JFrq*BPass) subexpression */
    
Loop:
	lh	    t1, 0(t7)	        /* Samp = *data++; */
	addiu	t7, t7, 2
    
    /* LowPass = LowPass + ((JFrq * BPass) >> 9); */

	mflo	v0
	sra	    v1, v0, 9         /* (JFrq * BPass) >> 9) */
	addu	t2, t2, v1

    /* HiPass = Samp - LowPass - ((Res * BPass) >> 8); */

    /* We can skip the next mult and mflo if resonance is fixed for a Q of 1 */

	mult	t5, t3	        /* (Res * BPass) */
	subu	v0, t1, t2       /* Samp - LowPass */
	mflo	v1		

    /* If Q = 1, optimise next line to: sra t3, t3, 8 */

	sra 	v1, v1, 8         /* ((Res * BPass) >> 8) */
	subu	t4, v0, v1

    /* BPass = ((JFrq * HiPass) >> 9) + BPass; */

	mult	t6, t4            /* (JFrq * HiPass) */
	mflo	v0	                /* Typically six cycles multiplier stall */

	sra	    v1, v0, 9         /* (JFrq * HiPass) >> 9 */
	add	    t3, t3, v1

    /* Apply clamp; presumes the result's in t2 (low pass) although the */
    /* same code works for band or high pass with t3 or t4 respectively. */
    /* Remember to also change the sh line that stores the clamped result */

    .set    noreorder           /* Leave instructions in branch delay slots */

	slti	v1, t2, -32767
	beq	    v1, zero, HighEnough
	slti	v1, t2, 32767			

   	li	    t2, -32768         /* Clamp to SAMPLE_MINIMUM */

HighEnough:		

    bne     v1, zero, LowEnough
	mult	t6, t3            /* Start next (JFrq*BPass) subexpression */

   	li	    t2, 32767          /* Clamp to SAMPLE_MAXIMUM */

    /* Loop till t7 points at the end of the table, storing results over the input */

LowEnough:

	bne	    t0, t7, Loop
    sh	    t2, -2(t7)

    .set    reorder

    /* Store Bandpass and Low pass accumulator states for the next block */

/*    sw	    t3, 0x4(sp)
    sw	    t2, 0x0(sp)
*/


	move	v0,t2		/* lowpass */
	move	v1,t3		/* bandpass */


	lw	s0, 0*4(sp)	; 	lw	s1, 1*4(sp)
	lw	s2, 2*4(sp)	; 	lw	s3, 3*4(sp)
	lw	s4, 4*4(sp)	; 	lw	s5, 5*4(sp)
	lw	s6, 6*4(sp)	; 	lw	s7, 7*4(sp)
	addu	sp, (8*4)


	j	ra
	.end	_FastFilter
