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
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/PS2/Sony/sce-302/iop/include"\
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
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/PS2/Sony/sce-302/iop/include"\
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
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/PS2/Sony/sce-302/iop/include"\
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
	"SNPS2Debug/PS2Keyboard.obj"\
	"SNPS2Debug/PS2Mouse.obj"\
	"SNPS2Debug/util.obj"\
	"SNPS2Debug/MemoryCard.obj"\
	"SNPS2Debug/MemoryCard_ps2.obj"\
	"SNPS2Debug/cfocus.obj"\
	"SNPS2Debug/CMouse.obj"\
	"SNPS2Debug/defcontrolmap.obj"\
	"SNPS2Debug/PlatformPCH_SN.obj"\
	"SNPS2Debug/wantpal.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/PS2Keyboard.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/Ps2/PS2Keyboard.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/Ps2/PS2Keyboard.cpp" -o $@

"SNPS2Debug/PS2Mouse.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/Ps2/PS2Mouse.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/Ps2/PS2Mouse.cpp" -o $@

"SNPS2Debug/util.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/Ps2/util.c"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/Ps2/util.c" -o $@

"SNPS2Debug/MemoryCard.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/MemoryCard/MemoryCard.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/MemoryCard/MemoryCard.cpp" -o $@

"SNPS2Debug/MemoryCard_ps2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/MemoryCard/MemoryCard_ps2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/MemoryCard/MemoryCard_ps2.cpp" -o $@

"SNPS2Debug/cfocus.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/cfocus.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/cfocus.cpp" -o $@

"SNPS2Debug/CMouse.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/CMouse.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/CMouse.cpp" -o $@

"SNPS2Debug/defcontrolmap.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/defcontrolmap.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/defcontrolmap.cpp" -o $@

"SNPS2Debug/PlatformPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/PlatformPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/PlatformPCH_SN.cpp" -o $@

"SNPS2Debug/wantpal.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/platform/wantpal.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/platform/wantpal.cpp" -o $@

LIB: "SNPS2Debug/platform.lib"

"SNPS2Debug/platform.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/PS2Keyboard.d"
include "SNPS2Debug/PS2Mouse.d"
include "SNPS2Debug/util.d"
include "SNPS2Debug/MemoryCard.d"
include "SNPS2Debug/MemoryCard_ps2.d"
include "SNPS2Debug/cfocus.d"
include "SNPS2Debug/CMouse.d"
include "SNPS2Debug/defcontrolmap.d"
include "SNPS2Debug/PlatformPCH_SN.d"
include "SNPS2Debug/wantpal.d"

