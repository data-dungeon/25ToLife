/***************************************************************************/
// Sound and Sound Instance stuff
/***************************************************************************/
#include "Audio/AudioPCH.h"

// statics
const int SoundInstance::MAXFREQUENCY = DSBFREQUENCY_MAX;
const int SoundInstance::MINFREQUENCY = DSBFREQUENCY_MIN;
const int SoundInstance::ORIGINALFREQUENCY =	DSBFREQUENCY_ORIGINAL;

// Overrides
float SoundInstance::masterVolume = 1.0f;
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
// Rewind the sound (no effect if playing)
/***************************************************************************/
bool SoundInstance::Rewind()
{
	ASSERT(buffer);

 	HRESULT ret = buffer->SetCurrentPosition(0);
 	SoundError::SetLastError(ret);
 	return (ret == DS_OK);
}

/***************************************************************************/
// Win32 only get play position
/***************************************************************************/
int SoundInstance::GetPlayPosition()
{
	ASSERT(buffer);

	DWORD playPos;
 	HRESULT ret = buffer->GetCurrentPosition(&playPos, 0);
 	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return -1;
	else
 		return (int)playPos;
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
	HRESULT ret = buffer->Stop();
	SoundError::SetLastError(ret);
	return (ret == DS_OK);
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
 	HRESULT ret = buffer->SetVolume(SoundVolume::UnitToHectodB(volume * masterVolume));
 	SoundError::SetLastError(ret);
 	return (ret == DS_OK);
}

/***************************************************************************/
// Set the volume [0..1], 0.0 is silent 1.0 is full volume
/***************************************************************************/
bool SoundInstance::SetVolumeDirect(
float volume)
{
	ASSERT(buffer);

	// track the volume internally
	this->volume = volume;
	if (paused)
		return true;

	// Set the state
 	HRESULT ret = buffer->SetVolume(SoundVolume::UnitToHectodB(volume));
 	SoundError::SetLastError(ret);
 	return (ret == DS_OK);
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
// Panning [-1..1] (-1.0 is hard left, 1.0 is hard right)
/***************************************************************************/
bool SoundInstance::SetPan(
float pan)
{
	ASSERT(buffer);

	// save this for queries
	this->pan = pan;
	if (paused)
		return true;

	// set the buffer
 	HRESULT ret = buffer->SetPan(SoundVolume::UnitPanToHectodB(pan * masterPan));
 	SoundError::SetLastError(ret);
 	return (ret == DS_OK);
}

/***************************************************************************/
// Get the current pan
/***************************************************************************/
float SoundInstance::GetPan()
{
	return pan;
}

/***************************************************************************/
// Volume and Pan combined
/***************************************************************************/
bool SoundInstance::SetVolumeAndPan(
float volume,
float pan)
{
	bool result;
	result  = SetVolume( volume );
	result &= SetPan( pan );

	return result;
}

/***************************************************************************/
// Set the pitch shift, given in semitones.  0.0 plays the sound at
// the original pitch (see semitone.h for helpful hints)
/***************************************************************************/
bool SoundInstance::SetPitch(
float semiTones)
{
	// Save the state
	pitchShift = semiTones;
	if (paused)
		return true;

	// Compute the shifted frequency
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
	return sound->Attr().PlayTimeMS(frequency);
}

/***************************************************************************/
// Someone else creates and manages us
/***************************************************************************/
SoundInstance::SoundInstance(
Sound *parentSound,
LPDIRECTSOUNDBUFFER soundBuffer)
{
	sound = parentSound;
	buffer = soundBuffer;
	ASSERT(buffer);
#ifdef DIRECTX_PC
	memset(&EAXBufferInfo, 0, sizeof(EAX::BUFFER));
	// create the EAX stuff
	if (SUCCEEDED(soundBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&EAXBufferInfo.lpDSB8)))
	{
		if (SUCCEEDED(EAXBufferInfo.lpDSB8->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&EAXBufferInfo.lpDS3DB8)))
		{
			if (SUCCEEDED(EAXBufferInfo.lpDS3DB8->QueryInterface(IID_IKsPropertySet, (void**)&EAXBufferInfo.lpPS)))
			{
				EAXBufferInfo.lpDSB = soundBuffer;
			}
		}
	}
#endif

	paused = false;
	looped = false;
	volume = 0.0f;
	pan = 0.0f;
	pitchShift = 0.0f;
	frequency = sound->Attr().SampleRate();
}

/***************************************************************************/
// Someone else creates and manages us
/***************************************************************************/
SoundInstance::SoundInstance(
SoundData *parentSound,
LPDIRECTSOUNDBUFFER soundBuffer)
{
	soundData = parentSound;
	buffer = soundBuffer;
	ASSERT(buffer);
#ifdef DIRECTX_PC
	usingEAX = false;
	memset(&EAXBufferInfo, 0, sizeof(EAX::BUFFER));
	if (usingEAX)
	{
	// create the EAX stuff
		if (SUCCEEDED(soundBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&EAXBufferInfo.lpDSB8)))
		{
			if (SUCCEEDED(EAXBufferInfo.lpDSB8->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&EAXBufferInfo.lpDS3DB8)))
			{
				if (SUCCEEDED(EAXBufferInfo.lpDS3DB8->QueryInterface(IID_IKsPropertySet, (void**)&EAXBufferInfo.lpPS)))
				{
					EAXBufferInfo.lpDSB = soundBuffer;
				}
			}
		}
	}
