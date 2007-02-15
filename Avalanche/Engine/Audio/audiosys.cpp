/***************************************************************************/
// Audio system
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "Math/sfilter.h"

// The singleton
Audio *g_audioPtr = NULL;

// Some nice volume interpolation time constants
const float Audio::smoothVolumeInstant = 0.0f;
const float Audio::smoothVolumeFast = 0.5f;
const float Audio::smoothVolumeSlow = 1.0f;
const float Audio::smoothVolumeVerySlow = 2.5f;

// Fader names
const char *Audio::volumeGroupName[VOLUMEGROUPS] =
{
	"FULL",		// Always returns one!
	"SFX",		// SFX
	"DIALOG",	// Dialog
	"SFX2D",		// 2D emitters
	"SFX3D",		// 3D emitters
	"MUSIC",		// music
};

/***************************************************************************/
// Setup stuff
/***************************************************************************/
Audio::Audio() :
groupList(8, 4, false),
aliasList(8, 4, false),
group2DPool(8, 4),
group3DPool(16, 8),
groupAliasPool(8, 4)
{
	// Don't ask me about this, I don't know
	groupList.Lock();
	aliasList.Lock();
	group2DPool.Lock();
	group3DPool.Lock();
	groupAliasPool.Lock();

	// We ain't paused
	paused = false;

	// Create the approprate faders
	for (int i = 0; i < VOLUMEGROUPS; i++)
	{
		fader[i].Setup(volumeGroupName[i]);
		faderBoard.Add(&fader[i]);
	}

	// Setup some relationships
	fader[DIALOG].SetParent(&fader[SFX]);
	fader[SFX2D].SetParent(&fader[SFX]);
	fader[SFX3D].SetParent(&fader[SFX]);

	// We always return full volume
	fader[FULL].SetVolume(1.0f);

	// Make the main volume fade in
	fader[SFX].SetVolume(0.0);
	fader[MUSIC].SetVolume(0.0);

	// Our master pitch
	masterPitch = 0.0f;
}

/***************************************************************************/
// clean up stuff
/***************************************************************************/
Audio::~Audio()
{
	Destroy();

	// Remove our faders (they can't be delete'd)
	for (int i = 0; i < VOLUMEGROUPS; i++)
		faderBoard.Remove(&fader[i]);
}

/***************************************************************************/
// Create sound device
/***************************************************************************/
bool Audio::Create(
SoundDriver::OSCREATEINFO osInfo)
{
	// Create the driver and setup the parameters
	driver = SoundDriver::CreateAndSetOutputFormat(osInfo);
	if (!driver)
		return false;

	// Create the sound handle system
	SoundHandleData::Initialize();

	// Init the jukebox & dialog system
	jukebox.Initialize(*driver);
	dialog.Initialize(*driver);

	// We are set!
	return true;
}

/***************************************************************************/
// Mount the stream volumes
/***************************************************************************/
bool Audio::MountStreamLibrary()
{
	if (!driver)
		return false;

	return driver->MountStreamLibrary();
}

/***************************************************************************/
// Destroy stuff
/***************************************************************************/
void Audio::Destroy()
{
	// Get rid of everything
	Flush(HARDFLUSH);

	// Terminate
	dialog.Terminate();
	jukebox.Terminate();

	// Destroy the sound handle system
	SoundHandleData::Terminate();

	// Kill the driver
	if (driver)
		driver->Destroy();

	driver = NULL;
}

/***************************************************************************/
// Pause all currently playing sounds, won't prevent new sounds from playing
/***************************************************************************/
void Audio::Pause(
bool pauseJukebox)
{
	paused = true;

	if (pauseJukebox)
		jukebox.Pause(true);

	dialog.Pause(true);

	// Don't use the driver pause, it isn't smart enough
	for (int i = 0; i < groupList.Count(); i++)
		groupList[i]->Pause();

	// Tell the driver
	if (driver)
		driver->Pause(SoundDriver::PAUSESTREAMS);
}

