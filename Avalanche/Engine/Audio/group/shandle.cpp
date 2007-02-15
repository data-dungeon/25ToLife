/***************************************************************************/
// A sound handle is returned from a play, if you request it.
//
// Sound handles MUST BE FREE'd or else.
/***************************************************************************/
#include "Audio/AudioPCH.h"

// The static pool
Pool<SoundHandleData> *SoundHandleData::pool = NULL;

#ifdef TRACKSOUNDHANDLES
// The list for the handle tracker
DoublyLinkedList<SoundHandleTracker::HandleTrack>	SoundHandleTracker::handleList;
FixedPool<SoundHandleTracker::HandleTrack, 1024>	SoundHandleTracker::handlePool("DanglingSoundHandles", false);
uint																SoundHandleTracker::uid = 0;

#endif

/***************************************************************************/
// Some handy functions that need to be called to get this thing going
/***************************************************************************/
void SoundHandleData::Initialize()
{
	// Create the pool
	pool = new Pool<SoundHandleData>(256, 8);
	ASSERT(pool);
}

/***************************************************************************/
/***************************************************************************/
void SoundHandleData::Terminate()
{
	// Clean it up
	delete pool;
	pool = NULL;
}

/***************************************************************************/
/***************************************************************************/
void SoundHandleData::Reset()
{
	Terminate();
	Initialize();
}

/***************************************************************************/
// Must be created with purpose
/***************************************************************************/
SoundHandleData::SoundHandleData(
SoundChannel *theChannel) :
channel(theChannel)
{
}

/***************************************************************************/
/***************************************************************************/
SoundHandleData::~SoundHandleData()
{
	// Tell the channel we lost the handle
	if (channel)
		channel->SetHandle(NULL);
}

/***************************************************************************/
// Get the channel
/***************************************************************************/
SoundChannel *SoundHandleData::Channel()
{
	return channel;
}

/***************************************************************************/
// Clear the channel
/***************************************************************************/
void SoundHandleData::LostChannel()
{
	channel = NULL;
}

/***************************************************************************/
/***************************************************************************/
void *SoundHandleData::operator new(size_t size)
{
	// This would be bad, initialize/terminate is unbalanced
	ASSERT(pool);

	// Sanity check
	ASSERT(size == sizeof(SoundHandleData));

	// Chug one off the pool (should never fail)
	void *buffer = pool->NewPtr();
	ASSERT(buffer);

	// Hand it back to the system
	return buffer;
}

/***************************************************************************/
/***************************************************************************/
void SoundHandleData::operator delete(void *me)
{
	// This would be bad, initialize/terminate is unbalanced
	ASSERT(pool);

	// Do we have to check this?
	if (me)
	{
		// Alias it back into ourselves
		SoundHandleData *sh = (SoundHandleData *)me;

		// Are they deleing a looped sound's handle??
		if (sh->Channel() && sh->Channel()->Looped())
		{
			ASSERT(sh->Channel()->Dead());
		}

		// Tell the tracker
		SoundHandleTracker::Remove(sh);

		// Put it back in the pool
	 	pool->Delete(sh);
	}
}

#ifdef TRACKSOUNDHANDLES
/***************************************************************************/
/***************************************************************************/
void SoundHandleTracker::Add(
SoundHandle handle,
const char *emitterName,
const char *soundName)
{
	// Okay to call w/null
	if (!handle)
		return;

	// Make a new one (should use a pool)
	HandleTrack *track = handlePool.New();
	ASSERT(track);

	// Give it a uid
	handle->uid = ++uid;

	// Fill it out and add it to the list
	if (track)
	{
		track->handle = handle;
		track->uid = uid;
		strncpy(track->emitterName, emitterName, sizeof(track->emitterName) - 1);
		track->emitterName[sizeof(track->emitterName) - 1] = '\0';
		strncpy(track->soundName, soundName, sizeof(track->soundName) - 1);
		track->soundName[sizeof(track->soundName) - 1] = '\0';

		handleList.AddTail(track);
	}
}

/***************************************************************************/
/***************************************************************************/
void SoundHandleTracker::Remove(
SoundHandle handle)
{
	// Okay to call w/null
	if (!handle)
		return;

	// Scan the list for it (ugh)
	HandleTrack *track = handleList.Head();
	while (track)
	{
		if (track->handle == handle && track->handle->uid == track->uid)
		{
			handleList.Unlink(track);
			handlePool.Delete(track);
			return;
		}

		track = track->next;
	}

	// Can't find the handle in our tracking list
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
void SoundHandleTracker::Clear()
{
	HandleTrack *track = handleList.Head();
	while (track)
	{
		HandleTrack *next = track->next;

		handleList.Unlink(track);
		handlePool.Delete(track);

		track = next;
	}
}

/***************************************************************************/
/***************************************************************************/
void SoundHandleTracker::Report(
bool assertIfNotEmpty)
{
	// Anything to do?
	if (!handleList.Head())
		return;

	HandleTrack *track = handleList.Head();
	int danglingSoundHandles = 0;
	while (track)
	{
		if (!g_audio.GetEmitter(track->emitterName))
		{
			if (danglingSoundHandles == 0)
			{
				conPuts("****************************************************\n");
				conPuts("Dangling sound handles!\n");
				conPuts("****************************************************\n");
			}

			char temp[256];
			sprintf(temp, "Emitter: \'%s\" Sound: \"%s\" Value 0x%x uid: %d\n", track->emitterName, track->soundName, track->handle, track->uid);
			conPuts(temp);
			++danglingSoundHandles;
		}

		track = track->next;
	}

	if (danglingSoundHandles > 0)
	{
		char temp[256];
		sprintf(temp, "%d dangling sound handles\n", danglingSoundHandles);
		conPuts(temp);
	}

	// Handle list is not empty and it shold be
	ASSERT(danglingSoundHandles == 0 || !assertIfNotEmpty);
}

#endif
