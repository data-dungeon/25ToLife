// include the basic vcl include file (it has some modifications to it)		  
.alternate
.include "vcl_sml.i"
.include "High.i"
.macro InitKernel passcount
		  FieldWalker_Init
		  ; vtx is implicit, its always the first 3 fields
		  NextField
		  NextField
		  NextField
KernelPasses .assigna \passcount 
ClipLayers .assigna (\&KernelPasses + 1) ; the StandardClip routine counts the color as a layer...
PerpsCorrectTexCoords .assigna 0
ColorsAreCompressed .assigna 0
RegOptNum .assigna 9
MAX_LINES .assigna 10
STATE_HEADER_SIZE .assigna (3)
TEX_MATRIX_START .assigna (11)
CLIP_WORK_VTX_SIZE .assigna (\passcount + 2)
MAX_CLIP_VERTS .assigna (32)
SCALE_VERTS_UP .assigna (0)
TOP_OF_MEM .assigna (1024)
TEMP_STACK .assigna (\&TOP_OF_MEM)
STACK_SIZE .assigna (11)
TOP_OF_MEM .assigna (\&TOP_OF_MEM - \&STACK_SIZE)
CLIP_WORK_BUFFER_SIZE .assigna (\&CLIP_WORK_VTX_SIZE * \&MAX_CLIP_VERTS)
CLIP_WORK_START_0 .assigna (\&TOP_OF_MEM - \&CLIP_WORK_BUFFER_SIZE)
CLIP_WORK_START_1 .assigna (\&CLIP_WORK_START_0 - \&CLIP_WORK_BUFFER_SIZE)
CLIP_VTX_0 .assigna (0 * \&CLIP_WORK_VTX_SIZE)
CLIP_VTX_1 .assigna (1 * \&CLIP_WORK_VTX_SIZE)
CLIP_VTX_2 .assigna (2 * \&CLIP_WORK_VTX_SIZE)
CLIP_VTX_3 .assigna (3 * \&CLIP_WORK_VTX_SIZE)
PassDataAddr .assigna \&KernelPasses*4 + \&TEX_MATRIX_START

		  max					ones,	  VF00,	 VF00[w]
		  sub					zeros,  VF00,	 VF00
		  loi					255.0
		  mul					fogMax, ones,	I
		  lq.XYzw			inittemp,		 0(VI00)
		  mtir				flagsA,			 inittemp[y]
		  move.zw			fogParams,		 inittemp
		  fcset				0x3FFFF
		  ; init clipping flags
		  iadd				ClipFlags, VI00, VI00
		  ;;  init clipping space versions of verts
		  move				CSVertex[1], VF00
		  move				CSVertex[2], VF00
		  iaddiu				Stack, VI00, \&TEMP_STACK
.endm

.macro MatrixMultiplyVertex3 vertexresult, matrix, vertex
   add            acc,           \matrix[3], VF00[x]
   madd           acc,           \matrix[0], \vertex[x]
   madd           acc,           \matrix[1], \vertex[y]
   madd.xyz       \vertexresult, \matrix[2], \vertex[z]
.endm		  
		  
.macro BeginMicrocode mname
MicrocodeName .assignc \mname
		  .global \&MicrocodeName
		  .global End\&MicrocodeName
		  .name \&MicrocodeName
--enter
--endenter
.endm

.macro BeginBuffer
BufferLoop\&MicrocodeName:	 
--barrier
.raw
		  nop nop
		  nop nop
		  nop nop
.endraw
		  iaddiu	 temp1, vi00, \&CLIP_WORK_START_0
		  iaddiu	 temp2, vi00, \&CLIP_WORK_START_1
		  iaddiu  temp3, vi00, \&TOP_OF_MEM
		  GenOutputPointers
.raw
		  nop nop
		  nop nop
		  nop nop
.endraw

		  
		  xtop				input
		  LoadIntVector4	0,		  input, loopCount, flagsC, flagsD, flagsE
		  PrepOutputPointers
		  iaddiu				input,  input,	 1

		  ; init clipping flags
		  iadd				ClipFlags, VI00, VI00
.endm

.macro EndBuffer
--barrier
		  GenOutputPointers
		  xgkick output[0]
--barrier
--cont
		  b BufferLoop\&MicrocodeName
.endm

.macro KICK reg
		  --barrier
.raw
			  nop nop
			  nop nop
			  nop nop
