/***************************************************************************/
// SoundEmiiter
//
// An emmiter provides the mechanisms for allocating sounds and playing
// sounds from the sound driver.
/***************************************************************************/
#ifndef SEMITTER_H
#define SEMITTER_H

#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Audio/group/shandle.h"
#include "Audio/group/sgroup.h"
#include "Audio/fader.h"

// Sound bank
class SoundInstance;
class SoundDriver;

// Abstract base an emitter
class SoundEmitter
{
public:
	// What is our name
	const char *Name();

	// We really need RTTI here, but this is as good as it gets
	typedef enum
	{
		EMITTER2D,
		EMITTER3D
	} TYPE;
	virtual TYPE Type() = 0;

	// What volume do we want for this emitter [0..1]
	void SetVolume(float emitterVolume);

	// return the volume you just set
	float GetRawVolume();

	// Connect a fader to this emitter
	void SetFader(Fader *fader);
	Fader *GetFader();

	// Get the actual volume (including the fader)
	float GetVolume();

	// Set a pitch offset for all sounds in semitones
	void SetPitch(float emitterPitch);
	float GetPitch();

	// Cue up a sound for play.  Returns a handle that you can free
	// (so long as it is not looped)
	// You MUST free the handle (just call delete on it!)
	// If NULL is returned, the sound is not loaded
	// Note: This allocates and holds a voice
	virtual SoundHandle Cue(const char *name);

	// Change the volume of the voice [0..1]
	bool SetVolume(SoundHandle handle, float volume);

	// Change the pitch of the sound in semitones
	bool SetPitch(SoundHandle handle, float pitchShift);

	// Set wet/dry
	bool SetWetDry(SoundHandle handle, float wet, float dry);

	// Now that you have cue'd a sound, and setup it's play
	// parameters you can start the sound playing
	bool Play(SoundHandle handle);

	// Stop a playing sound (can restart with play)
	bool Stop(SoundHandle handle);

  	// Is this sound still active (playing)
	bool Active(SoundHandle handle);

	// Is this sound looped?
	bool Looped(SoundHandle handle);

	// Kill this sound and revoke the handle
	void Kill(SoundHandle &handle);

	// Kill all channels
	virtual void KillAll();

	// Mark the emitter for death
	// (will allow all non-looped sounds to finish)
	virtual bool MarkForDeath();
	bool MarkedForDeath();

	// Is this emitter ready to die?
	bool ReadyToDie();

	// Should this emitter die on a soft flush?
	void KillOnSoftFlush(bool killOnSoftFlush = true);
	bool DieOnSoftFlush();

	// Is this the emitter's group
	bool IsGroup(SoundGroup* testGroup);

	// Run every frame or so
	virtual void Daemon();

protected:
	// Only the sound system can create us
	SoundEmitter(const char *emitterName,
						SoundDriver &soundDriver,
						SoundGroup &soundGroup);

	// Unique name
	char					name[32];

	// Our parent sound system & driver & group
	SoundDriver			&driver;
	SoundGroup			&group;

	// Our overall state
	enum STATE
	{
		ACTIVE,
		MARKEDFORDEATH,
		DEAD
	};
	STATE					state;

	// Allow death on soft flush
	bool					dieOnSoftFlush;

	// Some global parameters for the whole group
	Fader					*fader;
	float					volume;
	float					pitch;

	// The sound system creates us
	friend class AudioSystem;

	// A utility to resolve a handle to a channel
	SoundChannel *ResolveHandle(SoundHandle handle);

public:
	// This has to be public for list management
	SoundEmitter		*prev, *next;
	virtual ~SoundEmitter();
};

#endif
