/*
**
**  File:   cactor.cpp
**  Date:   March 28, 2000
**  By:     Bryant Collard
**  Desc:   Basic CActor class.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/cactor.cpp $
**  $Revision: #403 $
**  $DateTime: 2005/08/05 03:07:10 $
**    $Author: Rob $
**
*/

/******************* includes ****************************/

#include "stage/StagePCH.h"

#include "EngineHelper/timer.h"
#include "EngineHelper/PackBuffer.h"
#include "Game/Managers/IdManager.h"
#include "Effects/dropshad.h"
#include "stage/ActorCreator.h"
#include "cmdcon/cmdcon.h"

#ifdef PS2
#include "Display/PS2/Renderer/Code/PS2Renderer.h"
#endif

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

#ifdef NETZ
#include "Network/NetZ/DOActor.h"
#endif

// Declare handle curator data.
DeclareHandleCurator(CActor, ACTOR_HANDLE_MASK_WIDTH);

// Define a creator and add it to the creator list. Since the name of the
// creator defined by the macro is the class name with "Creator" appended
// (i.e. CActorCreator for CActor) and CActorCreator is the name of the base
// class, use a custom creator to avoid the resulting name conflict.
CUSTOM_ACTOR_CREATOR(CPlainActor, actor)::Create(void)
{
	return(new CActor);
}

//#define ANIMDEBUGINFO //define this to cause animation information to be printed onscreen when renderer stats are enabled
#ifdef ANIMDEBUGINFO
#define DEBUGOUT(line, stuff) WriteDebugMessage( line, stuff )
void WriteDebugMessage( int line, char *Message );
#include "hud/hud.h"
#endif

// Bits
const uint CActor::INVOLVE_NONE                    = 0x0;
const uint CActor::INVOLVE_ACTIVE                  = 0x1;
const uint CActor::INVOLVE_STAGE                   = 0x2;
const uint CActor::INVOLVE_PROTAGONIST             = 0x4;

// Categories
const uint CActor::INVOLVE_INACTIVE_NONPROTAGONIST = 0x2;
const uint CActor::INVOLVE_ACTIVE_NONPROTAGONIST   = 0x3;
const uint CActor::INVOLVE_INACTIVE_PROTAGONIST    = 0x6;
const uint CActor::INVOLVE_ACTIVE_PROTAGONIST      = 0x7;

// Control
const uint INVOLVE_MASK                            = 0x7;
const uint INVOLVE_SHIFT                           = 3;

/////////////////////////////    CActor    ////////////////////////////

#pragma warning(disable : 4355)  // Allow this pointer in initializer list

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    CActor()
 *  Access:      public
 *  Description: Default constructor.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
