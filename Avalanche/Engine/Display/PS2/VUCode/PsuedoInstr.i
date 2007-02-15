;; PseudoInstr.i is a bunch of vu instruction look-alikes
;; very simple, but can be useful.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; These are useful for the psuedo
;;; upper instructions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro extractFArgs4	arg1,		arg2,		arg3,		arg4
.aif "\arg4" eq ""
FIELDS .assignc ".xyzw"
REG1 .assignc "\arg1"
REG2 .assignc "\arg2"
LABEL .assignc "\arg3"
.aelse
FIELDS .assignc "\arg1"
REG1 .assignc "\arg2"
REG2 .assignc "\arg3"
LABEL .assignc "\arg4"
.aendi
.endm

.macro extractFArgs3	arg1,		arg2,		arg3
.aif "\arg3" eq ""
FIELDS .assignc ".xyzw"
REG1 .assignc "\arg1"
LABEL .assignc "\arg2"
.aelse
FIELDS .assignc "\arg1"
REG1 .assignc "\arg2"
LABEL .assignc "\arg3"
.aendi
.endm
	
.macro loadFMACZFlags	dest
.aif "\&FIELDS" eq ".xyzw"
	iliu					\dest,	0x000F
.aendi
.aif "\&FIELDS" eq ".xyz"
	iliu					\dest,	0x000E
.aendi
.aif "\&FIELDS" eq ".xyw"
	iliu					\dest,	0x000D
.aendi
.aif "\&FIELDS" eq ".xy"
	iliu					\dest,	0x000C
.aendi
.aif "\&FIELDS" eq ".xzw"
	iliu					\dest,	0x000B
.aendi
.aif "\&FIELDS" eq ".xz"
	iliu					\dest,	0x000A
.aendi
.aif "\&FIELDS" eq ".xw"
	iliu					\dest,	0x0009
.aendi
.aif "\&FIELDS" eq ".x"
	iliu					\dest,	0x0008
.aendi
.aif "\&FIELDS" eq ".yzw"
	iliu					\dest,	0x0007
.aendi
.aif "\&FIELDS" eq ".yz"
	iliu					\dest,	0x0006
.aendi
.aif "\&FIELDS" eq ".yw"
	iliu					\dest,	0x0005
.aendi
.aif "\&FIELDS" eq ".y"
	iliu					\dest,	0x0004
.aendi
.aif "\&FIELDS" eq ".zw"
	iliu					\dest,	0x0003
.aendi
.aif "\&FIELDS" eq ".z"
	iliu					\dest,	0x0002
.aendi
.aif "\&FIELDS" eq ".w"
	iliu					\dest,	0x0001
.aendi
.endm

.macro loadFMACSFlags	dest
.aif "\&FIELDS" eq ".xyzw"
	iliu					\dest,	0x00F0
.aendi
.aif "\&FIELDS" eq ".xyz"
	iliu					\dest,	0x00E0
.aendi
.aif "\&FIELDS" eq ".xyw"
	iliu					\dest,	0x00D0
.aendi
.aif "\&FIELDS" eq ".xy"
	iliu					\dest,	0x00C0
.aendi
.aif "\&FIELDS" eq ".xzw"
	iliu					\dest,	0x00B0
.aendi
.aif "\&FIELDS" eq ".xz"
	iliu					\dest,	0x00A0
.aendi
.aif "\&FIELDS" eq ".xw"
	iliu					\dest,	0x0090
.aendi
.aif "\&FIELDS" eq ".x"
	iliu					\dest,	0x0080
.aendi
.aif "\&FIELDS" eq ".yzw"
	iliu					\dest,	0x0070
.aendi
.aif "\&FIELDS" eq ".yz"
	iliu					\dest,	0x0060
.aendi
.aif "\&FIELDS" eq ".yw"
	iliu					\dest,	0x0050
.aendi
.aif "\&FIELDS" eq ".y"
	iliu					\dest,	0x0040
.aendi
.aif "\&FIELDS" eq ".zw"
	iliu					\dest,	0x0030
.aendi
.aif "\&FIELDS" eq ".z"
	iliu					\dest,	0x0020
.aendi
.aif "\&FIELDS" eq ".w"
	iliu					\dest,	0x0010
.aendi
.endm

