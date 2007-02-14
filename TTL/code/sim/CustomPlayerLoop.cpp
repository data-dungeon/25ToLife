/********************************************************************
	$File: //depot/TTL/code/sim/CustomPlayerLoop.cpp $
	$Revision: #34 $
	$DateTime: 2005/08/24 10:05:31 $
	$Author: Rob $
	purpose: Implement the initial game loop.
*********************************************************************/
#include "TTLPCH.h"
#include "sim/CustomPlayerLoop.h"
#include "frontend/frontend.h"
#include "frontend/customplayer.h"
#include "effects/pain_decal.h"
#include "main/PlayerDB.h"
#include "camera/CameraPCH.h"
#include "GameHelper/ErrorStack.h"

#define CAMERA_DISTANCE			2.4f
#define CAMERA_FOCUS_HEIGHT	0.2f

//============================================================================

SLoopEntry CCustomPlayerLoop::s_stateArray[ CCustomPlayerLoop::NUM_STATES ] =
{
	"INITIALIZE",           (LoopFunc) &CCustomPlayerLoop::Initialize,
	"RUN_SETTINGS",         (LoopFunc) &CCustomPlayerLoop::Run,
	"SHUTDOWN_CHARACTER",   (LoopFunc) &CCustomPlayerLoop::ShutdownCharacter,
	"INITIALIZE_CHARACTER", (LoopFunc) &CCustomPlayerLoop::InitializeCharacter,
	"RUN_CHARACTER",        (LoopFunc) &CCustomPlayerLoop::Run,
	"TERMINATE",            (LoopFunc) &CCustomPlayerLoop::Terminate
};

//============================================================================

CCustomPlayerLoop::CCustomPlayerLoop() :
	GameLoop(s_stateArray, UNINITIALIZED),
	d_messageOwner("CustomPlayerLoop")
{
}

//============================================================================

CCustomPlayerLoop::~CCustomPlayerLoop()
{
}

//============================================================================

int CCustomPlayerLoop::DetermineState( int i_state )
{
	if (g_errorStack.IsFatal())
		d_mode = MODE_EXIT;

	switch ( i_state )
	{
	case UNINITIALIZED:
		d_mode = MODE_SETTINGS;
		return INITIALIZE;

	case INITIALIZE:
	case RUN_SETTINGS:
	case SHUTDOWN_CHARACTER:
		switch(d_mode)
		{
		 case MODE_CHARACTER:
			return INITIALIZE_CHARACTER;
		 case MODE_EXIT:
			return TERMINATE;
		}
		return RUN_SETTINGS;

	case INITIALIZE_CHARACTER:
	case RUN_CHARACTER:
		if (d_mode != MODE_CHARACTER)
			return SHUTDOWN_CHARACTER;
		return RUN_CHARACTER;

	case TERMINATE:
		return UNINITIALIZED;

	default:
		ASSERT( false );
		return UNINITIALIZED;
	}
}

//============================================================================

void CCustomPlayerLoop::Initialize()
{
	// Register message handlers.
	m_ExitSettings_Handler.Initialize("ExitSettings",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	m_StartCustomPlayer_Handler.Initialize("StartCustomPlayer",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	m_EndCustomPlayer_Handler.Initialize("EndCustomPlayer",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	m_AcceptInvitation_Handler.Initialize("AcceptInvitation",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

	g_FrontEnd.ClearScreenHistory();
	g_FrontEnd.GotoScreen( "SCREEN_ID_MYSETTINGS" );
}

//============================================================================

void CCustomPlayerLoop::Run()
{
}

//============================================================================

void CCustomPlayerLoop::InitializeCharacter()
{
	// Tell the path follower camera to suspend.
	g_messageDispatcher.SendMessageToAll( "Suspend", NULL, INVALID_OBJECT_HANDLE );

	// Get the scene.
	CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();

	// Build a camera group and add it to the scene.
	CameraGroup* group = new CameraGroup( "CreatePlayerGroup" );
	ASSERT_PTR( group );
	cameraScene->AddGroup( group );

	// Custom player camera
	GeneralCamera *customCamera = new GeneralCamera( "Custom" );
	ASSERT_PTR( customCamera );
	customCamera->SetMode( GeneralCamera::CAMMODE_FOCAL_POINT );
	group->AddTail( customCamera );
	group->Select( customCamera );

	// Push the group.
	cameraScene->PushGroup( group );

	// Find the camera for the custom player
	CustomPlayer::SetCamera( (GeneralCamera*) &g_cameraDirector.CurrentCamera() );

	// Initialize the custom player interface
	CustomPlayer::Intro();
}

//============================================================================

void CCustomPlayerLoop::ShutdownCharacter()
{
	// clean up
	CustomPlayer::Exit();

	// Dump the camera group.
	CustomPlayer::SetCamera( NULL );
	CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();
	CameraGroup* group = cameraScene->PopGroup( "CreatePlayerGroup" );
	cameraScene->RemoveGroup( group );

	// Tell the path follower camera to resume.
	g_messageDispatcher.SendMessageToAll( "Resume", NULL, INVALID_OBJECT_HANDLE );
}

//============================================================================

void CCustomPlayerLoop::Terminate()
{
	// clean up
	d_messageOwner.UnregisterAll();
}

//============================================================================

void CCustomPlayerLoop::RequestExit(void)
{
	d_mode = MODE_EXIT;
}

//============================================================================

void CCustomPlayerLoop::ExitSettings_Handler::HandleMessage( void* i_data,
		ActorHandle i_sender, bool i_posted )
{
	d_ref->RequestExit();
}

//============================================================================

void CCustomPlayerLoop::StartCustomPlayer_Handler::HandleMessage( void* i_data,
		ActorHandle i_sender, bool i_posted )
{
	if (d_ref->d_mode != CCustomPlayerLoop::MODE_EXIT)
		d_ref->d_mode = CCustomPlayerLoop::MODE_CHARACTER;
}

//============================================================================

void CCustomPlayerLoop::EndCustomPlayer_Handler::HandleMessage( void* i_data,
		ActorHandle i_sender, bool i_posted )
{
	if (d_ref->d_mode != CCustomPlayerLoop::MODE_EXIT)
		d_ref->d_mode = CCustomPlayerLoop::MODE_SETTINGS;
}

//============================================================================

void CCustomPlayerLoop::AcceptInvitation_Handler::HandleMessage( void* i_data,
		ActorHandle i_sender, bool i_posted )
{
	d_ref->RequestExit();
}

//============================================================================
