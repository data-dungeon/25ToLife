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
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/Gamelib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/PS2/Eznetcnf/include"\
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
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/Gamelib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/PS2/Eznetcnf/include"\
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
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/Gamelib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/PS2/Eznetcnf/include"\
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
	"SNPS2Debug/AvalancheDDF.obj"\
	"SNPS2Debug/DOActor.obj"\
	"SNPS2Debug/DOActorDDL.obj"\
	"SNPS2Debug/DOGeneric.obj"\
	"SNPS2Debug/DOGenericDDL.obj"\
	"SNPS2Debug/DOHost.obj"\
	"SNPS2Debug/DOHostDDL.obj"\
	"SNPS2Debug/DOMessageOwner.obj"\
	"SNPS2Debug/DOMessageOwnerDDL.obj"\
	"SNPS2Debug/DOPathFollower.obj"\
	"SNPS2Debug/DOPathFollowerDDL.obj"\
	"SNPS2Debug/DOTalkerDDL.obj"\
	"SNPS2Debug/DOWorld.obj"\
	"SNPS2Debug/DOWorldDDL.obj"\
	"SNPS2Debug/DRDataset.obj"\
	"SNPS2Debug/DRGroundFollower.obj"\
	"SNPS2Debug/DRPathFollower.obj"\
	"SNPS2Debug/DRSixDOF.obj"\
	"SNPS2Debug/DSActorInteraction.obj"\
	"SNPS2Debug/DSActorInteractionDDL.obj"\
	"SNPS2Debug/DSActorMigrateDDL.obj"\
	"SNPS2Debug/DSActorPropsDDL.obj"\
	"SNPS2Debug/DSBuffer.obj"\
	"SNPS2Debug/DSBufferDDL.obj"\
	"SNPS2Debug/DSGroundFollowerDDL.obj"\
	"SNPS2Debug/DSHostState.obj"\
	"SNPS2Debug/DSHostStateDDL.obj"\
	"SNPS2Debug/DSPathFollowerDDL.obj"\
	"SNPS2Debug/DSSessionLock.obj"\
	"SNPS2Debug/DSSessionLockDDL.obj"\
	"SNPS2Debug/DSSixDOF.obj"\
	"SNPS2Debug/DSSixDOFDDL.obj"\
	"SNPS2Debug/DSStartTimeDDL.obj"\
	"SNPS2Debug/DSTalkerPropsDDL.obj"\
	"SNPS2Debug/DSVector.obj"\
	"SNPS2Debug/DSVectorDDL.obj"\
	"SNPS2Debug/DSWorldExtentsDDL.obj"\
	"SNPS2Debug/NetZCompress.obj"\
	"SNPS2Debug/NetZConnection.obj"\
	"SNPS2Debug/NetZEncrypt.obj"\
	"SNPS2Debug/NetZMessage.obj"\
	"SNPS2Debug/NetZSession.obj"\
	"SNPS2Debug/NetZSessionSearch.obj"\
	"SNPS2Debug/SessionTime.obj"\
	"SNPS2Debug/Translators.obj"\
	"SNPS2Debug/NetZPlatformConnection.obj"\
	"SNPS2Debug/Connection.obj"\
	"SNPS2Debug/NetActor.obj"\
	"SNPS2Debug/NetworkPCH.obj"\
	"SNPS2Debug/OnlineSessionSearch.obj"\
	"SNPS2Debug/Session.obj"\
	"SNPS2Debug/SessionMgr.obj"\
	"SNPS2Debug/SessionSpec.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/AvalancheDDF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/AvalancheDDF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/AvalancheDDF.cpp" -o $@

"SNPS2Debug/DOActor.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOActor.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOActor.cpp" -o $@

"SNPS2Debug/DOActorDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOActorDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOActorDDL.cpp" -o $@

"SNPS2Debug/DOGeneric.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOGeneric.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOGeneric.cpp" -o $@

"SNPS2Debug/DOGenericDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOGenericDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOGenericDDL.cpp" -o $@

"SNPS2Debug/DOHost.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOHost.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOHost.cpp" -o $@

"SNPS2Debug/DOHostDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOHostDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOHostDDL.cpp" -o $@

