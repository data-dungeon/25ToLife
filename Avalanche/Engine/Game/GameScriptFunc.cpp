//============================================================================
//=
//= GameScriptFunc.cpp - 
//=    Script Interface routines to module "game"
//=
//============================================================================

#include "Game/GamePCH.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

#include "Effects/halo.h"

#include "path/PathCollection.h"

//============================================================================

static void Script_GetMotionInt( ScriptVM* vm )
{
	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	int motionIndex = vm->PopInt();
	const char *name = vm->PopString();

	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );

	int var = actor->GetMotionInt( name, motionIndex );
	vm->Push( var );
}

//============================================================================

static void Script_GetMotionFloat( ScriptVM* vm )
{
	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	int motionIndex = vm->PopInt();
	const char *name = vm->PopString();

	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );

	float var = actor->GetMotionFloat( name, motionIndex );
	vm->Push( var );
}

//============================================================================

static void Script_GetMotionString( ScriptVM* vm )
{
	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	int motionIndex = vm->PopInt();
	const char *name = vm->PopString();

	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );

	const char *var = actor->GetMotionString( name, motionIndex );
	vm->Push( (char *)var );
}

//============================================================================

static void Script_EnableMotion( ScriptVM* vm )
{
	CActor* actor				= (CActor*) vm->GetOwner();
	int enable              = vm->PopBool();
	int index					= vm->PopInt();

	if( enable )
		actor->EnableMotion( index );
	else
		actor->DisableMotion( index );
}

//============================================================================

static void Script_StartMotion( ScriptVM* vm )
{
	CActor* actor          = (CActor*) vm->GetOwner();
	int flags              = vm->PopInt();
	int startFrame         = vm->PopInt();
	const char* motionName = vm->PopString();

	actor->StartMotion( motionName, startFrame, flags );
}

//============================================================================

static void Script_StartInterpMotion( ScriptVM* vm )
{
	CActor* actor          = (CActor*) vm->GetOwner();
	int frames             = vm->PopInt();
	int flags              = vm->PopInt();
	int startFrame         = vm->PopInt();
	const char* motionName = vm->PopString();

	int index;
	switch ( flags & ( MOTNINF_FLAG_MOTION2 | MOTNINF_FLAG_MOTION3 | MOTNINF_FLAG_MOTION4 ) )
	{
		default:								ASSERT( false );		// unknown case-- can't combine bits, fall through to motion1
		case 0:								index = MOTION1;		break;
		case MOTNINF_FLAG_MOTION2:		index = MOTION2;		break;
		case MOTNINF_FLAG_MOTION3:		index = MOTION3;		break;
		case MOTNINF_FLAG_MOTION4:		index = MOTION4;		break;
	}

	actor->StartMotion( motionName, startFrame, flags );
	CMotion__StartInterpolation( actor->GetMotnInf( index ), (unsigned short) frames );
}

//============================================================================

static void Script_StopMotion( ScriptVM* vm )
{
	const char* motionName = vm->PopString();
	int motionIndex = vm->PopInt();
	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );
	CActor* actor = (CActor*) vm->GetOwner();
	actor->StopMotion( motionName, motionIndex );
}

//============================================================================

static void Script_MotionEnded( ScriptVM* vm )
{
	int motionIndex = vm->PopInt();
	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );
	CActor* actor   = (CActor*) vm->GetOwner();
	vm->Push( CMotion__IsMotionDone( actor->GetMotnInf( motionIndex ) ) );
}

//============================================================================

static void Script_InterpDone( ScriptVM* vm )
{
	int motionIndex = vm->PopInt();
	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );
	CActor* actor   = (CActor*) vm->GetOwner();
	vm->Push( CMotion__IsInterpDone( actor->GetMotnInf( motionIndex ) ) );
}

//============================================================================

static void Script_SetAction( ScriptVM* vm )
{
	int action = vm->PopInt();
	vm->SetAction( action );
}

//============================================================================

static void Script_AnimAction( ScriptVM* vm )
{
	int   motionIndex = vm->PopInt();
	CActor *actor = vm->PopActor();

	vm->Push( actor->AnimActionSecondary( motionIndex ) );
}

//============================================================================

static void Script_GetAction( ScriptVM* vm )
{
	int motionIndex = vm->PopInt();
	CActor* actor   = (CActor*) vm->GetOwner();
	int action = actor->AnimActionSecondary( motionIndex );
	vm->Push( action );
}

//============================================================================

static void Script_SetAnimationRate( ScriptVM* vm )
{
	CActor* actor   = (CActor*) vm->GetOwner();
	float rate      = vm->PopFloat();
	int   motionIndex = vm->PopInt();
	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	CMotion__SetAnimationRate( actor->GetMotnInf( motionIndex ), rate );
}

