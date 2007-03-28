/*
**
**  File:   SetupLoop.cpp
**  Date:   September 21, 2004
**  By:     Bryant Collard
**  Desc:   Meta loop used to setup sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/SetupLoop.cpp $
**  $Revision: #37 $
**  $DateTime: 2005/08/24 16:05:52 $
**    $Author: Ken_Harward $
**
*/

// Precompiled header.
#include "TTLPCH.h"

#include "EngineHelper/LoadScreen.h"

// Our camera
#include "camera/lookaround.h"
#include "Components/PathFollowerCam.h"

// My header
#include "sim/SetupLoop.h"

#ifdef NETZ // Sigh
#include "Network/NetZ/NetZConnection.h"
#endif

#include "platform/MemoryCard/MemoryCard.h"
#include "frontend/frontend.h"
#include "weapons/armoryview.h"
#include "main/TTL.h"
#include "environ/ActorCreator.h"

#ifdef MASSIVE_AD
#include "Massive/MassiveClientDriver.h" //added for PC version only for Massive Ad Client
#endif

#ifdef DIRECTX_PC
int g_buttonAbstractWeAreBinding = -1; // SUPER-GHETTO-HACK
#endif // DIRECTX_PC



// Initialize function array.
SLoopEntry CSetupLoop::s_stateArray[CSetupLoop::NUM_STATES] =
{
	"MAIN_MENU",          (LoopFunc)&CSetupLoop::Run,
	"CUSTOM_PLAYER",      (LoopFunc)&CSetupLoop::Run,
	"SINGLEPLAYER_LOBBY", (LoopFunc)&CSetupLoop::Run,
	"LAN_LOBBY",          (LoopFunc)&CSetupLoop::Run,
	"ONLINE_LOBBY",       (LoopFunc)&CSetupLoop::Run,
	"DEV_LOBBY",          (LoopFunc)&CSetupLoop::Run,
	"NUM_LOOPS",          (LoopFunc)NULL,
	"INITIALIZE",         (LoopFunc)&CSetupLoop::Initialize,
	"TERMINATE",          (LoopFunc)&CSetupLoop::Terminate
};

/******************************************************************************/
/******************************************************************************/
CSetupLoop::CSetupLoop() : CTTLLoop(s_stateArray, UNINITIALIZED)
{
	// Loop has yet to be run.
	d_topLevelLoop = ENTER_LOOP;

	// Load sub-loop array.
	d_subLoop[MAIN_MENU]          = &d_mainMenu;
	d_subLoop[CUSTOM_PLAYER]      = &d_customPlayer;
	d_subLoop[SINGLEPLAYER_LOBBY] = &d_singlePlayerLobby;
#if defined(NETWORKING)
	d_subLoop[LAN_LOBBY]          = &d_lanLobby;
#if defined( _XBOX ) || defined( PS2 ) || defined ( DIRECTX_PC )
	d_subLoop[ONLINE_LOBBY]       = &d_onlineLobby;
#endif
#endif
	d_subLoop[DEV_LOBBY]          = &d_devLobby;
	// Clear parameters.
	d_params = NULL;

	// Reset owner name.
	d_messageOwner.SetOwnerName("SetupLoop");
}

/******************************************************************************/
/******************************************************************************/
void CSetupLoop::RequestExit(void)
{
	int loopState = GetLoopState();
	if ((loopState >= 0) && (loopState < NUM_LOOPS))
		d_subLoop[loopState]->RequestExit();
	d_topLevelLoop = EXIT_LOOP;
	CTTLLoop::RequestExit();
}

/******************************************************************************/
/******************************************************************************/
void CSetupLoop::ClearRequestExit(void)
{
	int loopState = GetLoopState();
	if ((loopState >= 0) && (loopState < NUM_LOOPS))
		d_subLoop[loopState]->ClearRequestExit();
	CTTLLoop::ClearRequestExit();
}