"SNPS2Debug/DOMessageOwner.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOMessageOwner.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOMessageOwner.cpp" -o $@

"SNPS2Debug/DOMessageOwnerDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOMessageOwnerDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOMessageOwnerDDL.cpp" -o $@

"SNPS2Debug/DOPathFollower.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOPathFollower.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOPathFollower.cpp" -o $@

"SNPS2Debug/DOPathFollowerDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOPathFollowerDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOPathFollowerDDL.cpp" -o $@

"SNPS2Debug/DOTalkerDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOTalkerDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOTalkerDDL.cpp" -o $@

"SNPS2Debug/DOWorld.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOWorld.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOWorld.cpp" -o $@

"SNPS2Debug/DOWorldDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DOWorldDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DOWorldDDL.cpp" -o $@

"SNPS2Debug/DRDataset.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DRDataset.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DRDataset.cpp" -o $@

"SNPS2Debug/DRGroundFollower.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DRGroundFollower.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DRGroundFollower.cpp" -o $@

"SNPS2Debug/DRPathFollower.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DRPathFollower.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DRPathFollower.cpp" -o $@

"SNPS2Debug/DRSixDOF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DRSixDOF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DRSixDOF.cpp" -o $@

"SNPS2Debug/DSActorInteraction.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSActorInteraction.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSActorInteraction.cpp" -o $@

"SNPS2Debug/DSActorInteractionDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSActorInteractionDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSActorInteractionDDL.cpp" -o $@

"SNPS2Debug/DSActorMigrateDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSActorMigrateDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSActorMigrateDDL.cpp" -o $@

"SNPS2Debug/DSActorPropsDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSActorPropsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSActorPropsDDL.cpp" -o $@

"SNPS2Debug/DSBuffer.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSBuffer.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSBuffer.cpp" -o $@

"SNPS2Debug/DSBufferDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSBufferDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSBufferDDL.cpp" -o $@

"SNPS2Debug/DSGroundFollowerDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSGroundFollowerDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSGroundFollowerDDL.cpp" -o $@

"SNPS2Debug/DSHostState.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSHostState.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSHostState.cpp" -o $@

"SNPS2Debug/DSHostStateDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSHostStateDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSHostStateDDL.cpp" -o $@

"SNPS2Debug/DSPathFollowerDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSPathFollowerDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSPathFollowerDDL.cpp" -o $@

"SNPS2Debug/DSSessionLock.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSSessionLock.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSSessionLock.cpp" -o $@

"SNPS2Debug/DSSessionLockDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSSessionLockDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSSessionLockDDL.cpp" -o $@

"SNPS2Debug/DSSixDOF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSSixDOF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSSixDOF.cpp" -o $@

"SNPS2Debug/DSSixDOFDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSSixDOFDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSSixDOFDDL.cpp" -o $@

"SNPS2Debug/DSStartTimeDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSStartTimeDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSStartTimeDDL.cpp" -o $@

"SNPS2Debug/DSTalkerPropsDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSTalkerPropsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSTalkerPropsDDL.cpp" -o $@

"SNPS2Debug/DSVector.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSVector.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSVector.cpp" -o $@

"SNPS2Debug/DSVectorDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSVectorDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSVectorDDL.cpp" -o $@

"SNPS2Debug/DSWorldExtentsDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/DSWorldExtentsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/DSWorldExtentsDDL.cpp" -o $@

"SNPS2Debug/NetZCompress.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/NetZCompress.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/NetZCompress.cpp" -o $@

"SNPS2Debug/NetZConnection.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/NetZConnection.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/NetZConnection.cpp" -o $@

"SNPS2Debug/NetZEncrypt.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/NetZEncrypt.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/NetZEncrypt.cpp" -o $@

"SNPS2Debug/NetZMessage.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/NetZMessage.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/NetZMessage.cpp" -o $@

"SNPS2Debug/NetZSession.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/NetZSession.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/NetZSession.cpp" -o $@

"SNPS2Debug/NetZSessionSearch.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/NetZSessionSearch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/NetZSessionSearch.cpp" -o $@

"SNPS2Debug/SessionTime.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/SessionTime.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/SessionTime.cpp" -o $@

