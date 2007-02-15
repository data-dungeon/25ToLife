########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Release
INTDIR = SNPS2Release

LIBS=\

AS=ps2cc
AS_FLAGS=\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/GameSpy/include"\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include/libeenet"\
	-I"C:/Dev/NetZ/include"\
	-I. -c\
	-fdevstudio\
	-Wa,-sn\
	-g\

DVP=ps2cc
DVP_FLAGS=\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/GameSpy/include"\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include/libeenet"\
	-I"C:/Dev/NetZ/include"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DDSKINNER\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/GameSpy/include"\
	-I"C:/Dev/PS2/Sony/sce-302/ee/include/libeenet"\
	-I"C:/Dev/NetZ/include"\
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
	-O2\

CXX_FLAGS=$(CC_FLAGS)

LD_OBJS=\
	"SNPS2Release/LANLobby.obj"\
	"SNPS2Release/LANLobbyAsyncJobs.obj"\
	"SNPS2Release/RendezVousLobby.obj"\
	"SNPS2Release/XboxLobby.obj"\
	"SNPS2Release/AccountManagerPS2.obj"\
	"SNPS2Release/AccountManager.obj"\
	"SNPS2Release/Lobby.obj"\
	"SNPS2Release/LobbyPCH.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Release/LANLobby.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Lobby/LANLobby.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Lobby/LANLobby.cpp" -o $@

"SNPS2Release/LANLobbyAsyncJobs.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Lobby/LANLobbyAsyncJobs.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Lobby/LANLobbyAsyncJobs.cpp" -o $@

"SNPS2Release/RendezVousLobby.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Lobby/RendezVousLobby.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Lobby/RendezVousLobby.cpp" -o $@

"SNPS2Release/XboxLobby.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Lobby/XboxLobby.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Lobby/XboxLobby.cpp" -o $@

"SNPS2Release/AccountManagerPS2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Lobby/AccountManagerPS2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Lobby/AccountManagerPS2.cpp" -o $@

"SNPS2Release/AccountManager.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Lobby/AccountManager.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Lobby/AccountManager.cpp" -o $@

"SNPS2Release/Lobby.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Lobby/Lobby.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Lobby/Lobby.cpp" -o $@

"SNPS2Release/LobbyPCH.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Lobby/LobbyPCH.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Lobby/LobbyPCH.cpp" -o $@

LIB: "SNPS2Release/Lobby.lib"

"SNPS2Release/Lobby.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif


