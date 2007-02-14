
/*
**
**  File:   TTLLoop.h
**  Date:   April 8, 2004
**  By:     Bryant Collard
**  Desc:   A general TTL game loop.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/TTLLoop.cpp $
**  $Revision: #140 $
**  $DateTime: 2005/08/31 13:08:48 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header
#include "main/TTLLoop.h"

// Non-precompiled headers.
#include "EngineHelper/LoadScreen.h"
#include "Components/weapons/weaplib.h"
#include "frontend/frontend.h"
#include "platform/defcontrolmap.h"
#include "environ/LevelScript.h"
#include "Navigate/NavManager.h"
#include "environ/PlayerSpawnMgr.h"
#include "environ/ObjectiveMgr.h"
#include "Game/Managers/ParticleMgr.h"
#include "stage/SurfaceLookup.h"
#include "components/PlantParam.h"
#include "Components/RigidBodyTable.h"
#include "Effects/ahfmgr.h"
#include "Effects/Water/WaterMgr.h"
#include "Effects/worldspacedistort.h"
#include "weapons/penetrate.h"
#include "weapons/debugshot.h"
#include "main/PlayerDB.h"
#include "effects/pain_decal.h"
#include "environ/ConfigMgr.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "frontend/savegame.h"
#include "Layers/loadprofile.h"
#include "main/ErrorMonitor.h"
#include "weapons/seethru.h"
#include "Weapons/armoryview.h"
#include "effects/customtag.h"
#include "Layers/SpotProfile.h"

#include "avatar/sobj.h"

#ifdef DIRECTX_PC
#include "hud/screenmanager.h"
#include "Audio/Win32/EaxHelper.h"
#endif

bool CTTLLoop::s_noMusic = false;

CPlayers CTTLLoop::s_players;
WorldActorLoader CTTLLoop::s_worldLoader;

// Initialize function array.
SLoopEntry CTTLLoop::stateArray[CTTLLoop::NUM_STATES] =
{
	"INITIALIZE", (LoopFunc)&CTTLLoop::Initialize,
	"RUN",        (LoopFunc)&CTTLLoop::Run,
	"TERMINATE",  (LoopFunc)&CTTLLoop::Terminate
};

/***************************************************************************/
/***************************************************************************/
CTTLLoop::CTTLLoop() : CActorLoop(stateArray, UNINITIALIZED)
{
	d_levelScript = NULL;
}

/***************************************************************************/
/***************************************************************************/
CTTLLoop::CTTLLoop(SLoopEntry* i_stateArray, int i_loopState) :
	CActorLoop(i_stateArray, i_loopState),
	d_messageOwner("TTLLoop")
{
	d_levelScript = NULL;
}

