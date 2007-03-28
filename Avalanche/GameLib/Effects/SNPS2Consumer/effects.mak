####################################################
# PS2 Makefile generated for GNU Make by ProDG VSI #
####################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Consumer
INTDIR = SNPS2Consumer

LIBS=\

AS=ps2cc
AS_FLAGS=\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-DCDROM\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-g\
	-Wa,-sn\

DVP=ps2cc
DVP_FLAGS=\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-DCDROM\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-DCDROM\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-g\
	-fno-exceptions\
	-Os\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\

CXX=ps2cc
CXX_FLAGS=\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-DCDROM\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-g\
	-fno-exceptions\
	-Os\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-fno-rtti\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\

AR=snarl

OBJS=\
	"SNPS2Consumer/WaterPS2.obj"\
	"SNPS2Consumer/EffectsPCH_SN.obj"\
	"SNPS2Consumer/fsdfx.obj"\
	"SNPS2Consumer/fsdsys.obj"\

COMPILE: $(OBJS)

"SNPS2Consumer/WaterPS2.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Effects/Water/WaterPS2.dsm"
	$(DVP) $(DVP_FLAGS) -Wd,-MD,"$(INTDIR)/WaterPS2.d" "d:/Ttl/Avalanche/GameLib/Effects/Water/WaterPS2.dsm" -o $@

"SNPS2Consumer/EffectsPCH_SN.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Effects/EffectsPCH_SN.cpp"
	$(CXX)\
	-DSN\
	-DSN_TARGET_PS2\
	-DPS2\
	-DJJS\
	-DCDROM\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/GameLib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-g -G0 -fno-exceptions -Os -Wno-inline-static-semantics -Wa,-sn -fno-rtti -mno-check-zero-division -ffast-math -mvu0-use-vf0-vf15\
	 "d:/Ttl/Avalanche/GameLib/Effects/EffectsPCH_SN.cpp" -o $@

"SNPS2Consumer/fsdfx.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Effects/fsdfx.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Effects/fsdfx.cpp" -o $@

"SNPS2Consumer/fsdsys.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Effects/fsdsys.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Effects/fsdsys.cpp" -o $@

LIB: "SNPS2Consumer/effects.lib"

"SNPS2Consumer/effects.lib" : $(OBJS)
	@echo ProDG Creating Library $@
	$(AR) -M <"$(INTDIR)/_lib.txt"

