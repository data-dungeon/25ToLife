/***************************************************************************/
// Sound driver
/***************************************************************************/
#include "Audio/AudioPCH.h"

//#define DISABLE_SOUND

/***************************************************************************/
// The usual
/***************************************************************************/
SoundDriver::SoundDriver() :
bankStack(8, 8, false)
{
	MEM_SET_BLOCK_NAME(bankStack.ArrayAddress(), "bankStack");	// have to set it after malloc since it's done as part of routine declaration
	bankStack.Lock();	// this is permanent memory
	outputMode = OUTPUTMODE_STEREO;

	// Setup the listeners
	listeners = 0;
	for (int i = 0; i < MAX_LISTENERS; i++)
		listener[i] = NULL;

	// no streams yet
	current3DStreams = 0;
}

/***************************************************************************/
/***************************************************************************/
SoundDriver::~SoundDriver()
{
}

/***************************************************************************/
// Get the current mode
/***************************************************************************/
SoundDriver::OUTPUTMODE SoundDriver::GetOutputMode()
{
	return outputMode;
}

/***************************************************************************/
// Set the mode, returns false if not supported
/***************************************************************************/
bool SoundDriver::SetOutputMode(OUTPUTMODE mode)
{
	ASSERT(OutputModeSupported(mode));

	if (!OutputModeSupported(mode))
		return false;

	// subclass must do the real work
	outputMode = mode;
	return true;
}

/***************************************************************************/
// Stream a platform specific file off of the disk w/3D
// DO NOT PUT AN EXTENSION ON THIS NAME!
// Some platforms may not support this at all
/***************************************************************************/
SoundStream3D *SoundDriver::CreateStream3D(
const char *fileName,
bool loop,
SoundStream3D::Priority priority)
{
	// Make sure this exists
	if (!StreamExist(fileName))
		return NULL;

	// Create it
	SoundStream3D *stream = new SoundStream3D(*this, fileName, loop, priority);
	ASSERT(stream);
	if (!stream)
		return NULL;

	// Give them the stream
	Add3DStream(stream);
	return stream;
}


/***************************************************************************/
// Flush all playing 3d streams (returns number flushed)
/***************************************************************************/
int SoundDriver::Flush3DStreams()
{
	int flushed = 0;

	SoundStream3D *stream = soundStream3D.Head();
	while (stream)
	{
		SoundStream3D *next = stream->next;
		if (!stream->IsLost())
		{
			stream->Release();
			++flushed;
		}
		stream = next;
	}

	return flushed;
}

/***************************************************************************/
// Destroy it, don't count on destructor
/***************************************************************************/
bool SoundDriver::Destroy()
{
	// Make sure all the sound banks have been destroyed
	UnloadAllBanks();

	return true;
}

/***************************************************************************/
// High speed timer (in ms)
/***************************************************************************/
uint SoundDriver::Time()
{
	return g_timer.GetOSTime();
}

/***************************************************************************/
// Call this frequently (once per frame).  Threads are nice, but messy
/***************************************************************************/
void SoundDriver::Daemon()
{
	// Update the listeners
	for (int i = 0; i < listeners; i++)
		listener[i]->Update();

	// Do the streams
	if (listeners > 0)	//rhn, Food we put this "if" here to fix a crash, is this ok?
	{
		SoundStream3D *stream = soundStream3D.Head();
		while (stream)
		{
			stream->Update();
			stream = stream->next;
		}
	}
}

/***************************************************************************/
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
/***************************************************************************/
const char *SoundDriver::TokenizeStreamName(const char *streamFile)
{
	// Who cares, pretend it is all around
#if 0
	// Make sure it is around
	if (!StreamExist(streamFile))
		return NULL;
#else
	if (!streamFile || !*streamFile)
		return NULL;
#endif

	// Make a copy
	char *token = (char *)memAlloc(strlen(streamFile) + 1);
	if (token)
		strcpy(token, streamFile);
	return token;
}

/***************************************************************************/
/***************************************************************************/
void SoundDriver::ReleaseStreamNameToken(const char *streamToken)
{
	memFree((void *)streamToken);
}

