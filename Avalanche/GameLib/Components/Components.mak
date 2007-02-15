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
	-I"C:/Dev/Avalanche/Gamelib"\
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
	-I"C:/Dev/Avalanche/Gamelib"\
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
	-I"C:/Dev/Avalanche/Gamelib"\
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
	"SNPS2Debug/weapdep.obj"\
	"SNPS2Debug/ActorList.obj"\
	"SNPS2Debug/ComponentsPCH_SN.obj"\
	"SNPS2Debug/InstantMover.obj"\
	"SNPS2Debug/InventoryList.obj"\
	"SNPS2Debug/PathFollowerCam.obj"\
	"SNPS2Debug/Rider.obj"\
	"SNPS2Debug/Slave.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/weapdep.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Components/weapons/weapdep.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Components/weapons/weapdep.cpp" -o $@

"SNPS2Debug/ActorList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Components/ActorList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Components/ActorList.cpp" -o $@

"SNPS2Debug/ComponentsPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Components/ComponentsPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Components/ComponentsPCH_SN.cpp" -o $@

"SNPS2Debug/InstantMover.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Components/InstantMover.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Components/InstantMover.cpp" -o $@

"SNPS2Debug/InventoryList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Components/InventoryList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Components/InventoryList.cpp" -o $@

"SNPS2Debug/PathFollowerCam.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Components/PathFollowerCam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Components/PathFollowerCam.cpp" -o $@

"SNPS2Debug/Rider.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Components/Rider.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Components/Rider.cpp" -o $@

"SNPS2Debug/Slave.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Components/Slave.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Components/Slave.cpp" -o $@

LIB: "SNPS2Debug/Components.lib"

"SNPS2Debug/Components.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/weapdep.d"
include "SNPS2Debug/ActorList.d"
include "SNPS2Debug/ComponentsPCH_SN.d"
include "SNPS2Debug/InstantMover.d"
include "SNPS2Debug/InventoryList.d"
include "SNPS2Debug/PathFollowerCam.d"
include "SNPS2Debug/Rider.d"
include "SNPS2Debug/Slave.d"

