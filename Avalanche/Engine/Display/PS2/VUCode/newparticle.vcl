;;; File		  newparticle.vcl
;;; Purpose	  particlemgr drawing for PS2
;;; Notes	  This also does view culling, suppression-box culling, scene culling, invisible culling, etc as per PatricleMgr.cpp
;;;			  Some naming conventions for variables
;;;				  _c version of variable representing only particles to be clipped.

;;; 
;;; this microcode expects particles as (each letter is a nibble, each particle is 2 qwords):
;;; WWWWWWRR, XXXXXXGG, HHHHHHBB, YYYYYYAA
;;; ZZZZZZff, xxxxxxff, yyyyyyff, zzzzzzff
;;; 
;;; where the xyz parameters are additional orientation information (ie
;;; either the particles velocity or its arbitary rotation). and the f
;;; parameters are flags.  W is width, H is height, XYZ are obvious,
;;; RGBA should be to. 
;;; 

;;; flags are named flagset[0], flagset[1], flagset[2], flagset[3]
;;; where [0] is from X, [1] is from Y, etc...
;;;
;;; flags in use are:
;;;	  flagset[0]
;;;				 0x0F				  scene N only bits (unchecked)
;;;	  flagset[1]
;;;				 0xFF				  box enable bits.  one per suppression box
;;;	  flagset[2]
;;;				 0x03				  orientation method
;;;				 0x04				  lighted
;;;				 0x08				  has rotation
;;;				 0x10				  alignvel
;;;				 0x20				  distortion renderer
;;;	  flagset[3]
;;;				 ?
;;;
		  
.alternate
.include "High.i"

;;; give vcl all the vu registers
.init_vf_all
.init_vi_all

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; constants that can be changed fairly easily
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; suppression boxes are turned off until we have a test case to see if they work.
	;; I havent implimented the actual upload of the boxes, right now, all 0s are uploaded
NUM_SUPPR_BOXES		.assigna 0	; Can be as high as 16 without problems (other than vu code size problems, memory, etc)
MAX_CLIP_VERTS			.equ (14)
CLIP_BUFFER_SIZE		.equ (MAX_CLIP_VERTS * 3)
OUTBUFFERSIZE			.equ (MAX_CLIP_VERTS * 3)
OUTBUFFEROVERHEAD		.equ (7)
OUTBUFFERSHIFT			.equ (OUTBUFFERSIZE + OUTBUFFEROVERHEAD)
STACK_SIZE				.equ (5)
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; addresses of fixed data in vumem...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CONSTANTS				.equ 0
LDIR_X					.equ 1
LDIR_Y					.equ 2
LDIR_Z					.equ 3
LCOLOR_0					.equ 4
LCOLOR_1					.equ 5
LCOLOR_2					.equ 6
LCOLOR_3					.equ 7
AMBIENT					.equ 8
CAM_TO_WRLD				.equ 10
WRLD_TO_CAM				.equ 14
WRLD_TO_CLIP			.equ 18
CLIP_TO_SCRN			.equ 22
SUPPBOX_1				.equ 26
SUPPBOX_2				.equ 30
SUPPBOX_3				.equ 34
SUPPBOX_4				.equ 38
SUPPBOX_5				.equ 42
STACK						.equ 46
STACK_END				.equ (STACK + STACK_SIZE)
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; generated constants based on above
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; some macros used below
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro TransformParticle dest, mtx
	mul					ACC,			\mtx[3],	vf00[w]
	madd					ACC,			\mtx[0],	pline1[y]
	madd					ACC,			\mtx[1],	pline1[w]
	madd					\dest,		\mtx[2],	pline2[x]
.endm
	
.macro TransformOrient dest, mtx
	mul					ACC,			\mtx[0],	pline2[y]
	madd					ACC,			\mtx[1],	pline2[z]
	madd					\dest,		\mtx[2],	pline2[w]
.endm
	
.macro ClipTestBox box
	TransformParticle	posbox,		SBox
	clipw.xyz			posbox,		vf00[w]
	fcand					vi01,			0x3F
.aif \box eq 0
	iaddiu				boxresults,	vi01,				0
.aelse
	iadd					boxresults,	boxresults,		boxresults ; boxresults <<= 1
	ior					boxresults,	boxresults,		vi01
.aendi
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; main entry point
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
.name Particle
--enter
--endenter		  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; one time setup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; stack builds backwards...
	iaddiu		 Stack,	vi00,	  STACK_END

	;; ok, ready to start a buffer full
	;; stall until the vif is ready for us..
--cont

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; buffer setup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
NewBuffer:		 

	;; buffer start
	xtop					buffertop
	LoadIVector4		buffertop,	buffline1
	LoadIVector4		buffertop,	buffline2
	LoadIVector4		buffertop,	buffline3
	LoadIVector4		buffertop,	buffline4
	LoadIVector4		buffertop,	buffline5

	;; particle counts
	mtir					count,		buffline2[x]

	;; update world-to-clip matrix z value to implement particle z-bias. see comments in ParticleMgr.cpp
	sq.z					buffline5,	(WRLD_TO_CLIP + 3)(vi00)

	;; generate x & y scales to correct distortion sample coords...
	;; stored as int in buffline5[w] : ( WIDTH << 15 ) | ( HEIGHT )

	mtir					heightscale, buffline5[w]
	iandiu				heightscale, heightscale, 0x7fff
	mfir.y				distortScl,	 heightscale
	itof0.y				distortScl,	 distortScl

	itof15.w				buffline5,	buffline5
	ftoi0.w				buffline5,	buffline5
	itof0.w				buffline5,	buffline5
	add.x					distortScl,	VF00,				buffline5[w]

	div					Q,				buffline5[x],	distortScl[x]
	add.x					distortScl,	VF00,				Q
	div					Q,				buffline5[y],	distortScl[y]
	add.y					distortScl,	VF00,				Q
		  
	;; output/input pointers
	xitop					maxinput
	iadd					output1,		buffertop,		maxinput
	iaddiu				output2,		output1,			OUTBUFFERSHIFT
	iaddiu				tempbuffer,	output2,			OUTBUFFERSHIFT
	iaddiu				input,		buffertop,		0

	;; get kick pointers
	iaddiu				kick1,		output1,			0
	iaddiu				kick2,		output2,			0

	;; get the fog constants and flags
	lq						constants,	CONSTANTS(vi00)

	;; put giftag & stuff into place...
	;; tag
	sq.xyzw				buffline1,	0(output1)

	;; prim
	sq						constants,	1(output1)
	
	;; tex0_1
	mr32					TEX0_1,		buffline2
	mr32					TEX0_1,		TEX0_1
	iaddiu				REG,			vi00,				0x06
	mfir.z				TEX0_1,		REG
	sq.xyz				TEX0_1,		2(output1)
  
	;; test_1
	mr32					TEST_1,		buffline2
	iaddiu				REG,			vi00,				0x47
	mfir.z				TEST_1,		REG
	sq.xz					TEST_1,		3(output1)

	;; alpha_1
	mr32					ALPHA_1,		buffline3
	mr32					ALPHA_1,		ALPHA_1
	iaddiu				REG,			vi00,				0x42
	mfir.z				ALPHA_1,		REG
	sq.xyz				ALPHA_1,		4(output1)

	;; zbuf_1
	iaddiu				REG,			vi00,				0x4E
	mfir.z				buffline3,	REG
	sq.xyz				buffline3,	5(output1)

	;; tag
	sq.xyzw				buffline4,	6(output1)
	
	;; copy data to second output buffer
	StaticCopyData		output2,		0,					output1,				  0, OUTBUFFEROVERHEAD
  		  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; process loop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; generate write pointers & later read pointers (reserve 4 qw / particle),
	;; for now we use the output buffer
	;; for temp storage, we write first particles to be clipped, then particles that are not clipped.
	;; non-clipped particles overwrite this space as they write out.  8 qw of slush is allocted in the
	;; middle to make sure we dont overwrite the last particle before its loaded.
	;; data stored in temp buffer is:
	;;
	;;						ScrnX,		ScrnY,			ScrnZ,			  FOG
	;;						ClipX,		ClipY,			ClipZ,			  ClipW
	;;						Red,			Green,			Blue,				  Alpha
	;;						

	;; generate termination condition (2 qw / particle)
	iadd					endinput,	input,			count
	iadd					endinput,	endinput,		count

	;; init the clip register
	fcset					0x000000

	;; the current scene
	mtir					currscene,	constants[y]
		
ProcessParticleLoop:
	;; load particle
	lq						pline1,		0(input)
	lq						pline2,		1(input)

	;; get flags
	mtir					flagset[0],	pline2[x]
	mtir					flagset[1],	pline2[y]
	mtir					flagset[2],	pline2[z]
	mtir					flagset[3],	pline2[w]

	;; to allow us to remove the suppression box code
CURRBOX .assigna 0
.aif \&NUM_SUPPR_BOXES ne 0
.awhile \&CURRBOX LT \&NUM_SUPPR_BOXES
ADDR .equ (\&CURRBOX*4 + SUPPBOX_1)
	;; load the supression boxes (dynamically)
	LoadMatrix			SBox,			ADDR,				vi00

	;; cull against suppression boxes
	;; suppression boxes are represented by a transform into their space such that
	;; -1 to 1 is the size of the box in box space.
	;; boxresults will contain a 1 if we are outside the box...
	ClipTestBox \&CURRBOX
CURRBOX .assigna \&CURRBOX + 1
.aendw	

	;; boxresults = ~boxresults
	;; this will make boxresults have a 1 for inside the box...
	isub					boxresults,	vi00,				boxresults
	isubiu				boxresults,	boxresults,		1

	;; mask off ignored boxes
	iand					boxresults,	boxresults,		flagset[1]
	
	;; TJC - removed because the engine has to walk the particles anyway
	;; cull for correct scene (bottom 4 bits of flagset[0])
