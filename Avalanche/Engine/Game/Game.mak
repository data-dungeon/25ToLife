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
	-I"C:/Dev/Avalanche/GameLib/Patcher/share"\
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
	-I"C:/Dev/Avalanche/GameLib/Patcher/share"\
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
	-I"C:/Dev/Avalanche/GameLib/Patcher/share"\
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
	"SNPS2Debug/OccluderMgr.obj"\
	"SNPS2Debug/replayMgr.obj"\
	"SNPS2Debug/ResourceManager.obj"\
	"SNPS2Debug/TextureCache.obj"\
	"SNPS2Debug/ActorInstance.obj"\
	"SNPS2Debug/GamePCH_SN.obj"\
	"SNPS2Debug/GameScriptFunc.obj"\
	"SNPS2Debug/ParameterSet.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/OccluderMgr.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Game/Managers/OccluderMgr.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Game/Managers/OccluderMgr.cpp" -o $@

"SNPS2Debug/replayMgr.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Game/Managers/replayMgr.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Game/Managers/replayMgr.cpp" -o $@

"SNPS2Debug/ResourceManager.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Game/Managers/ResourceManager.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Game/Managers/ResourceManager.cpp" -o $@

"SNPS2Debug/TextureCache.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Game/Managers/TextureMgr/TextureCache.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Game/Managers/TextureMgr/TextureCache.cpp" -o $@

"SNPS2Debug/ActorInstance.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Game/Misc/ActorInstance.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Game/Misc/ActorInstance.cpp" -o $@

"SNPS2Debug/GamePCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Game/GamePCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Game/GamePCH_SN.cpp" -o $@

"SNPS2Debug/GameScriptFunc.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Game/GameScriptFunc.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Game/GameScriptFunc.cpp" -o $@

"SNPS2Debug/ParameterSet.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Game/Misc/ParameterSet.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Game/Misc/ParameterSet.cpp" -o $@

LIB: "SNPS2Debug/Game.lib"

"SNPS2Debug/Game.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/OccluderMgr.d"
include "SNPS2Debug/replayMgr.d"
include "SNPS2Debug/ResourceManager.d"
include "SNPS2Debug/TextureCache.d"
include "SNPS2Debug/ActorInstance.d"
include "SNPS2Debug/GamePCH_SN.d"
include "SNPS2Debug/GameScriptFunc.d"
include "SNPS2Debug/ParameterSet.d"

