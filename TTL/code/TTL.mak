########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Debug
INTDIR = SNPS2Debug

LCFILE = "c:/dev/Ttl/code/relapp.cmd"
LIBS=\
	 "C:/Dev/Avalanche/Engine/Display/PS2/Renderer/SNPS2Debug/Renderer.lib"\
	 "libgcc.a"\
	 "libkernl.a"\
	 "libpad.a"\
	 "libmrpc.a"\
	 "libcdvd.a"\
	 "liblgkbm.a"\
	 "libm.a"\
	 "libc.a"\
	 "libUtilitiesd.a"\
	 "libgraph.a"\
	 "libscf.a"\
	 "libstdc++.a"\
	 "libsn.a"\
	 "libipu.a"\
	 "libmpeg.a"\
	 "libdma.a"\
	 "libdev.a"\
	 "libpkt.a"\
	 "libvu0.a"\
	 "libmc.a"\
	 "libsdr.a"\
	 "sntty.lib"\
	 "libNetZd.a"\
	 "netglue_insck.a"\
	 "C:/Dev/PS2/Sony/sce-302/ee/lib/eznetcnf.a"\
	 "C:/Dev/PS2/Sony/sce-302/ee/lib/netcnfif.a"\
	 "C:/Dev/PS2/Sony/sce-302/ee/lib/libinsck.a"\
	 "C:/Dev/PS2/Sony/sce-302/ee/lib/eznetctl_inet.a"\
	 "libQNetd.a"\
	 "libexcep.a"\
	 "libVoiceChatExtd.a"\
	 "libAVStreamsCored.a"\
	 "libPHBDRExtd.a"\
	 "libAccountManagementClientd.a"\
	 "libBackEndManagementClientd.a"\
	 "libCompetitionClientd.a"\
	 "libFriendsClientd.a"\
	 "libMatchMakingClientd.a"\
	 "libMessagingClientd.a"\
	 "libNATTraversalClientd.a"\
	 "libPeerClientd.a"\
	 "libPersistentStoreClientd.a"\
	 "libQuazalCored.a"\
	 "libRendezVousd.a"\
	 "libHostMigrationExtd.a"\
	 "libdbc.a"\
	 "libmc2.a"\
	 "c:/dev/Avalanche/GameLib/Effects/SNPS2Debug/effects.lib"\
	 "c:/dev/Avalanche/GameLib/Osi/SNPS2Debug/OSI.lib"\
	 "c:/dev/Avalanche/GameLib/WatchClient/SNPS2Debug/WatchClient.lib"\
	 "c:/dev/Avalanche/GameLib/path/SNPS2Debug/path.lib"\
	 "c:/dev/Avalanche/Engine/Game/SNPS2Debug/Game.lib"\
	 "c:/dev/Avalanche/GameLib/Loop/SNPS2Debug/Loop.lib"\
	 "c:/dev/Avalanche/Engine/Math/SNPS2Debug/Math.lib"\
	 "c:/dev/Avalanche/GameLib/profile/SNPS2Debug/Profile.lib"\
	 "c:/dev/Avalanche/GameLib/collide/SNPS2Debug/collide.lib"\
	 "c:/dev/Avalanche/GameLib/geomUtil/SNPS2Debug/geomUtil.lib"\
	 "c:/dev/Avalanche/GameLib/Environ/SNPS2Debug/Environ.lib"\
	 "c:/dev/Avalanche/GameLib/fmv/SNPS2Debug/fmv.lib"\
	 "c:/dev/Avalanche/GameLib/mathUtil/SNPS2Debug/mathUtil.lib"\
	 "c:/dev/Avalanche/GameLib/Xml/SNPS2Debug/XML.lib"\
	 "c:/dev/Avalanche/GameLib/Interfaces/SNPS2Debug/Interfaces.lib"\
	 "c:/dev/Avalanche/GameLib/TupperAttrib/SNPS2Debug/TupperAttrib.lib"\
	 "c:/dev/Avalanche/Engine/Display/SNPS2Debug/Display.lib"\
	 "c:/dev/Avalanche/GameLib/Tupperware/SNPS2Debug/tupperware.lib"\
	 "c:/dev/Avalanche/GameLib/platform/SNPS2Debug/platform.lib"\
	 "c:/dev/Avalanche/GameLib/CollideHelper/SNPS2Debug/CollideHelper.lib"\
	 "c:/dev/Avalanche/GameLib/Network/SNPS2Debug/Network.lib"\
	 "c:/dev/Avalanche/GameLib/EngineHelper/SNPS2Debug/EngineHelper.lib"\
	 "c:/dev/Avalanche/GameLib/Script/SNPS2Debug/Script.lib"\
	 "c:/dev/Avalanche/Engine/Layers/SNPS2Debug/Layers.lib"\
	 "c:/dev/Avalanche/GameLib/cmdcon/SNPS2Debug/cmdcon.lib"\
	 "c:/dev/Avalanche/GameLib/camera/SNPS2Debug/camera.lib"\
	 "c:/dev/Avalanche/Engine/Audio/SNPS2Debug/audio.lib"\
	 "c:/dev/Avalanche/GameLib/GameHelper/SNPS2Debug/GameHelper.lib"\
	 "c:/dev/Avalanche/GameLib/stage/SNPS2Debug/stage.lib"\
	 "c:/dev/Avalanche/GameLib/Components/SNPS2Debug/Components.lib"\
	 "c:/dev/Avalanche/GameLib/hud/SNPS2Debug/hud.lib"\
	 "c:/dev/Avalanche/GameLib/Navigate/SNPS2Debug/Navigate.lib"\

AS=ps2cc
AS_FLAGS=\
	-Wa,-g\
	-D_DEBUG\
	-DSN\
	-DSN_TARGET_PS2_RELAPP\
	-DPS2\
	-DTUPPERWARE_FOR_ENGINE\
	-DDSKINNER\
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/NetZ/PlayStation2/Include"\
	-I"C:/Dev/RendezVous/PlayStation2/include"\
	-I"c:/dev/Ttl/code/"\
	-I"c:/dev/TTL/"\
	-I"C:/Dev/NellyMoser/Lib"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/eznetcnf/include"\
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
	-DTUPPERWARE_FOR_ENGINE\
	-DDSKINNER\
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/NetZ/PlayStation2/Include"\
	-I"C:/Dev/RendezVous/PlayStation2/include"\
	-I"c:/dev/Ttl/code/"\
	-I"c:/dev/TTL/"\
	-I"C:/Dev/NellyMoser/Lib"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/eznetcnf/include"\
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
	-DTUPPERWARE_FOR_ENGINE\
	-DDSKINNER\
	-DNETZ\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/GameLib"\
	-I"C:/Dev/NetZ/PlayStation2/Include"\
	-I"C:/Dev/RendezVous/PlayStation2/include"\
	-I"c:/dev/Ttl/code/"\
	-I"c:/dev/TTL/"\
	-I"C:/Dev/NellyMoser/Lib"\
	-I"C:/dev/PS2/SN/ProDG for PlayStation2/ee-include"\
	-I"C:/Dev/PS2/eznetcnf/include"\
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
	-strip-unused\

CXX_FLAGS=$(CC_FLAGS)

LD=ps2dlllk
LD_FLAGS=\
	"c:\dev\Ttl\code\relapp-PS2Debug.lk" ps2cc.exe @_ps2cc.txt

