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
	"SNPS2Debug/HudPCH_SN.obj"\
	"SNPS2Debug/screencontrols.obj"\
	"SNPS2Debug/screenmanager.obj"\
	"SNPS2Debug/hudobject.obj"\
	"SNPS2Debug/text3d.obj"\
	"SNPS2Debug/formattedfile.obj"\
	"SNPS2Debug/memorybank.obj"\
	"SNPS2Debug/treenode.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/HudPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/hud/HudPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/hud/HudPCH_SN.cpp" -o $@

"SNPS2Debug/screencontrols.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/hud/screencontrols.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/hud/screencontrols.cpp" -o $@

"SNPS2Debug/screenmanager.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/hud/screenmanager.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/hud/screenmanager.cpp" -o $@

"SNPS2Debug/hudobject.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/hud/hudobject.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/hud/hudobject.cpp" -o $@

"SNPS2Debug/text3d.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/hud/text3d.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/hud/text3d.cpp" -o $@

"SNPS2Debug/formattedfile.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/hud/formattedfile.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/hud/formattedfile.cpp" -o $@

"SNPS2Debug/memorybank.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/hud/memorybank.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/hud/memorybank.cpp" -o $@

"SNPS2Debug/treenode.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/hud/treenode.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/hud/treenode.cpp" -o $@

LIB: "SNPS2Debug/hud.lib"

"SNPS2Debug/hud.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/HudPCH_SN.d"
include "SNPS2Debug/screencontrols.d"
include "SNPS2Debug/screenmanager.d"
include "SNPS2Debug/hudobject.d"
include "SNPS2Debug/text3d.d"
include "SNPS2Debug/formattedfile.d"
include "SNPS2Debug/memorybank.d"
include "SNPS2Debug/treenode.d"