#endif

	paused = false;
	looped = false;
	volume = 0.0f;
	pan = 0.0f;
	pitchShift = 0.0f;
	frequency = sound->Attr().SampleRate();
}

/***************************************************************************/
// Clean up stuff
/***************************************************************************/
SoundInstance::~SoundInstance()
{
	if (buffer)
	{
		Stop(false);
#ifdef DIRECTX_PC
		if (EAXBufferInfo.lpPS)
		{
			EAXBufferInfo.lpPS->Release();
			EAXBufferInfo.lpPS = NULL;
		}

		if (EAXBufferInfo.lpDS3DB8)
		{
			EAXBufferInfo.lpDS3DB8->Release();
			EAXBufferInfo.lpDS3DB8 = NULL;
		}

		if (EAXBufferInfo.lpDSB8)
		{
			EAXBufferInfo.lpDSB8->Release();
			EAXBufferInfo.lpDSB8 = NULL;
		}

		if (EAXBufferInfo.lpDSB)
		{
			EAXBufferInfo.lpDSB = NULL;
		}
#endif
		buffer->Release();
	}
}

/***************************************************************************/
// Lock the entire buffer
/***************************************************************************/
void *SoundData::Lock()
{
	void		*ptr1;
	DWORD		region1size;

	ASSERT(buffer);

	// Lock it
	HRESULT ret = buffer->Lock(0, 0,
										&ptr1, &region1size,
										NULL, NULL, DSBLOCK_ENTIREBUFFER);
 	SoundError::SetLastError(ret);

	// Did it work?
	if (ret != DS_OK)
		return NULL;

	// This should be a true statement
	ASSERT((DWORD)size == region1size);
	return ptr1;
}

/***************************************************************************/
// Lock a certain sized region (from the start)
/***************************************************************************/
void *SoundData::Lock(
int bytesToLock)
{
	ASSERT(buffer);

	// Lock it
	void		*ptr1;
	DWORD		region1size;
	HRESULT ret = buffer->Lock(0, bytesToLock,
										&ptr1, &region1size,
										NULL, NULL, 0);
	SoundError::SetLastError(ret);

	// Did it work?
	if (ret != DS_OK)
		return NULL;

	// This should be a true statement
	ASSERT((DWORD)bytesToLock == region1size);
	return ptr1;
}

/***************************************************************************/
// This is when stuff may go into the sound card, i.e. PCI or worse, ISA
// data transfer.  Time could be used up here...
/***************************************************************************/
bool SoundData::Unlock(
void *bufferPointer,
int bytesWritten)
{
	ASSERT(buffer);

	HRESULT ret = buffer->Unlock(bufferPointer, bytesWritten, NULL, 0);
 	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/***************************************************************************/
// Circular lock
/***************************************************************************/
bool SoundData::Lock(
int offset,
int bytesToLock,
void **ptr1,
int *size1,
void **ptr2,
int *size2)
{
	ASSERT(buffer);

	// Lock it
	DWORD	region1size, region2size;
	HRESULT ret = buffer->Lock(offset, bytesToLock,
										ptr1, &region1size,
										ptr2, &region2size, 0);
	SoundError::SetLastError(ret);

	// Did it work?
	if (ret != DS_OK)
		return false;

	*size1 = region1size;
	*size2 = region2size;
	return true;
}

/***************************************************************************/
// Unlock a circular lock
/***************************************************************************/
bool SoundData::Unlock(
void *ptr1,
int written1,
void *ptr2,
int written2)
{
	ASSERT(buffer);

	HRESULT ret = buffer->Unlock(ptr1, written1, ptr2, written2);
 	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/***************************************************************************/
// Create free instances other than the root
/***************************************************************************/
SoundInstance *Sound::NewInstance(
const char *name)
{
	LPDIRECTSOUNDBUFFER target;

#if 0
	// Is there a "root" instance?
	if (instances.Head() || attr.AutoReclaim())
	{
		// Make a dup
  		if (!driver.DuplicateSoundBuffer(buffer, &target))
			return NULL;
	}
	else
	{
		// Just use the root buffer
		target = buffer;
		target->AddRef();
	}
#else
	// Always make a dup, seems more compatible
	if (!driver.DuplicateSoundBuffer(buffer, &target))
		return NULL;
#endif

	// make a new instance
	SoundInstance *instance = new SoundInstance(this, target);
	if (!instance)
	{
		target->Release();
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
LPDIRECTSOUNDBUFFER theBuffer,
int bufferSize,
SoundAttr &bufferAttr) :
driver(driverServices)
{
	// Setup all of the obvious quantities
	buffer = theBuffer;
	ASSERT(buffer);

	// Keep track of these for fun
	size = bufferSize;
	ASSERT(size > 0);
	attr = bufferAttr;
}

/***************************************************************************/
// Clean stuff up
/***************************************************************************/
SoundData::~SoundData()
{
	if (buffer)
		buffer->Release();
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



