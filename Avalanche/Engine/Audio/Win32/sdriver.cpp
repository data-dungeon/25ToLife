/***************************************************************************/
// Sound driver
/***************************************************************************/
#include "Audio/AudioPCH.h"

#ifdef DIRECTX_PC
#include "EAXHelper.h"
#endif

/***************************************************************************/
// All subclasses of sounddriver must write this function
// must be implemented it in SoundDriver:: namespace
/***************************************************************************/
SoundDriver *SoundDriver::CreateAndSetOutputFormat(
OSCREATEINFO osInfo)
{
	// Create the object
	MEM_SET_ONE_SHOT_NEW_NAME("SoundDriverWin32");
	SoundDriverWin32 *driver = new SoundDriverWin32;
	if (!driver)
	{
		ASSERT(false);
		return NULL;
	}

	// Call it's create
	if (!driver->Create(osInfo))
	{
		ASSERT(false);
		delete driver;
		return NULL;
	}

	// Set the default/best output format
	if (!driver->SetOutputFormat(SAMPLE44KHz, true))
	{
		ASSERT(false);
		driver->Destroy();
		delete driver;
		return NULL;
	}

	return driver;
}

/***************************************************************************/
// Just init stuff
/***************************************************************************/
SoundDriverWin32::SoundDriverWin32() :
autoReclaim(false)
{
	services = NULL;
	hWndOwner = NULL;
	directSound = NULL;
	primaryBuffer = NULL;
	nextTimeout = 0xffffffff;
}

/***************************************************************************/
// Clean everything up
/***************************************************************************/
SoundDriverWin32::~SoundDriverWin32()
{
	Destroy();
}

/***************************************************************************/
// Create the sound driver, has platform specific parameters
// This is called by CreateAndSetOutputFormat
/***************************************************************************/
bool SoundDriverWin32::Create(
OSCREATEINFO osInfo)
{
	return Create((HWND)osInfo, NULL);
}

/***************************************************************************/
// Pass in NULL for the SOUNDDRIVERID if you want to select the default
// sound device
/***************************************************************************/
bool SoundDriverWin32::Create(
HWND hWnd,
SoundDriverEnumerator::SOUNDDRIVERID deviceID)
{
	// Save the creator
	hWndOwner = hWnd;

	// Pick the "best" device
	if (!deviceID)
		deviceID = BestDevice();

	// Do the create
#ifdef DIRECTX_PC
	if (EAX::usingEAX)
	{
		// create the device so EAX likes it
		bool created = false;
		DSBUFFERDESC dsbdPrimary;
		memset(&dsbdPrimary, 0, sizeof(DSBUFFERDESC));
		dsbdPrimary.dwSize = sizeof(DSBUFFERDESC);
		dsbdPrimary.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

		if (SUCCEEDED(DirectSoundCreate8(deviceID, &directSound, NULL)))
		{
			if(SUCCEEDED(directSound->CreateSoundBuffer(&dsbdPrimary, &EAX::DSBPrimaryPtr, NULL )))
			{
				if (SUCCEEDED(EAX::DSBPrimaryPtr->Play(0,0,DSBPLAY_LOOPING)))
				{
					if (SUCCEEDED(EAX::DSBPrimaryPtr->QueryInterface(IID_IDirectSound3DListener8, (void**)&EAX::DS3DListenerPtr)))
					{
						if (SUCCEEDED(EAX::DS3DListenerPtr->SetPosition(0,0,0,DS3D_DEFERRED)))
						{
							created = true;
							// mess with our conversion factor for more realistic sound
							EAX::DS3DListenerPtr->SetDistanceFactor(0.01f, DS3D_DEFERRED);
						}
					}
				}
			}
		}
		if (!created)
			return false;
	}
	else
	{
			HRESULT ret = DirectSoundCreate8(deviceID, &directSound, NULL);
			SoundError::SetLastError(ret);
			if (ret != DS_OK)
			return false;
	}
#else
	HRESULT ret = DirectSoundCreate(deviceID, &directSound, NULL);
	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return false;
#endif

	// Set our priority
	SetPriority(NORMAL);

	// Create our services
	MEM_SET_ONE_SHOT_NEW_NAME("SoundDriverServices");
	services = new SoundDriverServices(*this);
	if (!services)
		return false;

	return true;
}

