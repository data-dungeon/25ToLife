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
	"SNPS2Debug/CapsuleBonesNode.obj"\
	"SNPS2Debug/CapsuleNode.obj"\
	"SNPS2Debug/EventMiner.obj"\
	"SNPS2Debug/LineEntity.obj"\
	"SNPS2Debug/ObbBonesNode.obj"\
	"SNPS2Debug/obbchild.obj"\
	"SNPS2Debug/ObbNode.obj"\
	"SNPS2Debug/ProtagonistNode.obj"\
	"SNPS2Debug/SightEntity.obj"\
	"SNPS2Debug/XZEntity.obj"\
	"SNPS2Debug/zone.obj"\
	"SNPS2Debug/zonemap.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/CapsuleBonesNode.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/CapsuleBonesNode.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/CapsuleBonesNode.cpp" -o $@

"SNPS2Debug/CapsuleNode.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/CapsuleNode.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/CapsuleNode.cpp" -o $@

"SNPS2Debug/EventMiner.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/EventMiner.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/EventMiner.cpp" -o $@

"SNPS2Debug/LineEntity.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/LineEntity.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/LineEntity.cpp" -o $@

"SNPS2Debug/ObbBonesNode.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/ObbBonesNode.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/ObbBonesNode.cpp" -o $@

"SNPS2Debug/obbchild.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/obbchild.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/obbchild.cpp" -o $@

"SNPS2Debug/ObbNode.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/ObbNode.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/ObbNode.cpp" -o $@

"SNPS2Debug/ProtagonistNode.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/ProtagonistNode.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/ProtagonistNode.cpp" -o $@

"SNPS2Debug/SightEntity.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/SightEntity.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/SightEntity.cpp" -o $@

"SNPS2Debug/XZEntity.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/XZEntity.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/XZEntity.cpp" -o $@

"SNPS2Debug/zone.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/zone.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/zone.cpp" -o $@

"SNPS2Debug/zonemap.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/CollideHelper/zonemap.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/CollideHelper/zonemap.cpp" -o $@

LIB: "SNPS2Debug/CollideHelper.lib"

"SNPS2Debug/CollideHelper.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/CapsuleBonesNode.d"
include "SNPS2Debug/CapsuleNode.d"
include "SNPS2Debug/EventMiner.d"
include "SNPS2Debug/LineEntity.d"
include "SNPS2Debug/ObbBonesNode.d"
include "SNPS2Debug/obbchild.d"
include "SNPS2Debug/ObbNode.d"
include "SNPS2Debug/ProtagonistNode.d"
include "SNPS2Debug/SightEntity.d"
include "SNPS2Debug/XZEntity.d"
include "SNPS2Debug/zone.d"
include "SNPS2Debug/zonemap.d"