.endraw
		  --barrier
		  xgkick \reg
.endm
		  		  
.macro BeginLoop
loop\&MicrocodeName:
--loopCS 4, 0
VtxWReady .assigna 0
VtxWValid .assigna 0
		  LoadLines
skipResetFC:	 
.endm

.macro CopyVtxDataToClip dest, offset, srcoffset
		  FieldWalker_UpdateLineCnt
		  
passcnt .assigna 0
.awhile \&passcnt LT \&FieldWalker_LineCnt
addr .assigna \&FieldWalker_LineCnt * srcoffset + \&passcnt
		  VectorLoad	  templine[\&passcnt], \&addr, input
passcnt .assigna \&passcnt + 1
.aendw

		  FieldWalker_Save
		  FieldWalker_Init

		  ;; skip vtx data
		  NextField
		  NextField
		  NextField
		  
		  ;; load color
.aif \&ColorsAreCompressed eq 1
		  move.xyzw			color, templine[0]
		  WrapColor4		color
		  itof0				color, color
		  sq.xyzw			color, \offset(\dest)
.aelse
		  ERROR! need to write this still
.aendi

		  ;; re-transform tex coords
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
realdstoffset .assigna (\offset + \&passcnt + 1)
;;; texpass
		  --barrier
		  LoadTexMatrix \&passcnt
		  TransformTexCoordsL tex, \&passcnt, 2, templine
		  sq.xy				tex[\&passcnt], \&realdstoffset(\dest)
		  --barrier
passcnt .assigna \&passcnt + 1
.aendw
		  
		  FieldWalker_Restore
		  
.endm

.macro PushOutputPtrs
.aif \&KernelPasses eq 1
		  PushIntVector4 output[0]
.aelse
.aif \&KernelPasses eq 2
		  PushIntVector4 output[0], output[1]
.aelse
.aif \&KernelPasses eq 3
		  PushIntVector4 output[0], output[1], output[2]
.aelse
		  PushIntVector4 output[0], output[1], output[2], output[3]
.aif \&KernelPasses eq 5
		  PushIntVector4 output[4]
.aelse
.aif \&KernelPasses eq 6
		  PushIntVector4 output[4], output[5]
.aelse
.aif \&KernelPasses eq 7
		  PushIntVector4 output[4], output[5], output[6]
.aelse
.aif \&KernelPasses eq 8
		  PushIntVector4 output[4], output[5], output[6], output[7]
.aendi
.aendi
.aendi
.aendi

.aendi
.aendi
.aendi
.endm
		  
.macro PopOutputPtrs
.aif \&KernelPasses eq 1
		  PopIntVector4 output[0]
.aelse
.aif \&KernelPasses eq 2
		  PopIntVector4 output[0], output[1]
.aelse
.aif \&KernelPasses eq 3
		  PopIntVector4 output[0], output[1], output[2]
.aelse
		  PopIntVector4 output[0], output[1], output[2], output[3]

.aif \&KernelPasses eq 5
		  PopIntVector4 output[4]
.aelse
.aif \&KernelPasses eq 6
		  PopIntVector4 output[4], output[5]
.aelse
.aif \&KernelPasses eq 7
		  PopIntVector4 output[4], output[5], output[6]
.aelse
.aif \&KernelPasses eq 8
		  PopIntVector4 output[4], output[5], output[6], output[7]
.aendi
.aendi
.aendi
.aendi

.aendi
.aendi
.aendi
.endm
		  
.macro PushTexCoords
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
		  PushVector3		finalTex[\&passcnt]
passcnt .assigna (\&passcnt + 1)
.aendw
.endm		  
		  
.macro PopTexCoords
passcnt .assigna (\&KernelPasses - 1)
.awhile \&passcnt GE 0
		  PopVector3		finalTex[\&passcnt]
passcnt .assigna (\&passcnt - 1)
.aendw
.endm		  

.macro FixupClippedVert
		  div					Q, vf00[w], vtx[w]
		  mulq				vtx, vtx, q
		  mr32.z				tex, vf00
		  mul.xyz			tex, tex, q
		  MatrixMultiplyVertex3	  vtx,			cullmtx, vtx
		  ftoi0				color, color
		  ftoi4				vtx, vtx
		  mini.w				vtx, vf00, vf00[x]
.endm
		  		  
