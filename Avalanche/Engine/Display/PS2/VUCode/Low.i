.include "MacroMagic.i"
.include "PsuedoInstr.i"
.include "Assert.i"
	
;;; now on to the real work
.macro   TransformVertex result,matrix,vertex
   add					acc,			\matrix[3],		VF00[x]
   madd					acc,			\matrix[0],		\vertex[x]
   madd					acc,			\matrix[1],		\vertex[y]
   madd					\result,		\matrix[2],		\vertex[z]
.endm

.macro   TransformVertexF result,matrix,vertex
   mul					acc,			\matrix[0],		\vertex[x]
   madd					acc,			\matrix[1],		\vertex[y]
   madd					acc,			\matrix[2],		\vertex[z]
   madd					\result,		\matrix[3],		\vertex[w]
.endm

.macro	TransformVertex3 result, matrix, vertex
   add			 		acc,        \matrix[3], 	VF00[x]
   madd         		acc,        \matrix[0], 	\vertex[x]
   madd         		acc,        \matrix[1],		\vertex[y]
   madd.xyz     		\result, 	\matrix[2],		\vertex[z]
.endm		  

.macro   TransformVector result,matrix,vector
   mul			 		acc,			\matrix[0],		\vector[x]
   madd			 		acc,			\matrix[1],		\vector[y]
   madd			 		\result,		\matrix[2],		\vector[z]
.endm

.macro MakeIdentMatrix matrix
	move					Orient[3],	vf00
	mr32					Orient[2],	Orient[3]
	mr32					Orient[1],	Orient[2]
	mr32					Orient[0],	Orient[1]
.endm	

.macro LengthVector3 out, in
local ones, sum, squares
	mul					squares,		\in,				\in
	One					ones
	add					ACC,			VF00,				squares[x]
	madd					ACC,			ones,				squares[y]
	madd					sum,			ones,				squares[z]
	sqrt					Q,				sum[x]
.aif "\out" ne "Q"
.aif .INSTR("\out","w") ne -1
	Zero					zeros
	add					\out,			zeros,			Q
.aelse
	add					\out,			vf00,				Q
.aendi
.aendi
.endm	
	
.macro RLengthVector3 out, in
local ones, sum, squares
	mul.xyz				squares,		\in,				\in
	One					ones
	add					ACC,			VF00,				squares[x]
	madd					ACC,			ones,				squares[y]
	madd					sum,			ones,				squares[z]
	rsqrt					Q,				VF00[w],			sum[x]
.aif "\out" ne "Q"
.aif .INSTR("\out","w") ne -1
	Zero					zeros
	add					\out,			zeros,			Q
.aelse
	add					\out,			vf00,				Q
.aendi
.aendi
.endm	
	
.macro NormalizeVector3 out, in
	RLengthVector3		Q,				\in
	mul.xyz				\out,			\in,				Q
.endm
	
.macro RotateIntoPos					dest,				src,				  destfields,  srcfields
local t1
.aif "\destfields" eq "\srcfields"
.aif "\dest" ne "\src"
	move.\destfields	\dest,		\src
.aendi
.aelse
	MR32Fields \srcfields
.aif "\&mr32Fields" eq "\destfields"
	mr32.\destfields	\dest,		\src
.aelse
	mr32.\&mr32Fields	t1,			\src
	MR32Fields \&mr32Fields
.aif "\&mr32Fields" eq "\destfields"
	mr32.\destfields	\dest,		t1
.aelse
	mr32.\&mr32Fields	t1,			t1
	mr32.\destfields	\dest,		t1
.aendi	
.aendi
.aendi
.endm

;;; do 4 cosines..  sorry about the variable names, converted
;;; from someone elses code..  Doesnt deal with out of range
;;; values!  use CCCC if you need that...  This is just the
;;; taylor series.. (right now this isnt written)
.macro CCCCFast		dest,			src
	CCCC					\dest,		\src
.endm

