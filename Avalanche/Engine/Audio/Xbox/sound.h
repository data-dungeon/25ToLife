/***************************************************************************/
// Sound and SoundInstance classes
/***************************************************************************/
#ifndef SOUND_H
#define SOUND_H

#include "platform/BaseType.h"
#include "container/dllist.h"
#include "Audio/Xbox/sdriver.h"

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
	static void SetMasterPanFlip(bool flipLeftRight);
	static bool GetMasterPanFlip();
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

	// Set the wet/dry mix.
	bool SetWetDry(float newWet, float newDry);
	float GetDry()
		{ return dry; }
	float GetWet()
		{ return wet; }

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

private:
	// Someone else creates and manages us
	SoundInstance(Sound *parentSound, IDirectSoundBuffer8 *soundBuffer);

	// internals
	SoundDriverServices	&driver;
	Sound		  				*sound;
	IDirectSoundBuffer8	*buffer;			// direct sound buffer
	void						*userData;		// User data

	// Track our state
	bool						paused;
	bool						looped;
	float						volume;
	float						pan;
	float						pitchShift;
	int						frequency;
	float						wet, dry;

	// Apply mixbins
	bool ApplyMixBins();

	// Global overrides
	static float masterVolume;
	static float masterPanFlip;
	static float masterPitch;
	static float masterPan;

	// our friend creates us
	friend class Sound;
	friend class SoundStream;

public:
	// These are limited access, don't touch!
	~SoundInstance();

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

	// Get at our attr
	SoundAttr &Attr()
		{ return attr; }

	// What is the maximum/minimum pitch you can set?
	float GetMaxPitch();
	float GetMinPitch();

	// What is our (approximate) play time (in ms)
	unsigned int PlayTime()
		{ return playTime; }

	SoundDriverServices &Driver()
		{ return driver; }

protected:
	// You cannot call this, only the sound driver can
	SoundData(SoundDriverServices &driverServices,
					void *bankData, int bankSize,
					int dataOffset, int dataSize,
					WAVEFORMATEX &waveFormat);

	SoundDriverServices		&driver;		// sound driver services class
	WAVEFORMATEX				wfx;			// The format from the input
	SoundAttr					attr;       // our buffer attr
	unsigned int				playTime;	// approximate play time (in ms)

	void							*data;		// actual data
	unsigned int				size;			// buffer size
	unsigned int				playOffset;	// play offset
	unsigned int				playLength;	// play size
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
	Sound(SoundDriverServices &driverServices,
			void *bankData, int bankSize,
			int dataOffset, int dataSize,
			WAVEFORMATEX &waveFormat) :
		SoundData(driverServices, bankData, bankSize, dataOffset, dataSize, waveFormat) {}

	// Maintain a list of our instances
	DoublyLinkedList<SoundInstance>	instances;

	// Our friends, mostly for the constructor
	friend class SoundDriverXbox;
};

#endif