/***************************************************************************/
// Load a sound bank, will automatically use BaseDataPath
// Set for3D to true if you are going to use this bank in a 3D emitter
// If you load twice, it is only loaded once and the pointer is
// returned.
/***************************************************************************/
SoundBank *SoundDriver::LoadBank(const char* bankName)
{
	SoundBank* bank = NULL;
	char fileName[128];
	SoundBank::MakeBankLoadName( bankName, BaseDataPath(), fileName );
	t_Handle handle = dblLoad( fileName );
	if ( handle != INVALID_HANDLE )
		bank = (SoundBank*) dblGetChunkData( DB_SOUNDBANK_MOD, DEFAULT_CHUNK_KEY_STRING, handle );
	return bank;
}

/***************************************************************************/
/***************************************************************************/
bool SoundDriver::LoadSoundBankChunk(
void *data,
ts_DBChunkHdr *chunkHdr,
void **retData,
u32 *dataSize)
{
	chunkHdr->u16Flags |= DBL_FLUSHABLE;

	// Create one
	SoundBank *soundBank = CreateBank();
	if (!soundBank)
		return false;

	// Load it
	if (!soundBank->Load(data))
	{
		soundBank->MarkForDelete();
		DestroyBank(soundBank);
		return false;
	}

	// add it to the stack (at the end)
	bankStack.Add(soundBank);

	*retData = soundBank;

	return true;
}

/***************************************************************************/
// How many banks do we have loaded?
/***************************************************************************/
int SoundDriver::Banks()
{
	return bankStack.Count();
}

/***************************************************************************/
// Unload a bank, may not truly unload until
// everything loaded after this bank is released.
/***************************************************************************/
bool SoundDriver::UnloadBank(
SoundBank *killBank)
{
	// This is legal
	if (!killBank)
		return false;

	// Mark the bank for deletion
	killBank->MarkForDelete();

	// Garbage collect the bank stack
	GarbageCollectBanks();

	// Could not find it
	return true;
}

/***************************************************************************/
// Call this to flush all the banks!
/***************************************************************************/
bool SoundDriver::UnloadAllBanks()
{
	// Mark all the banks for deletion
	for (int i = 0; i < bankStack.Count(); i++)
		bankStack[i]->MarkForDelete();

	// Garbage collect the bank stack
	GarbageCollectBanks();

	return true;
}

/***************************************************************************/
// set the camera position for 3d sound
// You can set up to MAX_LISTENERS listeners (for 4-player split screen)
/***************************************************************************/
bool SoundDriver::CreateListener(
const Cartesian4x4 *listenerCamera)
{
	// Make sure this is in range
	ASSERT(listeners < MAX_LISTENERS);
	ASSERT(listenerCamera);

	// Get out on an error
	if (!listenerCamera || listeners == MAX_LISTENERS)
		return false;

	// Create the listener
	listener[listeners] = NewListener(*listenerCamera);
	if (!listener[listeners])
		return false;

	++listeners;
	return true;
}

/***************************************************************************/
// Destroy all created listeners
/***************************************************************************/
bool SoundDriver::DestroyListeners()
{
	// Any work to do?
	for (int i = 0; i < listeners; i++)
	{
		DeleteListener(listener[i]);
		listener[i] = NULL;
	}

	// We are clean
	listeners = 0;
	return true;
}

/***************************************************************************/
/***************************************************************************/
int SoundDriver::Listeners()
{
	return listeners;
}

/***************************************************************************/
/***************************************************************************/
SoundListener *SoundDriver::Listener(int who)
{
	ASSERT(who >= 0 && who < listeners);
	return listener[who];
}

/***************************************************************************/
// What is the best listener
// (just closest, but more criteria could be considered
// perhaps the two could be averged...)
/***************************************************************************/
SoundListener &SoundDriver::BestListener(
Vector3CRef forPos)
{
	//ASSERT(listeners > 0);
	// If there aren't any listeners then return a NULL address.
	if (listeners == 0)
		return (SoundListener &)*(int*)NULL;

	// Find the closest listener
	int which = 0;
	float bestD = listener[which]->DistanceSquared(forPos);
	for (int i = 1; i < listeners; i++)
	{
		float d = listener[i]->DistanceSquared(forPos);
		if (d < bestD)
		{
			bestD = d;
			which = i;
		}
	}

	return *listener[which];
}

/***************************************************************************/
// Return distance squared to closest listenter
/***************************************************************************/
float SoundDriver::DistanceSquaredClosestListener(
Vector3CRef pos)
{
	// This would be very bad
	ASSERT(listeners > 0);

	// Find the closest listener
	float bestD = listener[0]->DistanceSquared(pos);
	for (int i = 1; i < listeners; i++)
	{
		float d = listener[i]->DistanceSquared(pos);
		if (d < bestD)
			bestD = d;
	}

	return bestD;
}

