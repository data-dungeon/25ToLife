; take as input:
;
; For simplicity sake, this microcode always works from relative addresses.  It
; is fully double buffered.
; 0x0000 - 0x1FF0 Is the first buffer
; 0x2000 - 0x3FF0 Is the second buffer.
; A detailed description of whats in each buffer follows.  Addresses listed are
; relative to the current buffer.
;
; 0x0000 - 0x0060 Output Buffer texture GIFTAG & registers
; 0x0070 -        Output Buffer particulate GIFTAG
; 0x0080 - 0x1700 Output buffer, 9 qw per particulate (39 particulates + one slop spot)
;     +0x000 -		STQ0
;     +0x010 -    RGBA
;     +0x020 -		XYZ0
;		+0x030 -		ST1
;		+0x040 - 	XYZ1
;		+0x050 -		ST2
;		+0x060 - 	XYZ2
;		+0x070 -		ST3
;		+0x080 - 	XYZ3
; 0x1710 - GIFTAG texture settings
; 0x1720 - TEX0_1
; 0x1730 - TEX1_1
; 0x1740 - ALPHA_1
; 0x1750 - CLAMP_1
; 0x1760 - MIPTBP1_1
; 0x1770 - MIPTBP2_1
; 0x1780- GIFTAG particulates
; 0x1790- WorldToCull Matrix Row 0
; 0x17A0- WorldToCull Matrix Row 1
; 0x17B0- WorldToCull Matrix Row 2
; 0x17C0- WorldToCull Matrix Row 3
; 0x17D0- CullToScreen Matrix Row 0 W = screen width
; 0x17E0- CullToScreen Matrix Row 1 W = screen height
; 0x17F0- CullToScreen Matrix Row 2 W = DragCoefficient
; 0x1800- CullToScreen Matrix Row 3 W = mCurrentTime
; 0x1810- color[0]		RGBA
; 0x1820- color[1]		RGBA
; 0x1830- color[2]		RGBA
; 0x1840- color[3]		RGBA
; 0x1850- X = scale point 2, Y = scale point 3, Z = scale point 1, W = scale point 0
; 0x1860- XYZ = AccelerationVector, W = AngularAccel
; 0x1870- 0x1FFF Input buffer, 3 qw per particulate (39 particulates)
;		+0x000 - 	XYZ = mInitialPosition, W = mInitialAngularPosition
;		+0x001 -    XYZ = mInitialVelocity, W = mInitialAngularVelocity
;		+0x002 -    X = mBirthTime, Y = mDeathTime, Z = mInitialScale, W = UNUSED
;

MAX_SCALE .equ 300.0

.init_vf_all
.init_vi_all

.alternate
.include "High.i"

.name Particulate
--enter
--endenter

--cont

Particulate_Restart:

	; get buffer pointer
	xtop				output
	iaddiu			buffertop,				output,					0x171
	imove				kickaddr,				output
	
	; copy giftag into place.
	lqi				giftagqw,				(buffertop++) ; GIFTAG
	sqi				giftagqw,				(output++)
	lqi				giftagqw,				(buffertop++) ; tex0
	sqi				giftagqw,				(output++)
	lqi				giftagqw,				(buffertop++) ; tex1
	sqi				giftagqw,				(output++)
	lqi				giftagqw,				(buffertop++) ; alpha
	sqi				giftagqw,				(output++)
	lqi				giftagqw,				(buffertop++) ; clamp
	sqi				giftagqw,				(output++)
	lqi				giftagqw,				(buffertop++) ; miptbp1
	sqi				giftagqw,				(output++)
	lqi				giftagqw,				(buffertop++) ; miptbp2
	sqi				giftagqw,				(output++)
	lqi				giftagqw,				(buffertop++) ; GIFTAG
	sqi				giftagqw,				(output++)
	
	; get number of particulates
	mtir				partcount,				giftagqw[x]
	iandiu			partcount,				partcount,				0x7FFF
	
	; load matrix
	lqi				worldtocull[0],		(buffertop++)
	lqi				worldtocull[1],		(buffertop++)
	lqi				worldtocull[2],		(buffertop++)
	lqi				worldtocull[3],		(buffertop++)
	lqi				culltoscreen[0],		(buffertop++)
	lqi				culltoscreen[1],		(buffertop++)
	lqi				culltoscreen[2],		(buffertop++)
	lqi				culltoscreen[3],		(buffertop++)
	
	; load effect parameters         	
	lqi				color[0],				(buffertop++)
	lqi				color[1],				(buffertop++)
	lqi				color[2],				(buffertop++)
	lqi				color[3],				(buffertop++)
	lqi				params[0],				(buffertop++)
	lqi				params[1],				(buffertop++)
	
	; generate texture coords
	one.xz			texcoord[0]
	zero.y			texcoord[0]
	one.xy			texcoord[1]
	zero.xy			texcoord[2]
	zero.x			texcoord[3]
	one.y				texcoord[3]

	; generate adc flag
	iaddiu			adcbit,					vi00,						0x7FFF
	iaddiu			adcbit,					adcbit,					0x0001

	; make vcl happy
	fcset				0x000000

	; particulate loop.