LD_OBJS=\
	 crt0.o\
	"SNPS2Debug/avatarfx.obj"\
	"SNPS2Debug/sobj.obj"\
	"SNPS2Debug/deathcam.obj"\
	"SNPS2Debug/ghostcam.obj"\
	"SNPS2Debug/lookaround.obj"\
	"SNPS2Debug/ConfigMgr.obj"\
	"SNPS2Debug/PlayerSpawnMgr.obj"\
	"SNPS2Debug/CharacterLoader.obj"\
	"SNPS2Debug/cheats.obj"\
	"SNPS2Debug/ErrorMonitor.obj"\
	"SNPS2Debug/Lobby.obj"\
	"SNPS2Debug/NetMonitor.obj"\
	"SNPS2Debug/OSILobby.obj"\
	"SNPS2Debug/ParticipantConfig.obj"\
	"SNPS2Debug/PlayerAgent.obj"\
	"SNPS2Debug/PlayerProfile.obj"\
	"SNPS2Debug/TTLSessionMgr.obj"\
	"SNPS2Debug/CustomPlayerLoop.obj"\
	"SNPS2Debug/InitialLoop.obj"\
	"SNPS2Debug/KillsScoringModel.obj"\
	"SNPS2Debug/MultiPlayerLoop.obj"\
	"SNPS2Debug/NetworkLoop.obj"\
	"SNPS2Debug/OnlineSetupLoop.obj"\
	"SNPS2Debug/RaidScoringModel.obj"\
	"SNPS2Debug/RobberyScoringModel.obj"\
	"SNPS2Debug/ScoringModel.obj"\
	"SNPS2Debug/SetupLoop.obj"\
	"SNPS2Debug/SimpleTeamingModel.obj"\
	"SNPS2Debug/SinglePlayerLoop.obj"\
	"SNPS2Debug/StandAloneLoop.obj"\
	"SNPS2Debug/TagScoringModel.obj"\
	"SNPS2Debug/TagTeamingModel.obj"\
	"SNPS2Debug/TeamingModel.obj"\
	"SNPS2Debug/TTLTimeDate.obj"\
	"SNPS2Debug/CutSceneMgr.obj"\
	"SNPS2Debug/affliction.obj"\
	"SNPS2Debug/DeathControl.obj"\
	"SNPS2Debug/footfall.obj"\
	"SNPS2Debug/loadout1.obj"\
	"SNPS2Debug/onfire.obj"\
	"SNPS2Debug/painstats.obj"\
	"SNPS2Debug/quicktag.obj"\
	"SNPS2Debug/say.obj"\
	"SNPS2Debug/shadow.obj"\
	"SNPS2Debug/BipedLadder.obj"\
	"SNPS2Debug/ViewDriver.obj"\
	"SNPS2Debug/WallMoves.obj"\
	"SNPS2Debug/AiKillbox.obj"\
	"SNPS2Debug/components.obj"\
	"SNPS2Debug/customplayer.obj"\
	"SNPS2Debug/gamesettings.obj"\
	"SNPS2Debug/keyboardchat.obj"\
	"SNPS2Debug/savegame.obj"\
	"SNPS2Debug/texteffects.obj"\
	"SNPS2Debug/screen_AskPassword.obj"\
	"SNPS2Debug/screen_BaseClass.obj"\
	"SNPS2Debug/screen_BasePopup.obj"\
	"SNPS2Debug/screen_BootOptions.obj"\
	"SNPS2Debug/screen_Briefing.obj"\
	"SNPS2Debug/screen_clan_create.obj"\
	"SNPS2Debug/screen_clan_invite.obj"\
	"SNPS2Debug/screen_clan_invite_popup.obj"\
	"SNPS2Debug/screen_clan_list.obj"\
	"SNPS2Debug/screen_clan_main.obj"\
	"SNPS2Debug/screen_clan_notices.obj"\
	"SNPS2Debug/screen_clan_popup.obj"\
	"SNPS2Debug/screen_CountryList.obj"\
	"SNPS2Debug/screen_CreateProfile.obj"\
	"SNPS2Debug/screen_CreateSession.obj"\
	"SNPS2Debug/screen_CustomPlayer.obj"\
	"SNPS2Debug/screen_DevMenu.obj"\
	"SNPS2Debug/screen_Dialog.obj"\
	"SNPS2Debug/screen_EOA.obj"\
	"SNPS2Debug/screen_EORConnect.obj"\
	"SNPS2Debug/screen_EORCreateAccount1.obj"\
	"SNPS2Debug/screen_EORCreateAccount2.obj"\
	"SNPS2Debug/screen_EORDeleteAccount.obj"\
	"SNPS2Debug/screen_EOREditAccount1.obj"\
	"SNPS2Debug/screen_EOREditAccount2.obj"\
	"SNPS2Debug/screen_EORLogin.obj"\
	"SNPS2Debug/screen_EORRecoverAccount.obj"\
	"SNPS2Debug/screen_friendConfirmed.obj"\
	"SNPS2Debug/screen_FriendRequested.obj"\
	"SNPS2Debug/screen_friends.obj"\
	"SNPS2Debug/screen_FriendsList.obj"\
	"SNPS2Debug/screen_FriendsPopup.obj"\
	"SNPS2Debug/screen_GamePadSettings.obj"\
	"SNPS2Debug/screen_GameSetting.obj"\
	"SNPS2Debug/screen_Keyboard.obj"\
	"SNPS2Debug/screen_MainMenu.obj"\
	"SNPS2Debug/screen_MemCard.obj"\
	"SNPS2Debug/screen_MultiplayerGameSettings.obj"\
	"SNPS2Debug/screen_MultiplayerMenu.obj"\
	"SNPS2Debug/screen_MySettings.obj"\
	"SNPS2Debug/screen_NetworkConfig.obj"\
	"SNPS2Debug/screen_Objectives.obj"\
	"SNPS2Debug/screen_OnlineChat.obj"\
	"SNPS2Debug/screen_OnlineLobby.obj"\
	"SNPS2Debug/screen_OnlineMenu.obj"\
	"SNPS2Debug/screen_OnlineProfile.obj"\
	"SNPS2Debug/screen_Options.obj"\
	"SNPS2Debug/screen_Pause.obj"\
	"SNPS2Debug/screen_players.obj"\
	"SNPS2Debug/screen_PlayersList.obj"\
	"SNPS2Debug/screen_Popups.obj"\
	"SNPS2Debug/screen_Quickmatch.obj"\
	"SNPS2Debug/screen_Sandbox.obj"\
	"SNPS2Debug/screen_ScoreBoard.obj"\
	"SNPS2Debug/screen_SearchResult.obj"\
	"SNPS2Debug/screen_Splash.obj"\
	"SNPS2Debug/screen_stats.obj"\
	"SNPS2Debug/screen_TagEditor.obj"\
	"SNPS2Debug/screen_Title.obj"\
	"SNPS2Debug/screen_VoiceSetting.obj"\
	"SNPS2Debug/screen_wait.obj"\
	"SNPS2Debug/screen_Weapons.obj"\
	"SNPS2Debug/screen_YesNoDialog.obj"\
	"SNPS2Debug/EORAccount.obj"\
	"SNPS2Debug/GameSettingConfig.obj"\
	"SNPS2Debug/ParamList.obj"\
	"SNPS2Debug/StringList.obj"\
	"SNPS2Debug/UseCase.obj"\
	"SNPS2Debug/UseCaseClans.obj"\
	"SNPS2Debug/UseCaseDisplayDialog.obj"\
	"SNPS2Debug/UseCaseEOA.obj"\
	"SNPS2Debug/UseCaseEORConfig.obj"\
	"SNPS2Debug/UseCaseEORConnect.obj"\
	"SNPS2Debug/UseCaseEORCreateAccount.obj"\
	"SNPS2Debug/UseCaseEORDeleteAccount.obj"\
	"SNPS2Debug/UseCaseEOREditAccount.obj"\
	"SNPS2Debug/UseCaseEORLogin.obj"\
	"SNPS2Debug/UseCaseEORPassword.obj"\
	"SNPS2Debug/UseCaseEORPatcher.obj"\
	"SNPS2Debug/UseCaseEORRecoverAccount.obj"\
	"SNPS2Debug/UseCaseFindMatch.obj"\
	"SNPS2Debug/UseCaseFriends.obj"\
	"SNPS2Debug/UseCaseHostMatch.obj"\
	"SNPS2Debug/UseCaseInvitePlayerToMatch.obj"\
	"SNPS2Debug/UseCaseJoinMatch.obj"\
	"SNPS2Debug/UseCaseMultiplayerMenu.obj"\
	"SNPS2Debug/UseCasePlayers.obj"\
	"SNPS2Debug/UseCaseQuickMatch.obj"\
	"SNPS2Debug/UseCaseStats.obj"\
	"SNPS2Debug/UseCaseWaitForOSIRequest.obj"\
	"SNPS2Debug/UseCaseYesNoDialog.obj"\
	"SNPS2Debug/ValueList.obj"\
	"SNPS2Debug/aiming.obj"\
	"SNPS2Debug/aiweapfire.obj"\
	"SNPS2Debug/aiweaphelper.obj"\
	"SNPS2Debug/aoeproj.obj"\
	"SNPS2Debug/armory.obj"\
	"SNPS2Debug/armoryview.obj"\
	"SNPS2Debug/carbomb.obj"\
	"SNPS2Debug/debugshot.obj"\
	"SNPS2Debug/ebrass.obj"\
	"SNPS2Debug/ebrassLib.obj"\
	"SNPS2Debug/emitproj.obj"\
	"SNPS2Debug/fireproj.obj"\
	"SNPS2Debug/flashproj.obj"\
	"SNPS2Debug/freeproj.obj"\
	"SNPS2Debug/hitsalvo.obj"\
	"SNPS2Debug/loadout.obj"\
	"SNPS2Debug/lsight.obj"\
	"SNPS2Debug/meleeproj.obj"\
	"SNPS2Debug/mlsight.obj"\
	"SNPS2Debug/muzzle.obj"\
	"SNPS2Debug/muzzleLib.obj"\
	"SNPS2Debug/penetrate.obj"\
	"SNPS2Debug/seethru.obj"\
	"SNPS2Debug/shockproj.obj"\
	"SNPS2Debug/subsonic.obj"\
	"SNPS2Debug/taser.obj"\
	"SNPS2Debug/tracerset.obj"\
	"SNPS2Debug/triggerproj.obj"\
	"SNPS2Debug/ttlprojectile.obj"\
	"SNPS2Debug/ttlweapinv.obj"\
	"SNPS2Debug/blood.obj"\
	"SNPS2Debug/fsdsys.obj"\
	"SNPS2Debug/lasersight.obj"\
	"SNPS2Debug/pain_decal.obj"\
	"SNPS2Debug/tag.obj"\
	"SNPS2Debug/DOCombatant.obj"\
	"SNPS2Debug/DOCombatantDDL.obj"\
	"SNPS2Debug/DOGlass.obj"\
	"SNPS2Debug/DOGlassDDL.obj"\
	"SNPS2Debug/dolobbed.obj"\
	"SNPS2Debug/DOLobbedDDL.obj"\
	"SNPS2Debug/DOPlayerAgent.obj"\
	"SNPS2Debug/DOPlayerAgentDDL.obj"\
	"SNPS2Debug/DOReferee.obj"\
	"SNPS2Debug/DORefereeDDL.obj"\
	"SNPS2Debug/dosubsonic.obj"\
	"SNPS2Debug/DOSubsonicDDL.obj"\
	"SNPS2Debug/DSAgentDDL.obj"\
	"SNPS2Debug/DSAppearance.obj"\
	"SNPS2Debug/DSAppearanceDDL.obj"\
	"SNPS2Debug/DSCombatantMode.obj"\
	"SNPS2Debug/DSCombatantModeDDL.obj"\
	"SNPS2Debug/DSCombatantStateDDL.obj"\
	"SNPS2Debug/DSGlassState.obj"\
	"SNPS2Debug/DSGlassStateDDL.obj"\
	"SNPS2Debug/DSLobbedProps.obj"\
	"SNPS2Debug/DSLobbedPropsDDL.obj"\
	"SNPS2Debug/DSParticipant.obj"\
	"SNPS2Debug/DSParticipantDDL.obj"\
	"SNPS2Debug/DSProfileDDL.obj"\
	"SNPS2Debug/DSRefereeAssignment.obj"\
	"SNPS2Debug/DSRefereeAssignmentDDL.obj"\
	"SNPS2Debug/DSRefereeId.obj"\
	"SNPS2Debug/DSRefereeIdDDL.obj"\
	"SNPS2Debug/DSRefereePhaseDDL.obj"\
	"SNPS2Debug/DSRefereePopulate.obj"\
	"SNPS2Debug/DSRefereePopulateDDL.obj"\
	"SNPS2Debug/DSRefereeRound.obj"\
	"SNPS2Debug/DSRefereeRoundDDL.obj"\
	"SNPS2Debug/DSRefereeRules.obj"\
	"SNPS2Debug/DSRefereeRulesDDL.obj"\
	"SNPS2Debug/DSSubsonicProps.obj"\
	"SNPS2Debug/DSSubsonicPropsDDL.obj"\
	"SNPS2Debug/DSTeamsDDL.obj"\
	"SNPS2Debug/TTLDDF.obj"\
	"SNPS2Debug/TTLSession.obj"\
	"SNPS2Debug/TTLTranslators.obj"\
	"SNPS2Debug/TTL_RendezVousDDF.obj"\
	"SNPS2Debug/RigidTest.obj"\
	"SNPS2Debug/HealthIF.obj"\
	"SNPS2Debug/ParticipantIF.obj"\
	"SNPS2Debug/TTLPCH_SN.obj"\

