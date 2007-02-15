/***************************************************************************/
// Sound driver
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "Audio/Xbox/wmastream.h"
#include "dsstdfx.h"
#include "Layers/DiscErrorHandler/DiscError.h"
#include "Audio/Xbox/sbankxb.h"
#include "GameHelper/Translator.h"

/***************************************************************************/
// All subclasses of sounddriver must write this function, it is not
// implemented here (you implement it in SoundDriver:: namespace)
/***************************************************************************/
SoundDriver *SoundDriver::CreateAndSetOutputFormat(
OSCREATEINFO osInfo)
{
	// Track problems down noW!
#ifdef _DEBUG
	g_dwDirectSoundDebugLevel = 3;
	//g_dwDirectSoundDebugBreakLevel = 3;
#endif

	// Create the object
	SoundDriverXbox *driver = new SoundDriverXbox;
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
SoundDriverXbox::SoundDriverXbox() :
autoReclaim(false),
mixBin(effectsVolume)
{
	services = NULL;
	directSound = NULL;
	nextTimeout = 0xffffffff;
	streamLibraryMounted = false;
	effectsVolume = 0.0f;

	// Use a thread to do this work
	streamThreadSignalExit = false;
	InitializeCriticalSection(&streamThreadCriticalSection);

	const int stackSize = 64 * 1024;
	const DWORD creationFlags = 0;
	streamThreadHandle = CreateThread(NULL, stackSize, StreamThreadProc, this, creationFlags, NULL);
	ASSERT(streamThreadHandle != NULL);

	int currentPrio = GetThreadPriority(GetCurrentThread());
	SetThreadPriority(streamThreadHandle, currentPrio + 1);
}

/***************************************************************************/
// Clean everything up
/***************************************************************************/
SoundDriverXbox::~SoundDriverXbox()
{
	// Shut down the thread
	if (streamThreadHandle)
	{
		streamThreadSignalExit = true;
		DWORD exitCode = 0;
		while (1)
		{
			BOOL ret = GetExitCodeThread(streamThreadHandle, &exitCode);
			if (ret && exitCode != 0)
				Sleep(100);
		}
	}

	DeleteCriticalSection(&streamThreadCriticalSection);
}

/***************************************************************************/
// Pass in NULL for the SOUNDDRIVERID if you want to select the default
// sound device
/***************************************************************************/
bool SoundDriverXbox::Create(
SoundDriverXbox::OSCREATEINFO osInfo)
{
	// Do the create
	HRESULT ret = DirectSoundCreate(NULL, &directSound, NULL);
	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return false;

	// Create our services
	services = new SoundDriverServices(*this);
	if (!services)
		return false;

	// Download the effects image
	if (!DownloadEffectsImage("dsstdfx"))
		return false;

	// somehow we succeded!
	return true;
}

/***************************************************************************/
// Destroy it (destructor does the same thing)
/***************************************************************************/
bool SoundDriverXbox::Destroy()
{
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

	// is there really anything to do here?
	streamLibrary.UnmountAll();

	return true;
}

/***************************************************************************/
// Initialize the output format, will return false with NORMAL priority
/***************************************************************************/
bool SoundDriverXbox::SetOutputFormat(
unsigned int desiredSampleRate,
bool sixteenBit)
{
	// Basically they can't do anything about this on the xbox
	return true;
}

/***************************************************************************/
// Set the mode, returns false if not supported
/***************************************************************************/
bool SoundDriverXbox::SetOutputMode(
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
bool SoundDriverXbox::OutputModeSupported(
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
int SoundDriverXbox::MaxChannels()
{
	// This depends on how many streams they have, but this is a good
	// estimate.
	return 45;
}

/***************************************************************************/
// What is the maximum number of streams that can be simultaneously played?
/***************************************************************************/
int SoundDriverXbox::Max3DStreams()
{
	return 10;
}

/***************************************************************************/
// The Daemon!
/***************************************************************************/
void SoundDriverXbox::Daemon()
{
	// Call the parent
	SoundDriver::Daemon();

	// Time to do work?
	if (Now() >= nextTimeout)
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
Sound *SoundDriverXbox::CreateSound(
SoundBank &bank,
void *data,
int size)
{
	ASSERT(data != NULL);
	ASSERT(size > 0);

	// The bank is going to do the hard work here
	SoundBankXbox &xBank = (SoundBankXbox &)bank;

	// Get the info
	WAVEFORMATEX *wfx;
	void *bankData;
	unsigned int bankSize, playOffset, playLength;
	bool ret = xBank.GetSoundCreateInfo(data, wfx, bankData, bankSize, playOffset, playLength);
	if (!ret)
		return NULL;

	// This is paranoid
	ASSERT(memcmp((char *)data + sizeof(WAVEFORMATEX), (char *)bankData + playOffset, size - sizeof(WAVEFORMATEX)) == 0);

	// Create it
	ASSERT(services);
	return new Sound(*services, bankData, bankSize, playOffset, playLength, *wfx);
}

/***************************************************************************/
/***************************************************************************/
const char *SoundDriverXbox::TokenizeStreamName(const char *streamFile)
{
	// Make sure this is mounted
	if (!MountStreamLibraries())
		return NULL;

	// Find the header file
	char trueFileName[64];
	JustFileName(streamFile, trueFileName);
	strcat(trueFileName, ".wma");
	return streamLibrary.GetTokenPtr(trueFileName);
}

/***************************************************************************/
// Does this stream exist?
/***************************************************************************/
bool SoundDriverXbox::StreamExist(
const char *fileName)
{
	// Make sure this is mounted
	if (!MountStreamLibraries())
		return false;

	// Find the header file
	char trueFileName[64];
	JustFileName(fileName, trueFileName);
	strcat(trueFileName, ".wma");
	return streamLibrary.Find(trueFileName);
}

/***************************************************************************/
// Stream a wave file off of the disk!
/***************************************************************************/
SoundStream *SoundDriverXbox::CreateStream(
const char *fileName,
bool repeat,
bool preLoad)
{
	// Make sure this is mounted
	if (!MountStreamLibraries())
		return NULL;

	// Find the header file
	char justFile[32];
	JustFileName(fileName, justFile);

	char trueFileName[64];
	strcpy(trueFileName, justFile);
	strcat(trueFileName, ".wma");
	ImbeddedStreamInfo info;
	if (!streamLibrary.Find(trueFileName, &info))
		return false;

	// Create it!
	return new WMAStream(*services, info, justFile, repeat);
}

/***************************************************************************/
// Stream a soundtrack file
/***************************************************************************/
SoundStream *SoundDriverXbox::CreateStream(
SONGID songID,
bool repeat)
{
	// Create it!
	return new WMAStream(*services, (DWORD)songID, repeat);
}

/***************************************************************************/
// Pause all currently playing sounds
/***************************************************************************/
void SoundDriverXbox::Pause(PauseType pauseType)
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
void SoundDriverXbox::Unpause(PauseType pauseType)
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
// equivalent for the platform is chosen.
/***************************************************************************/
bool SoundDriverXbox::SetEnvironmentEffect(
ENVEFFECT effect, float volume)
{
	// Map the effect
	static DSI3DL2LISTENER effectMap[] =
	{
		{ DSI3DL2_ENVIRONMENT_PRESET_NOREVERB },
		{ DSI3DL2_ENVIRONMENT_PRESET_DEFAULT },
		{ DSI3DL2_ENVIRONMENT_PRESET_PADDEDCELL },
		{ DSI3DL2_ENVIRONMENT_PRESET_ROOM },
		{ DSI3DL2_ENVIRONMENT_PRESET_BATHROOM },
		{ DSI3DL2_ENVIRONMENT_PRESET_LIVINGROOM },
		{ DSI3DL2_ENVIRONMENT_PRESET_STONEROOM },
		{ DSI3DL2_ENVIRONMENT_PRESET_AUDITORIUM },
		{ DSI3DL2_ENVIRONMENT_PRESET_CONCERTHALL },
		{ DSI3DL2_ENVIRONMENT_PRESET_CAVE },
		{ DSI3DL2_ENVIRONMENT_PRESET_ARENA },
		{ DSI3DL2_ENVIRONMENT_PRESET_HANGAR },
		{ DSI3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY },
		{ DSI3DL2_ENVIRONMENT_PRESET_HALLWAY },
		{ DSI3DL2_ENVIRONMENT_PRESET_STONECORRIDOR },
		{ DSI3DL2_ENVIRONMENT_PRESET_ALLEY },
		{ DSI3DL2_ENVIRONMENT_PRESET_FOREST },
		{ DSI3DL2_ENVIRONMENT_PRESET_CITY },
		{ DSI3DL2_ENVIRONMENT_PRESET_MOUNTAINS },
		{ DSI3DL2_ENVIRONMENT_PRESET_QUARRY },
		{ DSI3DL2_ENVIRONMENT_PRESET_PLAIN },
		{ DSI3DL2_ENVIRONMENT_PRESET_PARKINGLOT },
		{ DSI3DL2_ENVIRONMENT_PRESET_SEWERPIPE },
		{ DSI3DL2_ENVIRONMENT_PRESET_UNDERWATER }
	};

	effectsVolume = volume * 0.5f;
	HRESULT ret = directSound->SetI3DL2Listener(&effectMap[effect], DS3D_IMMEDIATE);
	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return false;

	return true;
}

/***************************************************************************/
// How many soundtracks are there?
/***************************************************************************/
int SoundDriverXbox::SoundTracks()
{
	// Have we enum'ed yet?
	if (soundTrackList.Count() == 0)
		EnumSoundTracks();

	// Return the count
	return soundTrackList.Count();
}

/***************************************************************************/
// Get the soundtrack information
/***************************************************************************/
const SoundDriver::SOUNDTRACK *SoundDriverXbox::GetSoundTrack(
int index)
{
	// Have we enum'ed yet?
	if (soundTrackList.Count() == 0)
		EnumSoundTracks();

	// Are they in range
	if (index < 0 || index >= (int)soundTrackList.Count())
		return NULL;

	// Sock it to 'em
	return soundTrackList.Get(index);
}

/***************************************************************************/
// Subclass these to use your own subclassed bank
/***************************************************************************/
SoundBank *SoundDriverXbox::CreateBank()
{
	memSetUseBit(MEM_AUDIO_DATA);
	SoundBank *bank = new SoundBankXbox(*this);
	memClearUseBit(MEM_AUDIO_DATA);
	return bank;
}

/***************************************************************************/
/***************************************************************************/
bool SoundDriverXbox::MountStreamLibraries()
{
	// Already done?
	if (streamLibraryMounted)
		return true;
	streamLibraryMounted = true;

	// Load the stream libraries (we don't care about errors)
 	TRACE("About to mount common library.\n");
	streamLibrary.Mount("common");
 	TRACE("About to mount language library.\n");
	streamLibrary.Mount(g_translator.GetLanguage());
	TRACE("About to mount music library.\n");
	streamLibrary.Mount("music");

	return true;
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverXbox::JustFileName(
const char *pathFile,
char *file)
{
	// Save the length
	int len = strlen(pathFile) - 1;

	// Find the beginning of the name, or a slash
	while (len > 0 && pathFile[len] != '\\')
		--len;
	if (pathFile[len] == '\\')
		++len;

	// Now copy to the dot or the end
	int fileLen = 0;
	while (pathFile[len] != '\0' && pathFile[len] != '.')
		file[fileLen++] = pathFile[len++];
	file[fileLen] = '\0';
}

/***************************************************************************/
// Enum sound tracks
/***************************************************************************/
void SoundDriverXbox::EnumSoundTracks()
{
	// Get the first soundtrack
	XSOUNDTRACK_DATA xSoundTrack;
	HANDLE hEnum = XFindFirstSoundtrack(&xSoundTrack);
	if (hEnum == INVALID_HANDLE_VALUE)
		return;
	AddSoundTrack(xSoundTrack);

	// Loop through the rest
	while (XFindNextSoundtrack(hEnum, &xSoundTrack))
		AddSoundTrack(xSoundTrack);

	// We are done
	XFindClose(hEnum);
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverXbox::AddSoundTrack(
XSOUNDTRACK_DATA &xSoundTrack)
{
	if (!xSoundTrack.uSongCount)
		return;

	// Fill out the sound track record
	SOUNDTRACK soundTrack;
	memset(&soundTrack, '\0', sizeof(soundTrack));
	TranslateString(xSoundTrack.szName, soundTrack.name, sizeof(soundTrack.name) - 1);
	soundTrack.length = (int)xSoundTrack.uSoundtrackLength;
	soundTrack.songs = (int)xSoundTrack.uSongCount;
	soundTrack.song = new SONG[soundTrack.songs];
	ASSERT(soundTrack.song);
	if (!soundTrack.song)
		return;

	// Clear the array
	memset(soundTrack.song, '\0', sizeof(SONG) * soundTrack.songs);

	// Copy in the songs
	int onSong = 0;
	for (int i = 0; i < soundTrack.songs; i++)
	{
		SONG &song = soundTrack.song[onSong];

		DWORD length;
		DWORD songID;
		WCHAR songName[64];
		if (!XGetSoundtrackSongInfo(xSoundTrack.uSoundtrackId, (UINT)i, &songID, &length, songName, sizeof(songName) - 1))
			continue;
		TranslateString(songName, song.name, sizeof(song.name) - 1);
		song.id = (SoundDriver::SONGID)songID;
		song.length = (int)length;

		++onSong;
	}

	// Add it to the list
	if (onSong > 0)
		soundTrackList.Add(soundTrack);
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverXbox::TranslateString(
const WCHAR *input,
char *output,
int outputMax)
{
	if (outputMax == 0)
		return;

	int len = 0;
	while (input[len] != L'\0' && len < outputMax - 1)
	{
		output[len] = (char)input[len];
		len++;
	}
	output[len] = '\0';
}

/***************************************************************************/
// Downloads an effects image to the DSP
/***************************************************************************/
bool SoundDriverXbox::DownloadEffectsImage(
const char *dspEffectsFile)
{
	// Make a filename
	char pathFile[128];
	PathFile::MakePathForceExt(pathFile, BaseDataPath(), dspEffectsFile, "bin");
	//char *trueFile = Media::GetFileName( "dsstdfx.bin" );
	char *trueFile = "d:\\data_xbox\\sound\\dsstdfx.bin"; //pab - todo	char *trueFile = medGetFileName(pathFile);

	// open scratch image file generated by xps2 tool
	HANDLE file = CreateFile(trueFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
#if defined (CDROM)
		DiscErrorHandler::DisplayError(trueFile, DiscErrorHandler::READ_FILE_FAILED);		// error warning for failed load
#endif
		return false;
	}

	// Read in the image
	unsigned int size = SetFilePointer(file, 0, NULL, FILE_END);
	SetFilePointer(file, 0, NULL, FILE_BEGIN);

	// Allocate memory to read the scratch image from disk
	char *buffer = new char [size];

	// Read the image in
	DWORD bytesRead;
	BOOL result = ReadFile(file, buffer, size, &bytesRead, 0);
	if (!result)
	{
		delete [] buffer;

#if defined (CDROM)
		DiscErrorHandler::DisplayError(trueFile, DiscErrorHandler::READ_FILE_FAILED);		// error warning for failed load
#endif

		return false;
	}

	// call dsound api to download the image..
	DSEFFECTIMAGELOC effectLoc;
	memset(&effectLoc, '\0', sizeof(effectLoc));
	effectLoc.dwI3DL2ReverbIndex = GraphI3DL2_I3DL2Reverb;
	effectLoc.dwCrosstalkIndex = GraphXTalk_XTalk;
	LPDSEFFECTIMAGEDESC desc;
	HRESULT ret = directSound->DownloadEffectsImage(buffer, size, &effectLoc, &desc);
	SoundError::SetLastError(ret);
	if (ret != DS_OK)
		return false;

	// We're outta here
	delete [] buffer;
	CloseHandle(file);

	return (ret == S_OK);
}

/***************************************************************************/
// Processing streams
/***************************************************************************/
bool SoundDriverXbox::UpdateStreams()
{
	EnterCriticalSection(&streamThreadCriticalSection);

	// Process all the registered streams
	bool haveStreams = false;
	SoundStreamXbox *stream = soundStream.Head();
	while (stream)
	{
		haveStreams = true;
		stream->Process();
		stream = stream->next;
	}

	LeaveCriticalSection(&streamThreadCriticalSection);

	return haveStreams;
}

/***************************************************************************/
// Create an instance of a sound buffer
/***************************************************************************/
IDirectSoundBuffer8 *SoundDriverServices::CreateSoundInstance(
void *bankData,
unsigned int bankSize,
unsigned int playOffset,
unsigned int playLength,
SoundAttr &attr,
WAVEFORMATEX &wfx)
{
	// make ADPCM struct
	XBOXADPCMWAVEFORMAT	adpcmwf;

	memcpy(&adpcmwf.wfx, &wfx, sizeof (WAVEFORMATEX));
	adpcmwf.wSamplesPerBlock = 64;
	adpcmwf.wfx.cbSize = sizeof(adpcmwf) - sizeof(adpcmwf.wfx);

	// fill out the buffer description
	static DSBUFFERDESC desc = { 0 };
	desc.dwSize = sizeof(desc);
	desc.dwFlags = attr.DSBufferDescFlags();// | DSBCAPS_LOCDEFER;
	desc.lpwfxFormat = (LPWAVEFORMATEX)&adpcmwf;

	// What mixbins do they want?
	desc.lpMixBins = &driver.mixBin.ForMonoSource();

	// Make the direct sound buffer
	IDirectSoundBuffer8 *sound;
	HRESULT ret = DirectSoundCreateBuffer(&desc, &sound);

	// Did we fail?  This is very bad
	if (FAILED(ret))
	{
		SoundError::SetLastError(ret);
		return NULL;
	}

	// Set the data
	ret = sound->SetBufferData(bankData, bankSize);
	SoundError::SetLastError(ret);
	if (FAILED(ret))
	{
		sound->Release();
		return NULL;
	}

	// Set the play offset
	ret = sound->SetPlayRegion(playOffset, playLength);
	SoundError::SetLastError(ret);
	if (FAILED(ret))
	{
		sound->Release();
		return NULL;
	}

	// Make sure they don't screw with us
	sound->SetHeadroom(0);

	// weee!
	return sound;
}

/***************************************************************************/
// Create a streaming sound buffer (will return NULL on failure,
// see GetLastError).
// Caller is responsible to for Release() on stream
/***************************************************************************/
IDirectSoundStream *SoundDriverServices::CreateStream(
DSSTREAMDESC &desc,
bool monoSource)
{
	// Setup the mixbin
	if (monoSource)
		desc.lpMixBins = &driver.mixBin.ForMonoSource();
	else
		desc.lpMixBins = &driver.mixBin.ForStereoSource();

	// Create the stream
	IDirectSoundStream *stream;
 	HRESULT ret = DirectSoundCreateStream(&desc, &stream);
	SoundError::SetLastError(ret);

	// Did we fail?
	if (FAILED(ret))
		return NULL;

	// Make sure they don't screw with us
	stream->SetHeadroom(0);

	return stream;
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
	unsigned int timeout = driver.Now() + instance->PlayTime();
	if (timeout < driver.nextTimeout)
		driver.nextTimeout = timeout;
}

/***************************************************************************/
// Register a stream for callbacks
/***************************************************************************/
void SoundDriverServices::RegisterStream(
SoundStreamXbox *stream)
{
	ASSERT(stream);
	EnterCriticalSection(&driver.streamThreadCriticalSection);
	driver.soundStream.AddTail(stream);
	LeaveCriticalSection(&driver.streamThreadCriticalSection);
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverServices::UnregisterStream(
SoundStreamXbox *stream)
{
	ASSERT(stream);
	EnterCriticalSection(&driver.streamThreadCriticalSection);
	driver.soundStream.Unlink(stream);
	LeaveCriticalSection(&driver.streamThreadCriticalSection);
}

/***************************************************************************/
// The thread itself
/***************************************************************************/
DWORD WINAPI SoundDriverXbox::StreamThreadProc(
LPVOID parameter)
{
	SoundDriverXbox *me = (SoundDriverXbox *)parameter;
	while (!me->streamThreadSignalExit)
	{
		// Do the actual work!
		bool haveStreams = me->UpdateStreams();

		// Tricky...is this thread safe?
		DirectSoundDoWork();

		// Try not to hog
		if (haveStreams)
			Sleep(33);
		else
			Sleep(200);
	}

	// If we are here, kill everything
	return 1;
}

/***************************************************************************/
// Set volumes
/***************************************************************************/
bool SoundDriverServices::ApplyMixBins3D(
IDirectSoundBuffer8 &voice,
float volume,
float pan,
float wet,
float dry)
{
	HRESULT ret = voice.SetVolume(SoundVolume::UnitToHectodB(volume));
	SoundError::SetLastError(ret);

	ret = voice.SetMixBinVolumes(&driver.mixBin.Get(volume, pan, wet, dry));
	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/***************************************************************************/
/***************************************************************************/
bool SoundDriverServices::ApplyMixBins2D(
IDirectSoundBuffer8 &voice,
float volume,
float wet,
float dry)
{
	HRESULT ret = voice.SetVolume(SoundVolume::UnitToHectodB(volume));
	SoundError::SetLastError(ret);

	ret = voice.SetMixBinVolumes(&driver.mixBin.Get(volume, wet, dry));
	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/***************************************************************************/
/***************************************************************************/
bool SoundDriverServices::ApplyMixBins3D(
IDirectSoundStream &stream,
float volume,
float pan,
float wet,
float dry)
{
	HRESULT ret = stream.SetVolume(SoundVolume::UnitToHectodB(volume));
	SoundError::SetLastError(ret);

	ret = stream.SetMixBinVolumes(&driver.mixBin.Get(volume, pan, wet, dry));
	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/***************************************************************************/
/***************************************************************************/
bool SoundDriverServices::ApplyMixBins2D(
IDirectSoundStream &stream,
float volume,
float wet,
float dry,
bool monoSource)
{
	HRESULT ret = stream.SetVolume(SoundVolume::UnitToHectodB(volume));
	SoundError::SetLastError(ret);

	ret = stream.SetMixBinVolumes(&driver.mixBin.Get(volume, wet, dry, monoSource));
	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}