/***************************************************************************/
// Create a listener
/***************************************************************************/
SoundListener *SoundDriver::NewListener(
const Cartesian4x4 &worldToListener)
{
	return new SoundListener(worldToListener);
}

/***************************************************************************/
// Destroy a listener
/***************************************************************************/
void SoundDriver::DeleteListener(
SoundListener *listener)
{
	delete listener;
}

/***************************************************************************/
// Pause all currently playing sounds
/***************************************************************************/
void SoundDriver::Pause(PauseType pauseType)
{
	// Do SFX
	if ((int)pauseType & PAUSESFX)
	{
		// Go through all the banks pausing sounds
		for (int i = 0; i < bankStack.Count(); i++)
		{
			SoundBank *bank = bankStack.Get(i);
			for (int s = 0; s < bank->Sources(); s++)
			{
				// Each sound knows all it's instances
				Sound *sound = bank->GetSourceEngineSound(s);

				if (sound)
					sound->Pause();
			}
		}
	}

	// Do streams
	if ((int)pauseType & PAUSESTREAMS)
	{
		SoundStream3D *stream = soundStream3D.Head();
		while (stream)
		{
			stream->Pause();
			stream = stream->next;
		}
	}
}

/***************************************************************************/
// Resume all currently paused sounds
/***************************************************************************/
void SoundDriver::Unpause(PauseType pauseType)
{
	if ((int)pauseType & PAUSESFX)
	{
		// Go through all the banks pausing sounds
		for (int i = 0; i < bankStack.Count(); i++)
		{
			SoundBank *bank = bankStack.Get(i);
			for (int s = 0; s < bank->Sources(); s++)
			{
				// Each sound knows all it's instances
				Sound *sound = bank->GetSourceEngineSound(s);

				if (sound)
					sound->Unpause();
			}
		}
	}

	// Do streams
	if ((int)pauseType & PAUSESTREAMS)
	{
		SoundStream3D *stream = soundStream3D.Head();
		while (stream)
		{
			stream->Unpause();
			stream = stream->next;
		}
	}
}

/***************************************************************************/
// Resolve an effect name into an enum
/***************************************************************************/
SoundDriver::ENVEFFECT SoundDriver::ResolveEnvironmentEffectName(
const char *name)
{
	if (strcmpi("PADDEDCELL", name) == 0)
		return SoundDriver::PADDEDCELL;
	else if (strcmpi("ROOM", name) == 0)
		return SoundDriver::ROOM;
	else if (strcmpi("BATHROOM", name) == 0)
		return SoundDriver::BATHROOM;
	else if (strcmpi("LIVINGROOM", name) == 0)
		return SoundDriver::LIVINGROOM;
	else if (strcmpi("STONEROOM", name) == 0)
		return SoundDriver::STONEROOM;
	else if (strcmpi("AUDITORIUM", name) == 0)
		return SoundDriver::AUDITORIUM;
	else if (strcmpi("CONCERTHALL", name) == 0)
		return SoundDriver::CONCERTHALL;
	else if (strcmpi("CAVE", name) == 0)
		return SoundDriver::CAVE;
	else if (strcmpi("ARENA", name) == 0)
		return SoundDriver::ARENA;
	else if (strcmpi("HANGAR", name) == 0)
		return SoundDriver::HANGAR;
	else if (strcmpi("CARPETEDHALLWAY", name) == 0)
		return SoundDriver::CARPETEDHALLWAY;
	else if (strcmpi("HALLWAY", name) == 0)
		return SoundDriver::HALLWAY;
	else if (strcmpi("STONECORRIDOR", name) == 0)
		return SoundDriver::STONECORRIDOR;
	else if (strcmpi("ALLEY", name) == 0)
		return SoundDriver::ALLEY;
	else if (strcmpi("FOREST", name) == 0)
		return SoundDriver::FOREST;
	else if (strcmpi("CITY", name) == 0)
		return SoundDriver::CITY;
	else if (strcmpi("MOUNTAINS", name) == 0)
		return SoundDriver::MOUNTAINS;
	else if (strcmpi("QUARRY", name) == 0)
		return SoundDriver::QUARRY;
	else if (strcmpi("PLAIN", name) == 0)
		return SoundDriver::PLAIN;
	else if (strcmpi("PARKINGLOT", name) == 0)
		return SoundDriver::PARKINGLOT;
	else if (strcmpi("SEWERPIPE", name) == 0)
		return SoundDriver::SEWERPIPE;
	else if (strcmpi("UNDERWATER", name) == 0)
		return SoundDriver::UNDERWATER;
	else
		return SoundDriver::NONE;
}

