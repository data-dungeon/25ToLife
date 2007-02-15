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
	-DSN_TARGET_PS2_RELAPP\
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
	-DSN_TARGET_PS2_RELAPP\
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
	-DSN_TARGET_PS2_RELAPP\
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
	-DSN_TARGET_PS2_RELAPP\
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
	"SNPS2Debug/PathPCH_SN.obj"\

COMPILE: $(OBJS)

"SNPS2Debug/PathPCH_SN.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/path/PathPCH_SN.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/path/PathPCH_SN.cpp" -o $@

LIB: "SNPS2Debug/path.lib"

"SNPS2Debug/path.lib" : $(OBJS)
	@echo ProDG Creating Library $@
	$(AR) -M <"$(INTDIR)/_lib.txt"

