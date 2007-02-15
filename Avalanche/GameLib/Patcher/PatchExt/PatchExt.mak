########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Debug
INTDIR = SNPS2Debug

LCFILE = "c:/dev/Avalanche/GameLib/Patcher/PatchExt/rel.cmd"
LIBS=\

AS=ps2cc
AS_FLAGS=\
	-Wa,-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DPS2\
	-DSN\
	-I"../share"\
	-I"C:/Dev/Avalanche/Gamelib/GameHelper/compression"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I. -c\
	-fdevstudio\
	-g\
	-Wa,-sn\

DVP=ps2cc
DVP_FLAGS=\
	-Wd,-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DPS2\
	-DSN\
	-I"../share"\
	-I"C:/Dev/Avalanche/Gamelib/GameHelper/compression"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DPS2\
	-DSN\
	-I"../share"\
	-I"C:/Dev/Avalanche/Gamelib/GameHelper/compression"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-g\
	-O0\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-fno-rtti\
	-fno-exceptions\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\
	-G0\

CXX_FLAGS=$(CC_FLAGS)

LD=ps2dlllk
LD_FLAGS=\
	"c:\dev\Avalanche\GameLib\Patcher\PatchExt\rel.lk" ps2cc.exe @_ps2cc.txt

LD_OBJS=\
	"SNPS2Debug/ContentTable.obj"\
	"SNPS2Debug/ContentTable_ps2.obj"\
	"SNPS2Debug/md5.obj"\
	"SNPS2Debug/PatchExtractor.obj"\
	"SNPS2Debug/PatchExtractor_ps2.obj"\
	"SNPS2Debug/PatchHelper.obj"\
	"SNPS2Debug/PatchReader.obj"\
	"SNPS2Debug/PatchReader_ps2.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/ContentTable.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Patcher/PatchExt/ContentTable.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Patcher/PatchExt/ContentTable.cpp" -o $@

"SNPS2Debug/ContentTable_ps2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Patcher/PatchExt/ContentTable_ps2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Patcher/PatchExt/ContentTable_ps2.cpp" -o $@

"SNPS2Debug/md5.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Patcher/share/md5.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Patcher/share/md5.cpp" -o $@

"SNPS2Debug/PatchExtractor.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Patcher/PatchExt/PatchExtractor.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Patcher/PatchExt/PatchExtractor.cpp" -o $@

"SNPS2Debug/PatchExtractor_ps2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Patcher/PatchExt/PatchExtractor_ps2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Patcher/PatchExt/PatchExtractor_ps2.cpp" -o $@

"SNPS2Debug/PatchHelper.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Patcher/share/PatcherCommon/PatchHelper.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Patcher/share/PatcherCommon/PatchHelper.cpp" -o $@

"SNPS2Debug/PatchReader.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Patcher/PatchExt/PatchReader.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Patcher/PatchExt/PatchReader.cpp" -o $@

"SNPS2Debug/PatchReader_ps2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Patcher/PatchExt/PatchReader_ps2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Patcher/PatchExt/PatchReader_ps2.cpp" -o $@

LINK: "SNPS2Debug/PatchExt.elf"

"SNPS2Debug/PatchExt.elf" : $(LD_OBJS)
	@echo ProDG Linking $@
	$(LD) $(LD_FLAGS) $(LD_OBJS) -o $@

include "SNPS2Debug/ContentTable.d"
include "SNPS2Debug/ContentTable_ps2.d"
include "SNPS2Debug/md5.d"
include "SNPS2Debug/PatchExtractor.d"
include "SNPS2Debug/PatchExtractor_ps2.d"
include "SNPS2Debug/PatchHelper.d"
include "SNPS2Debug/PatchReader.d"
include "SNPS2Debug/PatchReader_ps2.d"

