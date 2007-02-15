########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Debug
INTDIR = SNPS2Debug

LIBS=\
	 "c:/dev/Avalanche/GameLib/Network/SNPS2Debug/Network.lib"\

AS=ps2cc
AS_FLAGS=\
	-Wa,-g\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DDSKINNER\
	-DNETZ\
	-DUSE_STL\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/Rendezvous/PlayStation2/Include"\
	-I"C:/Dev/NetZ/PlayStation2/Include"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_mc_300_scea/ee/include"\
	-I"C:/Dev/ttl/code"\
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
	-DUSE_STL\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/Rendezvous/PlayStation2/Include"\
	-I"C:/Dev/NetZ/PlayStation2/Include"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_mc_300_scea/ee/include"\
	-I"C:/Dev/ttl/code"\
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
	-DUSE_STL\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/Rendezvous/PlayStation2/Include"\
	-I"C:/Dev/NetZ/PlayStation2/Include"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/Sony/DNAS/libdnas_mc_300_scea/ee/include"\
	-I"C:/Dev/ttl/code"\
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
	"SNPS2Debug/OSI.obj"\
	"SNPS2Debug/OSIAccount.obj"\
	"SNPS2Debug/OSIchar.obj"\
	"SNPS2Debug/OSIClans.obj"\
	"SNPS2Debug/OSIHardwareProfile.obj"\
	"SNPS2Debug/OSIMatch.obj"\
	"SNPS2Debug/OSIPlayer.obj"\
	"SNPS2Debug/OSIPlayerStats.obj"\
	"SNPS2Debug/OSIRoundStats.obj"\
	"SNPS2Debug/OSIStringTable.obj"\
	"SNPS2Debug/OSIAccountList.obj"\
	"SNPS2Debug/OSIClanIDList.obj"\
	"SNPS2Debug/OSIClanInfoList.obj"\
	"SNPS2Debug/OSIClanInvitationList.obj"\
	"SNPS2Debug/OSIClanMembershipList.obj"\
	"SNPS2Debug/OSIClanNoticeList.obj"\
	"SNPS2Debug/OSIHardwareProfileList.obj"\
	"SNPS2Debug/OSIMatchList.obj"\
	"SNPS2Debug/OSIPlayerIDList.obj"\
	"SNPS2Debug/OSIPlayerList.obj"\
	"SNPS2Debug/OSIPlayerStatsList.obj"\
	"SNPS2Debug/OSIStringList.obj"\
	"SNPS2Debug/EidosAccountPrivateDDL.obj"\
	"SNPS2Debug/EidosAccountPublicDDL.obj"\
	"SNPS2Debug/EidosAlertsDDL.obj"\
	"SNPS2Debug/EidosClanInviteDDL.obj"\
	"SNPS2Debug/EidosClanMemberDDL.obj"\
	"SNPS2Debug/EidosClanNoticeDDL.obj"\
	"SNPS2Debug/EidosClanPropsDDL.obj"\
	"SNPS2Debug/EidosFriendDDL.obj"\
	"SNPS2Debug/OSIDDF.obj"\
	"SNPS2Debug/OSIPS2.obj"\
	"SNPS2Debug/OSIPS2Alerts.obj"\
	"SNPS2Debug/TTLMatchMakingDDL.obj"\
	"SNPS2Debug/TTLRoundDDL.obj"\
	"SNPS2Debug/TTLSessionDDL.obj"\
	"SNPS2Debug/TTLStatsDDL.obj"\

COMPILE: $(LD_OBJS)

"SNPS2Debug/OSI.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSI.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSI.cpp" -o $@

"SNPS2Debug/OSIAccount.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIAccount.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIAccount.cpp" -o $@

"SNPS2Debug/OSIchar.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIchar.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIchar.cpp" -o $@

"SNPS2Debug/OSIClans.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIClans.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIClans.cpp" -o $@

"SNPS2Debug/OSIHardwareProfile.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIHardwareProfile.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIHardwareProfile.cpp" -o $@

"SNPS2Debug/OSIMatch.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIMatch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIMatch.cpp" -o $@

"SNPS2Debug/OSIPlayer.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIPlayer.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIPlayer.cpp" -o $@

"SNPS2Debug/OSIPlayerStats.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIPlayerStats.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIPlayerStats.cpp" -o $@

"SNPS2Debug/OSIRoundStats.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIRoundStats.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIRoundStats.cpp" -o $@

"SNPS2Debug/OSIStringTable.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIStringTable.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIStringTable.cpp" -o $@

"SNPS2Debug/OSIAccountList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIAccountList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIAccountList.cpp" -o $@

"SNPS2Debug/OSIClanIDList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIClanIDList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIClanIDList.cpp" -o $@

"SNPS2Debug/OSIClanInfoList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIClanInfoList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIClanInfoList.cpp" -o $@

"SNPS2Debug/OSIClanInvitationList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIClanInvitationList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIClanInvitationList.cpp" -o $@

"SNPS2Debug/OSIClanMembershipList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIClanMembershipList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIClanMembershipList.cpp" -o $@

"SNPS2Debug/OSIClanNoticeList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIClanNoticeList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIClanNoticeList.cpp" -o $@

