
;;; File					Occluder.vcl
;;; Purpose				vu0 occluder culling microcode
;;; Notes
;;;
;;;  The first 28 QWs of VU0 uMem are used by the frustum culling
;;;  code.  since we need to coexist with it, we cant use those.
;;;  we use addresses 28-248, leaving only 8 unused at the top.
;;;
;;;  expects a bunch of occlusion frustums (in groups of 4)
;;;  pre-uploaded to uMem like so:
;;;     N0x N1x N2x N3x
;;;     N0y N1y N2y N3y
;;;     N0z N1z N2z N3z
;;;     D0  D1  D2  D3
;;;     
;;;     N5x N6x N7x N8x
;;;     N5y N6y N7y N8y
;;;     N5z N6z N7z N8z
;;;     D5  D6  D7  D8
;;;     
;;;     NAx NBx NCx NDx
;;;     NAy NBy NCy NDy
;;;     NAz NBz NCz NDz
;;;     DA  DB  DC  DD
;;;     
;;;     NFx NGx NHx NIx
;;;     NFy NGy NHy NIy
;;;     NFz NGz NHz NIz
;;;     DF  DG  DH  DI
;;;
;;;     N4x N9x NEx NJx
;;;     N4y N9y NEy NJy
;;;     N4z N9z NEz NJz
;;;     D4  D9  DE  DJ
;;;
;;; Where:
;;;   Nn(x|y|z)	normal x y z components of plane n
;;;   Dn 			d value for plane n
;;;   Planes 0-4 make up one occluder
;;;   Planes 5-9 make up the second occluder
;;;   Planes A-E make up the third occluder
;;;   Planes F-J make up the fourth occluder
;;; 
;;; Returns in VI01 either a negative value if occluded
;;; or >= 0 if not occluded.

.alternate

.init_vf vf16, vf17, vf18, vf19, vf20, vf21, vf22, vf23, vf24, vf25, vf26, vf28, vf29, vf30, vf31
.init_vi_all

.name VU0_Occluder_Test

--enter
	in_vi		OccluderCount(vi01)
	in_vf		Sphere(vf16)
--endenter

;;	int plane = 0;
	iaddiu	plane,				vi00,					0x001C

	max		ones,					VF00,					VF00[W]

	;;int retval = 0;
	iaddiu	retvalue,			vi00,					0x0000

	iaddiu	signmask_x,			vi00,					0x0080
	iaddiu	signmask_y,			vi00,					0x0040

OccluderLoop:
--loopcs 1, 0

	iaddiu	signmask_z,			vi00,					0x0020
	iaddiu	signmask_w,			vi00,					0x0010
