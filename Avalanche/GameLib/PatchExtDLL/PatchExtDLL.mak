########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2CDRom
INTDIR = SNPS2CDRom

LCFILE = "c:/dev/Avalanche/GameLib/PatchExtDLL/rel.cmd"
LIBS=\

AS=ps2cc
AS_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-DPS2\
	-DSN\
	-DNDEBUG\
	-DCDROM\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include"\
	-I"C:/Dev/PS2/Sony/sce-302/common/include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_nohdd_300_scea/ee/include"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I. -c\
	-fdevstudio\
	-Wa,-sn\

DVP=ps2cc
DVP_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-DPS2\
	-DSN\
	-DNDEBUG\
	-DCDROM\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include"\
	-I"C:/Dev/PS2/Sony/sce-302/common/include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_nohdd_300_scea/ee/include"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I. -fdevstudio -c\

CC=ps2cc
CC_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-DPS2\
	-DSN\
	-DNDEBUG\
	-DCDROM\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include"\
	-I"C:/Dev/PS2/Sony/sce-302/common/include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_nohdd_300_scea/ee/include"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-O2\
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
	"c:\dev\Avalanche\GameLib\PatchExtDLL\rel.lk" ps2cc.exe @_ps2cc.txt

LD_OBJS=\
	"SNPS2CDRom/ContentTable.obj"\
	"SNPS2CDRom/md5.obj"\
	"SNPS2CDRom/PatchExtractor.obj"\
	"SNPS2CDRom/PatchHelper.obj"\
	"SNPS2CDRom/PatchReader.obj"\

COMPILE: $(LD_OBJS)

"SNPS2CDRom/ContentTable.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/PatchExtDLL/ContentTable.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/PatchExtDLL/ContentTable.cpp" -o $@

"SNPS2CDRom/md5.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/PatchExtDLL/md5.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/PatchExtDLL/md5.cpp" -o $@

"SNPS2CDRom/PatchExtractor.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/PatchExtDLL/PatchExtractor.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/PatchExtDLL/PatchExtractor.cpp" -o $@

"SNPS2CDRom/PatchHelper.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/PatchCommon/PatchHelper.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/PatchCommon/PatchHelper.cpp" -o $@

"SNPS2CDRom/PatchReader.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/PatchExtDLL/PatchReader.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/PatchExtDLL/PatchReader.cpp" -o $@

LINK: "SNPS2CDRom/PatchExt.elf"

"SNPS2CDRom/PatchExt.elf" : $(LD_OBJS)
	@echo ProDG Linking $@
	$(LD) $(LD_FLAGS) $(LD_OBJS) -o $@


