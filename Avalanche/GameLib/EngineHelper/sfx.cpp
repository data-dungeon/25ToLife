//***************************************************************************
// Utility class for maintaining the main emitters for a level
//
// I don't see why I'm bothering to put any comments in here, no one will
// read this anyway.
//***************************************************************************
#include "EngineHelper/EngineHelperPCH.h"

#include "stage/cactor.h"
#include "EngineHelper/timer.h"
#include "EngineHelper/occtable.h"

SoundBank *Sfx::bank = NULL;
SoundBank *Sfx::bank2D = NULL;
SoundEmitter2D	*Sfx::emitter2D = NULL;
SoundEmitter3D	*Sfx::emitter3D = NULL;
SoundEmitterEnvironment *Sfx::emitterEnv = NULL;

int Sfx::enableOcclusion = SFX_NO_OCCLUSIONS;
SoundCollisionTest Sfx::occlusionTest;

// The shizno for the grizno
DoublyLinkedList<Sfx::QueuedSound>						Sfx::soundQueue(false);
#ifdef _DEBUG
FixedPool<Sfx::QueuedSound, MAX_QUEUED_SOUNDS>		Sfx::soundPool("sfxqueue", false);
#else
FixedPool<Sfx::QueuedSound, MAX_QUEUED_SOUNDS>		Sfx::soundPool(false);
#endif

//***************************************************************************
//***************************************************************************
bool Sfx::Initialize2D(
const char *bankFile,
int channelsFor2D)
{
	// Load the bank
	bank2D = g_audio.LoadBank(bankFile);
	ASSERT(bank2D);

	// Create our groups
	bool ret = g_audio.Create2DGroup(Get2DGroupName(), channelsFor2D);
	ASSERT(ret);

	// Create emitter
	ASSERT(emitter2D == NULL);

	emitter2D = g_audio.Create2DEmitter("Sfx2D", Get2DGroupName());
	ASSERT(emitter2D);

	return(emitter2D != NULL);
}

//***************************************************************************
//***************************************************************************
void Sfx::Terminate2D()
{
	// Destroy the emitters
	g_audio.DestroyEmitter(emitter2D, false);
	emitter2D = NULL;

	// kill the groups
	g_audio.DestroyGroup(Get2DGroupName());

	// unload the bank
	bank2D = NULL;
}

//***************************************************************************
// These are obvious
//***************************************************************************
bool Sfx::Initialize3D(
const char *bankFile,
int channelsFor3D,
int channelsForEnvironment)
{
	// Load the bank
	bank = g_audio.LoadBank(bankFile);
	ASSERT(bank);

	// Create our groups
	bool ret = g_audio.Create3DGroup(Get3DGroupName(), channelsFor3D);
	ASSERT(ret);

	ret = g_audio.Create3DGroup(GetEnvironmentGroupName(), channelsForEnvironment);
	ASSERT(ret);

	// Create the emitters
	ASSERT(emitter3D == NULL && emitterEnv == NULL);

	emitter3D = g_audio.Create3DEmitter("Sfx3D", Get3DGroupName());
	ASSERT(emitter3D);

	emitterEnv = g_audio.CreateEnvironmentEmitter("SfxEnv", GetEnvironmentGroupName());
	ASSERT(emitterEnv);

	// Bind in the ambient emitter
	if (emitterEnv)
		emitterEnv->BindAmbientEmitter(emitter2D);

	// Make sure we have a cookie callback
	SoundEmitter3DParams::RegisterCookieCallback(ActorValidCallback);

	// Do they want occlusions?
	EnableOcclusion(enableOcclusion);

	// How'd it go?
	return (bank && emitter3D && emitterEnv);
}

//***************************************************************************
//***************************************************************************
void Sfx::Terminate3D()
{
	// Destroy the emitters
	g_audio.DestroyEmitter(emitterEnv, false);
	emitterEnv = NULL;

	g_audio.DestroyEmitter(emitter3D, false);
	emitter3D = NULL;

	// kill the groups
	g_audio.DestroyGroup(Get3DGroupName());
	g_audio.DestroyGroup(GetEnvironmentGroupName());

	// Destroy the bank
	bank = NULL;
}

