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
	"SNPS2Debug/StationaryCam.obj"\
	"SNPS2Debug/targetcam.obj"\
	"SNPS2Debug/SecurityCam.obj"\
	"SNPS2Debug/pathcam.obj"\
	"SNPS2Debug/generalcam.obj"\
	"SNPS2Debug/collidablecontrolcam.obj"\
	"SNPS2Debug/CameraPCH_SN.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/StationaryCam.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/camera/StationaryCam/StationaryCam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/camera/StationaryCam/StationaryCam.cpp" -o $@

"SNPS2Debug/targetcam.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/camera/targetcam/targetcam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/camera/targetcam/targetcam.cpp" -o $@

"SNPS2Debug/SecurityCam.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/camera/SecurityCamera/SecurityCam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/camera/SecurityCamera/SecurityCam.cpp" -o $@

"SNPS2Debug/pathcam.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/camera/pathcam/pathcam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/camera/pathcam/pathcam.cpp" -o $@

"SNPS2Debug/generalcam.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/camera/generalcam/generalcam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/camera/generalcam/generalcam.cpp" -o $@

"SNPS2Debug/collidablecontrolcam.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/camera/collidablecontrolcam/collidablecontrolcam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/camera/collidablecontrolcam/collidablecontrolcam.cpp" -o $@

"SNPS2Debug/CameraPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/camera/CameraPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/camera/CameraPCH_SN.cpp" -o $@

LIB: "SNPS2Debug/camera.lib"

"SNPS2Debug/camera.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/StationaryCam.d"
include "SNPS2Debug/targetcam.d"
include "SNPS2Debug/SecurityCam.d"
include "SNPS2Debug/pathcam.d"
include "SNPS2Debug/generalcam.d"
include "SNPS2Debug/collidablecontrolcam.d"
include "SNPS2Debug/CameraPCH_SN.d"

