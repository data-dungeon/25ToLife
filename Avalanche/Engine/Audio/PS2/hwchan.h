/***************************************************************************/
// Channel management & support
/***************************************************************************/
#ifndef HWCHAN_H
#define HWCHAN_H

#include "audio/ps2/streamirx.h"

class HWChannel
{
public:
	// Nothing in here
	inline HWChannel() {}

	// The initialize
	void Initialize(SOUNDCHANNEL soundChannel);

	// Lock/Unlock
	void Lock(const char *channelName);
	void Unlock();
	inline bool Locked()
		{ return locked; }

	// Our channel
	inline SOUNDCHANNEL Channel()
		{ return channel; }

	// Name of the channel
	inline const char *Name()
		{  return name; }

	// Trigger the voice on off
	bool KeyOn(SFXID id, int volumeL, int volumeR, int pitch);
	bool KeyOff();

	// check if things are still playing
	bool IsPlaying();

	// Set the pitch
	bool SetPitch(int pitch);

	// Set the volume
	bool SetVolume(int left, int right, bool smooth);

	// Access the mixer settings
	bool SetWetDry(bool wet, bool dry);

	// Update (actually apply changes)
	void Update();

	// Some debugging
	inline void ClearAt4()
		{ at4 = 0; }
	inline void IncrementAt4()
		{ ++at4; }
	inline int At4()
		{ return at4; }

private:
	// Basic info
	SOUNDCHANNEL	channel;
	char				name[32];
	bool 				locked;

	// Hardware states
	bool				hwKeyOn;
	int				hwVolL, hwVolR;
	int				hwPitch;
	bool				hwWet, hwDry;

	// Requested states
	enum
	{
		CHANGED_KEYON = 0x01,
		CHANGED_KEYOFF = 0x02,
		CHANGED_VOLUME = 0x04,
		CHANGED_PITCH = 0x08,
		CHANGED_WETDRY = 0x10
	};
	int				changed;
	SFXID				sfxID;
	bool				wantSmooth;
	int				wantVolL, wantVolR;
	int				wantPitch;
	bool				wantWet, wantDry;

	// Debugging thingie
	int				at4;
};

#endif


