/***************************************************************************/
// Sound and Sound Instance stuff
/***************************************************************************/
#include "Audio/AudioPCH.h"

#include "Audio/Xbox/sound.h"
#include "Audio/Xbox/sounderror.h"
#include "Audio/Math/volume.h"
#include "Audio/Math/semitone.h"

// statics
const int SoundInstance::MAXFREQUENCY = DSBFREQUENCY_MAX;
const int SoundInstance::MINFREQUENCY = DSBFREQUENCY_MIN;
const int SoundInstance::ORIGINALFREQUENCY =	DSBFREQUENCY_ORIGINAL;

// Overrides
float SoundInstance::masterVolume = 1.0f;
float SoundInstance::masterPanFlip = 1.0f;
float SoundInstance::masterPitch = 0.0f;

float SoundInstance::masterPan = 1.0f;

/***************************************************************************/
// Set the overall volume for all sound
/***************************************************************************/
void SoundInstance::SetMasterVolume(
float volume)
{
	masterVolume = Math::Clamp(volume);
}

/***************************************************************************/
// Get the overall volume for all sound
/***************************************************************************/
float SoundInstance::GetMasterVolume()
{
	return masterVolume;
}

/***************************************************************************/
// Are we flipping l/r
/***************************************************************************/
void SoundInstance::SetMasterPanFlip(
bool flipLeftRight)
{
	if (flipLeftRight)
		masterPanFlip = -1.0f;
	else
		masterPanFlip = 1.0f;
}

