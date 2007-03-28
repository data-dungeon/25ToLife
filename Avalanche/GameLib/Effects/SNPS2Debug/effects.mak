####################################################
# PS2 Makefile generated for GNU Make by ProDG VSI #
####################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Debug
INTDIR = SNPS2Debug

LIBS=\

AS=ps2cc
AS_FLAGS=\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-g\
	-Wa,-sn\

DVP=ps2cc
DVP_FLAGS=\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-g\
	-G0\
	-fno-exceptions\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\

CXX=ps2cc
CXX_FLAGS=\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-g\
	-G0\
	-fno-exceptions\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-fno-rtti\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\

AR=snarl

OBJS=\
	"SNPS2Debug/WaterPS2.obj"\
	"SNPS2Debug/EffectsPCH_SN.obj"\
	"SNPS2Debug/fsdfx.obj"\
	"SNPS2Debug/fsdsys.obj"\

COMPILE: $(OBJS)

"SNPS2Debug/WaterPS2.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Effects/Water/WaterPS2.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)/WaterPS2.d" "d:/Ttl/Avalanche/GameLib/Effects/Water/WaterPS2.dsm" -o $@

"SNPS2Debug/EffectsPCH_SN.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Effects/EffectsPCH_SN.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Effects/EffectsPCH_SN.cpp" -o $@

"SNPS2Debug/fsdfx.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Effects/fsdfx.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Effects/fsdfx.cpp" -o $@

"SNPS2Debug/fsdsys.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Effects/fsdsys.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Effects/fsdsys.cpp" -o $@

LIB: "SNPS2Debug/effects.lib"

"SNPS2Debug/effects.lib" : $(OBJS)
	@echo ProDG Creating Library $@
	$(AR) -M <"$(INTDIR)/_lib.txt"