;;; do 4 cosines... sorry about the variable names, converted
;;; from someone elses code..  use:
;;; CCCC.xyz	out, in
;;; or
;;; CCCC 		out, in
.macro CCCC				arg1,			arg2,				arg3
.aif "\arg3" eq ""
	abs					t1,			\arg2
	One					t2
	loi					0xbe22f983
	mul					ACC,			t1,				I
	loi					0x4b400000
	msub					ACC,			t2,				I
	madd					ACC,			t2,				I
	loi					0xbe22f983
	msub					ACC,			t1,				I
	loi					0.5
	msub					t1,			t2,				I
	abs					t1,			t1
	loi					0.25
	sub					t1,			t1,				I
	mul					t2,			t1,				t1
	loi					0xc2992661
	mul					t6,			t1,				I
	loi					0xc2255de0
	mul					t4,			t1,				I
	loi					0x42a33457
	mul					t5,			t1,				I
	mul					t3,			t2,				t2
	mul					t6,			t6,				t2
	mul					ACC,			t4,				t2
	loi					0x421ed7b7
	mul					t4,			t1,				I
	mul					t2,			t3,				t3
	madd					ACC,			t6,				t3
	madd					ACC,			t5,				t3
	loi					0x40c90fda
	madd					ACC,			t1,				I
	madd					\arg1,		t4,				t2
.aelse
	abs\arg1				t1,			\arg3
	One\arg1				t2
	loi					0xbe22f983
	mul\arg1				ACC,			t1,				I
	loi					0x4b400000
	msub\arg1			ACC,			t2,				I
	madd\arg1			ACC,			t2,				I
	loi					0xbe22f983
	msub\arg1			ACC,			t1,				I
	loi					0.5
	msub\arg1			t1,			t2,				I
	abs\arg1				t1,			t1
	loi					0.25
	sub\arg1				t1,			t1,				I
	mul\arg1				t2,			t1,				t1
	loi					0xc2992661
	mul\arg1				t6,			t1,				I
	loi					0xc2255de0
	mul\arg1				t4,			t1,				I
	loi					0x42a33457
	mul\arg1				t5,			t1,				I
	mul\arg1				t3,			t2,				t2
	mul\arg1				t6,			t6,				t2
	mul\arg1				ACC,			t4,				t2
	loi					0x421ed7b7
	mul\arg1				t4,			t1,				I
	mul\arg1				t2,			t3,				t3
	madd\arg1			ACC,			t6,				t3
	madd\arg1			ACC,			t5,				t3
	loi					0x40c90fda
	madd\arg1			ACC,			t1,				I
	madd\arg1			\arg2,		t4,				t2
.aendi
.endm

.macro _SSSS			dest,			src
	loi 0x3fc90fdb
	sub.xyzw				\dest,		\src,				I
.endm	
.macro _CSCS			dest,			src
	loi 0x3fc90fdb
	sub.yw				\dest,		\src,				I
	move.xz				\dest,		\src
.endm	
.macro _SCSC			dest,			src
	loi 0x3fc90fdb
	sub.xz				\dest,		\src,				I
	move.yw				\dest,		\src
.endm	
.macro _SSCC			dest,			src
	loi 0x3fc90fdb
	sub.xy				\dest,		\src,				I
	move.zw				\dest,		\src
.endm	
.macro _CCSS			dest,			src
	loi 0x3fc90fdb
	sub.zw				\dest,		\src,				I
	move.xy				\dest,		\src
.endm	
.macro _SCCS			dest,			src
	loi 0x3fc90fdb
	sub.xw				\dest,		\src,				I
	move.yz				\dest,		\src
.endm	
.macro _CSSC			dest,			src
	loi 0x3fc90fdb
	sub.yz				\dest,		\src,				I
	move.xw				\dest,		\src
.endm	

.macro SSSS				dest,			src
local t
	_SSSS					t,				\src
	CCCC					\dest,		t
.endm
.macro CSCS				dest,			src
local t
	_CSCS					t,				\src
	CCCC					\dest,		t
.endm
.macro SCSC				dest,			src
local t
	_SCSC					t,				\src
	CCCC					\dest,		t
.endm
.macro CSSC				dest,			src
local t
	_CSSC					t,				\src
	CCCC					\dest,		t
.endm
.macro SCCS				dest,			src
local t
	_SCCS					t,				\src
	CCCC					\dest,		t
.endm
.macro SSCC				dest,			src
local t
	_SSCC					t,				\src
	CCCC					\dest,		t
.endm
.macro CCSS				dest,			src
local t
	_CCSS					t,				\src
	CCCC					\dest,		t
.endm

.macro neg				arg1, arg2, arg3
.aif "\arg3" eq ""
	Zero					zero
	sub.xyzw				\arg1,	  zero,	 \arg2
.aelse
.aif .INSTR("\arg1","w") ne -1
local zero
	Zero					zero
	sub\arg1				\arg2,		zero,				\arg3
.aelse
	sub\arg1				\arg2,		vf00,				\arg3
.aendi
.aendi
.endm
	
