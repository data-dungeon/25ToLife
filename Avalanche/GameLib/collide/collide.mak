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
	"SNPS2Debug/Capsule.obj"\
	"SNPS2Debug/CapsuleToAabb.obj"\
	"SNPS2Debug/CapsuleToCapsule.obj"\
	"SNPS2Debug/CapsuleToObb.obj"\
	"SNPS2Debug/CapsuleToTerrain.obj"\
	"SNPS2Debug/CollidePCH_SN.obj"\
	"SNPS2Debug/LineToCapsule.obj"\
	"SNPS2Debug/LineToTriangleList.obj"\
	"SNPS2Debug/Object.obj"\
	"SNPS2Debug/Pair.obj"\
	"SNPS2Debug/SphereToCapsule.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/Capsule.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/Capsule.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/Capsule.cpp" -o $@

"SNPS2Debug/CapsuleToAabb.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/CapsuleToAabb.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/CapsuleToAabb.cpp" -o $@

"SNPS2Debug/CapsuleToCapsule.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/CapsuleToCapsule.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/CapsuleToCapsule.cpp" -o $@

"SNPS2Debug/CapsuleToObb.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/CapsuleToObb.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/CapsuleToObb.cpp" -o $@

"SNPS2Debug/CapsuleToTerrain.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/CapsuleToTerrain.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/CapsuleToTerrain.cpp" -o $@

"SNPS2Debug/CollidePCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/CollidePCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/CollidePCH_SN.cpp" -o $@

"SNPS2Debug/LineToCapsule.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/LineToCapsule.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/LineToCapsule.cpp" -o $@

"SNPS2Debug/LineToTriangleList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/LineToTriangleList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/LineToTriangleList.cpp" -o $@

"SNPS2Debug/Object.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/Object.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/Object.cpp" -o $@

"SNPS2Debug/Pair.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/Pair.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/Pair.cpp" -o $@

"SNPS2Debug/SphereToCapsule.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/collide/SphereToCapsule.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/collide/SphereToCapsule.cpp" -o $@

LIB: "SNPS2Debug/collide.lib"

"SNPS2Debug/collide.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/Capsule.d"
include "SNPS2Debug/CapsuleToAabb.d"
include "SNPS2Debug/CapsuleToCapsule.d"
include "SNPS2Debug/CapsuleToObb.d"
include "SNPS2Debug/CapsuleToTerrain.d"
include "SNPS2Debug/CollidePCH_SN.d"
include "SNPS2Debug/LineToCapsule.d"
include "SNPS2Debug/LineToTriangleList.d"
include "SNPS2Debug/Object.d"
include "SNPS2Debug/Pair.d"
include "SNPS2Debug/SphereToCapsule.d"

