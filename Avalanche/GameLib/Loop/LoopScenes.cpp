/*
**
**  File:   LoopScenes.cpp
**  Date:   April 2, 2004
**  By:     Bryant Collard
**  Desc:   Manage scenes.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/LoopScenes.cpp $
**  $Revision: #9 $
**  $DateTime: 2005/06/09 12:44:19 $
**    $Author: Tyler_Colbert $
**
*/

// Precompiled header.
#include "Loop/LoopPCH.h"

// My header.
#include "Loop/LoopScenes.h"

// Non-precompiled headers.
#include "Game/Database/SceneNode.h"
#include "Game/Database/DBFile.h"
#include "Game/Database/MeshLOD.h"

// For the free camera utility.
#include "platform/defcontrolmap.h"
#include "camera/controlcam/controlcam.h"
#include "camera/collidablecontrolcam/collidablecontrolcam.h"

#ifndef CONSUMER_BUILD
#include "Layers/IMM/IMM.h"
#endif

/***************************************************************************/
/***************************************************************************/
CLoopScenes::CLoopScenes()
{
	d_numScenes = 0;
	d_scene = NULL;
}

/***************************************************************************/
/***************************************************************************/
CLoopScenes::~CLoopScenes()
{
	delete[] d_scene;
}

/***************************************************************************/
/***************************************************************************/
void CLoopScenes::Initialize(int i_numScenes)
{
	// Initialize the camera director.
	CameraDirector::Initialize();

	// Make sure we are clean.
	if ( ( d_scene != NULL ) && ( d_numScenes != 0 ) )
	{
		dbgPrint( "CLoopScenes::Initialize called when already initialized.\n" );
		return;
	}

	if (i_numScenes < 1)
		return;

	// Initialize the camera and default mesh LODs.
	SetSingleton(CCameraLOD, &d_cameraLOD);
	g_cameraLOD.LoadTable("ActorLOD");
	g_defaultMeshLOD.Initialize(i_numScenes, "MeshLOD");

	// Allocate space for the scenes.
	d_numScenes = i_numScenes;
	d_scene = new CameraScene*[i_numScenes];
	ASSERT_PTR(d_scene);

	// Setup the scenes.
	for (int i = 0; i < d_numScenes; i++)
	{
		// Create the scene.
		d_scene[i] = g_cameraDirector.CreateScene();
		ASSERT_PTR(d_scene[i]);

		// Enable the scene by default.
		d_scene[i]->Scene()->EnableForRendering( true );

		// Set the camera and default mesh LOD cameras.
		g_cameraLOD.SetCameraN(i, &d_scene[i]->CameraMatrix());
		g_defaultMeshLOD.SetCamera(i, &d_scene[i]->CameraMatrix());

		// Make the player a listener
		g_audio.CreateListener(&d_scene[i]->CameraMatrix());
	}

	// Make the first scene the default.
	g_cameraDirector.SetDefaultScene(d_scene[0]);

	// Setup the scene tree. (A scene needs to have been created.)
	CSceneNode* sceneTree = (CSceneNode*)dblGetChunkData(DB_SCENE_TREE,
			DEFAULT_CHUNK_KEY_STRING, g_loadSOM.GetWorldHandle());
	if (sceneTree == NULL)
		sceneTree = SceneMgr::EmptySceneTree();
	ts_RecordList* visRegionList = (ts_RecordList*)dblGetChunkData(
			DB_VIS_REGIONS, DEFAULT_CHUNK_KEY_STRING, g_loadSOM.GetWorldHandle());
	SceneMgr::SetSceneTree(sceneTree, visRegionList);
}

/***************************************************************************/
/***************************************************************************/
void CLoopScenes::Run(void)
{
	if (g_cameraDirectorPtr)
		g_cameraDirector.Update();
}

/***************************************************************************/
/***************************************************************************/
void CLoopScenes::Terminate(void)
{
	delete[] d_scene;
	d_scene = NULL;
	d_numScenes = 0;
	CameraDirector::Terminate();
	ClearSingleton(CCameraLOD);
	g_defaultMeshLOD.Terminate();
	g_audio.DestroyListeners();
}

/***************************************************************************/
/***************************************************************************/
void CLoopScenes::CreateFreeCameras(void)
{
	for (int i = 0; i < d_numScenes; i++)
	{
		// Build a control mapper for the camera.
		DefaultControlMapper *control = new DefaultControlMapper("FreeCam");
		ASSERT_PTR(control);
		g_controlFocus.Add(control, i);

		// Build a camera group and add it to the scene.
		CameraGroup* group = new CameraGroup("FreeCam");
		ASSERT_PTR(group);
		d_scene[i]->AddGroup(group);

		// Build a camera and add it to the camera group.
		ControlCamera *camera = new ControlCamera("FreeCam", *control);
//	collidable version of the free cam	CollidableControlCamera *camera = new CollidableControlCamera("FreeCam", *control);
		ASSERT(camera);
		group->AddTail(camera);
		group->Select(camera);

		#ifndef CONSUMER_BUILD
		IMM::CreateCamera(d_scene[i]);
		#endif
	}
}

/***************************************************************************/
/***************************************************************************/
void CLoopScenes::DeleteFreeCameras(void)
{
	for (int i = 0; i < d_numScenes; i++)
	{
		// Remove the group from the scene. This will delete both the camera and
		// the group.
		CameraGroup* group = d_scene[i]->FindGroup("FreeCam");
		d_scene[i]->RemoveGroup(group);

		// Remove the controller. This will delete it.
		g_controlFocus.Remove("FreeCam", i);
	}
}

/***************************************************************************/
/***************************************************************************/
bool CLoopScenes::ActivateFreeCamera(int i_scene)
{
	// Make sure the scene exists.
	if ((i_scene < 0) || (i_scene >= d_numScenes))
		return false;

	// Get the mapper
	ControlMapper* mapper = g_controlFocus.Current(i_scene);
	if (!mapper)
		return false;
		
	// Do we have control focus?
	bool freecamControlFocus = (strcmp(mapper->Name(), "FreeCam") == 0);
	bool avatarControlFocus = (strcmp(mapper->Name(), "Avatar") == 0);

	// What is our current camera
	Camera& camera = g_cameraDirector.CurrentCamera(d_scene[i_scene]);
	bool inFreeCam = (strcmp(camera.Name(), "FreeCam") == 0);

	// Only transition from avatar
	if (!avatarControlFocus && !freecamControlFocus)
		return inFreeCam;

	enum FreecamState
	{
		NOTACTIVE,
		WITHCONTROL,
		NOCONTROL
	};
	// what is our current state
	FreecamState state;
	if (inFreeCam)
		state = freecamControlFocus ? WITHCONTROL : NOCONTROL;
	else
		state = NOTACTIVE;

	// Go to the next state
	if (state == NOTACTIVE) 
		state = WITHCONTROL;
	else if (state == WITHCONTROL) 
		state = NOCONTROL;
	else 
		state = NOTACTIVE;

	// What do we do for this state
	switch (state)
	{
		case NOTACTIVE:
			if (inFreeCam)
				d_scene[i_scene]->PopGroup("FreeCam");
			break;
		case WITHCONTROL:
			d_scene[i_scene]->PushGroup("FreeCam");
			g_controlFocus.Push("FreeCam", i_scene);
			break;
		case NOCONTROL:
			if (freecamControlFocus)
				g_controlFocus.Pop("FreeCam", i_scene);
			break;
	}

	return (state != NOTACTIVE);
}
