
;;; File					FrustumMicro.vcl
;;; Purpose				vu0 frustum culling microcode
;;; Notes


.alternate
.include "../../Display/PS2Renderer/VUCode/MacroMagic.i"
.include "../../Display/PS2Renderer/VUCode/Low.i"

.init_vf vf16, vf17, vf18, vf19, vf20, vf21, vf22, vf23, vf24, vf25, vf26, vf28, vf29, vf30, vf31
.init_vi_all

.global VU0_Frustum__SetPlanes
.global VU0_Frustum__IntersectsWorldBounds
.global VU0_Frustum__BoxIntersectsWorldBounds
.global VU0_Frustum__SphereIntersectsWorldBounds

		 
		  	
VU0_Frustum__SetPlanes:
--enter
	in_vf	Plane1(vf16)
	in_vf Plane2(vf17)
	in_vf Plane3(vf18)
	in_vf Plane4(vf19)
	in_vf Plane5(vf20)
	in_vf Plane6(vf21)
	in_vf Plane7(vf22)
	in_vi planeoffset(vi01)
--endenter
	sq		Plane1,				0(planeoffset)
	sq		Plane2,				1(planeoffset)
	sq		Plane3,				2(planeoffset)
	sq		Plane4,				3(planeoffset)
	sq		Plane5,				4(planeoffset)
	sq		Plane6,				5(planeoffset)
	sq		Plane7,				6(planeoffset)
--exit
--endexit

.macro LoadPlanes offset
	lq		Plane1,				0+offset(vi00)
	lq		Plane2,				1+offset(vi00)
	lq		Plane3,				2+offset(vi00)
	lq		Plane4,				3+offset(vi00)
	lq		Plane5,				4+offset(vi00)
	lq		Plane6,				5+offset(vi00)
	lq		Plane7,				6+offset(vi00)
.endm

.macro TestPlanes index
	;; test the box
	mul	ACC,					Plane4,			halfwidth[x]
	madd	ACC,					Plane5,			halfwidth[y]
	madd	projrad,				Plane6,			halfwidth[z]
	mul	ACC,					Plane7,			vf00[w]
	madd	ACC,					Plane1,			center[x]
	madd	ACC,					Plane2,			center[y]
	madd	ACC,					Plane3,			center[z]
	msub	vf00,					projrad,			vf00[w]
	fmor	boxin,				vi00
	madd	vf00,					projrad,			vf00[w]
	;; ignore the fog planes & unused planes
	fmor	boxout,				GBPlaneMask[index]

	;; test the sphere
	mul	ACC,					Plane7,			vf00[w]
	madd	ACC,					Plane1,			Sphere[x]
	madd	ACC,					Plane2,			Sphere[y]
	madd	ACC,					Plane3,			Sphere[z]
	msub	vf00,					ones,				Sphere[w]
	;; volume culling is based on if box and sphere are completely outside
	fmand	resultin[index],	boxin
	madd	vf00,					ones,				Sphere[w]
	;; clipping on/off is based if box or sphere is within gaurdband
	fmor	resultout[index],	boxout
.endm
		
.macro BoxTestPlanes index
	;; test the box
	mul	ACC,					Plane4,			halfwidth[x]
	madd	ACC,					Plane5,			halfwidth[y]
	madd	projrad,				Plane6,			halfwidth[z]
	mul	ACC,					Plane7,			vf00[w]
	madd	ACC,					Plane1,			center[x]
	madd	ACC,					Plane2,			center[y]
	madd	ACC,					Plane3,			center[z]
	msub	vf00,					projrad,			vf00[w]
	fmor	resultin[index],	vi00
	madd	vf00,					projrad,			vf00[w]
	;; ignore the fog planes & unused planes
	fmor	resultout[index],	GBPlaneMask[index]
.endm

.macro SphereTestPlanes index
	;; test the sphere
	mul	ACC,					Plane7,			vf00[w]
	madd	ACC,					Plane1,			Sphere[x]
	madd	ACC,					Plane2,			Sphere[y]
	madd	ACC,					Plane3,			Sphere[z]
	msub	vf00,					ones,				Sphere[w]
	;; volume culling is based on if box and sphere are completely outside
	fmor	resultin[index],	vi00
	madd	vf00,					ones,				Sphere[w]
	;; clipping on/off is based if box or sphere is within gaurdband
	fmor	resultout[index],	GBPlaneMask[index]
