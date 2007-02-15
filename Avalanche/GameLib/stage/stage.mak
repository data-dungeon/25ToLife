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
	-DDSKINNER\
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
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
	-DDSKINNER\
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-g\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DDSKINNER\
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
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
	"SNPS2Debug/ActorCreator.obj"\
	"SNPS2Debug/CCompActor.obj"\
	"SNPS2Debug/StagePCH_SN.obj"\
	"SNPS2Debug/StageScriptFunc.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/ActorCreator.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/stage/ActorCreator.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/stage/ActorCreator.cpp" -o $@

"SNPS2Debug/CCompActor.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/stage/CCompActor.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/stage/CCompActor.cpp" -o $@

"SNPS2Debug/StagePCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/stage/StagePCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/stage/StagePCH_SN.cpp" -o $@

"SNPS2Debug/StageScriptFunc.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/stage/StageScriptFunc.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/stage/StageScriptFunc.cpp" -o $@

LIB: "SNPS2Debug/stage.lib"

"SNPS2Debug/stage.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/ActorCreator.d"
include "SNPS2Debug/CCompActor.d"
include "SNPS2Debug/StagePCH_SN.d"
include "SNPS2Debug/StageScriptFunc.d"