//============================================================================

static void Script_GetAnimationRate( ScriptVM* vm )
{
	CActor* actor   = (CActor*) vm->GetOwner();
	int   motionIndex = vm->PopInt();
	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	vm->Push( CMotion__GetAnimationRate( actor->GetMotnInf( motionIndex ) ) );
}

//============================================================================

static void Script_SetProceduralPosition( ScriptVM* vm )
{
	CActor* actor   = (CActor*) vm->GetOwner();
	float zeroToOne      = vm->PopFloat();
	int   motionIndex = vm->PopInt();
	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	ASSERT( zeroToOne >= 0.0f && zeroToOne <= 1.0f );
	CMotion__SetProceduralPosition( actor->GetMotnInf( motionIndex ), zeroToOne );
}

//============================================================================

static void Script_SyncAnimFrame( ScriptVM* vm )
{
	int motionIndex = vm->PopInt();
	CActor *dest = vm->PopActor();
	CActor *source = vm->PopActor();
	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );
	ASSERT_PTR( dest );
	ASSERT( dest->IsA( CActor::ClassType() ) );
	ASSERT_PTR( source );
	ASSERT( source->IsA( CActor::ClassType() ) );
	CMotion__SetCurrentTime( dest->GetMotnInf( motionIndex ), CMotion__GetCurrentTime( source->GetMotnInf( motionIndex ) ) );
}

//============================================================================

static void Script_ClearFlag( ScriptVM* vm )
{
	CActor* actor   = (CActor*) vm->GetOwner();
	int flag        = vm->PopInt();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	actor->SetActionFlags( actor->GetActionFlags() & ~flag );
}

//============================================================================

static void Script_SetFlag( ScriptVM* vm )
{
	CActor* actor   = (CActor*) vm->GetOwner();
	int flag        = vm->PopInt();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	actor->SetActionFlags( actor->GetActionFlags() | flag );
}

//============================================================================

static void Script_GetFlag( ScriptVM* vm )
{
	CActor* actor   = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	vm->Push( (int)actor->GetActionFlags() );
}

//============================================================================

static void Script_AnimFrame( ScriptVM* vm )
{
	int motionIndex = vm->PopInt();
	ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS );
	CActor* actor   = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	vm->Push( CMotion__GetCurrentFrame( actor->GetMotnInf( motionIndex ) ) );
}

//============================================================================

static void Script_ParmGetInt( ScriptVM* vm )
{
	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	const char* key = vm->PopString();
	vm->Push( actor->Parm().GetInt( key ) );
}

//============================================================================

static void Script_ParmGetFloat( ScriptVM* vm )
{
	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	const char* key = vm->PopString();
	vm->Push( actor->Parm().GetFloat( key ) );
}

//============================================================================

static void Script_ParmGetString( ScriptVM* vm )
{
	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	const char* key = vm->PopString();
	char* str = actor->Parm().GetString( key );
	if( str )	
		vm->Push( str );
	else
		vm->Push( "" );
}

//============================================================================

static void Script_ParmSetInt( ScriptVM* vm )
{
	int value = vm->PopInt();
	const char* key = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	char buf[ 32 ];
	sprintf( buf, "%d", value );

	actor->Parm().Set( key, buf );
}

//============================================================================

static void Script_ParmSetFloat( ScriptVM* vm )
{
	float value = vm->PopFloat();
	const char* key = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	char buf[ 32 ];
	sprintf( buf, "%f", value );

	actor->Parm().Set( key, buf );
}

//============================================================================

static void Script_ParmSetString( ScriptVM* vm )
{
	const char* value = vm->PopString();
	const char* key   = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->Parm().Set( key, value );
}

//============================================================================

static void Script_ParmSet( ScriptVM* vm )
{
	const char* initString = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->Parm().SetFromString( initString );
}

//============================================================================

// CreatePointLight( Vector3 pos, Vector3 color, float attenuation )
static void Script_LightCreate( ScriptVM* vm )
{
	float   attenuation = vm->PopFloat();
	float   colorB      = vm->PopFloat();
	float   colorG      = vm->PopFloat();
	float   colorR      = vm->PopFloat();
	Vector3 pos         = vm->PopVector3();

	Vector4 color;
	color.Set( colorR, colorG, colorB, 0.0f );

	CLightRecord* light = new CLightRecord;

	light->setColor( color );
	light->setType( CLightRecord::LIGHT_OMNI );
	light->setPosition( pos );
	light->setConstantAttenuation( 0.0f );
	light->setLinearAttenuation( 0.0f );
	light->setAttenuation( attenuation );

	g_lightSet.addDynamicLight( light );

	vm->Push( (int)light );
}

//============================================================================

