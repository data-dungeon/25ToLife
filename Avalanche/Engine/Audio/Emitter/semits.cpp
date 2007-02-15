/***************************************************************************/
// 3D Sound Emitter that works with sound names instead of handles.
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// set up
/***************************************************************************/
SoundEmitter3DString::SoundEmitter3DString(
const char *name,
SoundDriver &soundDriver,
SoundGroup &soundGroup) :
SoundEmitter3D(name, soundDriver, soundGroup)
{
}

/***************************************************************************/
/***************************************************************************/
SoundEmitter3DString::~SoundEmitter3DString()
{
	KillAll();
}

/***************************************************************************/
// This cue will return NULL if the sound is already playing
/***************************************************************************/
SoundHandle SoundEmitter3DString::Cue(
const char *soundName)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);

	// If this sound is already out there...can't play it again
	if (channel)
		return NULL;

	// Give them the normal cue
	return SoundEmitter::Cue(soundName);
}

/***************************************************************************/
// After you cue/play the sound, GIVE ME THE HANDLE
/***************************************************************************/
void SoundEmitter3DString::PassHandle(
SoundHandle handle)
{
	// Nothing to see here
	if (!handle)
		return;

	// If the sound is looped keep it
	if (!SoundEmitter::Looped(handle))
		delete handle;

	// We now have the handle stored away in the
	// channel if it is looped.  Since the
	// only way to kill it is though Kill(name)
	// we will release the handle there
}

/***************************************************************************/
// Stop a playing sound (can restart with play)
/***************************************************************************/
bool SoundEmitter3DString::Stop(
const char *soundName)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);
	if (!channel)
		return false;

	// Do the operation
	channel->Stop();
	return true;
}

/***************************************************************************/
// Kill the sound
/***************************************************************************/
bool SoundEmitter3DString::Kill(
const char *soundName)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);
	if (!channel)
		return false;

	// Does this handle have a channel?
	SoundHandle handle = channel->GetHandle();

	// Kill the channel if it is still around
	channel->Kill();

	// Delete the handle
	delete handle;

	return true;
}

/***************************************************************************/
// Is this sound still active (playing)
/***************************************************************************/
bool SoundEmitter3DString::Active(
const char *soundName)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);

	// It's playing if the channel is still held
	return (channel != NULL);
}

/***************************************************************************/
// Is this sound looped?
/***************************************************************************/
bool SoundEmitter3DString::Looped(
const char *soundName)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);
	if (!channel)
		return false;

	// Do the operation
	return channel->Looped();
}

/***************************************************************************/
// Change the volume of the voice [0..1]
/***************************************************************************/
bool SoundEmitter3DString::SetVolume(
const char *soundName,
float volume)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);
	if (!channel)
		return false;

	// Do the operation
	channel->SetVolume(volume);
	return true;
}

/***************************************************************************/
// Change the pitch of the sound in semitones
/***************************************************************************/
bool SoundEmitter3DString::SetPitch(
const char *soundName,
float pitchShift)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);
	if (!channel)
		return false;

	// Do the operation
	channel->SetPitch(pitchShift);
	return true;

}

/***************************************************************************/
// Set the ambience [0..1], returns false if sound has expired
/***************************************************************************/
bool SoundEmitter3DString::SetAmbience(
const char *soundName,
float ambience)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);
	if (!channel)
		return false;

	// Do the operation
	channel->SetAmbience(ambience);
	return true;
}

/***************************************************************************/
// Set the presence [0..1], returns false if sound has expired
/***************************************************************************/
bool SoundEmitter3DString::SetPresence(
const char *soundName,
float presence)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveName(soundName);
	if (!channel)
		return false;

	// Do the operation
	channel->SetPresence(presence);
	return true;
}

/***************************************************************************/
// Resolve a sound name into a handle,
// not fool proof (as multiple sounds could have
// the same name.)
/***************************************************************************/
SoundHandle SoundEmitter3DString::GetHandle(
const char *soundName)
{
	SoundChannel3D *channel = ResolveName(soundName);
	if (!channel)
		return NULL;

	return channel->GetHandle();
}

/***************************************************************************/
// Kill all channels
/***************************************************************************/
void SoundEmitter3DString::KillAll()
{
	// Oh shit, we are in charge of all looped handles
	// clean them up
	SoundChannel *channel = group.FindFirst(this);
	while (channel)
	{
		// Does this sound have a handle?
		SoundHandle handle = channel->GetHandle();

		// Kill
		channel->Kill();

		// kill the handle
		delete handle;

		// next!
		channel = group.FindNext(this, channel);
	}
}

/***************************************************************************/
// Resolve and upcast
/***************************************************************************/
SoundChannel3D *SoundEmitter3DString::ResolveName(
const char *soundName)
{
	SoundChannel *channel = group.FindFirst(this);
	while (channel)
	{
		// Only check channels that are not dead.
		if (!channel->Dead())
		{
			// Is this the one?
			if (strcmpi(channel->SoundName(), soundName) == 0)
			{
				ASSERT(channel->Type() == SoundChannel::CHANNEL3D);
				return (SoundChannel3D *)channel;
			}
		}

		// next!
		channel = group.FindNext(this, channel);
	}

	return NULL;
}