;	iand					scene,		flagset[0],		currscene
;	ior					boxresults,	boxresults,		scene

	;; mask off any invalid bits
	iaddiu				mask,			vi00,				0xFF
	iand					boxresults,	boxresults,		mask

	;; see if any bit got set...
	IsNoBitSet			nocullflag,	boxresults
	ibeq					nocullflag,	vi00,				Skip_Particle

.aendi
	
	;; build default normal... (0,1,0,0)
	Zero					normal
	One.y					normal
	
	;; build orientation matrix...
	iaddiu				mask,			vi00,				0x08
	iand					hasRot,		flagset[2],		mask
	iaddiu				mask,			vi00,				0x10
	iand					alignVel,	flagset[2],		mask
	iaddiu				mask,			vi00,				0x03
	iand					orient,		flagset[2],		mask
	ibeq					orient,		vi00,				orientScreen
	isubiu				orient,		orient,			1
	ibeq					orient,		vi00,				orientFree
	isubiu				orient,		orient,			1
	ibeq					orient,		vi00,				orientGround

orientWall:
	;; not currently handled... fall through to screen orienting...
	
orientScreen:
	;; build corner vectors
	Zero					vCorner[0]
	Zero					vCorner[1]
	sub.x					vCorner[0],	vCorner[0],		pline1
	mr32.y				vCorner[0],	pline1
	move.x				vCorner[1], pline1
	mr32.y				vCorner[1], pline1
	
	;; build normal... (0,0,-1,0)
	;; the C code doesnt build a normal for screen oriented particles?  I think its uninitialized... 
	Zero					normal
	NegOne.z				normal
	
	;; get the camera to world matrix
	LoadMatrix			Orient,		CAM_TO_WRLD,	vi00
	;; we dont want the translation
	;Ident					Orient[3]
	
	ibne					hasRot,		vi00,				orientScreen_hasRot
	ibeq					alignVel,	vi00,				orientDone
	
orientScreen_alignVel:
	LoadMatrix			WrldToCam,	WRLD_TO_CAM,	vi00
	TransformOrient	vVel,			WrldToCam

	;; ok... we need to eliminate the sin(atan(y/x)) thing... ick...
	;; its much simpler than even what Ned did... 
	;; zero vVel[z], then just normalize vVel
	;; and vVel.x = cos(ang) & vVel.y = sin(ang)
	;; from the sin & cos we build the matrix

	;; we need to know if the velocity is zero as well
	;; normalize ends with a mulq, so just check if x and y were zero... ICK!...
	iaddiu				zflag,		vi00,				0x0C

	;; do the normalization
	Zero.zw				vVel
	NormalizeVector3	vVel,			vVel

	;; grab the zero flags
	fmand					zeroLength,	zflag

	;; now just make a matrix from the angles
	PreMultZRotMatrixCS3	OrientTemp,	Orient,	vVel

	;; branch if both x&y are zero...
	ibeqiu				zeroLength,	0x0C,				orientDone

	MoveMatrix			Orient,		OrientTemp

	b						orientDone

orientScreen_hasRot:	
	MakeZRotMatrix		ZRot,			pline2,			w,					  1
	MultiplyMatrix33	Orient,		ZRot,				Orient
	b						orientDone
	
orientGround:
	;; build corner vectors
	Zero					vCorner[0]
	Zero					vCorner[1]
	sub.xz				vCorner[0],	vCorner[0],		pline1
	move.x				vCorner[1],	pline1
	sub.z					vCorner[1],	vCorner[1],		pline1

	ibne					hasRot,		vi00,				orientGround_hasRot
	ibne					alignVel,	vi00,				orientGround_alignVel
	
	;; this calls for an identity
orientGround_ident:
	MakeIdentMatrix	Orient
	b						orientDone
	
orientGround_hasRot:
	MakeYRotMatrix		Orient,		pline2,			z,					  1
	b						orientDone
	
orientGround_alignVel:

	; if changes are made to this section, they should be copied into the first section of orientFree_alignVel below

	; this is a rarely-used mode, don't worry too much about performance

	; first rotate in y to match x/z velocity, then tilt to match up/down. if x/z velocity is 0, use identity.
	; we need y-angle cos/sin, so normalize x/z velocity. we'll need x/z velocity length later, so don't use
	; NormalizeVector3

	mr32					vVel,			pline2									; x/y/z velocity is in pline2.yzw
	sub.z					vVel,			vf00,			vVel						; we want -z velocity
	Zero.yw				vVel														; just x/z
	LengthVector3		Q,				vVel										; get length

	; save off x/z length, check if it's 0

	addq.x				xzLen,		vf00,			Q							; xzLen.x = x/z length
	iaddiu				zflag,		vi00,			0x08						; x zero flag
	fmand					zeroLength,	zflag
	ibeqiu				zeroLength,	0x08,			orientGround_ident

	; finish normalizing x/z vector

	div					Q,				vf00[w],		xzLen[x]					; Q = 1 / x/z length
	mulq.xz				vVel,			vVel,			Q

	; make a matrix from the angles

	MakeYRotMatrixCzS	OrientTemp,	vVel

	; now look at tilt-- relation of x/z length to y

	mr32					vVel,			pline2									; x/y/z velocity is in pline2.yzw
	add.x					vVel,			xzLen,		vf00						; vVel.x = length of x/z velocity
	Zero.zw				vVel														; just x/y
	NormalizeVector3	vVel,			vVel										; normalize to convert to cos/sin

	; add to matrix and done

	PreMultZRotMatrixCS3	Orient,	OrientTemp,	vVel
	b						orientDone