;;	for ( ; occcount > 0 ; occcount -= 4 )
;;	{
;;		Vector4 t0 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
;;		Vector4 t1 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
;;		Vector4 t2 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
;;		Vector4 t3 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
;;		Vector4 t4 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;

;;		if(
;;			(t0.x() < 0.0f || t0.y() < 0.0f || t0.z() < 0.0f || t0.w() < 0.0f || t4.x() < 0.0f) &&
;;			(occcount-1 <= 0 || t1.x() < 0.0f || t1.y() < 0.0f || t1.z() < 0.0f || t1.w() < 0.0f || t4.y() < 0.0f) &&
;;			(occcount-2 <= 0 || t2.x() < 0.0f || t2.y() < 0.0f || t2.z() < 0.0f || t2.w() < 0.0f || t4.z() < 0.0f) &&
;;			(occcount-3 <= 0 || t3.x() < 0.0f || t3.y() < 0.0f || t3.z() < 0.0f || t3.w() < 0.0f || t4.w() < 0.0f) &&
;;			;
;;		else
;;			retval |= 0x8000;
;;	}
;;	results = retval;

;;		Vector4 t0 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
	lqi			dat,				(plane++)
	mul			ACC,				dat,					Sphere[x]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[y]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[z]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					VF00[W]
	msub			VF00,				ones,					Sphere[w]
	; if any are negative, this is 0x2, else 0x0
	fsand			t0flags,			0x0002

;;		Vector4 t1 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
	lqi			dat,				(plane++)
	mul			ACC,				dat,					Sphere[x]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[y]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[z]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					VF00[W]
	msub			VF00,				ones,					Sphere[w]
	; if any are negative, this is 0x2, else 0x0
	fsand			t1flags,			0x0002

;;		Vector4 t2 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
	lqi			dat,				(plane++)
	mul			ACC,				dat,					Sphere[x]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[y]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[z]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					VF00[W]
	msub			VF00,				ones,					Sphere[w]
	; if any are negative, this is 0x2, else 0x0
	fsand			t2flags,			0x0002
	
;;		Vector4 t3 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
	lqi			dat,				(plane++)
	mul			ACC,				dat,					Sphere[x]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[y]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[z]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					VF00[W]
	msub			VF00,				ones,					Sphere[w]
	; if any are negative, this is 0x2, else 0x0
	fsand			t3flags,			0x0002


;;		Vector4 t4 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
	lqi			dat,				(plane++)
	mul			ACC,				dat,					Sphere[x]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[y]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					Sphere[z]
	lqi			dat,				(plane++)
	madd			ACC,				dat,					VF00[W]
	msub			VF00,				ones,					Sphere[w]
	; bits are 1 if negative  0x10, 0x20, 0x40, 0x80
	iaddiu		signmasks,		vi00,					0x00F0
	fmand			t4flags,			signmasks
	iand			t4flagsx,		t4flags,				signmask_x
	iand			t4flagsy,		t4flags,				signmask_y
	iand			t4flagsz,		t4flags,				signmask_z
	iand			t4flagsw,		t4flags,				signmask_w

;;			(t0.x() < 0.0f || t0.y() < 0.0f || t0.z() < 0.0f || t0.w() < 0.0f || t4.x() < 0.0f) &&
	; t0flags is 0x2 if t0.x() < 0.0f || t0.y() < 0.0f || t0.z() < 0.0f || t0.w() < 0.0f
	; t4flagsx is 0x80 if t4.x() < 0.0f
	; so, first we want all these to be in the same bit.  we do this by subtracting the flag value
	; and then using the sign bit.
	isubiu		t0flags,			t0flags,				0x0002
	isubiu		t4flagsx,		t4flagsx,			0x0080
	; now they contain 0 if the if is true, else 0xff??
	iand			occ0result,		t0flags,				t4flagsx
	; occ0result is either 0, or 0xFF??  if its 0, we dont want to change retval.

;;			(1-occcount >= 0 || t1.x() < 0.0f || t1.y() < 0.0f || t1.z() < 0.0f || t1.w() < 0.0f || t4.y() < 0.0f) &&
	; as above, cept we need to take occcount-1 < 0 into account
	isubiu		t1flags,			t1flags,				0x0002
	isubiu		t4flagsy,		t4flagsy,			0x0040
	iaddiu		ione,				vi00,					1
	isub			occcountsub,	ione,					OccluderCount
	iand			occ1result,		t1flags,				t4flagsy
	iand			occ1result,		occ1result,			occcountsub

;;			(2-occcount >= 0 || t2.x() < 0.0f || t2.y() < 0.0f || t2.z() < 0.0f || t2.w() < 0.0f || t4.z() < 0.0f) &&
	; as above, cept we need to take occcount-2 < 0 into account
	isubiu		t2flags,			t2flags,				0x0002
	isubiu		t4flagsz,		t4flagsz,			0x0020
	iaddiu		itwo,				vi00,					2
	isub			occcountsub,	itwo,					OccluderCount
	iand			occ2result,		t2flags,				t4flagsz
	iand			occ2result,		occ2result,			occcountsub

;;			(3-occcount >= 0 || t3.x() < 0.0f || t3.y() < 0.0f || t3.z() < 0.0f || t3.w() < 0.0f || t4.w() < 0.0f) &&
	; as above, cept we need to take occcount-3 < 0 into account
	isubiu		t3flags,			t3flags,				0x0002
	isubiu		t4flagsw,		t4flagsw,			0x0010
	iaddiu		ithree,			vi00,					3
	isub			occcountsub,	ithree,				OccluderCount
	iand			occ3result,		t3flags,				t4flagsw
	iand			occ3result,		occ3result,			occcountsub

	; now, if any of the occNresult values have a high bit set, we need to know it.
	; or them together, so that we can tell..
	ior			occresult,		occ0result,			occ1result
	ior			occresult,		occresult,			occ2result
	ior			occresult,		occresult,			occ3result

	; now if occresult has a high bit set, we we need to set retval.  we do this again with an or
	ior			retvalue,		retvalue,			occresult

;;	for ( ; occcount > 0 ; occcount -= 4 )
	isubiu		OccluderCount,	OccluderCount,		4
	ibgtz			OccluderCount,	OccluderLoop


--exit
	out_vi		retvalue(vi02)
--endexit

