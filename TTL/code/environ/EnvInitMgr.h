///////////////////////////////////////////////////////////////////////////////
//
//  EnvInitMgr.h
//
//  This class provides an interface for "environmental" initializers.
//  An environmental initializer acts as a low-cost env actor that is used in 
//  the case where an env actor needs only initialization and nothing more (for
//  example, a world sound emitter, or world particle emitter).
//
//  Nate Robins, February 2003
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ENVINITMGR_H
#define ENVINITMGR_H

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"


///////////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////////

typedef void ( *EnvInitFunc )(  ParameterSet& parm );


///////////////////////////////////////////////////////////////////////////////
// Classes
///////////////////////////////////////////////////////////////////////////////

class EnvInitMgr 
{
private:
	EnvInitMgr();
	~EnvInitMgr();

public:
	static int GetTypeIndex( const char* name );
	static bool InitFromIndex( int index, ParameterSet& parm );
	static bool InitFromName( const char* name, ParameterSet& parm );

private:
	static CInstance* GetInstance();

	static int GetParameterAsInt( const char* parameterName );
	static float GetParameterAsFloat( const char* parameterName );
	static const char* GetParameterAsString( const char* parameterName );

public:
	// specialized initialization function per env init type.

	// Create a pool of water.
	static void Water( ParameterSet& parm );

	// Create a sound emitter.
	static void SoundEmitter( ParameterSet& parm );

	// Create a sound field
	static void SoundField( ParameterSet& parm );

	// Override the player start position.
	static void StartPos( ParameterSet& parm );

	// Create wind distortion
	static void GlobalWind( ParameterSet& parm );
	static void WindGust( ParameterSet& parm );
	static void VortexWind( ParameterSet& parm );

	// Create spawn boxes
	static void SpawnBox( ParameterSet& parm );

	// Create visual occluders
	static void Occluder( ParameterSet& parm );
};

///////////////////////////////////////////////////////////////////////////////

#endif  // ENVINITMGR_H