.macro loadFMACSZFlags	dest
.aif "\&FIELDS" eq ".xyzw"
	iliu					\dest,	0x00FF
.aendi
.aif "\&FIELDS" eq ".xyz"
	iliu					\dest,	0x00EE
.aendi
.aif "\&FIELDS" eq ".xyw"
	iliu					\dest,	0x00DD
.aendi
.aif "\&FIELDS" eq ".xy"
	iliu					\dest,	0x00CC
.aendi
.aif "\&FIELDS" eq ".xzw"
	iliu					\dest,	0x00BB
.aendi
.aif "\&FIELDS" eq ".xz"
	iliu					\dest,	0x00AA
.aendi
.aif "\&FIELDS" eq ".xw"
	iliu					\dest,	0x0099
.aendi
.aif "\&FIELDS" eq ".x"
	iliu					\dest,	0x0088
.aendi
.aif "\&FIELDS" eq ".yzw"
	iliu					\dest,	0x0077
.aendi
.aif "\&FIELDS" eq ".yz"
	iliu					\dest,	0x0066
.aendi
.aif "\&FIELDS" eq ".yw"
	iliu					\dest,	0x0055
.aendi
.aif "\&FIELDS" eq ".y"
	iliu					\dest,	0x0044
.aendi
.aif "\&FIELDS" eq ".zw"
	iliu					\dest,	0x0033
.aendi
.aif "\&FIELDS" eq ".z"
	iliu					\dest,	0x0022
.aendi
.aif "\&FIELDS" eq ".w"
	iliu					\dest,	0x0011
.aendi
.endm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; branch if (reg cond imm)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.macro ibeqiu			value,		iuval,			label
local temp
.aif "\iuval" eq "0"
	ibeqz					\value,		\label
.aelse
	isubiu				temp,			\value,			\iuval
	ibeq					temp,			vi00,				\label
.aendi
.endm
.macro ibneiu			value,		iuval,			label
local temp
.aif "\iuval" eq "0"
	ibnez					\value,		\label
.aelse
	isubiu				temp,			\value,			\iuval
	ibne					temp,			vi00,				\label
.aendi
.endm
.macro ibleiu			value,		iuval,			label
local temp
.aif "\iuval" eq "0"
	iblez					\value,		\label
.aelse
	isubiu				temp,			\value,			\iuval
	iblez					temp,			\label
.aendi
.endm
.macro ibgeiu			value,		iuval,			label
local temp
.aif "\iuval" eq "0"
	ibgez					\value,		\label
.aelse
	isubiu				temp,			\value,			\iuval
	ibgez					temp,			\label
.aendi
.endm
.macro ibgtiu			value,		iuval,			label
local temp
.aif "\iuval" eq "0"
	ibgtz					\value,		\label
.aelse
	isubiu				temp,			\value,			\iuval
	ibgtz					temp,			\label
.aendi
.endm
.macro ibltiu			value,		iuval,			label
local temp
.aif "\iuval" eq "0"
	ibltz					\value,		\label
.aelse
	isubiu				temp,			\value,			\iuval
	ibltz					temp,			\label
.aendi
.endm
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; branch if (reg cond reg)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro ible				value1,		value2,			label
local temp
	isub					temp,			\value1,			\value2
	iblez					temp,			\label
.endm
.macro ibge				value1,		value2,			label
local temp
	isub					temp,			\value1,			\value2
	ibgez					temp,			\label
.endm
.macro ibgt				value1,		value2,			label
local temp
	isub					temp,			\value1,			\value2
	ibgtz					temp,			\label
.endm
.macro iblt				value1,		value2,			label
local temp
	isub					temp,			\value1,			\value2
	ibltz					temp,			\label
.endm
				
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; branches for flag checks...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; branch if all matching bits are clear
.macro ibafc	 		value,	bits,					label
local temp
	iliu					temp,		\bits
	iand					temp,		\value,				temp
	ibeq					temp,		VI00,					\label
.endm
	;; branch if all matching bits are set
.macro ibafs			value,	bits,					label
local temp, mask
	iliu					mask,		\bits
	iand					temp,		\value,				mask
	ibeq					temp,		mask,					\label
.endm
	;; branch if any matching bit is set