/******************************************************************************/
/******************************************************************************/
int CSetupLoop::DetermineState(int i_state)
{
	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
		if (d_requestExit)
			return(UNINITIALIZED);

		// Let's make this a nicer transition
		RenderMgr__FadeIn(FADE_TO_BLACK_SLOW);

		// If this is the first time in the setup loop, parse the .var file and
		// command line overrides. If a level has been set, go right into a
		// sim loop.
		if (d_topLevelLoop == ENTER_LOOP)
		{
			if (d_devLobby.ParseOverrides())
			{
				d_devLobby.Launch();
				switch(d_devLobby.GetNextLoop())
				{
				 case CDevLobby::SINGLEPLAYER_NEXT:
					d_topLevelLoop = SINGLEPLAYER_LOOP;
					return(UNINITIALIZED);
				 case CDevLobby::MULTIPLAYER_NEXT:
					d_topLevelLoop = SYSTEMLINK_LOOP;
					return(UNINITIALIZED);
				 default:
					ASSERTS(false, "Unexpected developer loop state");
					break;
				}
			}
		}
		else
			d_devLobby.SyncLevel();
#ifdef NETZ
		CNetZConnection::InitializeLog();
#endif
		return(INITIALIZE);

	 case INITIALIZE:
		if (d_requestExit)
			return(TERMINATE);
		if (d_devLobby.Active())
			return(DEV_LOBBY);
		if ((d_topLevelLoop == ONLINE_LOOP) || g_acceptingInvitation)
		{
#ifdef NETZ
			if (!CNetZConnection::ConnectionUp() || !CNetZConnection::GetLinkStatus()) 
			{
#if defined( _XBOX ) || defined( PS2 ) || defined( DIRECTX_PC )
				d_onlineLobby.TerminateEarly();
#endif
				return(MAIN_MENU);
			}
#endif
			return(ONLINE_LOBBY);
		}
		if (d_topLevelLoop == SYSTEMLINK_LOOP)
		{
#ifdef NETZ
			if (!CNetZConnection::ConnectionUp() || !CNetZConnection::GetLinkStatus())
				return(MAIN_MENU);
#endif
			return(LAN_LOBBY);
		}
		return(MAIN_MENU);

	 case MAIN_MENU:
		if (!d_subLoopActive)
		{
			if (d_requestExit)
				return(TERMINATE);
			if (g_acceptingInvitation)
				return(ONLINE_LOBBY);
			switch(d_mainMenu.GetNextLoop())
			{
			 case CInitialLoop::SYSTEMLINK_NEXT:
				return(LAN_LOBBY);
			 case CInitialLoop::ONLINE_NEXT:
				return(ONLINE_LOBBY);
			 case CInitialLoop::SINGLEPLAYER_NEXT:
				return(SINGLEPLAYER_LOBBY);
			 case CInitialLoop::DEVELOPER_NEXT:
				return(DEV_LOBBY);
			 default:
				d_topLevelLoop = EXIT_LOOP;
				ASSERTS(false, "Undefined initial loop state");
				return(TERMINATE);
			}
		}
		return(MAIN_MENU);

	 case CUSTOM_PLAYER:
		if (!d_subLoopActive)
		{
			if (d_requestExit)
				return(TERMINATE);
			if ((d_topLevelLoop == ONLINE_LOOP) || g_acceptingInvitation)
				return(ONLINE_LOBBY);
			if (d_topLevelLoop == SYSTEMLINK_LOOP)
				return(LAN_LOBBY);
			return(MAIN_MENU);
		}
		return(CUSTOM_PLAYER);

	 case SINGLEPLAYER_LOBBY:
		if (!d_subLoopActive)
		{
			if (d_requestExit)
				return(TERMINATE);
			if (g_acceptingInvitation)
				return(ONLINE_LOBBY);
			switch(d_singlePlayerLobby.GetNextLoop())
			{
			 case CSinglePlayerLoop::INITIAL_NEXT:
				return(MAIN_MENU);
			 case CSinglePlayerLoop::SIM_NEXT:
				d_topLevelLoop = SINGLEPLAYER_LOOP;
				break;
			 default:
				d_topLevelLoop = EXIT_LOOP;
				ASSERTS(false, "Undefined initial loop state");
				break;
			}
			return(TERMINATE);
		}
		return(SINGLEPLAYER_LOBBY);
#if defined(NETWORKING)
	 case LAN_LOBBY:
		if (!d_subLoopActive)
		{
			if (d_requestExit)
				return(TERMINATE);
			if (g_acceptingInvitation)
				return(ONLINE_LOBBY);
			switch(d_lanLobby.GetNextLoop())
			{
			 case CMultiPlayerLoop::INITIAL_NEXT:
				return(MAIN_MENU);
			 case CMultiPlayerLoop::CUSTOMPLAYER_NEXT:
				d_topLevelLoop = SYSTEMLINK_LOOP;
				return(CUSTOM_PLAYER);
			 case CMultiPlayerLoop::SIM_NEXT:
				d_topLevelLoop = SYSTEMLINK_LOOP;
				break;
			 default:
				d_topLevelLoop = EXIT_LOOP;
				ASSERTS(false, "Undefined initial loop state");
				break;
			}
			return(TERMINATE);
		}
		return(LAN_LOBBY);

	 case ONLINE_LOBBY:
#if !defined( _XBOX ) && !defined( PS2 ) && !defined( DIRECTX_PC )
		ASSERTS( false, "Online lobby not defined for PC builds" );
		g_acceptingInvitation = false;
		return(MAIN_MENU);
#else
		if (!d_subLoopActive)
		{
			ASSERT( !g_acceptingInvitation );
			if( !d_requestExit )
			{
				switch( d_onlineLobby.GetNextLoop() )
				{
				 case COnlineSetupLoop::NEXTLOOP_NETWORK_LOOP:
					d_topLevelLoop = ONLINE_LOOP;
					break;
				 case COnlineSetupLoop::NEXTLOOP_MAIN_MENU:
					d_topLevelLoop = EXIT_LOOP;
					return (MAIN_MENU);
				 case COnlineSetupLoop::NEXTLOOP_CUSTOM_PLAYER:
					d_topLevelLoop = ONLINE_LOOP;
				   return(CUSTOM_PLAYER);
				 default:
					d_topLevelLoop = EXIT_LOOP;
					ASSERTS(false, "Undefined initial loop state");
					break;
				}
			}
			return TERMINATE;
		}
#ifdef DIRECTX_PC
		if (d_requestExit)
			return(TERMINATE);
#endif
		return ONLINE_LOBBY;
#endif
#endif
	 case DEV_LOBBY:
		if (!d_subLoopActive)
		{
			if (d_requestExit)
				return(TERMINATE);
			if (g_acceptingInvitation)
				return(ONLINE_LOBBY);
			switch(d_devLobby.GetNextLoop())
			{
			 case CDevLobby::INITIAL_NEXT:
				return(MAIN_MENU);
			 case CDevLobby::SINGLEPLAYER_NEXT:
				d_topLevelLoop = SINGLEPLAYER_LOOP;
				break;
			 case CDevLobby::MULTIPLAYER_NEXT:
				d_topLevelLoop = SYSTEMLINK_LOOP;
				break;
			 default:
				d_topLevelLoop = EXIT_LOOP;
				ASSERTS(false, "Undefined developer loop state");
				break;
			}
			return(TERMINATE);
		}
		return(DEV_LOBBY);

	 case TERMINATE:
#ifdef NETZ
		CNetZConnection::TerminateLog();
#endif
		return(UNINITIALIZED);
	}

	// Let any externally defined states pass through.
	return(i_state);
}

