///////////////////////////////////////////////////////////////////////////////
//
//  CActorSoundEmitter.h
//
//  CActor-centric sound emitter.
//
//  Nate Robins, March 2003
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CACTORSOUNDEMITTER_H
#define CACTORSOUNDEMITTER_H

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include "Math/Vector.h"
//#include "Audio/AudioPCH.h"
#include "Audio/audiolib.h"
#include "container/Darray.h"


///////////////////////////////////////////////////////////////////////////////
// Classes
///////////////////////////////////////////////////////////////////////////////

class CActor;
class SoundEmitter3D;

class CActorSoundEmitter
{
public:
	CActorSoundEmitter();
	~CActorSoundEmitter();

	// initialize/create the sound emitter - must be called before any actions
	// can be performed on the CActorSoundEmitter.
	void Initialize( CActor* actor, const char* groupName );

	// Are we initialized already?
	inline bool Initialized()
		{ return (d_emitter != NULL); }

	// destroy the sound emitter - should be called when sound effects are no
	// longer needed (such as when the actor dies)
	void Terminate();

	// play a sound (at the actors position).
	void Play( const char* soundEffect );

	// play a sound (at the actors position plus a relative offset).
	void PlayAtPos( const char* soundEffect, const Vector3& offset );

	// play a sound (at the bone specified in the actor).
	void PlayAtBone( const char* soundEffect, const char* boneName );

	// stop a sound.
	void Stop( const char* soundEffect );

	// change the volume of a sound (volume in [0,1] range).
	void ChangeVolume( const char* soundEffect, float volume );

	// change the pitch of a sound (pitch in semi-tones).
	void ChangePitch( const char* soundEffect, float pitch );

	// returns true if the effect is currently playing, false otherwise.
	bool IsPlaying( const char* soundEffect );

	// play the voice (at the actors mouth bone).  the voice differs from a
	// normal sound effect in that there can be only one single voice effect
	// going at any one time.  if an attempt to play another voice effect
	// happens while a voice effect is active, the old effect is clipped and
	// the new one is started.
	void PlayVoice( const char* soundEffect );

	// stop the voice.
	void StopVoice();

	// is the voice playing?
	bool IsVoicePlaying();

private:
	CActor* d_actor;
	SoundEmitter3DString* d_emitter;
	SoundHandle d_voice;
	DynamicArray<SoundHandle> d_handles;
};

#endif // CACTORSOUNDEMITTER_H