orientFree:
	;; build corner vectors
	Zero					vCorner[0]
	Zero					vCorner[1]
	sub.xz				vCorner[0],	vCorner[0],		pline1
	move.x				vCorner[1],	pline1
	sub.z					vCorner[1],	vCorner[1],		pline1

	ibne					hasRot,		vi00,				orientFree_hasRot
	ibne					alignVel,	vi00,				orientFree_alignVel

orientFree_ident:		
	;; oh.. .ok, just an identity
	MakeIdentMatrix	Orient
	b						orientDone
	
orientFree_hasRot:
	MakeXYZRotMatrix	Orient,		pline2,			y,z,w,			  1
	b						orientDone
	
orientFree_alignVel:

	; this mode is like the ground align-to-velocity, but with a twist along velocity vector so that particle
	; faces screen as much as possible. start out with the ground align-to-velocity calculation...

	; this is a rarely-used mode, don't worry too much about performance

	; first rotate in y to match x/z velocity, then tilt to match up/down. if x/z velocity is 0, use identity.
	; we need y-angle cos/sin, so normalize x/z velocity. we'll need x/z velocity length later, so don't use
	; NormalizeVector3

	mr32					vVel,			pline2									; x/y/z velocity is in pline2.yzw
	sub.z					vVel,			vf00,			vVel						; we want -z velocity
	Zero.yw				vVel														; just x/z
	LengthVector3		Q,				vVel										; get length

	; save off x/z length, check if it's 0

	addq.x				xzLen,		vf00,			Q							; xzLen.x = x/z length
	iaddiu				zflag,		vi00,			0x08						; x zero flag
	fmand					zeroLength,	zflag
	ibeqiu				zeroLength,	0x08,			orientGround_ident

	; finish normalizing x/z vector

	div					Q,				vf00[w],		xzLen[x]					; Q = 1 / x/z length
	mulq.xz				vVel,			vVel,			Q

	; make a matrix from the angles

	MakeYRotMatrixCzS	OrientTemp,	vVel

	; now look at tilt-- relation of x/z length to y

	mr32					vVel,			pline2									; x/y/z velocity is in pline2.yzw
	add.x					vVel,			xzLen,		vf00						; vVel.x = length of x/z velocity
	Zero.zw				vVel														; just x/y
	NormalizeVector3	vVel,			vVel										; normalize to convert to cos/sin

	; add to matrix

	PreMultZRotMatrixCS3	Orient,	OrientTemp,	vVel

	; now calculate rotation around x-axis to tilt particle towards camera. i want to rotate into
	; "screen orient" mode so that we can see the particle as much as possible. to do this, rotate
	; the original velocity into camera space and look at the x/z components. a 0 x-component means
	; straight towards or away from camera, a 0 z-component means perpendicular to camera

	LoadMatrix			WrldToCam,	WRLD_TO_CAM,	vi00
	TransformOrient	vVel,			WrldToCam								; transforms pline2 by matrix

	; abs(vVel.z) so rotation goes back to 0 (instead of going to 180) for motion away from camera
	
	abs.z					vVel,			vVel

	; get cos and sin by normalizing, y-component gets cleared. we need to check if both x and z are 0,
	; so get do this by getting length and doing division manually instead of calling NormalizeVector3

	Zero.yw				vVel														; just x/z
	LengthVector3		Q,				vVel										; get length

	; save off x/z length, check if it's 0

	addq.x				xzLen,		vf00,			Q							; xzLen.x = x/z length
	iaddiu				zflag,		vi00,			0x08						; x zero flag
	fmand					zeroLength,	zflag
	ibeqiu				zeroLength,	0x08,			orientDone				; no rotation around x-axis needed, we're done

	; finish normalizing x/z vector

	div					Q,				vf00[w],		xzLen[x]					; Q = 1 / x/z length
	mulq.xz				vVel,			vVel,			Q

	; we are rotating around x-axis. the angle we want is ArcTan(vVel.x, vVel.z), but macro wants input
	; angles (cos/sin) in y and z

	addz.y				vVel,			vf00,			vVel						; vVel.y = 0 + vVel.z
	addx.z				vVel,			vf00,			vVel						; vVel.z = 0 + vVel.x
	Zero.xw				vVel
	PreMultXRotMatrixCS3	Orient,	Orient,	vVel

	; done

	b						orientDone
	
