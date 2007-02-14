/*************************************/
/*                                   */
/*   CutSceneMgr.cpp                 */
/*   big juju   5/2/03               */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   cut scenes!                     */
/*                                   */
/*************************************/

/* cut scenes provide a way for data to drive the engine without player input */

/*

  data types:

  actor
		move along paths (and spline to indicate speed)
		rotation away from path direction?		
		animate (and blend between)
  camera
		move along paths
		point at target
		move from target to target
		fov control
		depth of field?
	particles
		emitters
			position
			maybe move along paths
		effects
			position and orientation
	lights
		add, remove, update-- mostly local lights, i guess
*/

/******************* includes ****************************/

#include "TTLPCH.h"

#include "main/TTL.h"
#include "cutscene/CutSceneMgr.h"
#include "EngineHelper/sfx.h"
#include "EngineHelper/timer.h"
#include "environ/LevelScript.h"
#include "environ/ActorCreator.h"
#include "Effects/fullscreendistort.h"
//#include "components/combatant.h"
#include "Game/Database/DataMgr.h"
#include "Layers/DiscErrorHandler/DiscError.h"

/******************* defines *****************************/

/* when FORCE_AUDIO_TO_FIRST_FRAME is defined, the audio stream (if any) is forced to start on frame 0-- */
/* any timing info in the cut file data is ignored. for final release, starting the audio and the cut scene */
/* rendering at the same time is the only synchonization method we have */

//#ifdef CDROM
#define FORCE_AUDIO_TO_FIRST_FRAME
//#endif //CDROM

//******************* macros ******************************

//******************* structures **************************

struct ts_KeyDefinition
{
	const char *Name;
	int nValue;
	CCutSceneInterpreter::KEY_TYPE KeyType;
};

struct ts_NameAndValue
{
	const char *Name;
	int nValue;
};

//******************* externs *****************************

//******************* global variables ********************

CCutSceneMgr g_cutSceneManager;

//****************** local variables *********************

static const ts_KeyDefinition KeyList[] = {

	{"type",							CCutSceneInterpreter::CUT_SCENE_TYPE,						CCutSceneInterpreter::KEY_STRING},
	{"version",						CCutSceneInterpreter::CUT_SCENE_VERSION,					CCutSceneInterpreter::KEY_FLOAT},
	{"count",						CCutSceneInterpreter::CUT_SCENE_COUNT,						CCutSceneInterpreter::KEY_INT},
	{"at_label",					CCutSceneInterpreter::CUT_SCENE_AT_LABEL,					CCutSceneInterpreter::KEY_STRING},
	{"label",						CCutSceneInterpreter::CUT_SCENE_LABEL,						CCutSceneInterpreter::KEY_STRING},
	{"start",						CCutSceneInterpreter::CUT_SCENE_START,						CCutSceneInterpreter::KEY_FLOAT},
	{"length",						CCutSceneInterpreter::CUT_SCENE_LENGTH,					CCutSceneInterpreter::KEY_FLOAT},
	{"time",							CCutSceneInterpreter::CUT_SCENE_TIME,						CCutSceneInterpreter::KEY_FLOAT},
	{"end",							CCutSceneInterpreter::CUT_SCENE_END,						CCutSceneInterpreter::KEY_INT},
	{"event_type",					CCutSceneInterpreter::CUT_SCENE_EVENT_TYPE,				CCutSceneInterpreter::KEY_INT},
	{"distance",					CCutSceneInterpreter::CUT_SCENE_DISTANCE,					CCutSceneInterpreter::KEY_FLOAT},
	{"speed",						CCutSceneInterpreter::CUT_SCENE_SPEED,						CCutSceneInterpreter::KEY_FLOAT},
	{"path",							CCutSceneInterpreter::CUT_SCENE_PATH,						CCutSceneInterpreter::KEY_STRING},
	{"name",							CCutSceneInterpreter::CUT_SCENE_NAME,						CCutSceneInterpreter::KEY_STRING},
	{"new",							CCutSceneInterpreter::CUT_SCENE_NEW,						CCutSceneInterpreter::KEY_INT},
	{"class",				      CCutSceneInterpreter::CUT_SCENE_CLASS,		      		CCutSceneInterpreter::KEY_STRING},
	{"model_name",					CCutSceneInterpreter::CUT_SCENE_MODEL_NAME,				CCutSceneInterpreter::KEY_STRING},
	{"kfm",							CCutSceneInterpreter::CUT_SCENE_KFM,						CCutSceneInterpreter::KEY_STRING},
	{"target",						CCutSceneInterpreter::CUT_SCENE_TARGET,					CCutSceneInterpreter::KEY_STRING},
	{"index",						CCutSceneInterpreter::CUT_SCENE_INDEX,						CCutSceneInterpreter::KEY_INT},
	{"parent",						CCutSceneInterpreter::CUT_SCENE_PARENT,					CCutSceneInterpreter::KEY_INT},
	{"curve_count",				CCutSceneInterpreter::CUT_SCENE_CURVE_COUNT,				CCutSceneInterpreter::KEY_INT},
	{"curve_type",					CCutSceneInterpreter::CUT_SCENE_CURVE_TYPE,				CCutSceneInterpreter::KEY_INT},
	{"curve",						CCutSceneInterpreter::CUT_SCENE_CURVE,						CCutSceneInterpreter::KEY_STRING},
	{"x",								CCutSceneInterpreter::CUT_SCENE_X,							CCutSceneInterpreter::KEY_FLOAT},
	{"y",								CCutSceneInterpreter::CUT_SCENE_Y,							CCutSceneInterpreter::KEY_FLOAT},
	{"z",								CCutSceneInterpreter::CUT_SCENE_Z,							CCutSceneInterpreter::KEY_FLOAT},
	{"red",							CCutSceneInterpreter::CUT_SCENE_RED,						CCutSceneInterpreter::KEY_FLOAT},
	{"green",						CCutSceneInterpreter::CUT_SCENE_GREEN,						CCutSceneInterpreter::KEY_FLOAT},
	{"blue",							CCutSceneInterpreter::CUT_SCENE_BLUE,						CCutSceneInterpreter::KEY_FLOAT},
	{"effect",						CCutSceneInterpreter::CUT_SCENE_PARTICLE_EFFECT,		CCutSceneInterpreter::KEY_STRING},
	{"sound",						CCutSceneInterpreter::CUT_SCENE_SOUND,						CCutSceneInterpreter::KEY_STRING},
	{"loop",							CCutSceneInterpreter::CUT_SCENE_LOOP,						CCutSceneInterpreter::KEY_INT},
	{"focal_length",				CCutSceneInterpreter::CUT_SCENE_FOCAL_LENGTH,			CCutSceneInterpreter::KEY_FLOAT},
	{"vertical_aperture",		CCutSceneInterpreter::CUT_SCENE_VERTICAL_APERTURE,		CCutSceneInterpreter::KEY_FLOAT},
	{"horizontal_aperture",		CCutSceneInterpreter::CUT_SCENE_HORIZONTAL_APERTURE,	CCutSceneInterpreter::KEY_FLOAT},
	{"transform_xzy",				CCutSceneInterpreter::CUT_SCENE_TRANSFORM_XZY,			CCutSceneInterpreter::KEY_INT},
	{"fade",							CCutSceneInterpreter::CUT_SCENE_FADE,						CCutSceneInterpreter::KEY_INT},
	{"light_type",					CCutSceneInterpreter::CUT_SCENE_LIGHT_TYPE,				CCutSceneInterpreter::KEY_INT},
	{"restore_to_base_pose",	CCutSceneInterpreter::CUT_SCENE_RESTORE_TO_BASE_POSE,	CCutSceneInterpreter::KEY_INT},
	{"shadow",						CCutSceneInterpreter::CUT_SCENE_SHADOW,					CCutSceneInterpreter::KEY_INT},
	{"radius",						CCutSceneInterpreter::CUT_SCENE_SHADOW_RADIUS,			CCutSceneInterpreter::KEY_FLOAT},
	{"alpha",						CCutSceneInterpreter::CUT_SCENE_SHADOW_ALPHA,			CCutSceneInterpreter::KEY_FLOAT},
	{"trigger_event",				CCutSceneInterpreter::CUT_SCENE_TRIGGER_EVENT,			CCutSceneInterpreter::KEY_STRING},
	{"show_club",					CCutSceneInterpreter::CUT_SCENE_SHOW_CLUB,				CCutSceneInterpreter::KEY_INT},
	{"show_rattle",				CCutSceneInterpreter::CUT_SCENE_SHOW_RATTLE,				CCutSceneInterpreter::KEY_INT},
	{"show_blowgun",				CCutSceneInterpreter::CUT_SCENE_SHOW_BLOWGUN,			CCutSceneInterpreter::KEY_INT},
};

static const ts_NameAndValue InterpreterType[] = {

	{"file",							CCutSceneInterpreter::FILE},
	{"MainCharacter",				CCutSceneInterpreter::ACTOR},
	{"EnvironmentalActor",		CCutSceneInterpreter::ACTOR},
	{"Camera",						CCutSceneInterpreter::CAMERA},
	{"ParticleEmitter",			CCutSceneInterpreter::PARTICLE_EMITTER},
	{"ParticleEffect",			CCutSceneInterpreter::PARTICLE_EFFECTS},
	{"light",						CCutSceneInterpreter::LIGHT},
	{"distortion",					CCutSceneInterpreter::DISTORTION},
	{"sound",						CCutSceneInterpreter::SOUND},
	{"DummyParent",				CCutSceneInterpreter::DUMMY_PARENT},
	{"MotionBlur",					CCutSceneInterpreter::MOTION_BLUR},
	{"Trigger",						CCutSceneInterpreter::TRIGGER},
};

int CCutSceneInterpreter::d_nKeyListSize = sizeof(KeyList) / sizeof(KeyList[0]);
int CCutSceneInterpreter::d_nInterpreterTypeSize = sizeof(InterpreterType) / sizeof(InterpreterType[0]);

void *CCutSceneInterpreter::d_pAnimCurveChunkData[MAX_ANIM_CURVE_CHUNKS];
int CCutSceneInterpreter::d_nAnimCurveChunkCount = 0;

t_Handle CCutSceneInterpreter::d_AnimDataHandle = INVALID_HANDLE;

/******************* local prototypes ********************/

/******************* functions ***************************/


// cut scene interpreters ches through the cut scene data, one per data stream for the cut scene being played
CCutSceneInterpreter::CCutSceneInterpreter()
{
	Initialize( true );
}


CCutSceneInterpreter::~CCutSceneInterpreter()
{
	if( d_pInterpreterCurve )
	{
		// call destructor on anim curve records (not delete, since they are in malloc'd buffer)
		for( int i = 0 ; i < d_nInterpreterCurveMaxCount ; i++ )
		{
			d_pInterpreterCurve[ i ].AnimCurve.~CAnimCurve();
		}

		memFree( d_pInterpreterCurve );
		d_pInterpreterCurve = NULL;
	}
}


// do constructor-like initialization. this helps handle the fact that we have some permanent interpreters to
// support the loading screen, so getting an interpreter from CCutSceneMgr::AllocInterpreter() may not always
// malloc a new one and put it through the constructor. so here's one they can share
void CCutSceneInterpreter::Initialize( bool bFromConstructor )
{
	d_pData = NULL;
	d_pNext = NULL;
	d_Type = UNKNOWN;
	d_nIndex = -1;
	d_nParentIndex = -1;
	d_pParent = NULL;
	d_pTypeLabel = NULL;
	d_fNextTimecode = MAXFLOAT;	// if not reset by Initialize, interpreter will never execute

	// now the stuff that differs depending on where this called from
	if( (bFromConstructor) || ( ( d_u32Flags & PERMANENT ) == 0 ) )
	{
		d_pInterpreterCurve = NULL;
		d_u32Flags = 0;
	}
	else
	{
		// this is a permanent interpreter. d_pInterpreterCurve is permanent, and we need to protect the PERMANENT flags
		d_u32Flags &= ( PERMANENT | PERMANENT_AVAILABLE );
	}
}


// mark an interpreter as permanent, to support load screen without any malloc's
void CCutSceneInterpreter::SetPermanent()
{
	SetFlag( PERMANENT | PERMANENT_AVAILABLE );

	// pre-malloc the interpreter curve buffer to some max-needed size so that the malloc gets marked as permanent 
	MEM_SET_ALLOC_NAME( "InterpreterCurve" );
	d_pInterpreterCurve = (ts_InterpreterCurve *) memAlloc( MAX_PERMANENT_INTERPRETER_CURVES * sizeof( ts_InterpreterCurve ) );
	ASSERTS( d_pInterpreterCurve != NULL, "Couldn't malloc interpreter curve buffer" );
}


