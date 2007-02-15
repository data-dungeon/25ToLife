/***************************************************************************/
// Sound and SoundInstance classes
/***************************************************************************/
#ifndef SOUND_H
#define SOUND_H

#include "platform/BaseType.h"
#include "container/dllist.h"
#include "container/fpool.h"
#include "Math/MathClass.h"
#include "Audio/PS2/sdriver.h"

// Max simultaneous sound instances
#define MAX_SOUND_INSTANCES		128

// Our master!
class Sound;

// A sound instance is just the play data for a set of sound data
class SoundInstance
{
public:
	// Setup global defaults for sounds
	static void SetMasterVolume(float volume);
	static float GetMasterVolume();
	static void SetMasterPitch(float masterPitch);
	static float GetMasterPitch();
	static void SetOutputMode(SoundDriver::OUTPUTMODE mode);

	// Base playing functions
	bool Play(bool loop = false);
	bool Stop(bool rewind = true);
	bool IsPlaying();

	// Rewind the sound (probably no effect if playing)
	bool Rewind();

	// Pause/Unpause this sound
	bool Pause();
	bool Unpause();
	bool Paused();

	// Basic control functions

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	bool SetVolume(float volume);
	inline float GetVolume();

	// Set attenuation in dB, from -100db to 0db.
	// Note that this changes the volume...just like SetVolume so don't call both
	bool SetAttenuationdB(float dB);

	// Set the wet/dry mix.  Beware, on the PS2 anything over 0.0
	// will be 100% on
	bool SetWetDry(float wet, float dry);
	float GetDry();
	float GetWet();

	// Volume & Pan combined
	bool SetVolumeAndPan(float volume, float pan);

	// Panning [-1..1] (-1.0 is hard left, 1.0 is hard right)
	bool SetPan(float pan);
	float GetPan();

	// Set the pitch shift, given in semitones.  0.0 plays the sound at
	// the original pitch (see semitone.h for helpful hints)
	bool SetPitch(float semiTones);
	float GetPitch();

	// Set the playback frequency (in Hz) manually
	static const int MAXFREQUENCY;
	static const int MINFREQUENCY;
	static const int ORIGINALFREQUENCY;
	bool SetFrequency(int frequency);
	int GetFrequency();

	// We are friendly, and store some user data
	void SetUserData(void *data)
		{ userData = data; }
	void *UserData()
		{ return userData; }

	// What is our (approximate) play time (in ms)
	uint PlayTime();

	// Get at our parent sound
	inline Sound *Parent()
		{ ASSERT(sound); return sound; }

	// public for stream use
	static void ComputeVolume(float volume, float pan, int &volumeL, int &volumeR);
	static inline int FrequencyToPS2Pitch(int frequency);

private:
	// Someone else creates and manages us
	SoundInstance(Sound *parentSound, SPUChannel &onChannel);

	// Our current state
	bool		paused;
	bool		looped;
	float 	volume, pan, pitchShift;
	int		frequency;
	bool		wet, dry;

	// Volume management
	bool		smoothVolume;
	bool		SetVoiceVolume(int volumeL, int volumeR);

	// What is our SFX ID?
	inline SFXID ID();

	// internals
	Sound		 		*sound;
	void				*userData;		// User data
	SPUChannel		&channel;			// Our channel

	// Global overrides
	static float masterVolume;
	static float masterPitch;
	static float masterPan;

	// our friend creates us
	friend class Sound;
	friend class SoundDriverPS2;

public:
	// These are limited access, don't touch!
	~SoundInstance();

	// For the doubly linked list stuff
	SoundInstance 			*prev;
	SoundInstance 			*next;
};

// We are a sound that can have multiple simultaneous instances
class Sound
{
public:
	// The driver must create the sound, but it can be deleted directly
	~Sound();

	// Get at our attr
	SoundAttr &Attr()
		{ return attr; }

	// What is the maximum/minimum pitch you can set?
	float GetMaxPitch();
	float GetMinPitch();

	// What is our SFX ID?
	SFXID	ID()
		{ return id; }

	// Create/free sound instances
	SoundInstance *NewInstance(const char *name);
	void FreeInstance(SoundInstance *instance);

	// Pause/Unpause this sound instances
	bool Pause();
	bool Unpause();

	// Call this from driver autoreclaim to
	// really free up the instance
	static void AutoReclaimed(SoundInstance *instance);

private:
	// our constructor
	Sound(SoundDriverPS2 &driver, SFXID sfxID, int sfxSize, SoundAttr &bufferAttr);

	// Our attributes
	SoundDriverPS2							&driver;		// sound driver
	SFXID										id;
	SoundAttr								attr;       // our buffer attr
	int										size;

	// Maintain a list of our instances
	DoublyLinkedList<SoundInstance>	instances;
	static FixedPool<SoundInstance, MAX_SOUND_INSTANCES>	instancePool;

	// Our friends, mostly for the constructor
	friend class SoundDriverPS2;
};

/***************************************************************************/
/***************************************************************************/
inline int SoundInstance::FrequencyToPS2Pitch(
int frequency)
{
	int ps2Pitch = (frequency * 4096) / 48000;
	return Math::Clamp(ps2Pitch, 0, 0x3fff);
}

/***************************************************************************/
// What is our SFX ID?
/***************************************************************************/
inline SFXID SoundInstance::ID()
{
	return Parent()->ID();
}

#endif
