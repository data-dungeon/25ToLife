
;;; File					Math.vcl
;;; Purpose				fast math routines for vu0 cooperating with cpu
;;; Notes


.alternate
.include "../../Display/PS2Renderer/VUCode/MacroMagic.i"
.include "../../Display/PS2Renderer/VUCode/Low.i"
.include "../../Display/PS2Renderer/VUCode/Quat.i"

.init_vf vf16, vf17, vf18, vf19, vf20, vf21, vf22, vf23, vf24, vf25, vf26, vf28, vf29, vf30, vf31
.init_vi_all

.global PS2_Math_GetHeightSupport_Init
.global PS2_Math_GetHeightSupport_InitSeg
.global PS2_Math_GetHeightSupport_Loop
.global exitlabelA
.global exitlabelB

;;; Code to support the TerrainSector::GetHeight function.  This is
;;; inited in macro mode, then started up.  This runs while the
;;; cpu gathers and packs the information for the next.  I could
;;; have gained a little parallelism by making two versions of this
;;; code that work on separate sets of registers, then I could have
;;; done the uploads while this ran, but thats only a few instructions
;;; better than what I have...
;;; variable names have been shorted to protect my fingers.
;;; S == ray/segment start
;;; E == segment end
;;; D == ray direction
;;; A == vertex 1 of triangle (with normal X in w)
;;; B == vertex 2 of triangle (with normal Y in w)
;;; C == vertex 3 of triangle (with normal Z in w)
;;; 
.macro DeterminentSign dest, a, b, c
local ta, tb, tc, temp
	opmula.xyz			ACC,			\b,				\c
	opmsub.xyz			temp,			\c,				\b
	mul.xyz				temp,			temp,				\a
	adday.x				ACC,			temp,				temp
	maddz.x				vf00,			ones,				temp
	fmand					\dest,		macboth
.endm	
.macro PlaneRayIntersect dest, a, b, c, s, d, aminuss=NONE
	local diff, denom, numer
.aif "\aminuss" EQ "NONE"
	sub.xyz				diff,			\a,				\s
.aelse
	move.xyz				diff,			\aminuss
.aendi
	mulax.w				ACC,			\a,				\d
	madday.w				ACC,			\b,				\d
	maddz.w				denom,		\c,				\d
	mulax.w				ACC,			\a,				diff
	madday.w				ACC,			\b,				diff
	maddz.w				numer,		\c,				diff
.aif "\dest" eq "Q"
	div					Q,				numer[w],		denom[w]
.aelse
	div					Q,				numer[w],		denom[w]
	addq.x				\dest,		vf00,				Q					
.aendi
.endm
.macro entrypoint
--enter
	in_vf S(vf01)
	in_vf E(vf02)
	in_vf D(vf03)
	in_vf EMinusS(vf04)
	in_vf A(vf05)
	in_vf B(vf06)
	in_vf C(vf07)
	in_vf Dist(vf12)
	in_vf Pt(vf13)
	in_vi DidHit(vi12)
	in_vi Idx(vi13)
	in_vi BestIdx(vi15)
	in_vf BestDist(vf14)
	in_vf BestPt(vf15)
--endenter
.endm
.macro exitpoint
	iaddiu				Idx,			Idx,				1
--exit
	out_vf S(vf01)
	out_vf E(vf02)
	out_vf D(vf03)
	out_vf EMinusS(vf04)
	out_vf Dist(vf12)
	out_vf Pt(vf13)
	out_vi DidHit(vi12)
	out_vi Idx(vi13)
	out_vi BestIdx(vi15)
	out_vf BestDist(vf14)
	out_vf BestPt(vf15)
--endexit
.endm	
.init_vf vf02, vf03
PS2_Math_GetHeightSupport_Init:
--enter
	in_vf S(vf01)
	in_vf E(vf02)
	in_vf D(vf03)
	in_vf EMinusS(vf04)
	in_vi Idx(vi13)