//***************************************************************************
// Play with sound occlusions
//***************************************************************************
void Sfx::EnableOcclusion(
int enable)
{
	// Save it
	enableOcclusion = enable;

	if (emitter3D)
	{
		if (enableOcclusion & SFX_SFX3D_OCCLUSIONS)
			emitter3D->GetParams().SetupOcclusionTest(&occlusionTest);
		else
			emitter3D->GetParams().SetupOcclusionTest(NULL);
	}

	if (emitterEnv)
	{
		if (enableOcclusion & SFX_ENV_OCCLUSIONS)
			emitterEnv->GetParams().SetupOcclusionTest(&occlusionTest);
		else
			emitterEnv->GetParams().SetupOcclusionTest(NULL);
	}

	// Do it for 3d streams!
	if (enableOcclusion & SFX_STREAM3D_OCCLUSIONS)
		g_audio.GetStream3DParams().SetupOcclusionTest(&occlusionTest);
	else
		g_audio.GetStream3DParams().SetupOcclusionTest(NULL);
}

//***************************************************************************
// Setup specific property for a material occlusion
// When the source is blocked by the material, the overall volume
// is multiplied by volumeFactor...so if it is 0.0 no sound goes through
// Pan factor works the same way, by default it should be 1.0, but if a material really "disperses"
// a sound you can lower the panFactor towards 0.0.
//***************************************************************************
void Sfx::SetMaterialOcclusion(
u8 material,
float volumeFactor,
float panFactor)
{
	SoundOcclusionTest::MaterialProperty prop;

	prop.volumeFactor = volumeFactor;
	prop.panFactor = panFactor;

	occlusionTest.SetMaterialProperty(material, prop);
}

//***************************************************************************
//***************************************************************************
bool Sfx::LoadMaterialOcclusion(
const char *file)
{
	SoundOcclusionTable table;
	if (!table.Load(file))
		return false;

	const SoundOcclusionTest::MaterialProperty *prop = table.PropertyArray();
	int props = table.Properties();
	for (int m = 0; m < props; m++)
		occlusionTest.SetMaterialProperty(m, prop[m]);

	return true;
}

