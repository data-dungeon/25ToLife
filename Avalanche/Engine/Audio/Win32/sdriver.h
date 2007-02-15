/***************************************************************************/
// Sound driver
/***************************************************************************/
#ifndef SDRIVER_H
#define SDRIVER_H

#include "container/dllist.h"
#include "Audio/Win32/sdriverenum.h"
#include "Audio/Win32/sounderror.h"
#include "Audio/soundattr.h"
#include "Audio/sounddriver.h"

class Sound;
class SoundStream3D;
class SoundStreamWin32;
class SoundInstance;
class SoundDriverServices;
class SoundDriverCaps;

/***************************************************************************/
// The sound driver class.  Pass in NULL for the SOUNDDRIVERID if you
// want to select the default sound device
//
// Errors are returned via the dserror class.  If you get a false out of
// one of these calls, use the GetLastError call to see what happened.
/***************************************************************************/
class SoundDriverWin32 : public SoundDriver
{
public:
	// The usual
	SoundDriverWin32();
	virtual ~SoundDriverWin32();

	// Create the sound driver, has platform specific parameters
	// This is called by CreateAndSetOutputFormat
	bool Create(OSCREATEINFO osInfo);

	// Pass in NULL for the SOUNDDRIVERID if you want to select the default
	// sound device
	bool Create(HWND hWnd, SoundDriverEnumerator::SOUNDDRIVERID deviceID);

	// The game's available sound priorities
	typedef enum
	{
		NORMAL,					// Shared with other applications, no format control
		PRIORITY,				// Format control, but with sharing
		EXCLUSIVE				// Total control with application focus
	} SOUNDPRIORITY;
	bool SetPriority(SOUNDPRIORITY priority);

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

	// Does this stream exist?
	bool StreamExist(const char *fileName);

	// Stream a platform specific file off of the disk!
	// DO NOT PUT AN EXTENSION ON THIS NAME!
	SoundStream *CreateStream(const char *fileName, bool repeat, bool preLoad = false);

	// Pause all currently playing sounds
	void Pause(PauseType pauseType);

	// Resume all currently paused sounds
	void Unpause(PauseType pauseType);

	// Setup environmental effects
	bool SetEnvironmentEffect(ENVEFFECT effect, float volume)
		{ return false; }

	// What is the extension for sounds
	const char *FileFormat()
		{ return ".eat"; }

	// What is the default stream extension
	const char *StreamFormat()
#ifdef DIRECTX_PC
		{ return ".emt"; }
#else
		{ return ".wav"; }
#endif

	// What is our base path for sound?
	const char *BaseDataPath()
		{ return "sound\\"; }

	// What is the base path for streams?
	const char *BaseStreamPath()
		{ return "stream\\"; }

protected:
	SoundDriverServices				*services;		// Services (internal interface)
	HWND									hWndOwner;		// Our owning window
#ifdef DIRECTX_PC
	LPDIRECTSOUND8						directSound;	// The direct sound object
	bool								usingEAX;
#else
	LPDIRECTSOUND						directSound;	// The direct sound object
#endif
	SOUNDPRIORITY						priority;		// Our sound driver's system priority
	LPDIRECTSOUNDBUFFER 				primaryBuffer;	// primary buffer
	bool									stereo;			// Are we in stereo?
	unsigned int						sampleRate;

	// Pick the best device
	SoundDriverEnumerator::SOUNDDRIVERID BestDevice();

	// Allocate a stream based sound (doesn't set I/O method)
	SoundStreamWin32 *AllocateSoundStream(const char *name, bool loop, WAVEFORMATEX *wfx);

	// We must manage reclaimed sounds
	DoublyLinkedList<SoundInstance>	autoReclaim;
	unsigned int							nextTimeout;

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
	// Create an instance of a sound buffer
	bool DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER original,
										LPDIRECTSOUNDBUFFER *dup);

	// Remove this sound when it has stopped playing
	void AutoReclaim(SoundInstance *instance);

private:
	// Only our friends can create us (SoundDriver)
	SoundDriverServices(SoundDriverWin32 &parent) : driver(parent) {}
	~SoundDriverServices() {}
	friend class SoundDriverWin32;

	// Our internals
	SoundDriverWin32			&driver;
};

#endif