.macro ib1fs			value,	bits,					label
local temp
	iliu					temp,		\bits
	iand					temp,		\value,				temp
	ibne					temp,		VI00,					\label
.endm
	;; branch if any matching bits are clear
.macro ib1fc	 		value,	bits,					label
local temp, mask
	iliu					mask,		\bits
	iand					temp,		\value,				mask
	ibne					temp,		mask,					\label
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; some more branches...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; branch if reg == vi00
.macro ibeqz			reg,		label
	ibeq					\reg,		vi00,					\label
.endm

	;; branch if reg != vi00
.macro ibnez			reg,		label
	ibne					\reg,		vi00,					\label
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; floating point branches
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; branch if one reg.? > const
.macro bogti				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_bogt
.endm
	;; branch if one reg.? < const
.macro bolti				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_bolt
.endm
	;; branch if one reg.? <= const
.macro bolei				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_bole
.endm
	;; branch if one reg.? >= const
.macro bogei				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_boge
.endm
	;; branch if one reg.? == const
.macro boeqi				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_boeq
.endm
	;; branch if one reg.? != const
.macro bonei				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_bone
.endm

	;; branch if all reg.? > const
.macro bagti				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_bagt
.endm
	;; branch if all reg.? < const
.macro balti				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_balt
.endm
	;; branch if all reg.? <= const
.macro balei				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_bale
.endm
	;; branch if all reg.? >= const
.macro bagei				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_bage
.endm
	;; branch if all reg.? == const
.macro baeqi				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_baeq
.endm
	;; branch if all reg.? != const
.macro banei				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	loi					\&REG2
REG2 .assignc "I"
	_bane
.endm

	;; branch if all reg1.? > reg2.?
.macro bagt				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_bagt
.endm
	;; branch if one reg1.? > reg2.?
.macro bogt				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_bogt
.endm

	;; branch if all reg1.? < reg2.?
.macro balt				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_balt
.endm
	;; branch if one reg1.? < reg2.?
.macro bolt				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_bolt
.endm

	;; branch if all reg1.? <= reg2.?
.macro bale				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_bale
.endm
	;; branch if one reg1.? <= reg2.?
.macro bole				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_bole
.endm

	;; branch if all reg1.? >= reg2.?
.macro bage				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_bage
.endm
	;; branch if one reg1.? >= reg2.?
.macro boge				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_boge
.endm

	;; branch if all reg1.? != reg2.?
.macro bane				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_bane
.endm
	;; branch if one reg1.? != reg2.?
.macro bone				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_bone
.endm

	;; branch if all reg1.? == reg2.?
.macro baeq				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_baeq
.endm
	;; branch if one reg1.? == reg2.?
.macro boeq				arg1,	arg2,	arg3,	arg4
	extractFArgs4		\arg1,	\arg2,	\arg3,	\arg4
	_boeq
.endm

	;; branch if all reg1.? > reg2.?
.macro _bagt
local gt1, flags
	loadFMACSZFlags		flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	;; true if S==0 && Z==0 for all fields
	ibeqz					gt1,		\&LABEL
.endm
	;; branch if one reg1.? > reg2.?
.macro _bogt
local gt1, flags, gt14
	loadFMACSZFlags	flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	;; true if S==0 && Z==0 in any field
	;; << 4
	iadd					gt14,		gt1,		gt1
	iadd					gt14,		gt14,		gt14
	iadd					gt14,		gt14,		gt14
	iadd					gt14,		gt14,		gt14
	ior					gt1,		gt1,		gt14
	loadFMACSFlags		flags
	iand					gt1,		gt1,		flags
	ibne					gt1,		flags,	\&LABEL
.endm

	;; branch if all reg1.? < reg2.?
.macro _balt
local gt1, flags
	loadFMACSZFlags	flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	;; true if S==1 && Z==0 in all fields
	loadFMACSFlags		flags
	ibeq					gt1,		flags,	\&LABEL
.endm
	;; branch if one reg1.? < reg2.?