.macro ScissorCode
local AverageVertex
local ScaledVertex
local adcbit
ScissorTriangle:			

		  ;; push a bunch of ints off to make room
		  PushIntVector4			  loopCount, input, ClipFlags

		  ;; check to see if all 3 verts were clipped by any individual plane
		  ;; HUH?: shouldnt this check clipflags instead, isnt this is a bug
;		  fcor						  vi01,				0xfdf7df
;		  ibne						  vi01,				VI00,				 ScissorEndFast
;		  fcor						  vi01,				0xfefbef
;		  ibne						  vi01,				VI00,				 ScissorEndFast
;		  fcor						  vi01,				0xff7df7
;		  ibne						  vi01,				VI00,				 ScissorEndFast
;		  fcor						  vi01,				0xffbefb
;		  ibne						  vi01,				VI00,				 ScissorEndFast
;		  fcor						  vi01,				0xffdf7d
;		  ibne						  vi01,				VI00,				 ScissorEndFast
;		  fcor						  vi01,				0xffefbe
;		  ibne						  vi01,				VI00,				 ScissorEndFast

		  PushOutputPtrs
		  PushVector4				  perspVtx
		  PushVector4				  finalColor
		  PushTexCoords
		  PushVector4				  CSVertex[0]
		  PushVector4				  CSVertex[1]
		  PushVector4				  CSVertex[2]
		  

		  ;; average the 3 verts
.aif \&SCALE_VERTS_UP EQ 1
		  loi							  0.3333333
		  mul							  ACC,				CSVertex[0],		 I
		  madd						  ACC,				CSVertex[1],		 I
		  madd						  AverageVertex,	CSVertex[2],		 I

		  ;; we are using the same ugly scissor algorithm that vu1_scissor uses,
		  ;; which requires that we scale up by .1% to avoid cracks...
		  sub							  ScaledVertex[0],	CSVertex[0],	 AverageVertex
		  sub							  ScaledVertex[1],	CSVertex[1],	 AverageVertex
		  sub							  ScaledVertex[2],	CSVertex[2],	 AverageVertex

		  loi							  1.01
		  mul							  ACC,					AverageVertex,	 VF00[w]
		  madd						  ScaledVertex[0],	ScaledVertex[0],	 I
		  mul							  ACC,					AverageVertex,	 VF00[w]
		  madd						  ScaledVertex[1],	ScaledVertex[1],	 I
		  mul							  ACC,					AverageVertex,	 VF00[w]
		  madd						  ScaledVertex[2],	ScaledVertex[2],	 I
.aelse
		  ;; TJC - disabled scaling for now.
		  move						  ScaledVertex[0],   CSVertex[0]
		  move						  ScaledVertex[1],   CSVertex[1]
		  move						  ScaledVertex[2],   CSVertex[2]
.aendi		  

		  ;; work0,work1 point to blocks that looks like:
		  ;; 0() = Clip Space Vertex
		  ;; 1() = Original color as float
		  ;; 2+() = PrePersp st 0..
		  iaddiu				work0, VI00, \&CLIP_WORK_START_0
		  iaddiu				work1, VI00, \&CLIP_WORK_START_1
		  
		  ;; store the 3 verts in the work buffer..
		  VertexSave				  ScaledVertex[0], \&CLIP_VTX_2, work0
		  VertexSave				  ScaledVertex[1], \&CLIP_VTX_1, work0
		  VertexSave				  ScaledVertex[2], \&CLIP_VTX_0, work0
.aif \&ColorsAreCompressed eq 1
		  WrapColor4				  finalColor
