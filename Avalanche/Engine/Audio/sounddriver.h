/***************************************************************************/
// Sound driver
/***************************************************************************/
#ifndef SOUNDDRIVER_H
#define SOUNDDRIVER_H

#include "container/dynlist.h"
#include "container/dllist.h"
#include "Audio/soundattr.h"
#include "Audio/soundbank.h"
#include "Audio/sstream3d.h"
#include "Audio/listener.h"

class Sound;
class Cartesian4x4;

struct ts_DBChunkHdr;

// Define the maximum number of "listeners" or viewports we support
#define MAX_LISTENERS	2

/***************************************************************************/
// The sound driver class.  Pass in NULL for the SOUNDDRIVERID if you
// want to select the default sound device
/***************************************************************************/
class SoundDriver
{
public:
	// All subclasses of sounddriver must write this function, it is not
	// implemented here (you implement it in SoundDriver:: namespace)
	typedef void *OSCREATEINFO;
	static SoundDriver *CreateAndSetOutputFormat(OSCREATEINFO osInfo);

	// The usual
	SoundDriver();
	virtual ~SoundDriver();

	// Create the sound driver, has platform specific parameters
	// This is called by CreateAndSetOutputFormat
	virtual bool Create(OSCREATEINFO osInfo) = 0;

	// Destroy it, don't count on destructor
	virtual bool Destroy();

	// Initialize the output format, will return false with NORMAL priority
	// By default the platform is set to the "ideal" output format
	// This is called by CreateAndSetOutputFormat
	virtual bool SetOutputFormat(unsigned int sampleRate, bool sixteenBit = true) = 0;

	// Set the output mode for sound
	// Not all modes are supported...but MONO and STEREO are guaranteed
	typedef enum
	{
		OUTPUTMODE_MONO,
		OUTPUTMODE_STEREO,
		OUTPUTMODE_STEREOFLIPPED,
		OUTPUTMODE_SURROUND,
		OUTPUTMODE_DOLBY51
	} OUTPUTMODE;

	// Get the current mode
	virtual OUTPUTMODE GetOutputMode();

	// Set the mode, returns false if not supported
	virtual bool SetOutputMode(OUTPUTMODE mode);

	// Test to see if this mode is supported
	virtual bool OutputModeSupported(OUTPUTMODE mode) = 0;

	// Mount the stream volumes
	virtual bool MountStreamLibrary()
		{ return true; }

	// What is the (approximate) max channels that can be simultaneously played?
	virtual int MaxChannels() = 0;

	// What is the maximum number of streams that can be simultaneously played?
	int Current3DStreams()
		{ return current3DStreams; }
	virtual int Max3DStreams() = 0;

	// High speed timer (in ms)
	virtual uint Time();

	// Call this frequently (once per frame).  Threads are nice, but messy
	virtual void Daemon();

	// If the diver has a static list of all the streams, you can call
	// this to get back a const pointer to the driver version of the stream
	// This way you don't have to store the string, just the pointer.
	// For drivers that *don't* know all the streams beforehand, this
	// Gives you back a character buffer with the stream in it...in
	// either case you must Release the token.
	//
	// This *is* an effective and quick way to see if a stream
	// exists on startup, but not as good as StreamExist if you
	// don't want to hold the token.
	virtual const char *TokenizeStreamName(const char *streamFile);
	virtual void ReleaseStreamNameToken(const char *streamToken);

	// Does this stream exist?
	virtual bool StreamExist(const char *fileName) = 0;

	// Stream a platform specific file off of the disk
	// DO NOT PUT AN EXTENSION ON THIS NAME!
	// Some platforms may not support preload!!!!
	virtual SoundStream *CreateStream(const char *fileName, bool repeat, bool preLoad = false) = 0;

	// Stream a platform specific file off of the disk w/3D
	// DO NOT PUT AN EXTENSION ON THIS NAME!
	// Some platforms may not support this at all
	SoundStream3D *CreateStream3D(const char *fileName,
												bool loop = false,
												SoundStream3D::Priority priority = SoundStream3D::STREAM3D_PRIORITY_NORMAL);

	// Flush all playing 3d streams (returns number flushed)
	virtual int Flush3DStreams();

	// Stream a platform specific file off of the disk using a
	// platform specific ID.
	typedef unsigned long SONGID;
	virtual SoundStream *CreateStream(SONGID id, bool repeat)
		{ return NULL; }