COMPILE: $(LD_OBJS)

crt0.o: "SNPS2Debug/crt0.obj"
	copy "SNPS2Debug\crt0.obj" crt0.o

"SNPS2Debug/avatarfx.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/avatar/avatarfx.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/avatar/avatarfx.cpp" -o $@

"SNPS2Debug/sobj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/avatar/sobj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/avatar/sobj.cpp" -o $@

"SNPS2Debug/deathcam.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/camera/deathcam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/camera/deathcam.cpp" -o $@

"SNPS2Debug/ghostcam.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/camera/ghostcam.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/camera/ghostcam.cpp" -o $@

"SNPS2Debug/lookaround.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/camera/lookaround.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/camera/lookaround.cpp" -o $@

"SNPS2Debug/ConfigMgr.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/environ/ConfigMgr.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/environ/ConfigMgr.cpp" -o $@

"SNPS2Debug/PlayerSpawnMgr.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/environ/PlayerSpawnMgr.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/environ/PlayerSpawnMgr.cpp" -o $@

"SNPS2Debug/CharacterLoader.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/CharacterLoader.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/CharacterLoader.cpp" -o $@

"SNPS2Debug/cheats.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/cheats.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/cheats.cpp" -o $@

"SNPS2Debug/ErrorMonitor.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/ErrorMonitor.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/ErrorMonitor.cpp" -o $@

"SNPS2Debug/Lobby.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/Lobby.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/Lobby.cpp" -o $@

"SNPS2Debug/NetMonitor.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/NetMonitor.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/NetMonitor.cpp" -o $@

"SNPS2Debug/OSILobby.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/OSILobby.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/OSILobby.cpp" -o $@

"SNPS2Debug/ParticipantConfig.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/ParticipantConfig.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/ParticipantConfig.cpp" -o $@

"SNPS2Debug/PlayerAgent.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/PlayerAgent.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/PlayerAgent.cpp" -o $@

"SNPS2Debug/PlayerProfile.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/PlayerProfile.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/PlayerProfile.cpp" -o $@

"SNPS2Debug/TTLSessionMgr.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/TTLSessionMgr.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/TTLSessionMgr.cpp" -o $@

"SNPS2Debug/CustomPlayerLoop.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/CustomPlayerLoop.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/CustomPlayerLoop.cpp" -o $@

"SNPS2Debug/InitialLoop.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/InitialLoop.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/InitialLoop.cpp" -o $@

"SNPS2Debug/KillsScoringModel.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/KillsScoringModel.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/KillsScoringModel.cpp" -o $@

"SNPS2Debug/MultiPlayerLoop.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/MultiPlayerLoop.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/MultiPlayerLoop.cpp" -o $@