// clean up 
void CCutSceneInterpreter::Cleanup()
{
	switch( d_Type )
	{
		case PARTICLE_EMITTER:
			if( d_Handle != INVALID_HANDLE )
			{
				ParticleEmitter__Kill( d_Handle );
			}
		break;

		case DISTORTION:
			FullScreenDistort::Shutdown();
			d_pSignalGenerator1 = NULL;	// FullScreenDistort::Shutdown cleans up signal generator
			d_pSignalGenerator2 = NULL;	// FullScreenDistort::Shutdown cleans up signal generator
		break;

		case MOTION_BLUR:
			gfxMotionBlur( 0 , 0 );
		break;

	case ACTOR:
		// may have failed in initialization, so d_pCActor may be NULL
		if( d_pCActor == NULL )
		{
			break;
		}
		
		if( d_pCActor->IsA( CCompActor::ClassType() ) )
		{
			CCompActor *comp = (CCompActor *)d_pCActor;
			CActorComponent *controller = comp->GetFirstComponentInClass( "kinematicController" );
			if( controller )
			{			
				controller->Activate();
			}
			CActorComponent *driver = comp->GetFirstComponentInClass( "KinematicDriver" );
			if( driver )
			{
				driver->Activate();
			}
		}

		// maybe delete cut-scene-only actor
		if( d_u32Flags & DELETE_ACTOR )
		{
			delete d_pCActor;
			d_pCActor = NULL;
		}
		else
		{
			// actor may have been moved by cut scene. we want actor to maintain that position/rotation. we 
			// know that actor's instance's local-to-world matrix is a valid representation of his position 
			// and orientation, taking DUMMY_PARENT's into account, so use that 
			Vector3 vTranslation = d_pCActor->LocalToWorld().GetTranslation();
			Vector3 vForward = d_pCActor->LocalToWorld().GetRow2();
			float fHeading = Math::ArcTan2( vForward.X() , vForward.Z() );
			d_pCActor->Place( vTranslation , fHeading );

			// make sure actor is not hidden. i suppose that it's possible that the cut scene artist may want to 
			// attach to an existing actor, hide it, and want it to remain hidden, but we should figure out a way 
			// to kill the actor in that case 
			d_pCActor->Hide( false );
			
			// back to default motion data 
			d_pCActor->Instance()->SetMotionFile( 0 , (ts_MotionFileHeader*)NULL , (MotionNameHash*)NULL );
			d_pCActor->Instance()->ClearMotionFile( 1 );	// dbl will be removed, too, by cut scene manager
			d_pCActor->StopMotion( (char *)NULL , -1 );	// stop playing current animation. script will be responsible for starting up something

			// if actor has bones, reset the first bone matrix to identity. this is because there may be bone
			// animation data for the model bone in a cut scene (shouldn't really be-- typically it will come
			// through maya curves)
			if( d_pCActor->Instance()->Body() )
			{
				boneSetLocalMatrixToIdentity( d_pCActor->GetBone( (BoneID)0) );
			}

			/* may also want to restore the actor to base pose */
			if( d_u32Flags & RESTORE_TO_BASE_POSE )
			{
				boneSetBoneTreeIdentity( d_pCActor->GetBone( (BoneID)0 ) );
			}

			// restart actor scripts
			g_messageDispatcher.SendMessageToActor( "EndCutScene" , NULL , INVALID_OBJECT_HANDLE , d_pCActor->GetHandle() );

			// restore collision state
			if( d_u32Flags & WAS_PROTAGONIST )
			{
				((CProtagonist *) d_pCActor)->MoveToProtagonistCast();
			}
	
			// reset chains for good luck
			if( d_pCActor->GetChains() )
			{
				d_pCActor->GetChains()->ResetChains();
			}
		}
		break;
	}
}


//************************** CCutSceneInterpreter-- initialize *********************************
// initialize the interpreter
bool CCutSceneInterpreter::Initialize( char *pData , char *pDataEnd , CCutSceneInterpreter *pNext , CCutSceneMgr *pCutSceneMgr , bool bLoadScreen )
{
	d_pData = pData;
	d_pDataEnd = pDataEnd;
	d_pNext = pNext;
	d_pCutSceneMgr = pCutSceneMgr;

	// safe values
	d_vPosition.Clear();
	d_vRotationRadians.Clear();
	d_vScale.Set(1.0f, 1.0f, 1.0f);

	// what type of interpreter is this?
	if( GetExecuteKey( CUT_SCENE_TYPE ) == false )
	{
		return( false );
	}

	int i;
	for( i = 0 ; i < d_nInterpreterTypeSize ; i++ )
	{
		if ( stricmp( InterpreterType[ i ].Name , KeyValueString() ) == 0 )
		{
			break;
		}
	}

	if( i == d_nInterpreterTypeSize )
	{
		ASSERTF( false , ("Unknown cut scene interpreter type: %s", KeyValueString() ) );
		return( false );
	}
	d_Type = (INTERPRETER_TYPE) InterpreterType[ i ].nValue;
	d_pTypeLabel = InterpreterType[ i ].Name;

	// FILE type has to be handled by manager since it has to set up the other interpreters
	if( d_Type == FILE )
	{
		return( true );
	}

	// check for parent datastream-- this allows us to have hierarchical animation
	if( GetExecuteKey( CUT_SCENE_INDEX ) == false )
	{
		return( false );
	}
	d_nIndex = KeyValueInt();
	if( GetExecuteKey( CUT_SCENE_PARENT ) == false )
	{
		return( false );
	}
	d_nParentIndex = KeyValueInt();

	// next field should be number of animation curves required for this interpreter
	if( GetExecuteKey( CUT_SCENE_CURVE_COUNT ) == false )
	{
		return( false );
	}
	d_nInterpreterCurveMaxCount = KeyValueInt();
	if( d_nInterpreterCurveMaxCount > 0 )
	{
		// permanent interpreters have pre-allocated d_pInterpreterCurve buffer to avoid memAlloc'd for load screen
		if( d_pInterpreterCurve )	// already malloc'd?
		{
			ASSERT( d_u32Flags & PERMANENT );	// must be a permanent interpreter, or else this is a memory leak
			ASSERT( d_nInterpreterCurveMaxCount <= MAX_PERMANENT_INTERPRETER_CURVES );	// if this hits, increase MAX_PERMANENT_INTERPRETER_CURVES
		}
		else
		{
			MEM_SET_ALLOC_NAME( "InterpreterCurve" );
			d_pInterpreterCurve = (ts_InterpreterCurve *) memAlloc( d_nInterpreterCurveMaxCount * sizeof(ts_InterpreterCurve) );
			ASSERTS( d_pInterpreterCurve != NULL , "Couldn't malloc interpreter curve buffer" );
		}
	}
	d_nInterpreterCurveCount = 0;

	/* call constructor on anim curve records embedded in buffer */

//compiler doesn't like this...constructor does nothing, so no harm done currently, but really should call constructor
//	for (i = 0; i < d_nInterpreterCurveMaxCount; i++)
//		d_pInterpreterCurve[i].AnimCurve.CAnimCurve();
	
	// parse data until we reach the first time code, and then wait for Execute calls
	switch( d_Type )
	{
		case ACTOR:
			ASSERT( bLoadScreen == false );	// not supported in load screen mode
			if( InitActor() == false )
			{
				return( false );
			}
		break;

		case CAMERA:
			if( InitCamera() == false )
			{
				return( false );
			}
		break;

		case PARTICLE_EMITTER:
			if( InitEmitter() == false )
			{
				return( false );
			}
		break;

		case PARTICLE_EFFECTS:
			if( InitEffects() == false )
			{
				return( false );
			}
		break;

		case LIGHT:
			if( InitLight() == false )
			{
				return( false );
			}
		break;

		case DISTORTION:
			if( InitDistortion() == false )
			{
				return( false );
			}
		break;

		case MOTION_BLUR:
			if( InitMotionBlur() == false )
			{
				return( false );
			}
		break;

		case SOUND:
			ASSERT( bLoadScreen == false );	// not supported in load screen mode
			if( InitSound() == false )
			{
				return( false );
			}
		break;

		case TRIGGER:
			if( InitTrigger() == false )
			{
				return( false );
			}
		break;

		case DUMMY_PARENT:
			if( InitDummyParent() == false )
			{
				return( false );
			}
		break;

		default:
			ASSERTS( false , "Unhandled cut scene type-specific initialization" );
			return( false );
	}

	return( true );
}


// type-specific initialization. the goal is to get to the first timecode, and then wait for Execute calls
bool CCutSceneInterpreter::InitActor()
{
	// link to actor to control
	if( GetExecuteKey( CUT_SCENE_NAME ) == false )
	{
		return( false );
	}
	if( ( d_pCActor = LinkToActor() ) == NULL )
	{
		return( false );
	}

	// link to possible additional anim data, in second motion database slot
	if( d_AnimDataHandle != INVALID_HANDLE )
	{
		motLoadMotionChunk( d_AnimDataHandle , d_pCActor->Instance() , 1 );
	}

	// set transform order flag
	if( GetExecuteKey( CUT_SCENE_TRANSFORM_XZY ) == false )
	{
		return( false );
	}
	if( KeyValueInt() == 1 )
	{
		d_u32Flags |= TRANSFORM_XZY;
	}

	// get current position and rotation
	d_vPosition = d_pCActor->GetBodyInWorld();
	d_vScale    = d_pCActor->Scale();
	d_pCActor->GetBodyToWorld().DecodeToYXZ( d_vRotationRadians );	// same decode as used in Simple.cpp

	// next key should be timecode
	return( GetNextTimecode() );
}


bool CCutSceneInterpreter::InitCamera()
{
	bool tf;

	// get current camera position and rotation
	CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();
	CameraGroup &cameraGroup = cameraScene->CurrentGroup();
	Camera &camera = cameraScene->CurrentCamera();

	d_vPosition = camera.Eye();

	d_vRotationRadians.Clear();
	d_pCActor = NULL;	// used as possible target

	// link up to cameras 
	d_pGeneralCamera = (GeneralCamera *)cameraGroup.Find( "General" );
	ASSERT( d_pGeneralCamera );
	d_pPathCamera = (PathCamera *)cameraGroup.Find( "Path" );
	ASSERT( d_pPathCamera );

	// next key should be timecode
	tf = GetNextTimecode();
	return tf;
}


bool CCutSceneInterpreter::InitEmitter()
{
	// create emitter
	if( GetExecuteKey( CUT_SCENE_NAME ) == false )
	{
		return( false );
	}

	// for more flexibility, artist can rotate the particle emitter, and the ejection velocity of the particles 
	// will rotate with it 
	d_mTransform.Identity();
	ParticleMgr__SetOneShotEmitterVelTransform( &d_mTransform );

	// create the emitter
	d_Handle = ParticleEmitter__CreateFromRecord(
		KeyValueString(),		// u32 u32EmitterID,
		&d_vPosition,			// Vector3 *pvEmitterPos,
		NULL,						// Vector3 *pvEmitterVel,
		NULL,						// Vector3 *pvEjectVel,
		false,					// bool bEjectVelChanges,
		NULL,						// Vector3 *pvUp,
		false);					// bool bStationary)
	if( d_Handle == INVALID_HANDLE )
	{
		return( false );
	}
	ParticleEmitter__Deactivate( d_Handle );

	// set transform order flag
	if( GetExecuteKey( CUT_SCENE_TRANSFORM_XZY ) == false )
	{
		return( false );
	}
	if( KeyValueInt() == 1 )
	{
		d_u32Flags |= TRANSFORM_XZY;
	}

	// next key should be timecode
	return( GetNextTimecode() );
}


bool CCutSceneInterpreter::InitEffects()
{
	// next key should be timecode
	return( GetNextTimecode() );
}


bool CCutSceneInterpreter::InitLight()
{
	// what kind of light?
	if( GetExecuteKey( CUT_SCENE_LIGHT_TYPE ) == false )
	{
		return( false );
	}
	d_u32Flags |= KeyValueInt();	// value has correct flag bit set

	//local lights currently don't work
	if( d_u32Flags & LOCAL_LIGHT )
	{
		return(false);
	}

	// get light's initial color
	SetColor();

	// set transform order flag 
	if( GetExecuteKey( CUT_SCENE_TRANSFORM_XZY ) == false )
	{
		return( false );
	}
	if( KeyValueInt() == 1 )
	{
		d_u32Flags |= TRANSFORM_XZY;
	}

	// known initial values-- real values will come from executing the interpreter
	d_vPosition.Clear();
	d_vRotationRadians.Clear();
	d_vScale.Set( 1.0f , 1.0f , 1.0f );

	// next key should be timecode
	return( GetNextTimecode() );
}


bool CCutSceneInterpreter::InitDistortion()
{
	// set up the full screen distortion-- the signal generator settings are stolen from the numa dunes distortion
	// settings. signal generator amplitude (last parameter in SigGen constructor) gets updated every frame from
	// distortion's x-scale curve. opacity (the 3rd parameter to FullScreenDistort::Init) gets updated with the
	// distortion's y-scale curve
	d_pSignalGenerator1 = new SigGen( SigGen::FASTSMOOTHNOISE , 0.6667f , 0.1f );	// initial amplitude must be non-zero
	d_pSignalGenerator2 = new SigGen( SigGen::FASTSMOOTHNOISE , 2.0f , 0.1f );		// initial amplitude must be non-zero
	FullScreenDistort::Init( 20 , 15 , 0.0f , FullScreenDistort::NORMAL );				// opacity gets updated from y-scale
	FullScreenDistort::Add( FullScreenDistort::X , 0.5f , 0.5f , 0.0f, 0.5f , false , 0.0f , 0.0f , 0.0f , true , false , d_pSignalGenerator1 );
	FullScreenDistort::Add( FullScreenDistort::Y , 0.5f , 0.5f , 0.0f, 0.5f , false , 0.0f , 0.0f , 0.0f , true , false , d_pSignalGenerator2 );

	// next key should be timecode
	return( GetNextTimecode() );
}


bool CCutSceneInterpreter::InitMotionBlur()
{
	// start the motion blur. the interpreter's x-scale value controls the motion blur strength directly
	d_vScale.X( 0.0f );
	//matt -- old code - g_motionBlur.Start(MotionBlur::MEDIUM, (float *) &d_vScale);
	gfxMotionBlur( 64 , 0 );

	// next key should be timecode
	return( GetNextTimecode() );
}


