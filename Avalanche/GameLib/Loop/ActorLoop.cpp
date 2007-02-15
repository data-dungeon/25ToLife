/*
**
**  File:   ActorLoop.cpp
**  Date:   April 7, 2004
**  By:     Bryant Collard
**  Desc:   A gameloop that adds actor to the base functionality.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/ActorLoop.cpp $
**  $Revision: #23 $
**  $DateTime: 2005/08/11 12:55:25 $
**    $Author: Bryce $
**
*/

// Precompiled header.
#include "Loop/LoopPCH.h"

// My header
#include "Loop/ActorLoop.h"

// Non-precompiled headers.
#include "Loop/Environment.h"
#include "Audio/audiosys.h"
#include "EngineHelper/sfx.h"
#include "collide/collsys.h"
#include "stage/stage.h"
#include "stage/SurfaceType.h"
#include "stage/SurfaceLookup.h"
#include "Script/ScriptSystem.h"
#include "Game/Managers/ParticleMgr.h"
#include "CollideHelper/Particle.h"
#include "Components/ComponentList.h"

// Initialize function array.
SLoopEntry CActorLoop::stateArray[CActorLoop::NUM_STATES] =
{
	"INITIALIZE", (LoopFunc)&CActorLoop::Initialize,
	"RUN",        (LoopFunc)&CActorLoop::Run,
	"TERMINATE",  (LoopFunc)&CActorLoop::Terminate
};

/***************************************************************************/
/***************************************************************************/
CActorLoop::CActorLoop() : CBaseLoop(stateArray, UNINITIALIZED)
{
}

/***************************************************************************/
/***************************************************************************/
CActorLoop::CActorLoop(SLoopEntry* i_stateArray, int i_loopState) :
	CBaseLoop(i_stateArray, i_loopState)
{
}

/***************************************************************************/
/***************************************************************************/
CActorLoop::~CActorLoop()
{
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::BeginActorInitialize(void)
{
	// Begin base initialization.
	BeginBaseInitialize();
}

/***************************************************************************/
/***************************************************************************/
bool CActorLoop::ActorLoadEnvironment(CEnvironment* i_environment,
		int i_numScenes)
{
	// Initialize sounds
	const int channelsFor3D = 30;
	const int channelsForEnvironment = 12;
	Sfx::Initialize3D(i_environment->WorldName(), channelsFor3D, channelsForEnvironment);

	// Load the base environment.
	if (!BaseLoadEnvironment(i_environment, i_numScenes))
		return(false);

	// Initialize the collision system.
	NewSingleton(CollisionSystem);
	if (!g_collisionSystem.Initialize(g_loadSOM.GetWorldHandle()))
	{
		DeleteSingleton(CollisionSystem);
		BaseUnloadEnvironment();
		return(false);
	}
	SetSingleton(CCollisionStack, &g_collisionSystem.GetStack());

	// Do this here since the collision test is part of the collision system.
#ifndef PS2
	ParticleMgr__RegisterCollisionTest(ParticleCollisionTest);
#endif

	// Initialize surface tables
	NewSingleton(CSurfaceType);
	NewSingleton(CSurfaceLookup);
	g_surfaceType.Load(g_loadSOM.GetWorldHandle(), "tables\\SurfType.tbl");
	g_surfaceLookup.Init(&g_surfaceType);
	
	// Setup sound occlusions
	if (!Sfx::LoadMaterialOcclusion())	
		Sfx::EnableOcclusion(Sfx::SFX_NO_OCCLUSIONS);
	else
	{
#ifdef PS2
		Sfx::EnableOcclusion(Sfx::SFX_STREAM3D_OCCLUSIONS);
#else
		Sfx::EnableOcclusion(Sfx::SFX_ALL_OCCLUSIONS);
#endif
	}

	// Create an alias for scripted actors.
	g_audio.AliasGroup("EnvironmentalAI", Sfx::Get3DGroupName());

	// Setup the sounds from the world dbl
	Sfx::SetupSoundEnvironment(g_loadSOM.GetWorldHandle());

	// Initialize the stage. Do it here so the corresponding terminate happens
	// before the sound system is shutdown.
	NewSingleton(Stage);
	g_stage.Initialize();

	return(true);
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::EndActorInitialize(void)
{
	// Load the surface to surface data after all tables have been registered
	// but before any attempt to access data.
	g_surfaceLookup.LoadTables("tables\\SurfSurf.tbd");

	// Optimize all the internals...the environment is setup
	Sfx::DoneSetupSoundEnvironment();

	// Finish base initialization.
	EndBaseInitialize();
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::BeginActorRun(void)
{
	BeginBaseRun();
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::EndActorRun(void)
{
	g_stage.AdvanceSimulation();
	EndBaseRun();
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::ActorRender(void)
{
	BaseRender();
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::ActorUnloadEnvironment(void)
{
	// Delete the stage.
	g_stage.Terminate();
	DeleteSingleton(Stage);

	// Cleanup the component list pool.
	CActorComponentList::Cleanup();

	// Clean up audio.
	g_audio.Flush(Audio::SOFTFLUSH, false);
	g_audio.DestroyGroup("EnvironmentalAI");
	Sfx::Terminate3D();

	// Cleanup surface tables.
	DeleteSingleton(CSurfaceLookup);
	DeleteSingleton(CSurfaceType);

	// Cleanup collision system.
	g_collisionSystem.Terminate();
	ClearSingleton(CCollisionStack);
	DeleteSingleton(CollisionSystem);

	// Unload base environment.
	BaseUnloadEnvironment();
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::ActorTerminate(void)
{
	// Terminate the base.
	BaseTerminate();
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::ActorReset(void)
{
	BaseReset();
}

/***************************************************************************/
/***************************************************************************/
int CActorLoop::DetermineState(int i_state)
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
void CActorLoop::Initialize(void)
{
	BeginActorInitialize();

	d_initSucceeded = ActorLoadEnvironment(d_environment, 1);
	ASSERT(d_initSucceeded);

	d_scenes.CreateFreeCameras();
	d_scenes.ActivateFreeCamera(0);

	EndActorInitialize();
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::Run(void)
{
	BeginActorRun();
	EndActorRun();
	ActorRender();
}

/***************************************************************************/
/***************************************************************************/
void CActorLoop::Terminate(void)
{
	d_scenes.DeleteFreeCameras();
	ActorUnloadEnvironment();
	ActorTerminate();
}
