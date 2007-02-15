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
	-DPS2\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-D_DEBUG\
	-D_LIB\
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
	-DPS2\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-D_DEBUG\
	-D_LIB\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-g\
	-DPS2\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-D_DEBUG\
	-D_LIB\
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
	"SNPS2Debug/ScriptPCH_SN.obj"\
	"SNPS2Debug/ScriptScriptFunc.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/ScriptPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Script/ScriptPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Script/ScriptPCH_SN.cpp" -o $@

"SNPS2Debug/ScriptScriptFunc.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Script/ScriptScriptFunc.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Script/ScriptScriptFunc.cpp" -o $@

LIB: "SNPS2Debug/Script.lib"

"SNPS2Debug/Script.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/ScriptPCH_SN.d"
include "SNPS2Debug/ScriptScriptFunc.d"

