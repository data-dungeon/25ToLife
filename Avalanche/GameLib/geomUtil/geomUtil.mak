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
	"SNPS2Debug/AnimCurve.obj"\
	"SNPS2Debug/BoxExtents.obj"\
	"SNPS2Debug/GeomUtilPCH.obj"\
	"SNPS2Debug/GeomUtilPCH_SN.obj"\
	"SNPS2Debug/LineToLine.obj"\
	"SNPS2Debug/Spline.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/AnimCurve.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/geomUtil/AnimCurve.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/geomUtil/AnimCurve.cpp" -o $@

"SNPS2Debug/BoxExtents.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/geomUtil/BoxExtents.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/geomUtil/BoxExtents.cpp" -o $@

"SNPS2Debug/GeomUtilPCH.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/geomUtil/GeomUtilPCH.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/geomUtil/GeomUtilPCH.cpp" -o $@

"SNPS2Debug/GeomUtilPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/geomUtil/GeomUtilPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/geomUtil/GeomUtilPCH_SN.cpp" -o $@

"SNPS2Debug/LineToLine.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/geomUtil/LineToLine.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/geomUtil/LineToLine.cpp" -o $@

"SNPS2Debug/Spline.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/geomUtil/Spline.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/geomUtil/Spline.cpp" -o $@

LIB: "SNPS2Debug/geomUtil.lib"

"SNPS2Debug/geomUtil.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/AnimCurve.d"
include "SNPS2Debug/BoxExtents.d"
include "SNPS2Debug/GeomUtilPCH.d"
include "SNPS2Debug/GeomUtilPCH_SN.d"
include "SNPS2Debug/LineToLine.d"
include "SNPS2Debug/Spline.d"