.endm
		
.macro FrustumCheck testname
	;; these get stripped because they arent used.
   iaddiu GBPlaneMask[0],	vi00,				0
   iaddiu GBPlaneMask[1],	vi00,				0

	;; need ones...
	max	ones,					vf00,				vf00[w]
	
	;; generate box center
	loi	0.5
	mul	ACC,					BoxMax,			I
	madd	center,				BoxMin,			I

	;; we need the halfwidth of the box
	msub	halfwidth,			BoxMin,			I

	;; first set of 4 planes
	LoadPlanes 0
	\testname	0	
	
	;; if any xyzw is 0, then we were outside that plane...
	;; but we need a way to ignore planes, so we or in this
	;; value
	ior	resultin[0],		resultin[0],	VWPlaneMask0

	;; next set of 4 planes
	LoadPlanes 7
	\testname 1

	;; if any xyzw is 0, then we were outside that plane...
	;; but we need a way to ignore planes, so we or in this
	;; value
	ior	resultin[1],		resultin[1],	VWPlaneMask1

	;; at this point, we can and the two togenther...
	iand	outside,				resultin[0],	resultin[1]
	
	;; now if any bit is a 0, then we are outside on that plane...
	;; since we use the ior to set every bit that is unused, we
	;; should have 0xFFFF. adding 1 will give a value==0 if we
	;; were somewhat inside on all planes...
	iaddiu	outside,			outside,			1

	;; we also want a fog value to tell us if we were outside the near
	;; fog plane.  we get that from resultin[1][w]
	iandiu	fog,				resultin[1],	0x0010
		
	;; next set of 4 planes
	LoadPlanes 21
	\testname 3

	;; we also need a flag to tell us if it was clipped by the far plane...
	;; thats resultout[3] & 0x0040.  if its 0, we were clipped by far plane...
	iandiu	farclip,			resultout[3],		0x0040

	;; next set of 4 planes
	LoadPlanes 14
	\testname 2

	;; are we inside planeA and planeB
	iand	outgaurd,			resultout[2],		resultout[3]

	;; if(outgaurd == 0xffff) then we were completely
	;; inside the gaurdband.  so clipping can be turned off... lets make
	;; it a clipon == 0 for no clipping value...
	iaddiu	clipon,			outgaurd,			1
.endm

.macro FrustumCheckInputs
	in_vf BoxMin(vf16)
	in_vf BoxMax(vf17)
	in_vf Sphere(vf18)
	in_vi VWPlaneMask0(vi01)
	in_vi VWPlaneMask1(vi02)
	in_vi GBPlaneMask[2](vi03)
	in_vi GBPlaneMask[3](vi04)
.endm
		  
.macro FrustumCheckOutputs
	;; this will be 0 if we any part of the cube or sphere was inside
	;; all planes, so if != 0, there was at least one plane where we
	;; were totally outside, so we should return false from 
	;; Frustum__IntersectsWorldBounds()
	out_vi outside(vi01)
	
	;; this is used to get the fog enable bit.
	;; if(this & 0x0010) then fog on.
	;; else fog off
	out_vi fog(vi02)

	;; this is used to determine if clipping should be on.
	;; if its 0, clipping should be off, otherwise, clipping
	;; should be on.
	out_vi clipon(vi03)

	;; if(resultout[3] & 0x0040) then we were not clipped by
	;; the far fog plane...
	out_vi farclip(vi04)
.endm
		  		  
VU0_Frustum__IntersectsWorldBounds:
--enter
	FrustumCheckInputs
--endenter
	FrustumCheck TestPlanes
--exit
	FrustumCheckOutputs
--endexit

		  
VU0_Frustum__BoxIntersectsWorldBounds:
--enter
	FrustumCheckInputs
--endenter
	FrustumCheck BoxTestPlanes
--exit
	FrustumCheckOutputs
--endexit

		  
VU0_Frustum__SphereIntersectsWorldBounds:
--enter
	FrustumCheckInputs
--endenter
	FrustumCheck SphereTestPlanes
--exit
	FrustumCheckOutputs
--endexit
		  		  	
	END
	