/***************************************************************************/
// Subclass these to use your own subclassed bank
/***************************************************************************/
SoundBank *SoundDriver::CreateBank()
{
	memSetUseBit(MEM_AUDIO_DATA);
	SoundBank* bank = new SoundBank(*this);
	memClearUseBit(MEM_AUDIO_DATA);
	return bank;
}

/***************************************************************************/
// Garbage collect banks that are marked for deletion
/***************************************************************************/
void SoundDriver::GarbageCollectBanks()
{
	// Only destroy banks in reverse order of the stack
	int topBank = bankStack.Count() - 1;
	while (topBank >= 0)
	{
		// Is this bank marked deleted?
		SoundBank *bank = bankStack.Get(topBank);
		if (!bank->MarkedDeleted())
			return;

		// Whoa, we found one to delete
		DestroyBank(bank);
		bankStack.Delete(topBank);

		// Next bank
		--topBank;
	}
}

/***************************************************************************/
// Destroy a bank (guaranteed to be the top of the stack!!)
/***************************************************************************/
void SoundDriver::DestroyBank(
SoundBank *bank)
{
	delete bank;
}

/***************************************************************************/
// Find a sound by name in all the banks
/***************************************************************************/
SoundDef *SoundDriver::FindSoundByName(
const char *name,
SoundBank **bank)
{
	#ifdef DISABLE_SOUND
	return NULL;
	#endif

	if (name != NULL && name[0] != 0)
	{
		for (int i = 0; i < bankStack.Count(); i++)
		{
			SoundBank* b = bankStack.Get(i);
			SoundDef*  s = b->GetSoundByName(name);
			if (s != NULL)
			{
				if (bank != NULL)
					*bank = b;
				return s;
			}
		}
	}

	if (bank != NULL)
		*bank = NULL;

	return NULL;
}

/***************************************************************************/
/***************************************************************************/
void SoundDriver::Add3DStream(
SoundStream3D *stream)
{ 
	if (stream) 
		soundStream3D.AddTail(stream); 
}

/***************************************************************************/
/***************************************************************************/
void SoundDriver::Remove3DStream(
SoundStream3D *stream)
{ 
	if (stream) 
		soundStream3D.Unlink(stream); 
}

/***************************************************************************/
/***************************************************************************/
bool SoundDriver::Allocate3DStream(
SoundStream3D::Priority forPriority,
float volume)
{
	// Are we full
	ASSERT(current3DStreams <= Max3DStreams());

	// Free a stream
	if (current3DStreams >= Max3DStreams())
	{
		// Scan for one to kill (start with oldest)
		SoundStream3D *killMe = NULL;
		SoundStream3D *stream3D = soundStream3D.Head();
		while (stream3D)
		{
			// Does this channel really have a stream?
			if (!stream3D->IsLost())
			{
				// Always snarf a lower priority stream
				if (stream3D->GetPriority() < forPriority)
				{
					killMe = stream3D;
					break;
				}
				// Equal priorities require volume check
				else if (stream3D->GetPriority() == forPriority)
				{
					float streamVolume = stream3D->ComputeVolume();
					if (volume > streamVolume)
					{
						// is this better than the kill stream?
						if (!killMe || killMe->ComputeVolume() > streamVolume)
							killMe = stream3D;
					}
				}
			}

			// Check the next stream
			stream3D = stream3D->next;
		}

		// Did we find one?
		if (killMe)
			killMe->Release();
	}

	// Did we get one open
	if (current3DStreams >= Max3DStreams())
		return false;

	// They get one!
	++current3DStreams;
	return true;
}

/***************************************************************************/
/***************************************************************************/
bool SoundDriver::Free3DStream(
SoundStream3D *stream)
{
	ASSERT(stream && !stream->IsLost());
	if (!stream || stream->IsLost())
		return false;

	--current3DStreams;
	return true;
}