CActor::CActor() :
	CStageEntity(s_actorClass),
	d_messageOwner("Actor"),
	d_handleCurator(this)
{
	ConstructActor();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    CActor()
 *  Access:      public
 *  Description: Default constructor.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
CActor::CActor(const CEntityType &i_type) :
	CStageEntity(i_type),
	d_messageOwner("Actor"),
	d_handleCurator(this)
{
	ConstructActor();
}

#pragma warning(default : 4355)

/**********************************************************************/
void CActor::Destroy(CActor* i_actor)
{
	if (i_actor)
	{
		// inform others, and our script, of our death
		g_messageDispatcher.SendMessageToAll("ActorDied", NULL, i_actor->GetHandle());

		// and destroy
		delete i_actor;
	}
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    ~CActor()
 *  Access:      public
 *  Description: Destructor
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
CActor::~CActor(void)
{
	int i;

	// Disconnect from the network. If this actor is being destroyed because
	// of an explict call to remove it from the game, try to remove it from
	// the network as well.
	UnbindNetActor(d_markedForDeath);

	// remove dynamic chains
	delete d_chains;

	// kill scripts
	for ( i = 0; i < MAX_SCRIPTS; i++ )
	{
		if ( Script( i ) )
		{
			g_ScriptSystem.DestroyVM( Script( i ) );
			SetScript( i, NULL );
		}
	}

	// Delete us from all our casts
	CCast::DeleteActor(*this);

	// free geometries
	for ( i = 0 ; i < MAX_ACTOR_INSTANCES ; i++ )
	{
		if ( d_instance[ i ] )
		{
			DestroyInstance( i );
			delete d_instance[ i ];
			d_instance[ i ] = NULL;
		}
	}

	// free our name
	if ( d_Name )
	{
		delete [] d_Name;
	}

#ifdef ENABLE_RECORD
	DeleteRecordBuffer();
#endif
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    ConstructActor
 *  Access:      private
 *  Description: Shared by constructors
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActor::ConstructActor(void)
{
	int i;

	// not running any scripts
	for ( i = 0 ; i < MAX_SCRIPTS ; i++ )
		SetScript( i, NULL );
	SetActionFlags( 0 );

	// clear our geoms
	for ( i = 0 ; i < MAX_ACTOR_INSTANCES ; i++ )
		d_instance[ i ] = NULL;

	// clear motions
	for ( i = 0 ; i < MAX_MOTIONS ; i++ )
	{
		memset( &d_motnInf[ i ], 0, sizeof( d_motnInf[ i ] ) );
	}

	// clear identity
	d_Name = NULL;

/*
 *	Initialize state variables.
 */
	SET_MESSAGE_DISPATCHER_CALL_LOCATION();
	d_netActor = NULL;
	SetNetMaster(true);
	d_messageOwner.SetCoordinateSystem(&d_bodyInWorld, &d_bodyToWorld);
	ClearDiscontinuity();

	Place( Vector3( 0.0f, 0.0f, 0.0f ), 0.0f);
	d_scale.Set( 1.0f, 1.0f, 1.0f );

/*
 *	Set the max update seconds to something big.
 */
	d_maxUpdateSec = 100.0f;
/*
 *	Initialize cast list.
 */
	d_castHead = 0;
	d_deleteWhenNoCast = false;
	d_markedForDeath = false;
	d_lienCount = 0;

	d_category |= ACTOR_CATEGORY;

	d_chains = 0;
	d_meshList = 0;

	// Make actors active by default.
	SetInvolvement(INVOLVE_ACTIVE);

#ifdef ENABLE_RECORD
	d_recordBuffer = 0;
	d_recordState = IDLE;
#endif
	m_uiUniqueID = g_CIdManager.getUniqueId( );
	d_pLightInfluences = NULL;
	d_pBaseAmbientLight = NULL;
	d_nLightColorSource = LIGHT_COLOR_SOURCE_NONE;
	d_fLightColorFloor = 0.0f;
	SetLightColorDefaults();

	// set default skeleton indices
	SetSkeletonIndices(0,1,2);

	// --- instant replay setup
	//m_replayHandle = g_replayMgr.CreateReplayHandle( CReplayMgr::DESTROYABLE );
	//g_replayMgr.RegisterReplayFunctions( m_replayHandle, this, RecordReplayInterface, PlayReplayInterface );
}

bool CActor::SetGeometryFromWorldInstance( int index, const char *modelName, int master /* = -1*/ )
{
	ASSERT( index >= 0 && index < MAX_ACTOR_INSTANCES );

	// clean up the old instance
	if ( d_instance[ index ] )
	{
		DestroyInstance( index );
		delete d_instance[ index ];
		d_instance[ index ] = NULL;
	}

	CInstance* instance = g_loadSOM.FindInstanceByName( modelName );
	if ( !instance )
	{
		ASSERTF( false, ( "instance '%s' not found in world.\n", modelName ) );
		return false;
	}

	// set our geometry to be the world geometry
	ts_Geometry geom;
	instance->GetGeometry( &geom );
	if ( geom.pSOM )
	{
		MEM_SET_ALLOC_NAME( "CActorInstance" );
		d_instance[ index ] = new CActorInstance;
		d_instance[ index ]->SetMaster( master );
		d_instance[ index ]->SetVisualPropertiesFromInstance( instance );
		return MakeInstance( index, &geom );
	}
	else
	{
		ASSERTF( false, ( "instance '%s' has no geometry", modelName ) );
		return false;
	}
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    SetGeometry
 *  Access:      public
 *  Description: Initialize geometry 
 *  Arguments:
 *      i_geometry    - Pointer to the model LOD record or SOM data.
 *  Return:      Success true/false.
 *
 **********************************************************************/
bool CActor::SetGeometry( int index, ts_Geometry* i_geometry, int master /* = -1*/ )
{
	ASSERT( index >= 0 && index < MAX_ACTOR_INSTANCES );

	// clean up the old instance
	if ( d_instance[ index ] )
	{
		DestroyInstance( index );
		delete d_instance[ index ];
		d_instance[ index ] = NULL;
  	}

	//	Get memory for the actor (must be 16-byte aligned).
	MEM_SET_ALLOC_NAME("CActorInstance");
	d_instance[ index ] = new CActorInstance;
	d_instance[ index ]->SetMaster( master );

	return MakeInstance( index, i_geometry );
}

// detach a piece of geometry from this actor, create a new actor which is returned
// The detached geometry will be attached to the new Actor we create
// NOTE: Do not remove the master skeleton geometry if it has slaves
CActor* CActor::DetachGeometryWithNewActor( int DetachIndex, int AttachIndex )
{
	CActor*			NewActor;
	ts_Geometry		Geom;

	ASSERT( DetachIndex >=0 && DetachIndex < MAX_ACTOR_INSTANCES );
	ASSERT( AttachIndex >=0 && AttachIndex < MAX_ACTOR_INSTANCES );

	ASSERTS (d_instance[DetachIndex] != NULL, "Geom doesn't exist, can't detach");

	// check for slaves
	for (int i = 0; i < MAX_ACTOR_INSTANCES; i++)
	{
		if ( d_instance[i] != NULL )
		{
			ASSERTS ( d_instance[i]->GetMaster() != DetachIndex, "Detach Error: This geom has slaves" );
		}
	}

	// create a new actor
	NewActor = new CActor;

	// get the geometry we want to copy
	Instance( DetachIndex )->GetGeometry( &Geom );

	// attach geometry to new actor
	NewActor->SetGeometry( AttachIndex, &Geom );

	// set some actor stuff
	NewActor->InitializeGeometry(0);
	NewActor->Initialize();
	NewActor->Category() |= PROCESS_LAST_CATEGORY | ROAMER_CATEGORY | NO_SUPPORT_CATEGORY | PLAYER_CATEGORY;
	NewActor->SetPositiveInclude(ALL_CATEGORIES);
	//NewActor->SetPositiveExclude(DEFAULT_EXCLUDED_CATEGORIES);

	// clear his skeleton index if we have one to stop the motion
	// handler from trying to play a motion on a non-existant skeleton
	for (int i = 0; i < MAX_SKELETONS; i++)
	{
		if ( GetSkeletonIndex(i) == DetachIndex )
			SetSkeletonIndex(i, -1);
	}

	// delete geometry from this actor
	DestroyInstance( DetachIndex );
	delete d_instance[ DetachIndex ];
	d_instance[ DetachIndex ] = NULL;

	// pass back new actor with the detached geometry attached to it
	return NewActor;
}

// detach a piece of geometry from this actor
// NOTE: Do not remove the master skeleton geometry if it has slaves
// Setting Assert=0 will ignore the check for slaves.. use with caution
bool CActor::DetachGeometry ( int DetachIndex, int Assert )
{
	ASSERT( DetachIndex >=0 && DetachIndex < MAX_ACTOR_INSTANCES );

	// check for slaves
	if (Assert)
	{
		for (int i = 0; i < MAX_ACTOR_INSTANCES ; i++)
		{
			if ( d_instance[i] != NULL )
				ASSERTS ( d_instance[i]->GetMaster() != DetachIndex, "Detach Error: This geom has slaves" );
		}
	}

	// clear his skeleton index if we have one to stop the motion
	// handler from trying to play a motion on a non-existant skeleton
	for (int i = 0; i < MAX_SKELETONS; i++)
	{
		if ( GetSkeletonIndex(i) == DetachIndex )
			SetSkeletonIndex(i, -1);
	}

	// delete geometry from this actor
	DestroyInstance( DetachIndex );
	delete d_instance[ DetachIndex ];
	d_instance[ DetachIndex ] = NULL;

	return true;
}

// Set name of bone we want to attach this geometry to for positional purposes
bool CActor::SetGeometryAttachBone( int index, const char *boneName )
{
	strncpy( d_instance[ index ]->d_boneName, boneName, 16);
	return true;
}

// set offset from attached bone
bool CActor::SetGeometryOffset ( int index, const Vector3 &pos )
{
	d_instance[ index ]->d_offset = pos;
	return true;
}

// set rotation offset from attached bone
bool CActor::SetGeometryRotation ( int index, const Vector3 &rot )
{
	d_instance[ index ]->d_rotation = rot;
	return true;
}

/**********************************************************************/
void CActor::SetSkeletonIndices( int primary, int secondary, int tertiary )
{
	d_skeletons[PRIMARY_SKELETON] = primary;
	d_skeletons[SECONDARY_SKELETON] = secondary;
	d_skeletons[TERTIARY_SKELETON] = tertiary;
}

void CActor::SetSkeletonIndex (int WhichSkeleton, int GeomIndex)
{
	d_skeletons[WhichSkeleton] = GeomIndex;
}

int CActor::GetSkeletonIndex (int WhichSkeleton)
{
	return  d_skeletons[WhichSkeleton];
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    InitializeGeometry
 *  Access:      public
 *  Description: Initialize geometries
 *  Return:      none
 *
 **********************************************************************/
#define MAX_CHANNELS	7
void CActor::InitializeGeometry(int i_index)
{
	// Create the motion channel table (for concurrent grouped motion playback)
	ASSERT_PTR(Instance(i_index));
	if (Instance(i_index) && Instance(i_index)->Body())
	{
		Instance(i_index)->Init( Instance(i_index)->Body(), MAX_CHANNELS );
	}
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    Initialize
 *  Access:      public
 *  Description: Initialize installable stuff.
 *  Return:      Success true/false.
 *
 **********************************************************************/
bool CActor::Initialize(void)
{
	// init base geometry
	InitializeGeometry( 0 );

	// Build any chains and load params.
	d_chains = CActorChains::BuildChains( Instance(0) );
	LoadDynamicLinkageParams();

	// Init animation rate
	for ( int i = MOTION1 ; i < MAX_MOTIONS ; i++ )
	{
		motionDisabled[i] = false;
		SetAnimationRate( 1.0f, i );
	}

	// default lifetime is forever
	d_fLifetime = 0.0f;

	// Lighting-- leave ambient light and last color values at 0
	d_nLightColorSource = LIGHT_COLOR_SOURCE_NONE;
	d_fLightColorFloor = 0.0f;
	SetLightColorDefaults();
	d_pLightInfluences = Instance(0)->DynamicLightInfluences();

	// Make sure minimal state gets distributed if not done somewhere else.
	d_stateDistributed = false;
	d_netDataReceivedHandler.Initialize("NetDataReceived",
			MESSAGE_HANDLE_IF_DUPLICA, d_messageOwner, this);

	// run the motion data the next time through
	d_bFirstFrame = true;
	
	return true;
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    MakeInstance
 *  Access:      private
 *  Description: Make instance, possibly instances.
 *  Arguments:
 *      i_geometry    - Pointer to the model LOD record or SOM data.
 *  Return:      true for success, false otherwise.
 *
 **********************************************************************/
bool CActor::MakeInstance( int index, ts_Geometry* i_geometry )
{
	ASSERT( index >= 0 && index < MAX_ACTOR_INSTANCES );

	// If there is no geometry, let things stand as they are and return failure.
	if (i_geometry == NULL)
		return(false);

	//	Find the first non-mesh geometry and start counting mesh SOMs.
	int numMesh = 0;
	ts_PatchSOM* som = NULL;
	ts_Geometry geometry = *i_geometry;
	while (geometry.pSOM)
	{
		if (geometry.pSOM->dh.u8DataType != DATA_MULTI_SOM)
		{
			#ifndef SINGLE_MESH
			som = geometry.pSOM->pNextSOM;
			#endif
			break;
		}
		numMesh++;
		#ifndef SINGLE_MESH
		geometry.pSOM = geometry.pSOM->pNextSOM;
		#else
		geometry.pSOM = NULL;
		#endif
	}

	//	Make the root instance if appropriate and count the first non-mesh SOM.
	int numNonMesh = 0;
	if (geometry.pSOM || geometry.pLOD)
	{
		Instance( index )->InitFromGeometry( geometry );
		numNonMesh = 1;
		Instance( index )->SetOwner( this );
		SceneMgr::AddInstance( Instance( index ) );
	}

	// Finish counting.
	while (som)
	{
		if (som->dh.u8DataType != DATA_MULTI_SOM)
			numNonMesh++;
		else
			numMesh++;

		#ifndef SINGLE_MESH
		som = som->pNextSOM;
		#endif
	}

	// If there are no non-mesh SOMs, make sure there is at least one mesh SOM and
	// make a dummy instance.
	if (numNonMesh == 0)
	{
		if (numMesh == 0)
			return(false);
		CMeshList::MakeDummyInstance( *Instance( index ) );
		Instance( index )->SetOwner( this );
	}
	else if (numNonMesh > 1)
	{
		// Make a list of non-meshes.
		ASSERT(false);
	}

	// Build the list of meshes.
	if (numMesh > 0)
	{
		d_meshList = new CMeshList(numMesh, *Instance( index ) );
		som = i_geometry->pSOM;
		while (som)
		{
			if (som->dh.u8DataType == DATA_MULTI_SOM)
			{
				if (!d_meshList->AddMesh(*((SMeshSOM*)som), this))
					return(false);
			}
			#ifndef SINGLE_MESH
			som = som->pNextSOM;
			#else
			som = NULL;
			#endif
		}

		#ifdef SINGLE_MESH
		Instance( index )->SetDynamicModel( d_meshList->GetInstance(0).DynamicModel() );
		#endif
	}

	return true;
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    DestroyInstance
 *  Access:      private
 *  Description: Clean up instance, possibly instances
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActor::DestroyInstance( int index )
{
	ASSERT( index >= 0 && index < MAX_ACTOR_INSTANCES );

	#ifdef SINGLE_MESH
	if ( Instance( index ) && (d_meshList == NULL))
	#else
	if ( Instance( index ) && (Instance( index )->pSOM))
	#endif
	{
		SceneMgr::RemoveInstance(Instance( index ));
		Instance( index )->RemoveBones();
	}

	// Clean up possible meshes (e.g. cloth).
	if ( d_meshList )
	{
		delete d_meshList;
		d_meshList = 0;

		#ifdef SINGLE_MESH
		if ( Instance( index )->DynamicModel() )
		{
			Instance( index )->SetDynamicModel( NULL );
		}
		#endif
	}
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    AddInstanceToScene
 *  Access:      public
 *  Description: use this one if you already know the index of the vis
 *               region that you want to add the actor to
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActor::AddInstanceToScene( int index )
{
	ASSERT( index >= 0 && index < MAX_ACTOR_INSTANCES );

	#ifdef SINGLE_MESH
	if (Instance( index ) && (d_meshList == NULL))
	#else
	if (Instance( index ) && (Instance( index )->pSOM))
	#endif
	{
		SceneMgr::AddInstance(Instance( index ));
	}

	if (d_meshList)
		d_meshList->AddInstancesToScene();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    MarkForDeath
 *  Access:      public
 *  Description: Mark for deletion at the end of the frame, or after
 *               some number of seconds
 *  Arguments:   delay in seconds (default 0.0)
 *  Return:      None
 *
 **********************************************************************/
void CActor::MarkForDeath(float fSeconds /*= 0.0f*/)
{
	if (fSeconds > 0.0f)
		d_fLifetime = fSeconds;
	else
		d_markedForDeath = ConfirmDeath();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    Placed
 *  Access:      public
 *  Description: Collect sync routines.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActor::Placed(void)
{
	SyncState();
	SyncActor();
	PlaceGeometry();
	UpdateGeomBoundingVolumes();
	SetDiscontinuity(DISCONTINUITY_POSITION | DISCONTINUITY_VELOCITY |
			DISCONTINUITY_ORIENTATION | DISCONTINUITY_ANGULAR_VELOCITY |
			DISCONTINUITY_STATE | DISCONTINUITY_SPEED_TARGET | 
			DISCONTINUITY_SPEED_CURRENT | DISCONTINUITY_PATH_DISTANCE);
	WriteDistributedState();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    Place
 *  Access:      public
 *  Description: Place the actor at the specified <x,z> location and
 *               heading. This implementation assumes y = 0; however,
 *               subclasses should place the object on the ground
 *               in a sensible way for the object. Zero out the
 *               velocities.
 *  Arguments:
 *      i_x       - X offset from the world origin (meters).
 *      i_z       - Z offset from the world origin (meters).
 *      i_heading - Positive rotation about world Y-axis (radians).
 *  Return:      None
 *
 **********************************************************************/
void CActor::Place(float i_x, float i_z, float i_heading)
{
	d_bodyInWorld.Set (i_x, 0.0f, i_z);
	d_bodyToWorld.SetYRotation(i_heading);
	d_velocityInWorld.Clear();
	d_omegaInBody.Clear();

	Placed();
}


/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    Place
 *  Access:      public
 *  Description: Place the actor at the specified position and
 *               heading. Zero out the velocities.
 *  Arguments:
 *      i_pos     - Offset from world origin (meters).
 *      i_heading - Positive rotation about world Y-axis (radians).
 *  Return:      None
 *
 **********************************************************************/
void CActor::Place(Vector3CRef i_pos, float i_heading)
{
	d_bodyInWorld = i_pos;
	d_bodyToWorld.SetYRotation(i_heading);
	d_velocityInWorld.Clear();
	d_omegaInBody.Clear();

	Placed();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    Place
 *  Access:      public
 *  Description: Place the actor at the specified position and
 *               orientation. Zero out the velocities.
 *  Arguments:
 *      i_pos   - Offset from world origin (meters).
 *      i_euler - Euler angles (degrees). The are stored as rotations
 *                from the world to the body about the X-axis (-pitch),
 *                Y-axis (yaw) and Z-axis (roll). The order of rotation
 *                is yaw (Y-axis), pitch (-X-axis), roll (Z-axis).
 *  Return:      None
 *
 **********************************************************************/
void CActor::Place(Vector3CRef i_pos, Vector3CRef i_euler)
{
	d_bodyInWorld = i_pos;
	d_bodyToWorld.SetZRotation(Math::Deg2Rad(i_euler.z()));
	d_bodyToWorld.CatXRotation(Math::Deg2Rad(-i_euler.x()));
	d_bodyToWorld.CatYRotation(Math::Deg2Rad(i_euler.y()));
	d_velocityInWorld.Clear();
	d_omegaInBody.Clear();

	Placed();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    Place
 *  Access:      public
 *  Description: Place the actor at the specified position and
 *               orientation. Zero out the velocities.
 *  Arguments:
 *      i_pos         - Offset from world origin (meters).
 *      i_orientation - Body to world matrix.
 *  Return:      None
 *
 **********************************************************************/
void CActor::Place(Vector3CRef i_pos, const DirCos3x3 &i_orientation)
{
	d_bodyInWorld = i_pos;
	d_bodyToWorld = i_orientation;
	d_velocityInWorld.Clear();
	d_omegaInBody.Clear();
	
	Placed();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    Place
 *  Access:      public
 *  Description: Place the actor at the specified position and
 *               orientation. Zero out the velocities.
 *  Arguments:
 *  Return:      None
 *
 **********************************************************************/
void CActor::Place(const Graphics4x4 &i_transformation)
{
	d_bodyInWorld = i_transformation.GetTranslation();
	i_transformation.GetOrientation(d_bodyToWorld);
	d_velocityInWorld.Clear();
	d_omegaInBody.Clear();

	Placed();
}

/**********************************************************************/
bool CActor::LoadMotion( const char* motionFile, int geomIndex, int motionDBIndex )
{
	ASSERT_PTR( motionFile );
	ASSERT( geomIndex >= 0 && geomIndex < MAX_ACTOR_INSTANCES );
	ASSERT_PTR( Instance( geomIndex ) );

	t_Handle h = motLoadMotionChunk( motionFile, Instance( geomIndex ), motionDBIndex );

	return h != INVALID_HANDLE;
}

/**********************************************************************/
bool CActor::LoadAnimScript( const char* scriptFile, int scriptIndex )
{
	ASSERT_PTR( scriptFile );
	ASSERT( scriptIndex >= ANIM1_SCRIPT && scriptIndex < AI_SCRIPT );

	// release old script if we have one
	if ( Script( scriptIndex ) )
	{
		g_ScriptSystem.DestroyVM( Script( scriptIndex ) );
		SetScript( scriptIndex, NULL );
	}

	// load the new script
	ScriptExecutable* exe = g_ScriptSystem.LoadExecutable( scriptFile );
	if ( exe )
	{
		SetScript( scriptIndex, g_ScriptSystem.CreateVM( exe ) );
		Script( scriptIndex )->SetOwner( this );
		Script( scriptIndex )->RegisterMessageHandlers( &(GetMessageOwner()) );
		Script( scriptIndex )->Run( 0.0f );
		return true;
	}
	else
		return false;
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    StartAnimation
 *  Access:      public
 *  Description: Play a motion.
 *  Arguments:
 *      i_startAct - Starting action ID.
 *      i_force    - Start animation even if it thinks the requested
 *                   animation is the current animation.
 *      i_process  - Let the script process (but don't advance time).
 *  Return:      Always returns true.
 *
 **********************************************************************/
bool CActor::StartAnimation( const char* actionName, bool i_force, bool i_process, int nScriptIndex /*= ANIM1_SCRIPT*/ )
{
	if ( Script( nScriptIndex ) )
	{
		Script( nScriptIndex )->JumpToLabel( actionName );
		if ( IsActive() && i_process )
			Script( nScriptIndex )->Run( 0.0f );
	}
	return true;
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    GetSubMatrix
 *  Access:      public
 *  Description: Get SubMatrix.
 *  Arguments:
 *      iSubCount - SubMatrix ID.
 *  Return:      SubMatrix.
 *
 **********************************************************************/
Graphics4x4 CActor::GetSubMatrix(int iSubCount, int index)
{
	Graphics4x4 g4ReturnMatrix;
	boneGetMatrix(Instance(index)->Body(), iSubCount, &g4ReturnMatrix);
	return(g4ReturnMatrix);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetSubMatrix(int iSubCount, Graphics4x4 &g4InputMatrix, int index)
{
	boneSetMatrix(Instance(index)->Body(), iSubCount, &g4InputMatrix);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
bool CActor::GetBone(char *boneName, BoneID &boneID, int index)
{
//	return(boneGetBoneID(Body(), &boneID, boneName) != 0);
	if(boneGetBoneID(Instance(index)->Body(), &boneID, boneName) != 0)
		return 1;
	else
		return 0;
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
ts_Bone *CActor::GetBone(BoneID boneID, int index)
{
	return(boneGetBonePtr(Instance(index)->Body(), boneID));
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
ts_Bone *CActor::GetBone(char *boneName, int index)
{
	BoneID boneID = 0;
	if (boneGetBoneID(Instance(index)->Body(), &boneID, boneName) == 0)
		return NULL;
	else
		return(boneGetBonePtr(Instance(index)->Body(), boneID));
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
ts_ModelBone *CActor::GetModelBone(BoneID boneID, int index)
{
	return(boneGetModelBone(Instance(index)->Body(), boneID));
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
Vector3 CActor::GetEndPosition(BoneID boneID, int index)
{
	Vector3 v3RetVect;
	boneGetEndPosition(Instance(index)->Body(), boneID, &v3RetVect);
	return(v3RetVect);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
Vector3 CActor::GetStartPosition(BoneID boneID, int index)
{
	Vector3 v3RetVect;
	boneGetStartPosition(Instance(index)->Body(), boneID, &v3RetVect);
	return(v3RetVect);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
Vector3 CActor::PointLocalToWorld(Vector3CRef v3LocalPoint)
{
	return (v3LocalPoint * d_bodyToWorld + d_bodyInWorld);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
Vector3 CActor::PointWorldToLocal(Vector3CRef v3WorldPoint)
{
	return ((v3WorldPoint - d_bodyInWorld) / d_bodyToWorld);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
Vector3 CActor::PointModelToWorld(Vector3CRef v3ModelPoint)
{
	return (v3ModelPoint * (Graphics4x4 &)Instance( 0 )->Matrix());
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneMatrix(BoneID boneID, Matrix3x3 &g3NewMatrix)
{
	ts_ModelBone *pBone;
	Graphics4x4 g4Offset1;
	Graphics4x4 g4Offset2;

	pBone = GetModelBone(boneID);
	g4Offset1.SetTranslation(-pBone->vPivot);
	g4Offset2.SetTranslation(pBone->vPivot);

	Graphics4x4 TEMP; TEMP = g3NewMatrix;

	g4Offset1 *= TEMP;
	g4Offset1 *= g4Offset2;
	SetSubMatrix(boneID, g4Offset1);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneMatrix(BoneID boneID, Matrix3x3 &g3NewMatrix,
		float rPercentage)
{
	ts_ModelBone *pBone;
	Graphics4x4 g4Offset1;
	Graphics4x4 g4Offset2;
	Graphics4x4 g4Percent;

	g4Percent = g3NewMatrix;	// use percent as holder to convert 3x3 to 4x4
	g4Percent.SetIntermediate(rPercentage, g4Percent);
	pBone = GetModelBone(boneID);
	g4Offset1.SetTranslation(-pBone->vPivot);
	g4Offset2.SetTranslation(pBone->vPivot);
	g4Offset1 *= g4Percent;
	g4Offset1 *= g4Offset2;
	SetSubMatrix(boneID, g4Offset1);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneMatrix(BoneID boneID, Graphics4x4 &g4NewMatrix)
{
	ts_ModelBone *pBone;
	Graphics4x4 g4Offset1;
	Graphics4x4 g4Offset2;

	pBone = GetModelBone(boneID);
	g4Offset1.SetTranslation(-pBone->vPivot);
	g4Offset2.SetTranslation(pBone->vPivot);
	g4Offset1 *= g4NewMatrix;
	g4Offset1 *= g4Offset2;
	SetSubMatrix(boneID, g4Offset1);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneMatrix(BoneID boneID, Graphics4x4 &g4NewMatrix,
		float rPercentage)
{
	ts_ModelBone *pBone;
	Graphics4x4 g4Offset1;
	Graphics4x4 g4Offset2;
	Graphics4x4 g4Percent;

	g4Percent = g4NewMatrix;			 // use percent as holder for new
	g4Percent.SetIntermediate(rPercentage, g4Percent);
	pBone = GetModelBone(boneID);
	g4Offset1.SetTranslation(-pBone->vPivot);
	g4Offset2.SetTranslation(pBone->vPivot);
	g4Offset1 *= g4Percent;
	g4Offset1 *= g4Offset2;
	SetSubMatrix(boneID, g4Offset1);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneMatrix(ts_Quatvw &tsqFrom, BoneID boneID,
		Matrix3x3 &g3NewMatrix, float rMorphPct)
{
	Cartesian4x4 c4TempMatrix;
	Quat qFrom = tsqFrom;
	Quat qTo = g3NewMatrix;
	qTo = qFrom.Slerp(qTo, rMorphPct);
	c4TempMatrix = qTo.ConvertToC4x4();
	SetBoneMatrix(boneID, c4TempMatrix);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneMatrix(ts_Quatvw &tsqFrom, BoneID boneID,
		Graphics4x4 &g4NewMatrix, float rMorphPct)
{
	Cartesian4x4 c4TempMatrix;
	Quat qFrom = tsqFrom;
	Quat qTo = g4NewMatrix;
	qTo = qFrom.Slerp(qTo, rMorphPct);
	c4TempMatrix = qTo.ConvertToC4x4();
	SetBoneMatrix(boneID, c4TempMatrix);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneMatrix(ts_Quatvw &tsqFrom, BoneID boneID,
		Matrix3x3 &g3NewMatrix, float rPercentage, float rMorphPct)
{
	Quat	qFrom1;
	Quat	qTo1;
	Quat	qFrom2;
	Cartesian4x4 c4X;
/*
 *	rMorphPct from tsqFrom to (rPercentage from unit to g3NewMatrix)
 */
	qFrom1.Identity();
	qTo1 = g3NewMatrix;
	qFrom2 = tsqFrom;
	qFrom1 = qFrom1.Slerp(qTo1, rPercentage);
	qFrom1 = qFrom2.Slerp(qFrom1, rMorphPct);
	c4X = qFrom1.ConvertToC4x4();
	SetBoneMatrix(boneID, c4X);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneMatrix(ts_Quatvw &tsqFrom, BoneID boneID,
		Graphics4x4 &g4NewMatrix, float rPercentage, float rMorphPct)
{
	Quat	qFrom1;
	Quat	qTo1;
	Quat	qFrom2;
	Cartesian4x4 c4X;
/*
 *	rMorphPct from tsqFrom to (rPercentage from unit to g3NewMatrix)
 */
	qFrom1.Identity();
	qTo1 = g4NewMatrix;
	qFrom2 = tsqFrom;
	qFrom1 = qFrom1.Slerp(qTo1, rPercentage);
	qFrom1 = qFrom2.Slerp(qFrom1, rMorphPct);
	c4X = qFrom1.ConvertToC4x4();
	SetBoneMatrix(boneID, c4X);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::GetBoneCompositeMatrix(BoneID boneID, Graphics4x4 &g4OutputMatrix, int index)
{
	boneGetCompositeMatrix(Instance(index)->Body(), boneID,
			&g4OutputMatrix);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::GetBoneCompositeMatrix(BoneID boneID, Matrix3x3
		&g3OutputMatrix, int index)
{
	Graphics4x4 g4OutputMatrix;
	boneGetCompositeMatrix(Instance(index)->Body(), boneID,
			&g4OutputMatrix);

	g3OutputMatrix = g4OutputMatrix;
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::GetParentBoneCompositeMatrix(BoneID boneID, Graphics4x4 &g4OutputMatrix, int index)
{
	boneGetParentCompositeMatrix(Instance(index)->Body(), boneID,
			&g4OutputMatrix);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::GetParentBoneCompositeMatrix(BoneID boneID, Matrix3x3
		&g3OutputMatrix, int index)
{
	Graphics4x4 g4OutputMatrix;
	boneGetParentCompositeMatrix(Instance(index)->Body(), boneID,
			&g4OutputMatrix);

	g3OutputMatrix = g4OutputMatrix;
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::GetBoneToModel(BoneID boneID, Graphics4x4 &g4OutputMatrix, int index)
{
	boneGetCurrentBoneToModel(Instance(index)->Body(), boneID,
			&g4OutputMatrix);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::GetBoneToModel(BoneID boneID, Matrix3x3
		&g3OutputMatrix, int index)
{
	Graphics4x4 g4OutputMatrix;
	boneGetCurrentBoneToModel(Instance(index)->Body(), boneID,
			&g4OutputMatrix);

	g3OutputMatrix = g4OutputMatrix;
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
DirCos3x3 CActor::GetRelativeMatrix(BoneID boneID, const Matrix3x3 &g3Absolute)
{
	Graphics4x4 g4Parent;
	Matrix3x3 g3Working;
	Matrix3x3 g3Working2;
	GetParentBoneCompositeMatrix(boneID, g4Parent);
	g3Working = g4Parent;
	g3Working.Invert();
	g3Working2 = g3Absolute;
	g3Working2 *= g3Working;
	return(*(DirCos3x3*)&g3Working2);
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetBoneVisibility(BoneID boneID, bool bVisible)
{
	if(boneID != INVALID_BONEID)
	{
		ts_Bone *pBone = GetBone(boneID);
		boneSetVisibility(pBone, bVisible);
	}
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
bool CActor::GetBoneVisibility(BoneID boneID)
{
	if (boneID == INVALID_BONEID)
		return(false);
	ts_Bone *pBone = GetBone(boneID);
	return(boneGetVisibility(pBone));
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
void CActor::SetProceduralPosition(float zeroToOne, int motionIndex)
{
	CMotion__SetProceduralPosition( &d_motnInf[motionIndex], zeroToOne );
}

void CActor::SetSecondaryProceduralPosition(float zeroToOne)
{
	CMotion__SetProceduralPosition( &d_motnInf[MOTION2], zeroToOne );
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
Vector3 CActor::GetBoneWorldEndPosition(BoneID boneID, Vector3CRef v3Offset)
{
	return(PointModelToWorld(GetEndPosition(boneID) + v3Offset));
}

Vector3 CActor::GetBoneWorldEndPosition(BoneID boneID)
{
	return(PointModelToWorld(GetEndPosition(boneID)));
}

/**********************************************************************
 *
 *  Class:       CActor
 *
 **********************************************************************/
Vector3 CActor::GetBoneWorldStartPosition(BoneID boneID,
		Vector3CRef v3Offset)
{
	return(PointModelToWorld(GetStartPosition(boneID) + v3Offset));
}

Vector3 CActor::GetBoneWorldStartPosition(BoneID boneID)
{
	return(PointModelToWorld(GetStartPosition(boneID)));
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    EndFrame
 *  Access:      public
 *  Description: Performs generic end of frame functions.
 *               This function must be called be any overriding functions.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActor::EndFrame(void)
{
	// update actor instance's local-to-world matrix
	PlaceGeometry();

	// time to die?
	if (d_fLifetime > 0.0f)
	{
		d_fLifetime -= g_timer.GetFrameSec();
		if (d_fLifetime <= 0.0f)
		{
			d_fLifetime = 0.0f;	// must be before call to MarkForDeath, which might reset it
			MarkForDeath();
		}
	}

	// update actors light set
	//if( d_pLightInfluences )
	//	d_pLightInfluences->getLightInfluences( GetBodyInWorld( ) );

}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    PlaceGeometry
 *  Access:      public
 *  Description: Copies state information into the geometry instance.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActor::PlaceGeometry(void)
{
	for ( int g = 0 ; g < MAX_ACTOR_INSTANCES ; g++ )
	{
		if ( d_instance[ g ] && d_instance[ g ]->d_master < 0 )
		{
			Instance( g )->Matrix().Set(d_bodyToWorld);
			Instance( g )->Matrix().PokeTranslation(d_bodyInWorld);
		}
	}
	ScaleActor();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    SetLightColorDefaults
 *  Access:      public
 *  Description: sets up light color default values
 *  Arguments:   none
 *  Return:      None
 *
 **********************************************************************/
void CActor::SetLightColorDefaults()
{
#define DEFAULT_LIGHT_COLOR_MULT_STEP	0.05f
#define DEFAULT_AMBIENT_STEP	0.02f
	d_fLightColorMult = 1.0f;
	d_fLightColorMultTarget = 1.0f;
	d_bLightColorMultAtTarget = TRUE;
	d_fLightColorMultStep = DEFAULT_LIGHT_COLOR_MULT_STEP;
	d_AmbientLightMult.R = 1.0f;
	d_AmbientLightMult.G = 1.0f;
	d_AmbientLightMult.B = 1.0f;
	d_bAmbientLightMultAtTarget = TRUE;
	d_fAmbientStep = DEFAULT_AMBIENT_STEP;
	d_u16LastColorMult = 0xffff;
	d_CurrentColorMult.R = 1.0f;
	d_CurrentColorMult.G = 1.0f;
	d_CurrentColorMult.B = 1.0f;
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    SetLightColorSource
 *  Access:      public
 *  Description: sets up light color source mode. if none, real-time lighting
 *               works as usual. otherwise, may set up multipliers
 *  Arguments:   source mode
 *  Return:      None
 *
 **********************************************************************/
void CActor::SetLightColorSource(int nSource)
{
#ifndef CDROM
	/* it makes no sense to be adjusting the lights if this instance is not real-time lit */

	if (nSource != LIGHT_COLOR_SOURCE_NONE)
	{
		bool bError = false;

		if ( !Instance()->DynamicallyLit() )
			bError = true;

//		if ((Instance()->dh.u8Flags2 & INSTANCE_DYNAMIC) == 0)	// can't call Instance__GetHighestLODSOM if dynamic. i'm not sure if dynamic instances can be real-time lit
		{
			ts_PatchSOM* pSOM = Instance()->Model();
			if ( pSOM == NULL || ( pSOM->dh.u16Flags & SOMM_TRI_STRIP_NORMALS ) == 0 )
				bError = true;	// no normals, no lighting
		}
		if (bError)
		{
#ifndef GCN
//			ASSERT(false);
			g_console.Notify("*** SetLightColorSource error for actor %s\n", InstanceName());
#endif
			return;
		}
	}
#endif //_DEBUG

	/* we need to be at a known state */

	switch(nSource)
	{
	case LIGHT_COLOR_SOURCE_NONE:
		break;
	case LIGHT_COLOR_SOURCE_TERRAIN:
		if (d_nLightColorSource == LIGHT_COLOR_SOURCE_NONE)
			SetLightColorDefaults();
		else if (d_nLightColorSource == LIGHT_COLOR_SOURCE_MANUAL)
		{
			/* build from last ts_fRGB value. u16 value is 5-6-5 */

			d_CurrentColorMult.R = min(d_CurrentColorMult.R, LIGHT_COLOR_U16_MAX_MULT);
			d_CurrentColorMult.G = min(d_CurrentColorMult.G, LIGHT_COLOR_U16_MAX_MULT);
			d_CurrentColorMult.B = min(d_CurrentColorMult.B, LIGHT_COLOR_U16_MAX_MULT);
			d_u16LastColorMult  = ((int) ((float) 0x1f * d_CurrentColorMult.R / LIGHT_COLOR_U16_MAX_MULT) & 0x1f);
			d_u16LastColorMult |= ((int) ((float) 0x3f * d_CurrentColorMult.G / LIGHT_COLOR_U16_MAX_MULT) & 0x3f) << 5;
			d_u16LastColorMult |= ((int) ((float) 0x1f * d_CurrentColorMult.B / LIGHT_COLOR_U16_MAX_MULT) & 0x1f) << 11;
		}
		break;
	case LIGHT_COLOR_SOURCE_MANUAL:
		if (d_nLightColorSource == LIGHT_COLOR_SOURCE_NONE)
			SetLightColorDefaults();
		else if (d_nLightColorSource == LIGHT_COLOR_SOURCE_TERRAIN)
		{
			/* build from last u16 value. u16 value is 5-6-5 */

			d_CurrentColorMult.R = (float) ((d_u16LastColorMult >> 0) & 0x1f)  * (LIGHT_COLOR_U16_MAX_MULT / (float) 0x1f);	// red is 5 bits
			d_CurrentColorMult.G = (float) ((d_u16LastColorMult >> 5) & 0x3f)  * (LIGHT_COLOR_U16_MAX_MULT / (float) 0x3f);	// green is 6 bits
			d_CurrentColorMult.B = (float) ((d_u16LastColorMult >> 11) & 0x1f) * (LIGHT_COLOR_U16_MAX_MULT / (float) 0x1f);	// blue is 5 bits
		}
		break;
	default:
		ASSERT(0);	// unknown mode
		break;
	}

	/* and set new mode */

	d_nLightColorSource = nSource;
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    SetLightColor
 *  Access:      public
 *  Description: sets up non-ambient light color multiplier and ambient light color for
 *               actors that are affected by color data embedded in terrain
 *  Arguments:   16-bit (5-6-5) color
 *  Return:      None
 *
 **********************************************************************/
void CActor::SetLightColor(u16 u16ColorMult)
{
	// should be in terrain mode if calling this routine
	if (d_nLightColorSource != LIGHT_COLOR_SOURCE_TERRAIN)
		return;

	/* nothing to do if same as last color */

	if (u16ColorMult == d_u16LastColorMult)
		return;
	
	if ( u16ColorMult == 0xf81f )			//If this is the pink that we get from "unlit" collision, just return
		return;

	d_u16LastColorMult = u16ColorMult;

	/* convert to float multipliers and call routine to set up targets */

//yuch-- currently range is 0.0 to 1.0-- do we ever need color from terrain to exceed this? if so, adjust LIGHT_COLOR_U16_MAX_MULT
	ts_fRGB ColorMult;
	ColorMult.R = (float) ((u16ColorMult >> 0) & 0x1f)  * (LIGHT_COLOR_U16_MAX_MULT / (float) 0x1f);	// red is 5 bits
	ColorMult.G = (float) ((u16ColorMult >> 5) & 0x3f)  * (LIGHT_COLOR_U16_MAX_MULT / (float) 0x3f);	// green is 6 bits
	ColorMult.B = (float) ((u16ColorMult >> 11) & 0x1f) * (LIGHT_COLOR_U16_MAX_MULT / (float) 0x1f);	// blue is 5 bits

	/* terrain colors typically don't get very dark, so let's expand the dynamic range. the formula expands */
	/* the range LIGHT_MULT_FLOOR..LIGHT_MULT_CEILING to 0..1 */

	/* ned says-- we're getting a lot of darker areas in juju, so put the floor back at 0. actors will still reach */
	/* max brightness at LIGHT_MULT_CEILING */

#define LIGHT_MULT_FLOOR 0.0f
#define LIGHT_MULT_CEILING 0.8f
	if (ColorMult.R < LIGHT_MULT_FLOOR)
		ColorMult.R = 0.0f;
	else if (ColorMult.R > LIGHT_MULT_CEILING)
		ColorMult.R = 1.0f;
	else
		ColorMult.R = (ColorMult.R - LIGHT_MULT_FLOOR) * (1.0f / (LIGHT_MULT_CEILING - LIGHT_MULT_FLOOR));
	if (ColorMult.G < LIGHT_MULT_FLOOR)
		ColorMult.G = 0.0f;
	else if (ColorMult.G > LIGHT_MULT_CEILING)
		ColorMult.G = 1.0f;
	else
		ColorMult.G = (ColorMult.G - LIGHT_MULT_FLOOR) * (1.0f / (LIGHT_MULT_CEILING - LIGHT_MULT_FLOOR));
	if (ColorMult.B < LIGHT_MULT_FLOOR)
		ColorMult.B = 0.0f;
	else if (ColorMult.B > LIGHT_MULT_CEILING)
		ColorMult.B = 1.0f;
	else
		ColorMult.B = (ColorMult.B - LIGHT_MULT_FLOOR) * (1.0f / (LIGHT_MULT_CEILING - LIGHT_MULT_FLOOR));

	/* record the last known three-float version of the color, too. it is possible to mix calls to SetLightColor(u16) */
	/* with calls to SetLightColor(ts_fRGB), so we want both to be current */

	d_CurrentColorMult = ColorMult;

	/* and calculate new targets */

	CalculateLightColorMult();
//yuch-- we don't consider height above the ground when processing the new color. my gut feeling is that there should be
// some range above the ground between which the color moves from the actual color in the terrain to white, under the theory
// that as you get higher, you get out from behind things casting shadows on the ground. like fog distance, there should
// probably be a start height and an end height-- above the end height, always use white. such a check DOES NOT belong here--
// it should be handled at the calling site. this routine should be "pure," and can be used for color setting that is not
// related to colors embedded in the collision data
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    SetLightColor
 *  Access:      public
 *  Description: sets up light color multipliers for ambient and directional lights
 *  Arguments:   red, green, blue multipliers (can be > 1.0)
 *  Return:      None
 *
 **********************************************************************/
void CActor::SetLightColor(ts_fRGB &ColorMult)
{
	/* should be in manual mode if calling this routine */

	//ASSERT(d_nLightColorSource == LIGHT_COLOR_SOURCE_MANUAL);

	/* nothing to do if same as last color */

	if ((ColorMult.R == d_CurrentColorMult.R) && (ColorMult.G == d_CurrentColorMult.G) && (ColorMult.B == d_CurrentColorMult.B))
		return;
	d_CurrentColorMult = ColorMult;

	/* it is possible to mix calls to SetLightColor(u16) with calls to SetLightColor(ts_fRGB), so we want both to */
	/* be current. however, it's not worth calculating the u16 value that matches the current ts_fRGB value, so just */
	/* set to an unlikely value */

	d_u16LastColorMult = 0x001f;	// full red

	/* call routine to set up targets */

	CalculateLightColorMult();
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    ResetLightColor
 *  Access:      public
 *  Description: make it so next call to SetLightColor will execute
 *  Arguments:   source mode
 *  Return:      None
 *
 **********************************************************************/
void CActor::ResetLightColor()
{
#ifndef CDROM
	/* change last values so that it will detect a difference and set new target */

	d_u16LastColorMult++;
	d_CurrentColorMult.R++;
#endif //CDROM
}

/* apply ambient light multiplier to actor's current base ambient light, which is typically the world's ambient light */

ts_fRGB CActor::AmbientLight()
{
	ts_fRGB Color;
	Color.R = d_pBaseAmbientLight->R * d_AmbientLightMult.R;
	Color.G = d_pBaseAmbientLight->G * d_AmbientLightMult.G;
	Color.B = d_pBaseAmbientLight->B * d_AmbientLightMult.B;
	return(Color);
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    CalculateLightColorMult
 *  Access:      protected
 *  Description: calculate new light color mult and ambient targets
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActor::CalculateLightColorMult()
{
	/* ambient light multiplier is same as source color multiplier, but it's subject to same floor as grayscale */
	/* directional light multiplier */

	d_AmbientLightMultTarget = d_CurrentColorMult;

	/* test against floor value. luminance equation from foley and van dam */

	float fGrayscale = (0.299f * d_CurrentColorMult.R) + (0.587f * d_CurrentColorMult.G) + (0.114f * d_CurrentColorMult.B);
	if (fGrayscale < d_fLightColorFloor)
	{
		/* have to boost rgb components to match floor value */

		float fMult;
		//if fGrayscale is zero, ignore the floor and set color to zero.
		if( Math::Zero( fGrayscale))
		{
			fMult = 0.0f;
		}
		else
		{
			fMult = d_fLightColorFloor / fGrayscale;
		}

		ASSERT( !Math::IsNaN( fMult));
		d_AmbientLightMultTarget.R *= fMult;
		d_AmbientLightMultTarget.G *= fMult;
		d_AmbientLightMultTarget.B *= fMult;
		fGrayscale = d_fLightColorFloor;
	}

	/* set new ambient color */

	d_bAmbientLightMultAtTarget = false;	// assume we'll have to move toward new target
	ASSERT((d_fAmbientStep > 0.0f) && (d_fAmbientStep <= 1.0f));	// 0.0 will never get there, 1.0 is max necessary

	/* set non-ambient light color multiplier */

	d_fLightColorMultTarget = fGrayscale;
	d_bLightColorMultAtTarget = FALSE;	// assume we'll have to move toward new target
	ASSERT((d_fLightColorMultStep > 0.0f) && (d_fLightColorMultStep <= 1.0f));	// 0.0 will never get there, 1.0 is max necessary
}

/**********************************************************************
 *
 *  Class:       CActor
 *  Function:    AdvanceLightColorMult
 *  Access:      public
 *  Description: move towards targets
 *  Arguments:   none
 *  Return:      None
 *
 **********************************************************************/
void CActor::AdvanceLightColorMult()
{
	/* should be in some lighting mode if calling this routine */

	ASSERT(d_nLightColorSource != LIGHT_COLOR_SOURCE_NONE);

	/* move ambient light towards target */

	if (d_bAmbientLightMultAtTarget == FALSE)
	{
		float fRDelta = d_AmbientLightMultTarget.R - d_AmbientLightMult.R;
		float fGDelta = d_AmbientLightMultTarget.G - d_AmbientLightMult.G;
		float fBDelta = d_AmbientLightMultTarget.B - d_AmbientLightMult.B;

		/* which is largest? */

		float fDelta = max(Math::Abs(fRDelta), Math::Abs(fGDelta));
		fDelta = max(fDelta, Math::Abs(fBDelta));

		/* check largest delta against step size */

		if (fDelta <= d_fAmbientStep)
		{
			/* largest delta within step range, ok to go to target */

			ForceAmbientToTarget();
		}
		else
		{
			/* we want rgb channels to move towards target proportionally so we get a smooth transition. */
			/* we know that the largest delta will move the step size, so all the others should move */
			/* in proportion to the largest delta */

			float fStepOverDelta = d_fAmbientStep / fDelta;
			ASSERT( !Math::IsNaN( fStepOverDelta));
			d_AmbientLightMult.R += fRDelta * fStepOverDelta;
			d_AmbientLightMult.G += fGDelta * fStepOverDelta;
			d_AmbientLightMult.B += fBDelta * fStepOverDelta;
		}
	}

	/* move light color multiplier towards target */

	if (d_bLightColorMultAtTarget == FALSE)
	{
		float fDelta = d_fLightColorMultTarget - d_fLightColorMult;
		if (fDelta < -d_fLightColorMultStep)
			d_fLightColorMult -= d_fLightColorMultStep;	// move "step" amount towards target
		else if (fDelta > d_fLightColorMultStep)
			d_fLightColorMult += d_fLightColorMultStep;	// move "step" amount towards target
		else
		{
			/* within step range, ok to go to target */

			ForceLightColorMultToTarget();
		}
	}
}

/* go to target values immediately */

void CActor::ForceLightColorMultToTarget()
{
	d_fLightColorMult = d_fLightColorMultTarget;
	d_bLightColorMultAtTarget = true;
}
void CActor::ForceAmbientToTarget()
{
	ASSERT( !Math::IsNaN( d_AmbientLightMultTarget.R));
	ASSERT( !Math::IsNaN( d_AmbientLightMultTarget.G));
	ASSERT( !Math::IsNaN( d_AmbientLightMultTarget.B));
	d_AmbientLightMult = d_AmbientLightMultTarget;
	d_bAmbientLightMultAtTarget = true;
}
void CActor::ForceLightColorToTarget()
{
	ForceLightColorMultToTarget();
	ForceAmbientToTarget();
}

// update this actors bounding volumes for every geometry
void CActor::UpdateGeomBoundingVolumes()
{
	// loop through all geoms and update bounding volumes
	for ( int i = 0 ; i < MAX_ACTOR_INSTANCES ; i++ )
		if ( Instance( i ) )
			Instance( i )->UpdateBoundingVolumes();
}

//this version allows the application to ignore the off-screen-dont-update feature by setting bForceUpdate to TRUE
void CActor::RunAnimation(bool bForceUpdate)
{
	int	geomIndex = 0;

#if 0 //debug: when breakpointing and halting and such, use a constant slow update rate
	float secondsSinceLastTick = 0.07;
#else
	float secondsSinceLastTick = g_timer.GetFrameSec();
#endif
	STAGE_PROFILE_START(CStageProfile::CHAIN_PREP);

	if (g_timer.IsPaused())
		return;

	if (!IsActive() && !bForceUpdate)
		return;
	
	if (d_chains)
		d_chains->SetDeltaSec( secondsSinceLastTick );
	if ((d_chains || d_meshList) && (DistortionMgr__BuildApplyList(NULL, DISTORTION_AFFECTS_CHAINS)))
		DistortionMgr__TransformDistortions(NULL, NULL);
	STAGE_PROFILE_END(CStageProfile::CHAIN_PREP);

	STAGE_PROFILE_START(CStageProfile::ANIM_PROCESS);

	//process animation script(s)
	int i;
	for ( i = 0 ; i < AI_SCRIPT ; i++ ) // AI_SCRIPT is last, all others are anim scripts
		if ( Script( i ) )
			Script( i )->Run( g_timer.GetFrameSec() );

#ifdef ANIMDEBUGINFO
	//this debug info is dumped out right here so that it is accurate during pause-mode single frame step mode 
	if(secondsSinceLastTick
		&& stricmp(InstanceName(), "avatar") != 0) //choose instance to include or exclude
	{
		//print some animation debug info
		DEBUGOUT( 2, Printf("Action=0x%hx, ActionFlags=0x%x", Instance(d_skeletons[PRIMARY_SKELETON])->Script[ANIM1_SCRIPT].sAction, Instance(d_primarySkeleton)->u32ActionFlags));
		DEBUGOUT( 3, Printf("MFile=%*.*s, animRate=%1.3f", MHDR_NAME_LEN, MHDR_NAME_LEN, Instance(d_skeletons[PRIMARY_SKELETON])->motnInf[MOTION1].pCurMHdr->name, GetAnimationRate()));
		if(CMotion__IsInterpDone(&d_motnInf[MOTION1]))
		{
			DEBUGOUT( 4, Printf("F %d", (int)( SECONDS_TO_ANIMFRAMES( Instance(d_skeletons[PRIMARY_SKELETON])->motnInf[MOTION1].currentTime))));
//pab			DEBUGOUT( 5, Printf("F %d", (int)(Geom(d_secondarySkeleton)->motnInf[MOTION1].usCurrentTime / JIFFIES_PER_ANIMFRAME)));
//pab			DEBUGOUT( 6, Printf("F %d", (int)(Geom(d_tertiarySkeleton)->motnInf[MOTION1].usCurrentTime / JIFFIES_PER_ANIMFRAME)));
		}
		else
		{
			DEBUGOUT( 4, Printf("I %d F %d", (int)( SECONDS_TO_ANIMFRAMES( Instance(d_skeletons[PRIMARY_SKELETON])->motnInf[MOTION1].interpTime )), 
				(int)( SECONDS_TO_ANIMFRAMES( Instance()->motnInf[MOTION1].currentTime ))));
//pab			DEBUGOUT( 5, Printf("I %d F %d", (int)(Geom(d_secondarySkeleton)->motnInf[MOTION1].sInterpTime / JIFFIES_PER_ANIMFRAME), (int)(Geom()->motnInf[MOTION1].usCurrentTime / JIFFIES_PER_ANIMFRAME)));
//pab			DEBUGOUT( 6, Printf("I %d F %d", (int)(Geom(d_tertiarySkeleton)->motnInf[MOTION1].sInterpTime / JIFFIES_PER_ANIMFRAME), (int)(Geom()->motnInf[MOTION1].usCurrentTime / JIFFIES_PER_ANIMFRAME)));
		}
	}
#endif

	// CompActors want an entry point after running animation and AI scripts, but before the bones are actually run.
	// this is the only place to do it
	if( IsA( CCompActor::ClassType() ) )
		((CCompActor *)this)->RunBones();

	//process motion data
	if ( secondsSinceLastTick || bForceUpdate )
	{
		UpdateMesh( secondsSinceLastTick );

		// update all the masters
		int g;
		for ( g = 0 ; g < MAX_ACTOR_INSTANCES ; g++ )
		{
			if ( d_instance[ g ] && d_instance[ g ]->d_master < 0 && d_instance[ g ]->GetMotionChannelTable() )
			{
				d_instance[ g ]->GetMotionChannelTable()->Process( secondsSinceLastTick, ( d_bFirstFrame | bForceUpdate | InScene( g ) ) );
			}
		}

		// update all the slaves
		for ( g = 0 ; g < MAX_ACTOR_INSTANCES ; g++ )
		{
			if ( d_instance[ g ] && d_instance[ g ]->d_master >= 0 && d_instance[ g ]->d_boneName[ 0 ] != 0 )
			{
				Graphics4x4 m( d_bodyToWorld, d_bodyInWorld );
				CMotion__UpdateSlavePositions( d_instance[ d_instance[ g ]->d_master ], d_instance[ g ], m );
			}
		}
	}

	// not the first frame anymore, duh
	if( d_bFirstFrame )
		d_bFirstFrame = false;

//	CInstance::UpdateBoundingVolumes(Geom()->Inst());
	STAGE_PROFILE_END(CStageProfile::ANIM_PROCESS);
}

void CActor::UpdateMesh(float i_deltaSec)
{
	if (d_meshList)
	{
		d_meshList->Update(i_deltaSec);
#ifdef SINGLE_MESH
		Instance()->BoundingSphere() = d_meshList->GetInstance(0).BoundingSphere();
		Instance()->BoundingBox()    = d_meshList->GetInstance(0).BoundingBox();
#endif
	}
}

void CActor::LoadDynamicLinkageParams(void)
{
	if (d_chains)
		d_chains->LoadParams();
}

bool CActor::GetDynamicExtents(Vector3 &io_minExtent, Vector3 &io_maxExtent)
{
	if ((!IsActive()) || ((!d_chains || !InScene()) &&
			(!d_meshList || !d_meshList->NeedObjectList())))
		return(false);

	if (d_chains)
		d_chains->GetExtents(io_minExtent, io_maxExtent, false);
	if (d_meshList)
		d_meshList->GetExtents(io_minExtent, io_maxExtent, (d_chains != NULL));

	return(true);
}

void CActor::SetObjectList(CObjectList* i_objectList)
{
	if (d_chains)
		d_chains->SetObjectList(i_objectList);
	if (d_meshList)
		d_meshList->SetObjectList(i_objectList);
}

void CActor::ResetDynamicElements(void)
{
	if (d_chains)
		d_chains->ResetChains();
	if (d_meshList)
		d_meshList->Reset();
}

void CActor::SetInvolvement(uint i_involvement)
{
	i_involvement &= INVOLVE_MASK;
	d_involvement = i_involvement | (i_involvement << INVOLVE_SHIFT);
}

void CActor::RequestInvolvement(uint i_involvement)
{
	d_involvement &= ~INVOLVE_MASK;
	d_involvement |= i_involvement & INVOLVE_MASK;
}

void CActor::AcceptInvolvement(void)
{
	SetInvolvement(d_involvement);
}

uint CActor::GetCurrentInvolvement(void)
{
	return((d_involvement >> INVOLVE_SHIFT) & INVOLVE_MASK);
}

uint CActor::GetDesiredInvolvement(void)
{
	return(d_involvement & INVOLVE_MASK);
}

bool CActor::IsActive(void)
{
	return((d_involvement & INVOLVE_ACTIVE) != 0);
}

#ifdef ENABLE_RECORD

bool CActor::CreateRecordBuffer(int i_numTicks)
{
	if (d_recordBuffer)
		return (false);

	MEM_SET_ONE_SHOT_NEW_NAME("RecBuffer");
	d_recordBuffer = new SRecordEntry[i_numTicks];
	if (d_recordBuffer == NULL)
		return (false);

	d_numRecordBufferEntries = i_numTicks;
	d_nextWriteEntry = 0;
	d_bufferFull = false;
	return (true);
}

void CActor::DeleteRecordBuffer(void)
{
	if (d_recordBuffer)
		delete[] d_recordBuffer;
	d_recordBuffer = 0;
	d_recordState = IDLE;
}

void CActor::SetIdle(void)
{
	d_recordState = IDLE;
}

void CActor::SetRecord(void)
{
	if (!d_recordBuffer || (d_recordState == RECORDING))
		return;

	d_bufferFull = false;
	d_nextWriteEntry = 0;
	d_recordState = RECORDING;
}

int CActor::FindEntry(float i_targetTime)
{
	if (!d_recordBuffer)
		return(-1);

	int startEntry, endEntry;

	if (d_bufferFull)
	{
		startEntry = d_nextWriteEntry;
		if (d_nextWriteEntry == 0)
			endEntry = d_numRecordBufferEntries - 1;
		else
			endEntry = d_nextWriteEntry - 1;
	}
	else
	{
		startEntry = 0;
		endEntry = d_nextWriteEntry - 1;
		if (endEntry < 0)
			return (-1);
	}

	int entry = startEntry;
	int nextEntry = startEntry;
	while (entry != endEntry)
	{
		if (i_targetTime < d_recordBuffer[nextEntry].d_entrySec)
			break;
		entry = nextEntry;
		if (++nextEntry >= d_numRecordBufferEntries)
			nextEntry = 0;
	}
	return(entry);
}

void CActor::SetPlayback(float i_targetTime)
{
	d_currentReadEntry = FindEntry(i_targetTime);
	if (d_currentReadEntry < 0)
		d_recordState = IDLE;
	else
	{
		float deltaSec;
		d_recordState = PLAYING;
		ManageRecording(0.0f, deltaSec);
	}
}

bool CActor::ManageRecording(float i_frameOffsetSec, float &io_deltaSec)
{
	switch (d_recordState)
	{
	case RECORDING:
		RecordState(d_nextWriteEntry);
		if (d_recordBuffer)
		{
			d_recordBuffer[d_nextWriteEntry].d_deltaSec = io_deltaSec;
			d_recordBuffer[d_nextWriteEntry].d_entrySec =
					g_timer.GetStartSec() + i_frameOffsetSec;
		}
		d_nextWriteEntry++;
		if (d_nextWriteEntry >= d_numRecordBufferEntries)
		{
			d_nextWriteEntry = 0;
			d_bufferFull = true;
		}
		break;
	case PLAYING:
		LoadState(d_currentReadEntry);
		if (d_recordBuffer)
			io_deltaSec = d_recordBuffer[d_currentReadEntry].d_deltaSec;
		return(true);
	}
	return(false);
}

void CActor::RecordState(int i_entry)
{
	if (d_recordBuffer)
	{
		d_recordBuffer[i_entry].d_bodyInWorld = d_bodyInWorld;
		d_recordBuffer[i_entry].d_bodyToWorld = d_bodyToWorld;
		d_recordBuffer[i_entry].d_velocityInWorld = d_velocityInWorld;
		d_recordBuffer[i_entry].d_omegaInBody = d_omegaInBody;
	}
}

void CActor::LoadState(int i_entry)
{
	if (d_recordBuffer)
	{
		d_bodyInWorld = d_recordBuffer[i_entry].d_bodyInWorld;
		d_bodyToWorld = d_recordBuffer[i_entry].d_bodyToWorld;
		d_velocityInWorld = d_recordBuffer[i_entry].d_velocityInWorld;
		d_omegaInBody = d_recordBuffer[i_entry].d_omegaInBody;
		SyncState();
		SyncActor();
	}
}

float CActor::GetRecordingStart(void)
{
	if (!d_recordBuffer)
		return (-1.0f);

	if (d_bufferFull)
		return (d_recordBuffer[d_nextWriteEntry].d_entrySec);

	if (d_nextWriteEntry == 0)
		return (-1.0f);

	return (d_recordBuffer[0].d_entrySec);
}

float CActor::GetRecordingEnd(void)
{
	if (!d_recordBuffer)
		return (-1.0f);

	if (d_bufferFull)
	{
		if (d_nextWriteEntry == 0)
			return (d_recordBuffer[d_numRecordBufferEntries - 1].d_entrySec);
		else
			return (d_recordBuffer[d_nextWriteEntry - 1].d_entrySec);
	}

	if (d_nextWriteEntry == 0)
		return (-1.0f);

	return (d_recordBuffer[d_nextWriteEntry - 1].d_entrySec);
}

float CActor::GetForwardStepSize(float i_currentTime)
{
	int entry = FindEntry(i_currentTime);
	if (entry < 0)
		return (-1.0f);

	entry++;
	if (d_bufferFull && (entry >= d_numRecordBufferEntries))
		entry = 0;
	if (entry == d_nextWriteEntry)
		return(-1.0f);
	return(d_recordBuffer[entry].d_entrySec - i_currentTime);
}

float CActor::GetReverseStepSize(float i_currentTime)
{
	int entry = FindEntry(i_currentTime);
	if (entry < 0)
		return (-1.0f);

	if (d_bufferFull)
	{
		if (entry == d_nextWriteEntry)
			return(-1.0f);
		entry--;
		if (entry < 0)
			entry = d_numRecordBufferEntries - 1;
	}
	else if (entry == 0)
		return(-1.0f);
	else
		entry--;

	return(i_currentTime - d_recordBuffer[entry].d_entrySec);
}

#endif

// ------------------------------- Save/Load Game Functions ----------------------------------

const u32 CActor::GetLocalSaveGameMemoryNeeded( void ) const
{
	return( Align_RoundTo16( sizeof( cactorSaveGameData ) ) );
}

// -------------------------------------------------------------------------------------------

const u32 CActor::GetSaveGameMemoryNeeded( void )
{
	return GetLocalSaveGameMemoryNeeded();
}

// -------------------------------------------------------------------------------------------
const u32 CActor::SerializeSaveGame( char *pBuf, bool bDirection )
{
	// --- Save Data?
	if( bDirection )
	{
		cactorSaveGameData *pSaveTo = ( cactorSaveGameData * )pBuf;
		
		pSaveTo->d_bodyInWorld				= d_bodyInWorld;
		pSaveTo->d_velocityInWorld			= d_velocityInWorld;
		pSaveTo->d_omegaInBody				= d_omegaInBody;
		pSaveTo->d_bodyToWorld				= d_bodyToWorld;
		pSaveTo->d_fLifetime					= d_fLifetime;
		pSaveTo->d_lienCount					= d_lienCount;
		pSaveTo->d_deleteWhenNoCast		= d_deleteWhenNoCast;
		pSaveTo->d_markedForDeath			= d_markedForDeath;
		pSaveTo->d_involvement				= d_involvement;
//		pSaveTo->d_dh							= Instance( ).dh;
		pSaveTo->m_scriptsSaved				= 0;
		pSaveTo->m_ignoreEntityID			= 0;
		if( d_ignoreEntity && d_ignoreEntity->IsA( CActor::ClassType( ) ) )
		{
			CActor *pCActor = ( CActor * )d_ignoreEntity;
			pSaveTo->m_ignoreEntityID = pCActor->getID( );
		}

		for( int i = 0; i < MAX_SCRIPTS; i++ )
		{
			pSaveTo->d_motnInf[ i ].u32MotnFlags	= d_motnInf[ i ].u32MotnFlags;
			pSaveTo->d_motnInf[ i ].animationRate	= d_motnInf[ i ].animationRate;
			pSaveTo->d_motnInf[ i ].blendRatio		= d_motnInf[ i ].blendRatio;
			pSaveTo->d_motnInf[ i ].interpTime		= d_motnInf[ i ].interpTime;
			pSaveTo->d_motnInf[ i ].currentTime		= d_motnInf[ i ].currentTime;
			pSaveTo->d_uStartMotionFlags[ i ]		= ( u32 )d_motnInf[ i ].pMotionHeader;
		}
		// --- serialize the scripts
		pSaveTo->m_scriptsSaved = false;
	}

	// --- Load Data?
	else
	{
		cactorSaveGameData *pLoadFrom = ( cactorSaveGameData * )pBuf;
		
		d_bodyInWorld							= pLoadFrom->d_bodyInWorld;
		d_velocityInWorld						= pLoadFrom->d_velocityInWorld;
		d_omegaInBody							= pLoadFrom->d_omegaInBody;
		d_bodyToWorld							= pLoadFrom->d_bodyToWorld;
		d_fLifetime								= pLoadFrom->d_fLifetime;
		d_lienCount								= pLoadFrom->d_lienCount;
		d_deleteWhenNoCast					= pLoadFrom->d_deleteWhenNoCast;
		d_markedForDeath						= pLoadFrom->d_markedForDeath;
		d_involvement							= pLoadFrom->d_involvement;
//		Instance( ).dh							= pLoadFrom->d_dh;
		d_ignoreEntity							= NULL;

		// --- ignore another actor for collision if we had a pointer to him
		if( pLoadFrom->m_ignoreEntityID )
		{
			// --- this is ok, since it will return null if the actor no longer exists
			d_ignoreEntity = g_stage.MasterCast().FindActorByID( pLoadFrom->m_ignoreEntityID );
		}
		// Needed to update matricies else d_bodyInWorld != matrix translation positions. Jon M.
		SyncActor();


		// --- make sure the script wants to be serialized
		for( int i = 0; i < MAX_SCRIPTS; i++ )
		{
			d_motnInf[ i ].u32MotnFlags		= pLoadFrom->d_motnInf[ i ].u32MotnFlags;
			d_motnInf[ i ].animationRate		= pLoadFrom->d_motnInf[ i ].animationRate;
			d_motnInf[ i ].blendRatio			= pLoadFrom->d_motnInf[ i ].blendRatio;
			d_motnInf[ i ].interpTime			= pLoadFrom->d_motnInf[ i ].interpTime;
			d_motnInf[ i ].currentTime			= pLoadFrom->d_motnInf[ i ].currentTime;
		}
	}


	// --- return amount of memory we serialized in
	return( CActor::GetSaveGameMemoryNeeded( ) );
}

// ------------------------------------------------------------------------------------------------------

void CActor::setAsDynamicCActor( )
{ 
	m_uiUniqueID = g_CIdManager.setAsDynamicId( m_uiUniqueID );
}

// ------------------------------------------------------------------------------------------------------

void CActor::Draw(CCollisionEvent* i_event)
{
	if (d_collisionConfiguration == SIMPLE)
		d_collisionObject->Draw(DrawUtility::RED);
}

// ------------------------------------------------------------------------------------------------------

const char* CActor::GetClassName(void)
{
	return("Actor");
}

// ------------------------------------------------------------------------------------------------------

#ifndef CDROM
void CActor::RecordReplayInterface( void *object )
{
	( ( CActor * )object )->RecordReplayData( );
}

void CActor::PlayReplayInterface( void *object )
{
	( ( CActor * )object )->PlayReplayData( );
}

// ------------------------------------------------------------------------------------------------------

void CActor::RecordReplayData( )
{
	ts_Body *pBody	= Instance()->Body();

	g_replayMgr.RecordReplayData( ( u8 * )&( Instance()->Matrix() ), sizeof( Graphics4x4 ) );

	// --- make sure we have a body
	if( pBody )
	{
		unsigned short		usBones	= pBody->usBones;
		ts_Bone				*pBones	= pBody->pBones;

		for( int i = 0; i < usBones; i++ )
		{
			g_replayMgr.RecordReplayData( ( u8 * )&( pBones[ i ].Matrix ), sizeof( Graphics4x4 ) );
		}
	}
}

// ------------------------------------------------------------------------------------------------------

void CActor::PlayReplayData( )
{
	ts_Body *pBody	= Instance()->Body();

	g_replayMgr.PlayReplayData( ( u8 * )&( Instance()->Matrix() ), sizeof( Graphics4x4 ) );

	// --- make sure we have a body
	if( pBody )
	{
		unsigned short		usBones	= pBody->usBones;
		ts_Bone				*pBones	= pBody->pBones;

		for( int i = 0; i < usBones; i++ )
		{
			g_replayMgr.PlayReplayData( ( u8 * )&( pBones[ i ].Matrix ), sizeof( Graphics4x4 ) );
		}
	}
}
#endif

// ------------------------------------------------------------------------------------------------------

s16 CActor::AnimAction()
{
	return ( Script( ANIM1_SCRIPT ) ) ? Script( ANIM1_SCRIPT )->GetAction() : 0;
}

s16 CActor::AnimActionSecondary( int anim )
{
	return ( Script( anim ) ) ? Script( anim )->GetAction() : 0;
}

const char *CActor::GetMotionString( const char *name, int index )
{
	// User data is set up as follows:
	// After the motion header, there is an "array" of string pairs, key and value.
	// Each is null terminated, and there are two nulls after the last one.  So, we scan
	// through by incrementing past the nulls to get the strings
	char *pStr;
	pStr = (char *)(GetMotnInf( index )->pMotionHeader);
	ASSERT( pStr && "You cannot get motion vars unless a motion is currently playing" );
	if( pStr )
	{
		pStr += sizeof( ts_MotionHeader );
		while( *pStr )
		{
			if( strcmp( pStr, name ) == 0 )
			{
				pStr += strlen( pStr ); // skip to the terminating zero
				pStr++; // skip over the terminator.
				return pStr;
			}
			pStr += strlen( pStr ); // skip over this key
			pStr++; // skip over the terminator
			pStr += strlen( pStr ); // skip over this value
			pStr++; // skip the terminator
		}
	}
	return NULL;
}

float CActor::GetMotionFloat( const char *name, int index )
{
	float f = 0.0f;
	const char *text = GetMotionString( name, index );
	
	if( text )
	{
		f = (float)atof( text );
	}

	return f;
}

int CActor::GetMotionInt( const char *name, int index )
{
	int i = 0;
	const char *text = GetMotionString( name, index );
	
	if( text )
	{
		i = atoi( text );
	}

	return i;
}

// ------------------------------------------------------------------------------------------------------

void CActor::StartMotion( const char* name, s32 startFrame, u32 flags, u32 boneGroupBitMask /* = 0xffffffff */ )
{
	ASSERT( name && name[0] );

	int geomIndex;
	// get our geometry index
	switch ( flags & ( MOTNINF_FLAG_PRIMARY_SKELETON | MOTNINF_FLAG_SECONDARY_SKELETON | MOTNINF_FLAG_TERTIARY_SKELETON ) )
	{
		default:											ASSERTS( false, "can't combine skeletons" );	// fall through
		case 0:
		case MOTNINF_FLAG_PRIMARY_SKELETON:		geomIndex = d_skeletons[PRIMARY_SKELETON];	break;
		case MOTNINF_FLAG_SECONDARY_SKELETON:	geomIndex = d_skeletons[SECONDARY_SKELETON];	break;
		case MOTNINF_FLAG_TERTIARY_SKELETON:	geomIndex = d_skeletons[TERTIARY_SKELETON];	break;
	}

	// lookup the motion ID
	ts_MotionHeader* motHdr = Instance( geomIndex )->LookupMotion( name );

	StartMotion( motHdr, startFrame, flags, boneGroupBitMask );
}

// ------------------------------------------------------------------------------------------------------

void CActor::StartMotion( ts_MotionHeader* motHdr, s32 startFrame, u32 flags, u32 boneGroupBitMask /* = 0xffffffff */ )
{
	// which motionIndex are we controlling?
	int nIndex;
	u32 priority;
	switch (flags & (MOTNINF_FLAG_MOTION2 | MOTNINF_FLAG_MOTION3 | MOTNINF_FLAG_MOTION4))
	{
		default:							ASSERTS( false, "can't combine motion bits" );	// fall through
		case 0:							nIndex = MOTION1;	priority = 103;	break;
		case MOTNINF_FLAG_MOTION2:	nIndex = MOTION2;	priority = 102;	break;
		case MOTNINF_FLAG_MOTION3: nIndex = MOTION3;	priority = 101;	break;
		case MOTNINF_FLAG_MOTION4:	nIndex = MOTION4;	priority = 100;	break;
	}
  	
	ts_MotnInf* pMotnInf = &d_motnInf[ nIndex ];
	MotionChannel** ppMotionChannel = &d_motnInf[ nIndex ].currentChannel;

	//ASSERTF( !motionDisabled[nIndex], ("Trying to play motion %s, but motion is disabled", pMotnInf->pMotionHeader->d_Name) );
	if( motionDisabled[nIndex] )
		return;

	// priority overrides
	if ( flags & MOTNINF_FLAG_PRIORITY_LOW )
		priority += 4; // 0 is the highest
	if ( flags & MOTNINF_FLAG_PRIORITY_HIGH )
		priority -= 4; // 0 is the highest

	// PAB - added for multiple mesh/skeleton support
	// set our geometry index
	int geomIndex = -1; 
	switch ( flags & ( MOTNINF_FLAG_PRIMARY_SKELETON | MOTNINF_FLAG_SECONDARY_SKELETON | MOTNINF_FLAG_TERTIARY_SKELETON ) )
	{
		default:											ASSERTS( false, "can't combine skeletons" );	// fall through
		case 0:
		case MOTNINF_FLAG_PRIMARY_SKELETON:		geomIndex = d_skeletons[PRIMARY_SKELETON];	break;
		case MOTNINF_FLAG_SECONDARY_SKELETON:	geomIndex = d_skeletons[SECONDARY_SKELETON];	break;
		case MOTNINF_FLAG_TERTIARY_SKELETON:	geomIndex = d_skeletons[TERTIARY_SKELETON];	break;
	}
		
	// get out if we're not supposed to restart if we're already playing this motion
	if ( ( flags & MOTNINF_FLAG_IFNOTSAME ) && ( motHdr == pMotnInf->pMotionHeader ) )
		return;

	pMotnInf->pNextMotionHeader = motHdr;

	//startFrame == -1 is flag to start new anim at a frame number that matches the existing anim's frame number
	if( startFrame == ANIM_START_FRAME_PROPORTIONAL )
	{
		//compute the starting frame number: goto the frame in the new motion that is proportional to the current frame and motion
		float ratio;
		if( pMotnInf->pMotionHeader )
			ratio = pMotnInf->currentTime / CMotion__GetAnimTime( pMotnInf->pMotionHeader );
		else // this is no current motion being played, so pick a reasonable default start position
			ratio = 0.5f;
		startFrame = (s32)((float)CMotion__GetAnimFrames( pMotnInf->pNextMotionHeader ) * ratio);
	}
	
	pMotnInf->currentTime = (float)startFrame * SECONDS_PER_ANIMFRAME;
	
	//startFrame == -2 is flag to start new anim at the very end (minus 1 tick)...presumably so it can be played backwards
	if( startFrame == ANIM_START_FRAME_LAST)
	{
		//compute the starting time: goto the time at the end of the motion
		pMotnInf->currentTime = CMotion__GetAnimTime( pMotnInf->pNextMotionHeader );
	}

	//remove anything currently going on this motion channel
	Instance(geomIndex)->GetMotionChannelTable()->RemoveChannel( ppMotionChannel );
	
	//if the previous motion is the same as the current motion and the NOINTERPSAME flag is set, then clear the INTERP flag, else set it
	if( flags & MOTNINF_FLAG_INTERPIFNOTSAME )
	{
		if( pMotnInf->pMotionHeader == pMotnInf->pNextMotionHeader )
			flags |= MOTNINF_FLAG_NOINTERP;
		else
			flags &= ~MOTNINF_FLAG_NOINTERP;
	}

	//fire up this new animation
	pMotnInf->pMotionHeader = pMotnInf->pNextMotionHeader;
	
	pMotnInf->u32MotnFlags = flags;
	
	//get the mask of bone groups - flip if mirrored
	if(pMotnInf->u32MotnFlags & MOTNINF_FLAG_MIRROR)
		boneGroupBitMask &= CBoneGroup__MirrorMask(pMotnInf->pMotionHeader->d_BoneGroupBitMask, Instance( geomIndex )->Body()->pBoneGroupInfo );
	else
		boneGroupBitMask &= pMotnInf->pMotionHeader->d_BoneGroupBitMask;
	
	//this assumes that the app is topMinusOne setting animations...only from scripts.
	pMotnInf->inst = Instance( geomIndex );

	Instance( geomIndex )->GetMotionChannelTable()->AddChannel(
		&CMotion__Animate,				//routine to animate this data
		NULL,									//C++ context
		priority,							//priority - 0 is the highest
		boneGroupBitMask,					//bonegroup mask
		pMotnInf,							//addr of motInfo
		ppMotionChannel					//this gets filled in by CMotionChannel__Add
	);
}

// ------------------------------------------------------------------------------------------------------

void CActor::StopMotion( const char* name, int motionIndex )
{
	// stop motion "name".  if name is null or an empty string, then stop whatever motion
	// is currently playing.
	ts_MotionHeader* motHdr = NULL;

	if ( name && name[ 0 ] )
	{
		motHdr = d_motnInf[ motionIndex ].inst->LookupMotion( name );
	}

	StopMotion( motHdr, motionIndex );
}

// ------------------------------------------------------------------------------------------------------

void CActor::StopMotion( ts_MotionHeader* motHdr, int motionIndex /* = MOTION1 */ )
{
	// if motHdr is currently playing then stop it.  if motHdr is NULL stop whatever is playing.
	if ( ( motHdr == NULL ) || ( GetMotnInf()->pMotionHeader == motHdr ) )
	{
		if ( d_motnInf[ motionIndex ].inst )
		{
			d_motnInf[ motionIndex ].inst->GetMotionChannelTable()->RemoveChannel( &d_motnInf[ motionIndex ].currentChannel );
		}
	}
}

// ------------------------------------------------------------------------------------------------------

void CActor::DistributeNetActor(bool i_persistent)
{
	ASSERT(d_netActor == NULL);

	if (ValidSingleton(CSession) && g_session.IsNetworked())
	{
		d_netActor = CreateNetActor();
		if (d_netActor != NULL)
			d_netActor->Distribute(this, i_persistent);
	}
}

void CActor::BindNetActor(CNetActor* i_netActor)
{
	ASSERT((d_netActor == NULL) && (i_netActor != NULL) &&
			ValidSingleton(CSession) && g_session.IsNetworked());

	d_netActor = i_netActor;
}

void CActor::UnbindNetActor(bool i_killNetActor)
{
	if (d_netActor != NULL)
	{
		CNetActor* netActor = d_netActor;
		d_netActor = NULL;
		netActor->Unbind(i_killNetActor);
	}
}

void CActor::PropagateNetActor(float i_gameTime)
{
	if (d_netActor != NULL)
		d_netActor->Propagate(i_gameTime);
}

void CActor::SetNetMaster(bool i_netMaster)
{
	d_netMaster = i_netMaster;
	d_messageOwner.SetMaster(d_netMaster);
}

CNetActor* CActor::CreateNetActor(void)
{
#ifdef NETZ
	return(DOActor::Create());
#else
	return(NULL);
#endif
}

int CActor::GetNetMasterMetaDataSize(void)
{
	// Start with the core actor data. Include the terminating script index in
	// the count.
	int total = GetMasterMetaDataSize() + sizeof(uint8);

	// Add the scripts.
	for (int i = 0; i < MAX_SCRIPTS; i++)
	{
		if (Script(i) != NULL)
			total += Script(i)->GetMasterMetaDataSize() + sizeof(uint8);
	}

	// Meta data consists of:
	//   [MasterData][ScriptIndex][ScriptData] ... [MAX_SCRIPTS]
	return(total);
}

bool CActor::HasNetMasterMetaDataChangedSinceLastWrite(void)
{
	if (HasMasterMetaDataChangedSinceLastWrite())
		return(true);

	for (int i = 0; i < MAX_SCRIPTS; i++)
	{
		if ((Script(i) != NULL) &&
				Script(i)->HasMasterMetaDataChangedSinceLastWrite())
			return(true);
	}

	return(false);
}

void CActor::WriteNetMasterMetaData(void* &io_buffer)
{
	// Write the core actor meta data.
	WriteMasterMetaData(io_buffer);

	// Write the script meta data.
	for (int i = 0; i < MAX_SCRIPTS; i++)
	{
		if (Script(i) != NULL)
		{
			io_buffer = PackBuffer::Pack(io_buffer, (uint8)i);
			Script(i)->WriteMasterMetaData(io_buffer);
		}
	}

	// Write the terminating MAX_SCRIPTS script index.
	io_buffer = PackBuffer::Pack(io_buffer, (uint8)MAX_SCRIPTS);
}

int CActor::GetNetMasterDataSize(void)
{
	int total = GetMasterDataSize();

	for (int i = 0; i < MAX_SCRIPTS; i++)
	{
		if (Script(i) != NULL)
			total += Script(i)->GetMasterDataSize();
	}

	return(total);
}

bool CActor::HasNetMasterDataChangedSinceLastWrite(void)
{
	if (HasMasterDataChangedSinceLastWrite())
		return(true);

	for (int i = 0; i < MAX_SCRIPTS; i++)
	{
		if ((Script(i) != NULL) && Script(i)->HasMasterDataChangedSinceLastWrite())
			return(true);
	}

	return(false);
}

void CActor::WriteNetMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer)
{
	// Write the core actor data.
	WriteMasterData(io_metaDataBuffer, io_dataBuffer);

	// Process scripts.
	uint8 index;
	io_metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, index);
	while(index != (uint8)MAX_SCRIPTS)
	{
		// Write the script data.
		ASSERT_PTR(Script(index));
		Script(index)->WriteMasterData(io_metaDataBuffer, io_dataBuffer);

		// Find the script index specified in the meta data then skip the index.
		io_metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, index);
	}
}

void CActor::ReadNetMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer)
{
	// Read the core actor data.
	ReadMasterData(io_metaDataBuffer, io_dataBuffer);

	// Process scripts.
	uint8 index;
	io_metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, index);
	while(index != (uint8)MAX_SCRIPTS)
	{
		// Read the script data.
		ASSERT_PTR(Script(index));
		Script(index)->ReadMasterData(io_metaDataBuffer, io_dataBuffer);

		// Find the script index specified in the meta data then skip the index.
		io_metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, index);
	}
}

// Set the minimum time range between master data updates. The actual time is
// randomly selected in the range in an effort to distribute the updates.
void CActor::SetNetMasterDataMinUpdateTime(uint32 i_lowMS, uint32 i_highMS)
{
	if (d_netActor != NULL)
		d_netActor->SetMasterDataMinUpdateTime(i_lowMS, i_highMS);
}

// Provide an opportunity to setup net data when the master is distributed.
void CActor::SetupNetData(void)
{
	SetupDistributedState();
}

// Provide an opportunity to read the net data when the proxy is established.
void CActor::LoadNetData(void)
{
	ReadDistributedState();
}

// Request a slot with a given name and a maximum size. False is returned if the
// name is not unique (may be a hashed name).
bool CActor::RequestNetDataSlot(const char* i_name, int i_maxSize)
{
	if (d_netActor != NULL)
		return(d_netActor->RequestDataSlot(i_name, i_maxSize));
	return(true);
}

// Write data to a slot. If the slot is found and the size does not exceed the
// maximum size, the data is written into the buffer, the buffer is sent and
// true is returned.
bool CActor::WriteNetDataSlot(const char* i_name, void* i_data, int i_size)
{
	if (d_netActor != NULL)
		return(d_netActor->WriteDataSlot(i_name, i_data, i_size));
	return(true);
}

// Force a send of the current contents of the data buffer.
void CActor::SendNetData(void)
{
	if (d_netActor != NULL)
		d_netActor->SendData();
}

// Get a pointer to a slot for reading or writing. NULL is returned if the actor
// is not distrbuted, the actor is not the master, or slot cannot be found.
void* CActor::OpenNetDataSlot(const char* i_name)
{
	if (d_netActor != NULL)
		return(d_netActor->OpenDataSlot(i_name));
	return(NULL);
}

// Get a pointer to a slot for reading. NULL is returned if the actor is not
// distrbuted or slot cannot be found.
const void* CActor::ReadNetDataSlot(const char* i_name)
{
	if (d_netActor != NULL)
		return(d_netActor->ReadDataSlot(i_name));
	return(NULL);
}

void CActor::BeginFrame()
{
	if ( Script( AI_SCRIPT ) && !g_timer.IsPaused() )
	{
		Script( AI_SCRIPT )->Run( g_timer.GetFrameSec() );
	}
}

void CActor::ScaleActor()
{
	if ( !Math::Equal( d_scale.x(), 1.0f ) ||
		  !Math::Equal( d_scale.y(), 1.0f ) ||
		  !Math::Equal( d_scale.z(), 1.0f ) )
	{
		for ( int i = 0 ; i < MAX_ACTOR_INSTANCES ; i++ )
			if ( Instance( i ) )
				Instance( i )->Matrix().ReverseCatScale( d_scale.x(), d_scale.y(), d_scale.z() );
	}
}

void CActor::SetupDistributedState(void)
{
	if (!d_stateDistributed)
	{
		if (RequestNetDataSlot("DistributedState", sizeof(SDistributedState)))
		{
			SDistributedState*
					state = (SDistributedState*)OpenNetDataSlot("DistributedState");
			ASSERT_PTR(state);
			state->d_bodyInWorld = d_bodyInWorld;
			state->d_bodyToWorld = d_bodyToWorld;
			state->d_velocityInWorld = d_velocityInWorld;
			state->d_omegaInBody = d_omegaInBody;
		}
	}
}

void CActor::WriteDistributedState(void)
{
	SDistributedState* state;
	if (!d_stateDistributed && ((state =
			(SDistributedState*)OpenNetDataSlot("DistributedState")) != NULL))
	{
		state->d_bodyInWorld = d_bodyInWorld;
		state->d_bodyToWorld = d_bodyToWorld;
		state->d_velocityInWorld = d_velocityInWorld;
		state->d_omegaInBody = d_omegaInBody;
		SendNetData();
	}
}

bool CActor::ReadDistributedState(void)
{
	SDistributedState*
			state = (SDistributedState*)ReadNetDataSlot("DistributedState");
	if (state != NULL)
	{
		d_bodyInWorld = state->d_bodyInWorld;
		d_bodyToWorld = state->d_bodyToWorld;
		d_velocityInWorld = state->d_velocityInWorld;
		d_omegaInBody = state->d_omegaInBody;
		return(true);
	}
	return(false);
}

void CActor::CNetDataReceived::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (d_ref->ReadDistributedState())
		d_ref->Placed();
}