/******************************************************************************/
/******************************************************************************/
void CSetupLoop::Initialize(void)
{
	// Set the setup world.
	g_gameEnvironment.Set(CGameEnv::GetByName("frontend"));

	ActorCreator::SetCombatantLOD( COMBATANT_LOD_HIGH );

	// Make sure this is running
	LoadScreen::Kickoff();

	// The front end should know what screens to load
	g_FrontEnd.SetGameType( FrontEnd::GAMETYPE_MAINMENU );
	
	// Do TLL Initialization
	BeginTTLInitialize();

#ifdef MASSIVE_AD
	MassiveClientInitDriver(MASSIVE_GAME_NAME);
#endif

	// Setup the weapon restrictions for this mode
	// This is setup as multiplayer because of the menus
	g_armoryView.ClearRestrictions();
	g_armoryView.SetGameMode(ArmoryView::Multiplayer);
	g_armoryView.RestrictSuperweapons();

	d_sessionMgr.Open();

	d_initSucceeded = TTLLoadEnvironment(1);
	ASSERT(d_initSucceeded);

	// Build a camera group and add it to the scene.
	CameraGroup* group = new CameraGroup("FrontEndCam");
	ASSERT_PTR(group);
	d_scenes.Scene(0)->AddGroup(group);

	// Build a camera and add it to the camera group.
	LookAroundCamera *camera = new LookAroundCamera("LookAround");
	ASSERT_PTR(camera);
	group->AddTail(camera);
	group->Select(camera);

	// Custom player camera
	GeneralCamera *customCamera = new GeneralCamera("Custom");
	ASSERT_PTR(customCamera);
	customCamera->SetMode( GeneralCamera::CAMMODE_FOCAL_POINT );
	group->AddTail(customCamera);

	// Push the group.
	d_scenes.Scene(0)->PushGroup(group);

	// Setup parameters and camera.
	d_params = CParam<SSetupLoopParams>::GetParam("Setup", "var");
	d_params->d_camera = camera;
	camera->SetMode(GeneralCamera::CAMMODE_FOCAL_DIRECTION);
	camera->SetPosition(d_params->d_cameraPosition);
	camera->SetDirection(Vector3(Math::SinDeg(d_params->d_cameraHeading),
			Math::SinDeg(d_params->d_cameraPitch),
			Math::CosDeg(d_params->d_cameraHeading)));

	// Setup debugging.
#ifndef CONSUMER_BUILD
	d_ttlDebug.Initialize(this);
#endif

	// Get the actors in there
	s_worldLoader.Process();

	//CActor *actor = g_stage.MasterCast().FindActorByInstanceName( "cam" );
	//CPathFollowerCam *pfc= (CPathFollowerCam *)((CCompActor*)actor)->GetComponentByName( "PathFollowerCam" );
	//pfc->SetAddCinemaBars( false ); // turn off cinema bars for the frontend.
	//g_messageDispatcher.SendMessageToActor( "Start", NULL, INVALID_OBJECT_HANDLE, actor->GetHandle() );

	// Reset the player profile to what the memory card says
	g_playerProfile.SetCharacter(CPlayerProfile::COP, g_GlobalProfile.GetSelectedCharacter(CPlayerProfile::COP));
	g_playerProfile.SetCharacter(CPlayerProfile::GANG, g_GlobalProfile.GetSelectedCharacter(CPlayerProfile::GANG));

	EndTTLInitialize();
}