.macro Zero				arg1,       arg2
.aif "\arg2" eq ""
	mini.xyzw			\arg1,		vf00,				vf00[x]
.aelse
	mini\arg1			\arg2,	   vf00,				vf00[x]
.aendi
.endm
.macro One				arg1,			arg2
.aif "\arg2" eq ""
	max.xyzw				\arg1,	   vf00,				vf00[w]
.aelse
	max\arg1				\arg2,		vf00,				vf00[w]
.aendi
.endm
.macro NegOne			arg1,	  arg2
.aif "\arg2" eq ""
	neg					\arg1,	  vf00[w]
.aelse
	neg\arg1			\arg2,	  vf00[w]
.aendi
.endm
.macro Ident			dest,			destfields=xyzw
	move.\destfields	\dest,		vf00
.endm
	
.macro ClampAtZero	dest,			src
	max					\dest,		\src,				vf00[x]
.endm
.macro ClampAtOne		dest,			src
	mini					\dest,		\src,				vf00[w]
.endm
			
.macro StaticCopyData dest, destoffset, src, srcoffset, size
local copytemp
passcnt .assigna 0
.awhile \&passcnt LT \size
finalsrcoffset .assigna (\&passcnt + \srcoffset)
finaldestoffset .assigna (\&passcnt + \destoffset)		  
		  lq copytemp, \&finalsrcoffset(\src)
		  sq copytemp, \&finaldestoffset(\dest)
passcnt .assigna (\&passcnt + 1)
.aendw
.endm
		  
.macro CopyData dest, src, size, mindata=1
local copytemp
CopyDataLoop\@:
;--loopCS \mindata, 0
		  lqi copytemp, (\src++)
		  sqi copytemp, (\dest++)
		  isubiu \size, \size, 1
		  ibne \size, vi0, CopyDataLoop\@
.endm
		  		  		  		  		  		  
.macro CopyAcrossVector				dest,				src,				  destfields=xyzw
.aif .INSTR(\destfields, w, 0) eq -1
	add.\destfields	\dest,		vf00,				\src
.aelse
.aif "\destfields" eq "w"
	mul.\destfields	\dest,		vf00,				\src
.aelse
	One ones
	mul.\destfields	\dest,		ones,				\src
.aendi
.aendi
.endm

.macro MakeXRotMatrixCSCS			dest,				src
	;; build matrix...
	;; row[0] = 1,0,0,0
	One.x					\dest[0]
	Zero.yzw				\dest[0]
	;; row[1] = 0, c, s, 0
	Zero.xw				\dest[1]
	mr32.yz				\dest[1],	\src
	;; row[2] = 0, -s, c, 0
	Zero.xw				\dest[2]
	move.z				\dest[2],	\src
	sub.y					\dest[2],	vf00,				\src
	;; row[3] = 0,0,0,1
	Ident					\dest[3]
.endm	
		
.macro MakeYRotMatrixCSSC			dest,				src
	;; build matrix
	;; row[0] = c,0,-s,0
	Zero.yw				\dest[0]
	move.x				\dest[0],	\src
	sub.z					\dest[0],	vf00,				\src
	;; row[1] = 0,1,0,0
	One.y					\dest[1]
	Zero.xzw				\dest[1]
	;; row[2] = s,0,c,0
	Zero.yw				\dest[2]
	mr32.xz				\dest[2],	\src
	;; row[3] = 0,0,0,1
	Ident					\dest[3]
.endm	

.macro PreMultXRotMatrixCS3		dest,				srcMtx,			  srcAng
.aif "\dest" eq "\srcMtx"
local temp
dst .assignc "temp"
.aelse
dst .assignc "\dest"
.aendi

	; srcAng holds cos value in y, sin value in z

	local negAng
	;; row[0] = 1,0,0,0
	;; row[1] = 0,c,s,0
	;; row[2] = 0,-s,c,0
	;; row[3] = 0,0,0,1

	;; build the -s we will need
	neg.yz				negAng,		srcAng
	
	;; accumluate the matrix
   move					\dest[0],	\srcMtx[0]
   mul					acc,        \srcMtx[1],		\srcAng[y]
   madd					\&dst[1],   \srcMtx[2],		\srcAng[z]
   mul					acc,        \srcMtx[1],		negAng[z]
   madd					\dest[2],   \srcMtx[2],		\srcAng[y]
	Ident					\dest[3]
.aif "\dest" eq "\srcMtx"
	move					\dest[1],	\&dst[1]
.aendi
.endm

