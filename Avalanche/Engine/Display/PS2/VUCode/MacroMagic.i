;;; these are just macros that manipulate preprocessor variables
;;; they are long and horrendous...

mr32Fields .assignc ""

;; this is not complete, but handles most cases..
.macro MR32Fields		fields
.aif "\fields" eq "x"
mr32Fields .assignc "w"
.aelse
.aif "\fields" eq "y"
mr32Fields .assignc "x"
.aelse
.aif "\fields" eq "z"
mr32Fields .assignc "y"
.aelse
.aif "\fields" eq "w"
mr32Fields .assignc "z"
.aendi
.aendi
.aendi
.aendi
.aif "\fields" eq "xy"
mr32Fields .assignc "wx"
.aelse
.aif "\fields" eq "yz"
mr32Fields .assignc "xy"
.aelse
.aif "\fields" eq "zw"
mr32Fields .assignc "yz"
.aelse
.aif "\fields" eq "wx"
mr32Fields .assignc "zw"
.aendi
.aendi
.aendi
.aendi
.aif "\fields" eq "xz"
mr32Fields .assignc "wy"
.aelse
.aif "\fields" eq "yw"
mr32Fields .assignc "xz"
.aelse
.aif "\fields" eq "zx"
mr32Fields .assignc "yw"
.aelse
.aif "\fields" eq "wy"
mr32Fields .assignc "zx"
.aendi
.aendi
.aendi
.aendi
.aif "\fields" eq "xyz"
mr32Fields .assignc "wxy"
.aelse
.aif "\fields" eq "yzw"
mr32Fields .assignc "xyz"
.aelse
.aif "\fields" eq "zwx"
mr32Fields .assignc "yzw"
.aelse
.aif "\fields" eq "wxy"
mr32Fields .assignc "zwx"
.aendi
.aendi
.aendi
.aendi
.aif "\fields" eq "xyzw"
mr32Fields .assignc "wxyz"
.aelse
.aif "\fields" eq "wxyz"
mr32Fields .assignc "zwxy"
.aelse
.aif "\fields" eq "zwxy"
mr32Fields .assignc "yzwx"
.aelse
.aif "\fields" eq "yzwx"
mr32Fields .assignc "xyzw"
.aendi
.aendi
.aendi
.aendi
.aif "\fields" eq "xyw"
mr32Fields .assignc "wxz"
.aelse
.aif "\fields" eq "wxz"
mr32Fields .assignc "zwy"
.aelse
.aif "\fields" eq "zwy"
mr32Fields .assignc "yzx"
.aelse
.aif "\fields" eq "yzx"
mr32Fields .assignc "xyw"
.aendi
.aendi
.aendi
.aendi
.aif "\fields" eq "xzw"
mr32Fields .assignc "wyz"
.aelse
.aif "\fields" eq "wyz"
mr32Fields .assignc "zxy"
.aelse
.aif "\fields" eq "zxy"
mr32Fields .assignc "ywx"
.aelse
.aif "\fields" eq "ywx"
mr32Fields .assignc "xzw"
.aendi
.aendi
.aendi
.aendi
.endm

.macro MR32Between dest, src
.aif "\dest" eq "x"
	.aif "\src" eq "x"
mr32Between .assigna 0
	.aelse
	.aif "\src" eq "y"
mr32Between .assigna 1
	.aelse
	.aif "\src" eq "z"
mr32Between .assigna 2
	.aelse
mr32Between .assigna 3
	.aendi
	.aendi
	.aendi
.aelse
.aif "\dest" eq "y"
	.aif "\src" eq "x"
mr32Between .assigna 3
	.aelse
	.aif "\src" eq "y"
mr32Between .assigna 0
	.aelse
	.aif "\src" eq "z"
mr32Between .assigna 1
	.aelse
mr32Between .assigna 2
	.aendi
	.aendi
	.aendi
.aelse
.aif "\dest" eq "z"
	.aif "\src" eq "x"
mr32Between .assigna 2
	.aelse
	.aif "\src" eq "y"
mr32Between .assigna 3
	.aelse
	.aif "\src" eq "z"
mr32Between .assigna 0
	.aelse
mr32Between .assigna 1
	.aendi
	.aendi
	.aendi
.aelse
	.aif "\src" eq "x"
mr32Between .assigna 1
	.aelse
	.aif "\src" eq "y"
mr32Between .assigna 2
	.aelse
	.aif "\src" eq "z"
mr32Between .assigna 3
	.aelse
mr32Between .assigna 0
	.aendi
	.aendi
	.aendi
.aendi
.aendi
.aendi
.endm

.macro OppositeFields				fields
.endm

.macro FieldWalker_UpdateLineCnt
.aif "\&Field" EQ "x"
FieldWalker_LineCnt .assigna \&Line
.aelse
FieldWalker_LineCnt .assigna \&Line + 1
.aendi
.endm
		  
.macro FieldWalker_Save
FieldWalker_Field .assignc "\&Field"
FieldWalker_Line .assigna \&Line
.endm

.macro FieldWalker_Restore
Field .assignc "\&FieldWalker_Field"
Line .assigna \&FieldWalker_Line
.endm
		  		  
.macro FieldWalker_Init
Field .assignc "x"
Line .assigna 0
.endm		  
		  
.macro NextField
.aif "\&Field" eq "w"
Field .assignc "x"
Line .assigna \&Line + 1
.aelse		  
.aif "\&Field" eq "z"
Field .assignc "w"
.aendi
.aif "\&Field" eq "y"
Field .assignc "z"
.aendi
.aif "\&Field" eq "x"
Field .assignc "y"
.aendi
.aendi
.endm

