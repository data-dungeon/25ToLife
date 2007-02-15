;;; File					High.i
;;; Purpose				high level macros for VCL work
;;; Notes				Currently, this contains the following main macros
;;;						Clipping -
;;;										StandardClip
;;;										ScissorAgainstPlane
;;;
;;;						Matrices -
;;;										MakeXRotMatrix
;;;										MakeYRotMatrix
;;;										MakeZRotMatrix
;;;										MakeXYZRotMatrix
;;;						
;;;						Subroutines -
;;;									   HighLevelSubroutines
;;;									   ScissorInterpolater layers
;;;

;;;  TODO:  circular clip buffer?
;;;  TODO;  copy first vertex before clip operation, instead of after
	
.include "Low.i"

.macro LoadClipVtx work, dest, layers
local passcnt
		  LoadIVector4		\work, \dest[Vtx]
passcnt .assigna 0
.awhile \&passcnt LT \layers
		  LoadIVector4		\work, \dest[Tex][\&passcnt]
passcnt .assigna \&passcnt + 1
.aendw
.endm

.macro SaveClipVtx work, src, layers
local passcnt
		  SaveIVector4		\work, \src[Vtx]
passcnt .assigna 0
.awhile \&passcnt LT \layers
		  SaveIVector4		\work, \src[Tex][\&passcnt]
passcnt .assigna \&passcnt + 1
.aendw
.endm

.macro MoveClipVtx curr, next, layers
	move					\curr[Vtx],						\next[Vtx]
passcnt .assigna 0
.awhile \&passcnt LT \layers
	move					\curr[Tex][\&passcnt],		\next[Tex][\&passcnt]
passcnt .assigna (\&passcnt + 1)
.aendw
.endm		  

.macro ClipInterpolate new, curr, next, planeSign, rotVec, layers
local Temp1, Temp2, Difference, Answer
	;; temp1 = cv - (sign * cv[w])
	mul.w					Temp1,		\curr[Vtx],		\planeSign[x] 
	sub					Temp1,		\curr[Vtx],		Temp1[w]

	;; temp2 = nv - (sign * nv[w])
	mul.w					Temp2,		\next[Vtx],		planeSign[x]
	sub					Temp2,		\next[Vtx],		Temp2[w]

	;; rotate the verts around until the field we are checking against is in x
	mul.x					acc,			\rotVec[0],		Temp1[x]
	madd.x				acc,			\rotVec[1],		Temp1[y]
	madd.x				Temp1,		\rotVec[2],		Temp1[z]
	mul.x					acc,			\rotVec[0],		Temp2[x]
	madd.x				acc,			\rotVec[1],		Temp2[y]
	madd.x				Temp2,		\rotVec[2],		Temp2[z]

	;; calc diff between current & next vtx
	sub.x					Difference,	Temp2,			Temp1

	;; divide current by ratio between current and previous
	;; and get abs version
	;; q = (current.x - current.w * cplane) / (next.x - next.w * cplane)
	div					Q,				Temp1[x],		Difference[x]
	add.x					Answer,		VF00,				Q
	abs.x					Answer,		Answer			; ans = |(c-cw)/((n-nw)+(c-cw))|

	;; newvertex = ((nextVertex - currentVertex) * ratio) + currentvertex
	sub					\new[Vtx],	\next[Vtx],		\curr[Vtx]
	mul					\new[Vtx],	\new[Vtx],		Answer[x]
	add					\new[Vtx],	\new[Vtx],		\curr[Vtx]

passcnt .assigna 0
.awhile \&passcnt LT \layers
	sub					\new[Tex][\&passcnt],		\next[Tex][\&passcnt], \curr[Tex][\&passcnt]
	mul					\new[Tex][\&passcnt],		\new[Tex][\&passcnt],  Answer[x]
	add					\new[Tex][\&passcnt],		\new[Tex][\&passcnt],  \curr[Tex][\&passcnt]
passcnt .assigna (\&passcnt + 1)
.aendw
.endm
		
.macro ScissorInterpolation work, curr, next, rotVec, layers, curClipFlg, nxtClipFlg, planeSign, newCount
local ClipFlag, New
	;;  next to current
	MoveClipVtx			\curr,		\next,			\layers
	LoadClipVtx			\work[0],	\next,			\layers
		  
	;;  determine if both are on opposite sides of plane
	clipw.xyz			\curr[Vtx],	\curr[Vtx]
	clipw.xyz			\next[Vtx],	\next[Vtx]
	fcget					vi01

	;; if result is 0, then the first is 'inside'
	iand					ClipFlag,	vi01,				\curClipFlg
	ibeq					ClipFlag,	vi00,				CurrInside\@

CurrOutside\@:
	iand					ClipFlag,	vi01,				\nxtClipFlg
	ibeq					ClipFlag,	vi00,				CONextIn\@

CONextOut\@:
	;; both are outside.. we are done...
	b						ScissorInterpolationEnd\@

