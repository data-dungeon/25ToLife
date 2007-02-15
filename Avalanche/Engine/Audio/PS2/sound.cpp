/***************************************************************************/
// Sound and Sound Instance stuff
/***************************************************************************/
#include "Audio/AudioPCH.h"

// statics
const int SoundInstance::MAXFREQUENCY = 100000;
const int SoundInstance::MINFREQUENCY = 0;
const int SoundInstance::ORIGINALFREQUENCY =	-1;

// Overrides
float SoundInstance::masterVolume = 1.0f;
float SoundInstance::masterPitch = 0.0f;
float SoundInstance::masterPan = 1.0f;

// Rob Nelson, this is your plan!
#ifndef _DEBUG
FixedPool<SoundInstance, MAX_SOUND_INSTANCES> Sound::instancePool(true);
#else
FixedPool<SoundInstance, MAX_SOUND_INSTANCES> Sound::instancePool("SoundInstance", true);
#endif

/***************************************************************************/
// Set the overall volume for all sound
/***************************************************************************/
void SoundInstance::SetMasterVolume(
float volume)
{
	masterVolume = Math::Clamp(volume);
	ASSERT(!Math::Zero(masterVolume));
}

/***************************************************************************/
// Get the overall volume for all sound
/***************************************************************************/
float SoundInstance::GetMasterVolume()
{
	return masterVolume;
}

/***************************************************************************/
// Set the overall pitch for all sounds
/***************************************************************************/
void SoundInstance::SetMasterPitch(
float pitch)
{
	masterPitch = pitch;
}

/***************************************************************************/
// Get the overall pitch for all sound
/***************************************************************************/
float SoundInstance::GetMasterPitch()
{
	return masterPitch;
}

/***************************************************************************/
/***************************************************************************/
void SoundInstance::SetOutputMode(
SoundDriver::OUTPUTMODE mode)
{
	switch (mode)
	{
		case SoundDriver::OUTPUTMODE_MONO:
			masterPan = 0.0f;
			break;
		case SoundDriver::OUTPUTMODE_STEREO:
			masterPan = 1.0f;
			break;
		case SoundDriver::OUTPUTMODE_STEREOFLIPPED:
			masterPan = -1.0f;
			break;
		case SoundDriver::OUTPUTMODE_SURROUND:
		case SoundDriver::OUTPUTMODE_DOLBY51:
			masterPan = 1.0f;
			break;
	}
}

/***************************************************************************/
// Play the sound
/***************************************************************************/
bool SoundInstance::Play(
bool loop)
{
	// Play this if paused
	if (paused)
		Unpause();

	// Hmm, this doesn't work yet
	bool loopIt = (loop || sound->Attr().Streaming());
	looped = loopIt;

	// Compute the overall volume based on pan
	int volumeL, volumeR;
	ComputeVolume(volume * masterVolume, pan * masterPan, volumeL, volumeR);

	// Compute the current pitch
	int pitch = FrequencyToPS2Pitch(frequency);

	// play it
	channel.SetWetDry(wet, dry);
	return channel.KeyOn(ID(), volumeL, volumeR, pitch);
}

/***************************************************************************/
// Stop playing the sound
/***************************************************************************/
bool SoundInstance::Stop(
bool rewind)
{
	// stop it
	bool ret = channel.KeyOff();

	// Unpause
	Unpause();

	// Rewind it?
	if (rewind)
		return Rewind();

	return ret;
}

/***************************************************************************/
// Is this sound instance currently playing
/***************************************************************************/
bool SoundInstance::IsPlaying()
{
	// ENDX test doesn't seem to work correctly
	if (looped)
		return true;

	return channel.IsPlaying();
}

/***************************************************************************/
// Rewind the sound (no effect if playing)
/***************************************************************************/
bool SoundInstance::Rewind()
{
	// This doesn't work, so pretend it does
	return true;
}

/***************************************************************************/
// Pause this sound
/***************************************************************************/
bool SoundInstance::Pause()
{
	// set this state
	paused = true;

	return channel.SetPitch(0);
}