--endenter
	loi 100000000.0
	adda.xyz				ACC,			vf00,				S
	maddi.xyz			E,				D,					I
	sub.xyz				EMinusS,		E,					S
	iaddiu				Idx,			vi00,				0
--cont
--exit
	out_vf S(vf01)
	out_vf E(vf02)
	out_vf D(vf03)
	out_vf EMinusS(vf04)
	out_vi Idx(vi13)
--endexit

PS2_Math_GetHeightSupport_InitSeg:
--enter
	in_vf S(vf01)
	in_vf E(vf02)
	in_vf D(vf03)
	in_vf EMinusS(vf04)
	in_vi Idx(vi13)
--endenter
	One ones

	sub.xyz				D,				E,					S
	mul.xyz				D,				D,					D
	adday.x				ACC,			D,					D
	maddz.x				lensq,		ones,				D
	div					Q,				vf00[w],			lensq[x]
	mulq.xyz				D,				D,					Q

	sub.xyz				EMinusS,		E,					S
	iaddiu				Idx,			vi00,				0
--cont
--exit
	out_vf S(vf01)
	out_vf E(vf02)
	out_vf D(vf03)
	out_vf EMinusS(vf04)
	out_vi Idx(vi13)
--endexit

.init_vf vf04, vf05, vf06, vf07, vf12, vf13, vf14, vf15
PS2_Math_GetHeightSupport_Loop:
	entrypoint

.vsm
	nop nop
	nop nop
.endvsm
	
	;; some values I use...
	iandiu				maczero,		vi00,				0x0008
	iaddiu				macsign,		vi00,				0x0080
	iaddiu				macboth,		vi00,				0x0088
	One					ones

	;; these get used a few times...
	sub.xyz				AMinusS,		A,					S
	sub.xyz				BMinusS,		B,					S
	sub.xyz				CMinusS,		C,					S
	sub.xyz				BMinusA,		B,					A
	sub.xyz				CMinusA,		C,					A
	sub.xyz				SMinusA,		S,					A
	sub.xyz				EMinusA,		E,					A

	;; now do the plane calculation...
	PlaneRayIntersect Q, A, B, C, S, D, AMinusS

	;; check that S & E are on different sides of triangle
	DeterminentSign signs, BMinusA, CMinusA, SMinusA
	DeterminentSign signe, BMinusA, CMinusA, EMinusA

	;; determine the other 3 determinent signs we need
	DeterminentSign	signac, EMinusS, CMinusS, AMinusS
	DeterminentSign	signbc, EMinusS, BMinusS, CMinusS
	DeterminentSign	signab, EMinusS, AMinusS, BMinusS


	;; farther = Dist > BestDist
	subq.x				vf00,			BestDist,		Q
	fmand					farther,		macsign

	;; calculate the point of intersection...
	adda.xyz				ACC,			vf00,				S
	maddq.xyz			Pt,			D,					Q

	;; clear the did-hit flag
	iaddiu				DidHit,		vi00,				0
	
	;; zeroflag = (signab == 0 || signbc == 0 || signac == 0)
	ior					zerocheck,	signab,			signbc
	ior					zerocheck,	zerocheck,		signac
	iand					zerocheck,	zerocheck,		maczero

	;; on same side of poly?
	ibeq					signs,		signe,			NoHit

	;; perform the ifs..
	ibnez					zerocheck,	Zeros
	ibne					signab,		signbc,			NoHit
	ibne					signab,		signac,			NoHit

Hit:
	;; set the did-hit flag
	iaddiu				DidHit,		vi00,				1
	;; ok, now to see if its closer than the best hit...
	ibnez					farther,		NoHit
	;; ok, its closer... now save it...
	iaddiu				BestIdx,		Idx,				0
	addq.x				BestDist,	vf00,				Q
	move					BestPt,		Pt
	exitpoint

Zeros:
	ibeq					signab,		signbc,			Hit
	ibeq					signbc,		signac,			Hit
	ibeq					signab,		signac,			Hit
	
NoHit:
	exitpoint