"SNPS2Debug/NetworkLoop.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/NetworkLoop.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/NetworkLoop.cpp" -o $@

"SNPS2Debug/OnlineSetupLoop.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/OnlineSetupLoop.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/OnlineSetupLoop.cpp" -o $@

"SNPS2Debug/RaidScoringModel.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/RaidScoringModel.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/RaidScoringModel.cpp" -o $@

"SNPS2Debug/RobberyScoringModel.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/RobberyScoringModel.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/RobberyScoringModel.cpp" -o $@

"SNPS2Debug/ScoringModel.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/ScoringModel.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/ScoringModel.cpp" -o $@

"SNPS2Debug/SetupLoop.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/SetupLoop.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/SetupLoop.cpp" -o $@

"SNPS2Debug/SimpleTeamingModel.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/SimpleTeamingModel.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/SimpleTeamingModel.cpp" -o $@

"SNPS2Debug/SinglePlayerLoop.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/SinglePlayerLoop.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/SinglePlayerLoop.cpp" -o $@

"SNPS2Debug/StandAloneLoop.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/StandAloneLoop.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/StandAloneLoop.cpp" -o $@

"SNPS2Debug/TagScoringModel.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/TagScoringModel.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/TagScoringModel.cpp" -o $@

"SNPS2Debug/TagTeamingModel.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/TagTeamingModel.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/TagTeamingModel.cpp" -o $@

"SNPS2Debug/TeamingModel.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/sim/TeamingModel.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/sim/TeamingModel.cpp" -o $@

"SNPS2Debug/TTLTimeDate.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/main/TTLTimeDate.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/main/TTLTimeDate.cpp" -o $@

"SNPS2Debug/CutSceneMgr.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/cutscene/CutSceneMgr.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/cutscene/CutSceneMgr.cpp" -o $@

"SNPS2Debug/affliction.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/affliction.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/affliction.cpp" -o $@

"SNPS2Debug/DeathControl.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/DeathControl.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/DeathControl.cpp" -o $@

"SNPS2Debug/footfall.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/footfall.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/footfall.cpp" -o $@

"SNPS2Debug/loadout1.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/loadout.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/loadout.cpp" -o $@

"SNPS2Debug/onfire.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/onfire.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/onfire.cpp" -o $@

"SNPS2Debug/painstats.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/painstats.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/painstats.cpp" -o $@

"SNPS2Debug/quicktag.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/quicktag.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/quicktag.cpp" -o $@

"SNPS2Debug/say.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/say.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/say.cpp" -o $@

"SNPS2Debug/shadow.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/shadow.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/shadow.cpp" -o $@

"SNPS2Debug/BipedLadder.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/KineControl/BipedLadder.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/KineControl/BipedLadder.cpp" -o $@

"SNPS2Debug/ViewDriver.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/KineControl/ViewDriver.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/KineControl/ViewDriver.cpp" -o $@

"SNPS2Debug/WallMoves.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/KineControl/WallMoves.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/KineControl/WallMoves.cpp" -o $@

"SNPS2Debug/AiKillbox.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/components/Ai/AiKillbox.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/components/Ai/AiKillbox.cpp" -o $@

"SNPS2Debug/components.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/components.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/components.cpp" -o $@

"SNPS2Debug/customplayer.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/customplayer.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/customplayer.cpp" -o $@

"SNPS2Debug/gamesettings.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/gamesettings.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/gamesettings.cpp" -o $@

"SNPS2Debug/keyboardchat.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/keyboardchat.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/keyboardchat.cpp" -o $@

"SNPS2Debug/savegame.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/savegame.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/savegame.cpp" -o $@

"SNPS2Debug/texteffects.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/texteffects.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/texteffects.cpp" -o $@

"SNPS2Debug/screen_AskPassword.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_AskPassword.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_AskPassword.cpp" -o $@

"SNPS2Debug/screen_BaseClass.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_BaseClass.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_BaseClass.cpp" -o $@

"SNPS2Debug/screen_BasePopup.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_BasePopup.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_BasePopup.cpp" -o $@

"SNPS2Debug/screen_BootOptions.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_BootOptions.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_BootOptions.cpp" -o $@

"SNPS2Debug/screen_Briefing.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Briefing.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Briefing.cpp" -o $@

"SNPS2Debug/screen_clan_create.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_clan_create.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_clan_create.cpp" -o $@

"SNPS2Debug/screen_clan_invite.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_clan_invite.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_clan_invite.cpp" -o $@

"SNPS2Debug/screen_clan_invite_popup.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_clan_invite_popup.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_clan_invite_popup.cpp" -o $@

"SNPS2Debug/screen_clan_list.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_clan_list.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_clan_list.cpp" -o $@

"SNPS2Debug/screen_clan_main.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_clan_main.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_clan_main.cpp" -o $@

"SNPS2Debug/screen_clan_notices.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_clan_notices.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_clan_notices.cpp" -o $@

"SNPS2Debug/screen_clan_popup.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_clan_popup.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_clan_popup.cpp" -o $@

"SNPS2Debug/screen_CountryList.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_CountryList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_CountryList.cpp" -o $@

"SNPS2Debug/screen_CreateProfile.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_CreateProfile.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_CreateProfile.cpp" -o $@

"SNPS2Debug/screen_CreateSession.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_CreateSession.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_CreateSession.cpp" -o $@

"SNPS2Debug/screen_CustomPlayer.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_CustomPlayer.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_CustomPlayer.cpp" -o $@

"SNPS2Debug/screen_DevMenu.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_DevMenu.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_DevMenu.cpp" -o $@

"SNPS2Debug/screen_Dialog.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Dialog.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Dialog.cpp" -o $@

"SNPS2Debug/screen_EOA.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EOA.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EOA.cpp" -o $@

"SNPS2Debug/screen_EORConnect.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EORConnect.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EORConnect.cpp" -o $@

"SNPS2Debug/screen_EORCreateAccount1.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EORCreateAccount1.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EORCreateAccount1.cpp" -o $@

"SNPS2Debug/screen_EORCreateAccount2.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EORCreateAccount2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EORCreateAccount2.cpp" -o $@

"SNPS2Debug/screen_EORDeleteAccount.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EORDeleteAccount.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EORDeleteAccount.cpp" -o $@

"SNPS2Debug/screen_EOREditAccount1.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EOREditAccount1.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EOREditAccount1.cpp" -o $@

"SNPS2Debug/screen_EOREditAccount2.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EOREditAccount2.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EOREditAccount2.cpp" -o $@

"SNPS2Debug/screen_EORLogin.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EORLogin.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EORLogin.cpp" -o $@

"SNPS2Debug/screen_EORRecoverAccount.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_EORRecoverAccount.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_EORRecoverAccount.cpp" -o $@

"SNPS2Debug/screen_friendConfirmed.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_friendConfirmed.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_friendConfirmed.cpp" -o $@

"SNPS2Debug/screen_FriendRequested.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_FriendRequested.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_FriendRequested.cpp" -o $@

"SNPS2Debug/screen_friends.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_friends.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_friends.cpp" -o $@

"SNPS2Debug/screen_FriendsList.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_FriendsList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_FriendsList.cpp" -o $@

"SNPS2Debug/screen_FriendsPopup.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_FriendsPopup.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_FriendsPopup.cpp" -o $@

"SNPS2Debug/screen_GamePadSettings.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_GamePadSettings.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_GamePadSettings.cpp" -o $@

"SNPS2Debug/screen_GameSetting.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_GameSetting.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_GameSetting.cpp" -o $@

"SNPS2Debug/screen_Keyboard.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Keyboard.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Keyboard.cpp" -o $@

"SNPS2Debug/screen_MainMenu.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_MainMenu.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_MainMenu.cpp" -o $@

"SNPS2Debug/screen_MemCard.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_MemCard.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_MemCard.cpp" -o $@

"SNPS2Debug/screen_MultiplayerGameSettings.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_MultiplayerGameSettings.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_MultiplayerGameSettings.cpp" -o $@

"SNPS2Debug/screen_MultiplayerMenu.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_MultiplayerMenu.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_MultiplayerMenu.cpp" -o $@

"SNPS2Debug/screen_MySettings.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_MySettings.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_MySettings.cpp" -o $@

"SNPS2Debug/screen_NetworkConfig.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_NetworkConfig.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_NetworkConfig.cpp" -o $@

"SNPS2Debug/screen_Objectives.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Objectives.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Objectives.cpp" -o $@

"SNPS2Debug/screen_OnlineChat.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_OnlineChat.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_OnlineChat.cpp" -o $@

"SNPS2Debug/screen_OnlineLobby.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_OnlineLobby.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_OnlineLobby.cpp" -o $@

"SNPS2Debug/screen_OnlineMenu.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_OnlineMenu.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_OnlineMenu.cpp" -o $@

"SNPS2Debug/screen_OnlineProfile.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_OnlineProfile.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_OnlineProfile.cpp" -o $@

"SNPS2Debug/screen_Options.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Options.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Options.cpp" -o $@

"SNPS2Debug/screen_Pause.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Pause.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Pause.cpp" -o $@

"SNPS2Debug/screen_players.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_players.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_players.cpp" -o $@

"SNPS2Debug/screen_PlayersList.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_PlayersList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_PlayersList.cpp" -o $@

"SNPS2Debug/screen_Popups.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Popups.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Popups.cpp" -o $@

"SNPS2Debug/screen_Quickmatch.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Quickmatch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Quickmatch.cpp" -o $@

"SNPS2Debug/screen_Sandbox.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Sandbox.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Sandbox.cpp" -o $@

"SNPS2Debug/screen_ScoreBoard.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_ScoreBoard.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_ScoreBoard.cpp" -o $@

"SNPS2Debug/screen_SearchResult.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_SearchResult.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_SearchResult.cpp" -o $@

"SNPS2Debug/screen_Splash.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Splash.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Splash.cpp" -o $@

"SNPS2Debug/screen_stats.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_stats.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_stats.cpp" -o $@

"SNPS2Debug/screen_TagEditor.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_TagEditor.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_TagEditor.cpp" -o $@

"SNPS2Debug/screen_Title.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Title.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Title.cpp" -o $@

"SNPS2Debug/screen_VoiceSetting.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/code/screen_VoiceSetting.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/code/screen_VoiceSetting.cpp" -o $@

"SNPS2Debug/screen_wait.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_wait.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_wait.cpp" -o $@

"SNPS2Debug/screen_Weapons.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_Weapons.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_Weapons.cpp" -o $@

"SNPS2Debug/screen_YesNoDialog.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/screens/screen_YesNoDialog.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/screens/screen_YesNoDialog.cpp" -o $@

"SNPS2Debug/EORAccount.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/EORAccount.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/EORAccount.cpp" -o $@

"SNPS2Debug/GameSettingConfig.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/GameSettingConfig.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/GameSettingConfig.cpp" -o $@

"SNPS2Debug/ParamList.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/ParamList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/ParamList.cpp" -o $@

"SNPS2Debug/StringList.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/StringList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/StringList.cpp" -o $@

"SNPS2Debug/UseCase.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCase.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCase.cpp" -o $@

"SNPS2Debug/UseCaseClans.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseClans.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseClans.cpp" -o $@

"SNPS2Debug/UseCaseDisplayDialog.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseDisplayDialog.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseDisplayDialog.cpp" -o $@

"SNPS2Debug/UseCaseEOA.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEOA.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEOA.cpp" -o $@

"SNPS2Debug/UseCaseEORConfig.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORConfig.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORConfig.cpp" -o $@

"SNPS2Debug/UseCaseEORConnect.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORConnect.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORConnect.cpp" -o $@

"SNPS2Debug/UseCaseEORCreateAccount.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORCreateAccount.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORCreateAccount.cpp" -o $@

"SNPS2Debug/UseCaseEORDeleteAccount.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORDeleteAccount.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORDeleteAccount.cpp" -o $@

"SNPS2Debug/UseCaseEOREditAccount.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEOREditAccount.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEOREditAccount.cpp" -o $@

"SNPS2Debug/UseCaseEORLogin.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORLogin.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORLogin.cpp" -o $@

"SNPS2Debug/UseCaseEORPassword.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORPassword.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORPassword.cpp" -o $@

"SNPS2Debug/UseCaseEORPatcher.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORPatcher.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORPatcher.cpp" -o $@

"SNPS2Debug/UseCaseEORRecoverAccount.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORRecoverAccount.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseEORRecoverAccount.cpp" -o $@

"SNPS2Debug/UseCaseFindMatch.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseFindMatch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseFindMatch.cpp" -o $@

"SNPS2Debug/UseCaseFriends.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseFriends.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseFriends.cpp" -o $@

"SNPS2Debug/UseCaseHostMatch.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseHostMatch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseHostMatch.cpp" -o $@

"SNPS2Debug/UseCaseInvitePlayerToMatch.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseInvitePlayerToMatch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseInvitePlayerToMatch.cpp" -o $@

"SNPS2Debug/UseCaseJoinMatch.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseJoinMatch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseJoinMatch.cpp" -o $@

"SNPS2Debug/UseCaseMultiplayerMenu.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseMultiplayerMenu.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseMultiplayerMenu.cpp" -o $@

"SNPS2Debug/UseCasePlayers.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCasePlayers.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCasePlayers.cpp" -o $@

"SNPS2Debug/UseCaseQuickMatch.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseQuickMatch.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseQuickMatch.cpp" -o $@

"SNPS2Debug/UseCaseStats.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseStats.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseStats.cpp" -o $@

"SNPS2Debug/UseCaseWaitForOSIRequest.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseWaitForOSIRequest.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseWaitForOSIRequest.cpp" -o $@

"SNPS2Debug/UseCaseYesNoDialog.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/UseCaseYesNoDialog.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/UseCaseYesNoDialog.cpp" -o $@

"SNPS2Debug/ValueList.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/frontend/UseCase/ValueList.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/frontend/UseCase/ValueList.cpp" -o $@

"SNPS2Debug/aiming.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/aiming.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/aiming.cpp" -o $@

"SNPS2Debug/aiweapfire.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/aiweapfire.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/aiweapfire.cpp" -o $@

"SNPS2Debug/aiweaphelper.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/aiweaphelper.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/aiweaphelper.cpp" -o $@

"SNPS2Debug/aoeproj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/aoeproj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/aoeproj.cpp" -o $@

"SNPS2Debug/armory.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/armory.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/armory.cpp" -o $@

"SNPS2Debug/armoryview.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/armoryview.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/armoryview.cpp" -o $@

"SNPS2Debug/carbomb.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/carbomb.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/carbomb.cpp" -o $@

"SNPS2Debug/debugshot.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/debugshot.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/debugshot.cpp" -o $@

"SNPS2Debug/ebrass.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/ebrass.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/ebrass.cpp" -o $@

"SNPS2Debug/ebrassLib.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/ebrassLib.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/ebrassLib.cpp" -o $@

"SNPS2Debug/emitproj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/emitproj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/emitproj.cpp" -o $@

"SNPS2Debug/fireproj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/fireproj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/fireproj.cpp" -o $@

"SNPS2Debug/flashproj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/flashproj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/flashproj.cpp" -o $@

"SNPS2Debug/freeproj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/freeproj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/freeproj.cpp" -o $@

"SNPS2Debug/hitsalvo.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/hitsalvo.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/hitsalvo.cpp" -o $@

"SNPS2Debug/loadout.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/loadout.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/loadout.cpp" -o $@

"SNPS2Debug/lsight.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/lsight.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/lsight.cpp" -o $@

"SNPS2Debug/meleeproj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/meleeproj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/meleeproj.cpp" -o $@

"SNPS2Debug/mlsight.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/mlsight.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/mlsight.cpp" -o $@

"SNPS2Debug/muzzle.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/muzzle.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/muzzle.cpp" -o $@

"SNPS2Debug/muzzleLib.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/muzzleLib.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/muzzleLib.cpp" -o $@

"SNPS2Debug/penetrate.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/penetrate.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/penetrate.cpp" -o $@

"SNPS2Debug/seethru.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/seethru.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/seethru.cpp" -o $@

"SNPS2Debug/shockproj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/shockproj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/shockproj.cpp" -o $@

"SNPS2Debug/subsonic.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/subsonic.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/subsonic.cpp" -o $@

"SNPS2Debug/taser.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/taser.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/taser.cpp" -o $@

"SNPS2Debug/tracerset.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/tracerset.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/tracerset.cpp" -o $@

"SNPS2Debug/triggerproj.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/triggerproj.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/triggerproj.cpp" -o $@

"SNPS2Debug/ttlprojectile.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/ttlprojectile.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/ttlprojectile.cpp" -o $@

"SNPS2Debug/ttlweapinv.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/weapons/ttlweapinv.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/weapons/ttlweapinv.cpp" -o $@

"SNPS2Debug/blood.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/effects/blood.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/effects/blood.cpp" -o $@

"SNPS2Debug/fsdsys.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/effects/fsdsys.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/effects/fsdsys.cpp" -o $@

"SNPS2Debug/lasersight.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/effects/lasersight.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/effects/lasersight.cpp" -o $@

"SNPS2Debug/pain_decal.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/effects/pain_decal.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/effects/pain_decal.cpp" -o $@

"SNPS2Debug/tag.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/effects/tag.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/effects/tag.cpp" -o $@

"SNPS2Debug/DOCombatant.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOCombatant.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOCombatant.cpp" -o $@

"SNPS2Debug/DOCombatantDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOCombatantDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOCombatantDDL.cpp" -o $@

"SNPS2Debug/DOGlass.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOGlass.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOGlass.cpp" -o $@

"SNPS2Debug/DOGlassDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOGlassDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOGlassDDL.cpp" -o $@

"SNPS2Debug/dolobbed.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/dolobbed.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/dolobbed.cpp" -o $@

"SNPS2Debug/DOLobbedDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOLobbedDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOLobbedDDL.cpp" -o $@

"SNPS2Debug/DOPlayerAgent.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOPlayerAgent.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOPlayerAgent.cpp" -o $@

"SNPS2Debug/DOPlayerAgentDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOPlayerAgentDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOPlayerAgentDDL.cpp" -o $@

"SNPS2Debug/DOReferee.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOReferee.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOReferee.cpp" -o $@

"SNPS2Debug/DORefereeDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DORefereeDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DORefereeDDL.cpp" -o $@

"SNPS2Debug/dosubsonic.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/dosubsonic.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/dosubsonic.cpp" -o $@

"SNPS2Debug/DOSubsonicDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DOSubsonicDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DOSubsonicDDL.cpp" -o $@

"SNPS2Debug/DSAgentDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSAgentDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSAgentDDL.cpp" -o $@

"SNPS2Debug/DSAppearance.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSAppearance.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSAppearance.cpp" -o $@

"SNPS2Debug/DSAppearanceDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSAppearanceDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSAppearanceDDL.cpp" -o $@

"SNPS2Debug/DSCombatantMode.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSCombatantMode.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSCombatantMode.cpp" -o $@

"SNPS2Debug/DSCombatantModeDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSCombatantModeDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSCombatantModeDDL.cpp" -o $@

"SNPS2Debug/DSCombatantStateDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSCombatantStateDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSCombatantStateDDL.cpp" -o $@

"SNPS2Debug/DSGlassState.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSGlassState.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSGlassState.cpp" -o $@

"SNPS2Debug/DSGlassStateDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSGlassStateDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSGlassStateDDL.cpp" -o $@

"SNPS2Debug/DSLobbedProps.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSLobbedProps.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSLobbedProps.cpp" -o $@

"SNPS2Debug/DSLobbedPropsDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSLobbedPropsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSLobbedPropsDDL.cpp" -o $@

"SNPS2Debug/DSParticipant.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSParticipant.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSParticipant.cpp" -o $@

"SNPS2Debug/DSParticipantDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSParticipantDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSParticipantDDL.cpp" -o $@

"SNPS2Debug/DSProfileDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSProfileDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSProfileDDL.cpp" -o $@

"SNPS2Debug/DSRefereeAssignment.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereeAssignment.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereeAssignment.cpp" -o $@

"SNPS2Debug/DSRefereeAssignmentDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereeAssignmentDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereeAssignmentDDL.cpp" -o $@

"SNPS2Debug/DSRefereeId.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereeId.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereeId.cpp" -o $@

"SNPS2Debug/DSRefereeIdDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereeIdDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereeIdDDL.cpp" -o $@

"SNPS2Debug/DSRefereePhaseDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereePhaseDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereePhaseDDL.cpp" -o $@

"SNPS2Debug/DSRefereePopulate.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereePopulate.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereePopulate.cpp" -o $@

"SNPS2Debug/DSRefereePopulateDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereePopulateDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereePopulateDDL.cpp" -o $@

"SNPS2Debug/DSRefereeRound.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereeRound.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereeRound.cpp" -o $@

"SNPS2Debug/DSRefereeRoundDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereeRoundDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereeRoundDDL.cpp" -o $@

"SNPS2Debug/DSRefereeRules.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereeRules.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereeRules.cpp" -o $@

"SNPS2Debug/DSRefereeRulesDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSRefereeRulesDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSRefereeRulesDDL.cpp" -o $@

"SNPS2Debug/DSSubsonicProps.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSSubsonicProps.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSSubsonicProps.cpp" -o $@

"SNPS2Debug/DSSubsonicPropsDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSSubsonicPropsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSSubsonicPropsDDL.cpp" -o $@

"SNPS2Debug/DSTeamsDDL.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/DSTeamsDDL.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/DSTeamsDDL.cpp" -o $@

"SNPS2Debug/TTLDDF.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/TTLDDF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/TTLDDF.cpp" -o $@

"SNPS2Debug/TTLSession.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/TTLSession.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/TTLSession.cpp" -o $@

"SNPS2Debug/TTLTranslators.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/TTLTranslators.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/TTLTranslators.cpp" -o $@

"SNPS2Debug/TTL_RendezVousDDF.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/NetZTTL/RendezVous/TTL_RendezVousDDF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/NetZTTL/RendezVous/TTL_RendezVousDDF.cpp" -o $@

"SNPS2Debug/RigidTest.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/ActorsTTL/RigidTest.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/ActorsTTL/RigidTest.cpp" -o $@

"SNPS2Debug/HealthIF.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/InterfacesTTL/HealthIF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/InterfacesTTL/HealthIF.cpp" -o $@

"SNPS2Debug/ParticipantIF.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/InterfacesTTL/ParticipantIF.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/InterfacesTTL/ParticipantIF.cpp" -o $@

"SNPS2Debug/crt0.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/crt0.s"
	$(AS) $(AS_FLAGS) -xassembler-with-cpp -Wa,-MD,"$(INTDIR)\crt0.d" "c:/dev/Ttl/code/crt0.s" -o $@

"SNPS2Debug/TTLPCH_SN.obj":
	@echo ProDG Compiling "c:/dev/Ttl/code/TTLPCH_SN.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Ttl/code/TTLPCH_SN.cpp" -o $@

LINK: "SNPS2Debug/TTL.elf"

