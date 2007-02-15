

;;;
;;; Here lies the water code for the PS2.  This is a pretty nastly piece of work
;;; so if possible, I would suggest avoiding it *GRYN*
;;;
;;; Author:		  Tyler Colbert
;;;



.alternate
.include "High.i"
.include "Assert.i"
	
;;; give vcl all the vu registers
.init_vf_all
.init_vi_all


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; a bunch of constants where they can be modified easier...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
WATER_DEBUG					.equ 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Flag values passed to water...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FLG0_RENDER_NORMALS		.equ	1
FLG0_DONT_DRAW_WATER		.equ	2
FLG0_RAND_COLOR			.equ	16
FLG0_FADE_ON_DAMPENVAL	.equ	32

LAYERFLG_FADE_EDGES		.equ  1
LAYERFLG_FADE_CENTER		.equ	2
LAYERFLG_FADE_ALL			.equ	3
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; number of things that fit in some of the sections of memory
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MAX_SPRITES					.equ	10
MAX_BUMPS					.equ	1
MAX_AFFECTORS				.equ	5
MAX_LAYERS					.equ	5
MAX_WAKE_PAIRS				.equ	25
MAX_RINGS					.equ	3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; sizes of single objects in the memory map (in qwords)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SIZE_AFFECTOR				.equ	1
SIZE_SPRITE					.equ	7
SIZE_BUMP					.equ	1
SIZE_LAYER					.equ	6
SIZE_WAKE_PAIR				.equ	1
SIZE_RING					.equ	1
SIZE_MATRIX					.equ	4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; sizes of blocks in the memory map... (in qwords)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SIZE_MISC_DATA				.equ  29
SIZE_SPRITES				.equ	(MAX_SPRITES * SIZE_SPRITE)
SIZE_BUMPS					.equ	(SIZE_BUMP * MAX_BUMPS)
SIZE_WAKE_PAIRS			.equ	(SIZE_WAKE_PAIR * MAX_WAKE_PAIRS)
SIZE_RINGS					.equ	(SIZE_RING * MAX_RINGS)
SIZE_AFFECTORS				.equ	(SIZE_AFFECTOR * MAX_AFFECTORS)
SIZE_LAYERS					.equ	(SIZE_LAYER * MAX_LAYERS)
SIZE_NOISETABLE			.equ	16
SIZE_GRIDS					.equ	86
SIZE_STACK					.equ	48
SIZE_INPUT_DATA			.equ	(SIZE_NOISETABLE + SIZE_GRIDS + SIZE_LAYERS + SIZE_AFFECTORS + SIZE_WAKE_PAIRS + SIZE_BUMPS + SIZE_SPRITES + SIZE_MISC_DATA)

;;; quite a bit of math for these...
SIZE_SIGNS					.equ	4
SIZE_WORKSPACE				.equ	2
SIZE_MISCKICKBUFFER		.equ	2
SIZE_TEMPDATA				.equ	(SIZE_STACK + SIZE_SIGNS + SIZE_WORKSPACE + SIZE_MISCKICKBUFFER)
REMAINDER_SIZE				.equ	(1024 - (SIZE_INPUT_DATA + SIZE_TEMPDATA))
KICK_VTX_SIZE				.equ	3
NORMAL_VTX_SIZE			.equ	1
NUM_KICK_BUFFERS			.equ	3
NUM_NORMAL_BUFFERS		.equ	2
KICK_VERT_SIZE				.equ	(KICK_VTX_SIZE * NUM_KICK_BUFFERS)
NORMAL_VERT_SIZE			.equ	(NORMAL_VTX_SIZE * NUM_NORMAL_BUFFERS)
VERT_SIZE					.equ	(NORMAL_VERT_SIZE + KICK_VERT_SIZE)
MAX_VERTS					.equ	(REMAINDER_SIZE / VERT_SIZE)
SIZE_OUTPUT					.equ	(MAX_VERTS * KICK_VTX_SIZE)
SIZE_NORMALBUFF			.equ	(MAX_VERTS * NORMAL_VTX_SIZE)
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; memory map constants...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MISC_DATA					.equ  0
SPRITES						.equ	(MISC_DATA + SIZE_MISC_DATA)
BUMPS							.equ	(SPRITES + SIZE_SPRITES)
WAKE_PAIRS					.equ	(BUMPS + SIZE_BUMPS)
RINGS				 			.equ	(WAKE_PAIRS + SIZE_WAKE_PAIRS)
AFFECTORS					.equ	(RINGS + SIZE_RINGS)
LAYERS						.equ	(AFFECTORS + SIZE_AFFECTORS)
NOISETABLE					.equ	(LAYERS + SIZE_LAYERS)
GRIDS							.equ	(NOISETABLE + SIZE_NOISETABLE)
SIGNS							.equ	(GRIDS + SIZE_GRIDS)
STACK							.equ	(SIGNS + SIZE_SIGNS)
WORKSPACE					.equ	(STACK + SIZE_STACK)
MISC_KICKBUFFER			.equ	(WORKSPACE + SIZE_WORKSPACE)
OUTPUT_1						.equ	(MISC_KICKBUFFER + SIZE_MISCKICKBUFFER)
OUTPUT_2						.equ	(OUTPUT_1 + SIZE_OUTPUT)
OUTPUT_3						.equ	(OUTPUT_2 + SIZE_OUTPUT)
NORMAL_1						.equ	(OUTPUT_3 + SIZE_OUTPUT)
NORMAL_2						.equ	(NORMAL_1 + SIZE_NORMALBUFF)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; some more specific places in memory
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SCALE							.equ	(MISC_DATA)
SUBDIV_DEPTH				.equ	(SCALE)
NOISE_PART1					.equ	(SCALE)
GIFTAG						.equ	(SCALE + 1)
STRIPTAG						.equ	(GIFTAG + 1)
MISCTAG						.equ	(STRIPTAG + 1)
EYE_PT2						.equ	(MISCTAG)
EYE							.equ	(MISCTAG + 1)
FLAGS0						.equ	(EYE) ; 8 bits in X...
FLAGS1						.equ	(EYE) ; 8 bits in Y...
NOISE_PART2					.equ	(EYE)
LOCAL_TO_CLIP				.equ	(EYE + 1)
CLIP_TO_SCREEN				.equ	(LOCAL_TO_CLIP + SIZE_MATRIX)
WORLD_TO_LOCAL				.equ	(CLIP_TO_SCREEN + SIZE_MATRIX)
SCREEN_TO_CLIP				.equ	(WORLD_TO_LOCAL + SIZE_MATRIX)
CLIP_TO_WORLD				.equ	(SCREEN_TO_CLIP + SIZE_MATRIX)
WAKE_RADIUS_SQ				.equ	(CLIP_TO_WORLD + SIZE_MATRIX)	; w
SURFACE_HEIGHT				.equ	(WAKE_RADIUS_SQ) ; y
OFFSET_XZ					.equ	(WAKE_RADIUS_SQ) ; x & z
RING_PARAMS1				.equ	(WAKE_RADIUS_SQ + 1)	; x=growrate, y=wavestartpos, z=frequency, w=1/wavecount
RING_PARAMS2				.equ	(WAKE_RADIUS_SQ + 2)	; x=wavephaseshift y=waveheight, z=attacktime, w=1/attacktime
RING_PARAMS3				.equ	(WAKE_RADIUS_SQ + 3)	; x=attack+sustain, z=1/releasetime
AFFECTORCOUNT				.equ	(AFFECTORS)
RINGCOUNT					.equ	(RINGS)
TOP_OF_STACK				.equ	(WORKSPACE)
MT								.equ	(GIFTAG)	; stored in w
WS_LAYERFLAGS				.equ	(WORKSPACE) ; y=help reduce register usage...
WS_ACTIVEBUFFERS			.equ	(WORKSPACE) ; w=prior-buffer
WS_NOPTAG					.equ	(WORKSPACE) ; x=0x8000
WS_CLIPFLAGS				.equ	(WORKSPACE)	; z=3F & each clip flag result of each vertex.  used to throw out fans that are totally out..
WS_NORMAL_BUFFERS			.equ	(WORKSPACE+1) ; x=buff0, y=buff1
WS_DARS_OUT_OF_VIEW		.equ	(WORKSPACE+1) ; z = DARS cell totally out of view flag. & DARS cell totally in view flag
GRID_BASE					.equ	(GRIDS+1) ; there is one qword of zeros at each end of the grids (for wrapping)
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; some internal values used.. umm.. internally.. :)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Q_UL							.equ	0
Q_UR							.equ	1
Q_DL							.equ	2
Q_DR							.equ	3
SCISSOR_LAYER_COUNT		.equ	(KICK_VTX_SIZE - 1)
SET_LAYER_REGS				.equ	6
SET_LAYER_KICK_SIZE		.equ	(SET_LAYER_REGS + 1)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; a couple of macros to help maintainability
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.macro INIT_CURR_KICK_BUFFER		tagaddr,					vtxcount,		eop=1, buffer=mKickBufferPtr
local eopbit gifval temp

	lq				temp,					tagaddr(vi00)

.aif "\vtxcount" ne ""
	;; patch giftag
.aif \eop eq 1
	ihighbit		eopbit
	ior			gifval,				\vtxcount,				eopbit
	mfir.x		temp,					gifval
.aelse
	mfir.x		temp,					\vtxcount
.aendi
.aendi

	sqi			temp,					(\buffer++)	:KICKBUFFER
.endm	

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; render the passed value as a normal at position pos
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro KickGIFRegPacked	reg, val
local regval, kickbuf
	iliu			regval,				\reg
	isw.z			regval,				MISC_KICKBUFFER(vi00)
	sq				\val,					MISC_KICKBUFFER+1(vi00)
	iliu			kickbuf,				MISC_KICKBUFFER
	xgkick		kickbuf
	NOPKICK
.endm
.macro KickGIFReg	reg, val
local regval, kickbuf
	iliu			regval,				0xE
	isw.z			regval,				MISC_KICKBUFFER(vi00)
	iliu			regval,				\reg
	mfir.z		\val,					regval
	sq.xyz		\val,					MISC_KICKBUFFER+1(vi00)
	iliu			kickbuf,				MISC_KICKBUFFER
	xgkick		kickbuf
	NOPKICK
.endm
.macro KickGIFRegInt	reg, val
local regval, kickbuf, dest
	iliu			dest,					\val
	mfir			dest,					dest
	KickGIFReg	\reg,					dest
.endm
.macro RenderNormal n, pos
local regcount, buff, toclip, toscreen, clipspace, screenSpace, local, screenSpace
	;; ok.. here goes... we just use the misc tag and the MISC_KICKBUFFER to
	;; get things done.. kindof messy, but it works...
	;; to do it, we need to have the gif tag in place
	iliu			regcount,			1
	iliu			buff,					MISC_KICKBUFFER
	INIT_CURR_KICK_BUFFER			MISCTAG,					regcount,		1,	buff

	;; set prim to line mode
	KickGIFRegInt	0x00,				0x1

	;; set color to red
	loi				0x7F00007F
	mul				color,			ones,						I
	KickGIFReg		0x01,				color

	;; turn of ztesting...
	loi				0x38F30000
	mul				test,				ones,						I
	KickGIFReg		0x47,				test
		
	;; now we can just put the two verts in place and kick them...
	;; first transform the position into place.
	TransformVertex	clipspace,	mLocalToClip,			\pos
	div			Q,						vf00[w],					clipspace[w]
	
	;; cull it if its out
	clipw			clipspace,			clipspace
	fcand			vi01,					0x00003F
	ibnez			vi01,					skipit\@

	;; first point...
	mul.xyz		clipspace,			clipspace,				Q
	LoadMatrix	toscreen,			CLIP_TO_SCREEN
	TransformVertex screenSpace,	toscreen,				clipspace
	ftoi4.xyz	screenSpace,		screenSpace
	zero.w		screenSpace
	KickGIFRegPacked	0x4,			screenSpace

	;; second point
	move.xyz		scalednormal,		\n
;	mul.xz		scalednormal,		scalednormal,			mInvScale
	NormalizeVector3 scalednormal,	scalednormal