	// Load a sound bank, will automatically use BaseDataPath
	// If you load twice, it is only loaded once and the pointer is
	// returned.
	bool LoadSoundBankChunk(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
	virtual SoundBank *LoadBank(const char *bankName);

	// How many banks do we have loaded?
	virtual int Banks();

	// Unload a bank, may not truly unload until
	// everything loaded after this bank is released.
	virtual bool UnloadBank(SoundBank *bank);

	// Call this to flush all the banks!
	virtual bool UnloadAllBanks();

	// Create a sound from a platform specific image
	virtual Sound *CreateSound(SoundBank &bank, void *data, int size) = 0;

	// Find a sound in all the loaded sound banks
	virtual SoundDef *FindSoundByName(const char *name, SoundBank **bank);

	// set the camera position for 3d sound
	// You must pass in an address that stays valid during the game
	// You can set up to MAX_LISTENERS listeners (for 4-player split screen)
	// You need listeners to play 3d sounds
	virtual bool CreateListener(const Cartesian4x4 *listenerCamera);

	// Destroy all created listeners
	virtual bool DestroyListeners();

	// Get at the listener data
	virtual int Listeners();
	virtual SoundListener *Listener(int who);

	// What is the best listener
	// (just closest, but more criteria could be considered
	// perhaps the two could be averged...)
	virtual SoundListener &BestListener(Vector3CRef forPos);

	// Return distance squared to closest listenter
	virtual float DistanceSquaredClosestListener(Vector3CRef pos);

	// Pause all currently playing sounds
	enum PauseType
	{
		PAUSESFX = 0x1,
		PAUSESTREAMS = 0x2,
		PAUSEALL = 0x3
	};
	virtual void Pause(PauseType pauseType);

	// Resume all currently paused sounds
	virtual void Unpause(PauseType pauseType);

	// Environment effects are post-processing effects
	// that add spatial qualities to sound.  Environment
	// effects do not generate any sounds by themselves,
	// rather they affect how sounds get played.
	// Right now the driver just supports preset global
	// environment effects, eventually may be:
	// - Bank specific effects
	// - Programmable effects
	// Effects are largely for creating ambience, and
	// are potentially all implemented using reverb.
	typedef enum
	{
		NONE = 0,
		DEFAULT,
		PADDEDCELL,
		ROOM,
		BATHROOM,
		LIVINGROOM,
		STONEROOM,
		AUDITORIUM,
		CONCERTHALL,
		CAVE,
		ARENA,
		HANGAR,
		CARPETEDHALLWAY,
		HALLWAY,
		STONECORRIDOR,
		ALLEY,
		FOREST,
		CITY,
		MOUNTAINS,
		QUARRY,
		PLAIN,
		PARKINGLOT,
		SEWERPIPE,
		UNDERWATER,

		ENVEFFECTS
	} ENVEFFECT;

	// To ensure enough hardware resources for a game world, you
	// must inform the driver about all the effects you might use on a world.
	// You don't have to use this.
	virtual void BeginEnvironmentEffectProfile() {}
	virtual void EnvironmentEffectProfile(ENVEFFECT effect) {}
	virtual void EndEnvironmentEffectProfile() {}

	// If the effect chosen is not directly supported, an
	// equivalent for the platform is chosen.
	virtual bool SetEnvironmentEffect(ENVEFFECT effect, float volume) = 0;

	// Resolve an effect name into an enum
	static ENVEFFECT ResolveEnvironmentEffectName(const char *name);

	// If the platform has native soundtracks (ala Xbox) return
	// them via this mechanism.
	struct SONG
	{
		char 				name[64];	// Name of the song
		int				length;		// length in milliseconds
		SONGID			id;			// Platform specific ID
	};
	struct SOUNDTRACK
	{
		char				name[64];	// Name of this soundtrack
		int				length;	 	// length of all songs in milliseconds
		int				songs;		// Songs in the soundtrack
		SONG				*song;		// The array of songs in this soundtrack
	};

	// How many soundtracks are there?
	virtual int SoundTracks()
		{ return 0; }

	// Get the soundtrack information
	virtual const SOUNDTRACK *GetSoundTrack(int index)
		{ return NULL; }

	// What is the extension for sounds (includes .)
	virtual const char *FileFormat() = 0;

	// What is the default stream extension (includes .)
	virtual const char *StreamFormat() = 0;

	// What is our base path for sound? (includes terminating slash)
	virtual const char *BaseDataPath() = 0;

	// What is the base path for streams? (includes terminating slash)
	virtual const char *BaseStreamPath() = 0;

	// Special support for the FMV
	virtual void PrepareForFMV() {}
	virtual void RecoverFromFMV() {}

protected:
	// Sound banks
	DynList<SoundBank>	bankStack;

	// Subclass these to use your own subclassed bank
	virtual SoundBank *CreateBank();

	// Garbage collect banks that are marked for deletion
	virtual void GarbageCollectBanks();

	// Destroy a bank (guaranteed to be the top of the stack!!)
	virtual void DestroyBank(SoundBank *bank);

	// Our current sound output mode
	OUTPUTMODE			outputMode;

	// 3D Stream support
	DoublyLinkedList<SoundStream3D>	soundStream3D;
	int										current3DStreams;
	void Add3DStream(SoundStream3D *stream);
	void Remove3DStream(SoundStream3D *stream);
	bool Allocate3DStream(SoundStream3D::Priority forPriority, float volume);
	bool Free3DStream(SoundStream3D *stream);
	friend class SoundStream3D;

	// The list of listeners (we support MAX_LISTENERS viewports)
	int	 				listeners;
	SoundListener		*listener[MAX_LISTENERS];

	// Create/destroy a listener
	virtual SoundListener *NewListener(const Cartesian4x4 &worldToListener);
	virtual void DeleteListener(SoundListener *listener);
};

#endif


