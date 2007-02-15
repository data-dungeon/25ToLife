/***************************************************************************/
// Sound driver
/***************************************************************************/
#ifndef SDRIVER_H
#define SDRIVER_H

#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#include "container/dllist.h"
#include "Audio/sounddriver.h"
#include "Audio/soundattr.h"
#include "Audio/Xbox/sounderror.h"
#include "Audio/Xbox/xbmixbin.h"
#include "Audio/Xbox/streamlib.h"

class Sound;
class SoundStreamXbox;
class SoundInstance;
class SoundDriverServices;
class SoundDriverCaps;

// Define this for 5.1 support
#define DOLBY51

/***************************************************************************/
// The sound driver class.
//
// Errors are returned via the sounderror class.  If you get a false out of
// one of these calls, use the GetLastError call to see what happened.
/***************************************************************************/
class SoundDriverXbox : public SoundDriver
{
public:
	// The usual
	SoundDriverXbox();
	virtual ~SoundDriverXbox();

	// The game's available sound priorities
	typedef enum
	{
		NORMAL,					// Shared with other applications, no format control
		PRIORITY,				// Format control, but with sharing
		EXCLUSIVE				// Total control with application focus
	} SOUNDPRIORITY;
	bool SetPriority(SOUNDPRIORITY priority);

		// Create it
	bool Create(OSCREATEINFO osInfo);

	// Destroy it (destructor does the same thing)
	bool Destroy();

	// Initialize the output format, will return false with NORMAL priority
	bool SetOutputFormat(unsigned int sampleRate, bool sixteenBit = true);

	// Set the mode, returns false if not supported
	bool SetOutputMode(OUTPUTMODE mode);

	// Test to see if this mode is supported
	bool OutputModeSupported(OUTPUTMODE mode);

	// What is the (approximate) max channels that can be simultaneously played?
	int MaxChannels();

	// What is the maximum number of streams that can be simultaneously played?
	int Max3DStreams();

	// Call this frequently.  Threads are nice, but messy
	void Daemon();

	// Create a sound from a memory image (image for this driver is a .eat file,
	// that has a WFX followed by raw sound data
	Sound *CreateSound(SoundBank &bank, void *data, int size);

	// We do have tables of all streams, so this actually works!
	const char *TokenizeStreamName(const char *streamFile);
	void ReleaseStreamNameToken(const char *streamToken) {}

	// Does this stream exist?
	bool StreamExist(const char *fileName);

	// Stream a platform specific file off of the disk!
	// DO NOT PUT AN EXTENSION ON THIS NAME!
	SoundStream *CreateStream(const char *fileName, bool repeat, bool preLoad = false);

	// Stream a soundtrack file
	SoundStream *CreateStream(DWORD songID, bool repeat);

	// Pause all currently playing sounds
	void Pause(PauseType pauseType);

	// Resume all currently paused sounds
	void Unpause(PauseType pauseType);

	// Setup environmental effects
	bool SetEnvironmentEffect(ENVEFFECT effect, float volume);

	// How many soundtracks are there?
	int SoundTracks();

	// Get the soundtrack information
	const SOUNDTRACK *GetSoundTrack(int index);

	// What is the extension for sounds
	const char *FileFormat()
		{ return ".eat"; }

	// What is the default stream extension
	const char *StreamFormat()
		{ return ".wma"; }

	// What is our base path for sound?
	const char *BaseDataPath()
		{ return "sound\\"; }

	// What is the base path for streams?
	const char *BaseStreamPath()
		{ return "stream\\"; }

protected:
	// Subclass these to use your own subclassed bank
	SoundBank *CreateBank();

	SoundDriverServices				*services;		// Services (internal interface)
	LPDIRECTSOUND8						directSound;	// The direct sound object

	// We must manage reclaimed sounds
	DoublyLinkedList<SoundInstance>	autoReclaim;
	unsigned int							nextTimeout;
	inline unsigned int Now()
		{ return (unsigned int)timeGetTime(); }

	// Our stream library!
	StreamLibrary						streamLibrary;
	bool 									streamLibraryMounted;
	bool MountStreamLibraries();
	void JustFileName(const char *pathFile, char *filename);

	// Our soundtrack support stuff
	void EnumSoundTracks();
	void AddSoundTrack(XSOUNDTRACK_DATA &xSoundTrack);
	void TranslateString(const WCHAR *input, char *output, int outputMax);
	DynamicArray<SOUNDTRACK>			soundTrackList;

	// Downloads an effects image to the DSP
	float effectsVolume;
	bool DownloadEffectsImage(const char *dspEffectsFile);

	// Some default mixbin setup
#ifdef DOLBY51
	XBMixBin51				mixBin;
#else
	XBMixBinStereo			mixBin;
#endif

	// Thread stuff for streaming
	HANDLE										streamThreadHandle;
	CRITICAL_SECTION 							streamThreadCriticalSection;
	bool											streamThreadSignalExit;
	DoublyLinkedList<SoundStreamXbox>	soundStream;
	bool UpdateStreams();
	static DWORD WINAPI StreamThreadProc(LPVOID parameter);

	// We have a few friends
	friend class SoundDriverServices;
	friend class SoundDriverCaps;
};

/***************************************************************************/
// Services for the Sound class.  This basically is a private interface to
// the innards to the SoundDriver
/***************************************************************************/
class SoundDriverServices
{
public:
	// Create a sound instance (could return NULL if we are out)
	// Caller is responsible to for Release()
	IDirectSoundBuffer8 *CreateSoundInstance(void *bankData, unsigned int bankSize,
															unsigned int playOffset, unsigned int playLength,
															SoundAttr &attr, WAVEFORMATEX &wfx);

	// Create a streaming sound buffer (will return NULL on failure, see GetLastError)
	// Caller is responsible to for Release() on stream
	IDirectSoundStream *CreateStream(DSSTREAMDESC &desc, bool monoSource = true);

	// Remove this sound when it has stopped playing
	void AutoReclaim(SoundInstance *instance);

	// Register a stream for callbacks
	void RegisterStream(SoundStreamXbox *stream);
	void UnregisterStream(SoundStreamXbox *stream);

	// Set volumes
	bool ApplyMixBins3D(IDirectSoundBuffer8 &voice, float volume, float pan, float wet, float dry);
	bool ApplyMixBins2D(IDirectSoundBuffer8 &voice, float volume, float wet, float dry);
	bool ApplyMixBins3D(IDirectSoundStream &stream, float volume, float pan, float wet, float dry);
	bool ApplyMixBins2D(IDirectSoundStream &stream, float volume, float wet, float dry, bool monoSource);

	// What time is it?
	inline unsigned int Time()
		{ return driver.Now(); }

private:
	// Only our friends can create us (SoundDriver)
	SoundDriverServices(SoundDriverXbox &parent) : driver(parent) {}
	~SoundDriverServices() {}
	friend class SoundDriverXbox;

	// Our internals
	SoundDriverXbox		&driver;
};

#endif


