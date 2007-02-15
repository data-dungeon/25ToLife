//***************************************************************************
// Utility class for maintaining the main emitters during a level.
//
// I don't see why I'm bothering to put any comments in here, no one will
// read this anyway.
//***************************************************************************
#ifndef SFX_H
#define SFX_H

#include "Audio/audiosys.h"
#include "EngineHelper/soundcollide.h"
#include "GameHelper/Message.h"
#include "Math/Vector.h"
#include "container/dllist.h"
#include "container/fpool.h"

// Max sounds in the sound queue
#define MAX_QUEUED_SOUNDS		16

class CActor;

// Structures for sound messages
class SStationarySound
{
public:
	SStationarySound()
		{ d_volume = 1.0f; d_pitch = 0.0f; }
	const char	*d_name;
	Vector3		d_pos;
	float			d_volume, d_pitch;
};

class SAttachedSound
{
public:
	SAttachedSound()
		{ d_volume = 1.0f; d_pitch = 0.0f; }

	const char	*d_name;
	CActor		*d_actor;
	float			d_volume, d_pitch;
};

class Sfx
{
public:
	// These are obvious
	static bool Initialize2D(const char *bankFile, int channelsFor2D);
	static void Terminate2D();

	static bool Initialize3D(const char *bankFile, int channelsFor3D, int channelsforEnvironment);
	static void Terminate3D();

	// Play with sound occlusions
	enum OcclusionType
	{
		SFX_NO_OCCLUSIONS = 0x0,   					// No occlusions
		SFX_SFX3D_OCCLUSIONS = 0x1,					// Occlude stuff from the main 3D emitter
		SFX_ENV_OCCLUSIONS = 0x2,						// Occlude environmental sounds
		SFX_STREAM3D_OCCLUSIONS = 0x4,				// Occlude Stream sounds
		SFX_ALL_OCCLUSIONS = 0xff						// All all occlusions
	};
	static void EnableOcclusion(int enable);
	static int CurrentOcclusion()
		{ return enableOcclusion; }
	static SoundCollisionTest *OcclusionTest()
		{ return	&occlusionTest; }

	// Setup specific property for a material occlusion
	// When the source is blocked by the material, the overall volume
	// is multiplied by volumeFactor...so if it is 0.0 no sound goes through
	// Pan factor works the same way, by default it should be 1.0, but if a material really "disperses"
	// a sound you can lower the panFactor towards 0.0.
	static void SetMaterialOcclusion(u8 material, float volumeFactor, float panFactor = 1.0f);

	// Load the material occlusions from a file and enable them
	static bool LoadMaterialOcclusion(const char *file = "soundocc.txt");

	// Initialize the enivornmental sounds from the world dbl
	static bool SetupSoundEnvironment(t_Handle worldHandle);

	// Setup the global default effect
	static bool SetGlobalEnvironmentEffect(SoundDriver::ENVEFFECT effect, float volume);

	// Done adding sources and fields to the world
	static bool DoneSetupSoundEnvironment();

	// Interface for 2d sounds
	// Only use 2D sounds for things that AREN'T IN THE GAME WORLD
	// These calls would be good for the front-end
	// If you use these for objects in the world, I will kill you.
	// If you request a handle you must call Stop on it or delete it.
	static bool Play2D(const char *name, SoundHandle *handle = NULL);
	static bool Play2D(const char *name, float volume, float pitch, SoundHandle *handle = NULL);

	// Play a 3D sound, watching the object's position and velocity
	// BE EXTREMELY CAREFUL WITH THIS, the sounddriver will continue to read these pointers for a while
	// You must pass in a position
	// Pass in NULL if you don't have a velocity.
	// If you request a handle you must call Stop on it or delete it.
	static bool Play(const char *name, const Vector3 *pos, const Vector3 *velocity = NULL, SoundHandle *handle = NULL);
	static bool Play(const char *name, const Vector3 *pos, const Vector3 *velocity, float volume, float pitch, SoundHandle *handle = NULL);

	// Watch an actor's info, just like the above calls (these use focus)
	enum ActorPlayPos
	{
		ACTORPLAYPOS_BODY = 0,
		ACTORPLAYPOS_FOCUS = 1,
		ACTORPLAYPOS_DEFAULT = 2
	};
	static bool Play(const char *name, CActor &actor, ActorPlayPos actorPos, SoundHandle *handle = NULL);
	static bool Play(const char *name, CActor &actor, ActorPlayPos actorPos, float volume, float pitch, SoundHandle *handle = NULL);

	// Play a 3D sound, at the specified position
	// If you request a handle you must call Stop on it or delete it.
	static bool PlayStationary(const char *name, Vector3CRef pos, SoundHandle *handle = NULL);
	static bool PlayStationary(const char *name, Vector3CRef pos, float volume, float pitch, SoundHandle *handle = NULL);

	// Queue a stationary 3D sound. You can't get a handle back...
	static bool QueueStationary(const char *name, float delay, Vector3CRef pos, float volume = 1.0f, float pitch = 0.0f);
	static bool IsQueued(const char *name);
	static void FlushQueued();

	// Is this sound still active?
	static bool Active(SoundHandle handle);

	// Set the volume on a handle
	static bool SetVolume(SoundHandle handle, float volume);

	// Set the pitch on a handle
	static bool SetPitch(SoundHandle handle, float pitch);

	// Set the wet/dry mix
	static bool SetWetDry(SoundHandle handle, float wet, float dry);

	// Set the ambience on a handle
	static bool SetAmbience(SoundHandle handle, float ambience);

 	// Set the presence on a handle
	static bool SetPresence(SoundHandle handle, float presence);

 	// Detach a sound from watching pos/velocity
	static bool Detach(SoundHandle handle);

	// This will stop the sound and null the handle
	static void Stop(SoundHandle &handle);

	// Just free the handle without stoping sound
	static void ReleaseHandle(SoundHandle &handle);

	// Access the emitters directly
	static SoundEmitter2D &Get2DEmitter();
	static SoundEmitter3D &Get3DEmitter();
	static SoundEmitterEnvironment &GetEnvironmentEmitter();

	// Get at the group names
	static const char *Get2DGroupName();
	static const char *Get3DGroupName();
	static const char *GetEnvironmentGroupName();

	// Run the update
	static void Daemon();

	// Debugging (pass in NULL for pos if you want all)
	static void Debug(const Vector3 *cameraPos);

private:
	// The main banks
	static SoundBank 							*bank;
	static SoundBank							*bank2D;

	// Our emitters
	static SoundEmitter2D					*emitter2D;
	static SoundEmitter3D					*emitter3D;
	static SoundEmitterEnvironment		*emitterEnv;

	// The sound occlusion tester
	static int									enableOcclusion;
	static SoundCollisionTest				occlusionTest;

	// The all important cookie handler
	static const Vector3 *GetActorPlayPos(CActor &actor, ActorPlayPos actorPos);
	static bool ActorValidCallback(void *cookie);

	// The sound queue
	struct QueuedSound
	{
		const char 		*name;
		Vector3			pos;
		float				playTime;
		float				volume;
		float				pitch;

		QueuedSound 	*prev, *next;
	};
	static DoublyLinkedList<QueuedSound>						soundQueue;
	static FixedPool<QueuedSound, MAX_QUEUED_SOUNDS>		soundPool;

	// Utilities
	static SoundEmitter *GetEmitter(SoundHandle handle);
	static SoundEmitter3D *Get3DEmitter(SoundHandle handle);

	static void ClearHandle(SoundHandle *handle);
	static void SetHandle(SoundHandle *handle, SoundHandle sound);
};

#endif
