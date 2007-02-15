/***************************************************************************/
// The total audio system
/***************************************************************************/
#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef _XBOX
#include <windows.h>
#endif
#endif

#include "platform/BaseType.h"
#include "Game/Database/DBFile.h"

#include "container/dynlist.h"
#include "Audio/audiolib.h"
#include "Audio/fader.h"
#include "Audio/jukebox.h"
#include "Audio/voiceq.h"
#include "Audio/group/sgroup.h"
#include "Audio/group/reclaim.h"
#include "Audio/group/sgroup2d.h"
#include "Audio/group/sgroup3d.h"
#include "Audio/group/sgalias.h"
#include "Audio/sounddriver.h"

// High level class for doing in-game sound management.
class Audio
{
public:
	// Setup stuff
	Audio();
	~Audio();

	// Create the sound device
	// If you don't call create, there will be no sound
	bool Create(SoundDriver::OSCREATEINFO osInfo);

	// Destroy everything
	void Destroy();

	// Mount the stream volumes
	bool MountStreamLibrary();

	// Pause all currently playing sounds, won't prevent new sounds from playing
	void Pause(bool pauseJukebox = true);
	void Unpause(bool unpauseJukebox = true);
	bool Paused();

	// Flush out all emitters, listeners and banks, but keep the driver active
	enum AUDIOFLUSH
	{
		HARDFLUSH,
		SOFTFLUSH
	};
	void Flush(AUDIOFLUSH type, bool clearDialog = true);

	// The volume groups we support
	enum VolumeGroup
	{
		FULL,			// Always returns one!
		SFX,			// All sound effects
		DIALOG,		// (Subordinate to SFX) dialog
		SFX2D,		// (Subordinate to SFX) 2D Emitters
		SFX3D,		// (Subordinate to SFX) 3D Emitters

		MUSIC,		// music

		VOLUMEGROUPS
	};
	// Set the group volume (from [0.0, 1.0])
	const static float smoothVolumeInstant;
	const static float smoothVolumeFast;
	const static float smoothVolumeSlow;
	const static float smoothVolumeVerySlow;
	bool SetVolume(VolumeGroup group, float volume, float smooth = smoothVolumeInstant);
	bool SetVolumeFactor(VolumeGroup group, float factor);
	float GetVolume(VolumeGroup group);
	Fader	&GetFader(VolumeGroup group)
		{ return fader[group]; }

	// Set the master pitch
	void SetMasterPitch(float semitones, bool includeStream2D = false);
	void InferMasterPitch(float timeFactor, bool includeStream2D = false);
	float GetMasterPitch();

	// Get the current mode (see sounddriver.h for valid modes)
	SoundDriver::OUTPUTMODE GetOutputMode();

	// Set the mode, returns false if not supported
	bool SetOutputMode(SoundDriver::OUTPUTMODE mode);

	// Test to see if this mode is supported
	bool OutputModeSupported(SoundDriver::OUTPUTMODE mode);

	// set the camera position for 3d sound
	// You must pass in an address that stays valid during the game
	// You can set up to MAX_LISTENERS listeners (for 4-player split screen)
	// You need listeners to play 3d sounds
	bool CreateListener(const Cartesian4x4 *listenerCamera);

	// Destroy all created listeners
	bool DestroyListeners();

	// Return the best listener
	SoundListener *BestListener(Vector3CRef forPos);

	// Loaders
	bool LoadSoundBankChunk(void *data, ts_DBChunkHdr *chunkHdr, void **retData, u32 *dataSize);
  	SoundBank *LoadBank(const char *bankName);

	// How many banks do we have?
	int Banks();

	// Unload a bank (make sure you have gotten rid of emitters)
	bool UnloadBank(SoundBank *killBank);

	// Create/destroy a group
	bool Create2DGroup(const char *groupName, int channels, Reclaim::METHOD reclaimMethod = Reclaim::OLDEST);
	bool Create3DGroup(const char *groupName, int channels, Reclaim::METHOD reclaimMethod = Reclaim::VOLUME);
	bool AliasGroup(const char *aliasName, const char *groupName);
	bool DestroyGroup(const char *groupName);
	bool DestroyGroupWhenEmpty(const char *groupName, bool destroy = true);