"SNPS2Debug/OSIHardwareProfileList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIHardwareProfileList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIHardwareProfileList.cpp" -o $@

"SNPS2Debug/OSIMatchList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIMatchList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIMatchList.cpp" -o $@

"SNPS2Debug/OSIPlayerIDList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIPlayerIDList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIPlayerIDList.cpp" -o $@

"SNPS2Debug/OSIPlayerList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIPlayerList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIPlayerList.cpp" -o $@

"SNPS2Debug/OSIPlayerStatsList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIPlayerStatsList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIPlayerStatsList.cpp" -o $@

"SNPS2Debug/OSIStringList.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/OSIStringList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/OSIStringList.cpp" -o $@

"SNPS2Debug/EidosAccountPrivateDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosAccountPrivateDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosAccountPrivateDDL.cpp" -o $@

"SNPS2Debug/EidosAccountPublicDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosAccountPublicDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosAccountPublicDDL.cpp" -o $@

"SNPS2Debug/EidosAlertsDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosAlertsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosAlertsDDL.cpp" -o $@

"SNPS2Debug/EidosClanInviteDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosClanInviteDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosClanInviteDDL.cpp" -o $@

"SNPS2Debug/EidosClanMemberDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosClanMemberDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosClanMemberDDL.cpp" -o $@

"SNPS2Debug/EidosClanNoticeDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosClanNoticeDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosClanNoticeDDL.cpp" -o $@

"SNPS2Debug/EidosClanPropsDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosClanPropsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosClanPropsDDL.cpp" -o $@

"SNPS2Debug/EidosFriendDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosFriendDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/EidosFriendDDL.cpp" -o $@

"SNPS2Debug/OSIDDF.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/OSIDDF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/OSIDDF.cpp" -o $@

"SNPS2Debug/OSIPS2.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/OSIPS2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/OSIPS2.cpp" -o $@

"SNPS2Debug/OSIPS2Alerts.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/OSIPS2Alerts.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/OSIPS2Alerts.cpp" -o $@

"SNPS2Debug/TTLMatchMakingDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/TTLMatchMakingDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/TTLMatchMakingDDL.cpp" -o $@

"SNPS2Debug/TTLRoundDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/TTLRoundDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/TTLRoundDDL.cpp" -o $@

"SNPS2Debug/TTLSessionDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/TTLSessionDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/TTLSessionDDL.cpp" -o $@

"SNPS2Debug/TTLStatsDDL.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Osi/Ps2/TTLStatsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Osi/Ps2/TTLStatsDDL.cpp" -o $@

LIB: "SNPS2Debug/OSI.lib"

"SNPS2Debug/OSI.lib" : $(LD_OBJS)\
	"c:/dev/Avalanche/GameLib/Network/SNPS2Debug/Network.lib"
	@echo ProDG Creating Library $@
	snarl -M <_lib.txt
ifdef VERBOSE
	@echo _lib.txt = 
	@echo [
	@type _lib.txt
	@echo ]
endif

include "SNPS2Debug/OSI.d"
include "SNPS2Debug/OSIAccount.d"
include "SNPS2Debug/OSIchar.d"
include "SNPS2Debug/OSIClans.d"
include "SNPS2Debug/OSIHardwareProfile.d"
include "SNPS2Debug/OSIMatch.d"
include "SNPS2Debug/OSIPlayer.d"
include "SNPS2Debug/OSIPlayerStats.d"
include "SNPS2Debug/OSIRoundStats.d"
include "SNPS2Debug/OSIStringTable.d"
include "SNPS2Debug/OSIAccountList.d"
include "SNPS2Debug/OSIClanIDList.d"
include "SNPS2Debug/OSIClanInfoList.d"
include "SNPS2Debug/OSIClanInvitationList.d"
include "SNPS2Debug/OSIClanMembershipList.d"
include "SNPS2Debug/OSIClanNoticeList.d"
include "SNPS2Debug/OSIHardwareProfileList.d"
include "SNPS2Debug/OSIMatchList.d"
include "SNPS2Debug/OSIPlayerIDList.d"
include "SNPS2Debug/OSIPlayerList.d"
include "SNPS2Debug/OSIPlayerStatsList.d"
include "SNPS2Debug/OSIStringList.d"
include "SNPS2Debug/EidosAccountPrivateDDL.d"
include "SNPS2Debug/EidosAccountPublicDDL.d"
include "SNPS2Debug/EidosAlertsDDL.d"
include "SNPS2Debug/EidosClanInviteDDL.d"
include "SNPS2Debug/EidosClanMemberDDL.d"
include "SNPS2Debug/EidosClanNoticeDDL.d"
include "SNPS2Debug/EidosClanPropsDDL.d"
include "SNPS2Debug/EidosFriendDDL.d"
include "SNPS2Debug/OSIDDF.d"
include "SNPS2Debug/OSIPS2.d"
include "SNPS2Debug/OSIPS2Alerts.d"
include "SNPS2Debug/TTLMatchMakingDDL.d"
include "SNPS2Debug/TTLRoundDDL.d"
include "SNPS2Debug/TTLSessionDDL.d"
include "SNPS2Debug/TTLStatsDDL.d"

