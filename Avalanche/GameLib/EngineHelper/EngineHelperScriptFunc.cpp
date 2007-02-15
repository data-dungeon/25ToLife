//============================================================================
//=
//= EngineHelperScriptFunc.cpp -
//=    Script Interface routines to module "EngineHelper"
//=
//============================================================================

#include "EngineHelper/EngineHelperPCH.h"
#include "EngineHelper/timer.h"
#include "stage/cactor.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

//============================================================================

void Script_SfxInit( ScriptVM* vm )
{
	// Don't do anything here anymore, since it wastes RAM
}

//============================================================================

void Script_SfxPlay( ScriptVM* vm )
{
	char *name = vm->PopString();

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	CActorSoundEmitter &emitter = actor->Sfx();

	if ( name && name[ 0 ] )
	{
		if (!emitter.Initialized())
			emitter.Initialize( actor, Sfx::GetEnvironmentGroupName() );
		emitter.Play( name );
	}
}

//============================================================================

void Script_SfxStop( ScriptVM *vm )
{
	char *name = vm->PopString();

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	CActorSoundEmitter &emitter = actor->Sfx();

	if ( name && name[ 0 ] && emitter.Initialized() )
	{
		emitter.Stop( name );
	}
}

//============================================================================

void Script_SfxChangeVolume( ScriptVM *vm )
{
	float volume = vm->PopFloat();
	char *name = vm->PopString();

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	CActorSoundEmitter &emitter = actor->Sfx();

	if ( name && name[ 0 ] && emitter.Initialized() )
	{
		emitter.ChangeVolume( name, volume );
	}
}

//============================================================================

void Script_SfxIsPlaying( ScriptVM *vm )
{
	float volume = vm->PopFloat();
	char *name = vm->PopString();

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	CActorSoundEmitter &emitter = actor->Sfx();

	bool playing = false;
	if ( name && name[ 0 ] && emitter.Initialized() )
	{
		playing = emitter.IsPlaying( name );
	}

	vm->Push( (int)playing );
}

//============================================================================

void Script_DialogPlay( ScriptVM* vm )
{
	int preemt = vm->PopInt();
	char *name = vm->PopString();

	if ( !name || !name[ 0 ] )
		vm->Push( 0 );
	else
	{
		if (preemt && g_audio.dialog.Exists( name ))
			g_audio.dialog.Clear();

		bool ret = g_audio.dialog.Queue( name );
		vm->Push( (int)ret );
	}
}

//============================================================================

void Script_DialogStop( ScriptVM* vm )
{
	char *name = vm->PopString();
	bool ret = g_audio.dialog.Stop();
	vm->Push( (int)ret );
}

//============================================================================

void Script_DialogIsPlaying( ScriptVM* vm )
{
	bool playing = g_audio.dialog.Playing();
	vm->Push( (int)playing );
}

//============================================================================

void Script_StreamPlay( ScriptVM* vm )
{
	CActor *playAt = vm->PopActor();
	float volume = vm->PopFloat();
	char *sound = vm->PopString();

	CSIHandle *handle = NULL;

	// Uh, duh
	if (sound && sound[0] != '\0')
	{
		// Create the stream
		CommonStreamInterface *stream = g_audio.CreateStream3D(sound, false);
		if (stream)
		{
			// Get the actor
			if (!playAt)
				playAt = (CActor *)vm->GetOwner();
			ASSERT_PTR(playAt);
			ASSERT(playAt->IsA(CActor::ClassType()));

			// We made it!
			SoundStream3D *stream3D = stream->Stream3D();
			Vector3 pos = playAt->GetFocusInWorld();
			stream3D->SetPosition(pos);
			Vector3 vel = playAt->GetVelocityInWorld();
			stream3D->SetVelocity(vel);
			stream3D->SetVolume(volume);
			stream3D->Play();

			// Give them back the handle
			handle = stream->CreateHandle();
		}
	}

	vm->Push((int)handle);
}

//============================================================================

void Script_StreamPlayAtPosition( ScriptVM* vm )
{
	Vector3 pos = vm->PopVector3();
	float volume = vm->PopFloat();
	char *sound = vm->PopString();

	CSIHandle *handle = NULL;

	// Uh, duh
	if (sound && sound[0] != '\0')
	{
		// Create the stream
		CommonStreamInterface *stream = g_audio.CreateStream3D(sound, false);
		if (stream)
		{
			SoundStream3D *stream3D = stream->Stream3D();
			stream3D->SetPosition(pos);
			stream3D->SetVolume(volume);
			stream3D->Play();
			handle = stream->CreateHandle();
		}
	}

	vm->Push((int)handle);
}

//============================================================================

void Script_StreamStop( ScriptVM* vm )
{
	bool wasPlaying = false;

	CSIHandle *handle = (CSIHandle *)vm->PopInt();
	if (handle)
	{
		CommonStreamInterface *stream = handle->GetStream();
		if (stream)
		{
			wasPlaying = stream->IsPlaying();
			CSIHandle::Kill(handle);
		}
	}

	vm->Push(wasPlaying);
}

//============================================================================

void Script_StreamChangeVolume( ScriptVM* vm )
{
	float volume = vm->PopFloat();
	CSIHandle *handle = (CSIHandle *)vm->PopInt();
	bool result = false;

	if (handle)
	{
		CommonStreamInterface *stream = handle->GetStream();
		if (stream)
			result = stream->SetVolume(volume);
	}

	vm->Push(result);
}

//============================================================================