.macro PreMultZRotMatrixCS3		dest,				srcMtx,			  srcAng
.aif "\dest" eq "\srcMtx"
local temp
dst .assignc "temp"
.aelse
dst .assignc "\dest"
.aendi

	local negAng
	;; row[0] = c,s,0,0
	;; row[1] = -s,c,0,0
	;; row[2] = 0,0,1,0
	;; row[3] = 0,0,0,1

	;; build the -s we will need
	neg.xy				negAng,		srcAng
	
	;; accumluate the matrix
   mul					acc,        \srcMtx[0],		\srcAng[x]
   madd					\&dst[0],   \srcMtx[1],		\srcAng[y]
   mul					acc,        \srcMtx[0],		negAng[y]
   madd					\dest[1],   \srcMtx[1],		\srcAng[x]
   move					\dest[2],	\srcMtx[2]
	Ident					\dest[3]
.aif "\dest" eq "\srcMtx"
	move					\dest[0],	\&dst[0]
.aendi
.endm

.macro PreMultYRotMatrixCzS3		dest,				srcMtx,			  srcAng
local temp
	MakeYRotMatrixCzS					temp,				\srcAng
	MultiplyMatrix33					\dest,			temp,				  \srcMtx
.endm
	
.macro MakeYRotMatrixCzS			dest,				src
	;; build matrix
	;; row[0] = c,0,-s,0
	Zero.yw				\dest[0]
	move.x				\dest[0],	\src
	sub.z					\dest[0],	vf00,				\src
	;; row[1] = 0,1,0,0
	One.y					\dest[1]
	Zero.xzw				\dest[1]
	;; row[2] = s,0,c,0
	Zero.yw				\dest[2]
	CopyAcrossVector	\dest[2],	\src[x],			z
	CopyAcrossVector	\dest[2],	\src[z],			x
	;; row[3] = 0,0,0,1
	Ident					\dest[3]
.endm	
		
.macro MakeZRotMatrixCS				dest,				src
	;; build matrix
	;; row[0] = c,s,0,0
	Zero.zw				\dest[0]
	move.xy				\dest[0],	\src
	;; row[1] = -s,c,0,0
	Zero.zw				\dest[1]
	add.y					\dest[1],	vf00,				\src[x]
	sub.x					\dest[1],	vf00,				\src[y]	
	;; row[2] = 0,0,1,0
	mr32					\dest[2],	vf00
	;; row[3] = 0,0,0,1
	Ident					\dest[3]
.endm
	
.macro MakeZRotMatrixSCSC			dest,				src
	;; build matrix
	;; row[0] = c,s,0,0
	Zero.zw				\dest[0]
	mr32.xy				\dest[0],	\src
	;; row[1] = -s,c,0,0
	Zero.zw				\dest[1]
	move.y				\dest[1],	\src
	sub.x					\dest[1],	vf00,				\src
	;; row[2] = 0,0,1,0
	mr32					\dest[2],	vf00
	;; row[3] = 0,0,0,1
	Ident					\dest[3]
.endm

	;;;;;;;;;;;;;; 
	;; inputs:
	;;						out - output variable
	;;						v - input vector
	;;						layout - output layout, can be
	;;										SSCC  (sine, sine, cosine, cosine)
	;;										CCSS
	;;										SCCS
	;;										CSSC
	;;										SCSC
	;;										CSCS
	;;						opp - field in vector to be considered opposite the angle
	;;						adj - field in vector to be considered adjacent to the angle
	;;						flds - both opp and adj concatinated - couldnt figure it out in macros
	;;						length - optional variable to hold the length of the hypotinuse
	;;						rlength - optional variable to hold 1 / length
	;;						zeroVal - optional variable for zero length hypotinuse flag...
	;;;;;;;;;;;;;; 
	;; sample
	;;;;;;;;;;;;;; 
	;; given:
	;;  out, v, SCSC, x, y, xy
	;; calculates:
	;;              /|
	;;             / |
	;;				  /  |
	;;          h/   |v[x]
	;;          /    |
	;;         /     |
	;;        /a_____|
	;;				v[y]
	;; out[x] = out[z] = sine(a)
	;; out[y] = out[w] = cos(a)
	;;
	;; if zeroVal is specified, it will be written to with a 0 or 0x08 with
	;; 0x08 meaning that h is zero length.
	;;
	;; if length is specified, its w will contain the length of h
	;; if rlength is specified, its w will contain 1/length of h
	;;
