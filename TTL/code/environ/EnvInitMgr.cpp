///////////////////////////////////////////////////////////////////////////////
//
//  EnvInitMgr.cpp
//
//  This class provides an interface for "environmental" initializers.
//  An environmental initializer acts as a low-cost env actor that is used in
//  the case where an env actor needs only initialization and nothing more (for
//  example, a world sound emitter, or world particle emitter).
//
//  Nate Robins, February 2003
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "EnvInitMgr.h"
#include "EngineHelper/sfx.h"
#include "Game/Database/SceneNode.h"
#include "Game/Managers/OccluderMgr.h"
#include "PlayerSpawnMgr.h"
#include "sim/TeamingModel.h"

///////////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////////

struct ts_EnvInitType
{
	char*       name;
	EnvInitFunc	func;
};


///////////////////////////////////////////////////////////////////////////////
// Static
///////////////////////////////////////////////////////////////////////////////

static ts_EnvInitType s_TypeList[] =
{
	{ "SoundEmitter", EnvInitMgr::SoundEmitter },	// obsolete (not in objattr.cfg)
	{ "SoundField",	EnvInitMgr::SoundField },		// used in objattr.cfg, but not in the class field so it can't get here... (?)
	{ "StartPos",     EnvInitMgr::StartPos },			// ok
	{ "Water",        EnvInitMgr::Water },				// can't get here?
	{ "GlobalWind",   EnvInitMgr::GlobalWind },		// ok
	{ "WindGust",     EnvInitMgr::WindGust },			// ok
	{ "VortexWind",   EnvInitMgr::VortexWind },		// ok
	{ "SpawnBox",	   EnvInitMgr::SpawnBox },
	{ "occluder",     EnvInitMgr::Occluder },			// ok
};

///////////////////////////////////////////////////////////////////////////////
// Public
///////////////////////////////////////////////////////////////////////////////

/* check if instance is something that just initializes something without
creating an environmental actor */

int EnvInitMgr::GetTypeIndex( const char* name )
{
	int i;
	int numInits = sizeof ( s_TypeList ) / sizeof ( s_TypeList[ 0 ] );

	// try to find this init by matching name to the whole list
	for ( i = 0 ; i < numInits ; i++ )
	{
		if ( stricmp( s_TypeList[ i ].name, name ) == 0 )
			return i;
	}
	return -1;
}

/* initialize an instance */

bool EnvInitMgr::InitFromIndex( int index, ParameterSet& parm )
{
	// make sure the index is valid
	if ( ( index < 0 ) ||
			( index >= ( sizeof ( s_TypeList ) / sizeof ( s_TypeList[ 0 ] ) ) ) )
	{
		ASSERT( false );
		return false;
	}

	// call the init function.
	ASSERT( s_TypeList[ index ].func );
	s_TypeList[ index ].func( parm );
	return true;
}

/* called from CEnvActorMgr::Load to check if current environmental initialization record is something that just
initializes something without creating an environmental actor. we use this to create sound and particle emitters
via the .oa file, instead of having to bake them into .dbl chunks. with the binary .env data, it's pretty efficient.
if not found in the list, entity is probably an environmental actor */

