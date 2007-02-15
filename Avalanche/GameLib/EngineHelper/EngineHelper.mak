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
	"SNPS2Debug/crc.obj"\
	"SNPS2Debug/EngineHelperPCH_SN.obj"\
	"SNPS2Debug/EngineHelperScriptFunc.obj"\
	"SNPS2Debug/filestress.obj"\
	"SNPS2Debug/Hash.obj"\
	"SNPS2Debug/LoadScreen.obj"\
	"SNPS2Debug/sfx.obj"\
	"SNPS2Debug/simpleinst.obj"\
	"SNPS2Debug/soundcollide.obj"\
	"SNPS2Debug/streamtest.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/crc.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/crc.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/crc.cpp" -o $@

"SNPS2Debug/EngineHelperPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/EngineHelperPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/EngineHelperPCH_SN.cpp" -o $@

"SNPS2Debug/EngineHelperScriptFunc.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/EngineHelperScriptFunc.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/EngineHelperScriptFunc.cpp" -o $@

"SNPS2Debug/filestress.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/filestress.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/filestress.cpp" -o $@

"SNPS2Debug/Hash.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/Hash.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/Hash.cpp" -o $@

"SNPS2Debug/LoadScreen.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/LoadScreen.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/LoadScreen.cpp" -o $@

"SNPS2Debug/sfx.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/sfx.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/sfx.cpp" -o $@

"SNPS2Debug/simpleinst.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/simpleinst.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/simpleinst.cpp" -o $@

"SNPS2Debug/soundcollide.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/soundcollide.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/soundcollide.cpp" -o $@

"SNPS2Debug/streamtest.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/EngineHelper/streamtest.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/EngineHelper/streamtest.cpp" -o $@

LIB: "SNPS2Debug/EngineHelper.lib"

"SNPS2Debug/EngineHelper.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/crc.d"
include "SNPS2Debug/EngineHelperPCH_SN.d"
include "SNPS2Debug/EngineHelperScriptFunc.d"
include "SNPS2Debug/filestress.d"
include "SNPS2Debug/Hash.d"
include "SNPS2Debug/LoadScreen.d"
include "SNPS2Debug/sfx.d"
include "SNPS2Debug/simpleinst.d"
include "SNPS2Debug/soundcollide.d"
include "SNPS2Debug/streamtest.d"