.macro SinCosOfVector				out,				v,					  layout,  opp,  adj,  flds, length,  rlength, zeroVal
	ercpr					P,				\v[\adj] ; this is 1/x
	mfp.w					temp,			P
	SinCosOfVectorR					\out,				\v,				  temp, \layout, \opp, \adj, \flds, \length, \rlength,	  \zeroVal
.endm
	
	;; This is a little faster if you already have the reciprical of v[adj]... must be in w.  just pass it in... 
.macro SinCosOfVectorR				out,				v,					  radj, layout,  opp,  adj,  flds, length, rlength, zeroVal
local tan, temp, zflag
	mul.w					tan,			\radj,			\v[\opp] ; now tan[w] = o/a
	
	;; now we can work on the cosine... (we have about 12 cycles to get it
	;; in, ideally) c=a/h  so we need the hyp...
	;; h = sqrt(o^2 + a^2)  Also we need to check for 0 length...
	;; the branch for zero length has been moved below to keep VCL from optimizing
	;; poorly...
.aif "\zeroVal" ne ""
	iaddiu				zflag,		vi00,				0x08
.aendi
	mul.\flds			squares,		v,					v
	add.\opp				sqsum,		squares,			squares[\adj]
.aif "\zeroVal" ne ""
	fmand					\zeroVal,	zflag
.aendi
	
	;; now for q = a/sqrt(sqsum)
.aif "\length" ne ""
	sqrt					Q,				sqsum[\opp]
	mul.w					\length,		vf00,				Q
.aif "\rlength" ne ""
	div					Q,				vf00[w],			\length[w]
	mul.w					\rlength,	vf00,				Q
.aendi
	div					Q,				v[\adj],			\length[w]
.aelse
.aif "\rlength" ne ""
	rsqrt					Q,				vf00[w],			sqsum[\opp]
	mul.w					\rlength,	vf00,				Q
.aelse
	rsqrt					Q,				v[\adj],			sqsum[\opp]
.aendi
.aendi

	;; ok, Q now either contains 1/h or a/h... based on rlength
.aif "\rlength" ne ""
	;; it has 1/h, so we need a/h
	mul.\adj				temp,			v,					Q
	CopyAcrossVector	\out,			temp[\adj]
.aelse
	;; already has a/h... simply to move it into place
	CopyAcrossVector	\out,			Q
.aendi

	;; now we just need to convert cosines into sines by the identity
	;; tan = sine / cosine
	;; sine = tan * cosine
	;; it so happens that we have the tangent handy...
.aif "\layout" eq "SSCC"
	mul.xy				\out,			\out,				tan[w]
.aendi
.aif "\layout" eq "CCSS"
	mul.zw				\out,			\out,				tan[w]
.aendi
.aif "\layout" eq "SCCS"
	mul.xw				\out,			\out,				tan[w]
.aendi
.aif "\layout" eq "CSSC"
	mul.yz				\out,			\out,				tan[w]
.aendi
.aif "\layout" eq "SCSC"
	mul.xz				\out,			\out,				tan[w]
.aendi
.aif "\layout" eq "CSCS"
	mul.yw				\out,			\out,				tan[w]
.aendi
.endm	
	
	
.macro ArcTan2							dest,				src,				  destfield,  topfield, bottomfield
local temp
	
	MR32Between			\topfield,	\bottomfield
.aif \&mr32Between eq 1
.aif "\topfield" eq "y"
	eatanxy				P,				\src
.aelse
	RotateIntoPos		temp,			\src,				y,					  \topfield
	RotateIntoPos		temp,			\src,				x,					  \bottomfield
	eatanxy				P,				temp
.aendi
.aelse
.aif \&mr32Between eq 2
.aif "\srcfields" eq "zx"
	eatanxz				P,				\src
.aelse
	RotateIntoPos		temp,			\src,				z,					  \topfield
	RotateIntoPos		temp,			\src,				x,					  \bottomfield
	eatanxz				P,				temp
.aendi
.aelse
	RotateIntoPos		temp,			\src,				y,					  \topfield
	RotateIntoPos		temp,			\src,				x,					  \bottomfield
	eatanxy				P,				temp
.aendi
.aendi
	mfp.\destfield		\dest,		P	
.endm

.macro VectorLengthFlex				dest,				src,				  srcfields
.aif "\srcfields" eq "xyzw"
	mul.w					ACC,			vf00,				src[x]
	madd.w				ACC,			vf00,				src[y]
	madd.w				ACC,			vf00,				src[z]
	madd.w				\dest,		vf00,				src[w]
