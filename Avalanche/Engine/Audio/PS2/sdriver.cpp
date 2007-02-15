/***************************************************************************/
// Sound driver
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "platform/gameapp.h"
#include "EngineHelper/timer.h"
#include "Layers/mediahal.h"
#include "audio/ps2/sbankps2.h"
#include "GameHelper/Translator.h"

// Define this for some interesting sound stats.
#if defined(FOOD) && defined(_DEBUG)
#define SOUND_DEBUG
#endif

// CW doesnt like the attribute on these, but they seem to come out aligned anyway...
#if defined(PS2) && defined(SN)
#define ALIGN(x) __attribute__((aligned(x)))
#else
#define ALIGN(x)
#endif

// Here's how we map standard effects into PS2 space
SoundDriverPS2::EffectMapper SoundDriverPS2::effectMap[ENVEFFECTS] =
{
	{ SOUND_FX_OFF, 		0.0f, 0, 0 }, 	// NONE
	{ SOUND_FX_OFF, 		0.0f, 0, 0 }, 	// DEFAULT,
	{ SOUND_FX_OFF, 		0.0f, 0, 0 }, 	// PADDEDCELL,
	{ SOUND_FX_ROOM, 		0.2f, 0, 0 }, 	// ROOM,
	{ SOUND_FX_STUDIO_A, 0.8f, 0, 0 }, 	// BATHROOM,
	{ SOUND_FX_ROOM,		0.2f, 0, 0 }, 	// LIVINGROOM,
	{ SOUND_FX_SPACE,		0.2f, 0, 0 },	// STONEROOM,
	{ SOUND_FX_STUDIO_B,	1.0f, 0, 0 }, 	// AUDITORIUM,
	{ SOUND_FX_STUDIO_C, 1.0f, 0, 0 }, 	// CONCERTHALL,
	{ SOUND_FX_SPACE,   	0.4f, 0, 0 }, 	// CAVE,
	{ SOUND_FX_SPACE,		1.0f, 0, 0 }, 	// ARENA,
	{ SOUND_FX_HALL, 		0.6f, 0, 0 }, 	// HANGAR,
	{ SOUND_FX_OFF, 		0.0f, 0, 0 }, 	// CARPETEDHALLWAY,
	{ SOUND_FX_ROOM, 		0.1f, 0, 0 }, 	// HALLWAY,
	{ SOUND_FX_ECHO, 		0.2f, 32, 8 }, // STONECORRIDOR,
	{ SOUND_FX_ROOM,		0.1f, 0, 0 }, 	// ALLEY,
	{ SOUND_FX_OFF, 		0.0f, 0, 0 }, 	// FOREST,
	{ SOUND_FX_ROOM,		0.15f, 0, 0 },	// CITY,
	{ SOUND_FX_ECHO, 		0.2f, 255, 0 },// MOUNTAINS,
	{ SOUND_FX_ECHO, 		0.3f, 220, 1 },// QUARRY,
	{ SOUND_FX_OFF, 		0.0f, 0, 0 }, 	// PLAIN,
	{ SOUND_FX_OFF, 		0.0f, 0, 0 }, 	// PARKINGLOT,
	{ SOUND_FX_PIPE, 		0.8f, 0, 0 }, 	// SEWERPIPE,
	{ SOUND_FX_SPACE, 	0.8f, 0, 0 } 	// UNDERWATER
};