/***************************************************************************/
// Unpause this sound
/***************************************************************************/
bool SoundInstance::Unpause()
{
	// clear the state
	if (paused)
	{
		paused = false;
		SetFrequency(frequency);
		SetVolume(volume);
		SetPan(pan);
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool SoundInstance::Paused()
{
	return paused;
}

/***************************************************************************/
// Set the volume [0..1], 0.0 is silent 1.0 is full volume
/***************************************************************************/
bool SoundInstance::SetVolume(
float newVolume)
{
	// Get the volume
	volume = newVolume;

	// Get out of here
	if (paused)
		return true;

	// Compute the overall volume based on pan
	int volumeL, volumeR;
	ComputeVolume(volume * masterVolume, pan * masterPan, volumeL, volumeR);

	return SetVoiceVolume(volumeL, volumeR);
}

/***************************************************************************/
// Get the current volume
/***************************************************************************/
float SoundInstance::GetVolume()
{
	return volume;
}

/***************************************************************************/
// Set attenuation in dB, from -100db to 0db.
// Note that this changes the volume...just like SetVolume so don't call both
/***************************************************************************/
bool SoundInstance::SetAttenuationdB(
float dB)
{
	// Oh shit, not sure how to do this
	return SetVolume(dB / 100.0f + 1.0f);
}

/***************************************************************************/
// Set the wet/dry mix.  Beware, on the PS2 anything over 0.0
// will be 100% on
/***************************************************************************/
bool SoundInstance::SetWetDry(
float wetMix,
float dryMix)
{
	wet = wetMix > 0.0f;
	dry = dryMix > 0.0f;

	return channel.SetWetDry(wet, dry);
}

/***************************************************************************/
/***************************************************************************/
float SoundInstance::GetDry()
{
	return dry ? 1.0f : 0.0f;
}

/***************************************************************************/
/***************************************************************************/
float SoundInstance::GetWet()
{
	return wet ? 1.0f : 0.0f;
}

/***************************************************************************/
// Panning
/***************************************************************************/
bool SoundInstance::SetPan(
float newPan)
{
	// Save the pan
	pan = newPan;

	// Get out of here
	if (paused)
		return true;

	// Compute the overall volume based on pan
	int volumeL, volumeR;
	ComputeVolume(volume * masterVolume, pan * masterPan, volumeL, volumeR);

	return SetVoiceVolume(volumeL, volumeR);
}

/***************************************************************************/
// Get the current pan
/***************************************************************************/
float SoundInstance::GetPan()
{
	return pan;
}

/***************************************************************************/
// Volume and pan combined
/***************************************************************************/
bool SoundInstance::SetVolumeAndPan(
float newVolume,
float newPan)
{
	//Save
	volume = newVolume;
	pan = newPan;

	// Get out of here
	if( paused )
		return true;

	// Compute the overall volume base on pan
	int volumeL, volumeR;
	ComputeVolume(volume * masterVolume, pan * masterPan, volumeL, volumeR);

	return SetVoiceVolume(volumeL, volumeR);
}

/***************************************************************************/
// Set the pitch shift, given in semitones.  0.0 plays the sound at
// the original pitch (see semitone.h for helpful hints)
/***************************************************************************/
bool SoundInstance::SetPitch(
float semiTones)
{
	pitchShift = semiTones;
	int frequency = sound->Attr().SampleRate();
	int shift = SemitoneMath::ModifyFrequencyBySemitones(frequency, semiTones + masterPitch);
	shift = Math::Clamp(shift, MINFREQUENCY, MAXFREQUENCY);
	return SetFrequency(shift);
}

/***************************************************************************/
/***************************************************************************/
float SoundInstance::GetPitch()
{
	return pitchShift;
}

/***************************************************************************/
// Frequency (in Hz)
/***************************************************************************/
bool SoundInstance::SetFrequency(
int newFrequency)
{
	if (newFrequency == ORIGINALFREQUENCY)
		frequency = sound->Attr().SampleRate();
	else
		frequency = newFrequency;

	// Get out of here
	if (paused)
		return true;

	int pitch = FrequencyToPS2Pitch(frequency);
	return channel.SetPitch(pitch);
}

/***************************************************************************/
// Get the current frequency
/***************************************************************************/
int SoundInstance::GetFrequency()
{
	return frequency;
}

/***************************************************************************/
// What is our (approximate) play time (in ms)
/***************************************************************************/
unsigned int SoundInstance::PlayTime()
{
	return sound->Attr().PlayTimeMS(frequency);
}

/***************************************************************************/
/***************************************************************************/
inline void SoundInstance::ComputeVolume(
float volume,
float pan,
int &volumeL,
int &volumeR)
{
	// Make sure the volume is in range
	volume = Math::Clamp(volume);

	// This is right, according to the internet
	ASSERT(pan >= -1.0f && pan <= 1.0f);
	pan *= 0.5f;
	float vL = Math::Sqrt(0.5f - pan) * volume;
	float vR = Math::Sqrt(0.5f + pan) * volume;

	// Put this into sony space
	volumeL = Math::Clamp((int)(vL * (float)0x3fff), 0, 0x3fff);
	volumeR = Math::Clamp((int)(vR * (float)0x3fff), 0, 0x3fff);
}

/***************************************************************************/
// Someone else creates and manages us
/***************************************************************************/
SoundInstance::SoundInstance(
Sound *parentSound,
SPUChannel &onChannel) :
channel(onChannel)
{
	paused = false;
	looped = false;
	volume = 1.0f;
	pan = 0.0f;
	pitchShift = 0.0f;
	frequency = parentSound->Attr().SampleRate();
	wet = false;
	dry = true;
	smoothVolume = false;
	sound = parentSound;
	userData = 0;
}

/***************************************************************************/
// Clean up stuff
/***************************************************************************/
SoundInstance::~SoundInstance()
{
	if (IsPlaying())
		Stop();

	MultiStream::FreeChannel(&channel);
}

/***************************************************************************/
// Volume management
/***************************************************************************/
bool SoundInstance::SetVoiceVolume(
int volumeL,
int volumeR)
{
	//TRACE("requesting volume %d %d on channel %d\n", volumeL, volumeR, channel);
	if (channel.SetVolume(volumeL, volumeR, smoothVolume))
		return false;
	smoothVolume = true;
	return true;
}

/***************************************************************************/
// Create free instances other than the root
/***************************************************************************/
SoundInstance *Sound::NewInstance(
const char *name)
{
	// Allocate a channel
	SPUChannel *channel = MultiStream::AllocateChannel(name);
	if (!channel)
		return NULL;

	// make a new instance
	void *buffer = instancePool.NewPtr();
	SoundInstance *instance = new (buffer) SoundInstance(this, *channel);
	if (!instance)
	{
		// Deallocate channel
		MultiStream::FreeChannel(channel);
		return NULL;
	}

	// Add it to the proper list
	if (attr.AutoReclaim())
		driver.AutoReclaim(instance);
	else
		instances.AddHead(instance);

	return instance;
}

/***************************************************************************/
// Free an instance
/***************************************************************************/
void Sound::FreeInstance(
SoundInstance *instance)
{
	// Free it
	ASSERT(instance->sound == this);
	instances.Unlink(instance);
	instancePool.Delete(instance);
}

/***************************************************************************/
// Pause this sound instances
/***************************************************************************/
bool Sound::Pause()
{
	SoundInstance *inst = instances.Head();
	while (inst)
	{
		inst->Pause();
		inst = inst->next;
	}

	return true;
}

/***************************************************************************/
// Unpause this sound instances
/***************************************************************************/
bool Sound::Unpause()
{
	SoundInstance *inst = instances.Head();
	while (inst)
	{
		inst->Unpause();
		inst = inst->next;
	}

	return true;
}

/***************************************************************************/
// Call this from driver autoreclaim
/***************************************************************************/
void Sound::AutoReclaimed(SoundInstance *instance)
{
	if (instance)
		instancePool.Delete(instance);
}

/***************************************************************************/
// The sound interface
/***************************************************************************/
Sound::Sound(
SoundDriverPS2 &parent,
SFXID sfxID,
int sfxSize,
SoundAttr &bufferAttr) :
driver(parent),
instances(false)
{
	// Setup all of the obvious quantities
	id = sfxID;
	size = sfxSize;
	attr = bufferAttr;
}

/***************************************************************************/
// Clean stuff up
/***************************************************************************/
Sound::~Sound()
{
	// Loop through the instances and clean them up
	SoundInstance *inst = instances.Head();
	while (inst)
	{
		SoundInstance *next = inst->next;
		instances.Unlink(inst);
		instancePool.Delete(inst);
		inst = next;
	}

	// Get rid of our data
	MultiStream::FreeSFXID(id);
}