.aendi
.aif "\srcfields" eq "xyz"
	mul.w					ACC,			vf00,				src[x]
	madd.w				ACC,			vf00,				src[y]
	madd.w				\dest,		vf00,				src[z]
.aendi
.aif "\srcfields" eq "xyw"
	mul.w					ACC,			vf00,				src[x]
	madd.w				ACC,			vf00,				src[y]
	madd.w				\dest,		vf00,				src[w]
.aendi
.aif "\srcfields" eq "xzw"
	mul.w					ACC,			vf00,				src[x]
	madd.w				ACC,			vf00,				src[z]
	madd.w				\dest,		vf00,				src[w]
.aendi
.aif "\srcfields" eq "yzw"
	mul.w					ACC,			vf00,				src[y]
	madd.w				ACC,			vf00,				src[z]
	madd.w				\dest,		vf00,				src[w]
.aendi
.aif "\srcfields" eq "xy"
	mul.w					ACC,			vf00,				src[x]
	madd.w				\dest,		vf00,				src[y]
.aendi
.aif "\srcfields" eq "xz"
	mul.w					ACC,			vf00,				src[x]
	madd.w				\dest,		vf00,				src[z]
.aendi
.aif "\srcfields" eq "xw"
	mul.w					ACC,			vf00,				src[x]
	madd.w				\dest,		vf00,				src[w]
.aendi
.aif "\srcfields" eq "yz"
	mul.w					ACC,			vf00,				src[y]
	madd.w				\dest,		vf00,				src[z]
.aendi
.aif "\srcfields" eq "yw"
	mul.w					ACC,			vf00,				src[y]
	madd.w				\dest,		vf00,				src[w]
.aendi
.aif "\srcfields" eq "zw"
	mul.w					ACC,			vf00,				src[z]
	madd.w				\dest,		vf00,				src[w]
.aendi
.endm

.macro IsNoBitSet		dest, src
local temp
local ione
	mfir.w				temp,			\src
	itof0.w				temp,			temp
	add.w					vf00,			temp,				vf00[x]
	iliu					ione,			1
	fmand					\dest,		ione
.endm

.macro IsNoBitSet4	dest, src1, src2, src3, src4
local temp
local ione
	mfir.x				temp,			\src1
	mfir.y				temp,			\src2
	mfir.z				temp,			\src3
	mfir.w				temp,			\src4
	itof0					temp,			temp
	add					vf00,			temp,				vf00[x]
	iliu					ione,			1
	fsand					\dest,		ione
.endm

.macro MoveMatrix		dest, src
	move					\dest[0],	\src[0]
	move					\dest[1],	\src[1]
	move					\dest[2],	\src[2]
	move					\dest[3],	\src[3]
.endm
			
.macro MultiplyMatrix matresult, matleft, matright
.aif "\matright" eq "\matresult"
local temp
destmat .assignc "temp"
.aelse
destmat .assignc "\matresult"
.aendi
   mul            acc,           \matright[0], \matleft[0][x]
   madd           acc,           \matright[1], \matleft[0][y]
   madd           acc,           \matright[2], \matleft[0][z]
   madd           \&destmat[0], \matright[3], \matleft[0][w]

   mul            acc,           \matright[0], \matleft[1][x]
   madd           acc,           \matright[1], \matleft[1][y]
   madd           acc,           \matright[2], \matleft[1][z]
   madd           \&destmat[1], \matright[3], \matleft[1][w]

   mul            acc,           \matright[0], \matleft[2][x]
   madd           acc,           \matright[1], \matleft[2][y]
   madd           acc,           \matright[2], \matleft[2][z]
   madd           \&destmat[2], \matright[3], \matleft[2][w]

   mul            acc,           \matright[0], \matleft[3][x]
   madd           acc,           \matright[1], \matleft[3][y]
   madd           acc,           \matright[2], \matleft[3][z]
   madd           \&destmat[3], \matright[3], \matleft[3][w]
.aif "\matright" eq "\matresult"
	MoveMatrix		\matresult,		temp
.aendi
.endm

.macro MultiplyMatrix34 matresult, matleft, matright
.aif "\matright" eq "\matresult"
local temp
destmat .assignc "temp"
.aelse
destmat .assignc "\matresult"
.aendi
   mul            acc,           \matright[0],	\matleft[0][x]
   madd           acc,           \matright[1],	\matleft[0][y]
   madd           acc,           \matright[2],	\matleft[0][z]
   madd           \&destmat[0],	\matright[3],  vf00[w]

   mul            acc,           \matright[0],	\matleft[1][x]
   madd           acc,           \matright[1],	\matleft[1][y]
   madd           acc,           \matright[2],	\matleft[1][z]
   madd           \&destmat[1],	\matright[3],  vf00[w]

   mul            acc,           \matright[0],	\matleft[2][x]
   madd           acc,           \matright[1],	\matleft[2][y]
   madd           acc,           \matright[2],	\matleft[2][z]
   madd           \&destmat[2],	\matright[3],  vf00[w]

	move				\&destmat[3],	\matright[3]
	