//***************************************************************************
// Initialize the enivornmental sounds from the world dbl
//***************************************************************************
bool Sfx::SetupSoundEnvironment(
t_Handle worldHandle)
{
	// No environment emitter equals no dice...
	if (!emitterEnv)
		return false;

	// Do the sources
	ts_RecordList *recordList = (ts_RecordList *)dblGetChunkData(DB_SOUNDSOURCE, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if (recordList)
	{
		for (uint i = 0; i < recordList->usRecordCount; i++)
		{
			ts_EnvSoundSource *source = (ts_EnvSoundSource *)GetRecordAddress(recordList, i);
			ASSERT(source);

			if (source)
				emitterEnv->AddSource(*source);
		}
	}

	// Do the fields
	recordList = (ts_RecordList *)dblGetChunkData(DB_SOUNDFIELD, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if (recordList)
	{
		for (uint i = 0; i < recordList->usRecordCount; i++)
		{
			ts_EnvSoundField *field = (ts_EnvSoundField *)GetRecordAddress(recordList, i);
			ASSERT(field);

			if (field)
				emitterEnv->AddField(*field);
		}
	}

	return true;
}

//***************************************************************************
// Setup the global default effect
//***************************************************************************
bool Sfx::SetGlobalEnvironmentEffect(
SoundDriver::ENVEFFECT effect,
float volume)
{
	// Do we have an environmental emitter?
	if (!emitterEnv)
		return false;

	// Pass it on
	return emitterEnv->SetGlobalEnvironmentEffect(effect, volume);
}

//***************************************************************************
// Done adding sources and fields to the world
//***************************************************************************
bool Sfx::DoneSetupSoundEnvironment()
{
	if (emitterEnv)
	{
		emitterEnv->DoneSources();
		emitterEnv->DoneFields();
	}

	return true;
}

//***************************************************************************
// Interface for 2d sounds
// Only use 2D sounds for things that AREN'T IN THE GAME WORLD
// These calls would be good for the front-end
// If you use these for objects in the world, I will kill you.
// If you request a handle you must call Stop on it or delete it.
//***************************************************************************
bool Sfx::Play2D(
const char *name,
SoundHandle *handle)
{
	return Play2D(name, 1.0f, 0.0f, handle);
}

//***************************************************************************
// Interface for 2d sounds
// Only use 2D sounds for things that AREN'T IN THE GAME WORLD
// These calls would be good for the front-end
// If you use these for objects in the world, I will kill you.
// If you request a handle you must call Stop on it or delete it.
//***************************************************************************
bool Sfx::Play2D(
const char *name,
float volume,
float pitch,
SoundHandle *handle)
{
	// Always clear the handle
	ClearHandle(handle);

	// Duh?
	ASSERT(emitter2D);
	if (!emitter2D || !name || *name == '\0')
		return false;

	// Cue up the sound
	SoundHandle sound = emitter2D->Cue(name);
	if (!sound)
		return false;

	// Setup the stuff
	emitter2D->SetVolume(sound, volume);
	emitter2D->SetPitch(sound, pitch);

	// Start it playing
	emitter2D->Play(sound);

	// Give them back the handle
	SetHandle(handle, sound);

	return true;
}

//***************************************************************************
// Play a 3D sound, watching the object's position and velocity
// You must pass in a position
// Pass in NULL if you don't have a velocity.
// If you request a handle you must call Stop on it or delete it.
//***************************************************************************
bool Sfx::Play(
const char *name,
const Vector3 *pos,
const Vector3 *velocity,
SoundHandle *handle)
{
	return Play(name, pos, velocity, 1.0f, 0.0f, handle);
}

//***************************************************************************
// Play a 3D sound, watching the object's position and velocity
// You must pass in a position
// Pass in NULL if you don't have a velocity.
// If you request a handle you must call Stop on it or delete it.
//***************************************************************************
bool Sfx::Play(
const char *name,
const Vector3 *pos,
const Vector3 *velocity,
float volume,
float pitch,
SoundHandle *handle)
{
	// They must pass in a position
	ASSERT(pos);

	// Always clear the handle
	ClearHandle(handle);

	// Duh?
	ASSERT(emitter3D);
	if (!emitter3D || !name || *name == '\0')
		return false;

	// Cue up the sound
	SoundHandle sound = emitter3D->Cue(name);
	if (!sound)
		return false;

	// Setup the stuff
	emitter3D->SetVolume(sound, volume);
	emitter3D->SetPitch(sound, pitch);
 	emitter3D->WatchPosition(sound, pos);
	emitter3D->WatchVelocity(sound, velocity);

	// Start it playing
	emitter3D->Play(sound);

	// Give them back the handle
	SetHandle(handle, sound);

	return true;
}

//***************************************************************************
// Watch an actor's info
//***************************************************************************
bool Sfx::Play(
const char *name,
CActor &actor,
ActorPlayPos actorPos,
SoundHandle *handle)
{
	return Play(name,
					actor, actorPos,
					1.0f, 0.0f, handle);
}

//***************************************************************************
// Watch an actor's info
//***************************************************************************
bool Sfx::Play(
const char *name,
CActor &actor,
ActorPlayPos actorPos,
float volume,
float pitch,
SoundHandle *handle)
{
	// Always clear the handle
	ClearHandle(handle);

	// Duh?
	ASSERT(emitter3D);
	if (!emitter3D || !name || *name == '\0')
		return false;

	// Cue up the sound
	SoundHandle sound = emitter3D->Cue(name);
	if (!sound)
		return false;

	// Setup the stuff
	emitter3D->SetVolume(sound, volume);
	emitter3D->SetPitch(sound, pitch);
	emitter3D->SetCookie(sound, (void *)actor.GetHandle());
 	emitter3D->WatchPosition(sound, GetActorPlayPos(actor, actorPos));
	emitter3D->WatchVelocity(sound, &actor.GetVelocityInWorld());

	// Start it playing
	emitter3D->Play(sound);

	// Give them back the handle
	SetHandle(handle, sound);

	return true;
}

//***************************************************************************
// Play a 3D sound, at the specified position
// If you request a handle you must call Stop on it or delete it.
//***************************************************************************
bool Sfx::PlayStationary(
const char *name,
Vector3CRef pos,
SoundHandle *handle)
{
	return PlayStationary(name, pos, 1.0f, 0.0f, handle);
}

//***************************************************************************
// Play a 3D sound, at the specified position
// If you request a handle you must call Stop on it or delete it.
//***************************************************************************
bool Sfx::PlayStationary(
const char *name,
Vector3CRef pos,
float volume,
float pitch,
SoundHandle *handle)
{
	// Always clear the handle
	ClearHandle(handle);

	// Duh?
	ASSERT(emitter3D);
	if (!emitter3D || !name || *name == '\0')
		return false;

	// Cue up the sound
	SoundHandle sound = emitter3D->Cue(name);
	if (!sound)
		return false;

	// Setup the stuff
	emitter3D->SetVolume(sound, volume);
	emitter3D->SetPitch(sound, pitch);
 	emitter3D->SetPosition(sound, pos);

	// Start it playing
	emitter3D->Play(sound);

	// Give them back the handle
	SetHandle(handle, sound);

	return true;
}

//***************************************************************************
// Quaue a stationary 3D sound. You can't get a handle back...
//***************************************************************************
bool Sfx::QueueStationary(
const char *name,
float delay,
Vector3CRef pos,
float volume,
float pitch)
{
	QueuedSound *sound = soundPool.New();
	if (!sound)
		return false;

	// set it up
	sound->name = name;
	sound->pos = pos;
	sound->volume = volume;
	sound->pitch = pitch;
	sound->playTime = g_timer.GetEndSec() + delay;

	soundQueue.AddTail(sound);
	return true;
}

//***************************************************************************
//***************************************************************************
bool Sfx::IsQueued(
const char *name)
{
	QueuedSound *sound = soundQueue.Head();
	while (sound)
	{
		// Is this it?
		if (strcmpi(name, sound->name) == 0)
			return true;

		// next!
		sound = sound->next;
	}

	return false;
}

//***************************************************************************
//***************************************************************************
void Sfx::FlushQueued()
{
	QueuedSound *sound = soundQueue.Head();
	while (sound)
	{
		QueuedSound *next = sound->next;

		// Kill it
		soundQueue.Unlink(sound);
		soundPool.Delete(sound);

		// next!
		sound = next;
	}
}

//***************************************************************************
// Is this sound still active?
//***************************************************************************
bool Sfx::Active(
SoundHandle handle)
{
	SoundEmitter *emitter = GetEmitter(handle);
	if (!emitter)
		return false;

	return emitter->Active(handle);
}

//***************************************************************************
// Set the volume on a handle
//***************************************************************************
bool Sfx::SetVolume(
SoundHandle handle,
float volume)
{
	SoundEmitter *emitter = GetEmitter(handle);
	if (!emitter)
		return false;

	emitter->SetVolume(handle, volume);

	return true;
}

//***************************************************************************
// Set the pitch on a handle
//***************************************************************************
bool Sfx::SetPitch(
SoundHandle handle,
float pitch)
{
	SoundEmitter *emitter = GetEmitter(handle);
	if (!emitter)
		return false;

	emitter->SetPitch(handle, pitch);

	return true;
}

//***************************************************************************
// Set the wet/dry mix
//***************************************************************************
bool Sfx::SetWetDry(
SoundHandle handle,
float wet,
float dry)
{
	SoundEmitter *emitter = GetEmitter(handle);
	if (!emitter)
		return false;

	emitter->SetWetDry(handle, wet, dry);

	return true;
}

//***************************************************************************
// Set the ambience on a handle
//***************************************************************************
bool Sfx::SetAmbience(
SoundHandle handle,
float ambience)
{
	SoundEmitter3D *emitter = Get3DEmitter(handle);
	if (!emitter)
		return false;

	emitter->SetAmbience(handle, ambience);

	return true;
}

//***************************************************************************
// Set the presence on a handle
//***************************************************************************
bool Sfx::SetPresence(
SoundHandle handle,
float presence)
{
	SoundEmitter3D *emitter = Get3DEmitter(handle);
	if (!emitter)
		return false;

	emitter->SetPresence(handle, presence);

	return true;
}

//***************************************************************************
// Detach a sound from watching pos/velocity
//***************************************************************************
bool Sfx::Detach(
SoundHandle handle)
{
	SoundEmitter3D *emitter = Get3DEmitter(handle);
	if (!emitter)
		return false;

	emitter->Detach(handle);
	return true;
}

//***************************************************************************
// This will stop the sound and null the handle
//***************************************************************************
void Sfx::Stop(
SoundHandle &handle)
{
	// Okay to pass me null
	if (!handle)
		return;

	// Resolve the handle from the emitter
	SoundEmitter *emitter = GetEmitter(handle);

	// If we have an emitter, allow it to kill it.
	if (emitter)
		emitter->Kill(handle);
	else
		delete handle;

	// Null it out for them
	handle = NULL;
}

//***************************************************************************
// Just free the handle without stoping sound
//***************************************************************************
void Sfx::ReleaseHandle(
SoundHandle &handle)
{
	// Get rid of it!
	delete handle;
	handle = NULL;
}

//***************************************************************************
// Access the emitters directly
//***************************************************************************
SoundEmitter2D	&Sfx::Get2DEmitter()
{
	ASSERT(emitter2D != NULL);
	return *emitter2D;
}

//***************************************************************************
//***************************************************************************
SoundEmitter3D	&Sfx::Get3DEmitter()
{
	ASSERT(emitter3D != NULL);
	return *emitter3D;
}

//***************************************************************************
//***************************************************************************
SoundEmitterEnvironment &Sfx::GetEnvironmentEmitter()
{
	ASSERT(emitterEnv != NULL);
	return *emitterEnv;
}

//***************************************************************************
//***************************************************************************
const Vector3 *Sfx::GetActorPlayPos(CActor &actor, ActorPlayPos actorPos)
{
	if (actorPos == ACTORPLAYPOS_BODY)
		return &actor.GetBodyInWorld();
	else //ACTORPLAYPOS_FOCUS, ACTORPLAYPOS_DEFAULT
		return &actor.GetFocusInWorld();
}

//***************************************************************************
//***************************************************************************
bool Sfx::ActorValidCallback(void *cookie)
{
	return (CActor::FromHandle((ActorHandle)cookie) != NULL);
}

//***************************************************************************
// Utilities
//***************************************************************************
SoundEmitter *Sfx::GetEmitter(
SoundHandle handle)
{
	// Null handle is a null emitter
	if (!handle)
		return NULL;

	// Still has a channel?
	SoundChannel *channel = handle->Channel();
	if (!channel)
		return NULL;

	// Get the channel's emitter
	return &channel->Emitter();
}

//***************************************************************************
//***************************************************************************
SoundEmitter3D *Sfx::Get3DEmitter(
SoundHandle handle)
{
	// Resolve it into an ordinary emitter
	SoundEmitter *emitter = GetEmitter(handle);
	if (!emitter)
		return NULL;

	// Is it a 3D emitter
	ASSERT(emitter->Type() == SoundEmitter::EMITTER3D);
	if (emitter->Type() != SoundEmitter::EMITTER3D)
		return NULL;

	// Upcast it and give it back
	return (SoundEmitter3D *)emitter;
}

//***************************************************************************
//***************************************************************************
const char *Sfx::Get2DGroupName()
{
	return "Sfx2DGroup";
}

//***************************************************************************
//***************************************************************************
const char *Sfx::Get3DGroupName()
{
	return "Sfx3DGroup";
}

//***************************************************************************
//***************************************************************************
const char *Sfx::GetEnvironmentGroupName()
{
	return "SfxEnvGroup";
}

//***************************************************************************
//***************************************************************************
void Sfx::Daemon()
{
	float now = g_timer.GetEndSec();
	QueuedSound *sound = soundQueue.Head();
	while (sound)
	{
		QueuedSound *next = sound->next;
		if (now > sound->playTime)
		{
			// Do it
			PlayStationary(sound->name, sound->pos, sound->volume, sound->pitch);

			// Kill it
			soundQueue.Unlink(sound);
			soundPool.Delete(sound);
		}

		// next!
		sound = next;
	}
}

//***************************************************************************
// Debugging (pass in NULL for pos if you want all)
//***************************************************************************
void Sfx::Debug(
const Vector3 *cameraPos)
{
	if (emitterEnv)
		emitterEnv->Debug(cameraPos);
}

//***************************************************************************
//***************************************************************************
void Sfx::ClearHandle(
SoundHandle *handle)
{
	if (handle)
		*handle = NULL;
}

//***************************************************************************
//***************************************************************************
void Sfx::SetHandle(
SoundHandle *handle,
SoundHandle sound)
{
	if (handle)
		*handle = sound;
	else
		delete sound;
}