bool CCutSceneInterpreter::InitSound()
{
	// ignore sound for video recording-- it won't sync up anyway
	#ifdef ALLOW_VIDEO_RECORD_CUTSCENE
	if( g_videoRecordCutScene )
	{
		return( false );
	}
	#endif //ALLOW_VIDEO_RECORD_CUTSCENE

#ifdef FORCE_AUDIO_TO_FIRST_FRAME
	// dig through data stream looking for .wav file. on a cdrom build, we force the cut scene audio to
	// start at the same time as the cut scene, since there's no way to sync it otherwise-- there may be
	// a seek delay before the audio starts, so if, say, the cut scene calls for starting the audio at
	// 1 second into the cut scene, there's no way to sync it up properly. however, if the audio and the
	// cut scene start at the same time, then we can queue the audio and start the cut scene once the audio
	// actually starts

	// this is a hack, it would be much better to put the audio name in the file interpreter section, but
	// that would require re-export of all cut scenes
	char *pBookmark = CurrentData();
	bool bDone = false;
	while (bDone == false)
	{
		SkipToNextEvent();	// finds next timecode key/value pair
		if (d_fNextTimecode == MAXFLOAT)
			bDone = true;	// no more events
		else
		{
			/* found an event-- what is it? we're looking for GO events that start a sound */

			if ((GetEventType()) && (d_nEvent == DATA_STREAM_EVENT_GO))
			{
				if (GetExecuteKey(CUT_SCENE_SOUND))
				{
					/* got a sound. is it a wav file? */

					if (strstr(KeyValueString(), ".wav"))
					{
						d_pCutSceneMgr->SetAudioName(KeyValueString());
						bDone = true;
					}
				}
			}
		}
	}
	SetData(pBookmark);	// restore data pointer
#endif //FORCE_AUDIO_TO_FIRST_FRAME

	// next key should be timecode
	return( GetNextTimecode() );
}


bool CCutSceneInterpreter::InitTrigger()
{

	// next key should be timecode
	return( GetNextTimecode() );
}


bool CCutSceneInterpreter::InitDummyParent()
{
	d_vPosition.Clear();
	d_vRotationRadians.Clear();
	d_vScale.Set( 1.0f , 1.0f , 1.0f );

	// next key should be timecode
	return( GetNextTimecode() );
}


// report an unhandled key in an initialize routine
void CCutSceneInterpreter::UnhandledInitializeKey()
{
	ASSERTF( false , ("Cut scene %s (type %d), unhandled initialize key: %s" , d_pTypeLabel , d_Type , d_Key ) );
}


//************************** CCutSceneInterpreter-- execute ********************************
// execute an interpreter
bool CCutSceneInterpreter::Execute( float fTime )
{
	// follow parent chain-- parents have to be executed before child interpreter can be */
	if( d_pParent && ( d_pParent->IsFlagSet( EXECUTED ) == false ) )
	{
		d_pParent->Execute( fTime );	// mildly recursive
	}

	// mark as executed
	SetFlag( EXECUTED );

	// switch out based on type 
	bool bResult;
	switch( d_Type )
	{
		case ACTOR:
			bResult = ExecuteActor( fTime );
		break;
	
		case CAMERA:
			bResult = ExecuteCamera( fTime );
		break;
	
		case PARTICLE_EMITTER:
			bResult = ExecuteEmitter( fTime );
		break;
	
		case PARTICLE_EFFECTS:
			bResult = ExecuteEffects( fTime );
		break;
		
		case LIGHT:
			bResult = ExecuteLight( fTime );
		break;
	
		case DISTORTION:
			bResult = ExecuteDistortion( fTime );
		break;

		case MOTION_BLUR:
			bResult = ExecuteMotionBlur( fTime );
		break;

		case SOUND:
			bResult = ExecuteSound( fTime );
		break;

		case TRIGGER:	
			bResult = ExecuteTrigger( fTime );
		break;

		case DUMMY_PARENT:	
			bResult = ExecuteDummyParent( fTime );
		break;
	
		default:
			ASSERTS( false , "Unhandled cut scene type-specific execute" );
			return( false );
	}

	// if failure, try to skip to next event so we continue this interpreter the best we can
	if( bResult == false )
	{
		SkipToNextEvent();
	}

	// return result
	return( bResult );
}