ParticulateLoop:
--LoopCS 1, 1

	; load the particulate
	lqi					particulate[0],	(buffertop++)
	lqi					particulate[1],	(buffertop++)
	lqi					particulate[2],	(buffertop++)

	; calculate current age
	; age = absTime - part->mBirthTime
	sub.w					age,					cullToScreen[3],		particulate[2][X]

	; what percent of our life is that
	sub.y					lifelength,			particulate[2],		particulate[2][X]
	div					Q,						age[W],					lifelength[Y]
	add.y					lifepercent,		VF00,						Q

	; fT = lifepercent
	; om = 1-fT
	; color =
	; om * om * om * point 0 +
	; om * om * ft * point 1 +
	; om * ft * ft * point 2 +
	; ft * ft * ft * point 3

	; ft2 = ft * ft
	; om2 = om * om
	; ft3 = ft2 * ft
	; om3 = om2 * om

	; ?,			ft,			?,				om
	sub.w					lifepercent,		VF00,						lifepercent[Y]
	; ft,			ft,			om,			om
	mr32.xz				lifepercent,		lifepercent
	; ft,			ft,			om,			om
	move					coefficients,		lifepercent
	; ft,			ft*ft,		om,			om*om
	mul.yw				coefficients,		coefficients,			lifepercent
	; ft*ft,		ft*ft*ft,	om*om,		om*om*om
	mul					coefficients,		coefficients,			lifepercent
	; ft,			om,			om,			ft
	mr32					lifepercent,		lifepercent
	; om,			om,			ft,			ft
	mr32					lifepercent,		lifepercent
	; ft*ft*om,	ft*ft*ft,	om*om*ft,	om*om*om
	mul.xz				coefficients,		coefficients,			lifepercent

	; now we can do the spline...
	mul					ACC,					color[0],				coefficients[W]
	madd					ACC,					color[1],				coefficients[Z]
	madd					ACC,					color[2],				coefficients[X]
	madd					finalcolor,			color[3],				coefficients[Y]
	max					finalcolor,			finalcolor,				VF00[X]
	loi					128.0
	mini					finalcolor,			finalcolor,				I

	; to bytes.	
	ftoi0					finalcolor,			finalcolor

	; calculate current scale
	mul					scalevalues,		params[0],				coefficients
	mul.z					ACC,					particulate[2],			scalevalues[X]
	madd.z				ACC,					particulate[2],			scalevalues[Y]
	madd.z				ACC,					particulate[2],			scalevalues[Z]
	madd.z				scale,				particulate[2],			scalevalues[W]
	
	; caculate acceleration * time
	mul					acceltime,			params[1],				age[W]
	
	; calculate current position/orientation
	; rearrangement / splitting of original equation
	;   position = initial + ((velocity + accel * time) / (1 + drag * time)) * time
	;   acceltime = accel * time;
	;   dragtime = 1 + drag * time;
	;   position = initial + ((velocity + acceltime) / dragtime) * time
	;   position = initial + ((velocity + acceltime) * (time / dragtime)
	;   timedragtime = time / (1 + drag * time)
	;   position = initial + ((velocity + acceltime) * timedragtime
	;   position = initial + velocity*timedragtime + acceltime*timedragtime
	add.w             ACC,					VF00,						VF00[X]
	madd.w            dragtime,			age,						cullToScreen[2][W]
	div					Q,						age[W],					dragtime[W] ; Q = timedragtime
	add					ACC,					particulate[0],		VF00[X]
	madd					ACC,					particulate[1],		Q
	madd					position,			acceltime,				Q
	; rotational velocity was also dragged using that last bit, so we need to redo it
	; without drag... easy enough..
	add.w					ACC,					particulate[0],		VF00[X]
	madd.w				ACC,					particulate[1],		age[W]
	madd.w				angle,				acceltime,				age[W]
	
	; transform position to cull space
	TransformVertex	screenspace,		worldToCull,			position
	div					Q,						VF00[W],					screenspace[W]
	mul.xyz				screenspace,		screenspace,			Q
	
	; get clip scale factor
	; +1 for safety, divide to make into percent of GS coordinate space
	; 1- to represent amount of GS coordinate space that is valid.
	li.w           	clipscalefactor,  1.0-((MAX_SCALE+1.0)/2048.0)

	; cull
	clipw.xyz			screenspace,		clipscalefactor[W]
	clipw.xyz			screenspace,		VF00[W]
	fcand					vi01,					0x0003F0       ; previous +x/-x/+y/-y current +z/-z
	iaddiu				clipadc,				vi01,						0x7fff

	; go from cull space to screen space
	TransformVertex	screenspace,		cullToScreen,			screenspace

	; adjust scale for Z, and put in x & y
	mul.z					scale,				scale,					Q
	add.xy				scale,				VF00,						scale[Z]
	
	; correct for screen aspect ratio (hidden in cullToScreen W fields. since we
	; never reference screenspace[w], the w fields of cullToScreen are not used by
	; the above transform.)  We use them here though.
	mul.x					scale,				scale,					cullToScreen[0][W]
	mul.y					scale,				scale,					cullToScreen[1][W]

	; clamp scale to max of arbitrary 300....
	loi					MAX_SCALE
	mini.xy				scale,				scale,					I
	
	; zero out fog & set adc for first 2 verts
	mfir.w				screen[0],			adcbit
	mfir.w				screen[1],			adcbit
	mfir.w				screen[2],			clipadc
	mfir.w				screen[3],			clipadc

	; need sine & cosine of angle.
	add.xyz				angle,				VF00,						angle[W]
	SCSC					sincosxy,			angle
	loi					-1.0
	mul.x					sincosxy,			sincosxy,				I
	mr32.xy				cossinxy,			sincosxy
	move.z				sincosxy,			VF00
	move.z				cossinxy,			VF00

	; apply scale
	mul.xy				sincosxy,			sincosxy,				scale
	mul.xy				cossinxy,			cossinxy,				scale

   
	; make verticies
	add.xyz				screen[0],			screenspace,			sincosxy
	add.xyz				screen[1],			screenspace,			cossinxy
	sub.xyz				screen[2],			screenspace,			cossinxy
	sub.xyz				screen[3],			screenspace,			sincosxy

	; make GS friendly
	ftoi4.xyz			screen[0],			screen[0]
	ftoi4.xyz			screen[1],			screen[1]
	ftoi4.xyz			screen[2],			screen[2]
	ftoi4.xyz			screen[3],			screen[3]

	; store them out...
	sqi.xyz				texcoord[0],		(output++)
	sqi					finalcolor,			(output++)
	sqi					screen[0],			(output++)
	sqi.xy				texcoord[1],		(output++)
	sqi					screen[1],			(output++)
	sqi.xy				texcoord[2],		(output++)
	sqi					screen[2],			(output++)
	sqi.xy				texcoord[3],		(output++)
	sqi					screen[3],			(output++)
	
	isubiu				partcount,			partcount,				1
	ibne					partcount,			vi00,						ParticulateLoop

.raw
	nop nop
.endraw

	xgkick				kickaddr

.raw
	nop nop
.endraw

--barrier	
--cont
	b						Particulate_Restart


