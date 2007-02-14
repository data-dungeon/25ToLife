/*
**
**  File:   TTLDebug.h
**  Date:   March 25, 2004
**  By:     Bryant Collard
**  Desc:   Debugging stuff from simloop.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/TTLDebug.cpp $
**  $Revision: #195 $
**  $DateTime: 2005/08/25 17:27:19 $
**    $Author: Rob $
**
*/

#include "TTLPCH.h"

#include "collide/LineToCapsule.h"

#include "main/TTLDebug.h"
#include "main/TTL.h"
#include "sim/hud.h"

#include "Loop/LoopScenes.h"
#include "platform/ctroller.h"
#include "platform/ctrolmap.h"
#include "platform/defcontrolmap.h"
#include "CollideHelper/AabbEntity.h"
#include "Game/Database/SceneNode.h"
#include "Display/TextureUtil.h"
#include "cmdcon/vcontext.h"
#include "components/KineControl/BipedKineControl.h"
#include "frontend/frontend.h"
#include "frontend/screens/screen_Popups.h"
#include "main/PlayerDB.h"
#include "Environ/Munition.h"
#include "avatar/pstat.h"
#include "cutscene/CutSceneMgr.h"
#include "EngineHelper/Singleton.h"
#include "weapons/armory.h"
#include "components/painstats.h"
#include "Effects/shatter.h"
#include "EngineHelper/streamtest.h"
#include "GameHelper/ErrorStack.h"
#include "Game/Managers/ParticulateMgr/ParticulateMgr.h"
#include "GameHelper/GameLocks.h"
#include "main/EmailOutput.h"
#include "hud/imatspy.h"
#include "environ/PlayerSpawnMgr.h"
#include "Layers/Breakpoint/HardwareBreak.h"
#include "Environ/ActorCreator.h"

#ifndef CONSUMER_BUILD
#include "Layers/IMM/IMM.h"
#endif

/* debug menu functions */

enum
{
	MENUCMD_TOGGLEHIDEACTORS,
	MENUCMD_PRINTSURFACEDATA,
	MENUCMD_NEXTCAMERATARGET,
	MENUCMD_NEXTCAMERAMODEP1,
	MENUCMD_NEXTCAMERAMODEP2,
	MENUCMD_COLLISIONDEBUG,
	MENUCMD_SURFACEDEBUG,
	MENUCMD_RESETPLAYERS,
	MENUCMD_KILL,
	MENUCMD_SMACK,
	MENUCMD_HEAL,
	MENUCMD_ENDLEVEL,
	MENUCMD_NIGHTVISION,
	MENUCMD_DOF,
	MENUCMD_AIMDOWNCAMERA,
	MENUCMD_AIMTRIANGULATE,
	MENUCMD_SHATTERTEST,
	MENUCMD_AFFLICTDEAF,
	MENUCMD_AFFLICTBLIND,
	MENUCMD_AFFLICTSTUN,
	MENUCMD_AFFLICTFIRE,
	MENUCMD_AFFLICTSMOKE,
	MENUCMD_AFFLICTTEARGAS,
	MENUCMD_DECAPITATE,
	MENUCMD_YARDSALE,
	MENUCMD_PAINHEAD,
	MENUCMD_PAINCHEST,
	MENUCMD_PAINBACK,
	MENUCMD_PAINLSHOULDER,
	MENUCMD_PAINRSHOULDER,
	MENUCMD_PAINLLEG,
	MENUCMD_PAINRLEG,
	MENUCMD_NOPAIN,
	MENUCMD_SETERROR,
	MENUCMD_TOGGLEERROR,
	MENUCMD_ENDROUND,
	MENUCMD_TOGGLEDEADRECKONDRAW,
	MENUCMD_TOGGLEDEADRECKONDEBUG,
	MENUCMD_TOGGLESPAWNPOINTS,
	MENUCMD_TOGGLEDEBUGCLOCK,
	MENUCMD_TOGGLESTATS,
	MENUCMD_TOGGLEMAXIMIZE,
	MENUCMD_TOGGLEFBUFCLEAR,
	MENUCMD_TOGGLESHADOWDEBUG,
	MENUCMD_TOGGLEPLANTS,
	MENUCMD_TOGGLEWIREWATER,
	MENUCMD_TOGGLEVISIBILITY,	// rhn
	MENUCMD_TOGGLEPROFILING,
	MENUCMD_PRINTMESSAGEDEBUG,
	MENUCMD_TOGGLEMESSAGELOG,
	MENUCMD_TOGGLEOUTPUTLOG,
	MENUCMD_TOGGLESESSIONTIMELOG,
	MENUCMD_TOGGLEHEAPINFO,
	MENUCMD_TOGGLEINSTANCESTATS,
	MENUCMD_TOGGLEHEAPHIGHWATER,
	MENUCMD_TOGGLEMEMORYREPORT,
	MENUCMD_STATSTOCONSOLE,
	MENUCMD_TOGGLEPARTICLES,
	MENUCMD_TOGGLEPARTICULATES,
	MENUCMD_WATERDEBUG,
	MENUCMD_MUSICVOLUMEUP,
	MENUCMD_MUSICVOLUMEDOWN,
	MENUCMD_SAVESTARTPOS,
	MENUCMD_DOYOUROWNTHING,
	MENUCMD_ANIMTEST,
	MENUCMD_DUMPCOLLISIONSTACK,
	MENUCMD_PLOTCOLLISIONSTACK,
	MENUCMD_TOGGLEFPS,
	MENUCMD_TOGGLECOORDINATES,
	MENUCMD_TOGGLEHUD,
	MENUCMD_TOGGLETESTHUD,
	MENUCMD_TOGGLEPOINTLIGHTS,
	MENUCMD_NETWORK_DATA,
	MENUCMD_TOGGLEAISTATS,
	MENUCMD_DISABLEWORLDACTORS,
	MENUCMD_PLAYCULDESAC,
	MENUCMD_NEXTLOADOUT,
	MENUCMD_RANDOMLOADOUT,
	MENUCMD_FOODLOADOUT,
	MENUCMD_ROBLOADOUT,
	MENUCMD_TWONGLOADOUT,
	MENUCMD_MAJORLOADOUT,
	MENUCMD_CLETUSLOADOUT,
	MENUCMD_BIZNALOADOUT,
	MENUCMD_TRAVISLOADOUT,
	MENUCMD_DWAINLOADOUT,
	MENUCMD_VINCELOADOUT,
	MENUCMD_BRYCELOADOUT,
	MENUCMD_FULLAMMO,
	MENUCMD_BLOODON,
	MENUCMD_BLOODOFF,
	MENUCMD_JUKEBOXADDSOURCE,
	MENUCMD_STREAMTESTADD,
	MENUCMD_STREAMTESTREMOVE,
	MENUCMD_STREAMTESTCLEAR,
	MENUCMD_DUMPINSTANCELIST,
	MENUCMD_DUMPAGENTLIST,
	MENUCMD_TOGGLECOLLISIONSTATS,
	#ifndef CONSUMER_BUILD
	MENUCMD_TOGGLEMAYAINTERFACE,
	MENUCMD_AISPY,
	MENUCMD_MATERIALSPY,
	MENUCMD_ACTORSPY,
	MENUCMD_DEBUGAIMSIM,
	#endif
	MENUCMD_UNLOCKALL,
	MENUCMD_CHARACTERLOD_LOW,
	MENUCMD_CHARACTERLOD_MID,
	MENUCMD_CHARACTERLOD_HIGH,
	MENUCMD_FLUSHBUNDLES,
	MENUCMD_TOGGLEBUNDLING,
	MENUCMD_MARKTRACE,
	MENUCMD_TOGGLECONNSTAB,

	// Below are things that need ranges of values
	MENUCMD_EMAILBASE	 = 1100,
};

float CTTLDebug::s_consoleTimerSpeed = CMasterTimer::ONE_X;
bool CTTLDebug::s_consoleQuit = false;

#ifndef CONSUMER_BUILD
extern bool g_aiSpy;
extern bool g_debugAimSim;
#endif

GameHud* CTTLDebug::s_hud = NULL;
bool CTTLDebug::s_showMemoryReport = false;

#if defined (PS2) || defined (DIRECTX)
#define SNAP_ON_L1_START		// Snap screen shot without freezing on (L1 & START)
#endif

#define COMMAND_DEBUG_CATEGORY_BASE 200
#define COMMAND_DEBUG_DONE          COMMAND_DEBUG_CATEGORY_BASE - 1
#define COMMAND_DEBUG_SHOW          COMMAND_DEBUG_CATEGORY_BASE - 2
#define COMMAND_DEBUG_SELECT_ALL    COMMAND_DEBUG_CATEGORY_BASE - 3
#define COMMAND_DEBUG_DESELECT_ALL  COMMAND_DEBUG_CATEGORY_BASE - 4

/***************************************************************************/
/***************************************************************************/
void WriteDebugMessage(int line, const char *Message)
{
#ifndef CONSUMER_BUILD
	if (CTTLDebug::s_hud)
		CTTLDebug::s_hud->m_Debug.WriteDebugMessage(line, Message);
#endif
}

/***************************************************************************/
/***************************************************************************/
CTTLDebug::CTTLDebug() : d_messageOwner("Debug")
{
#ifndef CONSUMER_BUILD
	d_menuMapper = NULL;
#endif
}

/***************************************************************************/
/***************************************************************************/
CTTLDebug::~CTTLDebug()
{
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::Initialize(CTTLLoop* i_ttlLoop)
{
#ifndef CONSUMER_BUILD
	// Save our TTL loop.
	d_ttlLoop = i_ttlLoop;

	// Create a hud
	ASSERT(s_hud == NULL);
	s_hud = new GameHud;

	// Initialize variables.
	d_plotCollisionStack = false;
	d_profiling = false;
	d_surfaceDebug = false;
	d_collisionDebug = 0;
	d_collisionFrozen = false;

	// Initialize the debug hud
	memSetUseBit(MEM_HUD_DATA);
	s_hud->Initialize();
	memClearUseBit(MEM_HUD_DATA);

	// Setup the debug menu.
	SetupDebugMenu();

	// Setup a control mapper for the debug menu.
	d_menuMapper = new DefaultControlMapper("DebugMenu");
	ASSERT(d_menuMapper);
	g_controlFocus.Add(d_menuMapper, 0);

	// Setup debug camera groups and control mappers
	d_ttlLoop->GetScenes().CreateFreeCameras();

	// Prep for changing the timer speed from the command console.
	d_syncTimerHandler.Initialize("SyncTimer", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_syncTimerHandler.HandleMessage(NULL, INVALID_OBJECT_HANDLE, false);

	// Configure debug output
	d_debugOutputPopup = NULL;
	d_dialogCommandHandler.Initialize("DialogCommand", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_dialogCancelHandler.Initialize("DialogCancel", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);

	// Initialize the quit flag.
	s_consoleQuit = false;

	// Clear the selected actor.
	d_select = INVALID_OBJECT_HANDLE;

	// Clear the error handle
	d_errorHandle = ERROR_HANDLE_INVALID;

	d_loadout = 0;
	d_spawnPointDebug = false;

#ifndef CDROM
	// Create console variables.
	g_console.CreateToggle(NULL, "quit", s_consoleQuit);
	g_console.CreateVar(NULL, "speed", s_consoleTimerSpeed);
#endif

	dbgPrint("\nEnd of CTTLloop::Init() for World %s -  Level Starting  -  "
			"Memory Status follows:\n", g_gameEnvironment.WorldName());
	Layers__BriefShowMemoryState();

#if !defined(CDROM) && defined(MEM_DEBUG_BLOCKS)
	if (s_showMemoryReport)
		ShowMemoryReport();
#endif

#endif // CONSUMER_BUILD
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::Update(void)
{
#ifndef CONSUMER_BUILD

#ifdef DIRECTX_PC
#ifndef _DEBUG
	return;
#endif //_DEBUG
#endif //DIRECTX_PC

#ifdef DEBUGLIGHTING
	g_lightSet.drawLightSet( );
#endif

	// Handle timer updates
	HandleTimerChanges();

	// Handle any input
	if (!HandleDebugMenu())
		HandleInput();

	// Spawn points (clears the line buffer in CDROM so make it first to avoid
	// clearing more lines than necessary).
	if (d_spawnPointDebug && ValidSingleton(PlayerSpawnMgr))
		g_PlayerSpawnMgr.Draw(g_cameraDirector.CurrentCamera().Eye());

	// Update the Hud
	s_hud->Update();

	// reset all the stats
	SceneMgr::GetStats().Clear();

	// Debug collisions
	CollisionDebug();
	OctTreeLineIterator::Draw();

	// debug -- death cam sphere check
	//DrawUtility::Sphere( g_players.GetPlayer( 0 )->GetBodyInWorld() + Vector3( 0 , 2.6f , 0 ) , DrawUtility::RED , 1.5f );

	// Debug paths
	if (PathCollection::Collection())
		PathCollection::Collection()->Draw();

	// Draw Queued debug primitives
	DrawUtility::DrawQueue();

	if (d_plotCollisionStack)
	{
		PLOT_STACK(g_collisionSystem.GetStack());
	}

#ifdef ALLOW_DEBUG_MENU
	// Draw the Debug Menu last
	s_hud->m_Debug.m_Menu.DrawMenu();
#endif //ALLOW_DEBUG_MENU

	if (s_consoleQuit)
		g_messageDispatcher.SendMessageToAll("Quit", NULL, INVALID_OBJECT_HANDLE);

#endif // CONSUMER_BUILD
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::Terminate(void)
{
#ifndef CONSUMER_BUILD
	StreamTest::RemoveAll();
	d_ttlLoop->GetScenes().DeleteFreeCameras();
	g_controlFocus.Remove(d_menuMapper);
	d_menuMapper = NULL;
	DrawUtility::ClearQueue();
	d_messageOwner.UnregisterAll();
	delete s_hud;
	s_hud = NULL;
#endif // CONSUMER_BUILD
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::SetupDebugMenu(void)
{

// --- all this shite falls out on CONSUMER_BUILD
#ifndef CONSUMER_BUILD

	// Add some functions to the DebugMenu
	DebugMenu *pMenu = GetDebugMenu();

	if( pMenu )
	{

		// Stuff
		pMenu->AddMenuItem( "Toggle Maya Connection",  MENUCMD_TOGGLEMAYAINTERFACE,  ProcessMenuCommand,  this );
		pMenu->AddMenuItem( "AI Spy",  MENUCMD_AISPY,  ProcessMenuCommand,  this );
		pMenu->AddMenuItem( "Surface Spy",  MENUCMD_MATERIALSPY,  ProcessMenuCommand,  this );
		pMenu->AddMenuItem( "Actor Spy",  MENUCMD_ACTORSPY,  ProcessMenuCommand,  this );
		pMenu->AddMenuItem( "Kill", MENUCMD_KILL, ProcessMenuCommand, this );

		pMenu->AddMenuItem( "Cheat" );
		pMenu->Branch();
		pMenu->AddMenuItem( "Reset players", MENUCMD_RESETPLAYERS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Smack", MENUCMD_SMACK, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Heal", MENUCMD_HEAL, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "End level", MENUCMD_ENDLEVEL, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Custom" );
		pMenu->Branch();
		if(GameLocks::NumLocks() > 0)
			pMenu->AddMenuItem( "Unlock ALL", MENUCMD_UNLOCKALL, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Custom LOD: LOW", MENUCMD_CHARACTERLOD_LOW, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Custom LOD: MID", MENUCMD_CHARACTERLOD_MID, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Custom LOD: HIGH", MENUCMD_CHARACTERLOD_HIGH, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Loadout");
		pMenu->Branch();
		pMenu->AddMenuItem( "Next loadout", MENUCMD_NEXTLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Random loadout", MENUCMD_RANDOMLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Twong!", MENUCMD_TWONGLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Food!", MENUCMD_FOODLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Rob!", MENUCMD_ROBLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Major!", MENUCMD_MAJORLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Cletus!", MENUCMD_CLETUSLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Bizna!", MENUCMD_BIZNALOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Travis!", MENUCMD_TRAVISLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Dwain!", MENUCMD_DWAINLOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Vince!", MENUCMD_VINCELOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Bryce!", MENUCMD_BRYCELOADOUT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Full Ammo", MENUCMD_FULLAMMO, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Avatar" );
		pMenu->Branch();

		pMenu->AddMenuItem( "Nightvision", MENUCMD_NIGHTVISION, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Depth of Field", MENUCMD_DOF, ProcessMenuCommand, this );

		pMenu->AddMenuItem( "Aim");
		pMenu->Branch();
		pMenu->AddMenuItem( "Down camera", MENUCMD_AIMDOWNCAMERA, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Triangulate", MENUCMD_AIMTRIANGULATE, ProcessMenuCommand, this );
		pMenu->Return();
		pMenu->AddMenuItem( "Aim Debug",  MENUCMD_DEBUGAIMSIM,  ProcessMenuCommand,  this );

#ifndef CDROM
		pMenu->AddMenuItem( "Shatter Test", MENUCMD_SHATTERTEST, ProcessMenuCommand, this );
#endif

		pMenu->AddMenuItem( "Afflict" );
		pMenu->Branch();
		pMenu->AddMenuItem( "Deaf", MENUCMD_AFFLICTDEAF, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Blind", MENUCMD_AFFLICTBLIND, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Stun", MENUCMD_AFFLICTSTUN, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Fire", MENUCMD_AFFLICTFIRE, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Smoke", MENUCMD_AFFLICTSMOKE, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tear gas", MENUCMD_AFFLICTTEARGAS, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Pain");
		pMenu->Branch();
		pMenu->AddMenuItem( "Decapiate", MENUCMD_DECAPITATE, ProcessMenuCommand, this );
		//pMenu->AddMenuItem( "Yardsale!", MENUCMD_YARDSALE, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Head", MENUCMD_PAINHEAD, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Chest", MENUCMD_PAINCHEST, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Back", MENUCMD_PAINBACK, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Left Shoulder", MENUCMD_PAINLSHOULDER, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Right Shoulder", MENUCMD_PAINRSHOULDER, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Left Leg", MENUCMD_PAINLLEG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Right Leg", MENUCMD_PAINRLEG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Bourbon!", MENUCMD_NOPAIN, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Show Coords", MENUCMD_TOGGLECOORDINATES, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Toggle Hud", MENUCMD_TOGGLEHUD, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Do Your Own Thing", MENUCMD_DOYOUROWNTHING, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Stats" );
		pMenu->Branch();
		pMenu->AddMenuItem( "FPS", MENUCMD_TOGGLEFPS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Memory", MENUCMD_TOGGLEHEAPINFO, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Render", MENUCMD_TOGGLESTATS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Instance", MENUCMD_TOGGLEINSTANCESTATS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Network", MENUCMD_NETWORK_DATA, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "AI", MENUCMD_TOGGLEAISTATS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Collision", MENUCMD_TOGGLECOLLISIONSTATS, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Logs" );
		pMenu->Branch();
		pMenu->AddMenuItem( "Tgl Profiling", MENUCMD_TOGGLEPROFILING, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Print Message", MENUCMD_PRINTMESSAGEDEBUG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Message Log", MENUCMD_TOGGLEMESSAGELOG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Output Log", MENUCMD_TOGGLEOUTPUTLOG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Session Time Log", MENUCMD_TOGGLESESSIONTIMELOG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Dump Stack", MENUCMD_DUMPCOLLISIONSTACK, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Plot Stack", MENUCMD_PLOTCOLLISIONSTACK, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Dump Memory Report", MENUCMD_TOGGLEMEMORYREPORT, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Dump Instance List", MENUCMD_DUMPINSTANCELIST, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Dump Agent List", MENUCMD_DUMPAGENTLIST, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Game" );
		pMenu->Branch();
		pMenu->AddMenuItem( "Disable World Actors", MENUCMD_DISABLEWORLDACTORS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Hide Actors", MENUCMD_TOGGLEHIDEACTORS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Show Collision", MENUCMD_COLLISIONDEBUG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Show Surface", MENUCMD_SURFACEDEBUG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Prnt Surf. Data", MENUCMD_PRINTSURFACEDATA, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Dead Reck", MENUCMD_TOGGLEDEADRECKONDRAW, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Dead Reck Log", MENUCMD_TOGGLEDEADRECKONDEBUG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Set Error", MENUCMD_SETERROR, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Error", MENUCMD_TOGGLEERROR, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "End Round", MENUCMD_ENDROUND, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Spawn Point", MENUCMD_TOGGLESPAWNPOINTS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Tgl Debug Clock", MENUCMD_TOGGLEDEBUGCLOCK, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Render" );
		pMenu->Branch();
#if defined( WIN32 ) && !defined( _XBOX)
		pMenu->AddMenuItem( "Maximize/Window", MENUCMD_TOGGLEMAXIMIZE, ProcessMenuCommand, this );
#endif
		SceneMgr::RegisterDebugMenus( pMenu );
		pMenu->AddMenuItem( "Toggle Point Lights", MENUCMD_TOGGLEPOINTLIGHTS, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Toggle Wire-Frame", RenderMgr__ToggleWireframe );
#ifdef PS2
		pMenu->AddMenuItem( "Toggle Vertex Colors", RenderMgr__ToggleVertexColors );
#endif
		pMenu->AddMenuItem( "Toggle Fog", RenderMgr__ToggleFog );
		pMenu->AddMenuItem( "Force LOD Up", RenderMgr__ForceLODsUp );
		pMenu->AddMenuItem( "Force LOD Down", RenderMgr__ForceLODsDown );
		pMenu->AddMenuItem( "Toggle V-Sync", RenderMgr__ToggleVSync );
		pMenu->AddMenuItem( "Toggle FBuf Clear", MENUCMD_TOGGLEFBUFCLEAR, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Toggle View BackB", RenderMgr__ToggleViewBackBuffer );
		pMenu->AddMenuItem( "Toggle Shadow Dbg", MENUCMD_TOGGLESHADOWDEBUG, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Toggle Bone Render", RenderMgr__ToggleBoneRender );
		pMenu->AddMenuItem( "Toggle Textures", RenderMgr__ToggleTextures );
		pMenu->AddMenuItem( "Toggle Particles", MENUCMD_TOGGLEPARTICLES, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Toggle Particulates", MENUCMD_TOGGLEPARTICULATES, ProcessMenuCommand, this );
		pMenu->Return();

		// Walmart M = hypocrisy to the hilt
		pMenu->AddMenuItem( "Blood" );
		pMenu->Branch();
		pMenu->AddMenuItem( "Blood on", MENUCMD_BLOODON, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Blood off", MENUCMD_BLOODOFF, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem("Stream Test");
		pMenu->Branch();
		pMenu->AddMenuItem( "Add", MENUCMD_STREAMTESTADD, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Remove", MENUCMD_STREAMTESTREMOVE, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Clear", MENUCMD_STREAMTESTCLEAR, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem( "Network" );
		pMenu->Branch();
		pMenu->AddMenuItem( "Flush Bundles", MENUCMD_FLUSHBUNDLES, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Toggle Bundling", MENUCMD_TOGGLEBUNDLING, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Mark Trace", MENUCMD_MARKTRACE, ProcessMenuCommand, this );
		pMenu->AddMenuItem( "Toggle Conn Stab", MENUCMD_TOGGLECONNSTAB, ProcessMenuCommand, this );
		pMenu->Return();

		pMenu->AddMenuItem("EMail Output");
		pMenu->Branch();
		for (int i = 0; i < NEmailOutput::Count(); i++)
		{
			pMenu->AddMenuItem((char*)NEmailOutput::Label(i),
					MENUCMD_EMAILBASE + i, ProcessMenuCommand, this);
		}
		pMenu->Return();
	}
#endif // CONSUMER_BUILD
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::ProcessMenuCommand(int i_command, PARAM i_param)
{
#ifndef CONSUMER_BUILD
	if (i_param)
		((CTTLDebug*)i_param)->ProcessMenu(i_command);
#endif
}

/***************************************************************************/
/***************************************************************************/
#ifndef CONSUMER_BUILD
bool gbl_plants = true;
#ifdef PS2
extern struct
{
	bool disableTextures;
	bool disableAlpha;
	bool forceWireFrame;
} StripBuffer_Dbg;
#endif

#include "geomUtil/LineToLine.h"
void TestLineTri()
{
	Vector3 pointOnLine, pointOnTriangle;
	Vector3 A( -3.864734f, -0.042908f, 5.952684f );
	Vector3 B( 0.425971f, 2.395988f, -1.721598f );
	Vector3 C( 0.640108f, -0.042908f, -1.745903f );
	Vector3 N( -0.860943f, -0.070571f, -0.503782f );
	Vector3 O( -2.247233f, 0.732594f, 2.111898f );
	Vector3 E( -2.247233f, 1.552954f, 2.111898f );

	Vector3 axis = E - O;
	float axisLength = axis.Length();
	axis.SafeNormalize();

	float dSquared = SegmentToTriangleDistanceSquared( O, axis,
		0.0f, axisLength, N, A, B - A, C - A, pointOnLine, pointOnTriangle );

	DrawUtility::Point( pointOnLine, DrawUtility::GREEN, 0.1f );
	DrawUtility::Point( pointOnTriangle, DrawUtility::YELLOW, 0.1f );
	DrawUtility::Point( A, DrawUtility::RED, 0.1f );
	DrawUtility::Point( B, DrawUtility::GREEN, 0.1f );
	DrawUtility::Point( C, DrawUtility::BLUE, 0.1f );
	DrawUtility::Line( O, E, DrawUtility::YELLOW );
	DrawUtility::Line( pointOnTriangle, pointOnLine, DrawUtility::BLUE );
}

void CreateAnotherPlayer()
{
	static int playerIndex = 1;
	if ( playerIndex < 16  && g_players.GetPlayer(0))
	{
		Vector3 center = g_players.GetPlayer( 0 )->GetBodyInWorld();
		g_players.CreatePlayer( playerIndex, playerIndex % 8 );
		CCombatant* act = (CCombatant*) g_players.GetPlayer( playerIndex++ );
		act->Place( center + Vector3( 0.0f, 0.0f, 1.0f ), 0.0f );
		act->GetComponentByName( "BipedPhysicalController" )->Deactivate();
		act->GetComponentByName( "ViewDriver" )->Deactivate();
		g_players.ActivateControls( true );
	}
}
#endif // CONSUMER_BUILD

void CTTLDebug::ProcessMenu(int i_command)
{
#ifndef CONSUMER_BUILD
	// Special thing for email
	if ((i_command >= MENUCMD_EMAILBASE) &&
			(i_command < (MENUCMD_EMAILBASE + NEmailOutput::Count())))
	{
		NEmailOutput::Send(i_command - MENUCMD_EMAILBASE);
	}

#ifdef CDROM
	bool networked = (ValidSingleton(CSession) && g_session.IsNetworked());
#else
	bool networked = false;
#endif

	switch( i_command )
	{
		case MENUCMD_DISABLEWORLDACTORS:
			WorldActorLoader::DisableActors();
			break;
		case MENUCMD_DOYOUROWNTHING:
		{
			//TestLineTri();
			HardwareBreak::FaultTest();
		}
			break;

		case MENUCMD_TOGGLEPLANTS:
			gbl_plants = !gbl_plants;
			break;

#ifdef PS2
		case MENUCMD_TOGGLEWIREWATER:
			StripBuffer_Dbg.forceWireFrame = !StripBuffer_Dbg.forceWireFrame;
			break;
#endif

		case MENUCMD_TOGGLEPROFILING:
			d_profiling = !d_profiling;
			break;

		case MENUCMD_PRINTMESSAGEDEBUG:
			g_messageDispatcher.DumpDebug();
			break;

		case MENUCMD_TOGGLEMESSAGELOG:
			g_messageDispatcher.ToggleLog();
			break;

		case MENUCMD_TOGGLEOUTPUTLOG:
			g_debugOutput.ToggleLog();
			break;

		case MENUCMD_TOGGLESESSIONTIMELOG:
			g_session.ToggleSessionTimeDebug();
			break;

		case MENUCMD_TOGGLEHIDEACTORS:
			(g_stage.MasterCast()).ToggleHideActors();
			break;

		case MENUCMD_PRINTSURFACEDATA:
		{
			char text[80];
			g_console.Echo("\n");
			if (g_surfaceType.GetTitle(text, 80))
				g_console.Echo("%s\n", text);
			for (int j = 0; j < g_surfaceType.GetNumEntries(); j++)
			{
				char* info = g_surfaceType.GetInfo(j, text, 80);
				if (info)
					g_console.Echo("%s\n", info);
			}
			break;
		}

		case MENUCMD_SURFACEDEBUG:
			d_surfaceDebug = !d_surfaceDebug;
		break;

		case MENUCMD_SETERROR:
			g_errorStack.SetError("Debug", "Fatal Error", CErrorStack::ERROR_FATAL,
					5000);
			break;

		case MENUCMD_TOGGLEERROR:
			if ((d_errorHandle == ERROR_HANDLE_INVALID) ||
					!g_errorStack.ClearError(d_errorHandle))
				d_errorHandle = g_errorStack.SetError("Debug", "Warning Error",
						CErrorStack::ERROR_WARNING, ERROR_INFINITE_LIFETIME, "Ok");
			else
				d_errorHandle = ERROR_HANDLE_INVALID;
			break;

		case MENUCMD_ENDROUND:
			if (ValidSingleton(Referee))
				g_referee.CommandPlayingPhaseExit();
			break;

		case MENUCMD_TOGGLEDEADRECKONDRAW:
			g_messageDispatcher.SendMessageToAll("ToggleDrawDeadReckoning", NULL, INVALID_OBJECT_HANDLE);
			break;

		case MENUCMD_TOGGLEDEADRECKONDEBUG:
			g_messageDispatcher.SendMessageToActor("ToggleDeadReckoningDebug", NULL, INVALID_OBJECT_HANDLE, d_select);
			break;

		case MENUCMD_TOGGLESPAWNPOINTS:
			d_spawnPointDebug = !d_spawnPointDebug;
			break;

		case MENUCMD_TOGGLEDEBUGCLOCK:
			g_sessionTimestamp = !g_sessionTimestamp;
			g_debugOutput.Print(false, "Using %s clock",
					g_sessionTimestamp ? "session" : "OS");
			break;

		case MENUCMD_COLLISIONDEBUG:
			if (++d_collisionDebug == 5)
				d_collisionDebug = 0;
			break;

		case MENUCMD_RESETPLAYERS:
			if (!ValidSingleton(CSession) || !g_session.IsNetworked())
				g_players.ResetPlayerPositions();
			break;

		case MENUCMD_KILL:
			{
				if (!g_players.GetPlayer(0))
					break;
				PlayerStats *stats = (PlayerStats*)g_players.GetPlayer(0)->GetInterface(PlayerStats::GetID());
				if (stats)
					stats->Smack(100.0f);
			}
			break;

		case MENUCMD_SMACK:
			{
				if (!g_players.GetPlayer(0))
					break;
				PlayerStats *stats = (PlayerStats*)g_players.GetPlayer(0)->GetInterface(PlayerStats::GetID());
				if (stats)
					stats->Smack(9.0f);
			}
			break;

		case MENUCMD_ENDLEVEL:
			{
				int currentLevel = g_gameEnvironment.Get();
				int nextLevel = g_gameEnvironment.GetByType(currentLevel + 1, CGameEnv::TYPE_SINGLEPLAYER);
				const char *wantLevel;
				if (g_gameEnvironment.EndOfGame() || nextLevel < 0)
					wantLevel = "quit";
				else
				{
					wantLevel = g_gameEnvironment.GetWorldFileName(nextLevel);
					Cheats::SignalExternalCheat();
				}
				DeactivateDebugMenu();
				g_messageDispatcher.SendMessageToAll("SwitchLevel", (void *)wantLevel, (ActorHandle)INVALID_OBJECT_HANDLE);
			}
			break;

		case MENUCMD_HEAL:
			{
				if (!ValidSingleton(CSession) || g_session.IsNetworked())
					break;
				if (!g_players.GetPlayer(0))
					break;
				PlayerStats *stats = (PlayerStats*)g_players.GetPlayer(0)->GetInterface(PlayerStats::GetID());
				if (stats)
				{
					stats->AddHealth(50.0f);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_NIGHTVISION:
			{
#ifdef PS2
				extern bool gDOFOn;
				extern bool gDOFGreen;
				extern bool gDOFShowBlured;
				gDOFGreen = !gDOFGreen;
				gDOFShowBlured = gDOFGreen;
				gDOFOn = gDOFGreen;
#endif
			}
			break;

		case MENUCMD_DOF:
			{
#ifdef PS2
				extern bool gDOFOn;
				gDOFOn = !gDOFOn;
#endif
			}
			break;


		case MENUCMD_AIMDOWNCAMERA:
			{
			AimingSimulation::SetDefaultAimType(AimingSimulation::CAT_SIGHTLINE);
			if (!g_players.GetPlayer(0))
				break;
			WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
			if (inv)
				inv->Reset();
			}
			break;
		case MENUCMD_AIMTRIANGULATE:
			{
			AimingSimulation::SetDefaultAimType(AimingSimulation::CAT_TRIANGULATE);
			if (!g_players.GetPlayer(0))
				break;
			WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
			if (inv)
				inv->Reset();
			}
			break;

		case MENUCMD_SHATTERTEST:
			{
				CProtagonist *player = g_players.GetPlayer(0);
				if (player)
				{
					ShatterObject *shatter = g_shatter.Create("slowglass");
					if (shatter)
						shatter->Start(*player);
				}
			}
			break;

		case MENUCMD_AFFLICTDEAF:
			{
				const float duration = 5.0f;
				CProtagonist *player = g_players.GetPlayer(0);
				if (player)
					g_messageDispatcher.SendMessageToActor("MunitionDeaf", CMessageDispatcher::FloatToData(duration), player->GetHandle(), player->GetHandle());
			}
			break;


		case MENUCMD_AFFLICTBLIND:
			{
				CProtagonist *player = g_players.GetPlayer(0);
				if (player)
				{
					SMunitionFlash flash;
					flash.d_intensity = 15.0f;
					flash.d_munitionID = 0;
					flash.d_pos.Clear();
					g_messageDispatcher.SendMessageToActor("MunitionFlash", &flash, player->GetHandle(), player->GetHandle());
				}
			}
			break;

		case MENUCMD_AFFLICTSTUN:
			{
				const float duration = 8.0f;
				CProtagonist *player = g_players.GetPlayer(0);
				if (player)
					g_messageDispatcher.SendMessageToActor("MunitionStun", CMessageDispatcher::FloatToData(duration), player->GetHandle(), player->GetHandle());
			}
			break;

		case MENUCMD_AFFLICTFIRE:
			{
				CProtagonist *player = g_players.GetPlayer(0);
				if (player)
					g_messageDispatcher.SendMessageToActor("MunitionOnFire", CMessageDispatcher::IntToData(-1), player->GetHandle(), player->GetHandle());
			}
			break;

		case MENUCMD_AFFLICTSMOKE:
			{
				const float strength = 50.0f;
				CProtagonist *player = g_players.GetPlayer(0);
				if (player)
					g_messageDispatcher.SendMessageToActor("InSmoke", CMessageDispatcher::FloatToData(strength), player->GetHandle(), player->GetHandle());
			}
			break;

		case MENUCMD_AFFLICTTEARGAS:
			{
				const float strength = 50.0f;
				CProtagonist *player = g_players.GetPlayer(0);
				if (player)
					g_messageDispatcher.SendMessageToActor("InTearGas", CMessageDispatcher::FloatToData(strength), player->GetHandle(), player->GetHandle());
			}
			break;

		case MENUCMD_DECAPITATE:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->Decapitate(!stats->Decapitated());
			}
			break;

		case MENUCMD_YARDSALE:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->YardSaleIncrease();
			}
			break;

		case MENUCMD_PAINHEAD:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->UpThePain(CPainDecal::PAINZONE_HEAD);
			}
			break;

		case MENUCMD_PAINCHEST:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->UpThePain(CPainDecal::PAINZONE_CHEST);
			}
			break;

		case MENUCMD_PAINBACK:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->UpThePain(CPainDecal::PAINZONE_BACK);
			}
			break;

		case MENUCMD_PAINLSHOULDER:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->UpThePain(CPainDecal::PAINZONE_SHOULDER_L);
			}
			break;

		case MENUCMD_PAINRSHOULDER:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->UpThePain(CPainDecal::PAINZONE_SHOULDER_R);
			}
			break;

		case MENUCMD_PAINLLEG:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->UpThePain(CPainDecal::PAINZONE_LEG_L);
			}
			break;

		case MENUCMD_PAINRLEG:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->UpThePain(CPainDecal::PAINZONE_LEG_R);
			}
			break;

		case MENUCMD_NOPAIN:
			{
				if (!g_players.GetPlayer(0))
					break;
 				CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
				if (stats)
					stats->Reset();
			}
			break;

#ifdef ALLOW_DEBUG_MENU
		case MENUCMD_TOGGLESTATS:
			s_hud->m_Debug.ToggleRenderStats();
			break;

		case MENUCMD_TOGGLEINSTANCESTATS:
			s_hud->m_Debug.ToggleInstanceStats();
			break;

		case MENUCMD_TOGGLECOLLISIONSTATS:
			s_hud->m_Debug.ToggleCollisionStats();
			break;

		case MENUCMD_TOGGLEFPS:
			s_hud->m_Debug.ToggleFPS();
			break;

		case MENUCMD_TOGGLECOORDINATES:
			s_hud->m_Debug.ToggleCoordinates();
			break;

		case MENUCMD_TOGGLESHADOWDEBUG:
			RenderMgr__ToggleShadowDebug();
			s_hud->m_Debug.ShowRenderStats(RenderMgr__ShadowDebugOn());
			break;

		case MENUCMD_TOGGLEHEAPINFO:
			s_hud->m_Debug.ToggleMemoryStats();
			break;
#endif //ALLOW_DEBUG_MENU

		case MENUCMD_TOGGLEFBUFCLEAR:
		{
			CScene* scene = d_ttlLoop->GetScenes().Scene(0)->Scene();
			scene->d_sceneView.SetClearMode((scene->d_sceneView.GetClearMode() & RS_COLOR_BUFFER_BIT) == 0);
			break;
		}

		case MENUCMD_TOGGLEHEAPHIGHWATER:
			Layers__TrackMemoryHighWater(!Layers__MemoryHighWaterOn());
			break;

#ifndef CDROM
		case MENUCMD_STATSTOCONSOLE:
			GameEngine__StatsToConsole();
			break;
#endif

	case MENUCMD_TOGGLEPARTICULATES:
			if(ParticulateMgr::RenderEnabled())
				ParticulateMgr::RenderDisable();
			else
				ParticulateMgr::RenderEnable();
			break;

		case MENUCMD_TOGGLEPARTICLES:
			ParticleMgr__Toggle();
			break;

		case MENUCMD_TOGGLEMEMORYREPORT:
			//s_showMemoryReport = !s_showMemoryReport;
#if !defined(CDROM) && defined(MEM_DEBUG_BLOCKS)
			ShowMemoryReport();
#endif
			break;

		case MENUCMD_DUMPINSTANCELIST:
			SceneMgr::DumpInstanceList();
			break;

		case MENUCMD_DUMPAGENTLIST:
			g_referee.DumpAgentList();
			break;

		case MENUCMD_MUSICVOLUMEUP:
			g_ttl->AdjustMusicVolume(0.1f);
			break;

		case MENUCMD_MUSICVOLUMEDOWN:
			g_ttl->AdjustMusicVolume(-0.1f);
			break;

#if defined( WIN32 ) && !defined( _XBOX)
		case MENUCMD_TOGGLEMAXIMIZE:
			gfxToggleWindowMaximize();
			break;
#endif

		case MENUCMD_WATERDEBUG:
		{
			const char* msg = g_aniHFMgr.IncrementDebugState();
			g_console.Echo("%s\n", msg);
			break;
		}

		case MENUCMD_SAVESTARTPOS:
			g_players.SavePlayerStartPos();
			break;

		case MENUCMD_ANIMTEST:
			g_messageDispatcher.SendMessageToAll("AnimTest", NULL, INVALID_OBJECT_HANDLE);
			break;

		case MENUCMD_DUMPCOLLISIONSTACK:
			DUMP_STACK(g_collisionSystem.GetStack());
			break;

		case MENUCMD_PLOTCOLLISIONSTACK:
			d_plotCollisionStack = !d_plotCollisionStack;
			break;

		case MENUCMD_TOGGLEHUD:
			g_hud.Activate(!g_hud.Activated());
			break;

		case MENUCMD_TOGGLETESTHUD:
		{
			Screen* pScreen = g_FrontEnd.GetScreen( "SCREEN_ID_TESTHUD" );

			d_toggleTestHud = !d_toggleTestHud;

			if( pScreen )
			{
				if( d_toggleTestHud )
					pScreen->Show();
				else
					pScreen->Hide();
			}
			break;
		}

		case MENUCMD_TOGGLEPOINTLIGHTS:
			g_lightSet.togglePointLights( );
			break;

		case MENUCMD_NETWORK_DATA:
			s_hud->m_Debug.ToggleNetworkStats();
			break;

		case MENUCMD_TOGGLEAISTATS:
			s_hud->m_Debug.ToggleAIStats();
			break;

		case MENUCMD_PLAYCULDESAC:
			g_cutSceneManager.PlayCutScene( "cutscenes\\culdesac.cut" );
			break;

		case MENUCMD_NEXTLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout::BuildDebug(*inv, d_loadout++);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_RANDOMLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout::BuildRandom(*inv);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_TWONGLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=bottle; weapon1=twong; weapon2=usas12; weapon3=M67");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_FOODLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=bottle; weapon1=ilike; weapon2=m249; weapon3=food");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_ROBLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=bat; weapon1=deaglelaser; weapon2=aug; weapon3=smoke");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;


		case MENUCMD_MAJORLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=tireiron; weapon1=glock2; weapon2=dualuzi; weapon3=m79");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_CLETUSLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=bottle; weapon1=357mag; weapon2=sawedoff; weapon3=molotov");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_BIZNALOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=bat; weapon1=python; weapon2=dualmp5k; weapon3=FA37Tear");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_TRAVISLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=taser; weapon1=45csd; weapon2=psg1; weapon3=Concussion");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_DWAINLOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=bowie; weapon1=berettasd; weapon2=barret; weapon3=smoke");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_VINCELOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=stungun; weapon1=glock; weapon2=LAW; weapon3=M79");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_BRYCELOADOUT:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout loadout("weapon0=pepper; weapon1=45csd; weapon2=SniperSD; weapon3=flash");
					loadout.Build(*inv, true);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_FULLAMMO:
			{
				if (!g_players.GetPlayer(0) || networked)
					break;
				WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
				if (inv)
				{
					WeaponLoadout ammoTest;
					ammoTest.FullAmmo();
					ammoTest.Build(*inv);
					WeaponLoadout::DebugDump(*inv);
					Cheats::SignalExternalCheat();
				}
			}
			break;

		case MENUCMD_BLOODON:
			if (ValidSingleton(Blood))
				g_blood.Enable(true);
			break;
		case MENUCMD_BLOODOFF:
			if (ValidSingleton(Blood))
				g_blood.Enable(false);
			break;

		case MENUCMD_JUKEBOXADDSOURCE:
			{
				if (!g_players.GetPlayer(0))
					break;
				Vector3 pos = g_players.GetPlayer(0)->GetBodyInWorld() + Vector3(0.0f, 2.0f, 0.0f);
				g_audio.jukebox.AddPlayPosition(pos);
			}
			break;

		case MENUCMD_STREAMTESTADD:
			{
				if (!g_players.GetPlayer(0))
					break;
				Vector3 pos = g_players.GetPlayer(0)->GetBodyInWorld() + Vector3(0.0f, 2.0f, 0.0f);
				StreamTest::AddStream("stest", &pos);
			}
			break;

		case MENUCMD_STREAMTESTREMOVE:
			StreamTest::RemoveStream();
			break;

		case MENUCMD_STREAMTESTCLEAR:
			StreamTest::RemoveAll();
			break;
		case MENUCMD_UNLOCKALL:
			GameLocks::UnlockAll();
			break;
		case MENUCMD_CHARACTERLOD_LOW:
			ActorCreator::SetCombatantLOD(COMBATANT_LOD_LOW);
			ActorCreator::SetUseLOD(true);
			break;
		case MENUCMD_CHARACTERLOD_MID:
			ActorCreator::SetCombatantLOD(COMBATANT_LOD_MID);
			ActorCreator::SetUseLOD(true);
			break;
		case MENUCMD_CHARACTERLOD_HIGH:
			ActorCreator::SetCombatantLOD(COMBATANT_LOD_HIGH);
			ActorCreator::SetUseLOD(true);
			break;

		case MENUCMD_FLUSHBUNDLES:
			 g_connection.FlushBundles();
			 break;

		case MENUCMD_TOGGLEBUNDLING:
			 g_connection.ToggleBundling();
			 break;

		case MENUCMD_MARKTRACE:
			 g_connection.MarkTrace();
			 break;

		case MENUCMD_TOGGLECONNSTAB:
#ifdef NETZ
			CNetZConnection::s_netSettings.d_connectionStabilityEnabled =
					!CNetZConnection::s_netSettings.d_connectionStabilityEnabled;
#endif
			break;

		#ifndef CONSUMER_BUILD
		case MENUCMD_TOGGLEMAYAINTERFACE:
			IMM::ToggleConnection();
			break;
		case MENUCMD_AISPY:
			g_aiSpy = !g_aiSpy;
			break;
		case MENUCMD_MATERIALSPY:
			MaterialSpyInstrument::Toggle();
			break;
		case MENUCMD_ACTORSPY:
			g_messageDispatcher.SendMessageToAll("ToggleActorSpy", NULL,
					INVALID_OBJECT_HANDLE);
			break;
		case MENUCMD_DEBUGAIMSIM:
			g_debugAimSim = !g_debugAimSim;
			break;
		#endif
	}
#endif // CONSUMER_BUILD
}

/***************************************************************************/
/***************************************************************************/
bool CTTLDebug::HandleDebugMenu(void)
{
#ifndef CONSUMER_BUILD

	ASSERT_PTR(d_menuMapper);

	// Handle the controls
	if (s_hud->m_Debug.m_Menu.m_Active)
	{
		if (!g_controlFocus.OnTop(d_menuMapper))
		{
			g_controlFocus.Pop(d_menuMapper);
			g_controlFocus.Push(d_menuMapper);
		}

		if (d_menuMapper->ButtonPressed(DefaultControlMapper::BUTTON_UP) ||
				g_input.ButtonPressed(BC_UP))
			s_hud->m_Debug.m_Menu.CursorUp();

		if (d_menuMapper->ButtonPressed(DefaultControlMapper::BUTTON_DOWN) ||
				g_input.ButtonPressed(BC_DOWN))
			s_hud->m_Debug.m_Menu.CursorDown();

		if (d_menuMapper->ButtonPressed(DefaultControlMapper::BUTTON_LEFT) ||
				g_input.ButtonPressed(BC_LEFT))
			s_hud->m_Debug.m_Menu.CursorLeft();

		if (d_menuMapper->ButtonPressed(DefaultControlMapper::BUTTON_RIGHT) ||
				g_input.ButtonPressed(BC_RIGHT))
			s_hud->m_Debug.m_Menu.CursorRight();

		if (d_menuMapper->ButtonPressed(DefaultControlMapper::BUTTON_X) ||
				g_input.ButtonPressed(BC_ENTER))
			s_hud->m_Debug.m_Menu.CursorEnter();

		if (d_menuMapper->ButtonPressed(DefaultControlMapper::BUTTON_TRI) ||
				g_input.ButtonPressed(BC_ESCAPE))
			s_hud->m_Debug.m_Menu.CursorExit();

		if (d_menuMapper->ButtonPressed(DefaultControlMapper::BUTTON_SELECT) ||
				g_input.ButtonPressed(BC_TILDE))
			DeactivateDebugMenu();

		return(true);
	}
#endif

	return (false);
}

void CTTLDebug::ActivateDebugMenu( void )
{
#ifndef CONSUMER_BUILD
	if (!s_hud->m_Debug.m_Menu.m_Active)
	{
 		g_controlFocus.Push(d_menuMapper);
	 	s_hud->m_Debug.m_Menu.ActivateMenu();
	}
#endif
}

void CTTLDebug::DeactivateDebugMenu( void )
{
#ifndef CONSUMER_BUILD
	if (s_hud->m_Debug.m_Menu.m_Active)
	{
		s_hud->m_Debug.m_Menu.DeactivateMenu();
 		g_controlFocus.Pop(d_menuMapper);
	}
#endif
}

/***************************************************************************/
/***************************************************************************/
bool CTTLDebug::InterceptControls(void)
{
	bool eatButtonPresses = false;

#ifndef CONSUMER_BUILD
	ControlMapper* mapper;
	ControllerInputDriver* device;

	if ( ValidSingleton(CPlayers) &&	Cheats::Enabled(Cheats::FREECAMERA))
	{
		for (int i = 0; i < g_players.NumPlayers(); i++)
		{
			ControlMapper* mapper = g_players.GetMapper(i, false);
			device = NULL;
			if (mapper)
				device = mapper->GetDevice();

			bool wantFreeCamera;
	#ifdef _XBOX
			wantFreeCamera = (device && device->ButtonDown(CB_SELECT) && device->ButtonPressed(CB_R2));
	#else
			wantFreeCamera = (device && device->ButtonDown(CB_L2) && device->ButtonPressed(CB_SELECT));
	#endif
			if (wantFreeCamera)
			{
				bool inFreeCam = d_ttlLoop->GetScenes().ActivateFreeCamera(i);
				if (i == 0)
					eatButtonPresses = true;
				else
					device->EatButtonPresses();
			}
		}
	}

	// Now get the first controller. Exit if one does not exist.
	if ( g_input.ControllerLocked(0) )
		device = g_input.GetController(0);
	else
		device = NULL;

	if (device == NULL)
		return(false);

	// Put up the debug menu if not already up.
	if (!s_hud->m_Debug.m_Menu.m_Active)
	{
	#if defined (GCN)
		if (device->ButtonDown(CB_STICK1) && device->ButtonPressed(CB_O))
	#else
		bool wantDebug = (device->ButtonDown(CB_L1) && device->ButtonPressed(CB_SELECT));
		if (wantDebug)
	#endif
		{
			device->EatButtonPresses();
			ActivateDebugMenu();
		}
	}

	if (device->ButtonDown(CB_L2))
	{
		if (device->ButtonPressed(CB_START))
		{
			g_FrontEnd.ToggleDebugOutput();
			eatButtonPresses = true;
		}

		if (g_FrontEnd.IsDebugOutputShown())
		{
			static int selectedCategory = -2;
			if (device->ButtonPressed(CB_UP))
			{
				g_FrontEnd.GetDebugOutput().DisplayLineUp();
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_DOWN))
			{
				g_FrontEnd.GetDebugOutput().DisplayLineDown();
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_LEFT))
			{
				g_FrontEnd.GetDebugOutput().DisplayPageUp();
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_RIGHT))
			{
				g_FrontEnd.GetDebugOutput().DisplayPageDown();
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_STICK1))
			{
				g_FrontEnd.GetDebugOutput().DisplayOldest();
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_STICK2))
			{
				g_FrontEnd.GetDebugOutput().DisplayNewest();
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_SQUARE))
			{
				bool show = g_FrontEnd.GetDebugOutput().ToggleShowTimestamps();
				if (selectedCategory < 0)
					g_debugOutput.Print(false, "%showing timestamps",
							show ? "S" : "Not s");
				else
					g_debugOutput.Print((uint8)selectedCategory,
							"%showing timestamps", show ? "S" : "Not s");
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_O))
			{
				bool wrap = g_FrontEnd.GetDebugOutput().ToggleWrap();
				if (selectedCategory < 0)
					g_debugOutput.Print(false, "%srapping output",
							wrap ? "W" : "Not w");
				else
					g_debugOutput.Print((uint8)selectedCategory, "%srapping output",
							wrap ? "W" : "Not w");
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_TRI))
			{
				if (++selectedCategory >= g_debugOutput.GetNumCategories())
					selectedCategory = -2;
				if (selectedCategory == -2)
				{
					g_FrontEnd.GetDebugOutput().SetShowCategories(false);
					g_FrontEnd.GetDebugOutput().SelectAllCategories();
					g_debugOutput.Print("Not showing categories");
				}
				else if (selectedCategory == -1)
				{
					g_FrontEnd.GetDebugOutput().SetShowCategories(true);
					g_FrontEnd.GetDebugOutput().SelectAllCategories();
					g_debugOutput.Print("Showing all categories");
				}
				else
				{
					g_FrontEnd.GetDebugOutput().SetShowCategories(true);
					g_FrontEnd.GetDebugOutput().DeselectAllCategories();
					g_FrontEnd.GetDebugOutput().SelectCategory(selectedCategory);
					g_debugOutput.Print((uint8)selectedCategory,
							"Showing %s category",
							g_debugOutput.GetCategoryName(selectedCategory));
				}
				eatButtonPresses = true;
			}
			if (device->ButtonPressed(CB_X))
			{
				const char* level = NULL;
				switch(g_FrontEnd.GetDebugOutput().GetVerbosity())
				{
				 case DEBUG_BORE:
					g_FrontEnd.GetDebugOutput().SetVerbosity(DEBUG_INFORM);
					level = "INFORM";
					break;
				 case DEBUG_INFORM:
					g_FrontEnd.GetDebugOutput().SetVerbosity(DEBUG_NOTIFY);
					level = "NOTIFY";
					break;
				 case DEBUG_NOTIFY:
					g_FrontEnd.GetDebugOutput().SetVerbosity(DEBUG_ALERT);
					level = "ALERT";
					break;
				 case DEBUG_ALERT:
					g_FrontEnd.GetDebugOutput().SetVerbosity(DEBUG_BORE);
					level = "BORE";
					break;
				}
				if (selectedCategory < 0)
					g_debugOutput.Print(false, "Setting debug verbosity to %s",
							level);
				else
					g_debugOutput.Print((uint8)selectedCategory,
							"Setting debug verbosity to %s", level);
				eatButtonPresses = true;
			}
		}
		else if (device->ButtonPressed(CB_TRI))
		{
			if (d_debugOutputPopup == NULL)
			{
				ScreenPopupDialog* popup = g_FrontEnd.GetPopupDialog();
				if (popup != NULL)
				{
					popup->Clear();
					popup->SetTitle("Debug Output Options");
					popup->SetMessage("Configure Category Display");
					if (g_FrontEnd.GetDebugOutput().GetShowCategories())
						popup->AddMenuItem("Hide", COMMAND_DEBUG_SHOW, NULL);
					else
						popup->AddMenuItem("Show", COMMAND_DEBUG_SHOW, NULL);
					popup->AddMenuItem("Select All", COMMAND_DEBUG_SELECT_ALL, NULL);
					popup->AddMenuItem("Deslect All", COMMAND_DEBUG_DESELECT_ALL,
							NULL);
					for (int i = 0; i < g_debugOutput.GetNumCategories(); i++)
					{
						char item[40];
						sprintf(item, "[%s] %s",
								g_FrontEnd.GetDebugOutput().IsCategorySelected(i) ?
								"X" : "  ", g_debugOutput.GetCategoryName(i));
						popup->AddMenuItem(item, COMMAND_DEBUG_CATEGORY_BASE + i,
								NULL);
					}
					popup->AddMenuItem("Done", COMMAND_DEBUG_DONE, NULL);
					g_FrontEnd.ShowPopupDialog(true);
					d_debugOutputPopup = popup;
				}
			}
			else
			{
				g_FrontEnd.ShowPopupDialog(false);
				d_debugOutputPopup = NULL;
			}
		}
	}

	// Now do things that require player 0 to have focus.
	if ( ValidSingleton(CPlayers) )
		mapper = g_players.GetMapper(0, false);
	else
		mapper = NULL;
	if ((mapper != NULL) && mapper->HasFocus())
	{
		// Holding down L1 while pressing the start button will pause the
		// simulation but not put up the pause screen or go into the pause
		// front end.
#ifdef ALLOW_SCREEN_SNAPS
		if (device->ButtonPressed(CB_START) && device->ButtonDown(CB_L1))
		{
#if !defined (SNAP_ON_L1_START)
			if (g_timer.IsPaused())
				d_ttlLoop->SetTimerSpeed(CMasterTimer::ONE_X);
			else
				d_ttlLoop->SetTimerSpeed(CMasterTimer::PAUSE);
			eatButtonPresses = true;
#else
			gfxSnapScreen();
#endif
		}
#endif

		// If the simulation paused but the avatar still has controller focus,
		// the pause front end is not active.
		if (g_timer.IsPaused())
		{
			if (device->ButtonPressed(CB_UP))
			{
				g_timer.AdvanceTick();
				eatButtonPresses = true;
			}

			if (device->ButtonPressed(CB_LEFT))
			{
				gfxSnapScreen();
				eatButtonPresses = true;
			}
		}
	}

	// Eat all button presses if any have been used.
	if (eatButtonPresses)
		device->EatButtonPresses();

#endif // CONSUMER_BUILD
	return(eatButtonPresses);
}

/***************************************************************************/
// Handle input
/***************************************************************************/
void CTTLDebug::HandleInput(void)
{
#ifndef CONSUMER_BUILD

#ifdef WIN32
	if (g_input.ButtonPressed(BC_4))
		gfxSnapScreen();
#endif

#if defined(WIN32) && !defined(_XBOX)
	if (g_input.ButtonPressed(BC_6))
		gfxToggleWindowMaximize();
#endif

#ifdef _DEBUG
	// Sound debugging stuff
	enum SoundDebugState { NONE, LOCAL, GLOBAL };
	static SoundDebugState state = NONE;
	if (g_input.ButtonPressed(BC_MULTIPLY))
	{
		if (state == NONE)
			state = LOCAL;
		else if (state == LOCAL)
			state = GLOBAL;
		else if (state == GLOBAL)
			state = NONE;
	}
	if (state == GLOBAL)
		Sfx::Debug(NULL);
	else if (state == LOCAL)
	{
		Vector3 pos = g_cameraDirector.CurrentCamera().Eye();
		Sfx::Debug(&pos);
	}
#endif

	if (g_input.ButtonPressed(BC_1))
		RenderMgr__ToggleWireframe();

	if (g_input.ButtonPressed(BC_B))
		RenderMgr__ToggleDrawBoxes();

	if (g_input.ButtonPressed(BC_2))
		RenderMgr__ToggleLighting();

	if (g_input.ButtonPressed(BC_3))
		RenderMgr__ToggleFog();

	if (g_input.ButtonPressed(BC_P))
#ifndef DIRECTX_PC
		Profiler__WriteLog(true);
#else
		RenderMgr__ToggleShadowDebug();
#endif

	if (g_input.ButtonPressed(BC_5))
		(g_stage.MasterCast()).ToggleHideActors();

	if ( g_input.ButtonPressed( BC_ESCAPE ) )
		g_ttl->Exit();

	if (g_input.ButtonPressed(BC_S))
		d_surfaceDebug = !d_surfaceDebug;

	if (g_input.ButtonPressed(BC_R))
		d_spawnPointDebug = !d_spawnPointDebug;

	if (g_input.ButtonPressed(BC_B))
	{
		if (++d_collisionDebug == 5)
			d_collisionDebug = 0;
	}

	if (g_input.ButtonPressed(BC_F))
	{
		d_collisionFrozen = !d_collisionFrozen;
		if (d_collisionFrozen)
		{
			Camera &camera = g_cameraDirector.CurrentCamera();
			d_collisionDir = camera.Dir();
			d_collisionEyepoint = camera.Eye();
		}
	}

	if (g_input.ButtonPressed(BC_C))
	{
		DUMP_STACK(g_collisionSystem.GetStack());
	}

	if (g_input.ButtonPressed(BC_X))
		d_plotCollisionStack = !d_plotCollisionStack;

	if (g_input.ButtonPressed(BC_T))
	{
		FileStream* fs = FileStream::NewAndOpen(
				g_gameEnvironment.WorldPath("Suspect.log", NULL),
				FileStream::WRITE, true);
		g_collisionSystem.TerrainEntity()->Validate(fs);
		if (fs)
		{
			fs->Close();
			delete fs;
		}
	}

	if (g_input.ButtonPressed(BC_V))
		g_stage.ValidateRotations(1.0e-4f);

#if 0
	if (g_input.ButtonPressed(BC_W))
	{
		const char* msg = g_aniHFMgr.IncrementDebugState();
		g_console.Echo("%s\n", msg);
	}
#endif

	if (g_input.ButtonPressed(BC_G))
	{
		SPause pause;
		pause.d_type = SPause::PAUSE;
		pause.d_menu = SCREEN_PAUSEMENU;
		pause.d_affectAudio = true;
		g_messageDispatcher.PostMessageToAll("Pause", (void*)&pause,
				INVALID_OBJECT_HANDLE);
	}

	if (g_input.ButtonPressed(BC_H))
	{
		if (g_timer.IsPaused())
			d_ttlLoop->SetTimerSpeed(CMasterTimer::ONE_X);
		else
			d_ttlLoop->SetTimerSpeed(CMasterTimer::PAUSE);
	}

	if (g_timer.IsPaused() && g_input.ButtonPressed(BC_SPACE) &&
			!strcmpi(g_controlFocus.Current(0)->Name(), "Avatar"))
		g_timer.AdvanceTick();

	if (g_input.ButtonPressed(BC_M))
		g_messageDispatcher.DumpDebug();

	if (g_input.ButtonPressed(BC_L))
		g_messageDispatcher.ToggleLog();

	if (g_input.ButtonPressed(BC_O))
		g_debugOutput.ToggleLog();

	if (g_input.ButtonPressed(BC_A))
		g_session.ToggleSessionTimeDebug();

	if (g_input.ButtonPressed(BC_Z))
		g_messageDispatcher.SendMessageToAll("ToggleActorSpy", NULL,
				INVALID_OBJECT_HANDLE);

	if (g_input.ButtonPressed(BC_K))
	{
		HealthIF *health = HealthIF::GetInterface(g_players.GetPlayer(0));
		if (health)
			health->DoDamage(110.0f, HealthIF::FOR_FUN, INVALID_PARTICIPANT);
	}

#ifndef DIRECTX_PC
	if (g_input.ButtonPressed(BC_E))
		g_errorStack.SetError("Debug the Error Monitor...", "A Fatal Error occured because you pressed whatever button triggers a make believe Fatal Error.", CErrorStack::ERROR_FATAL, 5000 );

	if (g_input.ButtonPressed(BC_W))
	{
		if ((d_errorHandle == ERROR_HANDLE_INVALID) ||
				!g_errorStack.ClearError(d_errorHandle))
			d_errorHandle = g_errorStack.SetError("Debug", "Warning Error",
					CErrorStack::ERROR_WARNING, ERROR_INFINITE_LIFETIME, "Ok");
		else
			d_errorHandle = ERROR_HANDLE_INVALID;
	}
#endif

	if (g_input.ButtonPressed(BC_D))
		g_messageDispatcher.SendMessageToAll("ToggleDrawDeadReckoning", NULL, INVALID_OBJECT_HANDLE);

	if (g_input.ButtonPressed(BC_J))
		g_messageDispatcher.SendMessageToAll("DumpReferences", NULL, INVALID_OBJECT_HANDLE);

	// See if the selected actor is still valid. If d_select is a valid handle,
	// it referred to an active actor the last time it was checked. If it no
	// longer refers to an active actor, the actor went away.
	CActor* selectedActor = CActor::FromHandle(d_select);
	if (d_select.Valid() && (selectedActor == NULL))
	{
		g_console.Echo("Deselect deleted actor.\n");
		d_select = INVALID_OBJECT_HANDLE;
	}

	if (g_input.ButtonPressed(BC_N) && selectedActor)
		g_messageDispatcher.SendMessageToActor("ToggleDeadReckoningDebug",
				NULL, INVALID_OBJECT_HANDLE, selectedActor->GetHandle());

	// Try to select an actor if in free camera.
	ControlMapper* mapper = g_controlFocus.Find("FreeCam", 0);
	if ((mapper != NULL) && mapper->HasFocus())
	{
		if (((DefaultControlMapper*)mapper)->ButtonDown(
				DefaultControlMapper::BUTTON_X))
		{
			// Trying to select something other than an actor clears the selection.
			CActor* newSelection = NULL;

			// Setup and run the test. Performed this way, non-protagonists and
			// actors that don't respond to queries but are drawn will be detected.
			CLineEntity entity;
			Camera &camera = g_cameraDirector.CurrentCamera();
			entity.Category() |= DEBUG_DRAW_CATEGORY;
			entity.Line().SetOriginBasis(camera.Eye(), camera.Dir());
			entity.Line().SetMaxBoundary(20.0f);
			entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
			g_collisionSystem.ResetStack();
			CCollisionList* list = g_collisionSystem.Test(&entity,
					&g_stage.MasterCast());

			// The collision list (if any) will contain all of collision events
			// generated during the test. The modifier (if any) contains the
			// closest intersection. Find the event corresponding to the closest
			// intersection.
			if ((list != NULL) && (entity.Line().Modifier() != NULL))
			{
				CCollisionEvent* event = list->GetHeadEvent();
				while(event != NULL)
				{
					CCollisionPair* pair = event->GetHeadPair();
					while(pair != NULL)
					{
						if (pair->GetIntersection() == entity.Line().Modifier())
						{
							CStageEntity* entity = pair->GetEvent()->GetTargetEntity();
							if (entity->IsA(CActor::ClassType()))
								newSelection = (CActor*)entity;
							break;
						}
						pair = pair->GetNext();
					}
					if (pair != NULL)
						break;
					event = event->GetNext();
				}
			}
			if (newSelection != selectedActor)
			{
				if (selectedActor != NULL)
				{
					g_messageDispatcher.SendMessageToActor("ClearDeadReckoningDebug",
							NULL, INVALID_OBJECT_HANDLE, selectedActor->GetHandle());
					g_console.Echo("Deselect %s(%s)\n", selectedActor->Name(),
							selectedActor->InstanceName());
				}
				if (newSelection != NULL)
				{
					g_console.Echo("Select %s(%s)\n", newSelection->Name(),
							newSelection->InstanceName());
					d_select = newSelection->GetHandle();
				}
				else
					d_select = INVALID_OBJECT_HANDLE;
				selectedActor = newSelection;
			}
		}
	}

	// Draw a sphere around the selected actor.
	if (selectedActor != NULL)
	{
		ts_BoundingBox &box = *selectedActor->Instance()->ModelBoundingBox();
		Vector3 center = selectedActor->GetBodyInWorld() + 0.5f * (box.Min + box.Max);
		Vector3 corner = 0.5 * (box.Max - box.Min);
		DrawUtility::Sphere(center, DrawUtility::WHITE, corner.Length());
		WriteDebugMessage( 3, selectedActor->InstanceName());
	}

	// Allow the user to drop an avatar where the free cam is
	ControlMapper* freeMapper = g_controlFocus.Find("FreeCam", 0);
	if ((freeMapper != NULL) && freeMapper->HasFocus())
	{
		if (((DefaultControlMapper*)freeMapper)->ButtonDown(
			DefaultControlMapper::BUTTON_TRI))
		{
			Camera &camera = g_cameraDirector.CurrentCamera();
			CActor *avatar = g_players.GetPlayer(0);
			if (avatar != NULL)
			{
				avatar->d_bodyInWorld = camera.Eye();
				Cheats::SignalExternalCheat();
			}
		}
	}

#endif //CONSUMER_BUILD
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::CollisionDebug(void)
{
#ifndef CONSUMER_BUILD
	if (!g_cameraDirectorPtr)
		return;

	Camera &camera = g_cameraDirector.CurrentCamera();

	if (!d_collisionFrozen)
	{
		d_collisionDir = camera.Dir();
		d_collisionEyepoint = camera.Eye();
	}

	if (d_surfaceDebug)
	{
		// Get the current camera
		CLineQuery test;
		CLineEntity entity;
		entity.Line().SetOriginBasis(d_collisionEyepoint, -Display__GetFarClip() * d_collisionDir);
		entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		g_collisionSystem.ResetStack();
		if (test.Test(entity))
		{
			WriteDebugMessage( 3, Printf("ColXYZ: %.1f %.1f %.1f", (float)test.intersection.x(), (float)test.intersection.y(), (float)test.intersection.z()) );

			if (test.actor)
				WriteDebugMessage( 5, Printf("Actor: %s", ((CActor*)test.actor)->InstanceName()) );
			else
				WriteDebugMessage( 5, Printf("Static collision data"));

			if (test.material == INVALID_MATERIALINDEX)
				WriteDebugMessage( 4, Printf("Material: %s", "Invalid Surface") );
			else
				WriteDebugMessage( 4, Printf("Material: %s", g_surfaceType.GetName(test.material)) );
			DrawUtility::NormalPlane(test.intersection, test.normal,
					DrawUtility::YELLOW, DrawUtility::GREEN);
		}
		else
			WriteDebugMessage( 3, "No Surface" );
	}

	// Anything to do?
	if (d_collisionDebug == 0)
		return;

#ifndef OCTREELINEDEBUG
	// Find the location of a point directly in front of the camera.
	Vector3 center(0.0f, 0.0f, -5.0f);
	center = center * camera.ViewToWorld();

	// Define a box.
	Vector3 extent(5.0f, 5.0f, 5.0f);

	CAabbEntity entity;
	entity.Category() |= DEBUG_DRAW_CATEGORY;
	entity.Aabb().GetAABB().Set(center - extent, center + extent);
#else
	CLineEntity entity;
	entity.Category() |= DEBUG_DRAW_CATEGORY;
	entity.Line().SetOriginBasis(d_collisionEyepoint, d_collisionDir);
	entity.Line().SetMaxBoundary(1000.0f);
	entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	entity.Line().SetFindMode(CLineCollisionObject::FIND_ALL);
#endif
	g_collisionSystem.ResetStack();
	switch (d_collisionDebug)
	{
	 case 1:
		entity.SetPositiveExclude(TERRAIN_CATEGORY);
		g_collisionSystem.Draw(&entity, &g_stage.MasterCast(),
				CStageEntity::USE_COMPLEX);
		break;
	 case 2:
		g_collisionSystem.Draw(&entity, &g_stage.MasterCast(),
				CStageEntity::USE_COMPLEX);
		break;
	 case 3:
		OctTreeLineIterator::ClearTrace();
		OctTreeLineIterator::StartTrace();
		entity.SetPositiveInclude(TERRAIN_CATEGORY);
		g_collisionSystem.Draw(&entity);
		OctTreeLineIterator::EndTrace();
		break;
	 case 4:
		entity.SetPositiveInclude(TERRAIN_CATEGORY);
		CCollisionList* list = g_collisionSystem.Test(&entity);
		if (list)
		{
			CCollisionPair* pair = list->GetHeadEvent()->GetHeadPair();
			CAabbToTerrainIntersection* intersection =
					(CAabbToTerrainIntersection*)pair->GetIntersection();
			CTriangleListCollisionObject &triangles =
					intersection->GetTriangleList();
			triangles.Draw();
			triangles.DrawNormals();
		}
		break;
	}

	g_collisionSystem.TerrainEntity()->DrawSuspects();

#endif // CONSUMER_BUILD
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::CSyncTimer::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
#ifndef CONSUMER_BUILD
	d_ref->s_consoleTimerSpeed = g_timer.ConvertSpeed(g_timer.GetSetSpeed());
#endif
}

/***************************************************************************/
// Handle timer updates
/***************************************************************************/
void CTTLDebug::HandleTimerChanges(void)
{
#ifndef CONSUMER_BUILD
	CMasterTimer::ETimerSpeed timerSpeed;
	timerSpeed = g_timer.ConvertSpeed(s_consoleTimerSpeed);
	if (timerSpeed != g_timer.GetSetSpeed())
		d_ttlLoop->SetTimerSpeed(timerSpeed);
#endif
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::CDialogCommand::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
#ifndef CONSUMER_BUILD
	if (d_ref->d_debugOutputPopup != NULL)
	{
		int command = (int)i_data;

		if (command == COMMAND_DEBUG_DONE)
		{
			g_FrontEnd.ShowPopupDialog(false);
			d_ref->d_debugOutputPopup = NULL;
			return;
		}

		if (command == COMMAND_DEBUG_SHOW)
		{
			if (g_FrontEnd.GetDebugOutput().GetShowCategories())
			{
				g_FrontEnd.GetDebugOutput().SetShowCategories(false);
				d_ref->d_debugOutputPopup->SetMenuItemText("Show",
						COMMAND_DEBUG_SHOW);
			}
			else
			{
				g_FrontEnd.GetDebugOutput().SetShowCategories(true);
				d_ref->d_debugOutputPopup->SetMenuItemText("Hide",
						COMMAND_DEBUG_SHOW);
			}
			return;
		}

		if (command == COMMAND_DEBUG_SELECT_ALL)
			g_FrontEnd.GetDebugOutput().SelectAllCategories();

		else if (command == COMMAND_DEBUG_DESELECT_ALL)
			g_FrontEnd.GetDebugOutput().DeselectAllCategories();

		else
		{
			command -= COMMAND_DEBUG_CATEGORY_BASE;
			if ((command >= 0) || (command < g_debugOutput.GetNumCategories()))
			{
				bool selected =
						g_FrontEnd.GetDebugOutput().IsCategorySelected(command);
				if (selected)
					g_FrontEnd.GetDebugOutput().DeselectCategory(command);
				else
					g_FrontEnd.GetDebugOutput().SelectCategory(command);

				char item[40];
				sprintf(item, "[%s] %s", selected ? "  " : "X",
						g_debugOutput.GetCategoryName(command));
				d_ref->d_debugOutputPopup->SetMenuItemText(item,
						COMMAND_DEBUG_CATEGORY_BASE + command);
			}
			return;
		}

		for (int i = 0; i < g_debugOutput.GetNumCategories(); i++)
		{
			char item[40];
			sprintf(item, "[%s] %s",
					g_FrontEnd.GetDebugOutput().IsCategorySelected(i) ?
					"X" : "  ", g_debugOutput.GetCategoryName(i));
			d_ref->d_debugOutputPopup->SetMenuItemText(item,
					COMMAND_DEBUG_CATEGORY_BASE + i);
		}
	}
#endif
}

/***************************************************************************/
/***************************************************************************/
void CTTLDebug::CDialogCancel::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
#ifndef CONSUMER_BUILD
	if (d_ref->d_debugOutputPopup != NULL)
	{
		g_FrontEnd.ShowPopupDialog(false);
		d_ref->d_debugOutputPopup = NULL;
	}
#endif
}

#if !defined(CDROM) && defined(MEM_DEBUG_BLOCKS)
/***************************************************************************/
// show a detailed (and hopefully artist-understandable) memory report for
// the level just loaded
/***************************************************************************/
void CTTLDebug::ShowMemoryReport(void)
{
#ifndef CONSUMER_BUILD
	extern void memDumpBlockInfo( const char* filename );
	static int count = 0;
	char buf[ 32 ];
	sprintf( buf, "dump-%s-%d", g_gameEnvironment.GetWorldFileName(), count++ );
	memDumpBlockInfo( buf );
#endif
}
#endif //CDROM