.aif "\matright" eq "\matresult"
	MoveMatrix		\matresult,		temp
	move				\matresult[3],	\matright[3] ; cause the 2 above moves to this line to drop out
.aendi
.endm

.macro MultiplyMatrix43 matresult, matleft, matright
.aif "\matright" eq "\matresult"
local temp
destmat .assignc "temp"
.aelse
destmat .assignc "\matresult"
.aendi
   mul            acc,           \matright[0],	\matleft[0][x]
   madd           acc,           \matright[1],	\matleft[0][y]
   madd           acc,           \matright[2],	\matleft[0][z]
   madd           \&destmat[0],	vf00,				\matleft[0][w]

   mul            acc,           \matright[0],	\matleft[1][x]
   madd           acc,           \matright[1],	\matleft[1][y]
   madd           acc,           \matright[2],	\matleft[1][z]
   madd           \&destmat[1],	vf00,				\matleft[1][w]

   mul            acc,           \matright[0],	\matleft[2][x]
   madd           acc,           \matright[1],	\matleft[2][y]
   madd           acc,           \matright[2],	\matleft[2][z]
   madd           \&destmat[2],  vf00,				\matleft[2][w]

   mul            acc,           \matright[0],	\matleft[3][x]
   madd           acc,           \matright[1],	\matleft[3][y]
   madd           acc,           \matright[2],	\matleft[3][z]
   madd           \&destmat[3],  vf00,				\matleft[3][w]
.aif "\matright" eq "\matresult"
	MoveMatrix		\matresult,		temp
.aendi
.endm

.macro MultiplyMatrix33 matresult, matleft, matright
.aif "\matright" eq "\matresult"
local temp
destmat .assignc "temp"
.aelse
destmat .assignc "\matresult"
.aendi
   mul            acc,           \matright[0],	\matleft[0][x]
   madd           acc,           \matright[1],	\matleft[0][y]
   madd           \&destmat[0],  \matright[2],	\matleft[0][z]

   mul            acc,           \matright[0],	\matleft[1][x]
   madd           acc,           \matright[1],	\matleft[1][y]
   madd           \&destmat[1],  \matright[2],	\matleft[1][z]

   mul            acc,           \matright[0],	\matleft[2][x]
   madd           acc,           \matright[1],	\matleft[2][y]
   madd           \&destmat[2],  \matright[2],	\matleft[2][z]

	Ident				\&destmat[3]

.aif "\matright" eq "\matresult"
	MoveMatrix		\matresult,		temp
	Ident				\matresult	  ; causes the above ident and the move line to both drop out...
.aendi
.endm

.macro LoadMatrix  matrix,offset,vumemlocation=vi00
	lq    \matrix[0], \offset+0(\vumemlocation)
	lq    \matrix[1], \offset+1(\vumemlocation)
	lq    \matrix[2], \offset+2(\vumemlocation)
	lq		\matrix[3], \offset+3(\vumemlocation)
.endm

.macro SaveIntVector4 offset, reg, val1, val2, val3, val4
local savetemp
		  mfir.x	 savetemp, \val1
.aif "\val2" ne ""
		  mfir.y	 savetemp, \val2
.aif "\val3" ne ""
		  mfir.z	 savetemp, \val3
.aif "\val4" ne ""
		  mfir.w	 savetemp, \val4
		  sq.xyzw savetemp, \offset(\reg)
.aelse
		  sq.xyz savetemp, \offset(\reg)
.aendi
.aelse
		  sq.xy savetemp, \offset(\reg)
.aendi
.aelse
		  sq.x savetemp, \offset(\reg)
.aendi
.endm
		  
.macro LoadIntVector4 offset, reg, val1, val2, val3, val4
local savetemp		  
		  lq.xyzw savetemp, \offset(\reg)
.aif "\val1" ne ""
		  mtir	 \val1, savetemp[x]
.aendi
.aif "\val2" ne ""
		  mtir	 \val2, savetemp[y]
