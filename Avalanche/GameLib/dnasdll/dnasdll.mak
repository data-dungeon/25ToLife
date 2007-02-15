########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Debug
INTDIR = SNPS2Debug

LCFILE = "c:/dev/Avalanche/GameLib/dnasdll/rel.cmd"
LIBS=\
	 "dnas_net.a"\

AS=ps2cc
AS_FLAGS=\
	-Wa,-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include"\
	-I"C:/Dev/PS2/Sony/sce-302/common/include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_mc_300_scea/ee/include"\
	-I. -c\
	-fdevstudio\
	-g\

DVP=ps2cc
DVP_FLAGS=\
	-Wd,-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include"\
	-I"C:/Dev/PS2/Sony/sce-302/common/include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_mc_300_scea/ee/include"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include"\
	-I"C:/Dev/PS2/Sony/sce-302/common/include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_mc_300_scea/ee/include"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-G0\
	-g\
	-Wall\
	-fno-exceptions\

CXX_FLAGS=$(CC_FLAGS)

LD=ps2dlllk
LD_FLAGS=\
	"c:\dev\Avalanche\GameLib\Osi\Ps2\rel.lk" ps2cc.exe @_ps2cc.txt

LD_OBJS=\
	"SNPS2Debug/do_netstart_dll.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/do_netstart_dll.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/do_netstart_dll.c"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/do_netstart_dll.c" -o $@

LINK: "SNPS2Debug/dnasdll.elf"

"SNPS2Debug/dnasdll.elf" : $(LD_OBJS)
	@echo ProDG Linking $@
	$(LD) $(LD_FLAGS) $(LD_OBJS) -o $@

include "SNPS2Debug/do_netstart_dll.d"