.macro _bolt
local gt1, flags, gt14
	loadFMACSZFlags	flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	;; true if S==1 && Z==0 in any field
	;; << 4 to move S <- Z place
	iadd					gt14,		gt1,		gt1
	iadd					gt14,		gt14,		gt14
	iadd					gt14,		gt14,		gt14
	iadd					gt14,		gt14,		gt14
	;; !Z
	isub					gt14,		vi00,		gt14
	isubiu				gt14,		gt14,		1
	;; !Z & S
	iand					gt1,		gt1,		gt14
	;; mask the bits we want...
	loadFMACSFlags		flags
	iand					gt1,		gt1,		flags
	;; branch if any bit is set
	ibnez					gt1,		\&LABEL
.endm

	;; branch if all reg1.? <= reg2.?
.macro _bale
local gt1, flags, gt14
	loadFMACSZFlags	flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	;; true if S==1 || Z==1
	;; << 4 to move S <- Z place
	iadd					gt14,		gt1,		gt1
	iadd					gt14,		gt14,		gt14
	iadd					gt14,		gt14,		gt14
	iadd					gt14,		gt14,		gt14
	ior					gt1,		gt1,		gt14
	loadFMACSFlags	flags
	ibeq					gt1,		flags,	\&LABEL
.endm
	;; branch if one reg1.? <= reg2.?
.macro _bole
local gt1, flags
	loadFMACSZFlags		flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	;; opposite of _bagt
	ibnez					gt1,		\&LABEL
.endm

	;; branch if all reg1.? >= reg2.?
.macro _bage
	;; opposite of _bolt
local gt1, flags, gt14
	loadFMACSZFlags	flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	;; true if S==1 && Z==0 in any field
	;; << 4 to move S <- Z place
	iadd					gt14,		gt1,		gt1
	iadd					gt14,		gt14,		gt14
	iadd					gt14,		gt14,		gt14
	iadd					gt14,		gt14,		gt14
	;; !Z
	isub					gt14,		vi00,		gt14
	isubiu				gt14,		gt14,		1
	;; !Z & S
	iand					gt1,		gt1,		gt14
	;; mask the bits we want...
	loadFMACSFlags		flags
	iand					gt1,		gt1,		flags
	ibeqz					gt1,		\&LABEL
.endm
	;; branch if one reg1.? >= reg2.?
.macro _boge
	;; opposite of balt
local gt1, flags
	loadFMACSZFlags	flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	loadFMACSFlags		flags
	ibne					gt1,		flags,	\&LABEL
.endm

	;; branch if all reg1.? != reg2.?
.macro _bane
local gt1, flags
	loadFMACZFlags		flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	ibeqz					gt1,		\&LABEL
.endm
	;; branch if one reg1.? != reg2.?
.macro _bone
local gt1, flags
	loadFMACZFlags		flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	ibne					gt1,		flags,	\&LABEL
.endm

	;; branch if all reg1.? == reg2.?
.macro _baeq
local gt1, flags
	loadFMACZFlags		flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	ibeq					gt1,		flags,	\&LABEL
.endm
	;; branch if one reg1.? == reg2.?
.macro _boeq
local gt1, flags
	loadFMACZFlags		flags
	sub\&FIELDS			vf00,		\&REG1,	\&REG2
	fmand					gt1,		flags
	ibnez					gt1,		\&LABEL
.endm

	
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; float version of iliu
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.macro li				arg1,		arg2,		arg3
.aif "\arg3" eq ""
	loi					\arg2
	one					ones
	mul					\arg1,	ones,		I
.aelse
	loi					\arg3
.aif "\arg1" eq ".w"
	mul.w					\arg2,	vf00,		I
.aelse
.aif .INSTR("\arg1","w") ne -1
	one					ones
	mul\arg1				\arg2,	ones,		I
.aelse
	add\arg1				\arg2,	vf00,		I
.aendi
.aendi
.aendi
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; old versions of floating point branches
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; branch if reg.? > const
.macro bgti				field,	reg,		const,	label
local gt1, flags
	loi					\const
	sub					vf00,		\reg,					I
.aif "\field" eq ".x"
	iliu					flags,	0x0080
.aendi
.aif "\field" eq ".y"
	iliu					flags,	0x0040
.aendi
.aif "\field" eq ".z"
	iliu					flags,	0x0020
.aendi
.aif "\field" eq ".w"
	iliu					flags,	0x0010
.aendi
	fmand					gt1,		flags
	ibeqz					gt1,		\label
.endm

	;; branch if reg1.? > reg2.?
