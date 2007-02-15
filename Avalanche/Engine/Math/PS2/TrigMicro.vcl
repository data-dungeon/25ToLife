
;;; File					Math.vcl
;;; Purpose				fast math routines for vu0 cooperating with cpu
;;; Notes


.alternate
.include "../../Display/PS2Renderer/VUCode/MacroMagic.i"
.include "../../Display/PS2Renderer/VUCode/Low.i"

.init_vf vf16, vf17, vf18, vf19, vf20, vf21, vf22, vf23, vf24, vf25, vf26, vf28, vf29, vf30, vf31
.init_vi_all

.global PS2_Math_SinCos
.global PS2_Math_Sin4
.global PS2_Math_Cos4
.global PS2_Math_FastSinCos
.global PS2_Math_FastSin4
.global PS2_Math_FastCos4

;;; Sin of four values in radians...
;;; input:
;;; 						VF01.XYZW four inputs
;;; output:
;;;						VF02.XYZW sin(VF01)
PS2_Math_Sin4:
--enter
	in_vf					SinCosIn(vf01)
--endenter
	loi					0x3fc90fdb
	sub					SinCosIn,	SinCosIn,		I
	bal					CCCCRet,		sub_CCCC
--exit
	out_vf				SinCosOut(vf02)
--endexit

;;; Cos of four values in radians...
;;; input:
;;; 						VF01.XYZW four inputs
;;; output:
;;;						VF02.XYZW cos(VF01)
PS2_Math_Cos4:
--enter
	in_vf					SinCosIn(vf01)
--endenter
	bal					CCCCRet,		sub_CCCC
--exit
	out_vf				SinCosOut(vf02)
--endexit
	
	
	
;;; Sin/Cos of two values in radians...
;;; input:
;;;						VF01.X input 1
;;;						VF01.Z input 2
;;; output:
;;;						VF02.X = sin(VF01.X)
;;;						VF02.Y = cos(VF01.Z)
;;;						VF02.Z = sin(VF01.Z)
;;;						VF02.W = cos(VF01.X)
;;;						VF03.X = cos(VF01.X)
;;;						VF03.Y = cos(VF01.X)
;;;						VF03.Z = cos(VF01.X)
;;;						VF03.W = cos(VF01.X)
	
PS2_Math_SinCos:
--enter
	in_vf					SinCosIn(vf01)
--endenter
	loi					0x3fc90fdb
	mr32.yw				SinCosIn,	SinCosIn
	sub.xz				SinCosIn,	SinCosIn,		I
	One					ones
	bal					CCCCRet,		sub_CCCC
	mul					output2,		ones,				SinCosOut[w]
--exit
	out_vf				SinCosOut(vf02)
	out_vf				output2(vf03)
--endexit

	
;;; Sin of four values in radians... (no clamping to range
;;; input:
;;; 						VF01.XYZW four inputs
;;; output:
;;;						VF02.XYZW sin(VF01)
PS2_Math_FastSin4:
--enter
	in_vf					SinCosIn(vf01)
--endenter
	loi					0x3fc90fdb
	sub					SinCosIn,	SinCosIn,		I
	bal					CCCCRet,		sub_CCCCFast
--exit
	out_vf				SinCosOut(vf02)
--endexit

;;; Cos of four values in radians...
;;; input:
;;; 						VF01.XYZW four inputs
;;; output:
;;;						VF02.XYZW cos(VF01)
PS2_Math_FastCos4:
--enter
	in_vf					SinCosIn(vf01)
--endenter
	bal					CCCCRet,		sub_CCCCFast
--exit
	out_vf				SinCosOut(vf02)
--endexit
	
	
	
;;; Sin/Cos of two values in radians...
;;; input:
;;;						VF01.X input 1
;;;						VF01.Z input 2
;;; output:
;;;						VF02.X = sin(VF01.X)
;;;						VF02.Y = cos(VF01.Z)
;;;						VF02.Z = sin(VF01.Z)
;;;						VF02.W = cos(VF01.X)
;;;						VF03.X = cos(VF01.X)
;;;						VF03.Y = cos(VF01.X)
;;;						VF03.Z = cos(VF01.X)
;;;						VF03.W = cos(VF01.X)
	
PS2_Math_FastSinCos:
--enter
	in_vf SinCosIn(vf01)
--endenter
	loi 0x3fc90fdb
	mr32.yw				SinCosIn,	SinCosIn
	sub.xz				SinCosIn,	SinCosIn,		I
	One					ones
	bal					CCCCRet,		sub_CCCCFast
	mul					output2,		ones,				SinCosOut[w]
--exit
	out_vf				SinCosOut(vf02)
	out_vf				output2(vf03)
--endexit



	LowLevelSubroutines