// execute specific interpreter types 
bool CCutSceneInterpreter::ExecuteActor( float fTime )
{
	// may want to do some first-time processing 
	if ((d_u32Flags & HAS_EXECUTED) == 0)
	{
		d_u32Flags |= HAS_EXECUTED;
	}

	// record starting position for actor's chain check 
	Vector3 vPosition = d_pCActor->GetBodyInWorld();	// use body in world because it takes parent transforms into account

	// time to go. start processing events 
	while( d_fNextTimecode <= fTime )
	{
		// time to start next event 
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different 
		switch( d_nEvent )
		{
			case DATA_STREAM_EVENT_ANIMATE:
				if (GetExecuteKey(CUT_SCENE_KFM) == false)
				{
					return( false );
				}

				// find kfm in list. start with custom anims (regular anims are in motion database slot 0) 
				ts_MotionHeader *mh;
				mh = d_pCActor->Instance()->LookupMotion( KeyValueString() );

				// start motion. loop or one-shot? always use high priority so animation will have priority over dynamic 
				// chain animation (if animation data exists for chain bone). for tak, use low priority so that his hair, 
				// feathers and gourds will continue to behave like chains. passing i_allGroups == false means use 
				// bone groups in motion header, not all bone groups 
				if( GetExecuteKey( CUT_SCENE_LOOP ) == false )
				{
					return( false );
				}

				{
					u32 u32Flags = MOTNINF_FLAG_NOINTERP;
					if( KeyValueInt() == 0 )
					{
						u32Flags |= MOTNINF_FLAG_ONESHOT_AND_HOLD;
					}
					else
					{
						u32Flags |= MOTNINF_FLAG_LOOP;
					}
					d_pCActor->StartMotion( mh , 0 , u32Flags );

					// record the fact that actor is playing a kfm
					d_u32Flags |= KFM_PLAYING;
					if( u32Flags & MOTNINF_FLAG_LOOP )
					{
						d_u32Flags |= KFM_LOOPING;
					}
					d_u16AnimFrames = CMotion__GetAnimFrames( d_pCActor->GetMotnInf( MOTION1 )->pNextMotionHeader ) - 1;
					d_fKFMStartTime = d_fNextTimecode;

					// may need a global kfm start time adjustment to improve lip sync 
					d_fKFMStartTime += d_pCutSceneMgr->GetKFMStartTimeAdjust();
				}
			break;

			case DATA_STREAM_EVENT_ATTACH_TO_CURVE:
				if( AttachToCurve() == false )
				{
					return( false );
				}
			break;

			case DATA_STREAM_EVENT_SET_POSITION:
				if( SetPosition() == false )
				{
					return( false );
				}
			break;

			case DATA_STREAM_EVENT_SET_ROTATION:
				if( SetRotation() == false )
				{
					return( false );
				}
			break;

			case DATA_STREAM_EVENT_SHOW:
				Hide( false );
			break;

			case DATA_STREAM_EVENT_HIDE:
				Hide( true );
			break;

			default:
				UnhandledEvent();
				return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// process curves, if any, and build transform matrix (including parent contributions)
	EvaluateCurves( fTime );
	BuildTransform();

	// set actor's world position and orientation. this sets the local-to-world matrix directly, instead of relying 
	// on EndFrame to do it. CCutSceneMgr::Update is called after Stage::AdvanceSimulation, so this overwrites the 
	// mLocalToWorld set by EndFrame/SyncActor 
	d_pCActor->Instance()->Matrix() = d_mTransform;
	d_pCActor->Instance()->UpdateBoundingVolumes();

	// set actor's body in world for drop shadow use-- use d_mTransform.GetTranslation instead of d_vPosition
	// to take possible parent's into account 
	d_pCActor->GetBodyInWorld() = d_mTransform.GetTranslation();

	// force anim to sync to current time. don't worry about looping anims, they are presumably not sync'ed to audio. 
	// there is some drift in the update of the animation timer, so over a long period of time, the anim gets away 
	// from the real-time clock
	if( ( d_u32Flags & ( KFM_PLAYING | KFM_LOOPING ) ) == KFM_PLAYING )
	{
		s16 s16Frame = (s16) ( (fTime - d_fKFMStartTime) * 30.0f );
		if( s16Frame < 0 )
		{
			s16Frame = 0;
		}
		u16 u16Frame = (u16)s16Frame;
		if( u16Frame > d_u16AnimFrames )
		{
			u16Frame = d_u16AnimFrames;
		}
		CMotion__SetCurrentFrame( d_pCActor->GetMotnInf( MOTION1 ), u16Frame);
	}

	// maybe reset actors's dynamic chains (e.g. tak's feathers and hair) to account for sudden translations
	if ( ( d_pCActor->GetChains() ) && ( ( d_pCActor->GetBodyInWorld() - vPosition ).LengthSquared() > 1.0f * 1.0f ) )
	{
		d_pCActor->GetChains()->ResetChains();
	}

	return( true );
}


bool CCutSceneInterpreter::ExecuteCamera( float fTime )
{
	// time to go. start processing events 
	while( d_fNextTimecode <= fTime )
	{
      // time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
		
			case DATA_STREAM_EVENT_ATTACH_TO_PATH:
				if( AttachToPath() == false )
					return( false );
				SetActiveCamera( PATH_CAMERA );
			break;

		
			case DATA_STREAM_EVENT_PATH_DISTANCE:
				if ( GetExecuteKey( CUT_SCENE_DISTANCE ) == false )
				{
					return( false );
				}
				d_pPathCamera->SetDistance( KeyValueFloat() );
			break;

		
			case DATA_STREAM_EVENT_PATH_SPEED:
				if( GetExecuteKey( CUT_SCENE_SPEED ) == false )
				{
					return( false );
				}
				d_pPathCamera->SetSpeed( KeyValueFloat() );
			break;


			case DATA_STREAM_EVENT_TARGET:
				if( GetExecuteKey( CUT_SCENE_TARGET ) == false )
				{
					return( false );
				}
				d_pCActor = LinkToActor( true );	// ok to use d_pCActor since this is a camera interpreter, but tell LinkToActor that this is a target

				if( d_pCActor )
				{
					d_pPathCamera->SetTarget( d_pCActor );
				}
			break;

			case DATA_STREAM_EVENT_ATTACH_TO_CURVE:
				if( AttachToCurve() == false )
				{
					return( false );
				}
				SetActiveCamera( GENERAL_CAMERA );
			break;

			case DATA_STREAM_EVENT_SET_POSITION:
				if( SetPosition() == false )
				{
					return( false );
				}
			break;

			case DATA_STREAM_EVENT_SET_ROTATION:
         	if( SetRotation() == false )
				{
					return( false );
				}
			break;


			case DATA_STREAM_EVENT_CAMERA_SETTINGS:
				if( GetExecuteKey( CUT_SCENE_FOCAL_LENGTH ) == false )
				{
					return( false );
				}
				d_fFocalLength = KeyValueFloat();
				if( GetExecuteKey( CUT_SCENE_VERTICAL_APERTURE ) == false )
				{
					return( false );
				}
				d_fVerticalAperture = KeyValueFloat();
				if( GetExecuteKey( CUT_SCENE_HORIZONTAL_APERTURE ) == false)
				{
					return( false );
				}
				d_fHorizontalAperture = KeyValueFloat();
				if( GetExecuteKey( CUT_SCENE_TRANSFORM_XZY ) == false )
				{
					return( false );
				}
				if( KeyValueInt() == 1 )
				{
					d_u32Flags |= TRANSFORM_XZY;
				}
				else
				{
					d_u32Flags &= ~TRANSFORM_XZY;
				}
			break;

		
			default:
				UnhandledEvent();
				return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if( GetNextTimecode() == false )
			return( false );
	}

	// process curves, if any, and build transform matrix (including parent contributions)
	EvaluateCurves( fTime );

	// set field of view, calculated from focal length
#define FIELD_OF_VIEW_MAYA_CONSTANT	12.70001873f	// provided by adam clayton
	float fFieldOfView = Math::ArcTan(((FIELD_OF_VIEW_MAYA_CONSTANT * d_fVerticalAperture) / d_fFocalLength)) * 2.0f;
	SceneMgr::GetFirstScene()->d_sceneView.SetFieldOfViewY(Math::Rad2Deg(fFieldOfView));
	
	// aspect ratio
	SceneMgr::GetFirstScene()->d_sceneView.SetAspectRatio(d_fHorizontalAperture / d_fVerticalAperture);

	// process general camera if active
	if( d_u32Flags & GENERAL_CAMERA )
	{
		// set position
		d_pGeneralCamera->SetPosition( d_vPosition );

		// either looking at an actor or using rotation curves
		if( d_pCActor )
		{
			d_pGeneralCamera->SetFocalPoint( d_pCActor->GetBodyInWorld() );
		}
		else
		{
			// set up-vector and direction. we currently support x-y-z (maya default?) and x-z-y (engine default)
			DirCos3x3 mRotation;
			mRotation.SetXRotation( d_vRotationRadians.x() );
			if( d_u32Flags & TRANSFORM_XZY )
			{
				mRotation.CatZRotation( d_vRotationRadians.z() );
				mRotation.CatYRotation( d_vRotationRadians.y() );
			}
			else
			{
				mRotation.CatYRotation( d_vRotationRadians.y() );
				mRotation.CatZRotation( d_vRotationRadians.z() );
			}
			Vector3 vDirection;
			vDirection = mRotation.GetRow1();
			d_pGeneralCamera->SetUp( vDirection );
			vDirection = mRotation.GetRow2();
			d_pGeneralCamera->SetDirection( vDirection );
			d_pGeneralCamera->SetMode( GeneralCamera::CAMMODE_FOCAL_DIRECTION );
		}
	}

	// success
	return( true );
}


bool CCutSceneInterpreter::ExecuteEmitter( float fTime )
{
	// time to go. start processing events
	while( d_fNextTimecode <= fTime )
	{
		// time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
		
			case DATA_STREAM_EVENT_ATTACH_TO_CURVE:
				if( AttachToCurve() == false )
				{
					return( false );
				}
			break;

			case DATA_STREAM_EVENT_SET_POSITION:
				if( SetPosition() == false )
				{
					return( false );
				}
			break;

			case DATA_STREAM_EVENT_SET_ROTATION:
				if( SetRotation() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_SHOW:
				Hide( false );
			break;
		
			case DATA_STREAM_EVENT_HIDE:
				Hide( true );
			break;
	
			default:
				UnhandledEvent();
				return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// process curves, if any, and build transform matrix (including parent contributions)
	EvaluateCurves( fTime );
	BuildTransform();

	// success
	return( true );
}


bool CCutSceneInterpreter::ExecuteEffects( float fTime )
{
	// time to go. start processing events 
	while( d_fNextTimecode <= fTime )
	{
		// time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
			case DATA_STREAM_EVENT_SET_POSITION:
				if( SetPosition() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_TAK_POSITION:
				if (g_players.GetPlayer(0) != NULL)
					d_vPosition = g_players.GetPlayer( 0 )->GetBodyInWorld();
			break;
		
			case DATA_STREAM_EVENT_SET_ROTATION:
				if( SetRotation() == false )
				{
					return( false );
				}
			break;
	
			case DATA_STREAM_EVENT_GO:
				if( GetExecuteKey(CUT_SCENE_PARTICLE_EFFECT) == false )
				{
					return( false );
				}
				ParticleMgr__CreateEffect( KeyValueString() , &d_vPosition , NULL , NULL , NULL , 0.0f );
			break;
		
			default:
				UnhandledEvent();
				return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// success
	return( true );
}


bool CCutSceneInterpreter::ExecuteLight(float fTime)
{
	// may want to do some first-time processing
	if( ( d_u32Flags & HAS_EXECUTED ) == 0 )
	{
		d_u32Flags |= HAS_EXECUTED;

		// turn off light set, if any
		if( g_lightSet.lightSetIsLoaded() )
		{
			g_lightSet.enableLightSet( false );
		}

		// if this is an ambient light, then remove world's current ambient light
		if( d_u32Flags & AMBIENT_LIGHT )
		{
			if( ( d_pCutSceneMgr->GetFlags() & CCutSceneMgr::CUT_SCENE_AMBIENT_LIGHTS_REMOVED ) == 0 )
			{
				// only do this once
				//LightMgr__SaveAmbientLight();
				d_pCutSceneMgr->SetFlags( CCutSceneMgr::CUT_SCENE_AMBIENT_LIGHTS_REMOVED );
				//LightMgr__SetAmbient( &d_Color );
			}
		}

		// if this is a directional light, then remove world's current directional lights
		if( d_u32Flags & DIRECTIONAL_LIGHT )
		{
			if( ( d_pCutSceneMgr->GetFlags() & CCutSceneMgr::CUT_SCENE_DIRECTIONAL_LIGHTS_REMOVED ) == 0 )
			{
				// only do this once
				//LightMgr__SaveDirectionalLights();
				d_pCutSceneMgr->SetFlags( CCutSceneMgr::CUT_SCENE_DIRECTIONAL_LIGHTS_REMOVED );
			}
		}

		// add light. actual position/color values will get set in ExecuteLight after evaluating curves
		switch( d_u32Flags & ( AMBIENT_LIGHT | DIRECTIONAL_LIGHT | LOCAL_LIGHT ) )
		{
			case AMBIENT_LIGHT:
				//LightMgr__SetAmbient( &d_Color );
			break;
		
			case DIRECTIONAL_LIGHT:
				CalculateLightDirection();	// convert rotation to position for directional light
				{
					ts_Light Light;
					Light.Pos = d_vPosition;
					Light.Color = d_Color;
					Light.u32Flags = LIGHT_ON | LIGHT_DIRECTIONAL;
					//d_Handle = LightMgr__RegisterLight( &Light );
				}
			break;
		
			case LOCAL_LIGHT:
				{
					ts_Light Light;
					Light.Pos = d_vPosition;
					Light.Color = d_Color;
					Light.u32Flags = LIGHT_ON | LIGHT_POINT;
					//d_Handle = LightMgr__RegisterLight( &Light );
				}
			break;
	
			default:
				ASSERT( false );	// unhandled case
			break;
		}
	}

	// record start values so we know if light has changed-- updating lights takes some processing
	Vector3 vPosition = d_vPosition;
	Vector3 vRotationRadians = d_vRotationRadians;
	ts_fRGB vColor = d_Color;

	// time to go. start processing events
	while( d_fNextTimecode <= fTime )
	{
		// time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
			case DATA_STREAM_EVENT_SET_POSITION:
				if( SetPosition() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_SET_ROTATION:
				if( SetRotation() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_SET_COLOR:
				if( SetColor() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_ATTACH_TO_CURVE:
				if( AttachToCurve() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_SHOW:
//				Hide( false );
			break;
	
			case DATA_STREAM_EVENT_HIDE:	
//				Hide(t rue );
			break;

			default:
				UnhandledEvent();
			return( false );
		}

		// next key/value pair should be a new timecode or the end of data 
		if( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// process curves, if any
	EvaluateCurves(fTime);

	// any changes?
	bool bChanged = ( memcmp( &d_vPosition , &vPosition , sizeof(d_vPosition) ) != 0 );
	bChanged |= ( memcmp( &d_vRotationRadians , &vRotationRadians , sizeof(d_vRotationRadians) ) != 0 );
	bChanged |= ( memcmp ( &d_Color , &vColor , sizeof(d_Color) ) != 0 );

	// update the light
	if( bChanged )
	{
		switch( d_u32Flags & (AMBIENT_LIGHT | DIRECTIONAL_LIGHT | LOCAL_LIGHT) )
		{
			case AMBIENT_LIGHT:
				//LightMgr__SetAmbient( &d_Color );
			break;
		
			case DIRECTIONAL_LIGHT:
				CalculateLightDirection();	// convert rotation to position for directional light
				//LightMgr__UpdateLightPos( d_Handle , &d_vPosition );
				//LightMgr__UpdateLightColor( d_Handle , &d_Color );
			break;
		
			case LOCAL_LIGHT:
				//LightMgr__UpdateLightPos( d_Handle , &d_vPosition );
				//LightMgr__UpdateLightColor( d_Handle , &d_Color );
			break;
		
			default:
				ASSERT( false );	// unhandled case
			break;
		}
	}

	// success
	return( true );
}


bool CCutSceneInterpreter::ExecuteDistortion( float fTime )
{
	// time to go. start processing events
	while( d_fNextTimecode <= fTime )
	{
		// time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
			case DATA_STREAM_EVENT_SET_SCALE:
				if( SetScale() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_ATTACH_TO_CURVE:
				if( AttachToCurve() == false )
				{
					return( false );
				}
			break;
	
			default:	
				UnhandledEvent();
			return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// process curves, if any, and update the full screen distortion
	EvaluateCurves( fTime );
	d_pSignalGenerator1->SetAmplitude( d_vScale.x() );
	d_pSignalGenerator2->SetAmplitude( d_vScale.x() );
	FullScreenDistort::SetOpacity( d_vScale.y() );

	return( true );
}


bool CCutSceneInterpreter::ExecuteMotionBlur(float fTime)
{
	// time to go. start processing events
	while ( d_fNextTimecode <= fTime )
	{
		// time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
			case DATA_STREAM_EVENT_SET_SCALE:
				if( SetScale() == false )
				{
					return( false );
				}
			break;

			case DATA_STREAM_EVENT_ATTACH_TO_CURVE:
				if( AttachToCurve() == false )
				{
					return( false );
				}
			break;
		
			default:
				UnhandledEvent();
			return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// process curves, if any. the x-scale value controls the strength of the motion blur 
	// and the blur points directly to it, so there's nothing to do except evaluate the curve
	EvaluateCurves( fTime );

	return( true );
}


bool CCutSceneInterpreter::ExecuteSound( float fTime )
{
	// time to go. start processing events 
	while( d_fNextTimecode <= fTime )
	{
		// time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
			case DATA_STREAM_EVENT_SET_POSITION:
				if ( SetPosition() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_TAK_POSITION:
				if (g_players.GetPlayer(0) != NULL)
					d_vPosition = g_players.GetPlayer( 0 )->GetBodyInWorld();
			break;
		
			case DATA_STREAM_EVENT_GO:
				if( GetExecuteKey( CUT_SCENE_SOUND ) == false )
				{
					return( false );
				}

				// may be a .wav file. this is possibly-temporary support for getting .wav files through tool chain
				if( strstr( KeyValueString() , ".wav" ) )
				{
#ifndef FORCE_AUDIO_TO_FIRST_FRAME
					// play the audio stream. ignored on cdrom because it has already started it

					g_audio.dialog.Clear();	// clear out any existing voicestream, since this one has to be sync'd
					g_audio.dialog.Queue( KeyValueString() );
#endif //FORCE_AUDIO_TO_FIRST_FRAME
				}
				else
				{
					Sfx::Play( KeyValueString() , &d_vPosition );
				}
			break;
		
			default:
				UnhandledEvent();
				return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if ( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// success
	return( true );
}
bool CCutSceneInterpreter::ExecuteTrigger( float fTime )
{
	// time to go. start processing events
	while( d_fNextTimecode <= fTime )
	{
		// time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
			case DATA_STREAM_EVENT_GO:
			{
				/* what event? */

				if (GetExecuteKey(CUT_SCENE_TRIGGER_EVENT) == false)
				{
					return( false );
				}
				char goMessage[] = "Go";
				char stopMessage[] = "Stop";
				char* message;
				if( stricmp( KeyValueString() , "go" ) == 0 )
				{
					message = goMessage;
				}
				else if( stricmp(KeyValueString() , "stop" ) == 0 )
				{
					message = stopMessage;
				}
				else
				{
					ASSERTF( false , ( "Unknown cut scene trigger event: %s" , KeyValueString() ) );
					return( false );
				}

				/* who to send it to? */
				if( GetExecuteKey( CUT_SCENE_NAME ) == false )
				{
					return( false );
				}
				if( stricmp( KeyValueString() , "all" ) == 0 )
				{
					g_messageDispatcher.SendMessageToAll( message , NULL , INVALID_OBJECT_HANDLE );	// broadcast it
				}
				else
				{
					CActor *pCActor = g_stage.MasterCast().FindActorByInstanceName( KeyValueString() );
					if( pCActor == NULL )
					{
						ASSERTF( false , ( "Unknown cut scene trigger target actor: %s" , KeyValueString() ) );
						return( false );
					}
	
					// send it
					g_messageDispatcher.SendMessageToActor( message , NULL , INVALID_OBJECT_HANDLE , pCActor->GetHandle() );
				}
				break;
			}
			default:
				UnhandledEvent();
				return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// success
	return( true );
}


bool CCutSceneInterpreter::ExecuteDummyParent( float fTime )
{
	// time to go. start processing events
	while( d_fNextTimecode <= fTime )
	{
		// time to start next event
		if( GetEventType() == false )
		{
			return( false );
		}

		// process known event types for this interpreter-- every interpreter type is different
		switch( d_nEvent )
		{
			case DATA_STREAM_EVENT_ATTACH_TO_CURVE:
				if( AttachToCurve() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_SET_POSITION:
				if( SetPosition() == false )
				{
					return( false );
				}
			break;
	
			case DATA_STREAM_EVENT_SET_ROTATION:
				if( SetRotation() == false )
				{
					return( false );
				}
			break;
		
			case DATA_STREAM_EVENT_SHOW:
				Hide( false );
			break;
		
			case DATA_STREAM_EVENT_HIDE:
				Hide( true );
			break;
	
			default:
				UnhandledEvent();
				return( false );
		}

		// next key/value pair should be a new timecode or the end of data
		if( GetNextTimecode() == false )
		{
			return( false );
		}
	}

	// process curves, if any, and build transform matrix (including parent contributions)
	EvaluateCurves( fTime );
	BuildTransform();

	// success
	return( true );
}


// shared error reporting routines during execution
void CCutSceneInterpreter::UnhandledEvent()
{
	// report an unhandled event in an execute routine
	ASSERTF( false , ( "Cut scene %s (type %d), unhandled event: \"%s\"" , d_pTypeLabel , d_Type , KeyValueString() ) );
}


// get key to implement an event type
bool CCutSceneInterpreter::GetExecuteKey( int nCheckKey1 /*= CUT_SCENE_KEY_DONT_CARE*/ , int nCheckKey2 /*= CUT_SCENE_KEY_DONT_CARE*/ , int nCheckKey3 /*= CUT_SCENE_KEY_DONT_CARE*/ )
{
	if( GetKeyValuePair() == false )
	{
		return( false );
	}
	if( ( KeyID() == nCheckKey1 ) || ( KeyID() == nCheckKey2 ) || ( KeyID() == nCheckKey3 ) )
	{
		return( true );
	}

	// can also be called to get next key/value pair without caring what id it is, for peeking
	if( nCheckKey1 == CUT_SCENE_KEY_DONT_CARE )
	{
		return( true );
	}

	// look up expected strings for best error reporting 
	const char *pCheckKey1 = FindKnownKeyByID( nCheckKey1 );
	const char *pCheckKey2 = FindKnownKeyByID( nCheckKey2 );
	const char *pCheckKey3 = FindKnownKeyByID( nCheckKey3 );
	char ExpectedKeys[ 100 ];
	strcpy( ExpectedKeys , "" );
	if( pCheckKey1 )
	{
		strcat( ExpectedKeys , " \"" );
		strcat( ExpectedKeys , pCheckKey1 );
		strcat( ExpectedKeys , "\"" );
	}
	if( pCheckKey2 )
	{
		strcat( ExpectedKeys , " \"" );
		strcat( ExpectedKeys , pCheckKey2 );
		strcat( ExpectedKeys , "\"" );
	}
	if( pCheckKey3 )
	{
		strcat( ExpectedKeys , " \"" );
		strcat( ExpectedKeys , pCheckKey3 );
		strcat( ExpectedKeys , "\"" );
	}
	ASSERTF( false , ( "Cut scene %s (type %d), expected execute key(s)%s, got \"%s\"" , d_pTypeLabel , d_Type , ExpectedKeys , d_Key ) );
	return( false );
}


// look for an optional key. i do this so that i don't have rev the version number as often
bool CCutSceneInterpreter::GetOptionalKey( int nKey )
{
	// bookmark current spot, get next key
	char *pBookmark = CurrentData();
	if( GetExecuteKey( CUT_SCENE_KEY_DONT_CARE ) == false )
	{
		return(false);	// error
	}

	// if next key was requested key, then key value is now set correctly
	if( KeyID() == nKey )
	{
		return( true );
	}

	// next command wasn't optional fade command, restore it 
	SetData( pBookmark );
	return( false );
}


// the next data after an event has been processed should always be the next timecode, or the end-of-data marker
bool CCutSceneInterpreter::GetNextTimecode()
{
	if( GetExecuteKey( CUT_SCENE_TIME , CUT_SCENE_END ) == false )
	{
		return( false );
	}

	switch( KeyID() )
	{
		case CUT_SCENE_TIME:
			d_fNextTimecode = KeyValueFloat();
		break;

		case CUT_SCENE_END:
			d_fNextTimecode = MAXFLOAT;	// this interpreter is done
		break;
	
		default:
			ASSERT( false );	// if in GetExecuteKey list, must be in this case statement too
			return( false ); 
	}

	return( true );
}


// if something went wrong in executing an interpreter, try to skip to next event so that we can continue 
void CCutSceneInterpreter::SkipToNextEvent()
{
	// basically, we look for next timecode or end key
	while( true )
	{
		char *pBookMark = d_pData;
		if( GetKeyValuePair( true ) == false )	// ignore unknown keys-- we're just trying to find next timecode
		{
			d_fNextTimecode = MAXFLOAT;	// disable interpreter
			return;
		}
		if( ( KeyID() == CUT_SCENE_TIME ) || ( KeyID() == CUT_SCENE_END ) )
		{
			d_pData = pBookMark;	// back up so GetNextTimecode processes key/value pair
			GetNextTimecode();
			return;
		}
	}
}


// convert key value to an actor pointer 
CActor *CCutSceneInterpreter::LinkToActor( bool bAsTarget /*= false*/ )
{

	// tak is a special case
	CActor *pCActor;
//	if( stricmp( KeyValueString() , "tak" ) == 0 )
//	{
//		// put the avatar into cut scene mode, so that cut scene data can control him */
//		CActor *pAvatar = g_players.GetPlayer( 0 );
//
//		// need to either select a null controller, or attach a driver that does
//		// the right thing.  I'm not sure which is more appropriate here, so I'm punting.
////		ASSERTS( false , "TN - FIX_AVATAR Cut scenes need to be modified to work with the KinematicController/Driver concept" );
//
//		// some cut scenes call for weapons. set the weapon bone to visible (which sets the scale to 1.0). this 
//		// lets the cut scene artist scale the weapon bone up and down in the animation to make it visible when 
//		// desired. otherwise the scale stays at 0 (because all weapons have been put away by ToInventoryFromHand)
//		// and the weapon stays invisible regardless of the scale the artist puts on it 
//		if( ( GetOptionalKey( CUT_SCENE_SHOW_CLUB ) ) && ( KeyValueInt() != 0 ) )
//		{
//			d_u32Flags |= CLUB_VISIBLE;
//		}
//		if( ( GetOptionalKey( CUT_SCENE_SHOW_RATTLE ) ) && ( KeyValueInt() != 0 ) )
//		{
//			d_u32Flags |= RATTLE_VISIBLE;
//		}
//		if( ( GetOptionalKey( CUT_SCENE_SHOW_BLOWGUN ) ) && ( KeyValueInt() != 0 ) )
//		{
//			d_u32Flags |= BLOWGUN_VISIBLE;
//		}
//	}
//	else if( bAsTarget )
	if( bAsTarget )
	{
		pCActor = g_stage.MasterCast().FindActorByInstanceName( KeyValueString() );
	}
	else
	{
		// environmental actors in cut scene may need to be created
		if( GetExecuteKey( CUT_SCENE_NEW ) == false )
		{
			return( NULL );
		}
		if( KeyValueInt() == 1 )
		{
			// get env name to create
			if( GetExecuteKey( CUT_SCENE_CLASS ) == false )
			{
				return( NULL );
			}

			char EnvName[ 2048 ];
			strcpy( EnvName , KeyValueString() );
			if( GetExecuteKey( CUT_SCENE_MODEL_NAME ) == false )
			{
				return( NULL );
			}

			// crack the key=value; pairs into a parameter set
			ParameterSet p( EnvName );

			// override so we don't create scripts or an actor greater than what we need
			p.Remove( "ai" );
			p.Set( "class", "actor" );

			pCActor = ActorCreator::CreateFromParameters( p, Vector3(0,0,0), 0 );
			pCActor->Hide( true );

			// enable lighting
			ASSERT( pCActor->Instance()->Model() );
			if( pCActor->Instance()->Model()->dh.u16Flags & SOMM_TRI_STRIP_NORMALS )
			{
				pCActor->Instance()->SetDynamicLightFlags( INSTANCE_LIGHTING_ENABLED | INSTANCE_LIGHTING_DIRECTIONAL | INSTANCE_LIGHTING_AMBIENT );
			}

			// success-- need to clean-up after we're done
			d_u32Flags |= DELETE_ACTOR;
		}
		else
		{
			// a bit of a hack: i know that d_cKeyValue still holds the instance name of the actor to hook up to.
			// however, we've done another key load ("new", an int), so calling KeyValueString will fail because it 
			// checks the key type...so go directly to d_cKeyValue 
			ASSERT( ( d_u32Flags & DELETE_ACTOR ) == 0 );
			pCActor = g_stage.MasterCast().FindActorByInstanceName( d_cKeyValue );

			// optional command to avoid having to rev version number
			if( GetOptionalKey( CUT_SCENE_RESTORE_TO_BASE_POSE ) )
			{
				if( KeyValueInt() )
				{
					d_u32Flags |= RESTORE_TO_BASE_POSE;
				}
			}
		}
	}

	// failure?
	if( pCActor == NULL )
	{
		ASSERTF( false , ( "Cut scene %s (type %d), could not find actor \"%s\"" , d_pTypeLabel , d_Type , d_cKeyValue ) );
		return( NULL );
	}

	// set actor up for cut scene
	if( bAsTarget == false )
	{
		g_messageDispatcher.SendMessageToActor( "StartCutScene" , NULL , INVALID_OBJECT_HANDLE , pCActor->GetHandle() );
		//Actor__PauseScript(pCActor->Actor(), true);

		// turn off collisions so they can be moved around however artist wants
		if( g_stage.ProtagonistCast().IsMember( pCActor ) )
		{
			d_u32Flags |= WAS_PROTAGONIST;
			((CProtagonist *) pCActor)->MoveToNonProtagonistCast();
		}
	}

	return( pCActor );
}


// choose which camera is active
void CCutSceneInterpreter::SetActiveCamera( u32 u32CameraFlag )
{
	Camera &camera = g_cameraDirector.CurrentCamera();

	switch( u32CameraFlag )
	{
		case GENERAL_CAMERA:
			g_cameraDirector.SelectCamera( camera.GetGroup()->Name() , "General" );	
		break;
	
		case PATH_CAMERA:
			g_cameraDirector.SelectCamera( camera.GetGroup()->Name() , "Path" );	
		break;
	}

	d_u32Flags &= ~ALL_CAMERAS;
	d_u32Flags |= u32CameraFlag;
}


// evaluate this interpreter's curve set for this time
void CCutSceneInterpreter::EvaluateCurves( float fTime )
{
	for( int i = 0 ; i < d_nInterpreterCurveCount ; i++ )
	{
		float result = d_pInterpreterCurve[i].AnimCurve.Evaluate( fTime );

		switch( d_pInterpreterCurve[ i ].nType )
		{
			case CURVE_TYPE_TRANSLATE_X:
				d_vPosition.x(result);
			break;
		
			case CURVE_TYPE_TRANSLATE_Y:
				d_vPosition.y(result);
			break;
	
			case CURVE_TYPE_TRANSLATE_Z:
				d_vPosition.z(result);
			break;

			case CURVE_TYPE_ROTATE_X:
				d_vRotationRadians.x(result);
			break;
	
			case CURVE_TYPE_ROTATE_Y:
				d_vRotationRadians.y(result);
			break;
	
			case CURVE_TYPE_ROTATE_Z:
				d_vRotationRadians.z(result);
			break;
	
			case CURVE_TYPE_SCALE_X:
				d_vScale.x(result);
			break;
	
			case CURVE_TYPE_SCALE_Y:
				d_vScale.y(result);
			break;
	
			case CURVE_TYPE_SCALE_Z:
				d_vScale.z(result);
			break;
	
			case CURVE_TYPE_FOCAL_LENGTH:
				d_fFocalLength = result;
			break;
	
			case CURVE_TYPE_VERTICAL_APERTURE:
				d_fVerticalAperture = result;
			break;
	
			case CURVE_TYPE_HORIZONTAL_APERTURE:
				d_fHorizontalAperture = result;
			break;
	
			case CURVE_TYPE_COLOR_RED:
				d_Color.R = result;
			break;
	
			case CURVE_TYPE_COLOR_GREEN:
				d_Color.G = result;
			break;
	
			case CURVE_TYPE_COLOR_BLUE:
				d_Color.B = result;
			break;
	
			default:
				ASSERT( false );	// unhandled case
			break;
		}
	}
}


// build transform matrix for current position, rotation and scale values. take possible parent into account
void CCutSceneInterpreter::BuildTransform()
{
	// rotation
	d_mTransform.SetXRotation( d_vRotationRadians.x() );
	if( d_u32Flags & TRANSFORM_XZY )
	{
		d_mTransform.CatZRotation( d_vRotationRadians.z() );
		d_mTransform.CatYRotation( d_vRotationRadians.y() );
	}
	else
	{
		d_mTransform.CatYRotation( d_vRotationRadians.y() );
		d_mTransform.CatZRotation( d_vRotationRadians.z() );
	}

	// translation
	d_mTransform.PokeTranslation( d_vPosition );

	// scale 
	d_mTransform.ReverseCatScale( d_vScale.x() , d_vScale.y() , d_vScale.z() );

	// parent
	if( d_pParent )
	{
		d_mTransform *= d_pParent->d_mTransform;
	}
}


// directional light's direction is controlled by rotation from maya-- convert into a position for our light manager
void CCutSceneInterpreter::CalculateLightDirection()
{
	ASSERT( d_u32Flags & DIRECTIONAL_LIGHT );
	BuildTransform();	// scale should be (1, 1, 1)

	// rotation of (0, 0, 0) should yield position of (0, 0, 1) 
	static Vector3 vZeroZeroOne( 0.0f , 0.0f , 1.0f );
	d_vPosition.FastBasisTransform( vZeroZeroOne , d_mTransform );	// ignore translation
}


/*************************** CCutSceneInterpreter-- event processing (shared by different type interpreters) *******/
bool CCutSceneInterpreter::AttachToPath()
{
	// get path name
	if( GetExecuteKey( CUT_SCENE_PATH ) == false )
	{
		return( false );
	}

	// use of path varies depending on interpreter type 
	bool bResult = false;
	switch( d_Type )
	{
		case CAMERA:
			bResult = d_pPathCamera->SetPath( (SplinePath *)KeyValueString() );
		break;

		default:	
			ASSERT( false );	// unhandled case
		break;
	}

	if( bResult == false )
	{
		ASSERTF( false, ( "Cut scene %s (type %d), path not found: \"%s\"" , d_pTypeLabel , d_Type , d_cKeyValue ) );
		return( false );
	}

	// success 
	return( true );
}


// attach to a named curve
bool CCutSceneInterpreter::AttachToCurve()
{
	// get curve type
	int nType = GetCurveType();
	if( nType == CURVE_TYPE_UNKNOWN )
	{
		return( false );
	}

	// get curve to attach to-- curve key is next in data
	if( GetExecuteKey( CUT_SCENE_CURVE ) == false )
	{
		return( false );
	}

	// look for curve in loaded dbl chunks
	int i;
	ts_AnimCurve *pCurve = NULL;
	for( i = 0 ; i < d_nAnimCurveChunkCount ; i++ )
	{
		// loop through curves in this chunk 
		ts_RecordList *pRecordList = (ts_RecordList *) d_pAnimCurveChunkData[ i ];
		int j;
		for( j = 0 ; j < pRecordList->usRecordCount ; j++ )
		{
			pCurve = (ts_AnimCurve *) GetRecordAddress( pRecordList , j );
			if( stricmp( KeyValueString() , pCurve->Name) == 0 )
			{
				break;
			}
			pCurve = NULL;	// didn't match, keep it NULL for not-found check below
		}
		if( j < pRecordList->usRecordCount )
		{
			break;
		}
	}
	if( pCurve == NULL )
	{
		ASSERTF( false , ("Cut scene %s (type %d), curve \"%s\" not found" , d_pTypeLabel , d_Type , KeyValueString() ) );
		return( false );
	}

	// only one curve of each type active at a time. does this type already exist? if so, replace it
	int nExistingIndex = MAXINT;
	for( i = 0 ; i < d_nInterpreterCurveCount ; i++ )
	{
		if( nType == d_pInterpreterCurve[ i ].nType )
		{
			nExistingIndex = i;
			break;
		}
	}

	// found the curve, all is ok. either replace existing curve of same type or add to end of list
	if( nExistingIndex < d_nInterpreterCurveCount )
	{
		d_pInterpreterCurve[ nExistingIndex ].AnimCurve.Initialize( pCurve );
	}
	else
	{
		// make sure there's a curve slot available
		if( d_nInterpreterCurveCount >= d_nInterpreterCurveMaxCount )
		{
			ASSERT( false );	// curve_count key/value pair is too low
			return( false );
		}

		// add the curve
		d_pInterpreterCurve[d_nInterpreterCurveCount].AnimCurve.Initialize(pCurve);
		d_pInterpreterCurve[d_nInterpreterCurveCount].nType = nType;
		d_nInterpreterCurveCount++;	// another curve slot used for this interpreter
	}

	// success
	return( true );
}


// set the interpreter's position variable-- especially good for particle and sound interpreters, though can be useful
// for all
bool CCutSceneInterpreter::SetPosition()
{
	if( GetExecuteKey( CUT_SCENE_X ) == false )
	{
		return( false );
	}
	d_vPosition.x( KeyValueFloat() );
	if( GetExecuteKey( CUT_SCENE_Y ) == false )
	{
		return( false );
	}
	d_vPosition.y( KeyValueFloat() );
	if( GetExecuteKey( CUT_SCENE_Z ) == false )
	{
		return( false );
	}
	d_vPosition.z( KeyValueFloat() );

	return( true );
}


// same for rotation
bool CCutSceneInterpreter::SetRotation()
{
	if( GetExecuteKey( CUT_SCENE_X ) == false )
	{
		return( false );
	}
	d_vRotationRadians.x( KeyValueFloat() );
	if( GetExecuteKey( CUT_SCENE_Y ) == false )
	{
		return( false );
	}
	d_vRotationRadians.y( KeyValueFloat() );
	if( GetExecuteKey( CUT_SCENE_Z ) == false )
	{
		return( false );
	}
	d_vRotationRadians.z( KeyValueFloat() );

	return( true );
}


// same for scale
bool CCutSceneInterpreter::SetScale()
{
	if( GetExecuteKey( CUT_SCENE_X ) == false )
	{
		return( false );
	}
	d_vScale.x( KeyValueFloat() );
	if( GetExecuteKey( CUT_SCENE_Y ) == false )
	{
		return( false );
	}
	d_vScale.y( KeyValueFloat() );
	if( GetExecuteKey( CUT_SCENE_Z ) == false )
	{
		return( false );
	}
	d_vScale.z( KeyValueFloat() );

	return( true );
}


// same for color
bool CCutSceneInterpreter::SetColor()
{
	if( GetExecuteKey( CUT_SCENE_RED ) == false )
	{
		return( false );
	}
	d_Color.R = KeyValueFloat();
	if( GetExecuteKey( CUT_SCENE_GREEN ) == false )
	{
		return( false );
	}
	d_Color.G = KeyValueFloat();
	if( GetExecuteKey( CUT_SCENE_BLUE ) == false )
	{
		return( false );
	}
	d_Color.B = KeyValueFloat();

	return( true );
}


// hide/show have different meanings for different interpreters, and should also be passed to children
void CCutSceneInterpreter::Hide( bool bHide )
{
	// take action for this interpreter
	switch( d_Type )
	{
		case ACTOR:
			d_pCActor->Hide( bHide );
		break;

		case PARTICLE_EMITTER:	
			if( bHide )
			{
				ParticleEmitter__Deactivate( d_Handle );
			}
			else
			{
				ParticleEmitter__Activate( d_Handle );
			}
		break;
	}

	// now recurse through children. it's cheesy, data-design-wise, for interpreter to know about cut scene that
	// created it just so it can recurse through all the other interpreters, but i just didn't want to deal with a
	// variable number of children. also, this isn't efficient, looking through all interpreters for ones that are 
	// children of current interpreter
	CCutSceneInterpreter *pInterpreter = d_pCutSceneMgr->GetInterpreterHead();
	while( pInterpreter )
	{
		if( pInterpreter->GetParent() == this )
		{
			pInterpreter->Hide( bHide );	// mildly recursive
		}
		pInterpreter = pInterpreter->Next();
	}
}


//************************** CCutSceneInterpreter-- basic parsing of cut scene text data *********************
// get the next key/value pair 
bool CCutSceneInterpreter::GetKeyValuePair( bool bIgnoreUnknownKey /*= false*/ )
{
	// get key
	ASSERT( ( (int)d_pData & 3 ) == 0 );	// should be 4-byte aligned
	d_nKeyID = *(int *) d_pData;
#ifdef GCN
	ByteSwap( d_nKeyID );
#endif //GCN
	d_pData += sizeof(int);

	// find key in list
	int i;
	for( i = 0 ; i < d_nKeyListSize ; i++ )
	{
		if( KeyList[ i ].nValue == d_nKeyID )
		{
			break;
		}
	}
	if( i == d_nKeyListSize )
	{
		ASSERTF( false , ( "Cut scene %s (type %d), key not found: %d" , d_pTypeLabel , d_Type , d_nKeyID ) );
		return( false );
	}
	strcpy( d_Key , KeyList[ i ].Name );	// just to support debug messages

	// get value
	switch( KeyList[ i ].KeyType )
	{
		case KEY_INT:
			d_KeyType = KEY_INT;
			d_nKeyValue = *(int *) d_pData;
#ifdef GCN
			ByteSwap( d_nKeyValue );
#endif //GCN
			d_pData += sizeof(int);
		break;

		case KEY_FLOAT:
			d_KeyType = KEY_FLOAT;
			d_fKeyValue = *(float *) d_pData;
#ifdef GCN
			ByteSwap( d_fKeyValue );
#endif //GCN
			d_pData += sizeof(float);
		break;

		case KEY_STRING:
			d_KeyType = KEY_STRING;
			strcpy( d_cKeyValue , d_pData );
			d_pData += ( strlen( d_cKeyValue ) + 1 + 3 ) & ~3;	// keep it 4-byte aligned
		break;
	
		default:
			ASSERTF( false , ( "Cut scene %s (type %d), unknown key type: %d" , d_pTypeLabel , d_Type , KeyList[ i ].KeyType ) );
			return( false );
	}

	return( true );
}


// find the requested label in the key/value pair data
bool CCutSceneInterpreter::GoToLabel( const char *pLabel )
{
	// the passed label is usually a pointer to the interpreter's d_cKeyValue field, so copy it
	char Label[ 100 ];
	strcpy( Label , pLabel );

	// look for it
	while( true )
	{
		if( GetKeyValuePair( true ) == false )	// ignore unknown keys-- we're just trying to find a label
		{
			return( false );
		}
		if( ( KeyID() == CUT_SCENE_LABEL ) && ( stricmp( Label, KeyValueString() ) == 0) )
		{
			return( true );
		}
	}
}


//************************** CCutSceneInterpreter-- search defined lists *********************************
// find a key string by id
const char *CCutSceneInterpreter::FindKnownKeyByID( int nKeyID )
{
	int i;
	for( i = 0 ; i < d_nKeyListSize ; i++ )
	{
		if( nKeyID == KeyList[ i ].nValue )
		{
			break;
		}
	}
	if( i == d_nKeyListSize )
	{
		return( NULL );
	}

	return( KeyList[ i ].Name );
}


// figure out which event type this is
bool CCutSceneInterpreter::GetEventType()
{
	// event type key is next in data
	if( GetExecuteKey( CUT_SCENE_EVENT_TYPE ) == false )
	{
		return( false );
	}

	d_nEvent = KeyValueInt();
	ASSERT( d_nEvent < MAX_EVENT_TYPES );
	return( true );
}


// search the curve type table
int CCutSceneInterpreter::GetCurveType()
{
	// curve type key is next in data
	if( GetExecuteKey( CUT_SCENE_CURVE_TYPE ) == false )
	{
		return( CURVE_TYPE_UNKNOWN );
	}
	ASSERT( KeyValueInt() < MAX_CURVE_TYPES );
	return( KeyValueInt() );
}


// look through loaded dbl files for anim curve data that interpreters may need to hook up to
//** static function ***
void CCutSceneInterpreter::FindAnimCurveData()
{
	d_nAnimCurveChunkCount = 0;

	// look for curve in loaded dbl chunks
	for ( DataMgr::OwnerRecord* owner = DataMgr::FirstOwner() ; owner ; owner = DataMgr::NextOwner() )
	{
		if ( owner->d_type == DATA_DBL_FILE )
		{
			for ( DataMgr::DataRecord* dataRec = owner->FirstData() ; dataRec ; dataRec = dataRec->next )
			{
				if ( dataRec->d_type == DB_ANIM_CURVE )
				{
					ASSERT( d_nAnimCurveChunkCount < MAX_ANIM_CURVE_CHUNKS );
					d_pAnimCurveChunkData[ d_nAnimCurveChunkCount++ ] = dataRec->d_userData;
				}
			}
		}
	}
}


/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*** cut scene manager ***/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/

CCutSceneMgr::CCutSceneMgr()
{
	d_pMediaFile = NULL;
	d_pDBLBuffer = NULL;
	d_pCutSceneData = NULL;
	d_CutDataHandle = INVALID_HANDLE;
	d_CurveDataHandle = INVALID_HANDLE;
	d_AnimDataHandle = INVALID_HANDLE;
	d_pInterpreterHead = NULL;
	d_TopSpriteHandle = INVALID_HANDLE;
	d_BottomSpriteHandle = INVALID_HANDLE;
	d_u32Flags = 0;

	// create some permanent interpreters to be used during load screen to avoid memAlloc
	for( int i = 0 ; i < MAX_PERMANENT_INTERPRETERS ; i++ )
	{
		d_pPermanentInterpreter[ i ] = new CCutSceneInterpreter;
		d_pPermanentInterpreter[ i ]->SetPermanent();
	}
}


CCutSceneMgr::~CCutSceneMgr()
{
	Cleanup( true );
}

// clean up after cut scene ends, or is interrupted
bool CCutSceneMgr::Cleanup( bool bDeletePermanent /*= false*/ )
{
	// note whether a cut scene was interrupted, for development purposes
	bool bInterrupted = ( d_pCutSceneData != NULL );

	// free data
	if( d_CutDataHandle != INVALID_HANDLE )
	{
		dblRemove( d_CutDataHandle );
	}
	if( d_CurveDataHandle != INVALID_HANDLE )
	{
		dblRemove( d_CurveDataHandle );
	}
	if( d_AnimDataHandle != INVALID_HANDLE )
	{
		dblRemove( d_AnimDataHandle );
	}
	if( d_pDBLBuffer )
	{
		memFree( d_pDBLBuffer );
	}
	d_pCutSceneData = NULL;
	d_CutDataHandle = INVALID_HANDLE;
	d_CurveDataHandle = INVALID_HANDLE;
	d_AnimDataHandle = INVALID_HANDLE;
	d_pDBLBuffer = NULL;
	CCutSceneInterpreter::SetAnimDataHandle(INVALID_HANDLE);

	// cleanup interpreters
	DeallocInterpreters();

	// may want to delete permanent interpreters
	if( bDeletePermanent )
	{
		for( int i = 0 ; i < MAX_PERMANENT_INTERPRETERS ; i++ )
		{
			if( d_pPermanentInterpreter[ i ] )
			{
				delete d_pPermanentInterpreter[ i ];
			}
			d_pPermanentInterpreter[ i ] = NULL;
		}
	}

	// restore timer cap
	g_timer.SetCap( TIMER_MAX_MS );

	// restore lights
	if( d_u32Flags & CUT_SCENE_AMBIENT_LIGHTS_REMOVED )
	{
		//LightMgr__RestoreAmbientLight();
	}
	if( d_u32Flags & CUT_SCENE_DIRECTIONAL_LIGHTS_REMOVED )
	{
		//LightMgr__RestoreDirectionalLights();
	}
	if( d_u32Flags & CUT_SCENE_LIGHT_SET_ACTIVE )
	{
		g_lightSet.enableLightSet( true );
	}
	d_u32Flags &= ~(CUT_SCENE_AMBIENT_LIGHTS_REMOVED | CUT_SCENE_DIRECTIONAL_LIGHTS_REMOVED | CUT_SCENE_LIGHT_SET_ACTIVE);

	// remove black cinema bars (but only if cut scene created them-- somebody outside may have requested them)
	if( ( d_u32Flags & CUT_SCENE_DISABLE_BLACK_BARS ) == 0 )
	{
		RemoveBlackCinemaBars();
	}

	// we're done
	d_u32Flags &= ~( CUT_SCENE_PLAYING | CUT_SCENE_WAIT_FOR_AUDIO_START | CUT_SCENE_HAS_EXECUTED );

	// return interrupted status
	return( bInterrupted );
}


// dealloc any malloc'd interpreters
void CCutSceneMgr::DeallocInterpreters()
{
	while( d_pInterpreterHead )
	{
		CCutSceneInterpreter *pNextInterpreter = d_pInterpreterHead->Next();
		DeallocInterpreter( d_pInterpreterHead );
		d_pInterpreterHead = pNextInterpreter;
	}
	d_pInterpreterHead = NULL;
}


// alloc an interpreter, prefer one of the permanent interpreters
CCutSceneInterpreter *CCutSceneMgr::AllocInterpreter()
{
	// look for permanent interpreter first
	int i;
	for( i = 0 ; i < MAX_PERMANENT_INTERPRETERS ; i++ )
	{
		if( d_pPermanentInterpreter[ i ]->IsFlagSet( CCutSceneInterpreter::PERMANENT_AVAILABLE ) )
		{
			break;
		}
	}
	if( i != MAX_PERMANENT_INTERPRETERS )
	{
		ASSERT( d_pPermanentInterpreter[ i ]->IsFlagSet(CCutSceneInterpreter::PERMANENT ) );
		d_pPermanentInterpreter[ i ]->ClearFlag( CCutSceneInterpreter::PERMANENT_AVAILABLE );

		// since this interpreter is permanent, it does not go through the constructor, so make sure we're clean
		d_pPermanentInterpreter[ i ]->Initialize( false );

		// return it
		return( d_pPermanentInterpreter[ i ] );
	}

	// malloc a new one 
	ASSERT( ( d_u32Flags & CUT_SCENE_LOAD_SCREEN ) == 0 );	// if this hits, increase MAX_PERMANENT_INTERPRETERS or tell ned
	return( new CCutSceneInterpreter );
}


// de-alloc an interpreter, watching for permanent ones
void CCutSceneMgr::DeallocInterpreter( CCutSceneInterpreter *pInterpreter )
{
	pInterpreter->Cleanup();

	// may be a permanent interpreter
	if( pInterpreter->IsFlagSet( CCutSceneInterpreter::PERMANENT ) )
	{
		pInterpreter->SetFlag( CCutSceneInterpreter::PERMANENT_AVAILABLE );
	}
	else
	{
		delete pInterpreter;
	}
}


// play a cut scene (this starts it, Update runs it)
bool CCutSceneMgr::PlayCutScene( const char *pName ,	bool bInterruptable /*= false*/ , int nCameraIndex /*= 0*/ , bool bBlackBars /*= true*/ )
{
	// clean up-- typically nothing to do
	bool bCleanupInterrupt = Cleanup();

	// record input parameters
	d_u32Flags = 0;
	if( bInterruptable )
	{
		d_u32Flags |= CUT_SCENE_INTERRUPTABLE;
	}
	if( bBlackBars == false )
	{
		d_u32Flags |= CUT_SCENE_DISABLE_BLACK_BARS;
	}
	d_nCameraIndex = nCameraIndex;

	// record results of Cleanup call
	if( bCleanupInterrupt )
	{
		d_u32Flags |= CUT_SCENE_CLEANUP_INTERRUPT;
	}

#ifdef CDROM

	// start asynchronous load
	d_pMediaFile = Media::StartAsyncLoad( pName , &d_pDBLBuffer , &d_nDBLBufferSize );
	if( d_pMediaFile == NULL )
	{
		return( false );	// file not found, or not enough memory
	}

#else //CDROM

	// tell the file system that it's ok to load these files midgame, but don't record them for inclusion in uberfile
//	dblEnableLoad( true );
//	FileStream::EnableLoad( true );

	// load .cut file
	d_CutDataHandle = dblLoad( pName );

	// disable midgame file loads
//	dblEnableLoad( false );
//	FileStream::EnableLoad( false );

	// maybe failed
	if( d_CutDataHandle == INVALID_HANDLE )
	{
		return( false );
	}

#endif //CDROM

	// so far so good. let Update know that a cut scene is loading. also, cut scene is now officially playing
	// as far as outside program is concerned. also, for a cdrom we have to queue audio, if any, before cut
	// scene can start, for syncing purposes
	d_u32Flags |= ( CUT_SCENE_LOADING | CUT_SCENE_PLAYING | CUT_SCENE_QUEUE_AUDIO );

	// suspend user input during cut scene
	g_players.ActivateControls( false );

	// avatar is protected during cut scene
//	ASSERTS( false , "TN - FIX_AVATAR There is no longer an avatar.  Figure out how to make the focus object invincible" );

	// some cut scenes may need global time adjust to improve lip sync. not sure why-- the process of capturing the 
	// cut scenes as avi's should be solid. but the "cooperate" one, in particular, is way off 
	d_fKFMStartTimeAdjust = 0.0f;

	return( true );
}

// finish cut scene setup after call to PlayCutScene. this use to be part of PlayCutScene, but i needed to break it into
// two pieces to support asynchronous file loads in a cdrom build-- load is started, game render loop continues, when file
// is loaded this completes the initialization 
bool CCutSceneMgr::FinishLoad()
{
#ifdef CDROM

	// wait for asynchronous file load to finish
	t_Error error;
	if( d_pMediaFile->AwaitIO( 0 , NULL , NULL , &error ) == false )	// false means wait some more
	{
		return( false );
	}

	// file is loaded
	d_u32Flags &= ~CUT_SCENE_LOADING;
	d_pMediaFile->Close();
	d_pMediaFile = NULL;	// don;t have to delete it, it comes from Media::openList array

	// process the file contents
	d_CutDataHandle = dblLoad( "cutscene.cut" , (u8 *) d_pDBLBuffer , d_nDBLBufferSize );	// file name doesn't matter

	// handle unlikely failure
	if( d_CutDataHandle == INVALID_HANDLE )
	{
		StopCutScene( true );
		return( false );
	}

	// dbl buffer will be free'd when dbl is removed, so don't have to worry about it any more
	d_pDBLBuffer = NULL;

	// no audio name found yet (for syncing audio and cut scene in cdrom build)
	strcpy( d_AudioName, "" );

#else //CDROM

	// in a non-cdrom build, we've already loaded the .dbl file. so just clear the CUT_SCENE_LOADING flag
	// so this doesn't get called again
	d_u32Flags &= ~CUT_SCENE_LOADING;

#endif //CDROM

	// record if light set is active or not. there are two light managers: LightMgr and CLightSet
	if( g_lightSet.lightSetIsLoaded() )
	{
		d_u32Flags |= CUT_SCENE_LIGHT_SET_ACTIVE;
	}

	// record current camera/scene status so we can restore after cut scene is done. for development, it's ok 
	// to interrupt a cut scene with another (or a repeat of the same one). if interrupting, camera is wrong 
	// (it's set to camera being used by cut scene), so don't record it. have to do this before InitInterpreters 
	// JMH - dont save off old camera if flag is set (used for back-2-back cutscenes 
	if( ( ( d_u32Flags & CUT_SCENE_DONT_SAVE_CAMERA ) == 0 ) &&
			( ( d_u32Flags & CUT_SCENE_CLEANUP_INTERRUPT ) == 0 ) )
	{
		CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();
		Camera camera = cameraScene->CurrentCamera();

		d_SaveCameraName = (char *)camera.Name();
//	d_vSaveGeneralUp = d_pGeneralCamera->Up();
// *** need an up vector restored here ***

		d_fSaveFieldOfViewY = SceneMgr::GetFirstScene()->d_sceneView.GetFieldOfViewY();	// assumes that there's only one scene to worry about
		d_fSaveAspectRatio = SceneMgr::GetFirstScene()->d_sceneView.GetAspectRatio();	// assumes that there's only one scene to worry about
	}

	// JMH - this is a one-shot flag 
	d_u32Flags &= ~CUT_SCENE_DONT_SAVE_CAMERA;

	// point to cut scene data 
	int nCutSceneDataSize;
	d_pCutSceneData = (char *) dblGetChunkData( DB_CUT_BINARY , DEFAULT_CHUNK_KEY_STRING , d_CutDataHandle , &nCutSceneDataSize );
	ASSERTS( d_pCutSceneData , "Couldn't find binary cut scene data" );

	// log optional additional anim data. there is almost always additional anim data for cut scenes, for lip synching, etc
	ASSERT( CCutSceneInterpreter::GetAnimDataHandle() == INVALID_HANDLE );
	if( dblGetChunkData( DB_MOTION , DEFAULT_CHUNK_KEY_STRING , d_CutDataHandle ) != NULL )
	{
		CCutSceneInterpreter::SetAnimDataHandle( d_CutDataHandle );	// let interpreters know about the data
	}

	// log the locations of anim curves we may need to use 
	CCutSceneInterpreter::FindAnimCurveData();
	
	// if in preview mode, enable midgame file loads before interpreters are initialized, so that we don't complain
	// about models that get loaded as cut scene participants
//	if( d_u32Flags & CUT_SCENE_PREVIEW )
//	{
//		dblEnableLoad( true );
//		FileStream::EnableLoad( true );
//	}

	// process file to set up an interpreter for each data stream in the cut scene
	if( InitInterpreters() == false )
	{
		StopCutScene( true );
		return( false );
	}

	// disable midgame file loads
//	dblEnableLoad( false );
//	FileStream::EnableLoad( false );

	// no timer cap, so timing is accurate-- have to stay in sync with soundtrack
	g_timer.SetCap( TIMER_NO_CAP );

	// maybe start a fade
	if( d_u32Flags & CUT_SCENE_FADE_IN )
	{
		RenderMgr__FadeOut( FADE_TO_BLACK_FAST , true );	// fade out game scene before starting cut scene. true == can be paused
		d_u32Flags |= CUT_SCENE_WAIT_FOR_FADE_OUT;		// so Update knows to wait
	}

	// success
	d_fCutSceneTimer = d_fCutSceneStart;	// counts up to process timed events in data streams
	return( true );
}


// stop a cut scene
bool CCutSceneMgr::StopCutScene( bool bOverrideInterruptable /*= false*/ )
{
	// any cut scene playing?
	if( IsCutScenePlaying() == false )
	{
		return( false );
	}

	// check flags
	if( ( ( d_u32Flags & CUT_SCENE_INTERRUPTABLE ) == 0 ) && ( bOverrideInterruptable == false ) )
	{
		return( false );
	}

	// if fading out or faded out, need to fade back in. this may be due to fading out after cut scene is over, or
	// to interrupting a cut scene while it was fading out but before it began 
	bool bFadeInNeeded = ( ( RenderMgr__IsFadingOut() ) || ( RenderMgr__IsFadedOut() ) );

	// stop dialog
	g_audio.dialog.Clear();

	// clean up 
	Cleanup();

	// maybe fade back in
	if( bFadeInNeeded )
	{
		RenderMgr__FadeIn( FADE_TO_BLACK_FAST , true );	// true == can be paused
	}

	// restore camera 
#ifdef OLD_CAMERA
	g_cameraSystem[ d_nCameraIndex ].EnableCameraShake( true );
#endif
	if( ( d_u32Flags & CUT_SCENE_LEAVE_CAMERA ) == 0 )
	{
#ifdef OLD_CAMERA
		g_cameraSystem[ d_nCameraIndex ].SelectCamera( d_SaveCameraID );

		// restore general camera up vector
		( (GeneralCamera *) g_cameraSystem[ d_nCameraIndex ].GetCamera( CAMERA_ID_GENERAL ) )->SetUp( d_vSaveGeneralUp );
#else
		// *** restore up vector here *** 
#endif

		// restore field of view, aspect ratio 
		SceneMgr::GetFirstScene()->d_sceneView.SetFieldOfViewY( d_fSaveFieldOfViewY );
		SceneMgr::GetFirstScene()->d_sceneView.SetAspectRatio( d_fSaveAspectRatio );
	}

	// restore user input
	g_players.ActivateControls( true );

	// avatar is vulnerable again
//	ASSERTS( false , "TN - FIX_AVATAR There is no longer an avatar.  Figure out how to make the focus object vulnerable again" );

	return( true );
}


// create a set of interpreters, one per data stream in the cut scene
bool CCutSceneMgr::InitInterpreters()
{
	// first we have to find the requested cut scene's data. this should be a FILE type interpreter
	CCutSceneInterpreter *pFileInterpreter = AllocInterpreter();
	ASSERT( pFileInterpreter );
	pFileInterpreter->Initialize( d_pCutSceneData , d_pCutSceneDataEnd , NULL , this , ( d_u32Flags & CUT_SCENE_LOAD_SCREEN ) != 0 );
	if( pFileInterpreter->Type() != CCutSceneInterpreter::FILE )
	{
		ASSERTS( false , "InitializeInterpreters: Cut scene data must start with FILE-type interpreter" );
		return( false );
	}

	// get to data for requested cut scene. unlike the other interpreters, the FILE type has a known 
	// order of key/value pairs 
	if( pFileInterpreter->GetExecuteKey( CCutSceneInterpreter::CUT_SCENE_VERSION ) == false )
	{
		return( false );
	}
	float fVersion = pFileInterpreter->KeyValueFloat();
	if( fVersion != 1.3f )
	{
		ASSERTS( false , "InitializeInterpreters: Bad cut scene data version" );
		return( false );
	}

	// now we are at the data for the requested cut scene. a bit more housekeeping before we get to data stream(s)
	if( pFileInterpreter->GetExecuteKey( CCutSceneInterpreter::CUT_SCENE_START ) == false )
	{
		return( false );
	}
	d_fCutSceneStart = pFileInterpreter->KeyValueFloat();
	if( pFileInterpreter->GetExecuteKey( CCutSceneInterpreter::CUT_SCENE_LENGTH ) == false )
	{
		return( false );
	}
	d_fCutSceneLength = pFileInterpreter->KeyValueFloat();
	if( d_fCutSceneLength <= 0.0f )
	{
		return( false );	// no ASSERT, it may be handy to disable cut scenes by editing data file, setting length to 0
	}

	// i'm going to allow optional commands so i don't have to rev the version number as often 
	ASSERT( fVersion == 1.3f );	// if the version revs i can make these optional commands into required ones
	if( pFileInterpreter->GetOptionalKey( CCutSceneInterpreter::CUT_SCENE_FADE ) )
	{
		d_u32Flags |= pFileInterpreter->KeyValueInt();	// key value is CUT_SCENE_FADE_IN and/or CUT_SCENE_FADE_OUT
	}

	#ifdef ALLOW_VIDEO_RECORD_CUTSCENE
	if( g_videoRecordCutScene )
	{
		d_u32Flags &= ~( CUT_SCENE_FADE_IN | CUT_SCENE_FADE_OUT );	// can tell when cut scene starts when cinema bars show up/disappear
	}
	#endif

	// get number of data streams
	if( pFileInterpreter->GetExecuteKey( CCutSceneInterpreter::CUT_SCENE_COUNT ) == false )
	{
		return( false );
	}
	d_nCutSceneDataStreamCount = pFileInterpreter->KeyValueInt();

	// ok, now we are at data streams. create an interpreter for each data stream 
	for( int i = 0 ; i < d_nCutSceneDataStreamCount ; i++ )
	{
		if( pFileInterpreter->GetExecuteKey( CCutSceneInterpreter::CUT_SCENE_AT_LABEL ) == false )
		{
			return( false );
		}
		char *pBookmark = pFileInterpreter->CurrentData();

		// go to label to get to data stream
		if( pFileInterpreter->GoToLabel(pFileInterpreter->KeyValueString() ) == false )
		{
			return( false );
		}

		// create a new interpreter for this data stream
		CCutSceneInterpreter *pNewInterpreter = AllocInterpreter();
		ASSERT( pNewInterpreter );
		bool bResult = pNewInterpreter->Initialize( pFileInterpreter->CurrentData() , d_pCutSceneDataEnd , d_pInterpreterHead , this , ( d_u32Flags & CUT_SCENE_LOAD_SCREEN ) != 0 );
		if( bResult )
		{
			d_pInterpreterHead = pNewInterpreter;	// new interpreter is now head of list
		}
		else
		{
			DeallocInterpreter( pNewInterpreter );
		}

		// back to bookmark to get next data stream label
		pFileInterpreter->SetData( pBookmark );
	}

	// delete file interpreter-- its only task was to set up per-data-stream interpreters
	DeallocInterpreter( pFileInterpreter );

	// hook interpreters up to their parents, if any 
	CCutSceneInterpreter *pInterpreter = d_pInterpreterHead;
	while( pInterpreter )
	{
		if( pInterpreter->GetParentIndex() == -1 )
		{
			pInterpreter->SetParent( NULL );
		}
		else
		{
			// find parent
			CCutSceneInterpreter *pParentInterpreter = d_pInterpreterHead;
			while( pParentInterpreter && ( pParentInterpreter->GetIndex() != pInterpreter->GetParentIndex() ) )
			{
				pParentInterpreter = pParentInterpreter->Next();
			}
			if( pParentInterpreter )
			{
				pInterpreter->SetParent( pParentInterpreter );
			}
			else
			{
				ASSERTS( false , "Parent interpreter not found" );
			}
		}
		pInterpreter = pInterpreter->Next();
	}

	return( true );
}


// add opaque black sprites to top and bottom of screen to give the "cinema" look. this is available for outside use as well
void CCutSceneMgr::AddBlackCinemaBars()
{
	// if already on, don't do it again
	if( d_TopSpriteHandle != INVALID_HANDLE )
	{
		return;
	}

	// add black "cinema" bars on top and bottom of screen 
#define BAR_HEIGHT	0.05f
	static Vector3 TopVertices[ 4 ] = { Vector3( 0.0f , 0.0f , 0.0f ), Vector3( 1.0f , 0.0f , 0.0f ), Vector3( 1.0f , BAR_HEIGHT , 0.0f ), Vector3( 0.0f , BAR_HEIGHT , 0.0f ) };
	static Vector3 BottomVertices[ 4 ] = { Vector3( 0.0f , 1.0f - BAR_HEIGHT , 0.0f ) , Vector3( 1.0f , 1.0f - BAR_HEIGHT , 0.0f ) , Vector3( 1.0f , 1.0f , 0.0f ) , Vector3( 0.0f , 1.0f , 0.0f ) };
	static ts_bRGBA BlackColor = { 0 , 0 , 0 , 255 };
	d_TopSpriteHandle = SpriteMgr__RegisterSprite( TS_NO_TEXTURE , SPRITE_DEFAULT_GROUP , 0xffff , SPRITE_ALL_ONE_COLOR , TopVertices , NULL , &BlackColor );
	d_BottomSpriteHandle = SpriteMgr__RegisterSprite( TS_NO_TEXTURE , SPRITE_DEFAULT_GROUP , 0xffff , SPRITE_ALL_ONE_COLOR , BottomVertices , NULL , &BlackColor );
}


// remove them 
void CCutSceneMgr::RemoveBlackCinemaBars()
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
}


// tell camera interpreter to switch to general camera as default
void CCutSceneMgr::StartGeneralCamera()
{
	CCutSceneInterpreter *pInterpreter = d_pInterpreterHead;
	while( pInterpreter )
	{
		if( pInterpreter->Type() == CCutSceneInterpreter::CAMERA )
		{
			pInterpreter->SetActiveCamera( CCutSceneInterpreter::GENERAL_CAMERA );
		}
		pInterpreter = pInterpreter->Next();
	}
}


void CCutSceneMgr::RestoreCamera()
{
	CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();
	Camera camera = cameraScene->CurrentCamera();
	
	// *** set cam up vector here ***


	/* restore field of view, aspect ratio */
	SceneMgr::GetFirstScene()->d_sceneView.SetFieldOfViewY( d_fSaveFieldOfViewY );
	SceneMgr::GetFirstScene()->d_sceneView.SetAspectRatio( d_fSaveAspectRatio );
}

/* called by SimLoop::Run once per frame */

void CCutSceneMgr::Update()
{
	if( IsCutScenePlaying() == false )
	{
		return;
	}

	// waiting for asynchronous file load to finish?
	if( ( d_u32Flags & CUT_SCENE_LOADING ) && ( FinishLoad() == false ) )
	{
		//printf("waiting for .cut file to load\n");
		return;
	}

	// if paused and cut scene hasn't started executing, return. this fixes a bug where a cut scene plays right after 
	// the loading screen-- if the joystick is pulled, the game is paused, but the request to play the cut scene gets 
	// through (side note: can't just return here when paused if cut scene is already playing, since the actors have 
	// to be placed in their correct positions every frame) 
	if( g_timer.IsPaused() && ( ( d_u32Flags & CUT_SCENE_HAS_EXECUTED ) == 0 ) )
	{
		return;
	}

	// check for aborting interruptable cut scenes
	if( g_input.GetController( 0 )->ButtonPressed( CB_X ) )
	{
		StopCutScene();	// will not abort unless cut scene is marked as interruptable
	}

	// waiting for fade out (before cut scene starts) to complete?
	if( ( d_u32Flags & CUT_SCENE_WAIT_FOR_FADE_OUT ) && ( RenderMgr__IsFadingOut() ) )
	{
		return;
	}

#ifdef FORCE_AUDIO_TO_FIRST_FRAME
	// on cdrom, need to queue audio and then wait for it to start before starting cut scene so that they are synced
	if( d_u32Flags & CUT_SCENE_QUEUE_AUDIO )
	{
		d_u32Flags &= ~CUT_SCENE_QUEUE_AUDIO;
		if( d_AudioName[ 0 ] != 0 )
		{
			// we have an audio stream to queue 
			g_audio.dialog.Clear();	// clear out any existing voicestream, since this one has to be sync'd
			bool bResult = g_audio.dialog.QueueCallback( d_AudioName , DialogStartedCallback , NULL , this );
			if( bResult )
			{
				// we're waiting. register for a callback when audio actually starts
				d_u32Flags |= CUT_SCENE_WAIT_FOR_AUDIO_START;
				d_nAudioTimeOut = EngineTimer__Time() + 3000;	// abort if not started after 3 seconds
			}
		}
	}

	// waiting for audio to start? must come after queue check. DialogStartedCallback will clear this bit when called 
	if( d_u32Flags & CUT_SCENE_WAIT_FOR_AUDIO_START )
	{
		//printf("waiting for audio to start\n");
		// check time-out-- this is paranoid, but if audio track never starts we'll wait forever without a time-out
		if( EngineTimer__Time() > (u32)d_nAudioTimeOut )
		{
			// pretend that the callback was called
			DialogStartedCallback( this , NULL );
		}
		return;
	}
#endif //FORCE_AUDIO_TO_FIRST_FRAME

	// need to fade back in after fade out from game? (cut scene will start playing during fade in)
	if( d_u32Flags & CUT_SCENE_FADE_IN )
	{
		RenderMgr__FadeIn( FADE_TO_BLACK_FAST , true );	// true == can be paused
		d_u32Flags &= ~CUT_SCENE_FADE_IN;				// only do this once
	}

	// there are a couple of things to do the first time the cut scene gets to the point where it executes interpreters 
	if( ( d_u32Flags & CUT_SCENE_HAS_EXECUTED) == 0 )
	{
		if( ( d_u32Flags & CUT_SCENE_DISABLE_BLACK_BARS ) == 0 )
		{
			AddBlackCinemaBars();
		}
//OLD #ifdef OLD_CAMERA
//OLD		g_cameraSystem[ d_nCameraIndex ].EnableCameraShake( false );
//OLD #endif
		StartGeneralCamera();

		// send out an event to anyone who wants to know 
		g_messageDispatcher.SendMessageToAll( "CutSceneStarted" , NULL , INVALID_OBJECT_HANDLE );

		// audio volume
		g_audio.SetVolume( Audio::DIALOG , g_ttl->GetSoundVolume() * DIALOG_VOLUME_ADJUST );	// make sure we're at standard

		// mark that we've executed the first time
		d_u32Flags |= CUT_SCENE_HAS_EXECUTED;
	}

	// what timer should we use? for development, use the os timer to ensure that things are sync'd up to the
	// audio, since the cut scene may be running on a slow machine. on a cd release, use the game timer so
	// that we can pause the cut scene 
#ifdef CDROM
	float fDeltaSec = g_timer.GetFrameSec();
#else //CDROM
	float fDeltaSec = g_timer.GetOSFrameSec();
#endif //CDROM

	// use game time because video recording forces 30hz update rate
	#ifdef ALLOW_VIDEO_RECORD_CUTSCENE
	if( g_videoRecordCutScene )
	{
		fDeltaSec = 1.0f / 29.97f;	// match what avi-builder will want
	}
	#endif //ALLOW_VIDEO_RECORD_CUTSCENE

	// update timer. don't want interpreters to execute past end of cut scene 
	d_fCutSceneTimer += fDeltaSec;
	if( d_fCutSceneTimer > d_fCutSceneLength )
	{
		d_fCutSceneTimer = d_fCutSceneLength;
	}

	// no interpreter has been executed yet this frame. do this to support parent chains
	CCutSceneInterpreter *pInterpreter = d_pInterpreterHead;
	while( pInterpreter )
	{
		pInterpreter->ClearFlag( CCutSceneInterpreter::EXECUTED );
		pInterpreter = pInterpreter->Next();
	}

	// process cut scene data
	pInterpreter = d_pInterpreterHead;
	while( pInterpreter )
	{
		if( pInterpreter->IsFlagSet( CCutSceneInterpreter::EXECUTED ) == false )
		{
			pInterpreter->Execute( d_fCutSceneTimer );
		}
		pInterpreter = pInterpreter->Next();
	}

	// done with cut scene? 
	if( d_fCutSceneTimer >= d_fCutSceneLength )
	{
		// maybe fade out first-- cut scene will continue to render
		if( d_u32Flags & CUT_SCENE_FADE_OUT )
		{
			RenderMgr__FadeOut( FADE_TO_BLACK_FAST , true );	// true == can be paused
			d_u32Flags &= ~CUT_SCENE_FADE_OUT;					// only do this once
			d_u32Flags &= ~CUT_SCENE_WAIT_FOR_FADE_OUT;		// don't return from top of Update-- we still want to execute interpreters

			// actors should stay in last frame during fade out. there may be extra frames in the .kfm 
			// that the cut scene artist never sees since they are past the end of the cut scene in maya.
			// so make it as wysiwyg as possible
			CCutSceneInterpreter *pInterpreter = d_pInterpreterHead;
			while( pInterpreter )
			{
				if( ( pInterpreter->Type() == CCutSceneInterpreter::ACTOR ) && (pInterpreter->GetActor() ) )
				{
					pInterpreter->GetActor()->StopMotion( (char * )NULL );
				}
				pInterpreter = pInterpreter->Next();
			}
		}

		// fading out?
		if( RenderMgr__IsFadingOut() )
		{
			return;
		}

		// time to stop the cut scene
		StopCutScene( true );
		return;
	}
}


// this callback gets called when the audio stream actually starts (when you queue an audio stream, it still has to seek
//to it, etc, before it starts playing) 
//** static function  **
void CCutSceneMgr::DialogStartedCallback( void *context , CommonStreamInterface *stream )
{
	// get to the manager context
	CCutSceneMgr *pCutSceneMgr = (CCutSceneMgr *)context;

	// just clear the bit that says we're waiting, and then Update will start the cut scene playing. this syncs
	// the audio and the cut scene render-- they must start at the same time 
	ASSERT( pCutSceneMgr->d_u32Flags & CUT_SCENE_WAIT_FOR_AUDIO_START );
	pCutSceneMgr->d_u32Flags &= ~CUT_SCENE_WAIT_FOR_AUDIO_START;
}