/***************************************************************************/
// Unpause all currently paused sounds
/***************************************************************************/
void Audio::Unpause(
bool unpauseJukebox)
{
	if (paused)
	{
		paused = false;

		// Don't use the driver pause, it isn't smart enough
		for (int i = 0; i < groupList.Count(); i++)
			groupList[i]->Unpause();

		dialog.Pause(false);

		if (unpauseJukebox)
			jukebox.Pause(false);

		// Tell the driver
		if (driver)
			driver->Unpause(SoundDriver::PAUSESTREAMS);
	}
}

/***************************************************************************/
/***************************************************************************/
bool Audio::Paused()
{
	return paused;
}

/***************************************************************************/
// Flush out all emitters and banks, but keep the driver active
/***************************************************************************/
void Audio::Flush(
AUDIOFLUSH type,
bool clearDialog)
{
	// Let's stop the music
	jukebox.Stop();
	jukebox.Clear();
	jukebox.ClearPlayPositions();
	g_audio.jukebox.SetMode(Jukebox::JUKEBOX_STEREO);

	if (clearDialog)
	{
		// Kill all dialog
		dialog.Stop();
		dialog.Clear();
	}

	// Force all the marked emitters to DIE now!
	SoundEmitter *emitter = emitterList.Head();
	while (emitter)
	{
		SoundEmitter *next = emitter->next;

		if (strcmpi(emitter->Name(), "Fanbase_a4") == 0)
		{
			TRACE("this is it!");
		}

		// Do we kill them?
		if (emitter->MarkedForDeath() || emitter->ReadyToDie() || emitter->DieOnSoftFlush())
			emitterList.Remove(emitter);
		else
			emitter->KillAll();

		// Next!
		emitter = next;
	}

	// Check the status of the handles
	SoundHandleTracker::Report();

	// Kill empty groups if marked
	for (int g = 0; g < groupList.Count(); g++)
	{
		// The group in question
		SoundGroup *group = groupList[g];
		ASSERT(group);

		// Let it update (since emitters may have freed stuff)
		group->Daemon();

		// Try to shrink
		group->Shrink();

		if (group->DestroyWhenEmpty())
		{
			SoundEmitter *emitter = emitterList.Head();
			while (emitter)
			{
				if (emitter->IsGroup(group))
					break;
				emitter = emitter->next;
			}
			if (!emitter)
			{
				DeleteGroup(group);
				groupList.Delete(g);
			}
		}
	}

	// Bomb the bass?
	if (type == HARDFLUSH)
	{
		int i;

		// kill the emitters
		SoundEmitter *emitter = emitterList.Head();
		while (emitter)
		{
			emitterList.Remove(emitter);
			emitter = emitterList.Head();
		}

		// Kill the aliases
		for (i = 0; i < aliasList.Count(); i++)
			groupAliasPool.Delete(aliasList[i]);
		aliasList.Clear();

		// Kill the groups
		for (i = 0; i < groupList.Count(); i++)
			DeleteGroup(groupList[i]);
		groupList.Clear();

		// Kill all the banks (driver)
		if (driver)
			driver->UnloadAllBanks();

		// Reset the handle system
		SoundHandleTracker::Clear();
		SoundHandleData::Reset();
	}

	int flushed = CommonStreamInterface::FlushAutoDelete();
	TRACE("Flushed %d active CSI streams.\n", flushed);

	if (driver)
	{
		// Clear all 3d streams
		int flushed = driver->Flush3DStreams();
		TRACE("Flushed %d active driver streams.\n", flushed);

		// Clear the effects
		driver->SetEnvironmentEffect(SoundDriver::DEFAULT, 0.0f);

		// Call the driver update
		driver->Daemon();
	}
}

/***************************************************************************/
// Set the volume (0 - 1) for a group
/***************************************************************************/
bool Audio::SetVolume(
VolumeGroup group,
float volume,
float smooth)
{
	ASSERT(volume >= 0.0f && volume <= 1.0f);
	fader[group].SetVolume(volume, smooth);
	return true;
}

/***************************************************************************/
/***************************************************************************/
bool Audio::SetVolumeFactor(
VolumeGroup group,
float factor)
{
	ASSERT(factor >= 0.0f && factor <= 1.0f);
	fader[group].SetVolumeFactor(factor);
	return true;
}

/***************************************************************************/
// Get the group volume (0 - 1)
/***************************************************************************/
float Audio::GetVolume(
VolumeGroup group)
{
	return fader[group].Volume();
}