;	mul.xz		scalednormal,		scalednormal,			mInvScale
	loi			0.25
	mul.xyz		scalednormal,		scalednormal,			I
	
	add.xyz		local,				scalednormal,			\pos
	TransformVertex	clipspace,	mLocalToClip,			local
	div			Q,						vf00[w],					clipspace[w]
	mul.xyz		clipspace,			clipspace,				Q
	TransformVertex screenSpace,	toscreen,				clipspace
	ftoi4.xyz	screenSpace,		screenSpace
	zero.w		screenSpace
	KickGIFRegPacked	0x4,			screenSpace
skipit\@:	
.endm

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; SwapKickBuffer exchanges the buffer pointers and inits the buffer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro SWAPKICKBUFFER which, withwhich
local temp
	imove			temp,					mKickBuffer[\which]
	imove			mKickBuffer[\which],	mKickBuffer[\withwhich]
	imove			mKickBuffer[\withwhich],	temp
	imove			mKickBufferPtr,	mKickBuffer[\which]

	;; swap normal buffers
	ilw.x			mNormalBufferPtr,	WS_NORMAL_BUFFERS(vi00)		:	WS_NORMAL_BUFFERS
	ilw.y			mOtherNormalBuffer, WS_NORMAL_BUFFERS(vi00)	:	WS_NORMAL_BUFFERS
	isw.x			mOtherNormalBuffer, WS_NORMAL_BUFFERS(vi00)	:	WS_NORMAL_BUFFERS
	isw.y			mNormalBufferPtr, WS_NORMAL_BUFFERS(vi00)		:	WS_NORMAL_BUFFERS
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; do a nop kick
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro NOPKICK
	iliu			noptag,				WS_NOPTAG
	xgkick		noptag
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; get render infor for a specific layer
;;; input: layer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro GET_LAYER_STUFF doload=1
	
.aif \doload eq 1
	;; we need layer6 = layer*6
	iadd			layer2,				layer,					layer
	iadd			layer4,				layer2,					layer2
	iadd			layer6,				layer4,					layer2
	
	;;	load the layer.. (at least what we care of it)
	lq				layer[0],			LAYERS+0(layer6)
;	lq				layer[1],			LAYERS+1(layer6)
;	lq				layer[2],			LAYERS+2(layer6)
	lq				layer[3],			LAYERS+3(layer6)
	lq				layer[4],			LAYERS+4(layer6)
.aendi
	lq				layer[5],			LAYERS+5(layer6)
	
	;; generate the texture matrix we want...
	zero.yzw		mTexMatrix[0]
	zero.xyzw	mTexMatrix[1]
	zero.xzw		mTexMatrix[2]
	move.x		mTexMatrix[0],		layer[0]
	move.y		mTexMatrix[2],		layer[0]
	lq.w			mT,					MT(vi00)
	mr32.xyzw	rotlayer,			layer[0]
	mr32.xyzw	rotlayer,			rotlayer
	mul.xy		mTexMatrix[3],		rotlayer,				mT[w]
	zero.z		mTexMatrix[3]
	one.w			mTexMatrix[3]

	;; wrap the translation so that its 0-1...
	;; this is icky.. wish there was a better wya, but there are none that I know of..
	ftoi15.xy	mTexMatrix[3],		mTexMatrix[3]
	mtir			clampme,				mTexMatrix[3][x]
	mfir.x		mTexMatrix[3],		clampme
	mtir			clampme,				mTexMatrix[3][y]
	mfir.y		mTexMatrix[3],		clampme
	itof15.xy	mTexMatrix[3],		mTexMatrix[3]
	
	;; get flags
	mtir			layerFlags,			layer[5][x]
	isw.y			layerFlags,			WS_LAYERFLAGS(vi00)
	
	;; get the vertex adjustment parameter
	mr32.yz		mVtxAdj,				layer[3]
	mr32.xy		mVtxAdj,				mVtxAdj
	add.z			mVtxAdj,				vf00,						layer[4][x]
	
	;; get the normal adjustment parameter
	mr32.xyz		mNormAdj,			layer[4]
;	mul.xz		mNormAdj,			mNormAdj,				mScale

.endm	
	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Set the GS to render with a specific layer...
