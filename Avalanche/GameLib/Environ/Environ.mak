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
	"SNPS2Debug/ActorInitializer.obj"\
	"SNPS2Debug/EnvironPCH_SN.obj"\
	"SNPS2Debug/Munition.obj"\
	"SNPS2Debug/World.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/ActorInitializer.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Environ/ActorInitializer.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Environ/ActorInitializer.cpp" -o $@

"SNPS2Debug/EnvironPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Environ/EnvironPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Environ/EnvironPCH_SN.cpp" -o $@

"SNPS2Debug/Munition.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Environ/Munition.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Environ/Munition.cpp" -o $@

"SNPS2Debug/World.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Environ/World.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Environ/World.cpp" -o $@

LIB: "SNPS2Debug/Environ.lib"

"SNPS2Debug/Environ.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/ActorInitializer.d"
include "SNPS2Debug/EnvironPCH_SN.d"
include "SNPS2Debug/Munition.d"
include "SNPS2Debug/World.d"

