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
	"SNPS2Consumer/PathPCH_SN.obj"\

COMPILE: $(OBJS)

"SNPS2Consumer/PathPCH_SN.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/path/PathPCH_SN.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/path/PathPCH_SN.cpp" -o $@

LIB: "SNPS2Consumer/path.lib"

"SNPS2Consumer/path.lib" : $(OBJS)
	@echo ProDG Creating Library $@
	$(AR) -M <"$(INTDIR)/_lib.txt"

