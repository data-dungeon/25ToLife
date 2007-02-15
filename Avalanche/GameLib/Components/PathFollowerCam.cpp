/**********************************************/
/*                                            */
/* PathFollowerCam.cpp                           */
/* big juju							                */
/* visual c++ 6.0                             */
/*                                            */
/**********************************************/

#include "Components/ComponentsPCH.h"
#include "camera/CameraPCH.h"

#include "Interfaces/MoveIF.h"
#include "Components/PathFollowerCam.h"
#include "Components/PathFollowerCamDefs.h"
#include "Components/ComponentCreator.h"
#include "Game/Misc/ParameterSet.h"
#include "collide/collsys.h"
#include "CollideHelper/ObbEntity.h"
#include "CollideHelper/EventMiner.h"
#include "Math/sfilter.h"
#include "Audio/audiosys.h"


#include "stage/stage.h"

#include "Game/Managers/RenderMgr.h" 
#include "Game/Managers/SpriteMgr.h"
#include "Display/Common/TextureState.h"


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// Define a creator and add it to the creator list.
CUSTOM_COMPONENT_CREATOR(CPathFollowerCam, PathFollowerCam, COMPONENT_DRIVER_PRIORITY
		)::Create(CCompActor &i_actor)
{
	CPathFollowerCam *path = new CPathFollowerCam( i_actor );
	ASSERT_PTR( path );

	return path;
}
 
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CPathFollowerCam::CPathFollowerCam( CCompActor& i_actor ) : CActorComponent( i_actor )
{
	d_timeToReset			= 0.0f;
	d_speed					= 0.0f;
	d_accel					= 0.0f;
	d_decel					= 0.0f;
	d_endDelay				= 0.0f;
	d_timeRemaining		= 0.0f;
	d_timeActive			= 0.0f;
	d_targetingBehavior	= TARGET_BEHAVIOR_POINT_IN_SPACE;

	d_target			= NULL;
	d_pCamera		= NULL;
	d_oldCamName	= NULL;
	d_group			= NULL;
	d_path			= NULL;
	textMessage		= NULL;

	d_TopSpriteHandle = INVALID_HANDLE;
	d_BottomSpriteHandle = INVALID_HANDLE;

	m_suspend		= false;
	d_pathCameraActivated = false;

	m_fadeIn		= false;
	m_fadeOut	= false;

	m_addCinemaBars = true;  // on by default for now -- turn off as needed

	d_vPosition.Clear();
	d_targetPoint.Clear();

	Setup();

	// *** build cameras that will be used ***

	CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();

	// Build a camera group and add it to the scene.
	d_group = cameraScene->FindGroup( "ScriptCam" );
	if ( !d_group )
	{
		d_group = new CameraGroup( "ScriptCam" );
		ASSERT_PTR( d_group );
		cameraScene->AddGroup( d_group );

		// Build a camera and add it to the camera group.
		StationaryCamera *sCamera = new StationaryCamera( "StationaryCam" );
		ASSERT( sCamera );
		d_group->AddTail( sCamera );

		// Build a camera and add it to the camera group.
		PathCamera *tcamera = new PathCamera( "PathCam" );
		ASSERT( tcamera );
		d_group->AddTail( tcamera );
		d_group->Select( tcamera );
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CPathFollowerCam::~CPathFollowerCam()
{
}

 /* setup from environmental actor record */

void CPathFollowerCam::Setup()
{
	SplinePath			*path;

	d_suspendMessageHandler.Initialize( "Suspend" , MESSAGE_HANDLE_IF_MASTER , actor.GetMessageOwner() , this );
	d_resumeMessageHandler.Initialize( "Resume" , MESSAGE_HANDLE_IF_MASTER , actor.GetMessageOwner() , this );
	
	d_startCinematicMessageHandler.Initialize( "StartCinematic" , MESSAGE_HANDLE_IF_MASTER , actor.GetMessageOwner() , this );
	d_endCinematicMessageHandler.Initialize( "EndCinematic" , MESSAGE_HANDLE_IF_MASTER , actor.GetMessageOwner() , this );

	d_SubTitleText.SetFont( Font::GetFirstFont() );
	d_SubTitleText.SetWordWrap( false );
	d_SubTitleText.SetPosition( 320 , 40 );
	d_SubTitleText.SetOrigin( .5 , .5 );
	d_SubTitleText.SetPriority( 0xffff );

	// check path name
	const char *pPathName = actor.Parm().GetString( "path_name" );
	if( ( pPathName == NULL ) || ( *pPathName == 0 ) )
	{
		return;
	}
	if( stricmp( pPathName, "none" ) == 0 )
	{
		return;
	}

	/* we have a path that should be in the collection */
	path = PathCollection::Collection()->Named( pPathName );
	if( path == NULL )
	{
		path = PathCollection::Collection()->Main();	// not found, default to main path
	}
	d_path = path;

	if( path == NULL )
	{
		return;
	}
}

//-------------------------------------------------------------------------
//	Set which path to follow
//-------------------------------------------------------------------------
bool CPathFollowerCam::SetPath( SplinePath *i_path )
{
	if( i_path )
	{
		((PathCamera*)d_pCamera)->SetPath( i_path );
		return( true );
	}
	else
	{
		return( false );
	}
}


//-------------------------------------------------------------------------
// move along path
//-------------------------------------------------------------------------
void CPathFollowerCam::Go( float i_deltaSec )
{
	// only one path cam activated at a time
	if( d_pathCameraActivated == true )
	{
		return;
	}

	d_pCamera->Activate();
}

//-------------------------------------------------------------------------
// stop at current position
//-------------------------------------------------------------------------
void CPathFollowerCam::Stop( float i_deltaSec )
{
	d_pCamera->Deactivate();
}


// ----------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void CPathFollowerCam::BeginUpdate()
{
	if( !IsActive() )
	{
		return;
	}
}

// ----------------------------------------------------------------------------------

void CPathFollowerCam::AttemptUpdate( float i_deltaSec )
{
	PathCamera			*pc;
	StationaryCamera	*sc;
	bool					deactivateComponent = false;

	if( !IsActive() || m_suspend )
	{
		return;
	}

	d_SubTitleText.Update();

	// set up stationary targeting
	if( GetCameraType() == CAMERA_TYPE_STATIONARY )
	{
		sc =(StationaryCamera *)d_pCamera;

		if( d_targetingBehavior==TARGET_BEHAVIOR_ACTOR || d_targetingBehavior==TARGET_BEHAVIOR_PLAYER )
		{
			sc->SetTarget( d_target );
			// adjust for the player's height
			if( d_targetingBehavior==TARGET_BEHAVIOR_PLAYER )
			{
				sc->SetTargetOffset( Vector3( 0 , 1.5f , 0 ) );
			}
		}
		else if( d_targetingBehavior==TARGET_BEHAVIOR_POINT_IN_SPACE )
		{
			sc->FollowTarget( &d_targetPoint );
		}

		SetTimeRemaining( GetTimeRemaining() - i_deltaSec );

		//check for fade-out
		if( GetFadeOut() )
		{
			if( GetTimeRemaining() <= .5f )	
			{
				RenderMgr__FadeOut( FADE_TO_BLACK_SLOW , true );	// true == can be paused
			}
		}

		// we are done when the time is up

		if( GetTimeRemaining() <= 0 )
		{
			deactivateComponent = true;
		}
	}

	// set up path targeting
	if( GetCameraType() == CAMERA_TYPE_PATH )
	{
		pc = (PathCamera *)d_pCamera;
		
		SplinePath	*path = pc->GetPath();
		float			estTimeLeft = ( path->Length() - pc->GetDistance() )/d_speed;

		// get our speed to where it should be
		if( d_speed != d_targetSpeed )
		{
			// update the distance along the track here.
			if( pc->GetDistance() <= path->Length()/2.0f )
			{
				d_speed = FOFilterImplicit( d_speed , d_targetSpeed , i_deltaSec , d_accel );
			}
			pc->SetSpeed( d_speed );
		}

		// fade out if needed
		if( GetFadeOut() && estTimeLeft <= 1.0f )
		{
			RenderMgr__FadeOut( FADE_TO_BLACK_SLOW , true );	// true == can be paused
		}
		
		pc->SetTarget( NULL );
		pc->ClearTargetingPoint();
		if( d_targetingBehavior==TARGET_BEHAVIOR_POINT_IN_SPACE )
		{
			pc->SetTargetingPoint();
			pc->SetTarget( NULL );
			pc->SetTargetPoint( &d_targetPoint );
		}
		else if( d_targetingBehavior==TARGET_BEHAVIOR_ACTOR || d_targetingBehavior==TARGET_BEHAVIOR_PLAYER )
		{
			pc->SetTarget( d_target );
			// adjust for the player's height
			if( d_targetingBehavior==TARGET_BEHAVIOR_PLAYER )
			{
				pc->SetTargetOffset( Vector3( 0 , 1.5f , 0 ) );
			}
		}

		// send a done message when at the end of the path
		if( !pc->IsFollowingPath() )
		{
			if( d_endDelay>0)
			{
				d_endDelay -= i_deltaSec;
			}
			else
			{
				deactivateComponent = true;
			}
		}
	}

#ifdef _DEBUG // allow this for design use only -- too many bugs linger making it a mess for release.
	// button out of a cinematic?
#ifdef DIRECTX_PC
	ControllerInputDriver* inputDriver = g_input.GetController( 0 );
	if (inputDriver)
	{
		if( inputDriver->ButtonPressed( CB_X ) )
		{
			g_audio.SetVolume( Audio::MUSIC , 1.0f );
			g_messageDispatcher.SendMessageToAll( "EndCinematic" , NULL , INVALID_OBJECT_HANDLE );
			g_messageDispatcher.SendMessageToActor( "TerminateCinematic" , NULL , actor.GetHandle() , actor.GetHandle() );
		}
	}
#else
	ControllerInputDriver *c = NULL;
	if ( g_input.ControllerLocked(0) )
		c = g_input.GetController(0);

	if (c != NULL)
	{
		if( c->ButtonPressed( CB_X ) )
		{
			g_audio.SetVolume( Audio::MUSIC , 1.0f );
			g_messageDispatcher.SendMessageToAll( "EndCinematic" , NULL , INVALID_OBJECT_HANDLE );
			g_messageDispatcher.SendMessageToActor( "TerminateCinematic" , NULL , actor.GetHandle() , actor.GetHandle() );
		}
	}
#endif
#endif

	// deactivate the component if requested
	if( deactivateComponent )
	{
		g_audio.SetVolumeFactor( Audio::MUSIC , 1.0f );
		g_messageDispatcher.SendMessageToAll( "EndCinematic" , NULL , INVALID_OBJECT_HANDLE );
		g_messageDispatcher.SendMessageToActor( "EndPathFollowerCam" , NULL , actor.GetHandle() , actor.GetHandle() );
	}
}

void CPathFollowerCam::Activate( void )
{
	CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();

	// only one path cam activated at a time
	if( d_pathCameraActivated == true )
	{
		return;
	}

	d_oldCamName = "OldCam";

	if( GetAddCinemaBars() )
	{
		AddBlackCinemaBars();
	}

	// push our camera group onto the stack
	cameraScene->PushGroup( d_group );
	cameraScene = g_cameraDirector.GetDefaultScene();
	CameraGroup &cameraGroup = cameraScene->CurrentGroup();

	// fade in if requested
	if( GetFadeIn() )
	{
		RenderMgr__FadeIn( FADE_TO_BLACK_SLOW , true );	// true == can be paused
	}
			
	switch( GetCameraType() )
	{
		case CAMERA_TYPE_STATIONARY:
			StationaryCamera *sc;

			sc = (StationaryCamera *)cameraGroup.Find( "StationaryCam" );
			d_pCamera = sc;
			sc->Setup( this->GetActor().GetBodyInWorld() , Vector3( 0 , 0 , 0 ) );
			sc->SetHither( .1f );
			sc->Activate();
			cameraGroup.Select( sc );
			SetTimeRemaining( GetTimeActive() );
			sc->SetTargetOffset( Vector3( 0 , 0 , 0 ) );
			if( d_targetingBehavior==TARGET_BEHAVIOR_ACTOR || d_targetingBehavior==TARGET_BEHAVIOR_PLAYER )
			{
				sc->SetTarget( d_target );
			}
			else if( d_targetingBehavior==TARGET_BEHAVIOR_POINT_IN_SPACE )
			{
				sc->FollowTarget( &d_targetPoint );
			}
		break;

		case CAMERA_TYPE_PATH:			
			PathCamera *pc;

			pc = (PathCamera *)cameraGroup.Find( "PathCam" );
			d_pCamera = pc;
			ASSERT_PTR( pc );
			pc->Reset();
			pc->SetHither( .1f );
			pc->SetPath( d_path );
			pc->SetDistance( 0.0f );
			pc->SetSpeed( d_speed );
			pc->SetTarget( NULL );
			pc->ClearTargetingPoint();
			pc->SetTargetOffset( Vector3( 0 , 0 , 0 ) );
			
			if( d_targetingBehavior==TARGET_BEHAVIOR_POINT_IN_SPACE )
			{
				pc->SetTargetingPoint();
				pc->SetTarget( NULL );
				pc->SetTargetPoint( &d_targetPoint );
			}
			else if( d_targetingBehavior==TARGET_BEHAVIOR_ACTOR || d_targetingBehavior==TARGET_BEHAVIOR_PLAYER )
			{
				pc->SetTarget( d_target );
			}
			pc->Activate();
			cameraGroup.Select( pc );
		break;
	}

	if( strcmp( textMessage , "***" ) )
		g_audio.SetVolumeFactor( Audio::MUSIC , 0.2f );
	else
		g_audio.SetVolumeFactor( Audio::MUSIC , 1.0f );

	g_messageDispatcher.SendMessageToAll( "StartCinematic" , NULL , INVALID_OBJECT_HANDLE );

	CActorComponent::Activate();
}

void CPathFollowerCam::Deactivate( void )
{
	Camera &camera = g_cameraDirector.CurrentCamera();
	CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();

	// if a camera is active deactivate it
	if( d_oldCamName != NULL )
	{
		if( GetFadeOut() )
		{
			RenderMgr__FadeIn( FADE_TO_BLACK_SLOW , true );	// true == can be paused
		}
		if( GetAddCinemaBars() )
		{
			RemoveBlackCinemaBars();
		}
		d_pCamera->Deactivate();
		cameraScene->PopGroup(d_group);
		camera = g_cameraDirector.CurrentCamera();
		d_oldCamName = NULL;
	}

	CActorComponent::Deactivate();
}

// add opaque black sprites to top and bottom of screen to give the "cinema" look. this is available for outside use as well
void CPathFollowerCam::AddBlackCinemaBars()
{
	// if already on, don't do it again
	if( d_TopSpriteHandle != INVALID_HANDLE )
	{
		return;
	}

	d_SubTitleText.Show();
	d_SubTitleText.SetPriority( 0xfff5 );
	if( strcmp( textMessage , "***" ) )
		d_SubTitleText.SetText( textMessage );
	//d_SubTitleText.SetText( "TEST" );

	// add black "cinema" bars on top and bottom of screen 
#define BAR_HEIGHT	0.16f
	static Vector3 TopVertices[ 4 ] = { Vector3( 0.0f , 0.0f , 0.0f ), Vector3( 1.0f , 0.0f , 0.0f ), Vector3( 1.0f , BAR_HEIGHT , 0.0f ), Vector3( 0.0f , BAR_HEIGHT , 0.0f ) };
	static Vector3 BottomVertices[ 4 ] = { Vector3( 0.0f , 1.0f - BAR_HEIGHT , 0.0f ) , Vector3( 1.0f , 1.0f - BAR_HEIGHT , 0.0f ) , Vector3( 1.0f , 1.0f , 0.0f ) , Vector3( 0.0f , 1.0f , 0.0f ) };
	static ts_bRGBA BlackColor = { 0 , 0 , 0 , 255 };
//	d_TopSpriteHandle = SpriteMgr__RegisterSprite( TS_NO_TEXTURE , SPRITE_DEFAULT_GROUP , 0xfff0 , SPRITE_ALL_ONE_COLOR , TopVertices , NULL , &BlackColor );
//	d_BottomSpriteHandle = SpriteMgr__RegisterSprite( TS_NO_TEXTURE , SPRITE_DEFAULT_GROUP , 0xfff0 , SPRITE_ALL_ONE_COLOR , BottomVertices , NULL , &BlackColor );
	d_TopSpriteHandle = SpriteMgr__RegisterSprite( TS_NO_TEXTURE , SPRITE_DEFAULT_GROUP , 80 , SPRITE_ALL_ONE_COLOR , TopVertices , NULL , &BlackColor );
	d_BottomSpriteHandle = SpriteMgr__RegisterSprite( TS_NO_TEXTURE , SPRITE_DEFAULT_GROUP , 80 , SPRITE_ALL_ONE_COLOR , BottomVertices , NULL , &BlackColor );
}


// remove them 
void CPathFollowerCam::RemoveBlackCinemaBars()
{
	if( d_TopSpriteHandle != INVALID_HANDLE )
	{
		SpriteMgr__KillSprite( d_TopSpriteHandle );
	}
	if( d_BottomSpriteHandle != INVALID_HANDLE )
	{
		SpriteMgr__KillSprite( d_BottomSpriteHandle );
	}
	d_TopSpriteHandle = INVALID_HANDLE;
	d_BottomSpriteHandle = INVALID_HANDLE;

	d_SubTitleText.Hide();
}

// ----------------------------------------------------------------------------------

/* copy one path follower to another. like CPathFollower::Randomize, typically this routine should be used as part of
initialization. path followers can move from path to path, and you probably want to copy the path follower's original
state, when he is on his original path */

CPathFollowerCam &CPathFollowerCam::operator=( CPathFollowerCam &Source )
{
	// memcpy 
	memcpy(this, &Source, sizeof(CPathFollowerCam));

	// done 
	return *this;
}


void CPathFollowerCam::SuspendMessage::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->m_suspend = true;
}

void CPathFollowerCam::ResumeMessage::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->m_suspend = false;
}

void CPathFollowerCam::StartCinematic::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->d_pathCameraActivated = true;
	g_messageDispatcher.SendMessageToAll( "PauseObjTimer" , NULL , INVALID_OBJECT_HANDLE );
	g_messageDispatcher.SendMessageToAll( "DropHostage" , NULL , INVALID_OBJECT_HANDLE );

//	g_messageDispatcher.SendMessageToActor( "HostageTaker", CMessageDispatcher::HandleToData(INVALID_OBJECT_HANDLE), actor.GetHandle(), actor.GetHandle() );
//	g_messageDispatcher.SendMessageToActor( "Hostage", 0, INVALID_OBJECT_HANDLE, target->GetHandle() );
}

void CPathFollowerCam::EndCinematic::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->d_pathCameraActivated = false;
	g_messageDispatcher.SendMessageToAll( "UnpauseObjTimer" , NULL , INVALID_OBJECT_HANDLE );
}
