/***************************************************************************/
// Sound and SoundInstance classes
/***************************************************************************/
#ifndef SOUND_H
#define SOUND_H

#include "platform/BaseType.h"
#include "Audio/GCN/sdriver.h"
#include "container/dllist.h"
#include "Audio/GCN/dspadpcm.h"

#ifndef setHiLo
#define round8( val) ( ( ( val) + 7) & ~7)
#define round32( val) ( ( ( val) + 31) & ~31)

#define setHiLo( field, val) \
	field##Hi = (u16)( ( val) >> 16); \
	field##Lo = (u16)( ( val) & 0xFFFF);
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
	inline bool Rewind();

	// Pause/Unpause this sound
	bool Pause();
	bool Unpause();
	bool Paused();

	// Basic control functions

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	static const int MAXVOLUME;
	static const int DEFVOLUME;
	static const int MINVOLUME;
	bool SetVolume(float newVolume);
	inline float GetVolume();

	// Set attenuation in dB, from -100db to 0db.
	// Note that this changes the volume...just like SetVolume so don't call both
	bool SetAttenuationdB(float dB);

	// Set the wet/dry mix.
	bool SetWetDry( float wet, float dry);

	float GetDry( void)
	{
		return( m_bDry ? 1.0f : 0.0f);
	}

	float GetWet( void)
	{
		return( m_bWet ? 1.0f : 0.0f);
	}

	// Panning [-1..1] (-1.0 is hard left, 1.0 is hard right)
	static const int PANHARDLEFT;
	static const int PANHARDRIGHT;
	static const int PANCENTER;
	static const int SPANSURROUND;
	static const int SPANCENTER;
	static const int SPANFRONT;
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
	bool SetFrequency(int newFrequency);
	int GetFrequency();

	static inline int HectodBToGCNdB( int hectodB)
	{
		return( ( MINVOLUME * hectodB) / -10000);
	}

	// We are friendly, and store some user data
	void SetUserData(void *data)
		{ userData = data; }
	void *UserData()
		{ return(userData); }

	// What is our (approximate) play time (in ms)
	unsigned int PlayTime();

	// Get at our parent sound
	inline Sound *Parent()
		{ return(sound); }

private:
	// Someone else creates and manages us
	SoundInstance(Sound *parentSound, GCNVoice *pNewGCNVoice);
	SoundInstance(SoundData *parentSound, GCNVoice *pNewGCNVoice);

	// We store these locally
	float	volume,
			pan,
			pitchShift;

	int	frequency;

	bool	paused;

	// internals
	union
	{
		Sound			*sound;
		SoundData	*soundData;
	};

	void				*userData;		// User data

	GCNVoice 		*pGCNVoice;		// Our channel

	// Global overrides
	static float masterVolume;
	static float masterPitch;
	static float masterPan;

	// our friend creates us
	friend class Sound;

	bool		m_bWet,
				m_bDry;

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
	SoundAttr & Attr( void)
	{
		return( attr);
	}

	u32 AramAddr( void)
	{
		return myAramAddr;
	}

	// Get at the driver
	SoundDriverServices &Driver( void)
	{
		return( driver);
	}

protected:
	// You cannot call this, only the sound driver can
	SoundData( SoundDriverServices &driverServices, ADPCM_Hdr &apdmcHdr, u32 aramAddr, SoundAttr &bufferCaps);

	SoundDriverServices		&driver;		// sound driver services class
	SoundAttr					attr;       // our buffer attr

	u32							myAramAddr;

	AXPBADDR						myAXPBADDR;
	AXPBADPCM					myAXPBADPCM;
	AXPBADPCMLOOP				myAXPBADPCMLOOP;
};

// We are a sound that can have multiple simultaneous instances
class Sound : public SoundData
{
public:
	// The driver must create the sound, but it can be deleted directly
	~Sound() {}

	// Create/free sound instances
	SoundInstance *NewInstance( const char *name);
	void FreeInstance(SoundInstance *instance);

	// Pause/Unpause this sound instances
	bool Pause();
	bool Unpause();

private:
	// our constructor
	Sound(SoundDriverServices &driverServices, ADPCM_Hdr &adpcmHdr, u32 aramAddr, SoundAttr &bufferAttr) :
		SoundData(driverServices, adpcmHdr, aramAddr, bufferAttr) {}

	// Maintain a list of our instances
	DoublyLinkedList<SoundInstance>	instances;

	// Our friends, mostly for the constructor
	friend class SoundDriverGCN;
};

#endif
