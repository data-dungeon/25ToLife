/***************************************************************************/
// Channel management & support
/***************************************************************************/
#include "Audio/AudioPCH.h"

// Define this for debugging
#define DEBUG_SFX

/***************************************************************************/
// The initialize
/***************************************************************************/
void HWChannel::Initialize(
SOUNDCHANNEL soundChannel)
{
	channel = soundChannel;
	memset(name, '\0', sizeof(name));
	locked = false;

	hwKeyOn = false;
	hwVolL = hwVolR = 0;
	hwPitch = 0;
	hwWet = hwDry = true;

	changed = 0;

	at4 = 0;
}

/***************************************************************************/
// Lock/Unlock
/***************************************************************************/
void HWChannel::Lock(
const char *channelName)
{
	ASSERT(!locked);

	locked = true;
	strncpy(name, channelName, sizeof(name) - 1);
}

/***************************************************************************/
/***************************************************************************/
void HWChannel::Unlock()
{
	ASSERT(locked);
	ASSERT(!hwKeyOn);

	if (hwKeyOn)
		SOUND_StopSound(channel);

	locked = false;
}

/***************************************************************************/
// Trigger the voice on off
/***************************************************************************/
bool HWChannel::KeyOn(
SFXID id,
int volumeL,
int volumeR,
int pitch)
{
	ASSERT(locked);

	// Save the state
	sfxID = id;
	wantSmooth = false;
	wantVolL = volumeL;
	wantVolR = volumeR;
	wantPitch = pitch;
	changed |= (CHANGED_KEYON | CHANGED_VOLUME | CHANGED_PITCH);

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool HWChannel::KeyOff(
SOUNDCHANNEL channel)
{
	ASSERT(locked);

	changed |= (CHANGED_KEYOFF);
	return true;
}

/***************************************************************************/
/***************************************************************************/
bool HWChannel::IsPlaying(
SOUNDCHANNEL channel)
{
	ASSERT(locked);

	// Get the status of the key
	int status = SOUND_GetKeyStatus(channel);
	ASSERT(status >= 0);

	// Weeee status!
	return (status != 0);
}

/***************************************************************************/
// Set the pitch
/***************************************************************************/
bool HWChannel::SetPitch(
SOUNDCHANNEL channel,
int pitch)
{
	ASSERT(locked);

	// Set it
	wnatPitch = pitch;
	changed |= CHANGED_PITCH;

	return true;
}

/***************************************************************************/
// Set the volume
/***************************************************************************/
bool HWChannel::SetVolume(
SOUNDCHANNEL channel,
int left,
int right,
bool smooth)
{
	ASSERT(locked);

	// Set it
	wantVolL = left;
	wantVolR = right;
	wantSmooth = smooth;

	changed |= CHANGED_VOLUME;

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool HWChannel::SetWetDry(
SOUNDCHANNEL channel,
bool wet,
bool dry)
{
	ASSERT(locked);

	// Set it
	wantWet = wet;
	wantDry = dry;

	changed |= CHANGED_WETDRY;

	return true;
}

/***************************************************************************/
// Update a channel's state
/***************************************************************************/
void HWChannel::Update()
{
	// Anything to do here?
	if (!locked)
		return;

	// Key off is the highest priority
	if (changed & CHANGED_KEYOFF)
	{
#ifdef DEBUG_SFX
		printf("KEYOFF channel %d (%s)\n", channel, name);
#endif
		if (hwKeyOn)
		{
			SOUND_StopSound(channel);
			hwKeyOn = false;
		}

		// Clear the whole set of flags, this channel is dead
		changed = 0;
	}

	// Do the key on
	if (changed & CHANGED_KEYON)
	{
#ifdef DEBUG_SFX
		printf("KEYON channel %d (%s) volume %d/%d pitch %d\n", channel, name, wantVolL, wantVolR, wantPitch);
#endif

		int ret = SOUND_PlaySFX(sfxID, channel, wantVolL, wantVolR, wantPitch, 0, 0);
		if (ret >= 0)
		{
			hwKeyOn = true;
			hwVolL = wantVolL;
			hwVolR = wantVolR;
			hwPitch = wantPitch;
		}

		changed &= ~(CHANGED_KEYON | CHANGED_VOLUME | CHANGED_PITCH);
	}

	// Handle wet/dry
	if (changed & CHANGED_WETDRY)
	{
		// Apply it
		if (wantWet != hwWet)
		{
#ifdef DEBUG_SFX
			printf("Setting channel %d (%s) wet to %d\n", channel, name, (int)wantWet);
#endif
			if (wantWet)
				SOUND_SetChannelEffectOn(channel);
			else
				SOUND_SetChannelEffectOff(channel);
			hwWet = wantWet;
		}

		// Apply it
		if (wantDry != hwDry)
		{
#ifdef DEBUG_SFX
			printf("Setting channel %d (%s) dry to %d\n", channel, name, (int)wantDry);
#endif
			if (wantDry)
				SOUND_SetChannelOutputOn(channel);
			else
				SOUND_SetChannelOutputOff(channel);
			hwDry = wantDry;
		}

		// Update the flags
		changed &= ~CHANGED_WETDRY;
	}

	// Do volume
	if (changed & CHANGED_VOLUME)
	{
		if (wantVolL != hwVolL || hwVolR != wantVolR)
		{
#ifdef DEBUG_SFX
			printf("Setting channel %d (%s) volume to %d/%d\n", channel, name, wantVolL, wantVolR);
#endif
			if (smooth)
				SOUND_SetChannelVolumeSmooth(channel, wantVolL, wantVolR);
			else
				SOUND_SetChannelVolume(channel, wantVolL, wantVolR);

			hwVolL = wantVolL;
			hwVolR = wantVolR;
		}

		changed &= ~CHANGED_VOLUME;
	}

	// Do pitch
	if (changed & CHANGED_PITCH)
	{
		if (wantPitch != hwPitch)
		{
#ifdef DEBUG_SFX
			printf("Setting channel %d (%s) pitch to %d\n", channel, name, wantPitch);
#endif
			SOUND_SetChannelPitch(channel, wantPitch);

			hwPitch = wantPitch;
		}

		changed &= ~CHANGED_PITCH;
	}

	// Make sure we applied all changes
	ASSERT(changed == 0);
}