bool EnvInitMgr::InitFromName( const char* name, ParameterSet& parm )
{
	int i;
	int numInits = sizeof ( s_TypeList ) / sizeof ( s_TypeList[ 0 ] );

	// try to find this init by matching name to the whole list
	ts_EnvInitType* init = 0;
	for ( i = 0 ; i < numInits ; i++ )
	{
		ts_EnvInitType& t = s_TypeList[ i ];
		if ( stricmp( t.name, name ) == 0 )
		{
			init = &t;
			break;
		}
	}

	// if we didn't find the name, this entity might be a full-blown actor, so
	// bail out and let LoadFromTupperware() handle it.
	if ( init == 0 )
	{
		return false;
	}

	// call the init function.
	ASSERT( init->func );
	init->func( parm );

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Private
///////////////////////////////////////////////////////////////////////////////

#include "Effects/Water/Water.h"
#include "Effects/Water/WaterMgr.h"

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::Water( ParameterSet& parm )
{
    // find the instance
    const char *instanceName = parm.GetString( "instance" );
    if ( instanceName == NULL )
    {
        ASSERTS( instanceName, "Unable to find instance name for water" );
        return;
    }
    CInstance *instance = g_loadSOM.FindInstanceByName( instanceName );
    if ( instance == NULL )
    {
        ASSERTS( instance, "Unable to find instance for water" );
        return;
    }

    // get the parameters
    const char* name = instance->Name();
    const ts_BoundingBox& bounds = instance->BoundingBox();
    const char* varFile = parm.GetString( "varFile" );
    float wrap = parm.GetFloat( "wrap" );

    // setup the water defaults
    WaterParams wparams;
    WaterMgr::Defaults( wparams );

    // load the water var file
    if ( ! WaterMgr::LoadScript( g_gameEnvironment.WorldPath(), varFile, wparams ) )
    {
        ASSERTF( 0, ( "Unable to load water var file: %s", varFile ) );
        return;
    }

    // overwrite var file data with info from the instance
    wparams.pos.x( bounds.Min.x());
    wparams.pos.y( ( bounds.Max.y() + bounds.Min.y() ) / 2);
    wparams.pos.z( bounds.Min.z());
    wparams.cellSizeX = bounds.Max.x() - bounds.Min.x();
    wparams.cellSizeZ = bounds.Max.z() - bounds.Min.z();
    wparams.angle = Math::Deg2Rad( 0.0f );

    float scaleU = wparams.cellSizeX / wrap;		// divide by wrap: wrap = wrap texture every x meters.
    float scaleV = wparams.cellSizeZ / wrap;

    for ( int i = 0; i < MAX_LAYERS; i++ )
    {
        if ( strcmp( wparams.layers[ i ].mLayerType, "DISTORT" ) != 0 )
        {
            wparams.layers[ i ].mScaleU *= scaleU;
            wparams.layers[ i ].mScaleV *= scaleV;
        }
    }

    // create the pool
    WaterPool* pool = g_waterMgr.Create( wparams, name );
    ASSERT( pool != 0 );
}

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::SoundEmitter( ParameterSet& parm )
{
	/* find instance in world that lava water is derived from */

	const char* envName = parm.GetString( "instance" );
	if ( envName == NULL )
		return;
	CInstance* instance = g_loadSOM.FindInstanceByName( envName );
	if ( instance == NULL )
		return;

	/* get name of sound effect to emit */

#if 0
	const char* soundName = parm.GetString( "soundName" );

	float audibleDistance = parm.GetFloat( "audibleDistance" );

	int volumePercent = parm.GetInt( "soundVolume" );
	float volume = Math::Clamp( (float)volumePercent / 100.0f );

	float pitch = parm.GetFloat( "soundPitch" );

	int ambiencePercent = parm.GetInt( "soundAmbience" );
	float ambience = Math::Clamp( (float)ambiencePercent / 100.0f );

	int presencePercent = parm.GetInt( "soundPresence" );
	float presence = Math::Clamp( (float)presencePercent / 100.0f );

	float minTriggerTime = parm.GetFloat( "soundTriggerMin" );
  	float maxTriggerTime = parm.GetFloat( "soundTriggerMax" );
	int minTrigger = (int)(minTriggerTime / 1000.0f);
	int maxTrigger = (int)(maxTriggerTime / 1000.0f);
	if (minTrigger > maxTrigger)
	{
		minTrigger = 0;
		maxTrigger = 0;
	}

	SoundEmitterEnvironment &emitter = Sfx::GetEnvironmentEmitter();

	Vector3 origin = { 0, 0, 0 };
	Vector3 soundPos = origin * instance->mLocalToWorld;

	emitter.AddSource( soundName, soundPos, audibleDistance,
								minTrigger, maxTrigger,
								volume, pitch, ambience, presence );
#else
	const char* soundName = parm.GetString( "soundName" );

	float audibleDistance = parm.GetFloat( "distanceNoVolume" );

	float volume = parm.GetFloat( "soundVolume" );

	SoundEmitterEnvironment &emitter = Sfx::GetEnvironmentEmitter();

	Vector3 origin( 0, 0, 0 );
	Vector3 soundPos = origin * instance->Matrix();

	if (soundName && *soundName)
	{
#if 1
		TRACE("Obsolete sound emitter used %s\n", soundName);
#else
		emitter.AddSource( soundName, soundPos, audibleDistance, 0, 0, volume );
#endif
	} 
 #endif
}

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::SoundField( ParameterSet& parm )
{
	/* find instance in world that lava water is derived from */

	const char* envName = parm.GetString( "instance" );
	if ( envName == NULL )
		return;
	CInstance* instance = g_loadSOM.FindInstanceByName( envName );
	if ( instance == NULL )
		return;
}

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::StartPos( ParameterSet& parm )
{
	// grab a pointer to the instance
	const char *instanceName = parm.GetString( "instance" );
	if ( instanceName == 0 )
	{
		ASSERTS( instanceName, "Name of StartPos instance not found!" );
		return;
	}

	CInstance *instance = g_loadSOM.FindInstanceByName( instanceName );
	if ( instance == 0 )
	{
		ASSERTS( instance, "StartPos instance not found!" );
		return;
	}

	// override the simloop start position.
	Vector3 origin( 0, 0, 0 );
	Vector3 facing, forward( 0, 0, 1 );

	Vector3 startPos = origin * instance->Matrix();
	
	facing = instance->Matrix().GetRow2(  );
	facing.y( 0.0f );
	if( facing.LengthSquared() < 0.00001f )
		facing.Set( 0.0f, 0.0f, 1.0f );
	facing.Normalize();

	float heading = Math::ArcCos( facing.Dot( forward ) );
	if( facing.x() < 0 )
		heading = -heading;
	g_players.SetPlayerStartPos(0, startPos, heading);
}

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::GlobalWind( ParameterSet& parm )
{
	/* find instance in world that lava water is derived from */

	const char* envName = parm.GetString( "instance" );
	if ( envName == NULL )
		return;
	CInstance* instance = g_loadSOM.FindInstanceByName( envName );
	if ( instance == NULL )
		return;

	float fRandomness = parm.GetFloat( "strength" );
	float fStrength = parm.GetFloat( "strength" );
	float fDamping = parm.GetFloat( "damping" );
	float fLifetime = parm.GetFloat( "lifetime" );
	float fSinAngleStart = parm.GetFloat( "sinAngleStart" );
	float fSinAngleInc = parm.GetFloat( "sinAngleInc" );
	float fSinAngleMin = parm.GetFloat( "sinAngleMin" );
	float fSinAngleMax = parm.GetFloat( "sinAngleMax" );
	u16 flags1 = 0;
	u8 flags2 = 0;

	Vector3 origin;
	Vector3 facing( 0.0f, 0.0f, 1.0f );
	origin.Clear();
	instance->Matrix().PokeTranslation( origin );
	Vector3 vStrength = ( fStrength * facing ) * instance->Matrix();

	t_Handle Handle = DistortionMgr__AddGlobalWind(
		&vStrength,				// Vector3 *vStrength
		fRandomness,			// float Randomness
		fSinAngleStart,		// float SinAngleStart
		fSinAngleInc,			// float SinAngleInc
		fSinAngleMin,			// float SinMin
		fSinAngleMax,			// float SinMax
		fDamping,	  			// float Damping
		fLifetime,				// float Lifetime
		flags1,					// u16 u16DistortionFlags
		flags2);					// u8 u8Flags

	ASSERT(Handle != INVALID_HANDLE);	// if this hits, you may want to boost the init.maxDistortions setting in SetupGameInit
}

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::WindGust( ParameterSet& parm )
{
	/* find instance in world that lava water is derived from */

	const char* envName = parm.GetString( "instance" );
	if ( envName == NULL )
		return;
	CInstance* instance = g_loadSOM.FindInstanceByName( envName );
	if ( instance == NULL )
		return;

	float fVelocity = parm.GetFloat( "velocity" );
	float fStrength = parm.GetFloat( "strength" );
	float fRadius = parm.GetFloat( "radius" );
	float fDelay = parm.GetFloat( "delay" );
	float fLifetime = parm.GetFloat( "lifetime" );

	Vector3 origin;
	Vector3 facing( 0.0f, 0.0f, 1.0f );
	origin.Clear();
	Vector3 vPos = origin * instance->Matrix();

	instance->Matrix().PokeTranslation( origin );
	Vector3 vVelocity = ( fVelocity * facing ) * instance->Matrix();
	Vector3 vStrength = ( fStrength * facing ) * instance->Matrix();

	WindGustEmitters__Add(
		&vPos,					// Vector3 *pvPos
		&vVelocity,				// Vector3 *pvVel
		&vStrength,				// Vector3 *pvStrength
		fRadius,					// float fRadius
		fDelay,					// float fDelay
		fLifetime	 			// float fLifetime
		);
}

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::VortexWind( ParameterSet& parm )
{
	/* find instance in world that lava water is derived from */

	const char* envName = parm.GetString( "instance" );
	if ( envName == NULL )
		return;
	CInstance* instance = g_loadSOM.FindInstanceByName( envName );
	if ( instance == NULL )
		return;

	float fStartRadius = parm.GetFloat( "startRadius" );
	float fRange = parm.GetFloat( "range" );
	float fRadiusVelocity = parm.GetFloat( "radiusVelocity" );
	float fRadiusAcceleration = parm.GetFloat( "radiusAcceleration" );
	float fAmplitude = parm.GetFloat( "amplitude" );
	float fDamping = parm.GetFloat( "damping" );
	float fLifetime = parm.GetFloat( "lifetime" );
	u16 flags1 = 0;
	u8 flags2 = 0;

	Vector3 origin;
	origin.Clear();
	Vector3 vPos = origin * instance->Matrix();

	t_Handle Handle = DistortionMgr__AddVortexWind(
		&vPos,																		// Vector3 *vPos
		fStartRadius,			// float StartRadius
		fRange,					// float Range
		fRadiusVelocity,		// float RadiusVelocity
		fRadiusAcceleration,	// float RadiusAcceleration
		fAmplitude,				// float Amplitude
		fDamping,				// float Damping
		fLifetime,				// float Lifetime
		flags1,					// u16 u16DistortionFlags
		flags2);					// u8 u8Flags

	ASSERT(Handle != INVALID_HANDLE);	// if this hits, you may want to boost the init.maxDistortions setting in SetupGameInit
}

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::SpawnBox( ParameterSet& parm )
{
	// grab a pointer to the instance
	const char *instanceName = parm.GetString( "instance" );
	if ( instanceName == 0 )
	{
		ASSERTS( instanceName, "Name of StartPos instance not found!" );
		return;
	}

	CInstance *instance = g_loadSOM.FindInstanceByName( instanceName );
	if ( instance == 0 )
	{
		ASSERTS( instance, "SpawnBox instance not found!" );
		return;
	}

	// decide if spawn is only for a certain gametype
	int team = parm.GetInt( "team" );
	CTeamingModel::ERole role;
	if (team == 1)
		role = CTeamingModel::BLUE;
	else if (team == 2)
		role = CTeamingModel::RED;
	else
		role = CTeamingModel::AMBIENT;
	if (parm.GetInt("war_spawn") == 1)
		g_PlayerSpawnMgr.AddBox( instance, role, "war" );
	else if (parm.GetInt("raid_spawn") == 1)
		g_PlayerSpawnMgr.AddBox( instance, role, "raid" );
	else if (parm.GetInt("robbery_spawn") == 1)
		g_PlayerSpawnMgr.AddBox( instance, role, "robbery" );
	else if (parm.GetInt("tag_spawn") == 1)
		g_PlayerSpawnMgr.AddBox( instance, role, "tag" );
	else if (parm.GetInt("wipeout_spawn") == 1)
		g_PlayerSpawnMgr.AddBox( instance, role, "wipeout" );
	else
		g_PlayerSpawnMgr.AddBox( instance, role, NULL );
}

///////////////////////////////////////////////////////////////////////////////

void EnvInitMgr::Occluder( ParameterSet& parm )
{
	// get the instance
	const char* instanceName = parm.GetString( "instance" );
	ASSERT_PTR( instanceName );
	ASSERT( instanceName[ 0 ] );
	CInstance* instance = g_loadSOM.FindInstanceByName( instanceName );
	ASSERT_PTR( instance );
	ASSERT_PTR( instance->Model() );

	// add to scene mgr.
	SceneMgr::GetOccluderMgr().Add( COccluder( instance->Model()->BoundingBox.Min, instance->Model()->BoundingBox.Max, instance->Matrix() ) );
}

///////////////////////////////////////////////////////////////////////////////
