///////////////////////////////////////////////////////////////////////////////
//
//  CActorSoundEmitter.cpp
//
//  CActor-centric sound emitter.
//
//  Nate Robins, March 2003
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include "stage/CActorSoundEmitter.h"

#include "stage/StagePCH.h"
#include "Audio/audiosys.h"
#include "EngineHelper/sfx.h"


///////////////////////////////////////////////////////////////////////////////
// Public
///////////////////////////////////////////////////////////////////////////////

CActorSoundEmitter::CActorSoundEmitter()
{
	d_actor = 0;
	d_emitter = 0;
	d_voice = 0;
}

///////////////////////////////////////////////////////////////////////////////

CActorSoundEmitter::~CActorSoundEmitter()
{
	Terminate();
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::Initialize( CActor* actor, const char* groupName )
{
	ASSERT( actor );

	d_actor = actor;
	const char* name = actor->Instance()->Name();

	// only initialize the emitter once!
	if ( d_emitter == 0 )
	{
		d_emitter = g_audio.Create3DStringEmitter( name, groupName );
		ASSERT( d_emitter != 0 );
	}

	// always watch the actors transform (this way, all positions are relative
	// to the actor).
	d_emitter->WatchTransform( &( d_actor->GetBodyToWorld() ), &(d_actor->GetBodyInWorld()) );

	// always watch the actors velocity (this is a bit of a simplification - for
	// example, if a sound is played at the tip of the fingers of the actor and
	// their arm is swinging, the ideal velocity would be that of the finger
	// tips, but hopefully this will be good enough).
	d_emitter->WatchEmitterVelocity( &(d_actor->GetVelocityInWorld()) );
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::Terminate()
{
	( ( SoundEmitter* )d_emitter )->Kill( d_voice );

	if ( d_emitter )
	{
		g_audio.DestroyEmitter( d_emitter, true );
	}

	d_emitter = 0;
	d_actor = 0;
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::Play( const char* soundEffect )
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	Vector3 zero( 0, 0, 0 );
	PlayAtPos( soundEffect, zero );
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::PlayAtPos( const char* soundEffect, const Vector3& offset )
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	SoundHandle handle = d_emitter->Cue( soundEffect );
	if ( handle == 0 )
	{
		TRACE( "CActorSoundEmitter::PlayAtPos() : missing effect: %s.\n", soundEffect );
	}

	d_emitter->SetPosition( handle, offset );

	d_emitter->Play( handle );

	d_emitter->PassHandle( handle );
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::PlayAtBone( const char* soundEffect, const char* boneName )
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	Vector3 offset( 0, 0, 0 );

	BoneID boneId;
	if ( d_actor->GetBone( ( char* )boneName, boneId ) )
	{
		offset = d_actor->GetStartPosition( boneId );
	}
	else
	{
		TRACE( "CActorSoundEmitter::PlayAtBone() : missing bone: %s.\n", boneName );
	}

	PlayAtPos( soundEffect, offset );
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::Stop( const char* soundEffect )
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	d_emitter->Kill( soundEffect );
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::ChangeVolume( const char* soundEffect, float volume )
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	d_emitter->SetVolume( soundEffect, volume );
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::ChangePitch( const char* soundEffect, float pitch )
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	d_emitter->SetPitch( soundEffect, pitch );
}

///////////////////////////////////////////////////////////////////////////////

bool CActorSoundEmitter::IsPlaying( const char* soundEffect )
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	return d_emitter->Active( soundEffect );
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::PlayVoice( const char* soundEffect )
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	if ( d_voice && ! ( ( SoundEmitter* )d_emitter )->Active( d_voice ) )
	{
		( ( SoundEmitter* )d_emitter )->Kill( d_voice );
	}
	else if ( d_voice )
	{
		TRACE( "CActorSoundEmitter::PlayVoice() : effect dropped (voice already playing): %s.\n", soundEffect );
		return;
	}

	SoundHandle handle = d_emitter->Cue( soundEffect );
	if ( handle == 0 )
	{
		TRACE( "CActorSoundEmitter::PlayVoice() : missing effect: %s.\n", soundEffect );
	}

	Vector3 offset( 0, 0, 0 );

	BoneID boneId;
	if ( d_actor->GetBone( "head", boneId ) )
	{
		offset = d_actor->GetStartPosition( boneId );
	}
	else
	{
		TRACE( "CActorSoundEmitter::PlayVoice() : missing head bone.\n" );
	}

	d_emitter->SetPosition( handle, offset );

	if ( ! d_emitter->Play( handle ) )
	{
		TRACE( "CActorSoundEmitter::PlayVoice() : voice effect dropped (play failed): %s.\n", soundEffect );
	}

	d_voice = handle;
}

///////////////////////////////////////////////////////////////////////////////

void CActorSoundEmitter::StopVoice()
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	( ( SoundEmitter* )d_emitter )->Kill( d_voice );
}

///////////////////////////////////////////////////////////////////////////////

bool CActorSoundEmitter::IsVoicePlaying()
{
	ASSERTS( d_emitter != 0, "Must call Initialize() before calling support routines" );
	ASSERTS( d_actor != 0, "Must call Initialize() before calling support routines" );

	return ( ( SoundEmitter* )d_emitter )->Active( d_voice );
}

///////////////////////////////////////////////////////////////////////////////