/******************************************************************************/
/******************************************************************************/

void UpdateMemoryCard()
{
	// popup needed?
	if (g_MemCardManager->GetPopupRequest())
	{
		g_FrontEnd.ShowPopupCallBack();
		g_MemCardManager->SetPopupRequest(false);
	}

	// remove popup?
	if (g_MemCardManager->GetPopupClearRequest())
	{
//		if (g_MemCardManager->PopupResult() != MC_NONE)
		{
			g_FrontEnd.HidePopupCallBack();
			g_MemCardManager->SetPopupClearRequest(false);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CSetupLoop::Run(void)
{
	d_subLoopActive = d_subLoop[GetLoopState()]->RunLoop();

#ifdef DIRECTX_PC
	RunFramePC();
#endif // DIRECTX_PC

	BeginTTLRun();

#ifndef CONSUMER_BUILD
	d_ttlDebug.InterceptControls();
#endif

	UpdateMemoryCard();
	EndTTLRun();

#ifndef CONSUMER_BUILD
	d_ttlDebug.Update();
#endif

	TTLRender();
}


#ifdef DIRECTX_PC
#include "../../../Avalanche/GameLib/platform/keyctrl.h"
/******************************************************************************/
/******************************************************************************/
void CSetupLoop::RunFramePC( void )
{
	return; // we handle all of this stuff in the screen code now
			// . . . not that that's a super good place to do it
			// from a design standpoint, but because it allows
			// us to rebind keys when we're in the game, and not
			// just before a game has started
}
#endif // DIRECTX_PC


/******************************************************************************/
/******************************************************************************/
void CSetupLoop::Terminate(void)
{
#ifndef CONSUMER_BUILD
	d_ttlDebug.Terminate();
#endif

	CameraGroup* group = d_scenes.Scene(0)->FindGroup("FrontEndCam");
	CameraGroup* anotherGroup = d_scenes.Scene(0)->PopGroup("FrontEndCam");
	ASSERT(group == anotherGroup);
	d_scenes.Scene(0)->RemoveGroup(group);

	TTLUnloadEnvironment();
	TTLTerminate();
	d_sessionMgr.Close();
	TTLReset();
}

/******************************************************************************/
/******************************************************************************/
void CParam<SSetupLoopParams>::RegisterVariables(void)
{
	d_data.d_camera = NULL;
	RegisterVariable((Vector3Packed*)&d_data.d_cameraPosition, "CameraPosition",
			0.0f, 5.0f, 0.0f, -100.0f, 0.0f, -100.0f, 100.0f, 15.0f, 100.0f,
			(void*)&d_data);
	RegisterVariable(d_data.d_cameraHeading, "CameraHeading", 0.0f, -180.0f,
			180.0f, (void*)&d_data);
	RegisterVariable(d_data.d_cameraPitch, "CameraPitch", 0.0f, -90.0f, 90.0f,
			(void*)&d_data);
}

/******************************************************************************/
/******************************************************************************/
void CParam<SSetupLoopParams>::CParamMessageHandler::HandleMessage(
		void* i_data, ActorHandle i_sender, bool i_posted)
{
	SSetupLoopParams* params = (SSetupLoopParams*)i_data;
	if ((params != NULL) && (params->d_camera != NULL))
	{
		params->d_camera->SetPosition(params->d_cameraPosition);
		params->d_camera->SetDirection(Vector3(
				Math::SinDeg(params->d_cameraHeading),
				Math::SinDeg(params->d_cameraPitch),
				Math::CosDeg(params->d_cameraHeading)));
	}
}