static void Script_LightDestroy( ScriptVM* vm )
{
	CLightRecord* light = (CLightRecord*) vm->PopInt();
	if ( light )
	{
		g_lightSet.removeDynamicLight( light );
		//delete light;
	}
}

//============================================================================

static void Script_LightEnable( ScriptVM* vm )
{
	bool          enable = vm->PopBool();
	CLightRecord* light  = (CLightRecord*) vm->PopInt();
	if ( light )
	{
		light->enableLight( enable );
	}
}

//============================================================================

static void Script_LightBreak( ScriptVM* vm )
{
	int	damage		= vm->PopInt();
	Vector3Packed pos = vm->PopVector3( );
	CLightRecord* pLight  = (CLightRecord*) vm->PopInt();
	if ( pLight )
	{
		if ( damage > 3 )
		{
			pLight->enableLight( false );
		}
		else
		{
			CLightEffects *pLightEffects = new CLightEffects( pLight );
			ASSERTS( pLightEffects, "unable to create light effects" );
			if( pLightEffects )
			{
				if( pLightEffects->addEffects( ) )
				{
					if ( damage < 2 )
					{
						float f = g_random.InRange( 0.1f, 1.0f );
						pLightEffects->enableFlicker( true, (u32) ( 15.0f * f ) );
						pLightEffects->enableOverBrighten( true, 0.5f, 3 );
						pLightEffects->enableGlowReduction( true, 8 );
						//pLightEffects->enableRespawn( true, 15.0f );
						pLightEffects->setEffectsPos( pos );
					}
					else if ( damage < 3 )
					{
						float f = g_random.InRange( 1.0f, 2.0f );
						pLightEffects->enableFlicker( true, (u32) ( 15.0f * f ) );
						pLightEffects->enableOverBrighten( true, 0.5f, 3 );
						pLightEffects->enableGlowReduction( true, 8 );
						//pLightEffects->enableRespawn( true, 30.0f );
						pLightEffects->setEffectsPos( pos );
					}
					else
					{
						pLightEffects->enableFlicker( false, (u32) ( 999999.0f ) );
						//pLightEffects->enableRespawn( false, 100.0f );
					}
				}
				else
				{
					delete pLightEffects;
				}
			}
		}
	}
}

//============================================================================

static void Script_LightFindLightByName( ScriptVM* vm )
{
	const char		*name = vm->PopString( );
	CLightRecord	*light = NULL;

	if( name )
	{
		light = g_lightSet.findLightByName( name );
	}
	vm->Push( ( int )light );
}

//============================================================================

static void Script_HaloEnable( ScriptVM* vm )
{
	bool enable = vm->PopBool();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	if ( actor )
		g_halo.FindAndEnable( actor->GetAABB(), enable );
}

//============================================================================

static void Script_InstanceHide( ScriptVM* vm )
{
	bool hide = vm->PopBool();
	CInstance* instance = (CInstance*) vm->PopInt();
	if ( instance )
	{
		instance->Hide( hide );
	}
}

//============================================================================

static void Script_ActorSetInstanceVisRegionStatic( ScriptVM* vm )
{
	bool useWorld = vm->PopBool();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	SceneMgr::SetInstanceVisRegionStatic( actor->Instance(), useWorld );
}

//============================================================================

static void Script_ActorSetInstanceVisRegionDynamic( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	SceneMgr::SetInstanceVisRegionDynamic( actor->Instance() );
}

//============================================================================

static void Script_ParticleEffectCreate( ScriptVM* vm )
{
	Vector3 vel      = vm->PopVector3();
	Vector3 pos      = vm->PopVector3();
	const char* name = vm->PopString();

   ParticleMgr__CreateEffect( name, &pos, &vel, NULL, NULL, 0.0f );
	// JG: vel seems messed up
   //ParticleMgr__CreateEffect( name, &pos, NULL, NULL, NULL, 0.0f );
}

static void Script_ParticleEffectCreateNoVel( ScriptVM* vm )
{
	Vector3 pos      = vm->PopVector3();
	const char* name = vm->PopString();

   ParticleMgr__CreateEffect( name, &pos, NULL, NULL, NULL, 0.0f );
}

//============================================================================

static void Script_ParticleEmitterCreate( ScriptVM* vm )
{
	Vector3 pos      = vm->PopVector3();
	const char* name = vm->PopString();
	t_Handle emitterHandle;

	emitterHandle = ParticleEmitter__CreateFromRecord( name, &pos, NULL, NULL, false, NULL, true );
	if( emitterHandle != INVALID_HANDLE)
		ParticleEmitter__AdvancesWhileManagerLocked( emitterHandle );
	vm->Push( ( int )emitterHandle );
}

//============================================================================

