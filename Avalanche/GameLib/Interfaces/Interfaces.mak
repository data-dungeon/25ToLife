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
	-I"C:/Dev/Avalanche/Gamelib"\
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
	-I"C:/Dev/Avalanche/Gamelib"\
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
	-I"C:/Dev/Avalanche/Gamelib"\
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
	"SNPS2Debug/BasicMoverIF.obj"\
	"SNPS2Debug/CollisionIF.obj"\
	"SNPS2Debug/InterfacesPCH_SN.obj"\
	"SNPS2Debug/InventoryListIF.obj"\
	"SNPS2Debug/RigidBodyIF.obj"\
	"SNPS2Debug/SurfaceTypeIF.obj"\
	"SNPS2Debug/ViewIF.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/BasicMoverIF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Interfaces/BasicMoverIF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Interfaces/BasicMoverIF.cpp" -o $@

"SNPS2Debug/CollisionIF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Interfaces/CollisionIF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Interfaces/CollisionIF.cpp" -o $@

"SNPS2Debug/InterfacesPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Interfaces/InterfacesPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Interfaces/InterfacesPCH_SN.cpp" -o $@

"SNPS2Debug/InventoryListIF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Interfaces/InventoryListIF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Interfaces/InventoryListIF.cpp" -o $@

"SNPS2Debug/RigidBodyIF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Interfaces/RigidBodyIF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Interfaces/RigidBodyIF.cpp" -o $@

"SNPS2Debug/SurfaceTypeIF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Interfaces/SurfaceTypeIF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Interfaces/SurfaceTypeIF.cpp" -o $@

"SNPS2Debug/ViewIF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Interfaces/ViewIF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Interfaces/ViewIF.cpp" -o $@

LIB: "SNPS2Debug/Interfaces.lib"

"SNPS2Debug/Interfaces.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/BasicMoverIF.d"
include "SNPS2Debug/CollisionIF.d"
include "SNPS2Debug/InterfacesPCH_SN.d"
include "SNPS2Debug/InventoryListIF.d"
include "SNPS2Debug/RigidBodyIF.d"
include "SNPS2Debug/SurfaceTypeIF.d"
include "SNPS2Debug/ViewIF.d"