void Script_StreamUpdatePosition( ScriptVM* vm )
{
	CActor *playAt = vm->PopActor();
	CSIHandle *handle = (CSIHandle *)vm->PopInt();

	if (handle)
	{
		CommonStreamInterface *stream = handle->GetStream();
		if (stream)
		{
			// Get the actor
			if (!playAt)
				playAt = (CActor *)vm->GetOwner();
			ASSERT_PTR(playAt);
			ASSERT(playAt->IsA(CActor::ClassType()));

			// We made it!
			SoundStream3D *stream3D = stream->Stream3D();
			if (stream3D)
			{
				Vector3 pos = playAt->GetFocusInWorld();
				stream3D->SetPosition(pos);
				Vector3 vel = playAt->GetVelocityInWorld();
				stream3D->SetVelocity(vel);
			}
		}
	}
}

//============================================================================

void Script_StreamIsPlaying( ScriptVM* vm )
{
	CSIHandle *handle = (CSIHandle *)vm->PopInt();
	bool isPlaying = false;

	if (handle)
	{
		CommonStreamInterface *stream = handle->GetStream();
		if (stream)
			isPlaying = stream->IsPlaying();
	}

	vm->Push(isPlaying);
}

//============================================================================

void Script_StreamPlayAndForget( ScriptVM* vm )
{
	Vector3 pos = vm->PopVector3();
	float volume = vm->PopFloat();
	char *sound = vm->PopString();

	CommonStreamInterface *stream = NULL;

	// Uh, duh
	if (sound && sound[0] != '\0')
	{
		// Create the stream
		stream = g_audio.CreateStream3D(sound, false, true);
		if (stream)
		{
			SoundStream3D *stream3D = stream->Stream3D();
			stream3D->SetPosition(pos);
			stream3D->SetVolume(volume);
			stream3D->Play();
		}
	}

	vm->Push((int)(stream != NULL));
}

//============================================================================

void Script_GameTime( ScriptVM* vm )
{
	vm->Push( g_timer.GetStartSec() );
}

//============================================================================

void Script_GetFrameSec( ScriptVM* vm )
{
	vm->Push( g_timer.GetFrameSec() );
}

//============================================================================
// DEBUG FUNCTIONS
//============================================================================

#ifndef CONSUMER_BUILD
static void Script_DrawDebugLine( ScriptVM* vm )
{
	int		color = vm->PopInt();
	Vector3	Pt2 = vm->PopVector3();
	Vector3	Pt1 = vm->PopVector3();

	DrawUtility::Line(Pt1, Pt2, (DrawUtility::COLOR)color);
}
#endif
//============================================================================

#ifndef CONSUMER_BUILD
static void Script_QueueDebugLine( ScriptVM* vm )
{
	float lifetime = vm->PopFloat();
	int		color = vm->PopInt();
	Vector3	Pt2 = vm->PopVector3();
	Vector3	Pt1 = vm->PopVector3();

	DrawUtility::QueueLine(Pt1, Pt2, (DrawUtility::COLOR)color, lifetime);
}
#endif
//============================================================================

#ifndef CONSUMER_BUILD
static void Script_QueueDebugVector( ScriptVM* vm )
{
	float lifetime = vm->PopFloat();
	int		color = vm->PopInt();
	Vector3	Pt2 = vm->PopVector3();
	Vector3	Pt1 = vm->PopVector3();

	DrawUtility::QueueLine(Pt1, Pt1 + (Pt2.Normalized()), (DrawUtility::COLOR)color, lifetime);
}
#endif
//============================================================================

#ifndef CONSUMER_BUILD
static void Script_DrawDebugSphere( ScriptVM* vm )
{
	float		radius = vm->PopFloat();
	int		color = vm->PopInt();
	Vector3	Pt1 = vm->PopVector3();

	DrawUtility::Sphere(Pt1, (DrawUtility::COLOR)color, radius);
}
#endif
//============================================================================

#ifndef CONSUMER_BUILD
static void Script_DrawDebugPoint( ScriptVM* vm )
{
	int		color = vm->PopInt();
	Vector3	Pt1 = vm->PopVector3();

	DrawUtility::Point(Pt1, (DrawUtility::COLOR)color);
}
#endif
//============================================================================

#ifndef CONSUMER_BUILD
static void Script_QueueDebugPoint( ScriptVM* vm )
{
	float lifetime = vm->PopFloat();
	float radius = vm->PopFloat();
	int		color = vm->PopInt();
	Vector3	Pt1 = vm->PopVector3();

	DrawUtility::QueuePoint(Pt1, (DrawUtility::COLOR)color, radius, lifetime);
}
#endif
//============================================================================

void EngineHelper_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( SfxInit );
	SCRIPT_REGISTER( SfxPlay );
	SCRIPT_REGISTER( SfxStop );
	SCRIPT_REGISTER( SfxChangeVolume );
	SCRIPT_REGISTER( SfxIsPlaying );
	SCRIPT_REGISTER( DialogPlay );
	SCRIPT_REGISTER( DialogStop );
	SCRIPT_REGISTER( DialogIsPlaying );
	SCRIPT_REGISTER( StreamPlay );
	SCRIPT_REGISTER( StreamPlayAtPosition );
	SCRIPT_REGISTER( StreamStop );
	SCRIPT_REGISTER( StreamChangeVolume );
	SCRIPT_REGISTER( StreamUpdatePosition );
	SCRIPT_REGISTER( StreamIsPlaying );
	SCRIPT_REGISTER( StreamPlayAndForget );
	SCRIPT_REGISTER( GameTime );
	SCRIPT_REGISTER( GetFrameSec );

#ifndef CONSUMER_BUILD
	SCRIPT_REGISTER( DrawDebugLine );
	SCRIPT_REGISTER( DrawDebugSphere );
	SCRIPT_REGISTER( DrawDebugPoint );
	SCRIPT_REGISTER( QueueDebugLine );
	SCRIPT_REGISTER( QueueDebugVector );
	SCRIPT_REGISTER( QueueDebugPoint );
#endif
}

//============================================================================