"SNPS2Debug/Translators.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/Translators.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/Translators.cpp" -o $@

"SNPS2Debug/NetZPlatformConnection.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetZ/Ps2/NetZPlatformConnection.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetZ/Ps2/NetZPlatformConnection.cpp" -o $@

"SNPS2Debug/Connection.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/Connection.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/Connection.cpp" -o $@

"SNPS2Debug/NetActor.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetActor.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetActor.cpp" -o $@

"SNPS2Debug/NetworkPCH.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/NetworkPCH.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/NetworkPCH.cpp" -o $@

"SNPS2Debug/OnlineSessionSearch.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/OnlineSessionSearch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/OnlineSessionSearch.cpp" -o $@

"SNPS2Debug/Session.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/Session.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/Session.cpp" -o $@

"SNPS2Debug/SessionMgr.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/SessionMgr.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/SessionMgr.cpp" -o $@

"SNPS2Debug/SessionSpec.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Network/SessionSpec.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Network/SessionSpec.cpp" -o $@

LIB: "SNPS2Debug/Network.lib"

"SNPS2Debug/Network.lib" : $(LD_OBJS)
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/AvalancheDDF.d"
include "SNPS2Debug/DOActor.d"
include "SNPS2Debug/DOActorDDL.d"
include "SNPS2Debug/DOGeneric.d"
include "SNPS2Debug/DOGenericDDL.d"
include "SNPS2Debug/DOHost.d"
include "SNPS2Debug/DOHostDDL.d"
include "SNPS2Debug/DOMessageOwner.d"
include "SNPS2Debug/DOMessageOwnerDDL.d"
include "SNPS2Debug/DOPathFollower.d"
include "SNPS2Debug/DOPathFollowerDDL.d"
include "SNPS2Debug/DOTalkerDDL.d"
include "SNPS2Debug/DOWorld.d"
include "SNPS2Debug/DOWorldDDL.d"
include "SNPS2Debug/DRDataset.d"
include "SNPS2Debug/DRGroundFollower.d"
include "SNPS2Debug/DRPathFollower.d"
include "SNPS2Debug/DRSixDOF.d"
include "SNPS2Debug/DSActorInteraction.d"
include "SNPS2Debug/DSActorInteractionDDL.d"
include "SNPS2Debug/DSActorMigrateDDL.d"
include "SNPS2Debug/DSActorPropsDDL.d"
include "SNPS2Debug/DSBuffer.d"
include "SNPS2Debug/DSBufferDDL.d"
include "SNPS2Debug/DSGroundFollowerDDL.d"
include "SNPS2Debug/DSHostState.d"
include "SNPS2Debug/DSHostStateDDL.d"
include "SNPS2Debug/DSPathFollowerDDL.d"
include "SNPS2Debug/DSSessionLock.d"
include "SNPS2Debug/DSSessionLockDDL.d"
include "SNPS2Debug/DSSixDOF.d"
include "SNPS2Debug/DSSixDOFDDL.d"
include "SNPS2Debug/DSStartTimeDDL.d"
include "SNPS2Debug/DSTalkerPropsDDL.d"
include "SNPS2Debug/DSVector.d"
include "SNPS2Debug/DSVectorDDL.d"
include "SNPS2Debug/DSWorldExtentsDDL.d"
include "SNPS2Debug/NetZCompress.d"
include "SNPS2Debug/NetZConnection.d"
include "SNPS2Debug/NetZEncrypt.d"
include "SNPS2Debug/NetZMessage.d"
include "SNPS2Debug/NetZSession.d"
include "SNPS2Debug/NetZSessionSearch.d"
include "SNPS2Debug/SessionTime.d"
include "SNPS2Debug/Translators.d"
include "SNPS2Debug/NetZPlatformConnection.d"
include "SNPS2Debug/Connection.d"
include "SNPS2Debug/NetActor.d"
include "SNPS2Debug/NetworkPCH.d"
include "SNPS2Debug/OnlineSessionSearch.d"
include "SNPS2Debug/Session.d"
include "SNPS2Debug/SessionMgr.d"
include "SNPS2Debug/SessionSpec.d"

