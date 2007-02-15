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
	-DZLIB_ENABLED\
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
	-DZLIB_ENABLED\
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
	-DZLIB_ENABLED\
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
	"SNPS2Debug/minilzo.obj"\
	"SNPS2Debug/blowfish.obj"\
	"SNPS2Debug/rijndael.obj"\
	"SNPS2Debug/Helix.obj"\
	"SNPS2Debug/Creator.obj"\
	"SNPS2Debug/DebugOutput.obj"\
	"SNPS2Debug/ErrorStack.obj"\
	"SNPS2Debug/FileLoader.obj"\
	"SNPS2Debug/GameHelperPCH_SN.obj"\
	"SNPS2Debug/GameLocks.obj"\
	"SNPS2Debug/LogUtil.obj"\
	"SNPS2Debug/Message.obj"\
	"SNPS2Debug/ParamMgr.obj"\
	"SNPS2Debug/SharedBody.obj"\
	"SNPS2Debug/Sticky.obj"\
	"SNPS2Debug/stringHelper.obj"\
	"SNPS2Debug/Translator.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/minilzo.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/compression/Lzo/minilzo.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/compression/Lzo/minilzo.cpp" -o $@

"SNPS2Debug/blowfish.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/encryption/Blowfish/blowfish.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/encryption/Blowfish/blowfish.cpp" -o $@

"SNPS2Debug/rijndael.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/encryption/Rijndael/rijndael.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/encryption/Rijndael/rijndael.cpp" -o $@

"SNPS2Debug/Helix.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/encryption/Helix/Helix.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/encryption/Helix/Helix.cpp" -o $@

"SNPS2Debug/Creator.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/Creator.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/Creator.cpp" -o $@

"SNPS2Debug/DebugOutput.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/DebugOutput.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/DebugOutput.cpp" -o $@

"SNPS2Debug/ErrorStack.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/ErrorStack.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/ErrorStack.cpp" -o $@

"SNPS2Debug/FileLoader.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/FileLoader.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/FileLoader.cpp" -o $@

"SNPS2Debug/GameHelperPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/GameHelperPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/GameHelperPCH_SN.cpp" -o $@

"SNPS2Debug/GameLocks.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/GameLocks.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/GameLocks.cpp" -o $@

"SNPS2Debug/LogUtil.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/LogUtil.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/LogUtil.cpp" -o $@

"SNPS2Debug/Message.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/Message.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/Message.cpp" -o $@

"SNPS2Debug/ParamMgr.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/ParamMgr.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/ParamMgr.cpp" -o $@

"SNPS2Debug/SharedBody.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/SharedBody.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/SharedBody.cpp" -o $@

"SNPS2Debug/Sticky.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/Sticky.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/Sticky.cpp" -o $@

"SNPS2Debug/stringHelper.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/stringHelper.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/stringHelper.cpp" -o $@

"SNPS2Debug/Translator.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/GameHelper/Translator.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/GameHelper/Translator.cpp" -o $@

LIB: "SNPS2Debug/GameHelper.lib"

"SNPS2Debug/GameHelper.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/minilzo.d"
include "SNPS2Debug/blowfish.d"
include "SNPS2Debug/rijndael.d"
include "SNPS2Debug/Helix.d"
include "SNPS2Debug/Creator.d"
include "SNPS2Debug/DebugOutput.d"
include "SNPS2Debug/ErrorStack.d"
include "SNPS2Debug/FileLoader.d"
include "SNPS2Debug/GameHelperPCH_SN.d"
include "SNPS2Debug/GameLocks.d"
include "SNPS2Debug/LogUtil.d"
include "SNPS2Debug/Message.d"
include "SNPS2Debug/ParamMgr.d"
include "SNPS2Debug/SharedBody.d"
include "SNPS2Debug/Sticky.d"
include "SNPS2Debug/stringHelper.d"
include "SNPS2Debug/Translator.d"

