/////////////////////////////////////////////////////////////////////////////
// A hardware wrapper around an SPU channel
/////////////////////////////////////////////////////////////////////////////
#ifndef SPUCHAN_H
#define SPUCHAN_H

#include "container/pool.h"

// Forward declare us
class MultiStream;

// Soundchannel is just a core-independent id
typedef uint SOUNDCHANNEL;
#define MAX_SOUNDCHANNELS	    	48
#define SOUNDCHANNEL_INVALID		0xffff

// An SFX id is a holder for an SPU addr & tracking state in the IOP
typedef uint SFXID;
#define SFXID_INVALID				0xFFFFFFFF

// A hardware wrapper around an SPU channel
class SPUChannel
{
public:
	// Some info
	inline const char *Name()
		{ return m_name; }

	// Trigger the voice on off
	bool KeyOn(SFXID id, int volumeL, int volumeR, int pitch);
	bool KeyOff();

	// check if things are still playing
	bool IsPlaying();

	// Set the pitch
	bool SetPitch(int pitch);

	// Set the volume
	bool SetVolume(int left, int right, bool smooth = true);

	// Access the mixer settings
	bool SetWetDry(bool wet, bool dry);

	// Get the hardware channel
	SOUNDCHANNEL GetChannel()
		{ return	m_channel; }

private:
	SOUNDCHANNEL 	m_channel;
	char				m_name[32];
	int				m_at4;
	bool 				m_locked;

	// Only the mutlistream class can create us
	SPUChannel();
	friend class MultiStream;
};

#endif


