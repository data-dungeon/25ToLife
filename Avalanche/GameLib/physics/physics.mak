########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Release
INTDIR = SNPS2Release

LIBS=\

AS=ps2cc
AS_FLAGS=\
	-DSN_TARGET_PS2\
	-DPS2\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I. -fdevstudio -c\
	-Wa,-sn\

DVP=ps2cc
DVP_FLAGS=\
	-DSN_TARGET_PS2\
	-DPS2\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I. -fdevstudio -c\

CC=ps2cc
CC_FLAGS=\
	-DSN_TARGET_PS2\
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
	-Wl,-strip-unused\
	-Wl,-strip-unused-data\
	-mvu0-use-vf0-vf15\
	-g\

CXX_FLAGS=$(CC_FLAGS)

LD_OBJS=\
	"SNPS2Release/PhysicsPCH_SN.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Release/PhysicsPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/physics/PhysicsPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/physics/PhysicsPCH_SN.cpp" -o $@

LIB: "SNPS2Release/physics.lib"

"SNPS2Release/physics.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif


