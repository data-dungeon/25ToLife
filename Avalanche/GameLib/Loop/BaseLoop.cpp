/*g_
**
**  File:   BaseLoop.cpp
**  Date:   April 7, 2004
**  By:     Bryant Collard
**  Desc:   A gameloop that handles base functionality.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/BaseLoop.cpp $
**  $Revision: #55 $
**  $DateTime: 2005/07/22 23:55:36 $
**    $Author: Food $
**
*/

// Precompiled header.
#include "Loop/LoopPCH.h"

// My header
#include "Loop/BaseLoop.h"

// Non-precompiled headers.
#include "Loop/Environment.h"
#include "Layers/Media.h"
#include "Layers/Tasking.h"
#include "GameHelper/FileLoader.h"
#include "GameHelper/ParamMgr.h"
#include "GameHelper/ErrorStack.h"
#include "EngineHelper/filestream.h"
#include "hud/animator.h"
#include "hud/font.h"
#include "hud/sprite.h"
#include "hud/hudobject.h"
#include "Display/Common/DisplayState.h"
#include "Game/Managers/RenderMgr.h"
#include "Game/Managers/ShadowMgr.h"
#include "Game/Managers/ParticleMgr.h"
#include "Game/Misc/GameEngine.h"
#include "Game/Database/lightset.h"
#include "Game/Database/DBFile.h"
#include "Game/Database/World.h"
#include "cmdcon/cmdcon.h"
#include "Effects/scenesetup.h"
#include "Effects/ssemgr.h"
#include "Effects/sseffect.h"
#include "Effects/halo.h"
#include "Effects/lensflare.h"
#include "Effects/bloom.h"
#include "Effects/dropshad.h"
#include "Effects/footstep.h"
#include "Effects/ahfmgr.h"
#include "Effects/shatter.h"
#include "Effects/Water/WaterMgr.h"
#include "Effects/sfx/sfxsys.h"
#include "hud/ipanel/isys.h"
#include "camera/projection.h"
#include "path/PathCollection.h"
#include "EngineHelper/sfx.h"
#include "WatchClient/WatchClient.h"
#if defined(PS2) || defined(DIRECTX_PC)
#include "Display/Common/DisplayPlatform.h"
#endif

#if defined (_XBOX) && defined (PROFILE)
#pragma pack(push, 8)       // Make sure structure packing is set properly
#include <d3d8perf.h>
#pragma pack(pop)
#endif

CLoopScenes CBaseLoop::d_scenes;
CEnvironment* CBaseLoop::d_environment;

// Initialize function array.
SLoopEntry CBaseLoop::stateArray[CBaseLoop::NUM_STATES] =
{
	"INITIALIZE", (LoopFunc)&CBaseLoop::Initialize,
	"RUN",        (LoopFunc)&CBaseLoop::Run,
	"TERMINATE",  (LoopFunc)&CBaseLoop::Terminate
};

/***************************************************************************/
/***************************************************************************/
CBaseLoop::CBaseLoop() : GameLoop(stateArray, UNINITIALIZED)
{
	d_environment = NULL;
}

/***************************************************************************/
/***************************************************************************/
CBaseLoop::CBaseLoop(SLoopEntry* i_stateArray, int i_loopState) :
	GameLoop(i_stateArray, i_loopState)
{
	d_environment = NULL;
}

/***************************************************************************/
/***************************************************************************/
CBaseLoop::~CBaseLoop()
{
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::BeginBaseInitialize(void)
{
	// Nothing to see here
	SceneSetup::Reset();
	SceneSetup::Enable(false);

	// Initialize the parameter manager.
	if ( !ValidSingleton( CParamMgr ) )
	{
		NewSingleton(CParamMgr);
	}

	// Make sure the control stacks are clean.
	g_controlFocus.ClearStacks();
}

/***************************************************************************/
/***************************************************************************/
bool CBaseLoop::BaseLoadEnvironment(CEnvironment* i_environment, int i_numScenes)
{
	// Initialize drop shadows
	if (!ValidSingleton(DropShadow))
	{
		NewSingleton(DropShadow);
		g_dropShadow.Initialize("shadow\\shadow.dbl");
	}

	if (!ValidSingleton(FootstepSystem))
	{
		NewSingleton(FootstepSystem);
		g_footsteps.Initialize("footstep\\footstep.dbl", 128, 0xffffffff, 0xffffffff);
	}

	// The shatter system (does no work)
	if (!ValidSingleton(ShatterOverlord))
		NewSingleton(ShatterOverlord);

	// Initialize the screen space effects
	SSEffectsMgr::Initialize();
	Halo::Initialize();
	LensFlare::Initialize();

	// Instrument system (HUD)
	NewSingleton(InstrumentSystem);
	
	// Water
	NewSingleton(AniHFMgr);
	NewSingleton(WaterMgr);

	// Generic Special FX system
	NewSingleton(SpecialFXSystem);

//	// Pre-load the files.
//	g_fileLoader.LoadWorldFiles(i_environment->WorldName());

	// Load the world.
	g_loadSOM.Clear();
	memSetUseBit(MEM_WORLD_DATA);
	bool worldLoaded = g_loadSOM.World(i_environment->WorldRoot());
	memClearUseBit(MEM_WORLD_DATA);
	if (!worldLoaded)
		return false;

	// Do the work for the scene setup
	SceneSetup::Load("levels\\", i_environment->WorldPath());

	// Initialize camera director, create scenes, setup the scene tree, etc.
	d_scenes.Initialize(i_numScenes);

	// Load the light set.
	g_lightSet.loadLightSet();

	// Load the halos
	g_halo.LoadSprites("flares\\halos.dbl");
	g_halo.LoadHalosFromDBL(g_loadSOM.GetWorldHandle());

	// Do the bloom
	NewSingleton(CheapBlooms);
	g_bloom.LoadEffects("flares\\bloom.dbl");
	g_bloom.DoWorldDBL(g_loadSOM.GetWorldHandle());

	// Load lens flare. Only load textures if flare definitiaon file exists,
	// otherwise there will be no flare.
	if (Media::FileExists(i_environment->WorldPath("flare.var", NULL)))
	{
		g_lensFlare.LoadSprites("flares\\flares.dbl");
		g_lensFlare.Create(i_environment->WorldPath("flare", NULL));
		g_lensFlare.Hide();
	}

	// Load particles
	char particleFile[128];
	ParticleMgr__LoadData("particles\\TTLUniversal.dbl");
	//ParticleMgr__LoadData("particles\\TTLWater.dbl");
	sprintf(particleFile, "particles\\%s.dbl", d_environment->WorldName());
	ParticleMgr__LoadData(particleFile);

	return(true);
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::EndBaseInitialize(void)
{
#if defined(PS2)
	// Clamp the frame-rate to a maximum of 30 Hz.
	RenderMgr__SetNumVBlanksToSyncTo(2);
#elif defined(WIN32)
	// Allow PC to run as fast as possible.
	// RenderMgr__ToggleVSync();
#endif

	// Start the clock!
	g_timer.Reset();
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::BeginBaseRun(void)
{
//	// Update hud.
//	Animator::UpdateAll(g_timer.GetOSFrameSec());

	// Run the tasking system.
	tskDispatchLoop(g_timer.GetOSFrameSec(), g_timer.GetFrameSec(),
			g_timer.IsPaused());
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::EndBaseRun(void)
{
	// Let the sound system run
	Sfx::Daemon();

	// Update the cameras.
	d_scenes.Run();

	// Get the camera matrix
	Graphics4x4 &cameraMatrix = g_cameraDirector.GetCameraMatrix();
	
	// Update drop shadows
	g_dropShadow.Update(cameraMatrix);
	g_footsteps.Update(cameraMatrix);

	// Update shatters
	g_shatter.Update();

	// Update the light set.
	g_lightSet.updateLightSet();

	// Water
	g_aniHFMgr.Update(cameraMatrix);
	g_waterMgr.Update(cameraMatrix);
	
	// Audio pitch
	g_audio.InferMasterPitch(g_timer.GetSpeedFactor());
	
	// Generic fx
	g_sfx.Update();
}	

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::BaseRender(void)
{
	// Update the instrument system
	if (ValidSingleton(InstrumentSystem))
		g_instruments.Update();

	// Update the screen space effects.
	if (g_cameraDirectorPtr && g_ssEffectsPtr)
	{
		ProjectionController &projection = g_cameraDirector.GetCurrentProjection();
		ScreenSpaceEffect::SetTransform(projection.FOV(), projection.AspectRatio(),
				projection.Hither(), projection.Yon());
		g_ssEffects.Update();
	}

	if (g_cameraDirectorPtr && ValidSingleton(CheapBlooms))
		g_bloom.Update();

	// Update the scene setup
	SceneSetup::Enable(true);
	SceneSetup::Update();

	// Draw stuff.
	Profiler__Start(PROFILE_ENGINE_ADVANCE);
	GameEngine__Advance();
	Profiler__Stop(PROFILE_ENGINE_ADVANCE);
	Sprite::DrawAll();
	Profiler__Start(PROFILE_RENDER_MANAGER);
	// only render to the backbuffer when we need to (cause it's slow right now)
#ifdef DIRECTX_PC
	RenderMgr__Render(RenderToTexture::isRenderingToTextureOn);
#else
	RenderMgr__Render();
#endif
	Profiler__Stop(PROFILE_RENDER_MANAGER);

	#if defined (_XBOX) && defined (PROFILE)
	if ( D3DPERF_QueryRepeatFrame() )
	{
		Sprite::DrawAll();
		RenderMgr__Render();
	}
	#endif
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::BaseUnloadEnvironment(void)
{
	// Dump particles.
	ParticleMgr__Reset();

	// Unload the light set.
	g_lightSet.unLoad();

	// Unload environment.
	g_loadSOM.Clear();
	FileStream::CleanupPreLoad();

	// Shatter
	DeleteSingleton(ShatterOverlord);

	// Cleanup drop shadows
	g_dropShadow.Terminate();
	DeleteSingleton(DropShadow);

	// Footsteps
	g_footsteps.Terminate();
	DeleteSingleton(FootstepSystem);
	
	// Special fx
	DeleteSingleton(SpecialFXSystem);

	// Water
	DeleteSingleton(AniHFMgr);
	DeleteSingleton(WaterMgr);

	// Instruments
	DeleteSingleton(InstrumentSystem);
	
	// Bloom
	DeleteSingleton(CheapBlooms);

	// Cleanup screen space effects.
	Halo::Terminate();
	LensFlare::Terminate();
	SSEffectsMgr::Terminate();

	// Clean up scenes.
	//SceneMgr::RemoveAllInstances();
	d_scenes.Terminate();

	// Clean up the texture cache
	TextureMgr::Cache().Reset();

	// Watch stuff
	#ifdef ENABLE_WATCHCLIENT
	WatchClient::Instance()->Reset();
	#endif //ENABLE_WATCHCLIENT

	if ( ValidSingleton( CParamMgr ) )
		GetSingleton(CParamMgr).Clear();
	g_console.Clear();
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::BaseTerminate(void)
{
	// Cleanup command console.
	if ( ValidSingleton( CParamMgr ) )
	{
		DeleteSingleton(CParamMgr);
		g_console.Clear();
	}

	// Shut down any outstanding tasks.
	tskKillClass(0, 0);
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::BaseReset(void)
{
	// Cleanup hud.
	HudObject::UnregisterAllClasses();
	SpriteInterface::UnregisterAllClasses();
	Font::FlushAllFonts();

	// Clear out the error stack.
	g_errorStack.ClearAllErrors();

	// Purge the message dispatcher.
	g_messageDispatcher.Purge();

	// Reset the engine.
	GameEngine__Reset();
}

/***************************************************************************/
/***************************************************************************/
int CBaseLoop::DetermineState(int i_state)
{
	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
		return(INITIALIZE);
	 case INITIALIZE:
		if (!d_initSucceeded)
			return(TERMINATE);
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
void CBaseLoop::Initialize(void)
{
	BeginBaseInitialize();
	d_initSucceeded = BaseLoadEnvironment(d_environment, 1);
	ASSERT(d_initSucceeded);

	d_scenes.CreateFreeCameras();
	d_scenes.ActivateFreeCamera(0);

	EndBaseInitialize();
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::Run(void)
{
	BeginBaseRun();
	EndBaseRun();
	BaseRender();
}

/***************************************************************************/
/***************************************************************************/
void CBaseLoop::Terminate(void)
{
	d_scenes.DeleteFreeCameras();
	BaseUnloadEnvironment();
	BaseTerminate();
}
