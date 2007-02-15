/***************************************************************************/
// SoundEmiiter
//
// An emmiter provides the mechanisms for allocating sounds and playing
// sounds from the sound driver.
/***************************************************************************/
#include "Audio/AudioPCH.h"

// Override all sounds
const char *g_overrideSound = NULL;

/***************************************************************************/
/***************************************************************************/
SoundEmitter::SoundEmitter(
const char *emitterName,
SoundDriver &soundDriver,
SoundGroup &soundGroup) :
driver(soundDriver),
group(soundGroup)
{
	// Save our name
	strncpy(name, emitterName, sizeof(name) - 1);
	name[sizeof(name) - 1] = '\0';

	// Our initial state is active
	state = ACTIVE;

	// Set the overall factors
	fader = NULL;
	volume = 1.0f;
	pitch = 0.0f;

	// Don't die on soft flush be default
	dieOnSoftFlush = false;
}

/***************************************************************************/
/***************************************************************************/
SoundEmitter::~SoundEmitter()
{
	KillAll();
}

/***************************************************************************/
// What is our name
/***************************************************************************/
const char *SoundEmitter::Name()
{
	return name;
}

/***************************************************************************/
// What volume do we want for this group [0..1]
/***************************************************************************/
void SoundEmitter::SetVolume(
float emitterVolume)
{
	volume = Math::Clamp(emitterVolume);
}

/***************************************************************************/
/***************************************************************************/
float SoundEmitter::GetRawVolume()
{
	return volume;
}

/***************************************************************************/
// Connect a fader to this emitter
/***************************************************************************/
void SoundEmitter::SetFader(
Fader *newFader)
{
	fader = newFader;
}

/***************************************************************************/
/***************************************************************************/
Fader *SoundEmitter::GetFader()
{
	return fader;
}

/***************************************************************************/
/***************************************************************************/
float SoundEmitter::GetVolume()
{
	if (!fader)
		return volume;
	else
		return fader->MixVolume() * volume;
}

/***************************************************************************/
// Set a pitch offset for all sounds in semitones
/***************************************************************************/
void SoundEmitter::SetPitch(float emitterPitch)
{
	pitch = emitterPitch;
}

/***************************************************************************/
/***************************************************************************/
float SoundEmitter::GetPitch()
{
	return pitch;
}

/***************************************************************************/
// Cue up a sound for play.  Returns a handle that you can free
// (so long as it is not looped)
// You MUST free the handle
// If NULL is returned, the sound is not loaded
/***************************************************************************/
SoundHandle SoundEmitter::Cue(
const char *name)
{
	// Look up the sound
	SoundBank *bank = NULL;
	if (g_overrideSound)
		name = g_overrideSound;
	SoundDef *soundDef = driver.FindSoundByName(name, &bank);
	if (!soundDef)
	{
#if defined(FOOD)
#if defined(PS2) && !defined(CDROM)
		TRACE("Trying to play sound \"%s\" and it does not exist.\n", name);
		ASSERT(false);
#else
		ASSERTF(false, ("Trying to play sound \"%s\" and it does not exist.\n", name));
#endif
#endif
		return NULL;
	}

	// Wee!!
	if (soundDef->volume1 == 0 && soundDef->volume2 == 0)
	{
#if defined(FOOD) && defined(DEBUG)
		TRACE("Trying to play sound \"%s\" and it is SILENT.\n", name);
#endif
		return NULL;
	}

	// Allocate the channel from the group
	SoundChannel *channel = group.AllocateChannel(*this, *soundDef);
	if (!channel)
		return NULL;

	// We are flying!
	return channel->GetHandle();
}

/***************************************************************************/
// Set the volume of the voice [0..1]
/***************************************************************************/
bool SoundEmitter::SetVolume(
SoundHandle handle,
float volume)
{
	// Resolve the channel
	SoundChannel *channel = ResolveHandle(handle);
	if (!channel)
		return false;

 	// Change volume
	channel->SetVolume(volume);
	return true;
}

/***************************************************************************/
// Set the pitch of the sound in semitones
/***************************************************************************/
bool SoundEmitter::SetPitch(
SoundHandle handle,
float pitchShift)
{
	// Resolve the channel
	SoundChannel *channel = ResolveHandle(handle);
	if (!channel)
		return false;

	channel->SetPitch(pitchShift);
	return true;
}

/***************************************************************************/
// Set wet/dry
/***************************************************************************/
bool SoundEmitter::SetWetDry(
SoundHandle handle,
float wet,
float dry)
{
	// Resolve the channel
	SoundChannel *channel = ResolveHandle(handle);
	if (!channel)
		return false;

	channel->SetWetDry(wet, dry);
	return true;
}

