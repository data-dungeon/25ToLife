/***************************************************************************/
// Sound and SoundInstance classes
/***************************************************************************/
#ifndef SOUND_H
#define SOUND_H

#include "platform/BaseType.h"
#include "Audio/Win32/sdriver.h"
#include "container/dllist.h"
#ifdef DIRECTX_PC
#include "EAXHelper.h"
#endif

// Our master!
class SoundDriver;
class SoundData;


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

	// Basic control functions (will return false if SOUNDCAPS flag is not set)

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	bool SetVolume(float volume);
	bool SetVolumeDirect(float volume);
	inline float GetVolume();

	// Set attenuation in dB, from -100db to 0db.
	// Note that this changes the volume...just like SetVolume so don't call both
	bool SetAttenuationdB(float dB);

	// Set the wet/dry mix.
	bool SetWetDry(float wet, float dry) { return false; }
	float GetDry() { return 1.0f; }
	float GetWet() { return 0.0f; }

	// Panning [-1..1] (-1.0 is hard left, 1.0 is hard right)
	bool SetPan(float pan);
	float GetPan();

	// Volume and pan combined
	bool SetVolumeAndPan(float volume, float pan);

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
	unsigned int PlayTime();

	// Get at our parent sound
	inline Sound *Parent()
		{ return sound; }

	#ifdef DIRECTX_PC
	EAX::BUFFER				EAXBufferInfo;
	// sometimes we don't want EAX to control sound placement
	bool					usingEAX; 
	#endif

protected:
	// Someone else creates and manages us
	SoundInstance(Sound *parentSound, LPDIRECTSOUNDBUFFER soundBuffer);
	SoundInstance(SoundData *parentSound, LPDIRECTSOUNDBUFFER soundBuffer);

	// Pointer to our sound data
	union
	{
		Sound		  				*sound;
		SoundData  				*soundData;
	};
	LPDIRECTSOUNDBUFFER 	buffer;			// direct sound buffer

	// Track our state
	bool						paused;
	bool						looped;
	float						volume;
	float						pan;
	float						pitchShift;
	int						frequency;

	// User data (for driver stuff)
	void						*userData;

	// Global overrides
	static float masterVolume;
	static float masterPitch;
	static float masterPan;

	// our friend creates us
	friend class Sound;
	friend class SoundStreamWin32;

public:
	// These are limited access, don't touch!
	~SoundInstance();
	int GetPlayPosition();

	// For the doubly linked list stuff
	SoundInstance 			*prev;
	SoundInstance 			*next;
};

// We are the container for sound data
class SoundData
{
public:
	// The driver must create the sound, but it can be deleted directly
	virtual ~SoundData();

	// Lock the entire sound buffer (returns NULL on error)
	void *Lock();
	bool Unlock(void *bufferPointer)
		{ return Unlock(bufferPointer, size); }

	// Lock a certain sized region (from the start)
	void *Lock(int bytesToLock);
	bool Unlock(void *bufferPointer, int bytesWritten);

	// Circular lock
	bool Lock(int offset, int bytesToLock, void **ptr1, int *size1, void **ptr2, int *size2);
	bool Unlock(void *ptr1, int written1, void *ptr2, int written2);

	// Get at our attr
	SoundAttr &Attr()
		{ return attr; }

	// What is the maximum/minimum pitch you can set?
	float GetMaxPitch();
	float GetMinPitch();

protected:
	// You cannot call this, only the sound driver can
	SoundData(SoundDriverServices &driverServices, LPDIRECTSOUNDBUFFER theBuffer, int bufferSize, SoundAttr &bufferCaps);

	SoundDriverServices		&driver;		// sound driver services class
	LPDIRECTSOUNDBUFFER 		buffer;		// direct sound buffer
	int							size;			// buffer size
	SoundAttr					attr;       // our buffer attr
};

// We are a sound that can have multiple simultaneous instances
class Sound : public SoundData
{
public:
	// The driver must create the sound, but it can be deleted directly
	~Sound() {}

	// Create/free sound instances
	SoundInstance *NewInstance(const char *name);
	void FreeInstance(SoundInstance *instance);

	// Pause/Unpause this sound instances
	bool Pause();
	bool Unpause();

private:
  // our constructor
	Sound(SoundDriverServices &driverServices, LPDIRECTSOUNDBUFFER theBuffer, int bufferSize, SoundAttr &bufferAttr) :
		SoundData(driverServices, theBuffer, bufferSize, bufferAttr) {}

	// Maintain a list of our instances
	DoublyLinkedList<SoundInstance>	instances;

	// Our friends, mostly for the constructor
	friend class SoundDriverWin32;
};

#endif
