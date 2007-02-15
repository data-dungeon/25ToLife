/////////////////////////////////////////////////////////////////////////////
// Multistream wrapper for SPU and file managment
/////////////////////////////////////////////////////////////////////////////
#include "LayersPCH.h"

#include "Layers/mstream/mstream.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SPUChannel::SPUChannel()
{
	m_channel = SOUNDCHANNEL_INVALID;
	m_name[0] = '\0';
	m_at4 = 0;
	m_locked = false;
}

/////////////////////////////////////////////////////////////////////////////
// Trigger the voice on off
/////////////////////////////////////////////////////////////////////////////
bool SPUChannel::KeyOn(
SFXID id,
int volumeL,
int volumeR,
int pitch)
{
	// Are we not supposed to talk to the IOP?
	if (MultiStream::Suspended())
		return false;

	// Sanity check
	ASSERT(m_channel != SOUNDCHANNEL_INVALID);
	return (SOUND_PlaySFX(id, m_channel, volumeL, volumeR, pitch, 0, 0) >= 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool SPUChannel::KeyOff()
{
	// Are we not supposed to talk to the IOP?
	if (MultiStream::Suspended())
		return false;

	ASSERT(m_channel != SOUNDCHANNEL_INVALID);
	return SOUND_StopSound(m_channel) >= 0;
}

/////////////////////////////////////////////////////////////////////////////
// check if things are still playing
/////////////////////////////////////////////////////////////////////////////
bool SPUChannel::IsPlaying()
{
	ASSERT(m_channel != SOUNDCHANNEL_INVALID);

	// Get the status of the key
	int status = SOUND_GetKeyStatus(m_channel);
	ASSERT(status >= 0);

	// Weeee status!
	return (status != 0);
}

/////////////////////////////////////////////////////////////////////////////
// Set the pitch
/////////////////////////////////////////////////////////////////////////////
bool SPUChannel::SetPitch(
int pitch)
{
	// Are we not supposed to talk to the IOP?
	if (MultiStream::Suspended())
		return false;

	ASSERT(m_channel != SOUNDCHANNEL_INVALID);

	return (SOUND_SetChannelPitch(m_channel, pitch) >= 0);
}

/////////////////////////////////////////////////////////////////////////////
// Set the volume
/////////////////////////////////////////////////////////////////////////////
bool SPUChannel::SetVolume(
int left,
int right,
bool smooth)
{
	// Are we not supposed to talk to the IOP?
	if (MultiStream::Suspended())
		return false;

	ASSERT(m_channel != SOUNDCHANNEL_INVALID);

	int ret;
	if (smooth)
		ret = SOUND_SetChannelVolumeSmooth(m_channel, left, right);
	else
		ret = SOUND_SetChannelVolume(m_channel, left, right);

	return (ret >= 0);
}

/////////////////////////////////////////////////////////////////////////////
// Access the mixer settings
/////////////////////////////////////////////////////////////////////////////
bool SPUChannel::SetWetDry(
bool wet,
bool dry)
{
	// Are we not supposed to talk to the IOP?
	if (MultiStream::Suspended())
		return false;

	ASSERT(m_channel != SOUNDCHANNEL_INVALID);

	// Apply it
	if (wet)
		SOUND_SetChannelEffectOn(m_channel);
	else
		SOUND_SetChannelEffectOff(m_channel);

	// Apply it
	if (dry)
		SOUND_SetChannelOutputOn(m_channel);
	else
		SOUND_SetChannelOutputOff(m_channel);

	return true;
}