;;; input: layer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro BUILD_SETLAYER whichbuffer
	;; get ready to write to an output buffer
	iliu			regcount,			SET_LAYER_REGS
	imove			mKickBufferPtr,	mKickBuffer[\whichbuffer]
	INIT_CURR_KICK_BUFFER			MISCTAG,					regcount,	0

	;; we need layer6 = layer*6
	iadd			layer2,				layer,					layer
	iadd			layer4,				layer2,					layer2
	iadd			layer6,				layer4,					layer2
	
	;;	load the layer.. (at least what we care of it)
	lq				layer[0],			LAYERS+0(layer6)
	lq				layer[1],			LAYERS+1(layer6)
	lq				layer[2],			LAYERS+2(layer6)
	lq				layer[3],			LAYERS+3(layer6)
	lq				layer[4],			LAYERS+4(layer6)

	;; arrange stuff into register like we want them in memory...
	mr32			miptbp2,				layer[1]
	mr32			miptbp2,				miptbp2
	move			miptbp1,				layer[1]
	mr32			tex1,					layer[2]
	mr32			tex1,					tex1
	move			tex0,					layer[2]
	move			clamp,				layer[3]
	mr32			alpha1,				layer[0]
	mr32			alpha1,				alpha1

	;; we still need the register #s...
	iliu			regnum,				0x34
	mfir.z		miptbp1,				regnum
	iliu			regnum,				0x36
	mfir.z		miptbp2,				regnum
	iliu			regnum,				0x6
	mfir.z		tex0,					regnum
	iliu			regnum,				0x14
	mfir.z		tex1,					regnum
	iliu			regnum,				0x8
	mfir.z		clamp,				regnum
	iliu			regnum,				0x42
	mfir.z		alpha1,				regnum

	;; now we can store them in order
	sqi.xyz		miptbp1,				(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		miptbp2,				(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		tex0,					(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		tex1,					(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		clamp,				(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		alpha1,				(mKickBufferPtr++)	:KICKBUFFER

;test: [D]
	;; generate alpha scale value
	mtir			temp,					alpha1[y]
	iandiu		temp,					temp,						0xFF
	mfir.y		alphascale,			temp
	itof0.y		alphascale,			alphascale
	loi			0x3b808081 ; 1/255
	mul.y			alphascale,			alphascale,				I
	--barrier
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Set the GS to render with a specific sprite layer...
;;; input: sprite
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro GET_SPRITE_INFO
	;; we need sprite7 = sprite*7
	iadd			sprite2,				sprite,					sprite
	iadd			sprite4,				sprite2,					sprite2
	iadd			sprite6,				sprite4,					sprite2
	iadd			sprite7,				sprite6,					sprite
	
	;;	load the sprite..
	LoadMatrix	mTexMatrix,			SPRITES,					sprite7
	lq				sprite[4],			SPRITES+4(sprite7)
	lq				sprite[5],			SPRITES+5(sprite7)
	lq				sprite[6],			SPRITES+6(sprite7)

	;; layer flags...
	iliu			layerflags,			0
	isw.y			layerFlags,			WS_LAYERFLAGS(vi00)
.endm
		
.macro BUILD_SETSPRITE whichbuffer
	;; get ready to write to an output buffer
	iliu			regcount,			SET_LAYER_REGS
	imove			mKickBufferPtr,	mKickBuffer[\whichbuffer]
	INIT_CURR_KICK_BUFFER			MISCTAG,					regcount, 0

	;; we need sprite7 = sprite*7
	iadd			sprite2,				sprite,					sprite
	iadd			sprite4,				sprite2,					sprite2
	iadd			sprite6,				sprite4,					sprite2
	iadd			sprite7,				sprite6,					sprite
	
	;;	load the sprite..
	LoadMatrix	mTexMatrix,			SPRITES,					sprite7
	lq				sprite[4],			SPRITES+4(sprite7)
	lq				sprite[5],			SPRITES+5(sprite7)
	lq				sprite[6],			SPRITES+6(sprite7)

	;; arrange stuff into register like we want them in memory...
	mr32			miptbp2,				sprite[4]
	mr32			miptbp2,				miptbp2
	move			miptbp1,				sprite[4]
	mr32			tex1,					sprite[5]
	mr32			tex1,					tex1
	move			tex0,					sprite[5]
	move			clamp,				sprite[6]
	mr32			alpha1,				sprite[6]
	mr32			alpha1,				alpha1

	;; we still need the register #s...
	iliu			regnum,				0x34
	mfir.z		miptbp1,				regnum
	iliu			regnum,				0x36
	mfir.z		miptbp2,				regnum
	iliu			regnum,				0x6
	mfir.z		tex0,					regnum
	iliu			regnum,				0x14
	mfir.z		tex1,					regnum
	iliu			regnum,				0x8
	mfir.z		clamp,				regnum
	iliu			regnum,				0x42
	mfir.z		alpha1,				regnum

	;; now we can store them in order
	sqi.xyz		miptbp1,				(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		miptbp2,				(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		tex0,					(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		tex1,					(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		clamp,				(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		alpha1,				(mKickBufferPtr++)	:KICKBUFFER
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; given a dampenval, overwrite dest.w with the correct alpha value for this layer
;;; only overwrites if alpha != 0x80 should be used...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro GENERATE_ALPHA_FADE_RGBA	dest,	dampenval
local layerFlags
local dv
	;; color is constant... lets generate it..
	iliu			temp,					0x80
	mfir			\dest,				temp
	loi			128.0
	mul.w			\dest,				vf00,						I

	;; optionally alpha fade edges
	ilw.y			layerFlags,			WS_LAYERFLAGS(vi00)
	ibafc			layerFlags,			LAYERFLG_FADE_ALL,	RF_FadeDone\@
	ibafs			layerFlags,			LAYERFLG_FADE_CENTER,	RF_FadeCenter\@

RF_FadeEdges\@:		
	mul.w			\dest,				\dampenval,				I
	b				RF_FadeDone\@

RF_FadeCenter\@:

	sub.w			dv,					vf00,						\dampenval
	mul.w			\dest,				dv,						I
		
RF_FadeDone\@:	

	;; apply layer fading
	;; I know that the itof0 could be eliminated by changing the initial
	;; generation of the color to be all floats, but Im lazy and sick
	;; of looking at water code
	mul.w			\dest,				\dest,					alphascale[y]
	ftoi0.w		\dest,				\dest

.endm	
	

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Main entry is here....
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.name Water
--enter
--endenter

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; init stuff
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	fcset			0x000000
	
	LoadVector4	SCALE,				vi00,						mScale
	LoadMatrix	mLocalToClip,		LOCAL_TO_CLIP

	iliu			Stack,				TOP_OF_STACK
	One			ones

	loi			615651.564651651
	add			rinitval,			vf00,						I
	rinit			R,						rinitval[x]
	div			Q,						vf00[w],					mScale[x]
	addq.x		mInvScale,			vf00,						Q
	div			Q,						vf00[w],					mScale[z]
	addq.z		mInvScale,			vf00,						Q

	;; init the workspace
	iliu			noptag,				0x7fff
	iaddiu		noptag,				noptag,					1
	SaveIntVector4 WORKSPACE, vi00, noptag, vi00, vi00, vi00
	
	iliu			buff0,				NORMAL_1
	iliu			buff1,				NORMAL_2
	mfir.x		normbuff,			buff0
	mfir.y		normbuff,			buff1
	sq.xy			normbuff,			WS_NORMAL_BUFFERS(vi00)		:	WS_NORMAL_BUFFERS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; generate signs table
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	NegOne		negones
	Zero			zeros

	sq.y			zeros,				SIGNS+0(vi00)
	sq.xzw		negones,				SIGNS+0(vi00)

	sq.x			ones,					SIGNS+1(vi00)
	sq.y			zeros,				SIGNS+1(vi00)
	sq.zw			negones,				SIGNS+1(vi00)

	sq.z			ones,					SIGNS+2(vi00)
	sq.y			zeros,				SIGNS+2(vi00)
	sq.xw			negones,				SIGNS+2(vi00)

	sq.xz			ones,					SIGNS+3(vi00)
	sq.y			zeros,				SIGNS+3(vi00)
	sq.w			negones,				SIGNS+3(vi00)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; transform affectors to local space
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; get # of affectors
	lq.w			acount,				AFFECTORCOUNT(vi00)	:AFFECTORS
	mtir			affcount,			acount[w]
	iliu			bytemask,			0xFF
	iand			affcount,			affcount,				bytemask
	
	;; easy out if no affectors
	ibeqz			affcount,			ToLocal_Done

	;; get world to local matrix
	LoadMatrix	mWorldToLocal,		WORLD_TO_LOCAL

	;; affector loop
ToLocal_NextAffector:
	--LoopCS		1,						0
	;; laod the affector
	idecr			affcount
	lq				affector,			AFFECTORS(affcount)	:AFFECTORS

	;; grab world coords
	zero.yw		world
	move.xz		world,				affector

	;; put into local space
	TransformVertex3	local,		mWorldToLocal,			world

	;; preserve y & w
	move.yw		local,				affector

	;; save it out..
	sq				local,				AFFECTORS(affcount)	:AFFECTORS
	
	;; next?
	ibgtz			affcount,			ToLocal_NextAffector	

ToLocal_Done:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; set up the output pointers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	iliu			mKickBuffer[0],	OUTPUT_1
	iliu			mKickBuffer[1],	OUTPUT_2
	iliu			mKickBuffer[2],	OUTPUT_3


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; prep to render water...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	loi			0.5
	One			cell
	Zero.y		cell
	mul			cell,					cell,						I
	iliu			depth,				0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; this is the recursive algorithm to render the water...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
NextCell:
	;; handle depth of recursion
	iincr			depth

	;; generate halfsize vector
	loi			0.5
	mul			halfsize,			ones,						cell[w]
	mul			halfsize,			halfsize,				I

	;; for each quadrant
	iliu			quad,					0
	
NextQuadrant:	
	;; generate newcell vector
	lq				signs,				SIGNS(quad)
	mul			temp,					halfsize,				signs
	add			newcell,				temp,						cell

	;; determine if another depth of recursion is nessisary
	;; or if we are to draw this quadrant...
	iincr			depth
	bal			retaddr,				DoesAffectorRequestSubdivision
	idecr			depth
	ilw.z			outOfView,			WS_DARS_OUT_OF_VIEW(vi00)
	ibeqiu		outOfView,			1,							DrawComplete
	ibeqz			recurse,				DrawCell

	;; ok, subdivision was requested, so we recurse...

	;; save state
	PushIntVector4	quad
	PushVector4	cell
	PushVector4 halfsize

	;; do the recursion
	move			cell,					newcell
	b				NextCell
	
	;; here is the out point for recursion
EndRecursion:
	
	;; restore state
	PopVector4	halfsize
	PopVector4	cell
	PopIntVector4	quad

DrawComplete:	
		
	iincr			quad
	ibneiu		quad,					4,							NextQuadrant
		
	;; return from recursion
	idecr			depth
	ibgtz			depth,				EndRecursion

	;; have to handle multipass on the last fan, if there was one...
	ilw.w			mKickBuffer[1],	WS_ACTIVEBUFFERS(vi00)
	ibeqz			mKickBuffer[1],	NoFinalFan
	NOPKICK
	iliu			buffcount,			1
	bal			retaddr,				RenderMultiPass
NoFinalFan:	

	;; done... whew...
--exit
--endexit

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; this is another recursive function to get edge verts for a particular cell..
;;; inputs are gevcell, depth, quad1, quad2, gevretidx
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GenerateEdgeVerts:
	;; track depth of recursion
	iincr			depth
		
	bogt.xz		gevcell,				vf00[w],					GEV_AddSimple
	bolt.xz		gevcell,				vf00[x],					GEV_AddSimple

	;; prep for call
	move			gcell,				newcell
	move			newcell,				gevcell

	;; call
	bal			retaddr,				DoesAffectorRequestSubdivision

	;; cleanup after call
	move			newcell,				gcell

	ibeqz			recurse,				GEV_AddSimple

	;; ok, now the nasty case... we need to recurse... save state
	PushVector4	gevcell
	PushIntVector4	gevretidx

	;; generate halfsize vector
	loi			0.5
	mul			gevhalfsize,		ones,						gevcell[w]
	mul			gevhalfsize,		gevhalfsize,			I

	;; generate the new cell for quad1
	lq				signs,				SIGNS(quad1)
	mul			temp,					gevhalfsize,			signs
	add			gevcell,				temp,						gevcell
	
	;; recurse 1
	iliu			gevretidx,			1
	b				GenerateEdgeVerts
	
GEVReturn1:
	PopIntVector4	gevretidx
	PopVector4	gevcell
	;; push them back on...
	idecr			Stack,				2
	
	;; generate halfsize vector
	loi			0.5
	mul			gevhalfsize,		ones,						gevcell[w]
	mul			gevhalfsize,		gevhalfsize,			I

	;; generate the new cell for quad2
	lq				signs,				SIGNS(quad2)
	mul			temp,					gevhalfsize,			signs
	add			gevcell,				temp,						gevcell

	;; recurse 2
	iliu			gevretidx,			2
	b				GenerateEdgeVerts
GEVReturn2:
	
	;; restore state
	PopIntVector4 gevretidx
	PopVector4	gevcell

	b				GEV_Done

GEVReturn0:
	jr				gevretaddr
	
GEV_AddSimple:	
	lq				signs,				SIGNS(quad2)
	mul			temp,					signs,					gevcell[w]
	add			vtx,					temp,						gevcell
	PushIntVector4	depth,	gevretidx,	quad1,	quad2
	PushIntVector4	quad
	PushVector4	gevcell
	PushVector4	halfsize
	bal			retaddr,				AddVertex
	PopVector4	halfsize
	PopVector4	gevcell
	PopIntVector4	quad
	PopIntVector4	depth,	gevretidx,	quad1,	quad2

GEV_Done:	
	idecr			depth
	ibeqz			gevretidx,			GEVReturn0
	idecr			gevretidx
	ibeqz			gevretidx,			GEVReturn1
	b				GEVReturn2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; determine if any affector requests the current cell to subdivide
;;; output:	 recurse
;;; input: newcell, depth
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DoesAffectorRequestSubdivision:
	;; get min & max extents of cell
	sub			minpt,				newcell,					newcell[w]
	add			maxpt,				newcell,					newcell[w]

	;; transform into clip space..
.aif 0 eq 1
	zero.y		minpt
	zero.y		maxpt
	TransformVertex clipminpt,		mLocalToClip,			minpt
	TransformVertex clipmaxpt,		mLocalToClip,			maxpt

	;; we adjust the clip points to help reduce the chance of us throwing out
	;; data that will be good after GetY adjusts the Y position of the vertex.
	;; this is just a fudge, and if people complain, Ill shoot them.
	loi			0.88
	mul.xy		clipminpt,			clipminpt,					I
	mul.xy		clipmaxpt,			clipmaxpt,					I

	;; we also need the other 2 corners
	move.xy		minmaxpt,			minpt
	move.z		minmaxpt,			maxpt
	move.xy		maxminpt,			maxpt
	move.z		maxminpt,			minpt
	TransformVertex clipminmaxpt,	mLocalToClip,			minmaxpt
	TransformVertex clipmaxminpt,	mLocalToClip,			maxminpt
	
	clipw			clipminpt,			clipminpt
	clipw			clipmaxpt,			clipmaxpt
	clipw			clipminmaxpt,		clipminmaxpt
	clipw			clipmaxminpt,		clipmaxminpt

	;; this value is used if we touch +/- z... it forces scissoring...
	iliu			outOfView,			2
	isw.z			outOfView,			WS_DARS_OUT_OF_VIEW(vi00)
	
	;; if any one vertex leaves -/+ z, we need to skip the rest of these checks,
	;; or we might reject valid verts...
	fcand			vi01,					0xb30b30	; B'1100.00 11.0000 .1100.00 11.0000
	ibnez			vi01,					DARS_DontSubdivideBecauseOfMinusZ

	;; if the cell is entirely off the screen, we dont draw it at all
	iliu			outOfView			1
	isw.z			outOfView,			WS_DARS_OUT_OF_VIEW(vi00)

	;; if all verts fail -x test, its all off screen...
	fcor			vi01,					0xf7df7d	; B'1111.01 11.1101 .1111.01 11.1101
	ibnez			vi01,					DARS_Negative
	;; if all verts fail +x test, its all off screen...
	fcor			vi01,					0xfbefbe	; B'1111.10 11.1110 .1111.10 11.1110
	ibnez			vi01,					DARS_Negative
	;; if all verts fail -y test, its all off screen...
	fcor			vi01,					0xdf7df7	; B'1101.11 11.0111 .1101.11 11.0111
	ibnez			vi01,					DARS_Negative
	;; if all verts fail +y test, its all off screen...
	fcor			vi01,					0xefbefb	; B'1110.11 11.1011 .1110.11 11.1011
	ibnez			vi01,					DARS_Negative

	;; ok, normal drawing...
	iliu			outOfView,			0
	isw.z			outOfView,			WS_DARS_OUT_OF_VIEW(vi00)
	
	;; we assume that the safe zone is 4 times the size of the view zone
	;; .25 / .88 = 0.284
	loi			0.284
	mul.xy		clipminpt,			clipminpt,				I
	mul.xy		clipmaxpt,			clipmaxpt,				I
	mul.xy		clipminmaxpt,		clipminmaxpt,			I
	mul.xy		clipmaxminpt,		clipmaxminpt,			I

	;; generate safe zone clip flags...
	clipw			clipminpt,			clipminpt
	clipw			clipmaxpt,			clipmaxpt
	clipw			clipminmaxpt,		clipminmaxpt
	clipw			clipmaxminpt,		clipmaxminpt
	
	;; if any part of cell crosses safe zone, force a subdivide
	fcand			vi01,					0x3CF3CF	; B'0011.11 00.1111 .0011.11 00.1111
	ibnez			vi01,					DARS_Positive

DARS_DontSubdivideBecauseOfMinusZ:
.aendi
	
	;; simple cases
	ilw.w			maxSubDivDepth,	SUBDIV_DEPTH(vi00)
	iliu			bytemask,			0xFF
	iand			maxSubDivDepth,	maxSubDivDepth,		bytemask
	ibeq			depth,				maxSubDivDepth,		DARS_Negative
	ilw.z			minSubDivDepth,	SUBDIV_DEPTH(vi00)
	iand			minSubDivDepth,	minSubDivDepth,		bytemask
	iblt			depth,				minSubDivDepth,		DARS_Positive
	
	;; get # of affectors
	lq.w			acount,				AFFECTORCOUNT(vi00)	:AFFECTORS
	mtir			affcount,			acount[w]
	iand			affcount,			affcount,				bytemask
	
	;; easy out if no affectors
	ibeqz			affcount,			DARS_Negative

	;; convert from local space to meters...
	mul.xz		minpt,				minpt,					mScale
	mul.xz		maxpt,				maxpt,					mScale

	PushIntVector4	mKickBuffer[0], mKickBuffer[1], mKickBuffer[2]
	
	;; affector loop
DARS_NextAffector:
	;; laod the affector
	idecr			affcount
	lq				affector,			AFFECTORS(affcount)	:AFFECTORS

	;; easy out if we are too deep for the affector
	mtir			dmax,					affector[y]
	iand			dmax,					dmax,						bytemask
	iblt			dmax,					depth,					DARS_DoneWithThisOne

	;; ok, heres the basic algorithm.  First, determine what radius
	;; between min & max matches the depth we are querying.
	;; then find out if the point is within that radius..

	;; so, min is 0 so the equation for how far between 0 and dmax we are
	;; is ds = (dmax - d) / dmax
	;; we leave ds in Q for now..
	isub			depthdiff,			dmax,						depth
	mfir.x		fdepthdiff,			depthdiff
	mfir.x		fdmax,				dmax
	itof0.x		fdepthdiff,			fdepthdiff
	itof0.x		fdmax,				fdmax
	div			Q,						fdepthdiff[x],			fdmax[x]

	;; to better match the generic code, I double the min radius, and
	;; triple the max radius
	loi			1.0
	mul.y			affector,			affector,				I
	loi			3.0
	mul.w			affector,			affector,				I

	;; ok, now we need the radius we are to use.. its
	;; Ar = (Rmax - Rmin) * ds + Rmin
	sub.w			Ar,					affector,				affector[y]
	mul.w			ACC,					vf00,						affector[y]
	madd.w		Ar,					Ar,						Q

	;; now we need to know if the cell crosses that radius...
	;; now we have 2 corners of the cell minpt & maxpt.  we want to generate
	;; a circle around this for our test.
	;; Pc = (minpt + maxpt) * 0.5
	;; Pr = |maxpt - minpt| * 0.5
	;; so we need the offset of this circle from the affector
	;; O = Ac - Pc
	;; the final check then is
	;; |O| <= Ar + Pr
	;; since we have sqrts all over this mess, lets simplify it a little
	;; |O|**2 <= (Ar + Pr)(Ar + Pr)
	;; |O|**2 <= (Ar**2 + 2*Ar*Pr + Pr**2)
	;; now I only need the sqrt for Pr instead of both Pr & O...

	;; Pc = (minpt + maxpt) * 0.5
	add			Pc,					minpt,					maxpt
	loi			0.5
	mul			Pc,					Pc,						I
	
	;; Pr = |maxpt - minpt| * 0.5
	sub			Pr,					maxpt,					minpt
	
	LengthVector3	Q,					Pr
	add.x			Pr,					vf00,						Q
	mul.x			Pr,					Pr,						I

	;; O = Ac - Pc
	mul.xz		affector,			affector,				mScale
	sub			O,						affector,				Pc

	;; R**2
	mul.w			Arsq,					Ar,						Ar

	;; 2*R*Pr
	loi			2.0
	mul.x			twoArPr,				Pr,						Ar[w]
	mul.x			ACC,					twoArPr,					I

	;; + Pr**2
	madd.x		ACC,					Pr,						Pr

	;; (R**2 + 2*R*Pr + Pr**2)
	madd.x		sum,					ones,						Arsq[w]

	;; now we need |O|**2
	mul.xz		Osq,					O,							O
	add.x			Olensq,				Osq,						Osq[z]

	;; now is it further?
	bagt.x		Olensq,				sum,						DARS_DoneWithThisOne
	
	;; we hit one.. no reason to continue...
	PopIntVector4	mKickBuffer[0], mKickBuffer[1], mKickBuffer[2]
DARS_Positive:	
	iliu			recurse,				1
	jr				retaddr
	
DARS_DoneWithThisOne:	
	ibgtz			affcount,			DARS_NextAffector

DARS_Done:
	;; one was not hit
	PopIntVector4	mKickBuffer[0], mKickBuffer[1], mKickBuffer[2]
DARS_Negative:	
	iliu			recurse,				0
	jr				retaddr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Draw a single cell
;;; inputs (newcell, depth)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DrawCell:
	iliu			layer,				0
	GET_LAYER_STUFF

	;; build the cell into buffer 0 always
	imove			mKickBufferPtr,	mKickBuffer[0]
	
	;; add the center of the cell as a vertex
	move			vtx,					newcell

	;; track which planes we clip...
	iliu			totalclipflags,	0x00
	isw.z			totalclipflags,	WS_CLIPFLAGS(vi00)
		
	PushIntVector4	depth,	quad
	bal			retaddr,				AddVertex
	PopIntVector4	depth,	quad
	
	;; find twice the width of the cell..
	add			twice,				newcell,					newcell
	
	move			gevcell,				newcell
	sub.z			gevcell,				gevcell,					twice[w]
	iliu			quad1,				Q_DL
	iliu			quad2,				Q_DR
	iliu			gevretidx,			0
	bal			gevretaddr,			GenerateEdgeVerts

	move			gevcell,				newcell
	add.x			gevcell,				gevcell,					twice[w]
	iliu			quad1,				Q_UL
	iliu			quad2,				Q_DL
	iliu			gevretidx,			0
	bal			gevretaddr,			GenerateEdgeVerts
	
	move			gevcell,				newcell
	add.z			gevcell,				gevcell,					twice[w]
	iliu			quad1,				Q_UR
	iliu			quad2,				Q_UL
	iliu			gevretidx,			0
	bal			gevretaddr,			GenerateEdgeVerts

	move			gevcell,				newcell
	sub.x			gevcell,				gevcell,					twice[w]
	iliu			quad1,				Q_DR
	iliu			quad2,				Q_UR
	iliu			gevretidx,			0
	bal			gevretaddr,			GenerateEdgeVerts

	bal			retaddr,				RenderFan
	
	b				DrawComplete


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; spin through an output buffer, regenerate the original coords so that we can generate
;;; new texture coords using a decal scheme.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
.macro RMP_UPDATEDECAL_VERTS_LOOP	overwritecolor, applynormadj, usezasy, addoffset
	;; we need to generate the cliptolocal matrix
	LoadMatrix	mClipToWorld,		CLIP_TO_WORLD
	LoadMatrix	mWorldToLocal,		WORLD_TO_LOCAL
	MultiplyMatrix	mClipToLocal,	mClipToWorld,			mWorldToLocal
	
.aif \addoffset eq 1
	lq				miscqw24,			OFFSET_XZ(vi00)
.aendi	
	
RMP_DecalVerts_Loop\@:
	--LoopCS		3,						0	
	;; load the screen space vertex.  we will go back to local space
	lq				screenSpace,		2(mKickBufferPtr)		:KICKBUFFER
	itof4			screenSpace,		screenSpace

	;; load the Q value so that we can deal with it.
	lq				oldtex,				0(mKickBufferPtr)		:KICKBUFFER

	;; recip the Q value to get Z
	div			Q,						vf00[w],					oldtex[z]

	;; transform the vertex into clip space
	TransformVertex	clipSpace,	mScreenToClip,			screenSpace
	
	;; mul it through the vertex
	mul.xyzw		clipSpace,			clipSpace,				Q

	;; now transform it back to local space
	TransformVertexF	localSpace,	mClipToLocal,			clipSpace

.aif \addoffset eq 1
	add.xz		localSpace,			localSpace,				miscqw24
.aendi
		
.aif usezasy eq 1
	mr32.y		localSpace,			localSpace
	move.z		localSpace,			vf00
.aendi

	;; now transform it into texture space...
	TransformVertex3	textureSpace,	mTexMatrix,			localSpace

	lqi			normal,				(mNormalBufferPtr++)	:NORMALBUFFER

.aif applynormadj eq 1
	
	;; scale normal based on normal adj for this layer...
	mul.xyz		normal,				normal,					mNormAdj

	;; renomalize the normal
	NormalizeVector3	normal,		normal

	;; add the normal to the st coord...
	add.x			textureSpace,		textureSpace,			normal
	add.y			textureSpace,		textureSpace,			normal[z]
.aendi
	
	;; perspective correct it...
	mul.xy		textureSpace,		textureSpace,			oldtex[z]

	;; store the coordinate out
	sq.xy			textureSpace,		0(mKickBufferPtr)		:KICKBUFFER

.aif \overwritecolor eq 1
	;; in this case, the color is compressed into the texture matrix
	;; this is used for sprites
	sq.xyzw		mTexMatrix[0],		1(mKickBufferPtr)		:KICKBUFFER
.aelse
	GENERATE_ALPHA_FADE_RGBA	rgba,	normal
	sq.w			rgba,					1(mKickBufferPtr)		:KICKBUFFER
.aendi
		
	;; next vertex
	iincr			mKickBufferPtr,	KICK_VTX_SIZE
	idecr			vtxCount
	ibnez			vtxCount,			RMP_DecalVerts_Loop\@
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; spin through an output buffer, regenerate the original coords so that we can generate
;;; new texture coords using an envmapping scheme.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RMP_EnvironmentVerts:

	;; load eyepoint
	lq.xzw		eye,					EYE(vi00)
	lq.w			eyepart2,			EYE_PT2(vi00)
	mul.y			eye,					ones,						eyepart2[w]

	;; clip to world
	LoadMatrix	mClipToWorld,		CLIP_TO_WORLD
	LoadMatrix	mWorldToLocal,		WORLD_TO_LOCAL
	MultiplyMatrix	mClipToLocal,	mClipToWorld,			mWorldToLocal
	
	;; color is constant... lets generate it..
	iliu			temp,					0x80
	mfir			rgba,					temp

RMP_EnvironmentVerts_Loop:
	--LoopCS		3,						0	
	;; load the Q value so that we can deal with it.
	lq				oldtex,				0(mKickBufferPtr)		:KICKBUFFER

	;; load normal
	lqi			normal,				(mNormalBufferPtr++)	:NORMALBUFFER

	;; scale normal based on normal adj for this layer...
	mul.xyz		normal,				normal,					mNormAdj

	;; renormalize the normal...
	NormalizeVector3	normal,		normal

	;; TEMP!:	dont use the real normal as this code is broken
;	zero.xz		normal
;	one.y			normal
	
	;; get vertex in world coords
	;; load the screen space vertex.  we will go back to world space
	lq				screenSpace,		2(mKickBufferPtr)		:KICKBUFFER
	itof4			screenSpace,		screenSpace

	;; recip the Q value to get Z
	div			Q,						vf00[w],					oldtex[z]

	;; transform the vertex into clip space
	TransformVertex	clipSpace,	mScreenToClip,			screenSpace
	
	;; mul it through the vertex
	mul.xyzw		clipSpace,			clipSpace,				Q

	;; now transform it back to local space
	TransformVertexF	localSpace,	mClipToLocal,			clipSpace

	;; Vector3 toEye = mEyePoint - wvtx;
	;; toEye.Normalize()
	mul.xz		localSpace,			localSpace,				mScale
	sub			toEye,				eye,						localSpace
	NormalizeVector3	toEye,		toEye
	
	;; Vector3 refl = normal * (2.0f * Vector3::Dot(normal, toEye)) - toEye;
	mul.xyz		dotA,					normal,					toEye
	eleng			P,						dotA
	add.w			two,					vf00,						vf00
	mfp.w			dot,					P
	mul.w			twice,				two,						dot[w]
	mul.xyz		normalscaled,		normal,					twice[w]
	sub.xyz		refl,					normalscaled,			toEye
;	mr32.y		refl,					refl
	zero.y		refl
	one.w			refl
	
	;; now transform it into texture space...
	TransformVertex3	textureSpace,	mTexMatrix,			refl

	;; perspective correct it...
	mul.xy		textureSpace,		textureSpace,			oldtex[z]

;	GENERATE_ALPHA_FADE_RGBA	rgba,	normal
;	sq.w			rgba,					1(mKickBufferPtr)

	;; store the coordinate out
	sq.xy			textureSpace,		0(mKickBufferPtr)		:KICKBUFFER
	
	;; next vertex
	iincr			mKickBufferPtr,	KICK_VTX_SIZE
	idecr			vtxCount
	ibnez			vtxCount,			RMP_EnvironmentVerts_Loop
	b				RMP_DoneWithVerts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; spin through an output buffer, regenerate the original coords so that we can generate
;;; new texture coords using a specular mapping scheme..
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RMP_SpecularVerts:
	b				RMP_DoneWithVerts
	
RMP_DistortVerts:	
	;; load eyepoint
	lq.xzw		eye,					EYE(vi00)
	lq.w			eyepart2,			EYE_PT2(vi00)
	mul.y			eye,					ones,						eyepart2[w]

	;; clip to world
	LoadMatrix	mClipToWorld,		CLIP_TO_WORLD
	LoadMatrix	mWorldToLocal,		WORLD_TO_LOCAL
	MultiplyMatrix	mClipToLocal,	mClipToWorld,			mWorldToLocal
	
	;; color is constant... lets generate it..
	iliu			temp,					0x80
	mfir			rgba,					temp

RMP_DistortVerts_Loop:		
	--LoopCS		3,						0	
	;; load the Q value so that we can deal with it.
	lq				oldtex,				0(mKickBufferPtr)		:KICKBUFFER

	;; load normal
	lqi			normal,				(mNormalBufferPtr++)	:NORMALBUFFER

	;; scale normal based on normal adj for this layer...
	mul.xyz		normal,				normal,					mNormAdj

	;; renormalize the normal...
	NormalizeVector3	normal,		normal

	;; get vertex in world coords
	;; load the screen space vertex.  we will go back to world space
	lq				screenSpace,		2(mKickBufferPtr)		:KICKBUFFER
	itof4			screenSpace,		screenSpace

	;; recip the Q value to get Z
	div			Q,						vf00[w],					oldtex[z]

	;; transform the vertex into clip space
	TransformVertex	clipSpace,	mScreenToClip,			screenSpace
	
	;; mul it through the vertex
	mul.xyzw		clipSpace,			clipSpace,				Q

	;; now transform it back to local space
	TransformVertexF	localSpace,	mClipToLocal,			clipSpace

	;; distort the vertex
	add.xz		localSpace,			localSpace,				normal

	;; and go back to clip space...
	TransformVertexF	clipSpace,	mLocalToClip,			localSpace

	;; complete the -> canonicalSpace
	div			Q,						vf00[w],					clipSpace[w]
	mul.xyzw		canonicalSpace,	clipSpace,				Q

	;; change from -1 to 1 into 0 to 1 space...
	loi 0.5
	mul.xy		textureSpace,		clipSpace,				I
	add.xy		textureSpace,		textureSpace,			I

	;; zero out the z value...
	zero.z		textureSpace

	;; now transform it into texture space...
	TransformVertex3	textureSpace,	mTexMatrix,			textureSpace

	;; perspective correct it...
	mul.xy		textureSpace,		textureSpace,			oldtex[z]

	;; store the coordinate out
	sq.xy			textureSpace,		0(mKickBufferPtr)		:KICKBUFFER
	
	;; next vertex
	iincr			mKickBufferPtr,	KICK_VTX_SIZE
	idecr			vtxCount
	ibnez			vtxCount,			RMP_DistortVerts_Loop
	b				RMP_DoneWithVerts
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Adds a vertex to the current fan...
;;; Inputs:	vtx
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
AddVertex:
	;; get y position of vertex...
	One.w			vtx
	b				GetY
ReturnFromGetY:

.aif WATER_DEBUG eq 1
	ilw.x			flags0,				FLAGS0(vi00)
	ibafc			flags0,				FLG0_RENDER_NORMALS,	DontRenderNormals
	RenderNormal normal, vtx
DontRenderNormals:
.aendi
	
	;; put into world scale
	move			worldvtx,			vtx

	;; generate clip vtx
	TransformVertex	screenvtx,	mLocalToClip,			worldvtx

	;; store the results of the clip operation out for this vertex...
	;; scale to make up for safe zone..
	loi			0.28
	mul.xy		testvtx,				screenvtx,				I
	move.z		testvtx,				screenvtx
	
	clipw			testvtx,				screenvtx
	ilw.z			totalclipflags,	WS_CLIPFLAGS(vi00)
	fcget			clipflags
	ior			totalclipflags,	totalclipflags,		clipflags
	isw.z			totalclipflags,	WS_CLIPFLAGS(vi00)

	sqi			screenvtx,			(mKickBufferPtr++)	:KICKBUFFER
	sqi.xyz		vtx,					(mKickBufferPtr++)	:KICKBUFFER
	sqi			normal,				(mKickBufferPtr++)	:KICKBUFFER
	
	;; return
	jr				retaddr
	


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Get the Y component for a vertex
;;; input: vtx.xz
;;; output:	 vtx.y, normal.xyz, normal.w=dampen value 0-1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GetY:

	zero.y		vtx
	zero.xz		normal
	one.y			normal

	;; build offset xz
	lq				miscqw24,			OFFSET_XZ(vi00)
	add.xz		offsetvtx,			vtx,						miscqw24
;	add.xz		offsetvtx,			vtx,						vf00

	;; we use this quite a bit
	iliu			bytemask,			0xFF
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; get ring height/normal...
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	;; loop across all rings...
	ilw.w			counter,				RINGCOUNT(vi00)
	iand			counter,				counter,					bytemask
	zero			ringnormal

	ibeqz			counter,				GY_SkipRings

	;; load general ring params...
	;; 1x = growrate
	;; 1y = wavestartpos
	;; 1z = frequency
	;; 1w = 1/wavecount
	;; 2x = wavephaseshift
	;; 2y = waveheight
	;; 2z = attack time
	;; 2w = 1.0f / attack time
	;; 3x = attack+sustain time
	;; 3y = 1.0f / release time
	lq				ringParams1,		RING_PARAMS1(vi00)
	lq				ringParams2,		RING_PARAMS2(vi00)
	lq				ringParams3,		RING_PARAMS3(vi00)
	lq.w			mT,					MT(vi00)

	;; need the sign mask for w
	iliu			wsignmask,			0x10
			
GY_RING_LOOP:
	;; turn on the optimizer
	--LoopCS		1,	0

	;; loop header
	idecr			counter
	lq				ring,					RINGS(counter)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; determine age
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	sub.w			age,					mT,						ring[y]

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; determine scale on position
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	sub.xz		delta,				offsetvtx,				ring
	zero.y		delta

	;; generate distance
	eleng			P,						delta
	mfp.w			distance,			P

	;; generate angle
	mul.w			growth,				age,						ringParams1[x]
	sub.w			dbase,				distance,				growth
	add.w			dbase,				dbase,					ringParams1[y]
	mul.w			angle,				dbase,					ringParams1[z]

	;; we need a ramp value from 0 to 1 for some of the calcs below...
	mul.w			rampposinit,		angle,					ringParams1[w]
	
	;; if (rampposinit < 0 || rampposinit > 1.0), then no contribution...
	;; we do this by generating a 0 or 1 float for if we should contribute...
	;; this is also applied to the normal...

	;; first the < 0...  the mul above leaves us ready for this...
	fmand			wsignbit,			wsignmask
	mfir.w		wsign0,				wsignbit

	;; now the > 1...
	sub.w			vf00,					vf00,						rampposinit
	fmand			wsignbit,			wsignmask
	mfir.z		wsign0,				wsignbit

	;; now convert to float 0 or 1
	itof4.zw		wsign0,				wsign0

	;; reverse the condition on them... (now a 0 means they are out of bounds)
	one.zw		ones
	sub.zw		wsign0,				ones,						wsign0

	;; multiply them (if either is 0, the result is 0).. then we are done with the if
	;; except that below we use wsign0 to scale both the normal and the height...
	mul.w			wsign0,				wsign0,					wsign0[z]

	;; generate true ramppos (this converts 0-1 above into 0-1-0)
	loi			2.0
	mul.w			ramppos,				rampposinit,			I
	sub.w			ramppos,				ramppos,					vf00[w]
	abs.w			ramppos,				ramppos
	sub.w			ramppos,				vf00,						ramppos

	;; ok, now add phaseshift and do cos & sin (the sin is used to
	;; help with the normal calcs...)
	add.w			cosin,				angle,					ringParams2[x]
	mr32.z		cosin,				cosin
	zero.xy		cosin				  ; unfortunately, this is kind of a pain to eliminate...
	SCSC			cosout,				cosin

	;; scale by ramppos & height
	mul.w			posscale,			cosout,					ramppos
	mul.w			posscale,			posscale,				ringParams2[y]

	;; scaling posscale by wsign0 allows the if() above to function
	mul.w			posscale,			posscale,				wsign0[w]

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; determine scale on age...
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; in C, this is 3 comparisons.  in this case, we use the flags
	;; to generate multipliers for the 3 possible cases...
	;; the multipliers are all 0 except one which is 1.  these are
	;; the results are then all added in the accumulator to generate
	;; the final scale...
	;; the first is:
	
	;	// is attacking
	;	if(age < mRingAttack)
	;	{
	;		timescale = age / mRingAttack;
	;	}

	;; to do this, we do if(age - ringattack < 0)
	sub.w			vf00,					age,						ringParams2[z]
	fmand			wsignbit,			wsignmask

	;; now we have a 0 or 0x10 if we should use this method...
	;; we want a 0 or 1 float from this...
	mfir.x		wsign,				wsignbit
	itof4.x		wsign,				wsign

	;; ok, now do the math and multiply by this to 0 it out if we dont use it...
	;; we store the results of this in the acc for later adding the other 2 versions...
	mul.w			timescaleA,			age,						ringParams2[w]
	mul.w			ACC,					timescaleA,				wsign[x]

	
	;; the second is:
	
	;	// is sustaining...
	;	else if(age < attacksustain)
	;	{
	;		timescale = 1.0f;
	;	}

	;; to do this, we first want 1.0 - wsign from above to make up the else part...
	sub.w			wsign2,				vf00,						wsign[x]

	;; now we need the if part...  thats this (like above)
	sub.w			vf00,					age,						ringParams3[x]
	fmand			wsignbit,			wsignmask
	
	;; now 0 or 0x10 needs to become 0 or 1
	mfir.y		wsign3,				wsignbit
	itof4.y		wsign3,				wsign3

	;; mul with the other sign bit...
	;; this is our result since the math is just 1.0..
	mul.y			timescaleB,			wsign3,					wsign2[w]

	;; we still need to add it to the acc...
	madd.w		ACC,					vf00,						timescaleB[y]

	
	;; the third is:	

	;	// is releasing
	;	else
	;	{
	;		timescale = 1.0f - ((age - attacksustain) / mRingRelease);
	;	}

	;; again, we invert the case before it to determine the else..
	sub.w			wsign4,				vf00,						timescaleB[y]

	;; now the math
	sub.w			agediff,				age,						ringParams3[x]
	mul.w			agediff,				agediff,					ringParams3[y]
	sub.w			timescaleC,			vf00,						agediff

	;; now scale by the 0 or 1...  add it to the acc, and save it out as the scale...
	madd.w		timescale,			timescaleC,				wsign4

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; putting the vertex together...
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; once these are determined, they are multiplied together to generate 
	;; the final height for this vertex... (the wave height is already muled into
	;; posscale..
	mul.w			height,				timescale,				posscale

	;; now it needs to be added to the vertex y...
	add.y			vtx,					vtx,						height[w]

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; determine an approximate normal
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; in creating this normal, we generate a normal tilted away from or towards
	;; the center of the effect based on the results of the sine function...
	;; since the sine is the slope of the cosine, this should be simple...
	;; we just blend between a vertical normal and a horizontal normal based on
	;; the sine function..

	;; so first we need a vertical normal...
	mr32			vertnorm,			vf00
	mr32			vertnorm,			vertnorm

	;; ok, now we need an appropriate horizontal normal.. this is generated from the
	;; delta value (which gives direction).
	;; note that in the corner, this is incorrect, but hopefully, since its the corner,
	;; it wont cause too many problems.
	NormalizeVector3	horiznorm,	delta

	;; we skew the normal towards the vertical based on the time scale and wake height..
	mul.z			cosout,				cosout,					timescale[w]
	mul.z			cosout,				cosout,					ringParams2[y]
	
	;; now we interpolate bewteen them based on the sine value (cosout[z])
	;; for that we need 1-sin
	sub.w			oneminussin,		vf00,						cosout[z]
	mul.xyz		ACC,					horiznorm,				cosout[z]
	madd.xyz		thisringnorm,		vertnorm,				oneminussin[w]
	
	;; we need to also kill this rings normal contribution if wsign0 is clear...
	mul.xyz		thisringnorm,		thisringnorm,			wsign0[w]

	;; accumulate the normal
	add.xyz		ringnormal,			ringnormal,				thisringnorm
		
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; loop
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	ibnez			counter,				GY_RING_LOOP

	;; renormalize the ring normal...
	NormalizeVector3	ringnormal,	ringnormal

	;; contribute to the normal...
	add.xyz		normal,				normal,					ringnormal

	
GY_SkipRings:
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; get wake height/normal..
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; prep
	mul.xz		VtxXZXZ,				ones,						offsetvtx[x]
	mul.yw		VtxXZXZ,				ones,						offsetvtx[z]
	mul.xz		wakeradiussq,		ones,						miscqw24[w]
	div			Q,						vf00[w],					wakeradiussq[x]
	mul.w			invradiussq,		vf00,						Q
	zero			totalwakescale

	;; loop across all the wake pairs
	ilw.y,		counter,				WAKE_PAIRS(vi00)
	iand			counter,				counter,					bytemask
	zero			totalwakenormal
	iliu			hasnormal,			0

	ibeqz			counter,				GY_SkipWakes

GY_WAKE_LOOP:
	idecr			counter
	lq				wakepair,			WAKE_PAIRS(counter)

	;; determine scale based on distance
	sub			wakediff,			wakepair,				VtxXZXZ
	mul			wakediffsq,			wakediff,				wakediff
	mr32.xz		wakediffsqrot,		wakediffsq
	add.xz		wakelensq,			wakediffsq,				wakediffsqrot
	mini.xz		wakelensq,			wakeradiussq,			wakelensq
	sub.xz		wakedistsq,			wakeradiussq,			wakelensq
	mul.xz		wakedistscale,		wakedistsq,				invradiussq[w]

	;; determine passed scale value...
	mtir			wakescale1,			wakepair[x]
	mtir			wakescale2,			wakepair[z]
	iand			wakescale1,			wakescale1,				bytemask
	iand			wakescale2,			wakescale2,				bytemask
	mfir.x		wakepassscale,		wakescale1
	mfir.z		wakepassscale,		wakescale2
	itof0.xz		wakepassscale,		wakepassscale
	loi			0x3C008081		  ; (2.0 / 255)
	muli.xz		wakepassscale,		wakepassscale,			I

	;; merge them...
	mul.xz		wakescale,			wakedistscale,			wakepassscale

	;; now for the normal... ick...
	mul.xz		normysq,				wakedistscale,			wakedistscale
	sub.xz		oneminusy,			ones,						normysq

	;; make best normal..
	bole.x		wakescale,			totalwakescale,		GY_DontMakeNormal1
	bole.x		wakescale,			wakescale[z],			GY_DontMakeNormal1
	
	;; ick... make normal1...
	;; now the x & z parts are based on our direction around the sphere...
	;; which is to say, a scaled version of wakediff..
	;; the scale is m = (1 - y**2) / (x**2 + z**2)
	;; we already have x**2 + z**2 in wakelensq...
	;; we just need to recip and handle the top of the divide...
	div			Q,						oneminusy[x],			wakelensq[x]
	mul.w			temp,					vf00,						Q
	sqrt			Q,						temp[w]
	mul.xy		temp,					wakediff,				Q
	sub.x			wakenormal,			vf00,						temp[x]
	sub.z			wakenormal,			vf00,						temp[y]
	add.y			wakenormal,			vf00,						wakedistscale[x]
	mul.xz		wakenormal,			wakenormal,				wakescale[x]
	add.xyz		totalwakenormal,	totalwakenormal,		wakenormal

	iliu			hasnormal,			1
	
GY_DontMakeNormal1:	
	bole.z		wakescale,			totalwakescale,		GY_DontMakeNormal2

	div			Q,						oneminusy[z],			wakelensq[z]
	mul.w			temp,					vf00,						Q
	sqrt			Q,						temp[w]
	mul.zw		temp,					wakediff,				Q
	sub.x			wakenormal,			vf00,						temp[z]
	sub.z			wakenormal,			vf00,						temp[w]
	add.y			wakenormal,			vf00,						wakedistscale[z]
	mul.xz		wakenormal,			wakenormal,				wakescale[z]
	add.xyz		totalwakenormal,	totalwakenormal,		wakenormal
	
	iliu			hasnormal,			1
	
GY_DontMakeNormal2:	

	;; grab best scale
	max.xz		totalwakescale,	totalwakescale,		wakescale
	
	ibnez			counter,				GY_WAKE_LOOP

	;; do we need to play with the normal?
	ibeqz			hasnormal,			GY_SkipWakeNormal

	;; fart with the wake normal to make it less extreme...
;	mul.xz		wakenormal,			wakenormal,				mInvScale
	;; renormalize the wake normal...
	NormalizeVector3	totalwakenormal,	totalwakenormal

	add.xyz		normal,				normal,					totalwakenormal

GY_SkipWakeNormal:	
		
	;; apply the wake
	max.x			totalwakescale,	totalwakescale,		totalwakescale[z]
	add.y			vtx,					vtx,						totalwakescale[x]

GY_SkipWakes:	
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; get the physics field value..
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; we need the 4 verts surrounding the point, as well as the verts above & below that
	;; for calculation of the normal.. ie, the uppercase letters in this graphic
	;;  a  B  C  d
	;;  E  F  G  H
	;;  I  J  K  L
	;;  m  N  O  p
	;; where the point lies somewhere in the quad FGJK ...
	;; verts off the edge can simply be assigned heights of 1.0
	;; this will not quite match the generic version of the code, but it will
	;; be fairly close.
	;; Im going to look at that graph like Z runs across and X runs down.
	;; so, there are 4 X values in use.  for each X value, there are 4 possiblities
	;; Z specifies   X  Y  Z  W   (I call this field 1)
	;; or            Y  Z  W  X   (I call this field 2)
	;; or            Z  W  X  Y   (I call this field 3)
	;; or            W  X  Y  Z   (I call this field 0)
	;; or            0  0  X  Y   (field -1.  This only happens along the z=0 edge of the water and is handled iwth field3 code)
	;; the code below generates x & z indices in a nice clean two dimensional array
	;; but since we are in asm with a compressed array, we'll have to do some manual
	;; work to get the actual entries...

	;; this code impliments this C code
	;; // four nearest grid lines
	;; int gridxl = (int)(x * 16);
	;; int gridzl = (int)(z * 16);
	;; int gridxh = gridxl + 1;
	;; int gridzh = gridzl + 1;
	;; its purpose is to determine what the 2d indices of vtx F are.
	loi 16.0
	mul.xz		grid16,				vtx,						I
	ftoi0.xz		Fi,					grid16

	;; while we have these values, we ought to generate these...
	;; this determines the baricentric coords of our point in the quad FGJK
	;; // where between the lines are we
	;; float weightxh = x * (16) - gridxl;
	;; float weightzh = z * (16) - gridzl;
	;; float weightxl = 1.0f - weightxh;
	;; float weightzl = 1.0f - weightzh;
	itof0.xz		F,						Fi
	sub.xz		weighth,				grid16,					F
	sub.xz		weightl,				ones,						weighth

	;; now back to finding the right height values.
	;; we need to find the x/z integer indices for vertex a
	;; which are one less then F in both X & Z.  However, the
	;; code below for fields works as though z is already 1 less
	;; so we dont need to subtract 1 for z.
	sub.x			a,						F,							vf00[w]
	move.z		a,						F

	;; now we need to convert these into a qword & field# so that we can tell what
	;; we are to fetch from vumem..
	;; subqword = floor(z / 4)
	;; field = z - subqword * 4
	;; qword = subqword + x * 5
	;; ptr[z] = field
	;; ptr[x] = qword
	loi			0.25
	mul.z			subqword,			a,							I
	ftoi0.z		subqword,			subqword
	itof0.z		subqword,			subqword
	add.z			ACC,					vf00,						a
	loi			4.0
	msub.z		ptr,					subqword,				I
	loi			5.0
	add.x			ACC,					vf00,						subqword[z]
	madd.x		ptr,					a,							I
	ftoi0.xz		ptr,					ptr
	mtir			field,				ptr[z]
	mtir			qword,				ptr[x]

	;; all the combinations always need these 4 cells...
	;; the center 4 are gauranteed to be valid.
	zero			valqw[0]
	zero			valqw[2]
	zero			valqw[3]

	ibltz			qword,				GY_SkipPlus0
	lq				valqw[0],			GRID_BASE(qword)
GY_SkipPlus0:	
	lq				valqw[1],			GRID_BASE+5(qword)
	ibgtiu		qword,				74,						GY_SkipPlus10
	lq				valqw[2],			GRID_BASE+10(qword)
	ibgtiu		qword,				69,						GY_SkipPlus15
	lq				valqw[3],			GRID_BASE+15(qword)
GY_SkipPlus10:
GY_SkipPlus15:	

	;; now we switch on the field value.  There are 3 main pieces of code, for field
	;; values of 0, 1, or 2&3.
	ibeqz			field,				GY_Field0
	idecr			field
	ibeqz			field,				GY_HaveFields

	;; field is either 2 or 3..  (but we subtracted one from it above..)
	;; in this case, we want to load the current qword and the next qword..
	;; we do this for all 4 x values we are intersted in (x values are 5 qwords
	;; apart)
	zero			valqw1[0]
	zero			valqw1[2]
	zero			valqw1[3]
	ibltz			qword,				GY_SkipPlus1
	lq				valqw1[0],			GRID_BASE+1(qword)
GY_SkipPlus1:	
	lq				valqw1[1],			GRID_BASE+6(qword)
	ibgtiu		qword,				73,						GY_SkipPlus11
	lq				valqw1[2],			GRID_BASE+11(qword)
	ibgtiu		qword,				68,						GY_SkipPlus16
	lq				valqw1[3],			GRID_BASE+16(qword)
GY_SkipPlus11:	
GY_SkipPlus16:		
		
	idecr			field
	ibeqz			field,				GY_Field2

	;; field is 3...
GY_Field3:
	
	;; merge rows together before we rotate them
	move.xy		valqw[0],			valqw1[0]
	move.xy		valqw[1],			valqw1[1]
	move.xy		valqw[2],			valqw1[2]
	move.xy		valqw[3],			valqw1[3]

GY_Field3_Rotate:
	
	;; rotate them once... 
	mr32			valqw[0],			valqw[0]
	mr32			valqw[1],			valqw[1]
	mr32			valqw[2],			valqw[2]
	mr32			valqw[3],			valqw[3]

	;; then use FIeld2's rotate to rotate them again...
	b				GY_Field2_Rotate

GY_Field0:
	;; this is the rather unusual case where we need the entry BEFORE
	;; qword..
	;; TODO:	 deal with off edge
	zero			valqw1[0]
	zero			valqw1[1]
	zero			valqw1[2]
	zero			valqw1[3]
	ibltz			qword,				GY_SkipMinus1
	iblez			qword,				GY_SkipPlus4
	lq				valqw1[0],			GRID_BASE-1(qword)
GY_SkipMinus1:
	lq				valqw1[1],			GRID_BASE+4(qword)
GY_SkipPlus4:
	ibgtiu		qword,				75,						GY_SkipPlus9
	lq				valqw1[2],			GRID_BASE+9(qword)
	ibgtiu		qword,				70,						GY_SkipPlus14
	lq				valqw1[3],			GRID_BASE+14(qword)
GY_SkipPlus9:
GY_SkipPlus14:	
	
	;; merge them..
	move.w		valqw[0],			valqw1[0]
	move.w		valqw[1],			valqw1[1]
	move.w		valqw[2],			valqw1[2]
	move.w		valqw[3],			valqw1[3]

	;; rotate them once... 
	mr32			valqw[0],			valqw[0]
	mr32			valqw[1],			valqw[1]
	mr32			valqw[2],			valqw[2]
	mr32			valqw[3],			valqw[3]

	;; then use FIeld3's rotate to rotate them twice more
	b				GY_Field3_Rotate
	
GY_Field2:
	;; we just need to merge them.
	move.x		valqw[0],			valqw1[0]
	move.x		valqw[1],			valqw1[1]
	move.x		valqw[2],			valqw1[2]
	move.x		valqw[3],			valqw1[3]

	;; rotate them around..
GY_Field2_Rotate:	
	mr32			valqw[0],			valqw[0]
	mr32			valqw[1],			valqw[1]
	mr32			valqw[2],			valqw[2]
	mr32			valqw[3],			valqw[3]

GY_HaveFields:	

	;; ok, all the values are in valqw[n].  now we can blend the quad by the
	;; baricentric values to get the vertex height...

	;; so first we need the merges of the weights...
	mul.x			weightll,			weightl,					weightl[z]
	mul.x			weightlh,			weightl,					weighth[z]
	mul.x			weighthl,			weighth,					weightl[z]
	mul.x			weighthh,			weighth,					weighth[z]

	;; we break from physics for a moment to calculate the dampening value
	;; now we can do the muls and adds...
	iliu			twofiftyfive,		0xFF
	mtir			damp,					valqw[1][y]
	iand			damp,					damp,						twofiftyfive
	mfir.x		dampf,				damp
	mtir			damp,					valqw[1][z]
	iand			damp,					damp,						twofiftyfive
	mfir.y		dampf,				damp
	mtir			damp,					valqw[2][y]
	iand			damp,					damp,						twofiftyfive
	mfir.z		dampf,				damp
	mtir			damp,					valqw[2][z]
	iand			damp,					damp,						twofiftyfive
	mfir.w		dampf,				damp
	itof0			dampf,				dampf
	mul.x			ACC,					weightll,				dampf[x]
	madd.x		ACC,					weightlh,				dampf[y]
	madd.x		ACC,					weighthl,				dampf[z]
	madd.x		dampval,				weighthh,				dampf[w]

	;; now we can do the muls and adds...
	mul.x			ACC,					weightll,				valqw[1][y]
	madd.x		ACC,					weightlh,				valqw[1][z]
	madd.x		ACC,					weighthl,				valqw[2][y]
	madd.x		physicsheight,		weighthh,				valqw[2][z]

	;; put the height into the vertex...
	add.y			vtx,					vtx,						physicsheight[x]

	;; whew... now we can do the normal... its much worse than the height...
	;; again we use the values in valqw[n]
	;; for this we need to generate the normal at each corner...
	;; it goes something like this:
	;; N1 = (I-K, ?, F-N)*Whl
	;; N2 = (J-L, ?, G-O)*Whh
	;; N3 = (E-G, ?, B-J)*Wll
	;; N4 = (F-H, ?, C-K)*Wlh
	;; N = (N1 + N2 + N3 + N4) * 0.5
	;; there is no real clean way to do all this... so we'll do it the hard way
	;; many of these are generated into the wrong spots and need to be moved before
	;; we use them.
	sub.x			N1,					valqw[2],				valqw[2][z]	; N1[x] = N1.x = I - K
	sub.y			N2,					valqw[2],				valqw[2][w] ; N2[y] = N2.x = J - L
	sub.x			N3,					valqw[1],				valqw[1][z]	; N3[x] = N3.x = E - G
	sub.y			N4,					valqw[1],				valqw[1][w]	; N4[y] = N4.x = F - H

	;; we could merge these down to 2 subtracts instead of 4, but then they would be
	;; harder to get into the proper places.
	sub.y			N1,					valqw[1],				valqw[3]	; N1[y] = N1.z = F - N
	sub.z			N2,					valqw[1],				valqw[3]	; N2[z] = N2.z = G - O
	sub.y			N3,					valqw[0],				valqw[2]	; N3[y] = N3.z = B - J
	sub.z			N4,					valqw[0],				valqw[2]	; N4[z] = N4.z = C - K

	;; now we want scale & add the elements all up.  To do that, it would be best if they
	;; where in their proper places.. but we will use y instead of z since it will be easier
	;; that way
	mr32.xy		N2,					N2
	mr32.xy		N4,					N4

	;; now scale & add
	mul.xy		ACC,					N1,						weighthl[x]
	madd.xy		ACC,					N2,						weighthh[x]
	madd.xy		ACC,					N3,						weightll[x]
	madd.xy		phynormal,			N4,						weightlh[x]

	;; now we need to put y back where it belongs.
	;; you would think this would be Z, but, the arrangement of the
	;; data in the grid is actually swapped what you would think, so
	;; this is really x.. but that means we need x in z...
	mul.z			phynormal,			ones,						phynormal[x]
	mul.x			phynormal,			ones,						phynormal[y]

	;; scale to the corerct length (this was just tuned until it looked right
	;; when compared to the generic code, orignally, it was 0.5, which I think
	;; is the correct equation...)
	;; there is a control per layer that has affect in addition to this one, so
	;; this one just must match the generic version.
	loi			2.0
	mul.xz		phynormal,			phynormal,				I
	
	;; now we need to generate y...  it should make the normal unit length
	;; after the scale above
	add.x			ACC,					vf00,						vf00[w]
	msub.x		ACC,					phynormal,				phynormal
	add.x			temp,					vf00,						phynormal[z]
	msub.x		ysq,					temp,						temp
	sqrt			Q,						ysq[x]
	add.y			phynormal,			vf00,						Q

	;; finish off the calcuation of the dampening value...
	loi			0x3B808081		  ; 1 / 255
	mul.w			dampval,				vf00,						dampval[x]
	muli.w		normal,				dampval,					I

	;; apply the normal
	add.xyz		normal,				normal,					phynormal
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; handle noise field... ick...
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; grab the noise params
	lq				noise[0],			NOISE_PART1(vi00)
	lq				noise[1],			NOISE_PART2(vi00)
	mr32			noise[1],			noise[1]


	;; onetwothree[x] = 2.0
	;; onetwothree[y] = 1.0
	;; onetwothree[w] = 3.0
	loi			2.0
	mul			twos,					ones,						I
	move			onetwothree,		ones
	add.xw		onetwothree,		onetwothree,			vf00[w]
	add.w			onetwothree,		onetwothree,			vf00[w]

	;; scaled = x*mX, z*mZ
	mul.xz		scaled,				offsetvtx,				noise[1]

	;; tx[x] = t1 + scaledx
	;; tx[y] = t2 + scaledx
	;; tx[w] = t0 + scaledx
	;; tz[x] = t1 + scaledz
	;; tz[y] = t2 + scaledz
	;; tz[w] = t0 + scaledz
	;; note that we scale z by a tiny value to cause z to change
	;; at a different rate the x, which should help keep the noise
	;; from being too repetitive...
	mul.w			masterT,				mT,						noise[0]
	add.w			masterTx,			masterT,					scaled[x]
	ftoi0.w		txi,					masterTx
	itof0.w		tx,					txi
	add.w			masterTz,			masterT,					scaled[z]
	loi			0x3F700000 ; (15 / 16)
	mul.w			masterTz,			masterTz,				I
	ftoi0.w		tzi,					masterTz
	itof0.w		tz,					tzi

	
	;; mx[x] = m2
	;; mx[y] = m1
	;; mx[z] = m0
	;; mx[w] = m3
	sub.w			mx,					masterTx,				tx
	mr32.z		mx,					mx
	sub.xyw		mx,					onetwothree,			mx[z]
	sub.w			mz,					masterTz,				tz
	mr32.z		mz,					mz
	sub.xyw		mz,					onetwothree,			mz[z]

	;; look up the entries in the noise table.
	iliu			fifteen,				0x0F
	mtir			idx,					txi[w]
	iand			idx,					idx,						fifteen
	lq.x			b0,					NOISETABLE(idx)
	iaddiu		idx,					idx,						1
	iand			idx,					idx,						fifteen
	lq.x			b1,					NOISETABLE(idx)
	iaddiu		idx,					idx,						1
	iand			idx,					idx,						fifteen
	lq.x			b2,					NOISETABLE(idx)

	mtir			idx,					tzi[w]
	iand			idx,					idx,						fifteen
	lq.y			b0,					NOISETABLE(idx)
	iaddiu		idx,					idx,						1
	iand			idx,					idx,						fifteen
	lq.y			b1,					NOISETABLE(idx)
	iaddiu		idx,					idx,						1
	iand			idx,					idx,						fifteen
	lq.y			b2,					NOISETABLE(idx)

	;; xval = m2*xval1, m1*xval2, ?, m3*xval0
	mul.w			xval,					mx,						b0[x]
	mul.x			xval,					mx,						b1[x]
	mul.y			xval,					mx,						b2[x]
	mul.w			zval,					mz,						b0[y]
	mul.x			zval,					mz,						b1[y]
	mul.y			zval,					mz,						b2[y]

	;; val[w] = m3*xval0 - m1*xval2
	sub.w			val,					xval,						xval[y]
	;; ACC[w] = m2*xval1
	mul.w			ACC,					vf00,						xval[x]
	;; val[w] = (m2*xval1 - (m3*xval0 - m1*xval2) * 0.5f)
	loi			0.5
	msub.w		val,					val,						I
	;; ACC[w] = xval0
	mul.w			ACC,					vf00,						b0[x]
	;; xval[w] = xval0 + m0 * val[w]
	madd.w		xval,					val,						mx[z]

	;; val[w] = m3*xval0 - m1*xval2
	sub.w			val,					zval,						zval[y]
	;; ACC[w] = m2*xval1
	mul.w			ACC,					vf00,						zval[x]
	;; val[w] = (m2*xval1 - (m3*xval0 - m1*xval2) * 0.5f)
	loi			0.5
	msub.w		val,					val,						I
	;; ACC[w] = xval0
	mul.w			ACC,					vf00,						b0[y]
	;; xval[w] = xval0 + m0 * val[w]
	madd.w		zval,					val,						mz[z]

	;; now the final blend... 2 multiples to result with a value
	;; which is added to the value in vtx.y
	mul.w			val,					xval,						zval
	add.y			ACC,					vf00,						vtx
	madd.y		vtx,					noise[0],				val[w]

	;; while its incorrect, to determine the normal, I just get the two slopes
	;; between the three points  (slope of segment b0-b1 and slope of segment
	;; b1-b2), and linearly interpolate based on m0
	sub.xy		slope0,				b1,						b0
	sub.xy		slope1,				b2,						b1
	mul.x			slope0,				slope0,					mx[y]
	mul.x			slope1,				slope1,					mx[z]
	mul.y			slope0,				slope0,					mz[y]
	mul.y			slope1,				slope1,					mz[z]
	add.xy		n,						slope0,					slope1
	mul.xy		n,						n,							noise[0][y]
	add.z			n,						vf00,						n[y]

	;; we put a scale factor into y to try to get the whole thing back 
	;; into a decent range
	loi			4.0
	add.y			n,						vf00,						I

	;; renomalize (to keep our contribution equal to the others) and add it in...
	NormalizeVector3	n,				n
	add.xyz		normal,				normal,					n
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; apply the dampening field value to the Y value we retrieved..
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mul.y			vtx,					vtx,						normal[w]

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; apply the passed surface height value...
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	add.y			vtx,					vtx,						miscqw24
		
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; all thats left is to renormalize the normal...
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	NormalizeVector3	normal,		normal

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; GetY for now is done..
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			
	b				ReturnFromGetY

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; handle passes after the first
;;; inputs	buffcount
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		;; TODO:	because RF_EmptyFan can get called, we might have an odd # of fans.
RenderMultiPass:

	;; load the relevant matrices
	LoadMatrix	mScreenToClip,		SCREEN_TO_CLIP
	
	;; the two buffers that we have to work with
	;; buffer2 is currently rendering...
	;; buffer1 is the other buffer...
	
	;; for each layer after the first:
	iliu			layer,				1
	ilw.y			numLayers,			LAYERS(vi00)
	iandiu		numLayers,			numLayers,				0xFF
	ibeq			layer,				numLayers,				RMP_DoneWithLayers

RMP_NextLayer:	
	
	GET_LAYER_STUFF

	;; loop through both buffers
	imove			currbuff,			buffcount
	
RMP_NextBuffer:	
	;; get vert count
	ilw.x			vtxCount,			SET_LAYER_KICK_SIZE(mKickBuffer[1])	:KICKBUFFER
	iandiu		vtxCount,			vtxCount,				0x7fff

	;; get normal buffer pointer
	ilw.x			mNormalBufferPtr,	WS_NORMAL_BUFFERS(vi00)		:	WS_NORMAL_BUFFERS

	;; perform the layer change in the work buffer...
	BUILD_SETLAYER		1

	;; skip gif tag for verts
	iincr			mKickBufferPtr

	;; determine which vertex loop to run based on layer type...
	iadd			layer2,				layer,					layer
	iadd			layerptr,			layer2,					layer2
	iadd			layerptr,			layerptr,				layer2
	ilw.x			layerType,			LAYERS(layerptr)
	iandiu		layerType,			layerType,				0xFF

	idecr			layerType
	ibeqz			layerType,			RMP_EnvironmentVerts
	idecr			layerType
	ibeqz			layerType,			RMP_SpecularVerts
	idecr			layerType
	ibeqz			layerType,			RMP_DistortVerts
	RMP_UPDATEDECAL_VERTS_LOOP		0,	1, 0, 0

RMP_DoneWithVerts:

	;; kick it off
	xgkick		mKickBuffer[1]

	;; swap buffers
	ibeqiu		buffcount,			1,							RMP_SingleBuffer
	SWAPKICKBUFFER	1, 2
	
	b				RMP_DoneBufferHandle
RMP_SingleBuffer:
	NOPKICK
RMP_DoneBufferHandle:
	
	;; next buffer
	idecr			currbuff
	ibgtz			currbuff,			RMP_NextBuffer

	;; next layer
	iincr			layer
	ilw.y			numLayers,			LAYERS(vi00)
	iandiu		numLayers,			numLayers,				0xFF
	ibne			layer,				numLayers,				RMP_NextLayer	
	
RMP_DoneWithLayers:

	;; now to handle sprites... we handle these similarly to a decal layer...
	iliu			sprite,				0
	ilw.z			numSprites,			SPRITES+1(vi00)
	iandiu		numSprites,			numSprites,				0xFF
	ibeq			sprite,				numSprites,				RMP_DoneWithSprites

RMP_NextSprite:
	
	;; perform the layer change in the work buffer...
	GET_SPRITE_INFO

	;; loop through both buffers
	imove			currbuff,			buffcount

	;; get normal buffer pointer
	ilw.x			mNormalBufferPtr,	WS_NORMAL_BUFFERS(vi00)		:	WS_NORMAL_BUFFERS
	
RMP_NextSpriteBuffer:	
	;; get vert count
	ilw.x			vtxCount,			SET_LAYER_KICK_SIZE(mKickBuffer[1])	:KICKBUFFER
	iandiu		vtxCount,			vtxCount,				0x7fff

	;; get ready to loop over verts
	BUILD_SETSPRITE	1

	;; skip gif tag for verts
	iincr			mKickBufferPtr

	;; always run decal verts loop
	RMP_UPDATEDECAL_VERTS_LOOP		1, 0, 1, 1

	;; kick the buffer off
	xgkick		mKickBuffer[1]

	;; swap buffers
	ibeqiu		buffcount,			1,							RMP_SingleSpriteBuffer
	SWAPKICKBUFFER	1, 2

	b				RMP_DoneBufferHandleSprite
RMP_SingleSpriteBuffer:
	NOPKICK
RMP_DoneBufferHandleSprite:
	
	;; next buffer
	idecr			currbuff
	ibgtz			currbuff,			RMP_NextSpriteBuffer

	;; next layer
	iincr			sprite
	ilw.z			numSprites,			SPRITES+1(vi00)
	iandiu		numSprites,			numSprites,				0xFF
	ibne			sprite,				numSprites,				RMP_NextSprite
	
RMP_DoneWithSprites:
	
	jr				retaddr



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; kick the current fan (copy second vertex to last to close the fan...)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RenderFan:
	;; copy 2nd vtx to last to close fan...
	lq				vtx,					3(mKickBuffer[0])		:KICKBUFFER
	sq				vtx,					0(mKickBufferPtr)		:KICKBUFFER
	lq				vtx,					4(mKickBuffer[0])		:KICKBUFFER
	sq				vtx,					1(mKickBufferPtr)		:KICKBUFFER
	lq				vtx,					5(mKickBuffer[0])		:KICKBUFFER
	sq				vtx,					2(mKickBufferPtr)		:KICKBUFFER
	iincr			mKickBufferPtr,	KICK_VTX_SIZE

	;; get vertex count
	isub			size,					mKickBufferPtr,		mKickBuffer[0]
	mfir			fsize,				size
	loi			0x3EAAAAAB		  ; 1/3
	itof0			fsize,				fsize
	mul			fsize,				fsize,					I
	ftoi0			fsize,				fsize
	mtir			vtxcount,			fsize[x]

	;; detect buffer overruns
.aif WATER_DEBUG eq 1
	iliu			maxverts,			MAX_VERTS
	assertlt		vtxcount,			maxverts
.aendi
	
	;; we dont run clip code unless we touch +/- Z...
	;; or if we touched +/- Z while we were flat...
.aif 0 eq 1
	isw.z			totalclipflags,	WS_DARS_OUT_OF_VIEW(vi00)
	ibeqiu		totalclipflags,	2,							RF_DARSRequestScissor
.aendi
		  
	;; this caused problems because some polys were clipped that sat right
	;; next to polys that werent and with the distortion field's edge clamping
  	;; the polys wouldnt line up.  so now we clip them all...
.aif 0 eq 1
	ilw.z			totalclipflags,	WS_CLIPFLAGS(vi00)
	iandiu		totalclipflags,	totalclipflags,		0x3F	; B'11'1111
	ibeqz			totalclipflags,	RF_SkipClip
.aendi
RF_DARSRequestScissor:	
		
	;; copy first vtx to last to make edge list
	StaticCopyData	mKickBufferPtr, 0, mKickBuffer[0], 0, KICK_VTX_SIZE

	;; TODO:	 clip only if nessisary
	PushIntVector4	mKickBuffer[2],	quad,					depth
	StandardClip	vtxcount, mKickBuffer[0],	mKickBuffer[1],	SCISSOR_LAYER_COUNT,	"xXyYzZ",	RF_EmptyFan
	PopIntVector4	mKickBuffer[2],	quad,					depth
	
	;; detect buffer overruns
.aif WATER_DEBUG eq 1
	iliu			maxverts,			MAX_VERTS
	assertlt		vtxcount,			maxverts
.aendi

RF_SkipClip:
	
	;; need this to transform it all to Screen space...
	LoadMatrix	mClipToScreen,		CLIP_TO_SCREEN

	;; data will be in buffer 0 when Im done above... I move it to buffer 1 while transforming below...
	;; get ready to use the buffer
	;; by building the layer tags at the beginning of the buffer
	iliu			layer,				0
	BUILD_SETLAYER 1
	INIT_CURR_KICK_BUFFER			GIFTAG,					vtxcount, 1

	imove			InputPtr,			mKickBuffer[0]

	;; get normal buffer pointer ready
	ilw.x			mNormalBufferPtr,	WS_NORMAL_BUFFERS(vi00)		:	WS_NORMAL_BUFFERS
		
	;; color is constant... lets generate it..
	iliu			temp,					0x80
	mfir			rgba,					temp

	iadd			layer2,				layer,					layer
	iadd			layerptr,			layer2,					layer2
	iadd			layerptr,			layerptr,				layer2
	ilw.x			layerType,			LAYERS(layerptr)
	iandiu		layerType,			layerType,				0xFF
	idecr			layerType,			3
	ibeqz			layerType,			RF_DistortVtx
	
RF_NextVtx:
	--LoopCS		3,						0
	lq				vtx,					0(InputPtr)
	lq				st,					1(InputPtr)
	lq				normal,				2(InputPtr)
	iincr			InputPtr,			KICK_VTX_SIZE

	;; scale normal based on normal adj for this layer...
	mul.xyz		lnormal,				normal,					mNormAdj

	;; renomalize the normal
	NormalizeVector3	lnormal,		lnormal

	;; generate ST
	TransformVertex3	st,			mTexMatrix,				st

	;; add the normal to the st coord...
	add.x			st,					st,						lnormal
	add.y			st,					st,						lnormal[z]
	
	;; handle color...

	;; random option for debugging
.aif WATER_DEBUG eq 1
	ilw.x			flags0,				FLAGS0(vi00)
	ibafc			flags0,				FLG0_RAND_COLOR,	RF_DontRandColor
	rnext.x		rgba,					R
	rnext.y		rgba,					R
	rnext.z		rgba,					R
	ftoi15.xyz	rgba,					rgba
	;; skip fading if we are randomizing colors
	b				RF_FadeDone
RF_DontRandColor:
.aendi

	GENERATE_ALPHA_FADE_RGBA	rgba,	normal

RF_FadeDone:

		
	div			Q,						vf00[w],					vtx[w]
	zero.w		destvtx
	mul.xyz		destvtx,				vtx,						Q
	TransformVertex3	destvtx,		mClipToScreen,			destvtx
	ftoi4.xyz	destvtx,				destvtx
	One.z			st
	mul.xyz		st,					st,						Q

	sqi.xyz		st,					(mKickBufferPtr++)
	sqi			rgba,					(mKickBufferPtr++)
	sqi			destvtx,				(mKickBufferPtr++)
	sqi			normal,				(mNormalBufferPtr++)

	idecr			vtxcount
	ibgtz			vtxcount,			RF_NextVtx

	b				RF_VertsReady
	
RF_DistortVtx:
	--LoopCS		3,						0
	lq				vtx,					0(InputPtr)
	lq				st,					1(InputPtr)
	lq				normal,				2(InputPtr)
	iincr			InputPtr,			KICK_VTX_SIZE

	;; scale normal based on normal adj for this layer...
	mul.xyz		lnormal,				normal,					mNormAdj

	;; renomalize the normal
	NormalizeVector3	lnormal,		lnormal

	;; handle color...
	;; handle color...

	;; random option for debugging
.aif WATER_DEBUG eq 1
	ilw.x			flags0,				FLAGS0(vi00)
	ibafc			flags0,				FLG0_RAND_COLOR,	RF_DontRandColor2
	rnext.x		rgba,					R
	rnext.y		rgba,					R
	rnext.z		rgba,					R
	ftoi15.xyz	rgba,					rgba
	;; skip fading if we are randomizing colors
	b				RF_FadeDone2
RF_DontRandColor2:
.aendi

	GENERATE_ALPHA_FADE_RGBA	rgba,	normal

RF_FadeDone2:

	;; go from s
	
	;; add the normal to the local space vtx
	add.xz		st,					st,						lnormal

	;; now transform it into canonical space
	TransformVertex st,				mLocalToClip,			st
	div			Q,						vf00[w],					st[w]
	mul.xyz		st,					st,						Q

	;; convert from [-1,1] to [0,1]
	loi 0.5
	mul.xyz		st,					st,						I
	add.xyz		st,					st,						I

	;; now clamp to 0,1
	max.xyz		st,					st,						vf00[x]
	mini.xyz		st,					st,						vf00[w]

	;; we need to take into account the fact that 1.0 in y means 256.0f, but we can
	;; only go up to 224.0f... so here is a scale to correct that.
	loi 0x3F600000					  ; == .875 == 224/256
	mul.y			st,					st,						I

	;; ok, now we move the vertex into clip space...
	div			Q,						vf00[w],					vtx[w]
	zero.w		destvtx
	mul.xyz		destvtx,				vtx,						Q
	TransformVertex3	destvtx,		mClipToScreen,			destvtx
	ftoi4.xyz	destvtx,				destvtx
	One.z			st
	mul.xyz		st,					st,						Q

	sqi.xyz		st,					(mKickBufferPtr++)
	sqi			rgba,					(mKickBufferPtr++)
	sqi			destvtx,				(mKickBufferPtr++)
	sqi			normal,				(mNormalBufferPtr++)

	idecr			vtxcount
	ibgtz			vtxcount,			RF_DistortVtx
	b				RF_VertsReady
	
RF_VertsReady:	
	
	;; kick it all off.  Its in buffer 1.  we only kick verts in buffers 1 & 2.
	;; leaving buffer 0 for work.  so we need to swap buffers 1 & 2 now.
	xgkick		mKickBuffer[1]
	SWAPKICKBUFFER 1, 2

	;; check for prior buffer to know if we have two buffers full.
	;; once we have two buffers full, we need to do the other layers
	
	ilw.w			mOtherKickedBuff,	WS_ACTIVEBUFFERS(vi00)	:WS_ACTIVEBUFFERS
	ibeqz			mOtherKickedBuff,	RF_NoOtherBuffer

	;; reset the marker
	isw.w			vi00,					WS_ACTIVEBUFFERS(vi00)	:WS_ACTIVEBUFFERS

	;; ok, so we have two buffers to work with... lets use them.
	iliu			buffcount,			2
	b				RenderMultiPass

	
RF_NoOtherBuffer:
	;; save this buffer as active...
	isw.w			mKickBuffer[2],	WS_ACTIVEBUFFERS(vi00)	:WS_ACTIVEBUFFERS
	jr				retaddr

RF_EmptyFan:
	PopIntVector4	mKickBuffer[2],	quad,					depth
	jr				retaddr


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; expand needed subroutines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	;; create the scissor interpolator for the correct # of layers
	ScissorInterpolater SCISSOR_LAYER_COUNT
		
.END