"SNPS2Debug/TTL.elf" : $(LD_OBJS)\
	"c:/dev/Avalanche/GameLib/Effects/SNPS2Debug/effects.lib"\
	"c:/dev/Avalanche/GameLib/Osi/SNPS2Debug/OSI.lib"\
	"c:/dev/Avalanche/GameLib/WatchClient/SNPS2Debug/WatchClient.lib"\
	"c:/dev/Avalanche/GameLib/path/SNPS2Debug/path.lib"\
	"c:/dev/Avalanche/Engine/Game/SNPS2Debug/Game.lib"\
	"c:/dev/Avalanche/GameLib/Loop/SNPS2Debug/Loop.lib"\
	"c:/dev/Avalanche/Engine/Math/SNPS2Debug/Math.lib"\
	"c:/dev/Avalanche/GameLib/profile/SNPS2Debug/Profile.lib"\
	"c:/dev/Avalanche/GameLib/collide/SNPS2Debug/collide.lib"\
	"c:/dev/Avalanche/GameLib/geomUtil/SNPS2Debug/geomUtil.lib"\
	"c:/dev/Avalanche/GameLib/Environ/SNPS2Debug/Environ.lib"\
	"c:/dev/Avalanche/GameLib/fmv/SNPS2Debug/fmv.lib"\
	"c:/dev/Avalanche/GameLib/mathUtil/SNPS2Debug/mathUtil.lib"\
	"c:/dev/Avalanche/GameLib/Xml/SNPS2Debug/XML.lib"\
	"c:/dev/Avalanche/GameLib/Interfaces/SNPS2Debug/Interfaces.lib"\
	"c:/dev/Avalanche/GameLib/TupperAttrib/SNPS2Debug/TupperAttrib.lib"\
	"c:/dev/Avalanche/Engine/Display/SNPS2Debug/Display.lib"\
	"c:/dev/Avalanche/GameLib/Tupperware/SNPS2Debug/tupperware.lib"\
	"c:/dev/Avalanche/GameLib/platform/SNPS2Debug/platform.lib"\
	"c:/dev/Avalanche/GameLib/CollideHelper/SNPS2Debug/CollideHelper.lib"\
	"c:/dev/Avalanche/GameLib/Network/SNPS2Debug/Network.lib"\
	"c:/dev/Avalanche/GameLib/EngineHelper/SNPS2Debug/EngineHelper.lib"\
	"c:/dev/Avalanche/GameLib/Script/SNPS2Debug/Script.lib"\
	"c:/dev/Avalanche/Engine/Layers/SNPS2Debug/Layers.lib"\
	"c:/dev/Avalanche/GameLib/cmdcon/SNPS2Debug/cmdcon.lib"\
	"c:/dev/Avalanche/GameLib/camera/SNPS2Debug/camera.lib"\
	"c:/dev/Avalanche/Engine/Audio/SNPS2Debug/audio.lib"\
	"c:/dev/Avalanche/GameLib/GameHelper/SNPS2Debug/GameHelper.lib"\
	"c:/dev/Avalanche/GameLib/stage/SNPS2Debug/stage.lib"\
	"c:/dev/Avalanche/GameLib/Components/SNPS2Debug/Components.lib"\
	"c:/dev/Avalanche/GameLib/hud/SNPS2Debug/hud.lib"\
	"c:/dev/Avalanche/GameLib/Navigate/SNPS2Debug/Navigate.lib"
	@echo ProDG Linking $@
	$(LD) $(LD_FLAGS) @_ld_objs.txt -o $@

