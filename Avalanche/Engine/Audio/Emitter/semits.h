/***************************************************************************/
// 3D Sound Emitter that works with sound names instead of handles.
//
// Because everything is keyed by name, you cannot have more than one
// sound playing at any given time.
/***************************************************************************/
#ifndef SEMITS_H
#define SEMITS_H

#include "Audio/Emitter/semit3d.h"

// 3D Voice group
class SoundEmitter3DString : public SoundEmitter3D
{
public:
	// This cue will return NULL if the sound is already playing
	SoundHandle Cue(const char *name);

	// After you cue/play the sound, GIVE ME THE HANDLE
	void PassHandle(SoundHandle handle);

	 // Kill the sound
	bool Kill(const char *soundName);

	// Stop a playing sound (can restart with play)
	bool Stop(const char *soundName);

  	// Is this sound still active (playing)
	bool Active(const char *soundName);

	// Is this sound looped?
	bool Looped(const char *soundName);

	// Change the volume of the voice [0..1]
	bool SetVolume(const char *soundName, float volume);

	// Change the pitch of the sound in semitones
	bool SetPitch(const char *soundName, float pitchShift);

	// Set the ambience [0..1], returns false if sound has expired
	bool SetAmbience(const char *soundName, float ambience);

	// Set the presence [0..1], returns false if sound has expired
	bool SetPresence(const char *soundName, float presence);

	// Resolve a sound name into a handle,
	// not fool proof.
	SoundHandle GetHandle(const char *soundName);

	// Kill all channels
	void KillAll();

private:
	// set up
	SoundEmitter3DString(const char *name,
								SoundDriver &soundDriver,
								SoundGroup &soundGroup);

	// Resolve and upcast
	SoundChannel3D *ResolveName(const char *soundName);

	friend class Audio;

public:
	// Has to be public for list management
	~SoundEmitter3DString();
};

#endif
