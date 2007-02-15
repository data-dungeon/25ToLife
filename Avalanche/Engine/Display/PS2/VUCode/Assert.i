
;;; asserts!
;;; These asserts trigger a [D] bit halt when they fail...
;;; the macros defined in here are
;;;						assertlt a, b					(a < b)
;;;						assertle a, b					(a <= b)
;;;						assertgt a, b					(a > b)
;;;						assertge a, b					(a >= b)
;;;						assertrange a, low, high	(low <= a <= high)
;;;						assertmem a						(0 <= a <= 1023)

.macro assertfalse
.raw
exit\@:[D]
	nop nop
.endraw
.endm
		
.macro asserteq a, b
local test
	ibeq				   \a,			\b,			asserteq\@
	assertfalse
asserteq\@:				
.endm

.macro assertlt a, b
local test
	iblt				   \a,			\b,			assertlt\@
	assertfalse
assertlt\@:				
.endm

.macro assertle a, b
local test
	ible					\a,			\b,				assertle\@
	assertfalse
assertle\@:				
.endm
	
.macro assertgt a, b
local test
	ibgt					\a,			\b,				assertgt\@
	assertfalse
assertgt\@:				
.endm
	
.macro assertge a, b
local test
	ibge					\a,			\b,				assertge\@
	assertfalse
assertge\@:				
.endm
	
.macro assertrange a, low, high
local test
	ibltiu				\a,			\low,				assertrange\@
	ibleiu				\a,			\high,			assertrangedone\@
assertrange\@:
	assertfalse
assertrangedone\@:
.endm
	
.macro assertmem a
	assertrange			\a,			0,					1023
.endm
	