.aendi
		  itof0						  floatColor, finalColor
		  VectorSave				  floatColor, 1, work0
  		  SaveTexCoordsClip		  finalTex, 2+\&CLIP_VTX_0, work0
		  CopyVtxDataToClip		  work0, 1+\&CLIP_VTX_1, -1
		  CopyVtxDataToClip		  work0, 1+\&CLIP_VTX_2, -2

		  ;; store the first vertex in the 4th place to avoid an index wrap in the clip code...
		  StaticCopyData			  work0, \&CLIP_VTX_3, work0, \&CLIP_VTX_0, \&CLIP_WORK_VTX_SIZE

		  ;; ok, now for the real work...

		  ;; this is made much simpler by the standard clip method in high.i
		  iaddiu						  NewNbrVerts, VI00, 3
		  StandardClip				  NewNbrVerts, work0, work1, \&ClipLayers, "xXyYzZ", ScissorEnd

		  ;; draw triangle fan...
		  ;; TODO.. finish this thing...
		  ;; ok... ick... I dont know how to build the giftag stuff...  I guess we can do it one triangle
		  ;; at a time for now...
		  
		  ;; at this point, my fan is in work0... 

		  ;; we do this one pass, two verts at a time also.. ick...
		  ;; we use work1 as a kick buffer...
		  ;; ok, so here is how that buffer gets used... first
		  ;; the size of this buffer is
		  ;; (2 + KernelPasses) * 9
		  ;; so the smallest buffer size is 27 qwords.
		  ;; the layout (given a state size of 7 (current) is
		  ;; 0 = EOP=0 NLOOP=1 REG=PRIM
		  ;; 1 = PRIM fan
		  ;; 2 = EOP=1 State Data
		  ;; 8 = EOP=1 NLOOP=2 REG=STQ, RGBA, XYZ   ; called buff0
		  ;; 9 = STQ[0]
		  ;; 10 = RGBA[0]
		  ;; 11 = XYZ[0]
		  ;; 12 = EOP=1 NLOOP=2 REG=STQ, RGBA, XYZ ; called buff1
		  ;; 13 = STQ[0]
		  ;; 14 = RGBA[0]
		  ;; 15 = XYZ[0]
		  ;;

		  iaddiu				hex8000, vi00, 0x7fff
		  iaddiu				hex8000, hex8000, 1

		  ;; build prim reg tag ...
		  iaddiu				tag, vi00, 1
		  mfir.x				giftag, tag
		  move.zw			giftag, vf00
		  loi					2.524355e-29
		  max.y				giftag, vf00, I
		  sq					giftag, 0(work1)

		  ;; prim reg itself
		  ;; this is TriFan, Gruraud, TEX, Alpha, Context1
		  ;; if you need anything else, use PRMODE...
		  iaddiu				tag, vi00, 0x5D
		  isw.x				tag, 1(work1)

		  ;; get sizes of state data
		  VectorLoad		passStateSizes[0], 1,		  VI00
		  VectorLoad		passStateSizes[1], 2,		  VI00

		  ;; get the final transform matrix
		  LoadCullMatrix
		  
		  ;; for each pass
		  iaddiu				passData, VI00, \&PassDataAddr
		  iaddiu				pass, vi00, 0
		  iaddiu				passCnt, vi00, \&KernelPasses
FanPassLoop:

		  ;; generate dest ptr...
		  iaddiu				buff0, work1, 2

		  ;; copy pass init data into place...
		  mtir				passStateSize, passStateSizes[0][x]
		  mr32.xyz			passStateSizes[0], passStateSizes[0]
		  mr32.w				passStateSizes[0], passStateSizes[1]
		  mr32.xyz			passStateSizes[1], passStateSizes[1]
		  CopyData			buff0, passData, passStateSize

		  ;; patch eop bit of first qword...
		  ilw.x				tag, 2(work1)
		  iadd				tag, tag, hex8000
		  isw.x				tag, 2(work1)

		  ;; generate buff pointers
		  isubiu				buff0, buff0, 1
		  iaddiu				buff1, buff0, 4

		  ;; patch eop & nloop of last qword (which is gif tag...), and dup it...
		  lq.xyzw			giftag, 0(buff0)
		  iaddiu				tag, hex8000, 1
		  mfir.x				giftag, tag
		  sq.xyzw			giftag, 0(buff0)
		  sq.xyzw			giftag, 0(buff1)

		  ;; kick the pass data...
		  KICK				work1

		  ;; build input pointers we need.
		  iaddiu				vtxInput, work0, 0
		  iaddiu				texInput, vtxInput, 2
		  iadd				texInput, texInput, pass
		  
		  ;; do first 2 verts...
		  LoadVector4		0, vtxInput, vtx
		  LoadVector4		1, vtxInput, color
		  LoadVector2		0, texInput, tex
		  FixupClippedVert
		  SaveVector3		1, buff0, tex
		  SaveVector4		2, buff0, color
		  SaveVector4		3, buff0, vtx
  		  KICK				buff0
		  iaddiu				vtxInput, vtxInput, \&CLIP_WORK_VTX_SIZE
		  iaddiu				texInput, texInput, \&CLIP_WORK_VTX_SIZE
		  LoadVector4		0, vtxInput, vtx
		  LoadVector4		1, vtxInput, color
		  LoadVector2		0, texInput, tex
		  FixupClippedVert
		  SaveVector3		1, buff1, tex
		  SaveVector4		2, buff1, color
		  SaveVector4		3, buff1, vtx
		  KICK				buff1
		  		  
		  ;; for each vertex above the first 2
		  isubiu				loopCount, NewNbrVerts, 2

FanLoop:
--loopCS 1, 0
		  iaddiu				vtxInput, vtxInput, \&CLIP_WORK_VTX_SIZE
		  iaddiu				texInput, texInput, \&CLIP_WORK_VTX_SIZE
		  LoadVector4		0, vtxInput, vtx
		  LoadVector4		1, vtxInput, color
		  LoadVector2		0, texInput, tex
		  FixupClippedVert
		  SaveVector3		1, buff0, tex
		  SaveVector4		2, buff0, color
		  SaveVector4		3, buff0, vtx
		  KICK				buff0

		  ;; swap buffers
		  iadd				temp, vi00, buff1
		  iadd				buff1, vi00, buff0
		  iadd				buff0, vi00, temp

		  ;; loop through verts...
		  isubiu				loopCount, loopCount, 1
		  ibgtz				loopCount, FanLoop
		  
		  ;; loop through the passes
		  iaddiu				pass, pass, 1
		  ibne				pass, passCnt, FanPassLoop

		  ;; restore the prim reg
		  ;; this is TriStrip, Gruraud, TEX, Alpha, Context1
		  ;; if you need anything else, use PRMODE...
		  iaddiu				tag, vi00, 0x5C
		  isw.x				tag, 1(work1)
		  iaddiu				tag, hex8000, 1
		  isw.x				tag, 0(work1)
		  KICK				work1
		  xgkick				vi00	  ; nop
		  		  
ScissorEnd:
		  
		  ;; pop a bunch of ints off to restore the state
		  PopVector4				  CSVertex[2]
		  PopVector4				  CSVertex[1]
		  PopVector4				  CSVertex[0]

		  PopTexCoords
		  PopVector4				  finalColor
		  PopVector4				  perspVtx
		  PopOutputPtrs

		  ;; restore a bunch of floats to help alleviate float reg problems
.aif \&RegOptNum gt \&KernelPasses + 1
		  LoadCullMatrix
.aendi
.aif \&RegOptNum gt \&KernelPasses
		  LoadTransformMatrix
.aendi
loopcnt .assigna 0
.awhile \&loopcnt LT \&RegOptNum
		  LoadTexMatrix \&loopcnt
loopcnt .assigna \&loopcnt + 1
.aendw

ScissorEndFast: 		  
		  PopIntVector4			  loopCount, input, ClipFlags
		  
		  ;; just set the adc bit, the triangle should not be drawn in main strip
		  iaddiu						  adcbit,			VI00, 0x7fff
		  iaddiu						  adcbit,			adcbit, 1
		  mfir.w						  finalVtx,			adcbit

		  b							  KickUnscissoredTriangle
.endm
		  
.macro GetADCBit dest, withvi01
		  mtir	 ADCBit, line[0][x]
		  iaddiu	 FlagValue, VI00, 1
		  iand	 ADCBit, FlagValue, ADCBit
.aif withvi01 eq 1
		  ior		 ADCBit, vi01, ADCBit
.aendi
		  iaddiu ADCVal, ADCBit, 0x7fff
.aif \&VtxWValid eq 1
		  ftoi4.w culltempi, \dest
		  mtir	 culltemp2, culltempi[w]
		  ior		 ADCVal, culltemp2, ADCVal
.aendi
		  mfir.w	 \dest, ADCVal
.endm
		  
.macro EndLoop
		  FieldWalker_UpdateLineCnt
		  
		  iaddiu				mask,	  vi00, 0x03
		  iand				mask,	  mask, flagsA
		  iblez				mask,	  CullVtx
		  isubiu				mask,	  mask,	 1
		  iblez				mask,	  AlwaysVtx

ClipVtx:			 
		  ;; 
		  ;; clip!
		  ;; 

		  ;; set adc bit correctly...
		  GetADCBit					  finalVtx,			0

		  ;; move vtx stack down
		  add							  CSVertex[0],		CSVertex[1], VF00[x]
		  add							  CSVertex[1],		CSVertex[2], VF00[x]

		  ;; perform clip check
		  TransformVertex			  perspVtx,			transmtx
		  move						  CSVertex[2],		perspVtx
		  clipw.xyz					  perspVtx,			perspVtx
		  fcand						  vi01, 0x3f

		  ;; check if scissoring is nessisary...
		  ;; oldF = ((oldF << 1) | newF) & 0x7
		  iadd						  ClipFlags,		ClipFlags, ClipFlags
		  ior							  ClipFlags,		ClipFlags, vi01
		  iaddiu						  mask,				VI00, 0x7
		  iand						  ClipFlags,		ClipFlags, mask

		  ;; dont scissor if the adc bit is set... (since the triangle is unkicked anyway..)
		  ibne						  ADCBit,			VI00, KickUnscissoredTriangle

		  ;; if scissor is nessisary...
		  ibgtz						  ClipFlags,		ScissorTriangle
		  
KickUnscissoredTriangle:		  
		  div							  Q,					VF00[w], perspVtx[w]
		  PerspCorrectTex			  finalTex,			finalTex
		  mul.xyz					  perspVtx,			perspVtx, Q
		  MatrixMultiplyVertex3	  perspVtx,			cullmtx, perspVtx
		  b							  FinishLoop

AlwaysVtx:		 
		  ;; 
		  ;; always draw
		  ;;
		  GetADCBit			finalVtx, 0
		  TransformVertex	perspVtx, transmtx
		  div					Q,			 VF00[w], perspVtx[w]
		  mul.xyz			perspVtx, perspVtx, Q
		  PerspCorrectTex	finalTex, finalTex
		  b					FinishLoop

CullVtx:			 		  
		  ;; 
		  ;; cull
		  ;; 
		  TransformVertex	perspVtx,		transmtx
		  div					Q,					VF00[w], perspVtx[w]
		  mul.xyz			perspVtx, perspVtx, Q
		  PerspCorrectTex	finalTex, finalTex

		  TransformVertex	vtxcull,			cullmtx
		  clipw.xyz			vtxcull, vtxcull
		  fcand				vi01, 0x3FFFF
		  GetADCBit			finalVtx, 1

FinishLoop:		 		  
		  ;; 
		  ;; non-clipped...
		  ;;
		  ftoi4.xyz       finalVtx, perspVtx

		  ;;  save the vertex
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
		  sq.xyzw	 finalVtx,		  2(output[\&passcnt])
passcnt .assigna \&passcnt + 1
.aendw

		  ;; save colors
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
		  sq.xyzw	 finalColor,	1(output[\&passcnt])
passcnt .assigna \&passcnt + 1
.aendw

		  ;; save tex coords...
		  SaveTexCoordsOutput finalTex, 0, output

		  ;; increment outputs
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
		  iaddiu				output[\&passcnt],  output[\&passcnt], 3
passcnt .assigna \&passcnt + 1
.aendw

		  ;; increment input
		  iaddiu				input,  input, \&FieldWalker_LineCnt

		  ;; loop
		  isubiu				loopCount,		 loopCount, 1
		  ibne				loopCount,		 VI00,	loop\&MicrocodeName
.endm
		  
.macro EndMicrocode
		  ScissorCode
		  ScissorInterpolater \&ClipLayers
--exit
--endexit
End\&MicrocodeName:		 
.endm

.macro SaveTexCoordsClip regname, offset, base
local passcnt
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
realoffset .assigna (\&passcnt + offset)
		  sq.xy	 \regname[\&passcnt], \&realoffset(\base)
passcnt .assigna \&passcnt + 1
.aendw
.endm		  
		  
.macro SaveTexCoordsOutput regname, offset, base
local passcnt
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
		  sq.xyz	 \regname[\&passcnt],	\offset(\base[\&passcnt])
passcnt .assigna \&passcnt + 1
.aendw
.endm		  
		  
.macro GenOutputPointers
local passStateSize
local bufferSize
local passOutputSize		  		  		  		  
local passcnt
		  xtop				input
		  LoadIntVector4	0,		  input, loopCount, flagsC, flagsD, flagsE
		  xitop				bufferSize
		  iadd				output[\&KernelPasses], input, bufferSize

		  iadd				passOutputSize, loopCount, loopCount
		  iadd				passOutputSize, passOutputSize, loopCount

		  VectorLoad		passStateSizes[0], 1,		  VI00
		  VectorLoad		passStateSizes[1], 2,		  VI00
		  mtir				passStateSize[0], passStateSizes[0][x]
		  mtir				passStateSize[1], passStateSizes[0][y]
		  mtir				passStateSize[2], passStateSizes[0][z]
		  mtir				passStateSize[3], passStateSizes[0][w]
		  mtir				passStateSize[4], passStateSizes[1][x]
		  mtir				passStateSize[5], passStateSizes[1][y]
		  mtir				passStateSize[6], passStateSizes[1][z]
		  mtir				passStateSize[7], passStateSizes[1][w]
passcnt .assigna \&KernelPasses - 1
.awhile \&passcnt GE 0
priorpasscnt .assigna \&passcnt + 1
		  isub				output[\&passcnt], output[\&priorpasscnt], passOutputSize
		  isub				output[\&passcnt], output[\&passcnt], passStateSize[\&passcnt]
passcnt .assigna \&passcnt - 1
.aendw
.endm

.macro PrepOutputPointers
		  GenOutputPointers
local passStateSize
local passcnt
local eopBit
local passData
local field
local line
field .assignc "x"
line .assigna 0
		  iaddiu				passData, VI00, \&PassDataAddr
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
		  mtir		passStateSize, passStateSizes[\&line][\&field]
.aif "\&field" eq "w"
field .assignc "x"
line .assigna (\&line + 1)
.aelse
.aif "\&field" eq "z"
field .assignc "w"
.aendi
.aif "\&field" eq "y"
field .assignc "z"
.aendi
.aif "\&field" eq "x"
field .assignc "y"
.aendi
.aendi
		  CopyData			output[\&passcnt], passData, passStateSize
passcnt .assigna \&passcnt + 1
.aendw
		  
		  ; patch NLOOP for each gif tag & set EOP bit...
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses - 1
		  isw.x	 			loopCount, -1(output[\&passcnt])
passcnt .assigna \&passcnt + 1
.aendw
		  iaddiu				eopBit, VI00, 0x7fff
		  iaddiu				eopBit, eopBit, 0x0001
		  ior					eopBit, loopCount, eopBit
		  isw.x	 			eopBit, -1(output[\&passcnt])
.endm		  		  

.macro LoadTransformMatrix
		  MatrixLoad		transmtx, 3,	 VI00
.endm

.macro LoadCullMatrix
		  MatrixLoad		cullmtx, 7,		 VI00
.endm
		  
.macro LoadNormal dest
		  add.x	 \dest, zeros, line[\&Line][\&Field]
		  NextField
		  add.y	 \dest, zeros, line[\&Line][\&Field]
		  NextField
		  add.z	 \dest, zeros, line[\&Line][\&Field]
		  NextField
		  mr32.w	 dest, VF00
.endm

.macro LoadColor dest
ColorsAreCompressed .assigna 0		  
		  add.x	 \dest, zeros, line[\&Line][\&Field]
		  NextField
		  add.y	 \dest, zeros, line[\&Line][\&Field]
		  NextField
		  add.z	 \dest, zeros, line[\&Line][\&Field]
		  NextField
		  add.w	 \dest, zeros, line[\&Line][\&Field]
		  NextField
.endm
		  		  
.macro LoadColorCompr	dest
ColorsAreCompressed .assigna 1
		  move.xyzw \dest, line[0]
.endm

.macro LoadTexMatrices
local passcnt
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
		  LoadTexMatrix \&passcnt
passcnt .assigna \&passcnt + 1
.aendw
.endm		  		  

.macro LoadTexMatrix pass
local addr
addr .assigna ((\pass * 4) + \&TEX_MATRIX_START)
		  MatrixLoad	texmtx[\pass], \&addr, VI00
.endm
		  		  
.macro PerspCorrectTex dest, src
PerpsCorrectTexCoords .assigna 1
local passcnt
passcnt .assigna 0
.awhile \&passcnt LT \&KernelPasses
		  mul.xyz \dest[\&passcnt], \src[\&passcnt], q
passcnt .assigna \&passcnt + 1
.aendw
.endm
		  
.macro TransformVertex	dest, mtx
		  MatrixMultiplyVertex	  \dest, \mtx, line[0]
.endm

.macro LoadTexCoords dest, pass, depth
		  LoadTexCoord\depth \dest[\pass]
.endm

.macro LoadTexCoord1 dest
		  max.x	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  move.yzw \dest, VF00
.endm
.macro LoadTexCoord2 dest
		  max.x	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  max.y	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  move.zw \dest, VF00
.endm
.macro LoadTexCoord3 dest
		  max.x	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  max.y	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  max.z	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  move.w \dest, VF00
.endm
.macro LoadTexCoord4 dest
		  max.x	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  max.y	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  max.z	  \dest, zeros, line[\&Line][\&Field]
		  NextField
		  max.w	  \dest, zeros, line[\&Line][\&Field]
		  NextField
.endm

.macro SkipTexCoords pass
		  addiu	 output[\pass], output[\pass], 1
.endm
		  
.macro TransformTexCoords dest, pass, depth
		  TransformTexCoordsL \dest, \pass, \depth, line
.endm

.macro TransformTexCoordsL dest, pass, depth, line
		  MatrixMultiplyTexCoord\depth	  \dest[\pass], texmtx[\pass], line
		  mr32.z	 \dest[\pass], VF00
.endm

.macro LoadLines
local passcnt
passcnt .assigna 0
.awhile \&passcnt LT \&MAX_LINES
		  VectorLoad	  line[\&passcnt], \&passcnt, input
passcnt .assigna \&passcnt + 1
.aendw
.endm

.macro MatrixMultiplyTexCoord1 dest, matrix, line
		  mul     acc,   \matrix[3], VF00[w]
		  madd    acc,   \matrix[0], \line[\&Line][\&Field]
		  NextField
		  madd    acc,   \matrix[1], VF00[y]
		  madd	 \dest, \matrix[2], VF00[z]
.endm

.macro MatrixMultiplyTexCoord2 dest, matrix, line
		  mul     acc,   \matrix[3], VF00[w]
		  madd    acc,   \matrix[0], \line[\&Line][\&Field]
		  NextField
		  madd    acc,   \matrix[1], \line[\&Line][\&Field]
		  NextField
		  madd	 \dest, \matrix[2], VF00[z]
.endm
		  
.macro MatrixMultiplyTexCoord3 dest, matrix, line
		  mul     acc,   \matrix[3], VF00[w]
		  madd    acc,   \matrix[0], \line[\&Line][\&Field]
		  NextField
		  madd    acc,   \matrix[1], \line[\&Line][\&Field]
		  NextField
		  madd	 \dest, \matrix[2], \line[\&Line][\&Field]
		  NextField
.endm
		  
.macro MatrixMultiplyTexCoord4 dest, matrix, line
		  mul     acc,   \matrix[0], \line[\&Line][\&Field]
		  NextField
		  madd    acc,   \matrix[1], \line[\&Line][\&Field]
		  NextField
		  madd	 acc,	  \matrix[2], \line[\&Line][\&Field]
		  NextField
		  madd    \dest, \matrix[3], \line[\&Line][\&Field]
		  NextField
.endm

.macro ClipVertex regoptnum
;VtxWReady .assigna 3
.endm

.macro CullVertex dest
;VtxWReady .assigna 2		  
.endm

.macro AlwaysDrawVertex dest
;VtxWReady .assigna 1		  
.endm

.macro FogVertex dest, src
local fogTemp
VtxWValid .assigna 1
		  mul.w	  			ACC, VF00, fogParams[z]
		  madd.w				fogTemp, fogParams, \src[z]
		  mini.w				fogTemp, fogTemp, fogMax
		  max.w				\dest, fogTemp, VF00[x]
.endm
		  
;--------------------------------------------------------------------
; BOnFlagClear- check "flags" for immediate "value" and if eq0
; branch to "label"
;--------------------------------------------------------------------
.macro	BOnFlagClear flags, value, label, tempvar=flagtemp
	iaddiu		 \tempvar, VI00, \value
	iand			 \tempvar, \flags, \tempvar
	ibeq			 \tempvar, VI00, \label
.endm
		  
;--------------------------------------------------------------------
; BOnFlagSet - check "flags" for immediate "value" and if ne0
; branch to "label"
;--------------------------------------------------------------------
.macro	BOnFlagSet flags, value, label, tempvar=flagtemp
	iaddiu		 \tempvar, VI00, \value
	iand			 \tempvar, \flags, \tempvar
	ibne			 \tempvar, VI00, \label
.endm

		  		  