CONextIn\@:
	;; first is outside, second is inside, find intersection
	;; and store in work buffer
	ClipInterpolate	New, \curr, \next, \planeSign, \rotVec, \layers
	SaveClipVtx			\work[1],	New,				\layers
	iaddiu				\newCount,	\newCount,		1
	b						ScissorInterpolationEnd\@

CurrInside\@:
	SaveClipVtx			\work[1],	\curr,			\layers
	iand					ClipFlag,	vi01,				\nxtClipFlg
	ibeq					ClipFlag,	vi00,				CINextIn\@

CINextOut\@:
	;; first is inside, second is outside, find intersection
	ClipInterpolate	New, \next, \curr, \planeSign, \rotVec, \layers
	SaveClipVtx			\work[1],	New,				\layers
	iaddiu				\newCount,	\newCount,		2
	b						ScissorInterpolationEnd\@
CINextIn\@:
	;; both are inside, no interp
	iaddiu				\newCount,	\newCount,		1

ScissorInterpolationEnd\@:		    
.endm

;;; see StandardClip for usage...  It handles the scissoring against a single plane
;;; inputs are
;;; plane - X, Y, or Z
;;; sign - -1 or 1
;;; lastplane - bool is this the last plane we will be clipping against (removes a little prep code)
;;; rest same as StandardClip
.macro ScissorAgainstPlane plane, sign, lastplane, numVerts, layers, clipbuf1, clipbuf2, emptyLabel
local vertCheck
	;; get buffer pointers
	iaddiu				Work[0],		\clipbuf1,		0
	iaddiu				Work[1],		\clipbuf2,		0

	;; save a couple of ints...
	PushIntVector4		\clipbuf1, \clipbuf2
	
	;; build a bunch of variables based on the planes we are working against.
.aif "\plane" eq "X"
	One.x					RotVec[0]
	Zero.x				RotVec[1]
	Zero.x				RotVec[2]
.aif \sign eq -1
	iaddiu				CurClipFlg,	VI00,				0x080
	iaddiu				NxtClipFlg,	VI00,				0x002
	NegOne.x				PlaneSign
.aelse
	iaddiu				CurClipFlg,	VI00,				0x040
	iaddiu				NxtClipFlg,	VI00,				0x001
	One.x					PlaneSign
.aendi
.aendi
.aif "\plane" eq "Y"
	Zero.x				RotVec[0]
	One.x					RotVec[1]
	Zero.x				RotVec[2]
.aif \sign eq -1
	iaddiu				CurClipFlg,	VI00,				0x200
	iaddiu				NxtClipFlg,	VI00,				0x008
	NegOne.x				PlaneSign
.aelse
	iaddiu				CurClipFlg,	VI00,				0x100
	iaddiu				NxtClipFlg,	VI00,				0x004
	One.x					PlaneSign
.aendi
.aendi
.aif "\plane" eq "Z"
	Zero.x				RotVec[0]
	Zero.x				RotVec[1]
	One.x					RotVec[2]
.aif \sign eq -1
	iaddiu				CurClipFlg,	VI00,				0x800
	iaddiu				NxtClipFlg,	VI00,				0x020
	NegOne.x				PlaneSign
.aelse
	iaddiu				CurClipFlg,	VI00,				0x400
	iaddiu				NxtClipFlg,	VI00,				0x010
	One.x					PlaneSign
.aendi
.aendi

	;; zero out the new vertex count
	iaddiu				NewCount,	VI00,				0

	;; do the clipping
	LoadClipVtx			Work[0],		Next,			\layers
ScissorLoop\@:
--barrier
	isubiu				\numVerts,	\numVerts,		1
--barrier
	bal					ScissorReturn, _ScissorInterpolation\layers
	ibgtz					\numVerts,	ScissorLoop\@

	;; copy newcount back over vtx count..
	iaddiu				\numVerts,	NewCount,		0

	;; load and swap the clip buffer pointers
	PopIntVector4		\clipbuf2, \clipbuf1

	;; if its empty, jump to the empty label
.aif "\emptyLabel" ne ""
	;; Instead of just deciding based on 0, 
	;; see if there are enough verts left to care... (somehow we are ending up with
	;; one or two verts after clipping?  not quite sure how)
	isubiu				vertCheck,  \numVerts,		2
	iblez					vertCheck,  \emptyLabel
.aendi

	;; copy first vtx to end of fan...
.aif \lastplane ne 1
CLIPVTXSIZE				.equ			(1+\layers)
	iaddiu				Work[0],		\clipbuf2,		0
	StaticCopyData		Work[1],		0,					\clipbuf1,			  0, CLIPVTXSIZE
.aendi
.endm

