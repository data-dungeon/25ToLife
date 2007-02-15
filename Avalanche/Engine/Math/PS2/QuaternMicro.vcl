
;;; File					QaternMicro.vcl
;;; Purpose				fast math routines for vu0 cooperating with cpu
;;; Notes


.alternate
.include "../../Display/PS2Renderer/VUCode/MacroMagic.i"
.include "../../Display/PS2Renderer/VUCode/Low.i"
.include "../../Display/PS2Renderer/VUCode/Quat.i"

.init_vf vf16, vf17, vf18, vf19, vf20, vf21, vf22, vf23, vf24, vf25, vf26, vf28, vf29, vf30, vf31
.init_vi_all

.global PS2_Quat_To_Mtx


;;; Convert a quaternion into a matrix
;;; Input:
;;;						VF01 = quaternion
;;; Output:
;;;						VF01 = matrix row 0
;;;						VF02 = matrix row 1
;;;						VF03 = matrix row 2
;;;						VF04 = matrix row 3

PS2_Quat_To_Mtx:
--enter
	in_vf quat(vf01)
--endenter

	QuaternionToTransposeMatrix matresult, quat


--exit
	out_vf matresult[0](vf01)
	out_vf matresult[1](vf02)
	out_vf matresult[2](vf03)
	out_vf matresult[3](vf04)
--endexit


	
