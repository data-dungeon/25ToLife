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
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/Avalanche/Engine/Display"\
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
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/Avalanche/Engine/Display"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-g\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/Avalanche/Engine/Display"\
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
	"SNPS2Debug/newparticle.obj"\
	"SNPS2Debug/Particulate.obj"\
	"SNPS2Debug/Sprites.obj"\
	"SNPS2Debug/VUStripBuffer1x2CD.obj"\
	"SNPS2Debug/VUStripBuffer1x3CD.obj"\
	"SNPS2Debug/VUStripBuffer2x2CD.obj"\
	"SNPS2Debug/VUStripBuffer3x2CD.obj"\
	"SNPS2Debug/VUStripBuffer4x2CD.obj"\
	"SNPS2Debug/VUStripBuffer5x2CD.obj"\
	"SNPS2Debug/VUStripBuffer6x2CD.obj"\
	"SNPS2Debug/VUStripBuffer7x2CD.obj"\
	"SNPS2Debug/VUStripBuffer8x2CD.obj"\
	"SNPS2Debug/DisplayPCH_SN.obj"\
	"SNPS2Debug/TextureUtil.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/newparticle.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/newparticle.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\newparticle.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/newparticle.dsm" -o $@

"SNPS2Debug/Particulate.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/Particulate.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\Particulate.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/Particulate.dsm" -o $@

"SNPS2Debug/Sprites.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/Sprites.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\Sprites.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/Sprites.dsm" -o $@

"SNPS2Debug/VUStripBuffer1x2CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer1x2CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer1x2CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer1x2CD.dsm" -o $@

"SNPS2Debug/VUStripBuffer1x3CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer1x3CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer1x3CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer1x3CD.dsm" -o $@

"SNPS2Debug/VUStripBuffer2x2CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer2x2CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer2x2CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer2x2CD.dsm" -o $@

"SNPS2Debug/VUStripBuffer3x2CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer3x2CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer3x2CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer3x2CD.dsm" -o $@

"SNPS2Debug/VUStripBuffer4x2CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer4x2CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer4x2CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer4x2CD.dsm" -o $@

"SNPS2Debug/VUStripBuffer5x2CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer5x2CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer5x2CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer5x2CD.dsm" -o $@

"SNPS2Debug/VUStripBuffer6x2CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer6x2CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer6x2CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer6x2CD.dsm" -o $@

"SNPS2Debug/VUStripBuffer7x2CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer7x2CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer7x2CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer7x2CD.dsm" -o $@

"SNPS2Debug/VUStripBuffer8x2CD.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer8x2CD.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)\VUStripBuffer8x2CD.d" "c:/dev/Avalanche/Engine/Display/Ps2/VUCode/VUStripBuffer8x2CD.dsm" -o $@

"SNPS2Debug/DisplayPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/DisplayPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Display/DisplayPCH_SN.cpp" -o $@

"SNPS2Debug/TextureUtil.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Display/TextureUtil.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Display/TextureUtil.cpp" -o $@

LIB: "SNPS2Debug/Display.lib"

"SNPS2Debug/Display.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/newparticle.d"
include "SNPS2Debug/Particulate.d"
include "SNPS2Debug/Sprites.d"
include "SNPS2Debug/VUStripBuffer1x2CD.d"
include "SNPS2Debug/VUStripBuffer1x3CD.d"
include "SNPS2Debug/VUStripBuffer2x2CD.d"
include "SNPS2Debug/VUStripBuffer3x2CD.d"
include "SNPS2Debug/VUStripBuffer4x2CD.d"
include "SNPS2Debug/VUStripBuffer5x2CD.d"
include "SNPS2Debug/VUStripBuffer6x2CD.d"
include "SNPS2Debug/VUStripBuffer7x2CD.d"
include "SNPS2Debug/VUStripBuffer8x2CD.d"
include "SNPS2Debug/DisplayPCH_SN.d"
include "SNPS2Debug/TextureUtil.d"