include "SNPS2Debug/avatarfx.d"
include "SNPS2Debug/sobj.d"
include "SNPS2Debug/deathcam.d"
include "SNPS2Debug/ghostcam.d"
include "SNPS2Debug/lookaround.d"
include "SNPS2Debug/ConfigMgr.d"
include "SNPS2Debug/PlayerSpawnMgr.d"
include "SNPS2Debug/CharacterLoader.d"
include "SNPS2Debug/cheats.d"
include "SNPS2Debug/ErrorMonitor.d"
include "SNPS2Debug/Lobby.d"
include "SNPS2Debug/NetMonitor.d"
include "SNPS2Debug/OSILobby.d"
include "SNPS2Debug/ParticipantConfig.d"
include "SNPS2Debug/PlayerAgent.d"
include "SNPS2Debug/PlayerProfile.d"
include "SNPS2Debug/TTLSessionMgr.d"
include "SNPS2Debug/CustomPlayerLoop.d"
include "SNPS2Debug/InitialLoop.d"
include "SNPS2Debug/KillsScoringModel.d"
include "SNPS2Debug/MultiPlayerLoop.d"
include "SNPS2Debug/NetworkLoop.d"
include "SNPS2Debug/OnlineSetupLoop.d"
include "SNPS2Debug/RaidScoringModel.d"
include "SNPS2Debug/RobberyScoringModel.d"
include "SNPS2Debug/ScoringModel.d"
include "SNPS2Debug/SetupLoop.d"
include "SNPS2Debug/SimpleTeamingModel.d"
include "SNPS2Debug/SinglePlayerLoop.d"
include "SNPS2Debug/StandAloneLoop.d"
include "SNPS2Debug/TagScoringModel.d"
include "SNPS2Debug/TagTeamingModel.d"
include "SNPS2Debug/TeamingModel.d"
include "SNPS2Debug/TTLTimeDate.d"
include "SNPS2Debug/CutSceneMgr.d"
include "SNPS2Debug/affliction.d"
include "SNPS2Debug/DeathControl.d"
include "SNPS2Debug/footfall.d"
include "SNPS2Debug/loadout.d"
include "SNPS2Debug/onfire.d"
include "SNPS2Debug/painstats.d"
include "SNPS2Debug/quicktag.d"
include "SNPS2Debug/say.d"
include "SNPS2Debug/shadow.d"
include "SNPS2Debug/BipedLadder.d"
include "SNPS2Debug/ViewDriver.d"
include "SNPS2Debug/WallMoves.d"
include "SNPS2Debug/AiKillbox.d"
include "SNPS2Debug/components.d"
include "SNPS2Debug/customplayer.d"
include "SNPS2Debug/gamesettings.d"
include "SNPS2Debug/keyboardchat.d"
include "SNPS2Debug/savegame.d"
include "SNPS2Debug/texteffects.d"
include "SNPS2Debug/screen_AskPassword.d"
include "SNPS2Debug/screen_BaseClass.d"
include "SNPS2Debug/screen_BasePopup.d"
include "SNPS2Debug/screen_BootOptions.d"
include "SNPS2Debug/screen_Briefing.d"
include "SNPS2Debug/screen_clan_create.d"
include "SNPS2Debug/screen_clan_invite.d"
include "SNPS2Debug/screen_clan_invite_popup.d"
include "SNPS2Debug/screen_clan_list.d"
include "SNPS2Debug/screen_clan_main.d"
include "SNPS2Debug/screen_clan_notices.d"
include "SNPS2Debug/screen_clan_popup.d"
include "SNPS2Debug/screen_CountryList.d"
include "SNPS2Debug/screen_CreateProfile.d"
include "SNPS2Debug/screen_CreateSession.d"
include "SNPS2Debug/screen_CustomPlayer.d"
include "SNPS2Debug/screen_DevMenu.d"
include "SNPS2Debug/screen_Dialog.d"
include "SNPS2Debug/screen_EOA.d"
include "SNPS2Debug/screen_EORConnect.d"
include "SNPS2Debug/screen_EORCreateAccount1.d"
include "SNPS2Debug/screen_EORCreateAccount2.d"
include "SNPS2Debug/screen_EORDeleteAccount.d"
include "SNPS2Debug/screen_EOREditAccount1.d"
include "SNPS2Debug/screen_EOREditAccount2.d"
include "SNPS2Debug/screen_EORLogin.d"
include "SNPS2Debug/screen_EORRecoverAccount.d"
include "SNPS2Debug/screen_friendConfirmed.d"
include "SNPS2Debug/screen_FriendRequested.d"
include "SNPS2Debug/screen_friends.d"
include "SNPS2Debug/screen_FriendsList.d"
include "SNPS2Debug/screen_FriendsPopup.d"
include "SNPS2Debug/screen_GamePadSettings.d"
include "SNPS2Debug/screen_GameSetting.d"
include "SNPS2Debug/screen_Keyboard.d"
include "SNPS2Debug/screen_MainMenu.d"
include "SNPS2Debug/screen_MemCard.d"
include "SNPS2Debug/screen_MultiplayerGameSettings.d"
include "SNPS2Debug/screen_MultiplayerMenu.d"
include "SNPS2Debug/screen_MySettings.d"
include "SNPS2Debug/screen_NetworkConfig.d"
include "SNPS2Debug/screen_Objectives.d"
include "SNPS2Debug/screen_OnlineChat.d"
include "SNPS2Debug/screen_OnlineLobby.d"
include "SNPS2Debug/screen_OnlineMenu.d"
include "SNPS2Debug/screen_OnlineProfile.d"
include "SNPS2Debug/screen_Options.d"
include "SNPS2Debug/screen_Pause.d"
include "SNPS2Debug/screen_players.d"
include "SNPS2Debug/screen_PlayersList.d"
include "SNPS2Debug/screen_Popups.d"
include "SNPS2Debug/screen_Quickmatch.d"
include "SNPS2Debug/screen_Sandbox.d"
include "SNPS2Debug/screen_ScoreBoard.d"
include "SNPS2Debug/screen_SearchResult.d"
include "SNPS2Debug/screen_Splash.d"
include "SNPS2Debug/screen_stats.d"
include "SNPS2Debug/screen_TagEditor.d"
include "SNPS2Debug/screen_Title.d"
include "SNPS2Debug/screen_VoiceSetting.d"
include "SNPS2Debug/screen_wait.d"
include "SNPS2Debug/screen_Weapons.d"
include "SNPS2Debug/screen_YesNoDialog.d"
include "SNPS2Debug/EORAccount.d"
include "SNPS2Debug/GameSettingConfig.d"
include "SNPS2Debug/ParamList.d"
include "SNPS2Debug/StringList.d"
include "SNPS2Debug/UseCase.d"
include "SNPS2Debug/UseCaseClans.d"
include "SNPS2Debug/UseCaseDisplayDialog.d"
include "SNPS2Debug/UseCaseEOA.d"
include "SNPS2Debug/UseCaseEORConfig.d"
include "SNPS2Debug/UseCaseEORConnect.d"
include "SNPS2Debug/UseCaseEORCreateAccount.d"
include "SNPS2Debug/UseCaseEORDeleteAccount.d"
include "SNPS2Debug/UseCaseEOREditAccount.d"
include "SNPS2Debug/UseCaseEORLogin.d"
include "SNPS2Debug/UseCaseEORPassword.d"
include "SNPS2Debug/UseCaseEORPatcher.d"
include "SNPS2Debug/UseCaseEORRecoverAccount.d"
include "SNPS2Debug/UseCaseFindMatch.d"
include "SNPS2Debug/UseCaseFriends.d"
include "SNPS2Debug/UseCaseHostMatch.d"
include "SNPS2Debug/UseCaseInvitePlayerToMatch.d"
include "SNPS2Debug/UseCaseJoinMatch.d"
include "SNPS2Debug/UseCaseMultiplayerMenu.d"
include "SNPS2Debug/UseCasePlayers.d"
include "SNPS2Debug/UseCaseQuickMatch.d"
include "SNPS2Debug/UseCaseStats.d"
include "SNPS2Debug/UseCaseWaitForOSIRequest.d"
include "SNPS2Debug/UseCaseYesNoDialog.d"
include "SNPS2Debug/ValueList.d"
include "SNPS2Debug/aiming.d"
include "SNPS2Debug/aiweapfire.d"
include "SNPS2Debug/aiweaphelper.d"
include "SNPS2Debug/aoeproj.d"
include "SNPS2Debug/armory.d"
include "SNPS2Debug/armoryview.d"
include "SNPS2Debug/carbomb.d"
include "SNPS2Debug/debugshot.d"
include "SNPS2Debug/ebrass.d"
include "SNPS2Debug/ebrassLib.d"
include "SNPS2Debug/emitproj.d"
include "SNPS2Debug/fireproj.d"
include "SNPS2Debug/flashproj.d"
include "SNPS2Debug/freeproj.d"
include "SNPS2Debug/hitsalvo.d"
include "SNPS2Debug/loadout.d"
include "SNPS2Debug/lsight.d"
include "SNPS2Debug/meleeproj.d"
include "SNPS2Debug/mlsight.d"
include "SNPS2Debug/muzzle.d"
include "SNPS2Debug/muzzleLib.d"
include "SNPS2Debug/penetrate.d"
include "SNPS2Debug/seethru.d"
include "SNPS2Debug/shockproj.d"
include "SNPS2Debug/subsonic.d"
include "SNPS2Debug/taser.d"
include "SNPS2Debug/tracerset.d"
include "SNPS2Debug/triggerproj.d"
include "SNPS2Debug/ttlprojectile.d"
include "SNPS2Debug/ttlweapinv.d"
include "SNPS2Debug/blood.d"
include "SNPS2Debug/fsdsys.d"
include "SNPS2Debug/lasersight.d"
include "SNPS2Debug/pain_decal.d"
include "SNPS2Debug/tag.d"
include "SNPS2Debug/DOCombatant.d"
include "SNPS2Debug/DOCombatantDDL.d"
include "SNPS2Debug/DOGlass.d"
include "SNPS2Debug/DOGlassDDL.d"
include "SNPS2Debug/dolobbed.d"
include "SNPS2Debug/DOLobbedDDL.d"
include "SNPS2Debug/DOPlayerAgent.d"
include "SNPS2Debug/DOPlayerAgentDDL.d"
include "SNPS2Debug/DOReferee.d"
include "SNPS2Debug/DORefereeDDL.d"
include "SNPS2Debug/dosubsonic.d"
include "SNPS2Debug/DOSubsonicDDL.d"
include "SNPS2Debug/DSAgentDDL.d"
include "SNPS2Debug/DSAppearance.d"
include "SNPS2Debug/DSAppearanceDDL.d"
include "SNPS2Debug/DSCombatantMode.d"
include "SNPS2Debug/DSCombatantModeDDL.d"
include "SNPS2Debug/DSCombatantStateDDL.d"
include "SNPS2Debug/DSGlassState.d"
include "SNPS2Debug/DSGlassStateDDL.d"
include "SNPS2Debug/DSLobbedProps.d"
include "SNPS2Debug/DSLobbedPropsDDL.d"
include "SNPS2Debug/DSParticipant.d"
include "SNPS2Debug/DSParticipantDDL.d"
include "SNPS2Debug/DSProfileDDL.d"
include "SNPS2Debug/DSRefereeAssignment.d"
include "SNPS2Debug/DSRefereeAssignmentDDL.d"
include "SNPS2Debug/DSRefereeId.d"
include "SNPS2Debug/DSRefereeIdDDL.d"
include "SNPS2Debug/DSRefereePhaseDDL.d"
include "SNPS2Debug/DSRefereePopulate.d"
include "SNPS2Debug/DSRefereePopulateDDL.d"
include "SNPS2Debug/DSRefereeRound.d"
include "SNPS2Debug/DSRefereeRoundDDL.d"
include "SNPS2Debug/DSRefereeRules.d"
include "SNPS2Debug/DSRefereeRulesDDL.d"
include "SNPS2Debug/DSSubsonicProps.d"
include "SNPS2Debug/DSSubsonicPropsDDL.d"
include "SNPS2Debug/DSTeamsDDL.d"
include "SNPS2Debug/TTLDDF.d"
include "SNPS2Debug/TTLSession.d"
include "SNPS2Debug/TTLTranslators.d"
include "SNPS2Debug/TTL_RendezVousDDF.d"
include "SNPS2Debug/RigidTest.d"
include "SNPS2Debug/HealthIF.d"
include "SNPS2Debug/ParticipantIF.d"
include "SNPS2Debug/crt0.d"
include "SNPS2Debug/TTLPCH_SN.d"