	// Ask the driver what the max channels are
	int MaxChannels();

	// How many channels are allocated to groups?
	int AllocatedChannels();

	// What is the total number of channels currently being used
	int UsedChannels();

	// Create a 2d sound emitter
	SoundEmitter2D *Create2DEmitter(const char *name, const char *groupName);

	// Create a 3d sound emitter
	SoundEmitter3D *Create3DEmitter(const char *name, const char *groupName);

	// Create a 3d string sound emitter
	SoundEmitter3DString *Create3DStringEmitter(const char *name, const char *groupName);

	// Create an environmental sound emitter
	SoundEmitterEnvironment *CreateEnvironmentEmitter(const char *name, const char *groupName);

	// How many emitters do we have?
	int Emitters();

	// Get at an emitter by name
	SoundEmitter *GetEmitter(const char *emitterName);

	// Destroy an emitter, calls delete!
	bool DestroyEmitter(const char *emitterName, bool dieSoftly = true);
	bool DestroyEmitter(SoundEmitter *killEmitter, bool dieSoftly = true);

	// The music interface is all through the jukebox
	Jukebox			jukebox;

	// The dialog interface (for streams played on top of music)
	VoiceQueue		dialog;

	// 3D Streams interface
	// What is the maximum number of streams that can be simultaneously played?
	int Max3DStreams();
	int Current3DStreams();

	// Does this stream exists (you shouldn't worry about this too much)
	bool StreamExists(const char *stream)
		{ if (driver) return driver->StreamExist(stream); else return false; }

	// "Tokenize" a stream name...so they are only stored onece, you must free this later
	const char *TokenizeStreamName(const char *stream)
		{ if (driver) return driver->TokenizeStreamName(stream); else return NULL; }
	void ReleaseStreamNameToken(const char *streamToken)
		{ if (driver) driver->ReleaseStreamNameToken(streamToken); }

	// Set the default Stream3D params
	SoundEmitter3DParams &GetStream3DParams()
		{ return stream3DParams; }

	// Wee! (set auto-delete to automatically delete when done)
	CommonStreamInterface *CreateStream2D(const char *fileName, bool repeat, bool autoDelete = false,
														bool preLoad = false, VolumeGroup volumeGroup = FULL);
	CommonStreamInterface *CreateStream3D(const char *fileName, bool repeat, bool autoDelete = false,
														SoundStream3D::Priority priority = SoundStream3D::STREAM3D_PRIORITY_NORMAL,
														VolumeGroup volumeGroup = SFX3D);
	CommonStreamInterface *CreateStream2D(SoundDriver::SONGID id, bool repeat, bool autoDelete = false,
														VolumeGroup volumeGroup = FULL);

	// Call this every frame
	void Daemon();

	// Special support for the FMV
	void PrepareForFMV();
	void RecoverFromFMV();

private:
	// Our sound driver
	SoundDriver								*driver;

	// Are we paused?
	bool										paused;

	// The volume faders
	Fader										fader[VOLUMEGROUPS];
	FaderBoard								faderBoard;

	// Master pitch
	float										masterPitch;

	// Groups
	DynList<SoundGroup>					groupList;
	SoundGroup								*GetGroup(const char *groupName);
	DynList<SoundGroupAlias>			aliasList;
	SoundGroupAlias						*FindAlias(const char *aliasName);

	// Pools for fast group creation
	Pool<SoundGroup2D>					group2DPool;
	Pool<SoundGroup3D>					group3DPool;
	Pool<SoundGroupAlias>				groupAliasPool;
	void DeleteGroup(SoundGroup *group);

	// Emitters
	DoublyLinkedList<SoundEmitter>	emitterList;


	// Set the default Stream3D params
	SoundEmitter3DParams 				stream3DParams;

	// Fader names for master groups
	static const char *volumeGroupName[VOLUMEGROUPS];
};

// The sound system global(s)
extern Audio *g_audioPtr;
inline Audio &SafePtr(Audio *ptr)
{
	ASSERT(ptr);
	return *ptr;
}
#define g_audio	SafePtr(g_audioPtr)

#endif