/***************************************************************************/
// All subclasses of sounddriver must write this function
// must be implemented it in SoundDriver:: namespace
/***************************************************************************/
SoundDriver *SoundDriver::CreateAndSetOutputFormat(
OSCREATEINFO osInfo)
{
	// Create the object
	SoundDriverPS2 *driver = new SoundDriverPS2;
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
SoundDriverPS2::SoundDriverPS2() :
autoReclaim(false)
{
	stereo = true;
	sampleRate = SAMPLE44KHz;

	biggestEffect = NONE;
	currentEffect = NONE;
	currentEffectDepth = -1;

	streamLibraryMounted = false;

	bankLibraryMounted = false;

	nextTimeout = 0xffffffff;
}

/***************************************************************************/
// Clean everything up
/***************************************************************************/
SoundDriverPS2::~SoundDriverPS2()
{
	Destroy();
}

/***************************************************************************/
// Pass in NULL for the SOUNDDRIVERID if you want to select the default
// sound device
/***************************************************************************/
bool SoundDriverPS2::Create(
OSCREATEINFO osInfo)
{
	// Do the fatty
	//TRACE("About to init multistream sound system.\n");
	bool ret = MultiStream::InitializeSoundSystem(MAX_SFX, MAX_STREAMS, MAX_SRAM_HANDLES);
	if (!ret)
	{
		BigFatError("Init", "Can't start MultiStream sound");
		return false;
	}

	// No errors we are clean
	SoundError::SetLastError(SE_OK);
	return true;
}

/***************************************************************************/
// Destroy it (destructor does the same thing)
/***************************************************************************/
bool SoundDriverPS2::Destroy()
{
	// is there really anything to do here?
	streamLibrary.UnmountAll();

	// Yeah!
	MultiStream::TerminateSoundSystem();

	return true;
}

/***************************************************************************/
// Initialize the output format, will return false with NORMAL priority
/***************************************************************************/
bool SoundDriverPS2::SetOutputFormat(
uint desiredSampleRate,
bool sixteenBit)
{
	// Make sure we've set the output mode
	SetOutputMode(outputMode);

	return true;
}

/***************************************************************************/
// Set the mode, returns false if not supported
/***************************************************************************/
bool SoundDriverPS2::SetOutputMode(
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
bool SoundDriverPS2::OutputModeSupported(
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
// Mount the stream libary
/***************************************************************************/
bool SoundDriverPS2::MountStreamLibrary()
{
	return MountStreamLibraries();
}

/***************************************************************************/
// What is the (approximate) max channels that can be simultaneously played?
/***************************************************************************/
int SoundDriverPS2::MaxChannels()
{
	// This depends on how many streams they have, but this is a good
	// estimate.
	return 45;
}

/***************************************************************************/
// What is the maximum number of streams that can be simultaneously played?
/***************************************************************************/
int SoundDriverPS2::Max3DStreams()
{
	return MAX_STREAMS - 2;
}

/***************************************************************************/
// The Daemon!
/***************************************************************************/
void SoundDriverPS2::Daemon()
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
				autoReclaim.Unlink(instance);
				Sound::AutoReclaimed(instance);
			}
			instance = next;
		}
	}

#ifdef SOUND_DEBUG
	// Dump some useful sound information
	static int frame = 0;
	if (++frame == 60)
	{
		DebugDump();
		frame = 0;
	}
#endif
}

/***************************************************************************/
// Create a sound from a memory image (image for this driver is a .vag file)
/***************************************************************************/
Sound *SoundDriverPS2::CreateSound(
SoundBank &bank,
void *data,
int size)
{
	// The bank knows all
	SoundBankPS2 &ps2Bank = (SoundBankPS2 &)bank;
	uint spuAddr = ps2Bank.GetSoundSPUAddress(data, size);
	if (!spuAddr)
		return NULL;

	// Get an SFXID
	SFXID id = MultiStream::AllocateSFXID(spuAddr);
	if (id == SFXID_INVALID)
	{
		ASSERTS(false, "Cannot allocate SFX ID.");
		SoundError::SetLastError(SE_IOPTRANSFERFAIL);
		return NULL;
	}

	// Create the sound
	SoundAttr attr(SoundAttr::VOLUMECONTROL | SoundAttr::PANCONTROL | SoundAttr::PITCHCONTROL, SAMPLE44KHz);
	VAGHeader *header = (VAGHeader *)data;
	attr.InferFromVAG(*header);
	Sound *sound = new Sound(*this, id, size, attr);
	ASSERT(sound);

#if defined(DUMPBANKS) && defined(FOOD) && defined(_DEBUG)
	TRACE("Sound 0x%x \"%s\" - id = %d, spuAddr = 0x%x, size = 0x%x (hdr size 0x%x)\n", sound, header->name, id, spuAddr, size, header->dataSize);
#endif

	// We are done!
	return sound;
}