/***************************************************************************/
// Now that you have cue'd a sound, and setup it's play
// parameters you can start the sound playing
/***************************************************************************/
bool SoundEmitter::Play(
SoundHandle handle)
{
	// Resolve the channel
	SoundChannel *channel = ResolveHandle(handle);

	// If we lost our channel we can't play
	// We are ready to go...do we have a voice?
	if (!channel || !group.AquireVoice(channel))
		return false;

	// Play it!
	channel->Play(driver.Time());
	return true;
}

/***************************************************************************/
// Stop a playing sound (doesn't kill it)
/***************************************************************************/
bool SoundEmitter::Stop(
SoundHandle handle)
{
	// Resolve the channel
	SoundChannel *channel = ResolveHandle(handle);
	if (!channel)
		return false;

	// Stop it
	channel->Stop();
	return true;
}

/***************************************************************************/
// Is this sound still active
/***************************************************************************/
bool SoundEmitter::Active(
SoundHandle handle)
{
	// Resolve the channel
	SoundChannel *channel = ResolveHandle(handle);

	// It's playing if the channel is still held
	return channel != NULL;
}

/***************************************************************************/
// Is this sound looped?
/***************************************************************************/
bool SoundEmitter::Looped(SoundHandle handle)
{
	// Resolve the channel
	SoundChannel *channel = ResolveHandle(handle);
	if (!channel)
		return false;

	return channel->Looped();
}

/***************************************************************************/
// Kill this sound and revoke the handle
/***************************************************************************/
void SoundEmitter::Kill(
SoundHandle &handle)
{
	// Resolve the channel
	SoundChannel *channel = ResolveHandle(handle);

	// Kill the channel if it is still around
	if (channel)
		channel->Kill();

	// Kill the handle
	delete handle;

	// Make sure they don't touch it again
	handle = NULL;
}

/***************************************************************************/
// Kill all channels
/***************************************************************************/
void SoundEmitter::KillAll()
{
	// This isn't super fast, but people aren't going to use this
	// much
	SoundChannel *channel = group.FindFirst(this);
	while (channel)
	{
		// Kill it!
		channel->Kill();

		// next!
		channel = group.FindNext(this, channel);
	}
}

/***************************************************************************/
// Mark the emitter for death
// (will allow all non-looped sounds to finish)
/***************************************************************************/
bool SoundEmitter::MarkForDeath()
{
	if (state != ACTIVE)
		return false;

	state = MARKEDFORDEATH;
	return true;
}

/***************************************************************************/
// Mark the emitter for death
// (will allow all non-looped sounds to finish)
/***************************************************************************/
bool SoundEmitter::MarkedForDeath()
{
	return (state == MARKEDFORDEATH);
}

/***************************************************************************/
// Is this emitter ready to die?
/***************************************************************************/
bool SoundEmitter::ReadyToDie()
{
	return (state == DEAD);
}

/***************************************************************************/
// Make this emitter die on a soft flush
/***************************************************************************/
void SoundEmitter::KillOnSoftFlush(
bool killOnSoftFlush)
{
	dieOnSoftFlush = killOnSoftFlush;
}

/***************************************************************************/
// Should this emitter die on a soft flush?
/***************************************************************************/
bool SoundEmitter::DieOnSoftFlush()
{
	return dieOnSoftFlush;
}

/***************************************************************************/
// Is this the emitter's group?
/***************************************************************************/
bool SoundEmitter::IsGroup(
SoundGroup* testGroup)
{
	return (testGroup == &group);
}

/***************************************************************************/
// Run every frame or so
/***************************************************************************/
void SoundEmitter::Daemon()
{
	// Are they marked for death?
	if (state == MARKEDFORDEATH)
	{
		bool readyToDie = true;

		// If any non-looped sound is still playing we aren't ready to die
		SoundChannel *channel = group.FindFirst(this);
		while (channel && readyToDie)
		{
			if (!channel->Dead() && !channel->Looped())
				readyToDie = false;

			channel = group.FindNext(this, channel);
		}

		// Advance to death...
		if (readyToDie)
			state = DEAD;
	}
}

/***************************************************************************/
// A utility to resolve a handle to a channel
/***************************************************************************/
SoundChannel *SoundEmitter::ResolveHandle(
SoundHandle handle)
{
	// Null handles == no channel
	if (!handle)
		return NULL;

	// Pull out the channel
	SoundChannel *channel = handle->Channel();
	if (!channel)
		return NULL;

	// Is this a valid handle for us?
	ASSERT(&channel->Emitter() == this);
	if (&channel->Emitter() != this)
		return NULL;

	// Allow them access to the channel
	return channel;
}