orientDone:

	;; put the particle position into a standard vector
	mr32.xz				pos,			pline1
	mr32.y				pos,			pos
	mr32.w				pos,			pline2
	mr32.z				pos,			pos
	move.w				pos,			vf00
	
	;; get color into floats for lighting & clipping
	mtir					red,			pline1[x]
	mtir					green,		pline1[y]
	mtir					blue,			pline1[z]
	mtir					alpha,		pline1[w]
	iaddiu				mask,			vi00,				0xFF
	iand					red,			red,				mask
	iand					green,		green,			mask
	iand					blue,			blue,				mask
	iand					alpha,		alpha,			mask
	mfir.x				Vtx[Clr],	red
	mfir.y				Vtx[Clr],	green
	mfir.z				Vtx[Clr],	blue
	mfir.w				Vtx[Clr],	alpha
	itof0					Vtx[Clr],	Vtx[Clr]

	;; generate the lighting flag...
	iaddiu				mask,			vi00,				0x04
	iand					light,		flagset[2],		mask
	
	;; transform the corners
	TransformVector	vTransformedCorner[0],		Orient,			  vCorner[0]
	TransformVector	vTransformedCorner[1],		Orient,			  vCorner[1]

	;; add the corners on
	add					PartVtx[0],	pos,				vTransformedCorner[0]
	add					PartVtx[1],	pos,				vTransformedCorner[1]
	sub					PartVtx[2],	pos,				vTransformedCorner[0]
	sub					PartVtx[3],	pos,				vTransformedCorner[1]

	;; load clip matrix
	LoadMatrix			ToClip,		WRLD_TO_CLIP,	vi00
	
	;; transform particle to clip space...
	TransformVertexF	PartVtx[0],	ToClip,			PartVtx[0]
	TransformVertexF	PartVtx[1],	ToClip,			PartVtx[1]
	TransformVertexF	PartVtx[2],	ToClip,			PartVtx[2]
	TransformVertexF	PartVtx[3],	ToClip,			PartVtx[3]

	;; see if all verts exceeded any single plane...(if so it's out of view)
	clipw.xyz			PartVtx[0],	PartVtx[0]
	clipw.xyz			PartVtx[1],	PartVtx[1]
	clipw.xyz			PartVtx[2],	PartVtx[2]
	clipw.xyz			PartVtx[3],	PartVtx[3]
	fcor					vi01,			0xFBEFBE	; B 111110 111110 111110 111110 (all 4 +x)
	ibne					vi01,			vi00,				Skip_Particle
	fcor					vi01,			0xF7DF7D	; B 111101 111101 111101 111101 (all 4 -x)
	ibne					vi01,			vi00,				Skip_Particle
	fcor					vi01,			0xEFBEFB	; B 111011 111011 111011 111011 (all 4 +y)
	ibne					vi01,			vi00,				Skip_Particle
	fcor					vi01,			0xDF7DF7	; B 110111 110111 110111 110111 (all 4 -y)
	ibne					vi01,			vi00,				Skip_Particle
	fcor					vi01,			0xBEFBEF	; B 101111 101111 101111 101111 (all 4 +z)
	ibne					vi01,			vi00,				Skip_Particle
	fcor					vi01,			0x7DF7DF	; B 011111 011111 011111 011111 (all 4 -z)
	ibne					vi01,			vi00,				Skip_Particle
	
	;; light the particle
	ibeq					light,		vi00,				SkipLighting

	;; transform the normal for lighting...
	TransformVector	normal,		Orient,			normal

	;; load the light directions
	LoadVector4			LDIR_X,		vi00,				lightdirs[x]
	LoadVector4			LDIR_Y,		vi00,				lightdirs[y]
	LoadVector4			LDIR_Z,		vi00,				lightdirs[z]

	;; generate the light dots...
	mul					acc,			lightdirs[x],	normal[x]
	madd					acc,			lightdirs[y],	normal[y]
	madd					dots,			lightdirs[z],	normal[z]
	
	;; apparently, Ned tries to light these like they are two
	;; sided.  Therefore, we need a vector from particle to camera
	;; to check for backfacing to negate the dots..
	LoadVector4			CAM_TO_WRLD+3,	vi00,			camerapos
	sub					delta,		pos,				camerapos
	mul					delta,		delta,			normal					; we need dot product with normal
	addy.x				delta,		delta,			delta						; delta.x = delta.x + delta.y
	addz.x				delta,		delta,			delta						; delta.x = delta.x + delta.z-- delta.x holds dot product

	;; load the light colors...
	LoadVector4			AMBIENT,		vi00,				ambient
	LoadVector4			LCOLOR_0,	vi00,				lightcol[0]
	LoadVector4			LCOLOR_1,	vi00,				lightcol[1]
	LoadVector4			LCOLOR_2,	vi00,				lightcol[2]
	LoadVector4			LCOLOR_3,	vi00,				lightcol[3]
	
	;; now delta[x] has the dot in it... we just need the sign...
	;; 80 is the right bit for x neg flag...
	;; if we are backfacing, negate the dots from above...
	iaddiu				sflag,		vi00,				0x80
	fmand					sflag,		sflag											; test result from delta calculation above
	Zero					zeros
	ibne					sflag,		vi00,				SkipNegateDots
	sub					dots,			zeros,			dots
SkipNegateDots:		

	;; now generate colors based on the dots...
	;; first we clamp backlights out of the picture
	ClampAtZero			dots,			dots

	;; simple method here... just the standard (I should build macros for this lighting stuff)
	add					ACC,			ambient,			vf00[x]
	madd					ACC,			lightcol[0],	dots[x]
	madd					ACC,			lightcol[1],	dots[y]
	madd					ACC,			lightcol[2],	dots[z]
	madd					lighting,	lightcol[3],	dots[w]
	ClampAtOne			lighting,	lighting

	;; now we have the light color..
	;; modulate by it
	mul					Vtx[Clr],	Vtx[Clr],		lighting

SkipLighting:			

	;; for fogging, we alpha out the particle over the fog range (instead of using the gs fog register). we calculate
	;; a multiplier such that it is 1.0 at the near fog plane, and 0.0 at the far fog plane. then we multiply that
	;; into the alpha component. we have two fog constants, a scale and a translate (calculated by ParticleMgr__PS2PrepRender)
	;; in clip space, the w component is the distance to the camera plane (it is the negative of the z-component in
	;; camera space), so it changes linearly between the near and far clip planes, which is what we need-- the fog
	;; constant calculations are done with this is mind

	;; later, we decided to have a squared dropoff instead of a linear dropoff. the fog constants were modified to
	;; produce 0.0 at the near fog distance, and 1.0 at the far distance. then we square the value, subtract it from
	;; 1.0, and that's our alpha multiplier

	add					alpha,		PartVtx[0],		PartVtx[2]		; add opposite vertices-- w-component is all we really care about
	loi					0.5
	mul.w					alpha,		alpha,			I					; multiply by 0.5 to get center
	mul.w					alpha,		constants,		alpha				; multiply by scale factor in constants.w
	add.w					alpha,		alpha,			constants[z]	; translate by constants.z
	ClampAtZero			alpha,		alpha
	ClampAtOne			alpha,		alpha
	mul.w					alpha,		alpha,			alpha				; square it
	sub.w					alpha,		vf00,				alpha				; 1.0 - value squared (vf00.w is 1.0)
	mul.w					Vtx[Clr],	Vtx[Clr],		alpha				; update color alpha component

	;; where do we write the particle to
	iaddiu				clipptr1,	tempbuffer,		0
	iaddiu				clipptr2,	clipptr1,		CLIP_BUFFER_SIZE

	;; check if its a distortion particle... if so, we need a bit of a different generation block for it...
	;; we always must generate within the square built by the 4 verts we have worked with thus far, or else
	;; we will break the clipping check above.
	iaddiu				mask,			vi00,				0x20
	iand					distort,		flagset[2],		mask
	ibeq					distort,		vi00,				SkipDistortion


	;; for distortion particles, we throw out the entire particle if any part of it
	;; passes near or far clip...  this simplifies the microcode immensly.
	fcand					vi01,			0xC30C30	; B 110000 110000 110000 110000 (any +z or -z)
	ibne					vi01,			vi00,				Skip_Particle

	;; get to-screen matrix
	LoadMatrix			ToScreen,	CLIP_TO_SCRN,	vi00

	;; for now, this code keeps the square and just adds a vert in the middle as the center of the fan
	;; we also need to dup the second vert to close the fan...

	;; generate center vtx
	TransformVertexF	PartVtx[4],	ToClip,			pos

	;; generate center texture coordinate.. for now this is ALWAYS just down-right of center
	;; in theory, that should be sufficient, as the motion of the particle should be able to
	;; do the rest....  We may want to be able to adjust the strength later though...
	div					Q,				vf00[w],			PartVtx[4][w]
	One.zw				Vtx[Tex]
	move.xy				Vtx[Tex],	PartVtx[4]
	mul					Vtx[Tex],	Vtx[Tex],		Q
	;; adjust the center tex coord out of center.
	;; we use the alpha value of the color as the strength of the distortion.
	;; since we cant distort in an alpha friendly way.  That way its
	;; fade-able, and attached to an envelope.
	;; 0.04 * alpha
	;; where alpha is 0-1.  that gives us these points:
	;; alpha=0.0	adjustment=0
	;; alpha=0.5	adjustment=0.02
	;; alpha=1.0	adjustment=0.04
	;; we use vTransformedCorner[0] as the direction of the distortion
	;; since it rotates with the particle.
	
	;; convert alpha to 0-1 (separate step because we use alpha again below)
	loi					0x3c000000 ; 1 / 128
	mul.w					alpha,		Vtx[Clr],		I

	;; scale by 0.04
	loi					0x3D23D70A ; 0.04
;	loi					0.0
	mul.w					adj,			alpha,			I

	;; build tex coord delta and add it...
	mul.xy				adj,			vTransformedCorner[0],	adj[w]
	add.xy				Vtx[Tex],	Vtx[Tex],		adj

	;; have to fade color to white while Im at it...
	;; color = color*alpha + white - alpha
	mul.xyz				Vtx[Clr],	Vtx[Clr],		alpha[w]
	loi					0x43000000 ; 128
	add.xyz				Vtx[Clr],	Vtx[Clr],		I
	sub.xyz				Vtx[Clr],	Vtx[Clr],		Vtx[Clr][w]
	
	;; cant use alpha when drawing...
	loi					128.0
	mul.w					Vtx[Clr],	vf00,				I
	
	;; nasty correction code.. not sure what to do with it...
	loi					0x3f000000 ; 0.5
	mul.xy				Vtx[Tex],	Vtx[Tex],		I
	add.xy				Vtx[Tex],	Vtx[Tex],		I
	mul.xy				Vtx[Tex],	Vtx[Tex],		distortScl

	;; center vert
	sq						PartVtx[4],	0(clipptr1)
	sq						Vtx[Clr],	1(clipptr1)
	sq						Vtx[Tex],	2(clipptr1)
	
	;; repeat center for edge list
	sq						PartVtx[4],	18(clipptr1)
	sq						Vtx[Clr],	19(clipptr1)
	sq						Vtx[Tex],	20(clipptr1)
		
	;; cant use antyhing but white for the corners
	loi					128.0
	add.xyz				Vtx[Clr],	vf00,				I
	
	;; 4 corners & repeated first corner vtx
	sq						PartVtx[0],	3(clipptr1)
	sq						PartVtx[0],	15(clipptr1)
	sq						Vtx[Clr],	4(clipptr1)
	sq						Vtx[Clr],	16(clipptr1)
	div					Q,				vf00[w],			PartVtx[0][w]
	One.z					PartVtx[0]
	mul					Vtx[Tex],	PartVtx[0],		Q
	loi					0.5
	mul.xy				Vtx[Tex],	Vtx[Tex],		I
	add.xy				Vtx[Tex],	Vtx[Tex],		I
	mul.xy				Vtx[Tex],	Vtx[Tex],		distortScl
	sq						Vtx[Tex],	5(clipptr1)
	sq						Vtx[Tex],	17(clipptr1)

	sq						PartVtx[1],	6(clipptr1)
	sq						Vtx[Clr],	7(clipptr1)
	div					Q,				vf00[w],			PartVtx[1][w]
	One.z					PartVtx[1]
	mul					Vtx[Tex],	PartVtx[1],		Q
	loi					0.5
	mul.xy				Vtx[Tex],	Vtx[Tex],		I
	add.xy				Vtx[Tex],	Vtx[Tex],		I
	mul.xy				Vtx[Tex],	Vtx[Tex],		distortScl
	sq						Vtx[Tex],	8(clipptr1)
	
	sq						PartVtx[2],	9(clipptr1)
	sq						Vtx[Clr],	10(clipptr1)
	div					Q,				vf00[w],			PartVtx[2][w]
	One.z					PartVtx[2]
	mul					Vtx[Tex],	PartVtx[2],		Q
	loi					0.5
	mul.xy				Vtx[Tex],	Vtx[Tex],		I
	add.xy				Vtx[Tex],	Vtx[Tex],		I
	mul.xy				Vtx[Tex],	Vtx[Tex],		distortScl
	sq						Vtx[Tex],	11(clipptr1)
	
	sq						PartVtx[3],	12(clipptr1)
	sq						Vtx[Clr],	13(clipptr1)
	div					Q,				vf00[w],			PartVtx[3][w]
	One.z					PartVtx[3]
	mul					Vtx[Tex],	PartVtx[3],		Q
	loi					0.5
	mul.xy				Vtx[Tex],	Vtx[Tex],		I
	add.xy				Vtx[Tex],	Vtx[Tex],		I
	mul.xy				Vtx[Tex],	Vtx[Tex],		distortScl
	sq						Vtx[Tex],	14(clipptr1)

	;; number of verts in our fan right now...
	iaddiu				numVerts,	vi00,				6
	
	b						EdgeListReady

SkipDistortion:
	
	;; 4 corners & repeated corner vtx
	sq						PartVtx[0],	0(clipptr1)
	sq						Vtx[Clr],	1(clipptr1)
	Zero					Vtx[Tex]
	sq						Vtx[Tex],	2(clipptr1)
	sq						PartVtx[1],	3(clipptr1)
	sq						Vtx[Clr],	4(clipptr1)
	One.x					Vtx[Tex]
	sq						Vtx[Tex],	5(clipptr1)
	sq						PartVtx[2],	6(clipptr1)
	sq						Vtx[Clr],	7(clipptr1)
	One.y					Vtx[Tex]
	sq						Vtx[Tex],	8(clipptr1)
	sq						PartVtx[3],	9(clipptr1)
	sq						Vtx[Clr],	10(clipptr1)
	Zero.x				Vtx[Tex]
	sq						Vtx[Tex],	11(clipptr1)
	sq						PartVtx[0],	12(clipptr1)
	sq						Vtx[Clr],	13(clipptr1)
	Zero					Vtx[Tex]
	sq						Vtx[Tex],	14(clipptr1)

	;; number of verts in our fan right now...
	iaddiu				numVerts,	vi00,				4

EdgeListReady:	

	;; see if we need to clip...
	;; if any vert exceeded a plane...
	;; we already culled for if all verts exceeded any single plane.
	fcand					vi01,			0xFFFFFF
	ibeq					vi01,			vi00,				Skip_Clip
;	assertmem clipptr1
;	assertmem clipptr2
	
	;; save some registers off to help clipping keep from running out...
	PushIntVector4		kick1, kick2, input, endinput
	PushIntVector4		tempbuffer
	
	;; clip the fan...
	StandardClip		numVerts, clipptr1, clipptr2, 2, "xXyYzZ", Clipped_Particle

	;; restore the int registers we saved off... (also it it at Clippped_Particle)
	;; also swap kick pointers
	PopIntVector4		tempbuffer
	PopIntVector4		kick1, kick2, input, endinput
;	assertmem kick1
;	assertmem kick2
;	assertmem input
;	assertmem endinput
;	assertlt input, endinput
;	assertmem tempbuffer
;	assertmem clipptr1
;	assertmem clipptr2
;	assertrange numVerts, 3, 12
;	assertlt kick1, clipptr1
;	assertlt kick2, clipptr1

Skip_Clip:			
		
	;; ok, lets render this thing...

	;; get output pointers
	iaddiu				rout,			kick1,			OUTBUFFEROVERHEAD
	iadd					rout_end,	rout,				numVerts
	iadd					rout_end,	rout_end,		numVerts
	iadd					rout_end,	rout_end,		numVerts
	
	;; get to-screen matrix
	LoadMatrix			ToScreen,	CLIP_TO_SCRN,	vi00

	;; patch giftag... (first by adding 0x8000 to set EOP)
	iaddiu				tag,			numVerts,		0x7fff
	iaddiu				tag,			tag,				0x1
	isw.x					tag,			-1(rout)
renderLoop:
	--loopCS 3, 1
	lq						Vtx[Vtx],	0(clipptr1)
	lq						Vtx[Clr],	1(clipptr1)
	lq						Vtx[Tex],	2(clipptr1)
	ftoi0					Vtx[Clr],	Vtx[Clr]
	mr32.z				Vtx[Tex],	vf00
	TransformVertexF	Vtx[Vtx],	ToScreen,		Vtx[Vtx]
	div					Q,				vf00[w],			Vtx[Vtx][w]
	mul					Vtx[Vtx],	Vtx[Vtx],		Q
	mul					Vtx[Tex],	Vtx[Tex],		Q
	ftoi4					Vtx[Vtx],	Vtx[Vtx]
	sq						Vtx[Tex],	0(rout)
	sq						Vtx[Clr],	1(rout)
	sq						Vtx[Vtx],	2(rout)
	iaddiu				clipptr1,	clipptr1,		3
	iaddiu				rout,			rout,				3
;	assertle				rout,			rout_end
	ibne					rout_end,	rout,				renderLoop

	--barrier
.raw
	nop					nop
	nop					nop
	nop					nop
.endraw

;	assertmem			kick1
;	assertmem			kick2
	isub					t,				kick1,			kick2
	ibgtz					t,				kick1upper
;	assertlt				rout,			kick2
kick1upper:				
;	assertlt				rout,			clipptr1

	xgkick				kick1

	;; swap kick pointers
	iaddiu				temp,			kick1,			0	
	iaddiu				kick1,		kick2,			0	
	iaddiu				kick2,		temp,				0
	
Skip_Particle:
	;; loop iteration
	iaddiu				input,		input,			2
;	assertle				input,		endinput
	ibne					input,		endinput,		ProcessParticleLoop

done:						
			  
--cont
	b						NewBuffer
	
Clipped_Particle:
	;; restore the int registers we saved off... (also it it at Clippped_Particle)
	;; also swap kick pointers
	PopIntVector4		tempbuffer
	PopIntVector4		kick1, kick2, input, endinput
	b Skip_Particle	
	
	;; insert the low level routines (this inserts most of them... vcl prunes all that dont get called)
	HighLevelSubroutines
	ScissorInterpolater 2