/***************************************************************************/
/***************************************************************************/
const char *SoundDriverPS2::TokenizeStreamName(const char *streamFile)
{
	// Make sure this is mounted
	if (!MountStreamLibraries())
		return NULL;

	// Find the header file
	char justFile[32];
	JustFileName(streamFile, justFile);
	return streamLibrary.GetTokenPtr(justFile);
}

/***************************************************************************/
// Does this stream exist?
/***************************************************************************/
bool SoundDriverPS2::StreamExist(
const char *fileName)
{
	// Make sure this is mounted
	if (!MountStreamLibraries())
		return false;

	// Find the header file
	char justFile[32];
	JustFileName(fileName, justFile);
	StreamVolumeEntry stream;
	return streamLibrary.Find(justFile, stream);
}

/***************************************************************************/
// Stream a platform specific file off of the disk!
/***************************************************************************/
SoundStream *SoundDriverPS2::CreateStream(
const char *fileName,
bool repeat,
bool preLoad)
{
	// Make sure we are loaded
	if (!MountStreamLibraries())
		return NULL;

	// We don't support pre-loads!!
	ASSERT(preLoad == false);

	// fake the attr
	SoundAttr attr;
	attr.Set(SoundAttr::NORMAL);
	attr.SampleRate(SAMPLE44KHz);

	// Make the compltete filename
	char streamFile[128];
	PathFile::MakePathForceExt(streamFile, BaseStreamPath(), fileName, StreamFormat());

	// Find the header file
	char justFile[32];
	JustFileName(fileName, justFile);
	StreamVolumeEntry stream;
	if (!streamLibrary.Find(justFile, stream))
	{
		BigFatError(streamFile, "Can't find MIH!");
		return NULL;
	}

	// Not really required, but shows you info about the .MIH file.
	MIBStream::DisplayHeader(streamFile, stream.header);

	// This is bad!! These are obsolete
	if (stream.header.Pitch > 44100)
	{
		BigFatError(streamFile, "Whoa > 44k stream, see Jeff!!!!!!!");
		return NULL;
	}

	// Create it!
	MIBStream *mibStream = MultiStream::CreateStream(streamFile);
	if (!mibStream)
	{
		BigFatError(streamFile, "Cannot create MIBStream");
		return NULL;
	}

	// This the resource intenstive part
	if (!mibStream->Open(stream.header, stream.fileID, stream.offset, stream.size, repeat))
	{
		BigFatError(streamFile, "Cannot get resources for MIBStream");
		MultiStream::DestroyStream(mibStream);
		return NULL;
	}

	// Create the stream
	return new SoundStreamPS2(*this, fileName, attr, *mibStream, repeat);
}

