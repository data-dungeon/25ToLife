/***************************************************************************/
// Sound driver for PS2
/***************************************************************************/
#ifndef SDRIVER_H
#define SDRIVER_H

#include "container/dllist.h"
#include "container/fpool.h"
#include "Audio/sounddriver.h"
#include "Audio/PS2/streamlib.h"
#include "Audio/PS2/banklib.h"
#include "Layers/mstream/mstream.h"

class Sound;
class SoundInstance;
class SoundDriverCaps;

// Maximum number of simultaneously loaded sfx
#define MAX_SFX						300

// Maximum number of total streams
#define MAX_STREAMS					12

// Maximum number of SRAM handles
#define MAX_SRAM_HANDLES			(MAX_STREAMS * 2 + MAX_SFX + 2)

/***************************************************************************/
// The sound driver class.
//
// Errors are returned via the SoundError class.  If you get a false out of
// one of these calls, use the GetLastError call to see what happened.
/***************************************************************************/
class SoundDriverPS2 : public SoundDriver
{
public:
	// The usual
	SoundDriverPS2();
	~SoundDriverPS2();

	// Create it
	bool Create(OSCREATEINFO osInfo = NULL);

	// Destroy it (destructor does the same thing)
	bool Destroy();

	// Initialize the output format, will return false with NORMAL priority
	bool SetOutputFormat(uint sampleRate, bool sixteenBit = true);

	// Set the mode, returns false if not supported
	bool SetOutputMode(OUTPUTMODE mode);

	// Test to see if this mode is supported
	bool OutputModeSupported(OUTPUTMODE mode);

	// Mount the stream library
	bool MountStreamLibrary();

	// What is the (approximate) max channels that can be simultaneously played?
	int MaxChannels();

	// What is the maximum number of streams that can be simultaneously played?
	int Max3DStreams();

	// Call this frequently.  Threads are nice, but messy
	void Daemon();

	// Create a sound from a memory image (image for this driver is a .vag file)
	Sound *CreateSound(SoundBank &bank, void *data, int size);

	// We do have tables of all streams, so this actually works!
	const char *TokenizeStreamName(const char *streamFile);
	void ReleaseStreamNameToken(const char *streamToken) {}

	// Does this stream exist?
	bool StreamExist(const char *fileName);

	// Stream a platform specific file off of the disk!
	// DO NOT PUT AN EXTENSION ON THIS NAME!
	SoundStream *CreateStream(const char *fileName, bool repeat, bool preLoad = false);

	// Pause all currently playing sounds
	void Pause(PauseType pauseType);

	// Resume all currently paused sounds
	void Unpause(PauseType pauseType);

	// To ensure enough hardware resources for a game world, you
	// must inform the driver about all the effects you might use on a world.
	// You don't have to use this.
	void BeginEnvironmentEffectProfile();
	void EnvironmentEffectProfile(ENVEFFECT effect);
	void EndEnvironmentEffectProfile();

	// Setup an environmental effect
	bool SetEnvironmentEffect(ENVEFFECT effect, float volume);

	// What is the extension for sounds
	const char *FileFormat()
		{ return ".vag"; }

	// What is the default stream extension
	const char *StreamFormat()
		{ return ".mib"; }

	// What is our base path for sound?
	const char *BaseDataPath()
		{ return "sound\\"; }

	// What is the base path for streams?
	const char *BaseStreamPath()
		{ return "stream\\"; }

	// Special support for the FMV
	void PrepareForFMV();
	void RecoverFromFMV();

protected:
	// Is the bank library load
	bool			bankLibraryMounted;
	BankLibrary	bankLibrary;

	// Subclass these to use your own subclassed bank
	SoundBank *CreateBank();

	// Basic settings
	bool			stereo;
	uint			sampleRate;

	/////////////////////////////
	// stuff for reverb & effects
	/////////////////////////////
	struct EffectMapper
	{
		int			mode;      		// One of the following:
											// SD_REV_MODE_OFF 		Off 0x80
											// SD_REV_MODE_ROOM 		Room 0x26c0
											// SD_REV_MODE_STUDIO_A Studio (small) 0x1f40
											// SD_REV_MODE_STUDIO_B Studio (medium) 0x4840
											// SD_REV_MODE_STUDIO_C Studio (large) 0x6fe0
											// SD_REV_MODE_HALL 		Hall 0xade0
											// SD_REV_MODE_SPACE 	Space echo 0xf6c0
											// SD_REV_MODE_ECHO 		Echo 0x18040
											// SD_REV_MODE_DELAY 	Delay 0x18040
											// SD_REV_MODE_PIPE 		Pipe echo 0x3c00
		float			depth;			// -1.0 1.0f
		int			delay;			// 0 - 127 for echo and delay modes only
		int			feedback;      // 0 - 127 for echo and delay modes only
	};
	static EffectMapper effectMap[ENVEFFECTS];

	// Effects SRAM management
	bool			PrepareEffectsSRAM(int effect);

	// Profile effects tracking
	ENVEFFECT	biggestEffect;

	// The current effect and effect depth
	ENVEFFECT	currentEffect;
	int			currentEffectDepth;

	//////////////////////////////////
	// We must manage reclaimed sounds
	//////////////////////////////////
	DoublyLinkedList<SoundInstance>	autoReclaim;
	uint 										nextTimeout;
	void AutoReclaim(SoundInstance *instance);

	//////////////////////////
	// All the stream shiznit!
	//////////////////////////

	// The stream library maintains a lot of nice info for us
	StreamLibrary	streamLibrary;
	bool 				streamLibraryMounted;
	bool MountStreamLibraries();
	void JustFileName(const char *pathFile, char *filename);

	// Big fat error handler
	void BigFatError(const char *fileName, const char *description);

	// debugging
	void DebugDump();

	// Utiltity
	uint SizeBy64(uint s)
		{ return (s + (((s ^ 0x3f) + 1) & 0x3f)); }

	// We have lots of firends
	friend class SoundDriverCaps;
	friend class SoundStreamPS2;
	friend class Sound;
	friend class SoundInstance;
	friend class StreamVolume;
	friend class SoundBankPS2;
};

#endif