;;; clip data back and forth between the clip buffers against the specificed
;;; planes until the results is achieved.  For an odd # of planes, result
;;; will be in clipbuf2, for even, in clipbuf1.  Will change clipbuf1 to point
;;; and the output data (it swaps the buffer pointers back and forth)
;;; Works on n layers (sets of UVS or colors..).  Expects data in clip space
;;; xyzw, layer0, layer1, layer2, layer3, ...
;;; numVerts is updated to contain the new # of verts.  you must have an
;;; edgelist in clipbuf1 (ie, fan with first vtx repeated at end)
;;; planes can either be a variable or a combination of "xXyYzZ" where
;;; an uppercase letter represents the positive plane, and lowercase is
;;; the negative plane.  If its a variable, it should have a letter besides
;;; the plane letters in it.. (this isnt implimented yet)
;;; this uses a bunch of registers, save what you can before calling...
;;; if numVerts reaches 0, jumps to EmptyLabel, if its passed
;;; This uses 1 QW in the low.i stack.
.macro StandardClip	numVerts, clipbuf1, clipbuf2, layers, planes, emptyLabel
	;; order is -Z, +Z, -X, +X, -Y, +Y
	ScissorAgainstPlane				Z, -1, 0, \numVerts, \layers, \clipbuf1, \clipbuf2, \emptyLabel
	ScissorAgainstPlane				Z, +1, 0, \numVerts, \layers, \clipbuf1, \clipbuf2, \emptyLabel
	ScissorAgainstPlane				X, -1, 0, \numVerts, \layers, \clipbuf1, \clipbuf2, \emptyLabel
	ScissorAgainstPlane				X, +1, 0, \numVerts, \layers, \clipbuf1, \clipbuf2, \emptyLabel
	ScissorAgainstPlane				Y, -1, 0, \numVerts, \layers, \clipbuf1, \clipbuf2, \emptyLabel
	ScissorAgainstPlane				Y, +1, 1, \numVerts, \layers, \clipbuf1, \clipbuf2, \emptyLabel
.endm
	
.macro MakeXRotMatrix				dest,				src,				  srcfield,	  slow=0
	CopyAcrossVector	SinCosIn,	\src[\srcfield]
	;; take sin/cos
.aif slow eq 1
	_CSCS					SinCosIn,	SinCosIn
	bal					CCCCRet,		sub_CCCC
.aelse
	CSCS					SinCosOut,	SinCosIn
.aendi
	MakeXRotMatrixCSCS				\dest,			SinCosOut
.endm

.macro MakeYRotMatrix				dest,				src,				  srcfield,	  slow=0
	;; duplicate across the vector
	CopyAcrossVector	SinCosIn,	\src[\srcfield]
	;; take sin/cos
.aif slow eq 1
	_CSSC					SinCosIn,	SinCosIn
	bal					CCCCRet,		sub_CCCC
.aelse
	CSSC					SinCosOut,	SinCosIn
.aendi
	MakeYRotMatrixCSSC				\dest,			SinCosOut
.endm

.macro MakeZRotMatrix				dest,				src,				  srcfield,	  slow=0
	;; duplicate across the vector
	CopyAcrossVector	SinCosIn,	\src[\srcfield]
	;; take sin/cos
.aif slow eq 1
	_SCSC					SinCosIn,	SinCosIn
	bal					CCCCRet,		sub_CCCC
.aelse
	SCSC					SinCosOut,	SinCosIn
.aendi
	MakeZRotMatrixSCSC				\dest,			SinCosOut
.endm

.macro MakeXYZRotMatrix				dest,				src,				  xsrcfield, ysrcfield, zsrcfield, slow=0
local rx,ry,rz,temp
	;; for now we do this the easy way...
	MakeXRotMatrix		rx,			\src,				\xsrcfield,		  slow
	MakeYRotMatrix		ry,			\src,				\ysrcfield,		  slow
	MakeZRotMatrix		rz,			\src,				\zsrcfield,		  slow
	MultiplyMatrix		temp,			rx,				ry
	MultiplyMatrix		\dest,		temp,				rz
.endm
	
.macro MakeYXZRotMatrix				dest,				src,				  xsrcfield, ysrcfield, zsrcfield, slow=0
local rx,ry,rz,temp
	;; for now we do this the easy way...
	MakeXRotMatrix		rx,			\src,				\xsrcfield,		  slow
	MakeYRotMatrix		ry,			\src,				\ysrcfield,		  slow
	MakeZRotMatrix		rz,			\src,				\zsrcfield,		  slow
	MultiplyMatrix		temp,			ry,				rx
	MultiplyMatrix		\dest,		temp,				rz
.endm
	
.macro MakeZXYRotMatrix				dest,				src,				  xsrcfield, ysrcfield, zsrcfield, slow=0
local rx,ry,rz,temp
	;; for now we do this the easy way...
	MakeXRotMatrix		rx,			\src,				\xsrcfield,		  slow
	MakeYRotMatrix		ry,			\src,				\ysrcfield,		  slow
	MakeZRotMatrix		rz,			\src,				\zsrcfield,		  slow
	MultiplyMatrix		temp,			ry,				rx
	MultiplyMatrix		\dest,		temp,				rz
.endm
	
;;;
;;; High level subroutines... used currently for clipping & some matrix stuff sometimes (inlcudes the low level ones)
;;;
.macro HighLevelSubroutines
	LowLevelSubroutines
.endm	

.macro ScissorInterpolater layers
_ScissorInterpolation\layers:		
	ScissorInterpolation Work, Curr, Next, RotVec, \layers, CurClipFlg, NxtClipFlg, PlaneSign, NewCount
	jr						ScissorReturn
.endm
		
