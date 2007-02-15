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
	-I"C:/Dev/Avalanche/GameLib/Patcher/share"\
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
	-I"C:/Dev/Avalanche/GameLib/Patcher/share"\
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
	-I"C:/Dev/Avalanche/GameLib/Patcher/share"\
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
	"SNPS2Debug/thread.obj"\
	"SNPS2Debug/DiscError.obj"\
	"SNPS2Debug/DiscError_GCN.obj"\
	"SNPS2Debug/DiscError_ps2.obj"\
	"SNPS2Debug/DiscError_win32.obj"\
	"SNPS2Debug/DiscError_xbox.obj"\
	"SNPS2Debug/Semaphore.obj"\
	"SNPS2Debug/OSThread.obj"\
	"SNPS2Debug/contps2.obj"\
	"SNPS2Debug/LayersPCH_SN.obj"\
	"SNPS2Debug/loadprofile.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/thread.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/thread/thread.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/thread/thread.cpp" -o $@

"SNPS2Debug/DiscError.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError.cpp" -o $@

"SNPS2Debug/DiscError_GCN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError_GCN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError_GCN.cpp" -o $@

"SNPS2Debug/DiscError_ps2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError_ps2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError_ps2.cpp" -o $@

"SNPS2Debug/DiscError_win32.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError_win32.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError_win32.cpp" -o $@

"SNPS2Debug/DiscError_xbox.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError_xbox.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/DiscErrorHandler/DiscError_xbox.cpp" -o $@

"SNPS2Debug/Semaphore.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/Semaphore/Semaphore.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/Semaphore/Semaphore.cpp" -o $@

"SNPS2Debug/OSThread.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/OSThreading/OSThread.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/OSThreading/OSThread.cpp" -o $@

"SNPS2Debug/contps2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/Context/contps2.s"
	$(AS) $(AS_FLAGS) -Wa,-MD,"$(INTDIR)\contps2.d" "c:/dev/Avalanche/Engine/Layers/Context/contps2.s" -o $@

"SNPS2Debug/LayersPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/LayersPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/LayersPCH_SN.cpp" -o $@

"SNPS2Debug/loadprofile.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/Engine/Layers/loadprofile.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/Engine/Layers/loadprofile.cpp" -o $@

LIB: "SNPS2Debug/Layers.lib"

"SNPS2Debug/Layers.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/thread.d"
include "SNPS2Debug/DiscError.d"
include "SNPS2Debug/DiscError_GCN.d"
include "SNPS2Debug/DiscError_ps2.d"
include "SNPS2Debug/DiscError_win32.d"
include "SNPS2Debug/DiscError_xbox.d"
include "SNPS2Debug/Semaphore.d"
include "SNPS2Debug/OSThread.d"
include "SNPS2Debug/contps2.d"
include "SNPS2Debug/LayersPCH_SN.d"
include "SNPS2Debug/loadprofile.d"