/***************************************************************************/
// Pause all currently playing sounds
/***************************************************************************/
void SoundDriverPS2::Pause(PauseType pauseType)
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
void SoundDriverPS2::Unpause(PauseType pauseType)
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
// To ensure enough hardware resources for a game world, you
// must inform the driver about all the effects you might use on a world.
// You don't have to use this.
/***************************************************************************/
void SoundDriverPS2::BeginEnvironmentEffectProfile()
{
	biggestEffect = DEFAULT;
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverPS2::EnvironmentEffectProfile(
ENVEFFECT effect)
{
	// How big is this effect?
	int size = MultiStream::GetEffectSize(effectMap[effect].mode);

	// Bigger?
	if (size > MultiStream::GetEffectSize(effectMap[biggestEffect].mode))
		biggestEffect = effect;
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverPS2::EndEnvironmentEffectProfile()
{
	// Allocate it
	MultiStream::PrepareEffect(effectMap[biggestEffect].mode, true);
}

/***************************************************************************/
// Setup an environmental effect
/***************************************************************************/
bool SoundDriverPS2::SetEnvironmentEffect(
ENVEFFECT effect,
float volume)
{
	// Alias this
	EffectMapper &e = effectMap[effect];

	// First allocate memory
	if (!MultiStream::PrepareEffect(e.mode, false))
		e = effectMap[0];

	// Compute the real depth
	int depth = Math::Clamp((int)(volume * e.depth * (float)0x7fff), 0, 0x7fff);

	// Set the current effect volume
	bool ret = true;
	if (effect != currentEffect)
	{
		ret = MultiStream::EnableEffect(e.mode, depth, depth, e.delay, e.feedback);
		currentEffect = effect;
	}

	// Set the volume?
	if (depth != currentEffectDepth)
	{
		MultiStream::SetEffectVolume(depth, depth);
		currentEffectDepth = depth;
	}

	// We are done!
	return ret;
}

/***************************************************************************/
// Special support for the FMV
/***************************************************************************/
void SoundDriverPS2::PrepareForFMV()
{
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverPS2::RecoverFromFMV()
{
}

/***************************************************************************/
// Remove this sound when it has stopped playing
/***************************************************************************/
void SoundDriverPS2::AutoReclaim(
SoundInstance *instance)
{
	// Duh
	ASSERT(instance);

	// Just add it to our list
	autoReclaim.AddTail(instance);
	uint timeout = Time() + instance->PlayTime();
	if (timeout < nextTimeout)
		nextTimeout = timeout;
}

/***************************************************************************/
// Subclass these to use your own subclassed bank
/***************************************************************************/
SoundBank *SoundDriverPS2::CreateBank()
{
	// Doh!
	if (!bankLibraryMounted)
		bankLibraryMounted = bankLibrary.Mount();

	memSetUseBit(MEM_AUDIO_DATA);
	SoundBank *bank = new SoundBankPS2(*this);
	memClearUseBit(MEM_AUDIO_DATA);

	// This is a hack for now, but make sure no streams are playing
	if (current3DStreams > 0)
	{
		SoundStream3D *stream3D = soundStream3D.Head();
		while (stream3D)
		{
			// Does this channel really have a stream?
			if (!stream3D->IsLost())
				stream3D->Release();

			// Check the next stream
			stream3D = stream3D->next;
		}
	}


	return bank;
}

/***************************************************************************/
/***************************************************************************/
bool SoundDriverPS2::MountStreamLibraries()
{
	// Already done?
	if (streamLibraryMounted)
		return true;
	streamLibraryMounted = true;

	// Load the stream libraries (we don't care about errors)
	TRACE("About to mount stereo stream library.\n");
	streamLibrary.Mount("stereo");
 	TRACE("About to mount common stream library.\n");
	streamLibrary.Mount("common");
 	TRACE("About to mount language stream library.\n");
	streamLibrary.Mount(g_translator.GetLanguage());

	return true;
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverPS2::JustFileName(
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
// Big fat error handler
/***************************************************************************/
void SoundDriverPS2::BigFatError(
const char *fileName,
const char *description)
{
	TRACE("*******************************************************************\n");
	TRACE("*******************************************************************\n");
	TRACE("*******************************************************************\n");
	TRACE("Sound Driver Error:: file \"");
	TRACE(fileName);
	TRACE("\" --> \"");
	TRACE(description);
	TRACE("!!!\"\n");
	TRACE("*******************************************************************\n");
	TRACE("*******************************************************************\n");
	TRACE("*******************************************************************\n");
	ASSERT(false);
}

/***************************************************************************/
// Dump some useful sound information
/***************************************************************************/
void SoundDriverPS2::DebugDump()
{
	// This is most of the info
	MultiStream::DebugDump();

	// Print out the banks
	TRACE("Current sound bank stack (head first) %d banks:\n", bankStack.Count());
	for (int i = 0; i < bankStack.Count(); i++)
	{
		SoundBank *bank = (SoundBank *)bankStack[i];
		TRACE("\tSound bank %d has %d sources\n", i, bank->Sources());
	}
}