/***************************************************************************/
// Set the master pitch
/***************************************************************************/
void Audio::SetMasterPitch(
float semitones,
bool includeStream2D)
{
	if (!Math::Equal(masterPitch, semitones))
	{
		// Save it
		masterPitch = semitones;

		// tell all the playing sounds
		SoundInstance::SetMasterPitch(masterPitch);
		if (includeStream2D)
			SoundStream::SetMasterPitch(masterPitch);
		SoundStream3D::SetMasterPitch(masterPitch);
	}
}

/***************************************************************************/
/***************************************************************************/
void Audio::InferMasterPitch(
float timeFactor,
bool includeStream2D)
{
	if (Math::Zero(timeFactor))
		SetMasterPitch(0.0f, includeStream2D);
	else
		SetMasterPitch(12.0f * Math::Log2(timeFactor), includeStream2D);
}

/***************************************************************************/
/***************************************************************************/
float Audio::GetMasterPitch()
{
	return masterPitch;
}

/***************************************************************************/
// Get the current mode
/***************************************************************************/
SoundDriver::OUTPUTMODE Audio::GetOutputMode()
{
	ASSERT(driver);

	if( !driver ) return( SoundDriver::OUTPUTMODE_STEREO );
	return driver->GetOutputMode();
}

/***************************************************************************/
// Set the mode, returns false if not supported
/***************************************************************************/
bool Audio::SetOutputMode(
SoundDriver::OUTPUTMODE mode)
{
	ASSERT(driver);

	if( !driver ) return( false );
	return driver->SetOutputMode(mode);
}

/***************************************************************************/
// Test to see if this mode is supported
/***************************************************************************/
bool Audio::OutputModeSupported(
SoundDriver::OUTPUTMODE mode)
{
	ASSERT(driver);

	if( !driver ) return( false );
	return driver->OutputModeSupported(mode);
}

/***************************************************************************/
// set the camera position for 3d sound
// You can set up to MAX_LISTENERS listeners (for 4-player split screen)
/***************************************************************************/
bool Audio::CreateListener(
const Cartesian4x4 *listenerCamera)
{
	if (!driver)
		return false;

	return driver->CreateListener(listenerCamera);
}

/***************************************************************************/
// Destroy all created listeners
/***************************************************************************/
bool Audio::DestroyListeners()
{
	// Something bad happened?
	if (!driver)
		return false;

	return driver->DestroyListeners();
}

/***************************************************************************/
// Return the best listener
/***************************************************************************/
SoundListener *Audio::BestListener(
Vector3CRef forPos)
{
	if (!driver)
		return NULL;

	SoundListener &listener = driver->BestListener(forPos);
	return &listener;
}

/***************************************************************************/
// Load a sound bank
/***************************************************************************/
bool Audio::LoadSoundBankChunk(
void *data,
ts_DBChunkHdr *chunkHdr,
void **retData,
u32 *dataSize)
{
	if (!driver)
		return false;

	// Make sure these aren't hogging shit
#ifdef PS2
	jukebox.Clear();
	dialog.Clear();
#endif

	return driver->LoadSoundBankChunk(data, chunkHdr, retData, dataSize);
}

/***************************************************************************/
/***************************************************************************/
SoundBank *Audio::LoadBank(const char *bankName)
{
	if (!driver)
		return false;

	// Make sure these aren't hogging shit
#ifdef PS2
	jukebox.Clear();
	dialog.Clear();
#endif

	return driver->LoadBank(bankName);
}

/***************************************************************************/
// How many banks do we have?
/***************************************************************************/
int Audio::Banks()
{
	if (!driver)
		return 0;

	return driver->Banks();
}

/***************************************************************************/
/***************************************************************************/
bool Audio::UnloadBank(
SoundBank *killBank)
{
	if (!driver)
		return true;

	return driver->UnloadBank(killBank);
}