/***************************************************************************/
// Set our priority
/***************************************************************************/
bool SoundDriverWin32::SetPriority(
SOUNDPRIORITY priority)
{
	if (!directSound)
		return false;

	HRESULT ret;
	switch (priority)
	{
		case NORMAL:
	  		ret = directSound->SetCooperativeLevel(hWndOwner, DSSCL_NORMAL);
			break;
		case PRIORITY:
	  		ret = directSound->SetCooperativeLevel(hWndOwner, DSSCL_PRIORITY);
			break;
		case EXCLUSIVE:
	  		ret = directSound->SetCooperativeLevel(hWndOwner, DSSCL_EXCLUSIVE);
			break;
	}

	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/***************************************************************************/
// Destroy it (destructor does the same thing)
/***************************************************************************/
bool SoundDriverWin32::Destroy()
{
	// we are done with now
	if (primaryBuffer)
	{
		primaryBuffer->Stop();
		primaryBuffer->Release();
		primaryBuffer = NULL;
	}
	if (directSound)
	{
		directSound->Release();
		directSound = NULL;
	}
	if (services)
	{
		delete services;
		services = NULL;
	}

	return true;
}

/***************************************************************************/
// Initialize the output format, will return false with NORMAL priority
/***************************************************************************/
bool SoundDriverWin32::SetOutputFormat(
unsigned int desiredSampleRate,
bool sixteenBit)
{
	// Let's just leave windows at defaults right now,
	// as it's really just for debugging
#if 0
	// Setup a description
	DSBUFFERDESC soundDesc;
	soundDesc.dwSize = sizeof(DSBUFFERDESC);
	soundDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	soundDesc.dwBufferBytes = 0;
	soundDesc.lpwfxFormat = NULL;
	soundDesc.dwReserved = 0;

	// Get access to the primary
  	HRESULT ret = directSound->CreateSoundBuffer(&soundDesc, &primaryBuffer, NULL);
	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return false;

	// Set it to the desired format
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = wantStereo ? 2 : 1;
	wfx.nSamplesPerSec = desiredSampleRate;
	wfx.wBitsPerSample = sixteenBit ? 16 : 8;
  	wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = 0;

	// Set it
	ret = primaryBuffer->SetFormat(&wfx);
	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return false;

	// Start it looping (helps with latency)
	ret = primaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return false;
#endif

	// Make sure we've set the output mode
	SetOutputMode(outputMode);

	return true;
}

/***************************************************************************/
// Set the mode, returns false if not supported
/***************************************************************************/
bool SoundDriverWin32::SetOutputMode(
OUTPUTMODE mode)
{
	// Tell the parent
	if (!SoundDriver::SetOutputMode(mode))
		return false;

	// Tell the sound
	SoundInstance::SetOutputMode(outputMode);

	return true;
}

/***************************************************************************/
// Test to see if this mode is supported
/***************************************************************************/
bool SoundDriverWin32::OutputModeSupported(
OUTPUTMODE mode)
{
	switch (mode)
	{
		case OUTPUTMODE_MONO:
			return true;

		case OUTPUTMODE_STEREO:
			return true;

		case OUTPUTMODE_STEREOFLIPPED:
			return true;

		case OUTPUTMODE_SURROUND:
			return false;

		case OUTPUTMODE_DOLBY51:
			return false;

		default:
			ASSERT(false);
			return false;
	}
}

/***************************************************************************/
// What is the (approximate) max channels that can be simultaneously played?
/***************************************************************************/
int SoundDriverWin32::MaxChannels()
{
	// This depends on how many streams they have, but this is a good
	// estimate.
	return 45;
}

/***************************************************************************/
// What is the maximum number of streams that can be simultaneously played?
/***************************************************************************/
int SoundDriverWin32::Max3DStreams()
{
	return 4;
}

/***************************************************************************/
// The Daemon!
/***************************************************************************/
void SoundDriverWin32::Daemon()
{
	// Call the parent
	SoundDriver::Daemon();

	// Time to do work?
	if (Time() >= nextTimeout)
	{
		// Manage reclaimable sounds
		SoundInstance *instance = autoReclaim.Head();
		while (instance)
		{
			SoundInstance *next = instance->next;
			if (!instance->IsPlaying())
			{
				autoReclaim.Remove(instance);
				delete instance;
			}
			instance = next;
		}
	}
}

/***************************************************************************/
// Create a sound from a memory image (image for this driver is an .eat
// file, that has a WFX followed by raw sound data)
/***************************************************************************/
Sound *SoundDriverWin32::CreateSound(
SoundBank &bank,
void *data,
int size)
{
	ASSERT(data != NULL);
	ASSERT(size > 0);

	// This is dumb
	if (!services)
		return NULL;

	// Get the pointer to the wave format
	WAVEFORMATEX *wfx = (WAVEFORMATEX *)data;

	// Setup the pointer for the data
	size -= sizeof(WAVEFORMATEX);
	data = (unsigned char *)data + sizeof(WAVEFORMATEX);

	// We have to have an attribute, dummy one up
	SoundAttr attr(SoundAttr::VOLUMECONTROL | SoundAttr::PANCONTROL | SoundAttr::PITCHCONTROL, SAMPLE44KHz);
	attr.InferFromWFX(*wfx);

	// fill out the buffer description
	DSBUFFERDESC bufferDesc;
	memset(&bufferDesc, '\0', sizeof(bufferDesc));
	bufferDesc.dwSize = sizeof(bufferDesc);
	bufferDesc.dwBufferBytes = size;
	bufferDesc.dwFlags = attr.DSBufferDescFlags();
	bufferDesc.lpwfxFormat = wfx;

#ifdef DIRECTX_PC
	if (EAX::usingEAX)
	{
		bufferDesc.dwFlags =  DSBCAPS_LOCDEFER | DSBCAPS_CTRL3D;
	}
#endif

	// Make the direct sound buffer
	LPDIRECTSOUNDBUFFER soundBuffer;
	HRESULT ret = directSound->CreateSoundBuffer(&bufferDesc, &soundBuffer, NULL);
	if (ret != DS_OK)
		return NULL;

	// Allocate the buffer
	Sound *sound = new Sound(*services, soundBuffer, size, attr);
	if (!sound)
	{
		soundBuffer->Release();
		return NULL;
	}

	// Lock & load, bayybee.
	void *soundData = sound->Lock();
	if (!soundData)
	{
		delete sound;
		return NULL;
	}
	memcpy(soundData, data, size);
	sound->Unlock(soundData);

	// give it back to them, ready to go
	return sound;
}

/***************************************************************************/
// Does this stream exist?
/***************************************************************************/
bool SoundDriverWin32::StreamExist(
const char *fileName)
{
	// Clean up the filename
	char streamFile[128];
	PathFile::MakePathForceExt(streamFile, BaseStreamPath(), fileName, StreamFormat());

	return Media::FileExists(streamFile);
}

/***************************************************************************/
// Stream a platform specific file off of the disk
// DO NOT PUT AN EXTENSION ON THIS NAME!
/***************************************************************************/
SoundStream *SoundDriverWin32::CreateStream(
const char *fileName,
bool repeat,
bool preLoad)
{
	// We don't support preload
	ASSERT(preLoad == false);

	// Clean up the filename
	char streamFile[128];
	PathFile::MakePathForceExt(streamFile, BaseStreamPath(), fileName, StreamFormat());

	// create the wave io system
#ifdef DIRECTX_PC
	OggStreamIOSystem *waveIO = new OggStreamIOSystem(streamFile);
#else
	WaveStreamIOSystem *waveIO = new WaveStreamIOSystem(streamFile);
#endif
	if (!waveIO)
		return NULL;
	else if (!waveIO->ReadValid())
	{
		delete waveIO;
		return NULL;
	}

	// fill out the rest of the attr
	WAVEFORMATEX wfx = *waveIO->WaveFormat();

	// Create the sound
	SoundStreamWin32 *sound = AllocateSoundStream(fileName, repeat, &wfx);
	if (!sound)
	{
		delete waveIO;
		return NULL;
	}

	// Set the IO system
	if (!sound->SetIOSystem(waveIO))
	{
		delete sound;
		return NULL;
	}

	return sound;
}

/***************************************************************************/
// Pause all currently playing sounds
/***************************************************************************/
void SoundDriverWin32::Pause(PauseType pauseType)
{
	// Call the parent
	SoundDriver::Pause(pauseType);

	if ((int)pauseType & PAUSESFX)
	{
		// Handle the auto-reclaim list
		SoundInstance *sound = autoReclaim.Head();
		while (sound)
		{
			sound->Pause();
			sound = sound->next;
		}
	}
}

/***************************************************************************/
// Resume all currently paused sounds
/***************************************************************************/
void SoundDriverWin32::Unpause(PauseType pauseType)
{
	// Call the parent
	SoundDriver::Unpause(pauseType);

	if ((int)pauseType & PAUSESFX)
	{
		// Handle the auto-reclaim list
		SoundInstance *sound = autoReclaim.Head();
		while (sound)
		{
			sound->Unpause();
			sound = sound->next;
		}
	}
}

/***************************************************************************/
// Pick the best device
/***************************************************************************/
SoundDriverEnumerator::SOUNDDRIVERID SoundDriverWin32::BestDevice()
{
#if 0
	SoundDriverEnumerator sdEnum;
	static SoundDriverEnumerator::SOUNDDRIVERID nullID = { 0 };

	// How many are there?
	for (int i = 0; i < sdEnum.HowMany(); i++)
	{
		SoundDriverEnumerator::SOUNDDRIVER desc;
		sdEnum.GetDriver(i, &desc);
		//if (strcmpi(desc.driverName, "Hello world!") == 0)
		if (memcmp(&desc.driverID, &nullID, sizeof(nullID)) != 0)
			return desc.driverID;
	}
#endif
	return NULL;
}

/***************************************************************************/
// Allocate a stream based sound
/***************************************************************************/
SoundStreamWin32 *SoundDriverWin32::AllocateSoundStream(
const char *name,
bool loop,
WAVEFORMATEX *wfx)
{
	// Create the attr
	SoundAttr attr;
	attr.InferFromWFX(*wfx);
	attr.Streaming(true);

	// fill out the buffer description
	DSBUFFERDESC bufferDesc;
	int size = wfx->nAvgBytesPerSec * 2;
	bufferDesc.dwSize = sizeof(bufferDesc);
	bufferDesc.dwBufferBytes = size;
	bufferDesc.dwFlags = attr.DSBufferDescFlags();
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = wfx;

//#ifdef DIRECTX_PC
//	bufferDesc.dwFlags =  DSBCAPS_LOCDEFER | DSBCAPS_CTRL3D;
//#endif


	// Make the direct sound buffer
	LPDIRECTSOUNDBUFFER soundBuffer;
	HRESULT ret = directSound->CreateSoundBuffer(&bufferDesc, &soundBuffer, NULL);
	if (ret != DS_OK)
		return NULL;

	// Allocate the buffer
	MEM_SET_ONE_SHOT_NEW_NAME("sndstream");
	SoundStreamWin32 *sound = new SoundStreamWin32(*services, name, attr, loop, soundBuffer, size);
	return sound;
}

/***************************************************************************/
// Create an instance of a sound buffer
/***************************************************************************/
bool SoundDriverServices::DuplicateSoundBuffer(
LPDIRECTSOUNDBUFFER original,
LPDIRECTSOUNDBUFFER *dup)
{
	HRESULT ret = driver.directSound->DuplicateSoundBuffer(original, dup);
	//SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/***************************************************************************/
// Remove this sound when it has stopped playing
/***************************************************************************/
void SoundDriverServices::AutoReclaim(
SoundInstance *instance)
{
	// Duh
	ASSERT(instance);

	// Just add it to our list
	driver.autoReclaim.AddTail(instance);
	unsigned int timeout = driver.Time() + instance->PlayTime();
	if (timeout < driver.nextTimeout)
		driver.nextTimeout = timeout;
}