/***************************************************************************/
// Are we flipping l/r
/***************************************************************************/
bool SoundInstance::GetMasterPanFlip()
{
	return (masterPanFlip < 0.0f);
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
	ASSERT(buffer);

	// Unpause
	if (paused)
	{
		// restore the state of the sound
		paused = false;
		SetVolume(volume);
		SetPan(pan);
		SetFrequency(frequency);
	}

	looped = (loop || sound->Attr().Streaming());
	HRESULT ret = buffer->Play(0, 0, looped ? DSBPLAY_LOOPING : 0);
	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/***************************************************************************/
// Stop playing the sound
/***************************************************************************/
bool SoundInstance::Stop(
bool rewind)
{
	ASSERT(buffer);

	// Unpause
	if (paused)
	{
		// restore the state of the sound
		paused = false;
		SetVolume(volume);
		SetPan(pan);
		SetFrequency(frequency);
	}
	else
	{
		// Really stop the sound
		HRESULT ret = buffer->Stop();
		SoundError::SetLastError(ret);
		if (ret != DS_OK)
			return false;
	}

	// Rewind it?
	if (rewind)
		return Rewind();

	return true;
}

/***************************************************************************/
// Is this sound instance currently playing
/***************************************************************************/
bool SoundInstance::IsPlaying()
{
	ASSERT(buffer);

	// If the sound is paused, it is still "playing"
	if (paused)
		return true;

	// Query the buffer
	DWORD status;
	HRESULT ret = buffer->GetStatus(&status);
	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return false;

	return ((status & DSBSTATUS_PLAYING) != 0);
}

/***************************************************************************/
// Rewind the sound (probably no effect if playing)
/***************************************************************************/
bool SoundInstance::Rewind()
{
	ASSERT(buffer);

 	HRESULT ret = buffer->SetCurrentPosition(0);
 	SoundError::SetLastError(ret);
 	return (ret == DS_OK);
}

/***************************************************************************/
// Pause this sound
/***************************************************************************/
bool SoundInstance::Pause()
{
	if (!IsPlaying())
		return false;

	// set this state
	paused = true;

	// Make sure the buffer is stopped (dx shouldn' t lose play cursor)
#if 1
	ASSERT(buffer);
	DWORD status;
	buffer->StopEx(0, DSBSTOPEX_IMMEDIATE);
	do
	{
		buffer->GetStatus(&status);
	} while(status & DSBSTATUS_PLAYING);
	return (true);
#else
	HRESULT ret = buffer->Stop();
	SoundError::SetLastError(ret);
	return (ret == DS_OK);
#endif
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
		SetVolume(volume);
		SetPan(pan);
		SetFrequency(frequency);
		Play(looped);
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
float volume)
{
	ASSERT(buffer);

	// track the volume internally
	this->volume = volume;
	if (paused)
		return true;

	// Set the state
	return ApplyMixBins();
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
	ASSERT(buffer);

	if (paused)
		return true;

	// Oh shit, should do something with master volume in here
 	HRESULT ret = buffer->SetVolume((int)(dB * 100.0f));
 	SoundError::SetLastError(ret);
 	return (ret == DS_OK);
}

/***************************************************************************/
// Set the wet/dry mix.
/***************************************************************************/
bool SoundInstance::SetWetDry(
float newWet,
float newDry)
{
	wet = newWet;
	dry = newDry;
	return ApplyMixBins();
}

/***************************************************************************/
// Panning
/***************************************************************************/
bool SoundInstance::SetPan(
float newPan)
{
	ASSERT(buffer);

	// save this since we can't retreive it
	pan = newPan;

	return ApplyMixBins();
}

/***************************************************************************/
// Volume and Pan combined
/***************************************************************************/
bool SoundInstance::SetVolumeAndPan(
float volume,
float pan)
{
	return (SetVolume(volume) && SetPan(pan));
}

/***************************************************************************/
// Get the current pan
/***************************************************************************/
float SoundInstance::GetPan()
{
	return pan;
}

/***************************************************************************/
// Set the pitch shift, given in semitones.  0.0 plays the sound at
// the original pitch (see semitone.h for helpful hints)
/***************************************************************************/
bool SoundInstance::SetPitch(
float semiTones)
{
	pitchShift = semiTones;
	if (paused)
		return true;

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
int frequency)
{
	ASSERT(buffer);

	// save the state
	this->frequency = frequency;
	if (paused)
		return true;

	// setup the buffer
 	HRESULT ret = buffer->SetFrequency(frequency);
 	SoundError::SetLastError(ret);
 	return (ret == DS_OK);
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
	return sound->PlayTime();
}

/***************************************************************************/
// Someone else creates and manages us
/***************************************************************************/
SoundInstance::SoundInstance(
Sound *parentSound,
IDirectSoundBuffer8 *soundBuffer) :
driver(parentSound->Driver())
{
	sound = parentSound;
	buffer = soundBuffer;
	ASSERT(buffer);

	paused = false;
	looped = false;
	volume = 0.0f;
	pan = 0.0f;
	pitchShift = 0.0f;
	frequency = sound->Attr().SampleRate();
	dry = 1.0f;
	wet = 0.0f;
}

/***************************************************************************/
// Clean up stuff
/***************************************************************************/
SoundInstance::~SoundInstance()
{
	if (buffer)
	{
		DWORD status;
		buffer->StopEx(0, DSBSTOPEX_IMMEDIATE);
		do
		{
			buffer->GetStatus(&status);
		} while(status & DSBSTATUS_PLAYING);

		buffer->Release();
	}
}

/***************************************************************************/
// Apply mixbins
/***************************************************************************/
bool SoundInstance::ApplyMixBins()
{
	if (!buffer)
		return true;

	return driver.ApplyMixBins3D(*buffer, volume * masterVolume, pan * masterPanFlip, wet, dry);
}

/***************************************************************************/
// Create free instances other than the root
/***************************************************************************/
SoundInstance *Sound::NewInstance(const char *name)
{
	// Ask the driver for a sound
	IDirectSoundBuffer8 *sound = driver.CreateSoundInstance(data, size, playOffset, playLength, attr, wfx);
	if (!sound)
		return NULL;

	// Create the instance
	SoundInstance *instance = new SoundInstance(this, sound);
	if (!instance)
	{
		sound->Release();
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
	instances.Remove(instance);
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
// The sound interface
/***************************************************************************/
SoundData::SoundData(
SoundDriverServices &driverServices,
void *bankData,
int bankSize,
int dataOffset,
int dataLength,
WAVEFORMATEX &waveFormat) :
driver(driverServices),
attr(SoundAttr::VOLUMECONTROL | SoundAttr::PANCONTROL | SoundAttr::PITCHCONTROL, SAMPLE44KHz)
{
	ASSERT(bankData);
	ASSERT(bankSize > 0);
	ASSERT(dataOffset >= 0);
	ASSERT((dataOffset % 36) == 0);
	ASSERT(dataLength > 0);

	// Setup all of the obvious quantities
	data = bankData;
	size = (unsigned int)bankSize;
	playOffset = (unsigned int)dataOffset;
	playLength = (unsigned int)dataLength;

	// Save this
	wfx = waveFormat;

	// We have to have an attribute, dummy one up
	attr.InferFromWFX(wfx);

	// approximate play time
	playTime = (wfx.nAvgBytesPerSec * 1000) / dataLength;
}

/***************************************************************************/
// Free our memory
/***************************************************************************/
SoundData::~SoundData()
{
}

/***************************************************************************/
// What is the maximum/minimum pitch you can set?
/***************************************************************************/
float SoundData::GetMinPitch()
{
	int base = attr.SampleRate();
	int target = SoundInstance::MINFREQUENCY;
	return SemitoneMath::SemitoneDifference(base, target);
}

/***************************************************************************/
// What is the maximum/minimum pitch you can set?
/***************************************************************************/
float SoundData::GetMaxPitch()
{
	int base = attr.SampleRate();
	int target = SoundInstance::MAXFREQUENCY;
	return SemitoneMath::SemitoneDifference(base, target);
}