static void Script_ParticleEmitterKill( ScriptVM* vm )
{
	t_Handle handle = vm->PopInt();
	ParticleEmitter__Kill( handle );
}

//============================================================================

static void Script_ParticleEmitterActivate( ScriptVM* vm )
{
	t_Handle emitterHandle = vm->PopInt();
	
	ParticleEmitter__Activate( emitterHandle );
}

//============================================================================

static void Script_ParticleEmitterDeactivate( ScriptVM* vm )
{
	t_Handle emitterHandle = vm->PopInt();
	
	ParticleEmitter__Deactivate( emitterHandle );
}

//============================================================================

static void Script_ParticleEmitterSetLOD( ScriptVM* vm )
{
	float   lod2RadiusOverride = vm->PopFloat();
	float   lod1RadiusOverride = vm->PopFloat();
	t_Handle emitterHandle = vm->PopInt();
	
	ParticleEmitter__SetLOD( emitterHandle, lod1RadiusOverride, lod2RadiusOverride );
}

//============================================================================

static void Script_ParticleEmitterSetPath( ScriptVM* vm )
{
	const char* pathName		= vm->PopString();
	t_Handle emitterHandle	= vm->PopInt();
	
	SplinePath* path = PathCollection::Collection()->Named( pathName );
	if ( path )
		ParticleEmitter__SetParticlePath( emitterHandle, path );
	else
		ASSERTF(false, ( "world emitter can't find path '%s'", pathName ));
}

//============================================================================

static void Script_FadeOut( ScriptVM* vm )
{
	RenderMgr__FadeOut( FADE_TO_BLACK_FAST );
}

//============================================================================

static void Script_FadeIn( ScriptVM* vm )
{
	RenderMgr__FadeIn( FADE_TO_BLACK_FAST );
}

//============================================================================

static void Script_IsFading( ScriptVM* vm )
{
	vm->Push( RenderMgr__IsFading() );
}

//============================================================================

void Game_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( GetMotionInt      );
	SCRIPT_REGISTER( GetMotionFloat    );
	SCRIPT_REGISTER( GetMotionString   );
	SCRIPT_REGISTER( StartMotion       );
	SCRIPT_REGISTER( EnableMotion       );
	SCRIPT_REGISTER( StartInterpMotion );
	SCRIPT_REGISTER( StopMotion        );
	SCRIPT_REGISTER( MotionEnded       );
	SCRIPT_REGISTER( InterpDone        );
	SCRIPT_REGISTER( SetAction         );
	SCRIPT_REGISTER( GetAction         );
	SCRIPT_REGISTER( AnimAction        );
	SCRIPT_REGISTER( SetAnimationRate  );
	SCRIPT_REGISTER( GetAnimationRate  );
	SCRIPT_REGISTER( SetProceduralPosition  );
	SCRIPT_REGISTER( SyncAnimFrame );
	SCRIPT_REGISTER( ClearFlag         );
	SCRIPT_REGISTER( SetFlag           );
	SCRIPT_REGISTER( GetFlag           );
	SCRIPT_REGISTER( AnimFrame         );
	SCRIPT_REGISTER( ParmGetInt        );
	SCRIPT_REGISTER( ParmGetFloat      );
	SCRIPT_REGISTER( ParmGetString     );
	SCRIPT_REGISTER( ParmSetInt        );
	SCRIPT_REGISTER( ParmSetFloat      );
	SCRIPT_REGISTER( ParmSetString     );
	SCRIPT_REGISTER( ParmSet           );
	SCRIPT_REGISTER( LightCreate       );
	SCRIPT_REGISTER( LightDestroy      );
	SCRIPT_REGISTER( LightEnable       );
	SCRIPT_REGISTER( LightBreak        );
	SCRIPT_REGISTER( LightFindLightByName );
	SCRIPT_REGISTER( HaloEnable        );
	SCRIPT_REGISTER( InstanceHide      );
	SCRIPT_REGISTER( ActorSetInstanceVisRegionStatic );
	SCRIPT_REGISTER( ActorSetInstanceVisRegionDynamic );
	SCRIPT_REGISTER( ParticleEffectCreate );
	SCRIPT_REGISTER( ParticleEffectCreateNoVel );
	SCRIPT_REGISTER( ParticleEmitterCreate );
	SCRIPT_REGISTER( ParticleEmitterKill );
	SCRIPT_REGISTER( ParticleEmitterActivate );
	SCRIPT_REGISTER( ParticleEmitterDeactivate );
	SCRIPT_REGISTER( ParticleEmitterSetLOD );
	SCRIPT_REGISTER( ParticleEmitterSetPath );
	SCRIPT_REGISTER( FadeOut );
	SCRIPT_REGISTER( FadeIn );
	SCRIPT_REGISTER( IsFading );
}

//============================================================================