.aendi
.aif "\val3" ne ""
		  mtir	 \val3, savetemp[z]
.aendi
.aif "\val4" ne ""
		  mtir	 \val4, savetemp[w]
.aendi
.endm

.macro SaveVector4 offset, reg, val
		  sq.xyzw \val, \offset(\reg)
.endm

.macro LoadVector4 offset, reg, val
		  lq.xyzw \val, \offset(\reg)
.endm		  
		  
.macro SaveVector3 offset, reg, val
		  sq.xyz \val, \offset(\reg)
.endm

.macro LoadVector3 offset, reg, val
		  lq.xyz \val, \offset(\reg)
.endm		  

.macro SaveVector2 offset, reg, val
		  sq.xy \val, \offset(\reg)
.endm

.macro LoadVector2 offset, reg, val
		  lq.xy \val, \offset(\reg)
.endm		  
		  
.macro SaveIVector4 reg, val
		  sqi.xyzw \val, (\reg++)
.endm

.macro LoadIVector4 reg, val
		  lqi.xyzw \val, (\reg++)
.endm		  
		  
.macro SaveIVector3 reg, val
		  sqi.xyz \val, (\reg++)
.endm

.macro LoadIVector3 reg, val
		  lqi.xyz \val, (\reg++)
.endm		  
		  
.macro SaveIVector2 reg, val
		  sqi.xy \val, (\reg++)
.endm

.macro LoadIVector2 reg, val
		  lqi.xy \val, (\reg++)
.endm		  
		  
.macro PushVector4 val
		  sqd.xyzw			\val, (--Stack)
.endm

.macro PopVector4 val
		  lqi.xyzw			\val, (Stack++)
.endm

.macro PushVector3 val
		  sqd.xyz			\val, (--Stack)
.endm

.macro PopVector3 val
		  lqi.xyz			\val, (Stack++)
.endm

.macro PushIntVector4 val1, val2, val3, val4
local savetemp
		  mfir.x	 savetemp, \val1
.aif "\val2" ne ""
		  mfir.y	 savetemp, \val2
.aif "\val3" ne ""
		  mfir.z	 savetemp, \val3
.aif "\val4" ne ""
		  mfir.w	 savetemp, \val4
		  sqd.xyzw savetemp, (--Stack)
.aelse
		  sqd.xyz savetemp, (--Stack)
.aendi
.aelse
		  sqd.xy savetemp, (--Stack)
.aendi
.aelse
		  sqd.x savetemp, (--Stack)
.aendi
.endm

.macro PopIntVector4 val1, val2, val3, val4
local savetemp		  
		  lqi.xyzw savetemp, (Stack++)
.aif "\val1" ne ""
		  mtir	 \val1, savetemp[x]
.aendi
.aif "\val2" ne ""
		  mtir	 \val2, savetemp[y]
.aendi
.aif "\val3" ne ""
		  mtir	 \val3, savetemp[z]
.aendi
.aif "\val4" ne ""
		  mtir	 \val4, savetemp[w]
.aendi
local t
.endm

.macro WrapColor4 val1
.aif \&ColorsAreCompressed eq 1
local mask
local temp
		  iaddiu				mask, VI00, 0xFF
		  mtir				temp, \val1[x]
		  iand				temp, temp, mask
		  mfir.x				\val1, temp
		  mtir				temp, \val1[y]
		  iand				temp, temp, mask
		  mfir.y				\val1, temp
		  mtir				temp, \val1[z]
		  iand				temp, temp, mask
		  mfir.z				\val1, temp
		  mtir				temp, \val1[w]
		  iand				temp, temp, mask
		  mfir.w				\val1, temp
.aendi
.endm
		  		  
.macro WrapColor3 val1
.aif \&ColorsAreCompressed eq 1
local mask
local temp
		  iaddiu				mask, VI00, 0xFF
		  mtir				temp, \val1[x]
		  iand				temp, temp, mask
		  mfir.x				\val1, temp
		  mtir				temp, \val1[y]
		  iand				temp, temp, mask
		  mfir.y				\val1, temp
		  mtir				temp, \val1[z]
		  iand				temp, temp, mask
		  mfir.z				\val1, temp
.aendi
.endm

;;; here lie the called routines... if they arent used, vcl will drop them
;;; so dont worry too much
.macro LowLevelSubroutines
sub_CCCC:
	CCCC					SinCosOut,	SinCosIn
	jr						CCCCRet
sub_CCCCFast:
	CCCCFast				SinCosOut,	SinCosIn
	jr						CCCCRet
.endm
	