/***************************************************************************/
// Create destroy a group
/***************************************************************************/
bool Audio::Create2DGroup(
const char *groupName,
int channels,
Reclaim::METHOD reclaimMethod)
{
	// Have they already made the group?
	if (GetGroup(groupName))
		return true;

	// Allocate the memory
	void *memory = group2DPool.NewPtr();
	if (!memory)
		return false;

	// Create it
	SoundGroup2D *group = new (memory) SoundGroup2D(groupName, *driver, channels, Reclaim::Get(reclaimMethod));

	// Add it to the list
	groupList.Add(group);

	// Nice
	TRACE("2D Group %s created using %d channels. %d of %d channels used. %s\n",
			groupName, channels, AllocatedChannels(), MaxChannels(), AllocatedChannels() > MaxChannels() ?
			"WARNING channels overallocated!!!!" : "");

	return true;
}

/***************************************************************************/
// Create destroy a group
/***************************************************************************/
bool Audio::Create3DGroup(
const char *groupName,
int channels,
Reclaim::METHOD reclaimMethod)
{
	// Have they already made the group?
	if (GetGroup(groupName))
		return true;

	// Allocate the memory
	void *memory = group3DPool.NewPtr();
	if (!memory)
		return false;

	// Create it
	SoundGroup3D *group = new (memory) SoundGroup3D(groupName, *driver, channels, Reclaim::Get(reclaimMethod));

	// Add it to the list
	groupList.Add(group);

	// Nice
	TRACE("3D Group %s created using %d channels. %d of %d channels used. %s\n",
			groupName, channels, AllocatedChannels(), MaxChannels(), AllocatedChannels() > MaxChannels() ?
			"WARNING channels overallocated!!!!" : "");

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool Audio::AliasGroup(
const char *aliasName,
const char *groupName)
{
	// Did they already add it?
	if (FindAlias(aliasName))
		return true;

	// Find the group
	SoundGroup *group = GetGroup(groupName);
	if (!group)
		return false;

	// Allocate it
	void *memory = groupAliasPool.NewPtr();
	if (!memory)
		return false;

	// Make a new one
	SoundGroupAlias *alias = new (memory) SoundGroupAlias(aliasName, group);

	// Add it to the list!
	aliasList.Add(alias);

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool Audio::DestroyGroup(
const char *groupName)
{
	// Is this an alias?
	int i;
	for (i = 0; i < aliasList.Count(); i++)
	{
		if (strcmpi(aliasList[i]->Name(), groupName) == 0)
		{
			groupAliasPool.Delete(aliasList[i]);
			aliasList.Delete(i);
			return true;
		}
	}

	// Is it a group?
	for (i = 0; i < groupList.Count(); i++)
	{
		if (strcmpi(groupList[i]->Name(), groupName) == 0)
		{
			DeleteGroup(groupList[i]);
			groupList.Delete(i);
			return true;
		}
	}

	return false;
}

/***************************************************************************/
/***************************************************************************/
bool Audio::DestroyGroupWhenEmpty(
const char *groupName,
bool destroy)
{
	for (int i = 0; i < groupList.Count(); i++)
	{
		if (strcmpi(groupList[i]->Name(), groupName) == 0)
		{
			groupList[i]->SetDestroyWhenEmpty(destroy);
			return true;
		}
	}
	return false;
}

/***************************************************************************/
// Ask the driver what the max channels are
/***************************************************************************/
int Audio::MaxChannels()
{
	if (!driver)
		return 0;

	return driver->MaxChannels();
}

/***************************************************************************/
// How many channels are allocated to groups?
/***************************************************************************/
int Audio::AllocatedChannels()
{
	int allocated = 0;

	for (int i = 0; i < groupList.Count(); i++)
		allocated += groupList[i]->MaxChannels();

	return allocated;
}

/***************************************************************************/
// What is the total number of channels currently being used
/***************************************************************************/
int Audio::UsedChannels()
{
	int used = 0;

	for (int i = 0; i < groupList.Count(); i++)
		used += groupList[i]->UsedChannels();

	return used;
}

/***************************************************************************/
// Create a 2d sound emitter
/***************************************************************************/
SoundEmitter2D *Audio::Create2DEmitter(
const char *name,
const char *groupName)
{
	if (!driver)
		return NULL;

	// Get the group
	SoundGroup *group = GetGroup(groupName);
	ASSERT(group);
	if (!group)
		return NULL;
	ASSERT(group->Type() == SoundGroup::GROUP2D);

	// Create it
	SoundEmitter2D *emitter = new SoundEmitter2D(name, *driver, *group);
	if (!emitter)
		return NULL;

	// Set the default fader
	emitter->SetFader(&fader[SFX2D]);

	// Add it to our managed list
	emitterList.AddTail(emitter);
	return emitter;
}

/***************************************************************************/
// Create a 3d sound emitter
/***************************************************************************/
SoundEmitter3D *Audio::Create3DEmitter(
const char *name,
const char *groupName)
{
	if (!driver)
		return NULL;

	// Get the group
	SoundGroup *group = GetGroup(groupName);
	ASSERT(group);
	if (!group)
		return NULL;
	ASSERT(group->Type() == SoundGroup::GROUP3D);

	// Create it
	SoundEmitter3D *emitter = new SoundEmitter3D(name, *driver, *group);
	if (!emitter)
		return NULL;

	// Set the default fader
	emitter->SetFader(&fader[SFX3D]);

	// Add it to our managed list
	emitterList.AddTail(emitter);
	return emitter;
}

/***************************************************************************/
// Create a 3d string sound emitter
/***************************************************************************/
SoundEmitter3DString *Audio::Create3DStringEmitter(
const char *name,
const char *groupName)
{
	if (!driver)
		return NULL;

	// Get the group
	SoundGroup *group = GetGroup(groupName);
	ASSERT(group);
	if (!group)
		return NULL;
	ASSERT(group->Type() == SoundGroup::GROUP3D);

	// Create it
	SoundEmitter3DString *emitter = new SoundEmitter3DString(name, *driver, *group);
	if (!emitter)
		return NULL;

	// Set the default fader
	emitter->SetFader(&fader[SFX3D]);

	// Add it to our managed list
	emitterList.AddTail(emitter);
	return emitter;
}

/***************************************************************************/
// Create an environmental emitter
/***************************************************************************/
SoundEmitterEnvironment *Audio::CreateEnvironmentEmitter(
const char *name,
const char *groupName)
{
	if (!driver)
		return NULL;

	// Get the group
	SoundGroup *group = GetGroup(groupName);
	ASSERT(group);
	if (!group)
		return NULL;
	ASSERT(group->Type() == SoundGroup::GROUP3D);

	// Create it
	SoundEmitterEnvironment *emitter = new SoundEmitterEnvironment(name, *driver, *group);
	if (!emitter)
		return NULL;

	// Set the default fader
	emitter->SetFader(&fader[SFX3D]);

	// Add it to our managed list
	emitterList.AddTail(emitter);
	return emitter;
}

/***************************************************************************/
/***************************************************************************/
int Audio::Emitters()
{
	return emitterList.Items();
}

/***************************************************************************/
/***************************************************************************/
SoundEmitter *Audio::GetEmitter(
const char *emitterName)
{
	SoundEmitter *emitter = emitterList.Head();
	while (emitter)
	{
		if (strcmpi(emitter->Name(), emitterName) == 0)
			return emitter;

		emitter = emitter->next;
	}

	return NULL;
}

/***************************************************************************/
// Destroy an emitter, calls delete!
/***************************************************************************/
bool Audio::DestroyEmitter(
const char *emitterName,
bool dieSoftly)
{
	SoundEmitter *emitter = GetEmitter(emitterName);
	if (!emitter)
		return false;

	return DestroyEmitter(emitter, dieSoftly);
}

/***************************************************************************/
// Destroy a emitter, calls delete!
/***************************************************************************/
bool Audio::DestroyEmitter(
SoundEmitter *killEmitter,
bool dieSoftly)
{
	if (!killEmitter)
		return false;

	// Softly or not?
	if (!dieSoftly)
		emitterList.Remove(killEmitter);
	else
		killEmitter->MarkForDeath();

	return true;
}

/***************************************************************************/
// What is the maximum number of streams that can be simultaneously played?
/***************************************************************************/
int Audio::Max3DStreams()
{
	return driver ? driver->Max3DStreams() : 0;
}

/***************************************************************************/
/***************************************************************************/
int Audio::Current3DStreams()
{
	return driver ? driver->Current3DStreams() : 0;
}

/***************************************************************************/
/***************************************************************************/
CommonStreamInterface *Audio::CreateStream2D(
const char *fileName,
bool repeat,
bool autoDelete,
bool preLoad,
VolumeGroup volumeGroup)
{
	if (!driver)
		return NULL;

	CommonStreamInterface *stream = CommonStreamInterface::Create2D(*driver, fileName, repeat, autoDelete, preLoad);
	if (!stream)
		return NULL;

	stream->BindFader(&fader[volumeGroup]);
	return stream;
}

/***************************************************************************/
/***************************************************************************/
CommonStreamInterface *Audio::CreateStream3D(
const char *fileName,
bool repeat,
bool autoDelete,
SoundStream3D::Priority priority,
VolumeGroup volumeGroup)
{
	if (!driver)
		return NULL;

	CommonStreamInterface *stream = CommonStreamInterface::Create3D(*driver, fileName, repeat, autoDelete, priority);
	if (!stream)
		return NULL;

	// Bind in the fader
	stream->BindFader(&fader[volumeGroup]);

	// Set the default 3D params
	SoundStream3D *stream3D = stream->Stream3D();
	ASSERT(stream3D);
	stream3D->SetParams(stream3DParams);

	return stream;
}

/***************************************************************************/
/***************************************************************************/
CommonStreamInterface *Audio::CreateStream2D(
SoundDriver::SONGID id,
bool repeat,
bool autoDelete,
VolumeGroup volumeGroup)
{
	if (!driver)
		return NULL;

	CommonStreamInterface *stream = CommonStreamInterface::Create2D(*driver, id, repeat, autoDelete);
	if (!stream)
		return NULL;

	stream->BindFader(&fader[volumeGroup]);
	return stream;
}

/***************************************************************************/
// Call this every frame
/***************************************************************************/
void Audio::Daemon()
{
	// If they didn't create the driver get out
	if (!driver)
		return;

	// Update volumes
	float deltaT = g_timer.GetFrameSec();
	faderBoard.Update(deltaT);

	// This is temporary (until they support faders)
 	jukebox.SetVolume(fader[MUSIC].MixVolume());
	dialog.SetVolume(fader[DIALOG].MixVolume());

	// Call the driver update
	driver->Daemon();

	// Allow the jukebox & dialog to process
	jukebox.Daemon();
	dialog.Daemon();

	// Update the auto streams
	CommonStreamInterface::UpdateAutoDelete();

	// call all the emitter updates
	SoundEmitter *emitter = emitterList.Head();
	while (emitter)
	{
		SoundEmitter *next = emitter->next;

		// Call the update
		emitter->Daemon();

		// Do we kill them?
		if (emitter->ReadyToDie())
			emitterList.Remove(emitter);

		// Next!
		emitter = next;
	}

	// call all the group updates
	int count = groupList.Count();
	for (int i = 0; i < count; i++)
		groupList[i]->Daemon();
}

/***************************************************************************/
// Special support for the FMV
/***************************************************************************/
void Audio::PrepareForFMV()
{
	if (driver)
		driver->PrepareForFMV();
}

/***************************************************************************/
/***************************************************************************/
void Audio::RecoverFromFMV()
{
	if (driver)
		driver->RecoverFromFMV();
}

/***************************************************************************/
/***************************************************************************/
SoundGroup *Audio::GetGroup(
const char *groupName)
{
	// Check the group list
	int i;
	for (i = 0; i < groupList.Count(); i++)
	{
		if (strcmpi(groupList[i]->Name(), groupName) == 0)
			return groupList[i];
	}

	// Check the alias list...
	for (i = 0; i < aliasList.Count(); i++)
	{
		if (strcmpi(aliasList[i]->Name(), groupName) == 0)
			return aliasList[i]->Group();
	}

	// No matches
	return NULL;
}

/***************************************************************************/
/***************************************************************************/
SoundGroupAlias *Audio::FindAlias(
const char *aliasName)
{
	for (int i = 0; i < aliasList.Count(); i++)
	{
		if (strcmpi(aliasList[i]->Name(), aliasName) == 0)
			return aliasList[i];
	}

	return NULL;
}

/***************************************************************************/
/***************************************************************************/
void Audio::DeleteGroup(
SoundGroup *group)
{
	if (!group)
		return;

	if (group->Type() == SoundGroup::GROUP2D)
		group2DPool.Delete((SoundGroup2D *)group);
	else
		group3DPool.Delete((SoundGroup3D *)group);
}