.macro bgt				field,	reg1,		reg2,	label
local gt1, flags
	sub					vf00,		\reg1,	\reg2
.aif "\field" eq ".x"
	iliu					flags,	0x0080
.aendi
.aif "\field" eq ".y"
	iliu					flags,	0x0040
.aendi
.aif "\field" eq ".z"
	iliu					flags,	0x0020
.aendi
.aif "\field" eq ".w"
	iliu					flags,	0x0010
.aendi
	fmand					gt1,		flags
	ibeqz					gt1,		\label
.endm

	;; branch if reg.? < const
.macro blti				field,	reg,		const,	label
local gt1, flags
	loi					\const
	sub					vf00,		\reg,					I
.aif "\field" eq ".x"
	iliu					flags,	0x0080
.aendi
.aif "\field" eq ".y"
	iliu					flags,	0x0040
.aendi
.aif "\field" eq ".z"
	iliu					flags,	0x0020
.aendi
.aif "\field" eq ".w"
	iliu					flags,	0x0010
.aendi
	fmand					gt1,		flags
	ibnez					gt1,		\label
.endm

	;; branch if reg1.? < reg2.?
.macro blt				field,	reg1,		reg2,	label
local gt1, flags
	sub					vf00,		\reg1,	\reg2
.aif "\field" eq ".x"
	iliu					flags,	0x0080
.aendi
.aif "\field" eq ".y"
	iliu					flags,	0x0040
.aendi
.aif "\field" eq ".z"
	iliu					flags,	0x0020
.aendi
.aif "\field" eq ".w"
	iliu					flags,	0x0010
.aendi
	fmand					gt1,		flags
	ibnez					gt1,		\label
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; float math
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro addi		arg1, arg2, arg3, arg4
.aif "\arg4" eq ""
	loi					\arg3
	add					\arg1,	\arg2,				I
.aelse
	loi					\arg4
	add\arg1				\arg2,	\arg3,				I
.aendi
.endm
	
.macro subi		arg1, arg2, arg3, arg4
.aif "\arg4" eq ""
	loi					\arg3
	sub					\arg1,	\arg2,				I
.aelse
	loi					\arg4
	sub\arg1				\arg2,	\arg3,				I
.aendi
.endm
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; logic
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; and against immediate value
.macro iandiu	dest,	src, imm
local temp
	iliu					temp,		\imm
	iand					\dest,	\src,					temp
.endm

	;; or against immediate value
.macro ioriu	dest,	src, imm
local temp
	iliu					temp,		\imm
	ior					\dest,	\src,					temp
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; move & loads
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro imove	dest, src
	iaddiu				\dest,	\src,					0
.endm

.macro iliu		dest, value
	iaddiu				\dest,	vi00,					\value
.endm

.macro ihighbit dest
	iaddiu				\dest,	vi00,					1
	iaddiu				\dest,	\dest,				0x7fff
.endm

.macro iincr	dest, value=1
	iaddiu				\dest,	\dest,				\value
.endm

.macro idecr	dest, value=1
	isubiu				\dest,	\dest,				\value
.endm

.macro imax		dest, lhs, rhs
	iblt					\lhs,		\rhs,					imax_lhsltrhs\@
	imove					\dest,	\lhs
	b						imax_done\@
imax_lhsltrhs\@:
	imove					\dest,	\rhs
imax_done\@:	
.endm

.macro imin		dest, lhs, rhs
	iblt					\lhs,		\rhs,					imin_lhsltrhs\@
	imove					\dest,	\rhs
	b						imin_done\@
imin_lhsltrhs\@:
	imove					\dest,	\lhs
imin_done\@:	
.endm

.macro imaxiu	dest,	lhs,	rhs
	ibltiu				\lhs,		\rhs,					imax_lhsltrhs\@
	imove					\dest,	\lhs
	b						imax_done\@
imax_lhsltrhs\@:
	iliu					\dest,	\rhs
imax_done\@:	
.endm

.macro iminiu		dest, lhs, rhs
	ibltiu				\lhs,		\rhs,					imin_lhsltrhs\@
	iliu					\dest,	\rhs
	b						imin_done\@
imin_lhsltrhs\@:
	imove					\dest,	\lhs
imin_done\@:	
.endm
