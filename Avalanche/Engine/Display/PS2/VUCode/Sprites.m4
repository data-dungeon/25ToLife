/***********************************/
/*                                 */
/* Sprites.vsm                     */
/* new engine          04/9/01     */
/* ned martin  avalanche software  */
/* ps2 renderer                    */
/*                                 */
/***********************************/

/* this VU1 microcode handles sprite processing

each sprite is a screen-aligned quad, each vertex is st/rgba/xyz. there are (sprite count * 4) vertices in the data,
processed 4 at a time for ease

this file needs to go through the m4 macro preprocessor before it gets assembled by dvpasm */

; vu ram layout- first single-buffered stuff

define(WORLD_TO_SCREEN,		`0')	; world-to-screen matrix (4 words)
define(GS_GIFTAG,				`4')	; giftag for sending some registers to gs before sprites are rendered (e.g. tex0)
; if there are any registers in GS_GIFTAG, leave rom for them, 1 qw each
define(NOP_GIFTAG,			`5')	; nop giftag for gs sync control (see notes at bottom of this file)
define(VIF1_BASE,				`6')	; end of single buffer, start of double buffers (VIF1_BASE value). should match SPRITE_VIF1_BASE_QWC

; then doubly-buffered stuff. offsets are from itop register

define(CONTROL_WORDS,		`0')	; x is loop count (vertex count)
define(GIFTAG,					`1')	; correct giftag for dma'd data (i.e. nloop count is correct)
define(HEADER_SIZE,			`2')	; sprite data starts after header

; each set of sprite is tex0/st/rgb/xyz/nop/st/rgb/xyz/nop/st/rgb/xyz/nop/st/rgb/xyz

define(TEX0_OFFSET,			`0')
define(ST0_OFFSET,			`1')
define(RGB0_OFFSET,			`2')
define(VERTEX0_OFFSET,		`3')
define(ALPHA1_OFFSET,		`4')
define(ST1_OFFSET,			`5')
define(RGB1_OFFSET,			`6')
define(VERTEX1_OFFSET,		`7')
define(ST2_OFFSET,			`9')
define(RGB2_OFFSET,			`10')
define(VERTEX2_OFFSET,		`11')
define(ST3_OFFSET,			`13')
define(RGB3_OFFSET,			`14')
define(VERTEX3_OFFSET,		`15')
define(SPRITE_SIZE,			`16')

; float registers

define(float1,				`vf00w')	; point to vf00.w
define(v0,					`vf01')
define(v1,					`vf02')
define(v2,					`vf03')
define(v3,					`vf04')
define(wtsmat0,			`vf05')	; world-to-screen matrix
define(wtsmat1,			`vf06')
define(wtsmat2,			`vf07')
define(wtsmat3,			`vf08')
define(tex0,				`vf09')
define(nextv0,				`vf10')
define(nextv1,				`vf11')
define(nextv2,				`vf12')
define(nextv3,				`vf13')
define(qone,				`vf14')
define(alpha1,				`vf15')	; a+d value for alpha1 register
; available: vf16--vf31

; integer registers

define(int0, 				`vi00')	; always 0
define(temp, 				`vi01') 	; *** temp MUST be vi01-- vi01 is special for some instructions (like fcand)
define(inbuf, 				`vi02')	; start of input buffer, loaded from itop register at start of processing
define(outbuf,				`vi03')	; start of input buffer, loaded from itop register at start of processing
define(loopcount,			`vi04')	; vertex count
define(blendfunc,			`vi05')	; blend function to use
define(lastblendfunc,	`vi06')	; last blend function used (controls whether we need to update gs register)
define(nopgiftag,			`vi07')	; address of nop giftag for gs sync purposes
define(int1, 				`vi08')	; integer 1
; available: vi09--vi15

/* DMA setup-- this label can be used to dma the code to the vu1 */

.global SpriteVU1CodeDMA
.global SpritesEnd
.global SpritesVUMemStart
.global SpritesVUMemEnd


.data

SpriteVU1CodeDMA:
.DMAPackVif 1	; enable TTE (packs vif codes into extra space in dma tag)

/* VIF Initialization */
DMAcnt *
STCYCL 4, 4
STMASK 0
STMOD 0
.EndDmaData

/* VU1 MicroCode */
DMAcnt *
MPG 0, *

SpritesVUMemStart:

SpritesSetup:

	; load world-to-screen matrix

	nop										lq.xyzw	wtsmat0, eval(WORLD_TO_SCREEN+0)(int0)
	nop										lq.xyzw	wtsmat1, eval(WORLD_TO_SCREEN+1)(int0)
	nop										lq.xyzw	wtsmat2, eval(WORLD_TO_SCREEN+2)(int0)
	nop										lq.xyzw	wtsmat3, eval(WORLD_TO_SCREEN+3)(int0)

	; set adc bit in w component of 1st and 2nd vertices of each set of 4 to break tri strip up into quads

	addw.z		qone,vf00,float1		iaddiu	temp,int0,0x4000							; qone.z = 1.0
	nop										iaddiu	temp,temp,0x4000
	nop										mfir.w	v0,temp
	nop										mfir.w	v1,temp
	nop										mfir.w	v2,int0
	nop										mfir.w	v3,int0

	; load address of "nop" giftag

	nop										iaddiu	nopgiftag,int0,NOP_GIFTAG

	; load default value for alpha1 register

	sub.w alpha1,alpha1,alpha1			iaddiu temp,int0,0x42	; 0x42 is alpha1 register
	nop										mfir.z alpha1,temp
	nop										iaddiu temp,int0,0x80	; 0x80 is a good default FIX value
	nop										mfir.y alpha1,temp
	nop										iaddiu temp,int0,0x44	; 0x44 is default blend function
	nop										mfir.x alpha1,temp

	; set up handy values

	nop										iaddiu	int1,int0,1

SpritesWaitForDMA:

	; wait for a dma to finish and say we've got something to do, then continue

	nop										nop														; necessary?
	nop[e]									nop	
	nop										nop														; necessary? i think this one is necessary to give vu time to copy itops into itop before loading below

SpritesProcess:

	; point to input buffer-- we use the itop register to double buffer as described in ee user's manual 4.3.8

	nop										xtop		inbuf

	; copy gs register values into gs giftag output buffer and kick to update gs register status. this is to allow
	; sprite packet to send 1 or more gs registers that common to all sprites in the packet, if necessary

; currently no gs registers to be sent at start of each sprite group, though (empty) gs giftag is still there if we need to resurrect this
;	nop										lq.xyzw	tex0,TEX0_VALUE (inbuf)					; load tex0 register value for texture state
;	nop										sq.xyzw	tex0,eval(GS_GIFTAG + 1)(int0)		; save into gs register output buffer
;	nop										iaddiu	temp,int0,GS_GIFTAG						; address of output buffer
;	nop										xgkick	temp

	; point to giftag for xgkick (the giftag is the word before the sprite vertex data)

	nop										iaddiu	outbuf,inbuf,GIFTAG
	
	; load control word data

	nop										ilw.x		loopcount,CONTROL_WORDS (inbuf)		; load loop count (vertex count)
	nop										iaddiu	inbuf,inbuf,HEADER_SIZE					; increment past header (including output giftag)

	; load 4 vertices to transform. sprite data goes st/rgba/xyz, 4 vertices per sprite

	nop										lq.xyz	nextv0,VERTEX0_OFFSET (inbuf)
	nop										lq.xyz	nextv1,VERTEX1_OFFSET (inbuf)
	nop										lq.xyz	nextv2,VERTEX2_OFFSET (inbuf)
	nop										lq.xyz	nextv3,VERTEX3_OFFSET (inbuf)

	; transform each one

	mulax.xyzw	acc,wtsmat0,nextv0	ilw.x		blendfunc,ST0_OFFSET (inbuf)								; load first s (u-coord) value-- it has blend func flags in it
	madday.xyzw	acc,wtsmat1,nextv0	nop
	maddaz.xyzw	acc,wtsmat2,nextv0	nop
	maddw.xyz	v0,wtsmat3,float1		lq.xyz	nextv0,eval(VERTEX0_OFFSET + SPRITE_SIZE)(inbuf)	; don't overwrite v0.w, which has adc bit set
	mulax.xyzw	acc,wtsmat0,nextv1	iand		blendfunc,blendfunc,int1									; mask off blendfunc flags
	madday.xyzw	acc,wtsmat1,nextv1	nop
	maddaz.xyzw	acc,wtsmat2,nextv1	nop

SpriteLoop:

	; do we need to change blend functions? this hardly ever happens, so the cost is typically 2 instructions per sprite

	nop										ibeq		blendfunc,lastblendfunc,TransformSprite
	nop										iadd		lastblendfunc,blendfunc,int0								; lastblendfunc = blendfunc
	nop										ibeq		blendfunc,int1,UpdateAlpha1
	nop										iaddiu	temp,int0,0x48													; additive blend function

	; default blend mode
	; if we need it, subtractive is 0xa1

	nop										iaddiu	temp,int0,0x44													; default blend function

UpdateAlpha1:

	; update the part of alpha1 (which is a 128-bit a+d value) that sets the blend function

	nop										mfir.x	alpha1,temp

TransformSprite:

	maddw.xyz	v1,wtsmat3,float1		lq.xyz	nextv1,eval(VERTEX1_OFFSET + SPRITE_SIZE)(inbuf)	; don't overwrite v1.w, which has adc bit set
	mulax.xyzw	acc,wtsmat0,nextv2	isubiu	loopcount,loopcount,4										; each time through the loop is 4 vertices
	madday.xyzw	acc,wtsmat1,nextv2	ilw.x		blendfunc,eval(ST0_OFFSET + SPRITE_SIZE)(inbuf)		; load next sprite's u-coord value
	maddaz.xyzw	acc,wtsmat2,nextv2	sq.xyzw	alpha1,ALPHA1_OFFSET (inbuf)								; write out current blend func value
	maddw.xyz	v2,wtsmat3,float1		lq.xyz	nextv2,eval(VERTEX2_OFFSET + SPRITE_SIZE)(inbuf)	; don't overwrite v2.w, which is 0
	mulax.xyzw	acc,wtsmat0,nextv3	nop
	madday.xyzw	acc,wtsmat1,nextv3	iand		blendfunc,blendfunc,int1									; mask off blendfunc flags
	maddaz.xyzw	acc,wtsmat2,nextv3	nop
	maddw.xyz	v3,wtsmat3,float1		lq.xyz	nextv3,eval(VERTEX3_OFFSET + SPRITE_SIZE)(inbuf)	; don't overwrite v3.w, which is 0

	; convert to 12.4 integer-- x, y, z only. and set q value in st data to 1 (no perspective correction neeeded for sprites).
	; z-value is 32 bit integer. ftoi4 does an effective multiply by 16, but the world-to-screen matrix has a factor of
	; 1/16 baked into the z-buffer range transform
	
	ftoi4.xyz	v0,v0						sq.z		qone,ST0_OFFSET (inbuf)
	ftoi4.xyz	v1,v1						sq.z		qone,ST1_OFFSET (inbuf)
	ftoi4.xyz	v2,v2						sq.z		qone,ST2_OFFSET (inbuf)
	ftoi4.xyz	v3,v3						sq.z		qone,ST3_OFFSET (inbuf)

	; write back to output buffer-- write w-component, too, which has adc bit set for first 2 vertices, and start
	; next transform

	mulax.xyzw	acc,wtsmat0,nextv0	sq.xyzw	v0,VERTEX0_OFFSET (inbuf)
	madday.xyzw	acc,wtsmat1,nextv0	sq.xyzw	v1,VERTEX1_OFFSET (inbuf)
	maddaz.xyzw	acc,wtsmat2,nextv0	sq.xyzw	v2,VERTEX2_OFFSET (inbuf)
	maddw.xyz	v0,wtsmat3,float1		lq.xyz	nextv0,eval(VERTEX0_OFFSET + SPRITE_SIZE + SPRITE_SIZE)(inbuf)	; don't overwrite v0.w, which has adc bit set
	mulax.xyzw	acc,wtsmat0,nextv1	sq.xyzw	v3,VERTEX3_OFFSET (inbuf)

	; update input buffer, check loop counter

	madday.xyzw	acc,wtsmat1,nextv1	ibgtz		loopcount,SpriteLoop						; test loop counter
	maddaz.xyzw	acc,wtsmat2,nextv1	iaddiu	inbuf,inbuf,SPRITE_SIZE					; we just did 4 vertices-- one sprite

	; all sprites transformed, kick to gs as a big list

	nop										xgkick	outbuf

	; and kick a nop giftag, too. this will stall until the previous xgkick is ready to start processing. this fixes a bug
	; in the sprite display code. vu ram is double-buffered, and sprites are transformed in place-- that is, there is not
	; an "input" and "output" portion of each half-buffer. in fluff screens, sprites are often a large portion of the screen,
	; so they take a significant time to render. we have this situation: buffer 1 contains some sprites that are being rendered
	; by an xgkick. buffer 2 gets processed and xgkick'd. there is no stall because the gs has a 2-deep xgkick stack. so we
	; loop back to SpritesWaitForDMA and tell the dma controller that we are ready for more data. this data gets sent to
	; buffer 1, writing over data that is still waiting to be rendered by the first xgkick.
	;
	; using a "nop" xgkick makes sure that the buffer 1 has finished rendering before we loop back to SpritesWaitForDMA,
	; since the gs's xgkick stack will be full. actually, what will happen is that the xgkick to render buffer 2's
	; sprites will stall, since buffer 1 will have issued 2 xgkicks (one real, one nop). but that's fine, and the result
	; is that we will never branch to SpritesWaitForDMA before the other buffer is done rendering, so we won't have to
	; worry about writing over the data.

	nop										xgkick	nopgiftag

	; done, back to top and wait for more sprites to process

	nop										b	SpritesWaitForDMA
	nop										nop

/* end DMA setup */

SpritesVUMemEnd:

.EndMPG
.EndDMAData

DMAEnd

SpritesEnd:

/* clean up m4 defines */

undefine(`WORLD_TO_SCREEN')
undefine(`GS_GIFTAG')
undefine(`NOP_GIFTAG')
undefine(`VIF1_BASE')

; then doubly-buffered stuff. offsets are from itop register

undefine(`TEX0_VALUE')
undefine(`CONTROL_WORDS')
undefine(`GIFTAG')
undefine(`HEADER_SIZE')

; each set of sprite is tex0/st/rgb/xyz/nop/st/rgb/xyz/nop/st/rgb/xyz/nop/st/rgb/xyz

undefine(`TEX0_OFFSET')
undefine(`ST0_OFFSET')
undefine(`RGB0_OFFSET')
undefine(`VERTEX0_OFFSET')
undefine(`ST1_OFFSET')
undefine(`RGB1_OFFSET')
undefine(`VERTEX1_OFFSET')
undefine(`ST2_OFFSET')
undefine(`RGB2_OFFSET')
undefine(`VERTEX2_OFFSET')
undefine(`ST3_OFFSET')
undefine(`RGB3_OFFSET')
undefine(`VERTEX3_OFFSET')
undefine(`SPRITE_SIZE')

; float registers

undefine(`float1')
undefine(`v0')
undefine(`v1')
undefine(`v2')
undefine(`v3')
undefine(`wtsmat0')
undefine(`wtsmat1')
undefine(`wtsmat2')
undefine(`wtsmat3')
undefine(`tex0')
undefine(`nextv0')
undefine(`nextv1')
undefine(`nextv2')
undefine(`nextv3')
undefine(`qone')
undefine(`alpha1')
			  
; integer registers

undefine(`int0')
undefine(`temp')
undefine(`inbuf')
undefine(`outbuf')
undefine(`loopcount')
undefine(`blendfunc')
undefine(`lastblendfunc')
undefine(`nopgiftag')
