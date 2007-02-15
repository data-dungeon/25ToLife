########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Debug
INTDIR = SNPS2Debug

LIBS=\

AS=ps2cc
AS_FLAGS=\
	-Wa,-g\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DTUPPERWARE_FOR_ENGINE\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-Wa,-sn\
	-g\

DVP=ps2cc
DVP_FLAGS=\
	-Wd,-g\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DTUPPERWARE_FOR_ENGINE\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-g\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DTUPPERWARE_FOR_ENGINE\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-fno-rtti\
	-fno-exceptions\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\
	-g\

CXX_FLAGS=$(CC_FLAGS)

LD_OBJS=\
	"SNPS2Debug/TupperAttribPch_SN.obj"\
	"SNPS2Debug/AuxKeyValue.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/TupperAttribPch_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/TupperAttrib/TupperAttribPch_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/TupperAttrib/TupperAttribPch_SN.cpp" -o $@

"SNPS2Debug/AuxKeyValue.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/TupperAttrib/AuxKeyValue.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/TupperAttrib/AuxKeyValue.cpp" -o $@

LIB: "SNPS2Debug/TupperAttrib.lib"

"SNPS2Debug/TupperAttrib.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/TupperAttribPch_SN.d"
include "SNPS2Debug/AuxKeyValue.d"

