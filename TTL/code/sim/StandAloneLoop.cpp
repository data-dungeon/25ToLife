/*
**
**  File:   StandAloneLoop.h
**  Date:   September 29, 2004
**  By:     Food
**  Desc:   Gameplay loop for multi-player sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
*/

// Precompiled header.
#include "TTLPCH.h"

// My header
#include "sim/StandAloneLoop.h"

// GameLib headers
#include "Network/Session.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "EngineHelper/LoadScreen.h"

// TTL headers
#include "frontend/frontend.h"
#include "main/PlayerProfile.h"
#include "main/PlayerAgent.h"
#include "main/PlayerDB.h"
#include "sim/Referee.h"
#include "environ/PlayerSpawnMgr.h"
#include "frontend/savegame.h"
#include "GameHelper/FileLoader.h"
#include "fmv/fmv.h"
#include "weapons/armoryview.h"
#include "avatar/sobj.h"
#include "environ/ActorCreator.h"
#include "frontend/savegame.h"
#include "Display/TextureUtil.h"

#ifdef NETZ
#include "NetZTTL/TTLSession.h"
#endif

#ifdef DIRECTX_PC
#include "GameHelper/SharedBody.h"
#include "avatar/pstatSP.h"
#include "weapons/ttlweapinv.h"
#endif

#ifdef MASSIVE_AD
#include "Massive/MassiveClientDriver.h"
#endif

#if defined (_XBOX)
#include "platform/Xbox/XboxLive.h"
#endif

#include "Voice/NetZ/CVoiceChat.h"

//#define ASYNC_LOAD

#define MIN_POPULATING_TIME   0 // 5000


// async memcard stuff
static	int	m_MemCardState = STANDALONE_IDLE;

// Initialize function array.
SLoopEntry CStandAloneLoop::s_stateArray[CStandAloneLoop::NUM_STATES] =
{
	"INITIALIZE",      (LoopFunc)&CStandAloneLoop::Initialize,
	"WAIT_TO_PUBLISH", (LoopFunc)&CStandAloneLoop::Wait,
	"PUBLISH",         (LoopFunc)&CStandAloneLoop::Publish,
	"WAIT_TO_RUN",     (LoopFunc)&CStandAloneLoop::Wait,
	"RUN",             (LoopFunc)&CStandAloneLoop::Run,
	"REPORTING",       (LoopFunc)&CStandAloneLoop::Reporting,
	"UNPUBLISH",       (LoopFunc)&CStandAloneLoop::Unpublish,
	"TERMINATE",       (LoopFunc)&CStandAloneLoop::Terminate
};

extern void FreeOurMem();
extern void AllocOurMem();

/******************************************************************************/
/******************************************************************************/
CStandAloneLoop::CStandAloneLoop() : CTTLLoop(s_stateArray, UNINITIALIZED)
{
	// Reset owner name.
	d_messageOwner.SetOwnerName("StandAloneLoop");
}

