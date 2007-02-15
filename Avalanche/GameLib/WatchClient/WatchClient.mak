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
	"SNPS2Debug/MessageQueue.obj"\
	"SNPS2Debug/WatchClient.obj"\
	"SNPS2Debug/WatchClientGCN.obj"\
	"SNPS2Debug/WatchClientPS2.obj"\
	"SNPS2Debug/WatchClientWin32.obj"\
	"SNPS2Debug/watchclientxbox.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/MessageQueue.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/WatchClient/MessageQueue.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/WatchClient/MessageQueue.cpp" -o $@

"SNPS2Debug/WatchClient.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/WatchClient/WatchClient.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/WatchClient/WatchClient.cpp" -o $@

"SNPS2Debug/WatchClientGCN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/WatchClient/WatchClientGCN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/WatchClient/WatchClientGCN.cpp" -o $@

"SNPS2Debug/WatchClientPS2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/WatchClient/WatchClientPS2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/WatchClient/WatchClientPS2.cpp" -o $@

"SNPS2Debug/WatchClientWin32.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/WatchClient/WatchClientWin32.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/WatchClient/WatchClientWin32.cpp" -o $@

"SNPS2Debug/watchclientxbox.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/WatchClient/watchclientxbox.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/WatchClient/watchclientxbox.cpp" -o $@

LIB: "SNPS2Debug/WatchClient.lib"

"SNPS2Debug/WatchClient.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/MessageQueue.d"
include "SNPS2Debug/WatchClient.d"
include "SNPS2Debug/WatchClientGCN.d"
include "SNPS2Debug/WatchClientPS2.d"
include "SNPS2Debug/WatchClientWin32.d"
include "SNPS2Debug/watchclientxbox.d"