/***************************************************************************/
/***************************************************************************/
CTTLLoop::~CTTLLoop()
{
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::BeginTTLInitialize(void)
{
	SpotProfile p;

	// Load the weapons database
	p.Start("WeaponLibrary::Load");
	WeaponLibrary::Load();

	// Reset the cheat state
	p.Start("Cheats::Reset");
	Cheats::Reset();

	// Do basic initialization.
	p.Start("BeginActorInitialize");
	BeginActorInitialize();

	// Load up the front end screens.
	p.Start("g_FrontEnd.Init");
	g_FrontEnd.Init();

	// preload tag fonts
	p.Start("CustomTag::PreloadFonts");
	CustomTag::PreloadFonts();

	//// Create the hud
	//GameHUD::Create();

	// Fire up the error monitor.
	p.Start("g_errorMonitor.Initialize");
	g_errorMonitor.Initialize();

#ifdef DIRECTX_PC
	Sprite *spritePtr = new Sprite;
	spritePtr->Create(dblLoad("Screens\\38-snub-nose.DBL"), 0 );
	spritePtr->SetColor(1.0f, 0.0f, 0.0f, 0.99f);
	g_MenuCursor.Initialize(spritePtr, g_inputPtr);
	g_MenuCursor.SetMaxXandY((float)DisplayState.nScreenWidth,(float)DisplayState.nScreenHeight);
	//g_MenuCursor.SetHidden(true);
#endif

	// load up the xml, and create options
	p.Start("g_configmgr.Initialize");
	g_configmgr.Initialize();

	// Set default timer speed.
	SetTimerSpeed(CMasterTimer::ONE_X);

	// Register message handlers.
	d_pauseHandler.Initialize("Pause", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_timeSpeedHandler.Initialize("TimeSpeed", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_timerReset = 0;

	// Initialze the widgets
	p.Start("WidgetFactory::CreateSingleton");
	WidgetFactory::CreateSingleton();

	// Initialize the players.
	p.Start("g_players.Initialize");
	SetSingleton(CPlayers, &s_players);
	g_players.Initialize();

	// Get a default weapon loadout, this may change in the lobby or because
	// of level script.

	// god made me hack...
	p.Start("loadouts");
	int icop = PlayerDB::CharacterId( g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::COP ) ) & ~PLAYERDB_CUSTOM;
	int igang = PlayerDB::CharacterId( g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::GANG ) ) & ~PLAYERDB_CUSTOM;

	const char* pCopWeapons = g_GlobalProfile.GetCustomLoadout( icop );
	const char* pGangWeapons = g_GlobalProfile.GetCustomLoadout( igang );

	if( !pCopWeapons )
		pCopWeapons = ArmoryView::GetDefaultLoadout(ArmoryView::HumanCop, 0);

	if( !pGangWeapons )
		pGangWeapons = ArmoryView::GetDefaultLoadout(ArmoryView::HumanGangster, 0);;

	g_playerProfile.SetWeaponLoadout( CPlayerProfile::COP, pCopWeapons );
	g_playerProfile.SetWeaponLoadout( CPlayerProfile::GANG, pGangWeapons );

	p.Stop();
	p.Dump();
}

/***************************************************************************/
/***************************************************************************/
bool CTTLLoop::TTLLoadEnvironment(int i_numPlayers)
{
	// load pain textures
	CPainDecal::LoadPainTextures("pain\\pain_tx.dbl");

	// Specialty system
	NewSingleton(AvatarFX);

	// Queue the music please
	g_audio.jukebox.SetMode(Jukebox::JUKEBOX_3DPOSITIONAL);
	g_audio.jukebox.SetMin3DVolume(0.2f);
	g_audio.jukebox.ClearPlayPositions();
	g_audio.jukebox.AutoPauseFrames(4);

	// Spawn manager
	NewSingleton( PlayerSpawnMgr );

	// Objective manager
	NewSingleton(CObjectiveMgr);

	// Load the environment.
	if (!ActorLoadEnvironment(i_numPlayers))
		return(false);

	// Load the world info.
	s_worldLoader.Load(d_environment);

	// Load surface tables.
	g_surfaceLookup.AddDataTable(&g_collisionParticle);
	g_surfaceLookup.AddDataTable(&g_rigidBodyTable);
	g_surfaceLookup.AddDataTable(&g_penetrationTable);
	g_penetrationTable.LoadWatch();
	g_collisionParticle.LoadWatch();
	g_rigidBodyTable.LoadWatch();

	// weapons
	WeaponLibrary::PrepareForCombat();

	// Full screen distorts
	NewSingleton(CFSDSystem);

	// Load the players start positions.
	g_players.GetPlayerStartPos(d_environment->WorldPath("player", NULL));

	// Load the paths
	gbl_paths = (ts_SplinePathTable*)dblGetChunkData(DB_PATHS,
			DEFAULT_CHUNK_KEY_STRING, g_loadSOM.GetWorldHandle());
	PathCollection::Init();
	PathCollection::Collection()->AddAutoGroupedPaths(gbl_paths,
			PathCollection::Spline);

	// Load emitters
	ts_RecordList* emitters = (ts_RecordList*)dblGetChunkData(DB_WORLD_EMITTERS,
			DEFAULT_CHUNK_KEY_STRING, g_loadSOM.GetWorldHandle(), NULL);
	if (emitters != NULL)
	{
		u16 group = dblGetEmitterGroupID(g_loadSOM.GetWorldHandle());
		for (int i = 0; i < emitters->usRecordCount; i++)
		{
			ts_WorldEmitter* emitter = (ts_WorldEmitter*) GetRecordAddress(emitters, i);
			if (emitter->u16Type == 0)
			{
				t_Handle handle = ParticleEmitter__CreateFromRecord(emitter->Name, &emitter->vPos,
											NULL, NULL, FALSE, NULL, TRUE, group);

				// optional path name immediate follows emitter name in memory.
				const char* pathName = &emitter->Name[strlen(emitter->Name) + 1];
				if (pathName[0])
				{
					SplinePath* path = PathCollection::Collection()->Named(pathName);
					if ( path )
						ParticleEmitter__SetParticlePath(handle, path);
					else
						ASSERTF(false, ( "world emitter '%s' can't find path '%s'", emitter->Name, pathName ));
				}

				// emitter LOD
				ParticleEmitter__SetLOD(handle, emitter->fLOD1RadiusOverride, emitter->fLOD2RadiusOverride);
			}
		}
	}

	// Setup the navigation manager.
	NewSingleton(Navigate::Manager);
	gbl_NavManager.Initialize(256);

	// Start the level script.
	d_levelScript = CLevelScript::Load( d_environment->WorldName() );
	if ( d_levelScript && d_levelScript->Script()->HasFunction( "InitCharacter" ) )
		d_levelScript->Script()->CallFunction( "InitCharacter" );

	// Water
	memSetUseBit( MEM_WATER_DATA );
	g_aniHFMgr.CreateAll(d_environment->WorldPath());
	g_waterMgr.CreateAll(d_environment->WorldPath());
	memClearUseBit( MEM_WATER_DATA );

	// Blood! - needs surface table
	NewSingleton(Blood);
	g_blood.Load("blood\\blood.dbl");
	g_blood.Enable(g_GlobalProfile.GetBlood());

	// Distortions
	WorldSpaceDistort::Init();

	// secondary object tracker singleton
	NewSingleton(SecondaryObjective);

	// Wee!
	static bool showPermanent = true;
	LoadProfile::Done();
	LoadProfile::Report(d_environment->WorldName(), showPermanent);
#ifdef PS2
	LoadProfile::Summary(d_environment->WorldName(), showPermanent);
#endif
	showPermanent = false;

	return(true);
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::EndTTLInitialize(void)
{
	// Finish actor inititialization.
	EndActorInitialize();

	// Do the materials now!
	SeeThru::SetupMaterials();

	// Setup the proper HUD
	GameHUD::Create();
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		g_hud.CreateMPPanel();
	else
		g_hud.CreateSPPanel();

	// Set this up
	g_avatarFX.Setup();

#if defined(LOADSCREENDEBUG)
	for (int i = 0; i < 60; i++)
		Sleep(1000);
#endif

	// Make sure the load screen goes away
	LoadScreen::Kill();

	// Queue the music please
	if (!s_noMusic)
		g_audio.jukebox.QueueFromList(g_gameEnvironment.GetMusicList());
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::BeginTTLRun(void)
{
	BeginActorRun();
	g_avatarFX.StartFrame();
	gbl_NavManager.Update(g_timer.GetFrameSec());
	CPlantParam::Update();

	if (d_timerReset != 0 && g_timer.GetEndMS() >= d_timerReset)
	{
		SetTimerSpeed(CMasterTimer::ONE_X);
		d_timerReset = 0;
	}

	// Update the cheats
	Cheats::Daemon();

#ifdef DIRECTX_PC
	// update sound pos info
	Vector3 playerPos;
	Vector3 playerFwd;
	Vector3 playerUp;
	if (g_players.GetPlayer(0))
	{
        playerPos = g_players.GetPlayer(0)->d_bodyInWorld;
		playerFwd = g_players.GetPlayer(0)->d_bodyToWorld.GetRow2();
		if (EAX::DS3DListenerPtr)
		{
			EAX::DS3DListenerPtr->SetPosition(playerPos.X(),playerPos.Y(),playerPos.Z(),DS3D_DEFERRED);
			EAX::DS3DListenerPtr->SetOrientation(-playerFwd.X(), -playerFwd.Y(),	-playerFwd.Z(),
												 0.0f,			1.0f,			0.0f,			DS3D_IMMEDIATE);
		}
	}
#endif
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::EndTTLRun(void)
{
	if (d_levelScript)
		d_levelScript->Run();
	g_widgetFactory.Update(g_cameraDirector.GetCameraMatrix());
	EndActorRun();
	WorldSpaceDistort::UpdateAll(&g_cameraDirector.GetCameraMatrix());
	g_avatarFX.EndFrame();
	d_messageOwner.HandlePostedMessages();
	DebugShot::Draw();
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::TTLRender(void)
{
	if (ValidSingleton(GameHUD))
		g_hud.Update();
	if (ValidSingleton(CFSDSystem))
		g_fsdSystem.Update();

	g_errorMonitor.Update();
	g_FrontEnd.Update();
	ActorRender();
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::TTLUnloadEnvironment(void)
{
	// Make sure we get rid of these...
	g_audio.jukebox.ClearPlayPositions();

	// Weapons
	WeaponLibrary::DoneCombat();

	// Distortions
	WorldSpaceDistort::Shutdown();
	DeleteSingleton(CFSDSystem);

	// Clean up the blood
	DeleteSingleton(Blood);

	if ( ValidSingleton( Navigate::Manager ) )
	{
		gbl_NavManager.ShutDown();
		DeleteSingleton(Navigate::Manager);
	}
	PathCollection::Shutdown();
	g_rigidBodyTable.UnloadWatch();
	g_collisionParticle.UnloadWatch();
	g_penetrationTable.UnloadWatch();

	// Wee!
	g_avatarFX.Reset();
	DeleteSingleton(AvatarFX);

	// unload pain
	CPainDecal::UnloadPainTextures();

	// Clean up the hud
	GameHUD::Destroy();

	// Objective manager
	DeleteSingleton(CObjectiveMgr);

	ActorUnloadEnvironment();

	s_worldLoader.Terminate();

	// spawn manager
	DeleteSingleton( PlayerSpawnMgr );

	DeleteSingleton( SecondaryObjective );

	SceneMgr::Reset();

	// Clean up scripts
	delete d_levelScript;
	d_levelScript = NULL;
	g_ScriptSystem.PurgeUnusedExecutables();

	//// DEBUG DEBUG DEBUG
	//{
	//	extern void memDumpBlockInfo( const char* filename, bool append );
	//	static int count = 0;
	//	memDumpBlockInfo( "dump2", count++ > 0 );
	//}
	//// DEBUG DEBUG DEBUG
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::TTLTerminate(void)
{
	// Cleanup the players.
	if ( ValidSingleton( CPlayers ) )
	{
		g_players.Uninitialize();
		ClearSingleton(CPlayers);
	}

	// Cleanup widgets.
	if ( g_widgetFactoryPtr )
	{
		g_widgetFactory.Terminate();
		WidgetFactory::DestroySingleton();
	}

	// Unload the weapons database
	WeaponLibrary::Unload();

	// Flush the draw list.
	Sprite::DrawCancel();

	// Clean up config manager.
	g_configmgr.Uninitialize();

	// Free up message handlers.
	d_messageOwner.UnregisterAll();

	// Terminate the actor loop.
	ActorTerminate();
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::TTLReset(void)
{
	// Clean up the error monitor.
	g_errorMonitor.Terminate();

	// Shutdown the front end.
	g_FrontEnd.Shutdown();

	// Free system font.
	// Font::FreeSystemFont();

	ActorReset();
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::SetTimerSpeed(CMasterTimer::ETimerSpeed i_speed, bool smooth)
{
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		smooth = false;
	g_timer.SetSpeed(i_speed, smooth);

	g_messageDispatcher.SendMessageToAll("SyncTimer", NULL,
			INVALID_OBJECT_HANDLE);
	if (g_timer.IsPaused())
		Animator::SuspendAll(0);
	else
		Animator::SuspendAll(-1);
}

/***************************************************************************/
/***************************************************************************/
bool CTTLLoop::CPause::PostData(void *i_data, CPostedMessage &i_postedMessage)
{
	SPause* pause = (SPause*)i_data;
	if ((pause->d_menu == NULL) || (pause->d_menu[0] == '\0'))
	{
		SPause* postedPause =
				(SPause*)i_postedMessage.AllocateData(sizeof(SPause));
		*postedPause = *pause;
		postedPause->d_menu = NULL;
	}
	else
	{
		// Layout the structure of the saved data.
		struct SPostedPause
		{
			SPause d_pause;
			char d_menu;
		};

		// Allocate some space. The actual memory for the string will follow the
		// main structure. Note that SPostedPause contains one character for the
		// string so the total memory available for the string is the length of
		// the string plus 1.
		SPostedPause* postedPause = (SPostedPause*)i_postedMessage.AllocateData
				(sizeof(SPause) + strlen(pause->d_menu) + 1);

		// Fill in the data.
		postedPause->d_pause = *pause;
		postedPause->d_pause.d_menu = &(postedPause->d_menu);
		strcpy(postedPause->d_pause.d_menu, pause->d_menu);
	}
	return(true);
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::CPause::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	SPause* pauseData = (SPause*)i_data;

	bool pause;
	switch(pauseData->d_type)
	{
	 case SPause::PAUSE:
		if (g_timer.IsPaused())
			return;
		pause = true;
		break;
	 case SPause::UNPAUSE:
		if (!g_timer.IsPaused())
			return;
		pause = false;
		break;
	 case SPause::TOGGLE:
	 default:
		pause = !g_timer.IsPaused();
		break;
	}

	if (pause)
	{
		d_ref->SetTimerSpeed(CMasterTimer::PAUSE);
		if (pauseData->d_affectAudio)
		{
			g_audio.Pause();
			Sfx::GetEnvironmentEmitter().Pause();
		}
		g_players.ClearVibration();
		d_ref->SuspendControl();
		if ((pauseData->d_menu != NULL) && (pauseData->d_menu[0] != '\0'))
			g_FrontEnd.GotoScreen(pauseData->d_menu);
	}
	else
	{
		d_ref->SetTimerSpeed(CMasterTimer::ONE_X);
		g_audio.Unpause();
		Sfx::GetEnvironmentEmitter().Unpause();
#ifdef DIRECTX_PC
			g_players.ActivateControls(true);
#else
			g_players.ActivateControls(false);
#endif
		d_ref->RestoreControl(); // restore control to exactly the state it was in when we paused
	}
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::CTimeSpeed::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	bool networked = (ValidSingleton(CSession) && g_session.IsNetworked());
	if (!networked)
	{
		CMasterTimer::ETimerSpeed speed = (CMasterTimer::ETimerSpeed)(CMessageDispatcher::DataToInt(i_data) & 0xffff);
		uint32 duration = (CMessageDispatcher::DataToInt(i_data) >> 16) & 0x7fff;
		bool smooth = (CMessageDispatcher::DataToInt(i_data) & 0x80000000) != 0;
		if (duration != 0)
			d_ref->d_timerReset = g_timer.GetEndMS() + duration;
		else
			d_ref->d_timerReset = 0;
		d_ref->SetTimerSpeed(speed, smooth);
	}
}

void CTTLLoop::SuspendControl( void )
{
	for( int i = 0; i < MAX_PLAYERS; i++ )
	{
		controls[i] = g_players.GetCurrentControlState( i );
	}
	g_players.ActivateControls( false );
}

void CTTLLoop::RestoreControl( void )
{
	for( int i = 0; i < MAX_PLAYERS; i++ )
	{
		g_players.RestoreControlState( i, controls[i] );
	}
}

/***************************************************************************/
/***************************************************************************/
int CTTLLoop::DetermineState(int i_state)
{
	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
		return(INITIALIZE);
	 case INITIALIZE:
		if (!d_initSucceeded)
			return(UNINITIALIZED);
		return(RUN);
	 case RUN:
		if (d_requestExit)
			return(TERMINATE);
		return(RUN);
	 case TERMINATE:
		break;
	 default:
		// Let any externally defined states pass through.
		return(i_state);
	}
	return(UNINITIALIZED);
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::Initialize(void)
{
	BeginTTLInitialize();

	d_initSucceeded = TTLLoadEnvironment(1);
	ASSERT(d_initSucceeded);

	g_players.CreateFromStrings(&d_scenes, INVALID_PARTICIPANT,
		PlayerDB::CharacterInitString(g_playerProfile.GetCharacter(CPlayerProfile::COP)),
		g_playerProfile.GetWeaponLoadout(CPlayerProfile::COP));
	g_players.ActivateControls(true);

	EndTTLInitialize();
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::Run(void)
{
	BeginTTLRun();
	EndTTLRun();
	TTLRender();
}

/***************************************************************************/
/***************************************************************************/
void CTTLLoop::Terminate(void)
{
	g_players.Destroy();
	TTLUnloadEnvironment();
	TTLTerminate();
}