/******************************************************************************/
/******************************************************************************/
int CStandAloneLoop::DetermineState(int i_state)
{
// update memcard state machine
	MemCardStateEngine();

	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
		d_requestExit = false;
		d_loaded = false;
		d_switchingLevels = false;
		d_switchLevelTime = 0;
		d_nextLevel = -1;
		m_MemCardState = STANDALONE_IDLE;
		return(INITIALIZE);

	 case INITIALIZE:
		if (!d_stateSucceeded)
			return(UNINITIALIZED);
		if (d_requestExit)
			return(TERMINATE);
		RenderMgr__FadeOut(FADE_TO_BLACK_INSTANT);
		return(WAIT_TO_PUBLISH);

	 case WAIT_TO_PUBLISH:
		if (d_requestExit)
			return(TERMINATE);
#ifdef ASYNC_LOAD
		if (g_gameEnvironment.ProcessPreload())
			d_localAgent->SetMode(CPlayerAgent::LOADED);
#endif
		if (g_referee.GetPhase() == Referee::POPULATING)
		{
			if (!Load())
				return(TERMINATE);
			return(PUBLISH);
		}
		return(WAIT_TO_PUBLISH);

	 case PUBLISH:
		if (!d_stateSucceeded)
			return(TERMINATE);
		if (d_requestExit)
			return(UNPUBLISH);
		return(WAIT_TO_RUN);

	 case WAIT_TO_RUN:
//#ifdef DIRECTX_PC
//		g_MenuCursor.SetHidden(true);
//#endif
		if (d_requestExit)
			return(UNPUBLISH);
		if (g_referee.GetPhase() == Referee::PLAYING)
		{
			RenderMgr__FadeIn(FADE_TO_BLACK_SLOW);
			return(RUN);
		}
		return(WAIT_TO_RUN);

	 case RUN:
		 if( g_referee.GetPhase() == Referee::REPORTING )
		 {
// DAS 08/15/2005, we don't want the background to fade out here
//			 RenderMgr__FadeOut( FADE_TO_BLACK_FAST, false, 0x0000 );
			 g_FrontEnd.GotoScreen( "SCREEN_ID_STATISTICS" );
			 return(REPORTING);
		 }
		if (g_referee.GetPhase() != Referee::PLAYING || d_requestExit)
			return(UNPUBLISH);
		return(RUN);

	 case REPORTING:
		 if (g_FrontEnd.GetActiveScreen() == 0)
		 {
//pab			 if (g_MemCardManager->GetAutosave())		// removed autosave
			 if (1)
			 {
				 if (m_MemCardState == STANDALONE_IDLE)
					 m_MemCardState = STANDALONE_AUTOSAVE;
				 else if (m_MemCardState == STANDALONE_AUTOSAVE_DONE)
					 return(UNPUBLISH);
			 }
			 else
				 return(UNPUBLISH);
		 }
		 break;

	 case UNPUBLISH:
#ifdef DIRECTX_PC
		 g_MenuCursor.SetHidden(false);
#endif
		 // Are we saving a game
		if (g_referee.GetPhase() != Referee::REPORTING)
		{
			d_switchingLevels = false;
			return(TERMINATE);
		}
		else
		{
//			d_switchingLevels = !g_gameEnvironment.EndOfGame();
			RenderMgr__FadeOut(FADE_TO_BLACK_FAST);
			// draw two frames so auto-save icon doesn't flicker on fadeout.
			TTLRender();
			TTLRender();
			return(WaitForTask(&RenderMgr__IsFadedOut, (LoopFunc)&CStandAloneLoop::Wait, TERMINATE));
		}
		break;

	 case TERMINATE:
		return(UNINITIALIZED);
	}

	// Let any externally defined states pass through.
	return(i_state);
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::Initialize(void)
{
	g_ControllerRemoved = 0;

	// free NetZ Memory
#ifdef NETZ
	FreeOurMem();
	CNetZConnection::unLoadNetworkDLLs();
#endif

	ActorCreator::SetCombatantLOD( COMBATANT_LOD_MID );

	// clear invite flag
#if defined(_XBOX)
	CXboxLive::SetInviteFound(false);
#endif

	// This is an unseemly sort of hack, but if we got here by skipping
	// all kinds of frontend (i.e. debugging) then we probably don't
	// have a controller yet.
	if (!g_input.ControllerLocked(0))
		g_input.LockPlayerToController( 0, 0 );

	// Do we need to play a movie here?
	if (g_gameEnvironment.StartOfGame())
		FMVPlayer::Play("%1/begin", true);

	const char *movieName = g_gameEnvironment.GetIntroMovie();
	if (movieName)
		FMVPlayer::Play(movieName, true);

	// The end of round movie
	d_endofLevelFMV = NULL;

	// Make sure this is running
	LoadScreen::Kickoff();

	// enable/disable gamma on load
	if ( Math::Equal(g_GlobalProfile.GetGammaValue(),PROFILE_DEFAULT_GAMMA))
		TextureUtil::SetGammaOnLoadFlag(false);
	else
		TextureUtil::SetGammaOnLoadFlag(true);

	// The front end should know what screens to load
	g_FrontEnd.SetGameType( FrontEnd::GAMETYPE_SINGLEPLAYERGAME );

	// Do TTL initialization
	BeginTTLInitialize();

	// Setup the weapon restrictions for this mode
	g_armoryView.ClearRestrictions();
	g_armoryView.SetGameMode(ArmoryView::SinglePlayer);
	g_armoryView.RestrictSuperweapons();

	// Make a simple session.
	d_sessionMgr.Open();
	g_referee.EnablePhaseChanges();

	// Load any teaming model.
	g_referee.LoadTeamingModel();

	// Get the local agent and his assignment.
	d_localAgent = g_referee.CreateLocalPlayerAgent();
	d_localAgent->SetMode(CPlayerAgent::UNLOADED);
	g_referee.RegisterParticipant(d_localAgent);
	ASSERT(d_localAgent->GetAssignment() == CPlayerAgent::ASSIGNED);

	// Register message handlers.
	d_quitHandler.Initialize("Quit", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	m_switchLevelHandler.Initialize("SwitchLevel", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_acceptInvitationHandler.Initialize("AcceptInvitation",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

#ifdef MASSIVE_AD
		//Load in Zone - Hard Coded for testing
		MassiveClientLoadZone(g_gameEnvironment.GetWorldFileName());
#endif


#ifndef ASYNC_LOAD
	g_fileLoader.LoadWorldFiles(d_environment->WorldName());

	// Tag init
	NewSingleton(CustomTag);
	NewSingleton(ModeTag);
	g_tag.Init();

	// Load files associated with the environment.
	d_stateSucceeded = TTLLoadEnvironment(1);
	ASSERT(d_stateSucceeded);

	// World extents
	AABB worldBox = g_collisionSystem.WorldBox();
	g_world.SetExtents(worldBox.minV, worldBox.maxV, 5.0f);

	// The munition must be created after the world.
	NewSingleton(TTLMunition);

	// Setup debugging.
#ifndef CONSUMER_BUILD
	d_ttlDebug.Initialize(this);
#endif

	// Finish initialization.
	EndTTLInitialize();

	// The agent is LOADED.
	d_localAgent->SetMode(CPlayerAgent::LOADED);
#endif

	g_secondaryObjective.SetLevel( g_gameEnvironment.GetWorldFileName() ); //temp hack!
	g_secondaryObjective.Init();
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::Wait(void)
{
	d_sessionMgr.Update();
	g_FrontEnd.Update();
	d_messageOwner.HandlePostedMessages();
	d_scenes.Run();
	TTLRender();
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::Publish(void)
{
	// Load world
	s_worldLoader.Process();

	// Create player.
	g_players.CreateFromStrings(&d_scenes, d_localAgent->Id(),
		PlayerDB::CharacterInitString(g_playerProfile.GetCharacter(CPlayerProfile::COP)),
		g_playerProfile.GetWeaponLoadout(CPlayerProfile::COP));

	// Sync up dynamic collision entites.
	g_collisionSystem.ProtagonistsMoved();

	// Place the avatar.
	g_players.AttachHud(0);
	g_players.RespawnPlayer(0, true, true, true, true);

	// Do the mission briefing
	g_FrontEnd.ShowMissionBriefing();

#ifdef DIRECTX_PC
	SetCheckpoint();
#endif

	// Load the custom tags
	if (ValidSingleton(CustomTag))
		g_customTag.Initialize();

	// if not allowed to join, d_stateSucceeded = false;
	d_localAgent->SetMode(CPlayerAgent::PUBLISHED);
	d_stateSucceeded = true;
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::Run(void)
{
	BeginTTLRun();

#ifndef CONSUMER_BUILD
	d_ttlDebug.InterceptControls();
#endif

	InterceptControls();
	d_sessionMgr.Update();
	EndTTLRun();

#ifndef CONSUMER_BUILD
	d_ttlDebug.Update();
#endif

	// update secondary objective tracking
	g_secondaryObjective.Update();

	// Handle the switch level
	HandleSwitchLevel();

	// check for invites for multiplayer games.. (XBOX only)
#if defined (_XBOX)
	if (CXboxLive::AreWeLoggedIn())
	{
		bool result = CXboxLive::CheckInvites(0,  XONLINE_NOTIFICATION_GAME_INVITE );

		if (result == true)
			CXboxLive::SetInviteFound(true);
	}

	if (CXboxLive::GetInviteFound() == true)
	{
		// display notification of invite
		g_FrontEnd.SetInvite( Invitation::GameInvite_Received, "Match Invite");
		g_FrontEnd.ShowInviteIcon( 255 );
		g_FrontEnd.ShowInviteText( 255 );
	}
#endif

	TTLRender();

#ifdef DIRECTX_PC
	// check for checkponts
	if (g_gameEnvironment.GetCheckpointFlag())
		SetCheckpoint();
#endif
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::MemCardStateEngine()
{
	switch (m_MemCardState)
	{
	case	STANDALONE_IDLE:
	case	STANDALONE_AUTOSAVE_DONE:
		break;

	case	STANDALONE_AUTOSAVE:
		char	NewFileName[128];

		// set filename ext
		g_MemCardManager->SetExt(SAVEGAME_EXT);

		// setup fname
		sprintf(NewFileName, "%s%s", SAVEGAME_PREFIX, g_SaveGameParams.ProfileName);
		strcpy(g_GlobalProfile.ProfileName, NewFileName);

		// get custom chars from player database
		g_GlobalProfile.SaveCustomChars();

		// Copy game locks from GameLock system.
		g_GlobalProfile.SaveGameLocks();

		// update settings
		CGameSettings::SaveSettings();
#ifndef DIRECTX_PC
		g_MemCardManager->SetUsePopups(true);
#endif
		g_MemCardManager->SetOverwriteConfirm(true);
		g_MemCardManager->SetAreYouReallySure(true);
		g_MemCardManager->SetUseActualFileSize(false);
		g_MemCardManager->SaveFile(SAVEGAME_FILENAME, NewFileName, g_SaveGameParams.Port, (char*)&g_GlobalProfile, sizeof(g_GlobalProfile), true, MC_SAVED_GAME);
		m_MemCardState = STANDALONE_AUTOSAVE_RESULT;
		break;

	case	STANDALONE_AUTOSAVE_RESULT:
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
		{
			g_MemCardManager->ResetLoadSaveBuffer();
			m_MemCardState = STANDALONE_AUTOSAVE_DONE;
		}

		break;

	default:
		break;

	}

	// popup needed?
	if (g_MemCardManager->GetPopupRequest())
	{
		g_FrontEnd.ShowPopupCallBack();
		g_MemCardManager->SetPopupRequest(false);
	}

	// remove popup?
	if (g_MemCardManager->GetPopupClearRequest())
	{
		g_FrontEnd.HidePopupCallBack();
		g_MemCardManager->SetPopupClearRequest(false);
	}
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::Reporting(void)
{
	//	BeginTTLRun();

#ifndef CONSUMER_BUILD
	d_ttlDebug.InterceptControls();
#endif

	//	InterceptControls();
	d_sessionMgr.Update();
	//	EndTTLRun();

#ifndef CONSUMER_BUILD
	d_ttlDebug.Update();
#endif

	TTLRender();
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::Unpublish(void)
{
	// Must clear these out
	g_audio.jukebox.ClearPlayPositions();

	// Clean up our state
	g_players.ClearVibration();
	g_players.ActivateControls(false);

	g_players.Destroy();
	g_stage.Terminate();
	//	g_avatarFX.Reset();
	g_stage.Initialize();
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::Terminate(void)
{
	// Cleanup the tags
	DeleteSingleton(CustomTag);
	DeleteSingleton(ModeTag);

#ifndef ASYNC_LOAD

#ifndef CONSUMER_BUILD
	d_ttlDebug.Terminate();
#endif

	DeleteSingleton(TTLMunition);
	TTLUnloadEnvironment();
#endif
	TTLTerminate();

	d_sessionMgr.Close();

	TTLReset();

	// Do we need to play a movie here?
	if (d_endofLevelFMV)
	{
		// The end of round movie
		FMVPlayer::Play(d_endofLevelFMV, true);
		d_endofLevelFMV = NULL;
		if (g_gameEnvironment.EndOfGame())
		{
			FMVPlayer::Play("%1/end", true);
			FMVPlayer::Play("%2/endcred", true);
		}
	}

#ifdef NETZ
	AllocOurMem();	// realloc netz mem.
	CNetZConnection::loadNetworkDLLs();
#endif
}

/******************************************************************************/
/******************************************************************************/
bool CStandAloneLoop::Load(void)
{
	if (!d_loaded)
	{
#ifdef ASYNC_LOAD
		// Load files associated with the environment.
		bool success = TTLLoadEnvironment(1);
		ASSERT(success);

		// World extents
		AABB worldBox = g_collisionSystem.WorldBox();
		g_world.SetExtents(worldBox.minV, worldBox.maxV, 5.0f);

		// Load munitions.
		NewSingleton(TTLMunition);

		// Setup debugging.
#ifndef CONSUMER_BUILD
		d_ttlDebug.Initialize(this);
#endif

		// Finish initialization.
		EndTTLInitialize();
#endif
		d_loaded = true;
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::Unload(void)
{
	if (d_loaded)
	{
#ifdef ASYNC_LOAD

#ifndef CONSUMER_BUILD
		d_ttlDebug.Terminate();
#endif

		DeleteSingleton(TTLMunition);
		TTLUnloadEnvironment();
#endif
		d_loaded = false;
	}
}

/******************************************************************************/
/******************************************************************************/
bool CStandAloneLoop::InterceptControls(void)
{
#if defined (_XBOX) || defined (PS2)
	if (g_ControllerRemoved == 1)
		return false;
#endif

	ControllerInputDriver* device = g_players.GetDevice(0, true);
	if (device != NULL)
	{
#ifndef DIRECTX_PC
		if (device->ButtonPressed(CB_START))
#else // DIRECTX_PC
#ifdef _DEBUG
		if (device->ButtonPressed(CB_START))
#else
		if (device->ButtonPressed(CB_ESCAPE))
#endif // _DEBUG
#endif // DIRECTX_PC
		{
			if (!RenderMgr__IsFadingIn() && !RenderMgr__IsFadingOut())
			{
				SPause pause;
				pause.d_type = SPause::PAUSE;
				pause.d_menu = SCREEN_PAUSEMENU;

				pause.d_affectAudio = true;
				g_messageDispatcher.PostMessageToAll("Pause", (void*)&pause,
					INVALID_OBJECT_HANDLE);
				return(true);
			}
		}
		else
			if( device->ButtonPressed(CB_SELECT))
			{
				g_FrontEnd.ToggleObjectives();
			}
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CStandAloneLoop::HandleSwitchLevel()
{
	// Do we need to do work yet?
	if (d_switchLevelTime == 0 || g_timer.GetEndMS() < d_switchLevelTime)
		return;
	d_switchLevelTime = 0;

	// Can we switch?
	if (d_nextLevel >= 0)
	{
		// Set it to the new level
		if (d_nextLevel > g_GlobalProfile.GetCurrentLevel())
			g_GlobalProfile.SetCurrentLevel(d_nextLevel);
		g_gameEnvironment.Set(d_nextLevel);
		d_switchingLevels = true;
	}
	g_referee.CommandPlayingPhaseExit();

#ifdef DIRECTX_PC
	g_gameEnvironment.SetCheckpoint(0);
#endif
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::CQuit::HandleMessage(void* i_data, ActorHandle i_sender,
														 bool i_posted)
{
	d_ref->RequestExit();
}

/******************************************************************************/
/******************************************************************************/
void CStandAloneLoop::CAcceptInvitation::HandleMessage(void* i_data,
																		 ActorHandle i_sender, bool i_posted)
{
	d_ref->RequestExit();
}

/***************************************************************************/
/***************************************************************************/
void CStandAloneLoop::CSwitchLevel::HandleMessage(void* i_data,
																  ActorHandle i_sender, bool i_posted )
{
	// Where do they want to go?
	ASSERT_PTR(i_data);
	const char *requestLevel = (const char *)i_data;

	// Are we going to have an end of level movie?
	d_ref->d_endofLevelFMV = g_gameEnvironment.GetEndingMovie();

	// set level we're going to switch to
	if (strcmpi(requestLevel, "quit") == 0)
		d_ref->d_nextLevel = -1;
	else
	{
		d_ref->d_nextLevel = g_gameEnvironment.GetByFilename(requestLevel);
		ASSERT(d_ref->d_nextLevel >= 0);
	}
	int currentLevel = g_gameEnvironment.Get();

	// mark the current level as complete
	g_GlobalProfile.SetCompletedLevel(currentLevel, true);
	g_GlobalProfile.SetPreviousLevel(currentLevel);

	// unlock stuff for completing level
	char key[64];
	sprintf(key, "Complete_Lvl_%d", currentLevel + 1);
	if (GameLocks::IsValidKey(key))
		GameLocks::Unlock(key);

	// Setup the switch
	if (d_ref->d_nextLevel >= 0)
		d_ref->d_switchLevelTime = 1; // do it now!
	else
	{
		// Let the events sink in
		//g_players.ActivateControls(false);
		d_ref->d_switchLevelTime = g_timer.GetEndMS() + 500;
	}
}

#ifdef DIRECTX_PC
void CStandAloneLoop::SetCheckpoint()
{
	g_gameEnvironment.SetCheckpointFlag(false);
	if( g_players.GetPlayer(0))
	{
	int level = g_gameEnvironment.Get();
	int latestCheckpoint = g_gameEnvironment.GetCheckpoint();
	if ( latestCheckpoint > 0)
	{
		if (g_gameEnvironment.GetCheckpointFlag())
		{
			// basically unpublish and then republish the level.
			//unload stuff
			g_players.Destroy();
			g_stage.Terminate();
			g_stage.Initialize();
			// then reload
			s_worldLoader.Process();
			// Create player.
			g_players.CreateFromStrings(&d_scenes, d_localAgent->Id(),
				PlayerDB::CharacterInitString(g_playerProfile.GetCharacter(CPlayerProfile::COP)),
				g_playerProfile.GetWeaponLoadout(CPlayerProfile::COP));
			// Sync up dynamic collision entites.
			g_collisionSystem.ProtagonistsMoved();
		}
		g_MenuCursor.SetHidden(true);

		ts_Checkpoint checkpoint = g_GlobalProfile.GetLevelCheckpoint( level, latestCheckpoint );
		// set up the world
		g_PlayerSpawnMgr.AddBox( checkpoint.pos, checkpoint.facing );
		CTTLLoop::WorldLoader().RecreateActorsForCheckpoint( latestCheckpoint );
		// spawn our player
		g_players.RespawnPlayer(0, true, false, false, true);
		g_players.AttachHud(0);
		// set our player's stats
		// first the health
		PlayerStatsSP *stats =
			(PlayerStatsSP *)g_players.GetPlayer(0)->GetComponentByName( "PlayerStatsSP" );
		stats->SetHealth( (float)checkpoint.checkpointStats.Health );
		// and then the ammo
		TTLWeaponInventory *inv = (TTLWeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
		inv->RemoveAll();
		for (int c=0; c < SG_MAX_WEAPONS_PER_PLAYER; ++c)
		{
			TTLWeapon *weapon = (TTLWeapon *)g_weaponLib.CreateInstance(checkpoint.checkpointWeapons[c].WeaponName);
			if (!weapon)
				continue;
			weapon->SetAmmo(weapon->GetAmmoType(), checkpoint.checkpointWeapons[c].Ammo, true);
			inv->Add( weapon, weapon->GetSlot() );
		}
		// and eventually the level specific stuff
		g_GlobalProfile.SetLevelSpecificCharacterInfo( level, latestCheckpoint );
	}
	}
	SPause pauseMessage;
	pauseMessage.d_type = SPause::TOGGLE;
	pauseMessage.d_menu = NULL;
}
#endif
