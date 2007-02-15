/***************************************************************************/
// SoundGroup
//
// The sound group class contains a table of channels and extra information
// for the hardware voices.  Emitters allocate channels from these groups.
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// Only the sound system can create us
/***************************************************************************/
SoundGroup::SoundGroup(
const char *groupName,
SoundDriver &soundDriver,
int howManyChannels,
Reclaim &reclaimMethod) :
driver(soundDriver),
reclaim(reclaimMethod)
{
	ASSERT(groupName);
	ASSERT(howManyChannels > 0);

	strncpy(name, groupName, sizeof(name) - 1);
	name[sizeof(name) - 1 ] = '\0';
	maxChannels = howManyChannels;
	maxUsedVoices = 0;
	reclaims = 0;
	destroyWhenEmpty = false;
}

/***************************************************************************/
// this has to be public for list management
/***************************************************************************/
SoundGroup::~SoundGroup()
{
	// Give them some feedback
	TRACE("Sound Group %s used a maximum of %d voices of %d. %d reclaims attempted.\n",
				name, maxUsedVoices, maxChannels, reclaims);

	// Child has to clean up the list
	ASSERT(channelList.Head() == NULL);
}

/***************************************************************************/
/***************************************************************************/
const char *SoundGroup::Name() const
{
	return name;
}

/***************************************************************************/
// Setup our reclaim method
/***************************************************************************/
bool SoundGroup::SetReclaim(
Reclaim &reclaimMethod)
{
	reclaim = reclaimMethod;
	return true;
}

/***************************************************************************/
// How many channels are we actively using
/***************************************************************************/
int SoundGroup::UsedChannels()
{
	int usedChannels = 0;

	SoundChannel *channel = channelList.Head();
	while (channel)
	{
		// If they have a voice, it's a used channel
		if (channel->HasVoice())
			++usedChannels;

		// Next guy to process
		channel = channel->next;
	}

	return usedChannels;
}

/***************************************************************************/
// What is the maximum number of channels we will use?
/***************************************************************************/
int SoundGroup::MaxChannels() const
{
	return maxChannels;
}

/***************************************************************************/
// Adjust the number of channels (may cause voices to drop)
// Returns false if the size couldn't be fully adjusted (call MaxChannels
// to see what the current channels are)
/***************************************************************************/
bool SoundGroup::AdjustChannels(
int newChannels)
{
	// Set it
	maxChannels = newChannels;

	// Do we have to free some voices?
	if (newChannels < UsedChannels())
	{
		// We gotta shrink the number of channels
		while (UsedChannels() > maxChannels)
		{
			// If we can't reclaim, we are done
			if (!ReclaimVoice(NULL))
			{
				// This is our upper limit
				maxChannels = UsedChannels();
				return false;
			}
		}
	}

	// We are successful!
	return true;
}

/***************************************************************************/
// Allocate a channel (can return NULL)
// doesn't put it in the list, but holds a channel (either keep or delete it)
/***************************************************************************/
SoundChannel *SoundGroup::AllocateChannel(
SoundEmitter &emitter,
SoundDef &def)
{
	// Allocate the channel
	SoundChannel *channel = NewChannel(emitter, def);
	if (!channel)
		return NULL;

	// Allocate a handle
	SoundHandle handle = new SoundHandleData(channel);
	SoundHandleTracker::Add(handle, emitter.Name(), def.name);
	ASSERT(handle);
	if (!handle)
	{
		FreeChannel(channel);
		return NULL;
	}

	// Save off the handle
	channel->SetHandle(handle);

	// Put it in the list
	channelList.AddTail(channel);

	return channel;
}

/***************************************************************************/
// Aquire a hardware voice for the channel
/***************************************************************************/
bool SoundGroup::AquireVoice(
SoundChannel *channel)
{
	// Can't do anything if we lost the channel...
	if (!channel)
		return false;

	// Does this channel already have a voice?
	if (channel->HasVoice())
		return true;

	// What do we call this voice?
	const char *name = channel->SoundDefinition().name;

	// Get a pointer to the parent sound
	Sound *sound = SoundBank::GetEngineSound(&channel->SoundDefinition());
	if (!sound)
		return false;

	// Assume no hardware voice
	SoundInstance *voice = NULL;

	// Tell the channel we are about to look at it's info, so it should be updated
	if (!channel->UpdateForAquire())
		goto FailedAquire;

	// Get a voice (this may fail, which means we have to reclaim)
	voice = sound->NewInstance(name);

	// Do we have a free channel?
	if (!voice || UsedChannels() >= maxChannels)
	{
		// Reclaim a voice!
		++reclaims;
		if (!ReclaimVoice(channel))
			goto FailedAquire;
	}

	// Try again now that we freed a hardware voice
	if (!voice)
		voice = sound->NewInstance(name);
	if (!voice)
		goto FailedAquire;

	// We have a voice!
	channel->BindVoice(voice);

	return true;

FailedAquire:
	// We've failed
	if (voice)
		sound->FreeInstance(voice);

	// We should free this channel
	if (channel->GetAutoReclaim())
		FreeChannel(channel);

	return false;
}

/***************************************************************************/
// Free this channel
/***************************************************************************/
void SoundGroup::FreeChannel(
SoundChannel *who)
{
	// Okay to pass in NULL
	if (!who)
		return;

	// Make sure we clear the handle
	SoundHandle handle = who->GetHandle();
	if (handle)
		handle->LostChannel();

	// Take it out of the list
	channelList.Unlink(who);

	// Delete it
	DeleteChannel(who);
}

/***************************************************************************/
// Pause this group
/***************************************************************************/
void SoundGroup::Pause()
{
	SoundChannel *channel = channelList.Head();
	while (channel)
	{
		channel->Pause();
		channel = channel->next;
	}
}

/***************************************************************************/
// Unpause this group
/***************************************************************************/
void SoundGroup::Unpause()
{
	SoundChannel *channel = channelList.Head();
	while (channel)
	{
		channel->Unpause();
		channel = channel->next;
	}
}

/***************************************************************************/
// Call this every frame
/***************************************************************************/
void SoundGroup::Daemon()
{
	// What time is it?
	uint now = driver.Time();

	// Tracking
	int voices = 0;

	// What sounds have done playing?
	SoundChannel *channel = channelList.Head();
	while (channel)
	{
		// Next guy to process
		SoundChannel *next = channel->next;

		// Is this channel done playing?
		if (channel->NextUpdate() != 0 && now >= channel->NextUpdate())
			channel->Update(now);

		if (channel->HasVoice())
			++voices;

		if (channel->Dead())
			FreeChannel(channel);

		// advance the search
		channel = next;
	}

	// What is our max voices
	if (voices > maxUsedVoices)
		maxUsedVoices = voices;

	// Restart any auto restarts
	channel = channelList.Head();
	while (channel)
	{
		// Is this channel auto restart with no voice?
		if (channel->GetAutoRestart() && !channel->Paused() && !channel->HasVoice() &&
				now >= channel->GetNextAutoRestart())
		{
			if (AquireVoice(channel))
				channel->Play(now);
			else
				channel->UpdateNextAutoRestart(now);
#if 0//defined(_DEBUG) && defined(FOOD)
			TRACE("Attempting to restart %s (%x) %s at %d!!\n", 
						channel->SoundName(), channel, channel->HasVoice() ? "WORKED" : "FAILED", now);
#endif
		}

		// advance the search
		channel = channel->next;
	}
}

/***************************************************************************/
// Enumerate the channels for an emitter
/***************************************************************************/
SoundChannel *SoundGroup::FindFirst(
SoundEmitter *emitter)
{
	return FindNext(emitter, NULL);
}

/***************************************************************************/
/***************************************************************************/
SoundChannel *SoundGroup::FindNext(
SoundEmitter *emitter,
SoundChannel *current)
{
	// Setup the start
	if (current == NULL)
		current = channelList.Head();
	else
		current = current->next;

	// Loop until we find a valid
	while (current)
	{
		// Does the emitter match us?
		if (!emitter || emitter == &current->Emitter())
			break;

		current = current->next;
	}

	return current;
}

/***************************************************************************/
// Reclaim a voice
/***************************************************************************/
bool SoundGroup::ReclaimVoice(
SoundChannel *forWho)
{
	// forWho should not have a voice
	ASSERT(!forWho->HasVoice());

	// Can't reclaim if there are no voices...this should be an assert
	if (!channelList.Head())
		return false;

	// Find the best channel to kill
	SoundChannel *killMe = reclaim.Best(driver, channelList.Head(), forWho);
	ASSERT(killMe == NULL || killMe->HasVoice());

	// Dump out stuff about stuff
//	DebugReclaim(killMe);

	// Uh oh, we can't free a channel
	if (!killMe)
		return false;

	// Reclaim this channel
	if (killMe->GetAutoReclaim())
		FreeChannel(killMe);
	else // Can't auto-reclaim this voice, steal it's hardware channel instead
		killMe->StealVoice(driver.Time());

	// We are allowed to get another hardware voice
	return true;
}

/***************************************************************************/
// Dump out stuff about stuff
/***************************************************************************/
void SoundGroup::DebugReclaim(
SoundChannel *whoIsGoingToDie)
{
#ifdef _DEBUG
	TRACE("Sound Group %s with %d voices needs to reclaim a voice:\n", name, maxChannels);
	SoundChannel *channel = channelList.Head();
	int i = 0;
	while (channel)
	{
		if (channel == whoIsGoingToDie)
			TRACE("**** DIE ");

		TRACE("Channel %d \"%s\"", i, channel->SoundName());
		if (!channel->HasVoice())
			TRACE(" No Voice");

		if (channel->Looped())
			TRACE(" Looped" );

		TRACE(" Volume %1.3f", channel->GetVolume());
		TRACE(" TOC %d", channel->GetTimeOfCompletion());
		TRACE("\n");

		// next!
		channel = channel->next;
		++i;
	}

	if (!whoIsGoingToDie)
		TRACE("**************** No voice was selected to die!!!!!!!!!!!\n");
#endif
}

/***************************************************************************/
// Kill all the channels
/***************************************************************************/
void SoundGroup::KillChannels()
{
	// We better delete everything
	SoundChannel *killMe = channelList.Head();
	while (killMe)
	{
		// Look out this is a virtual function, must be called in child destructor
		FreeChannel(killMe);
		killMe = channelList.Head();
	}
}

/***************************************************************************/
// Should this group be destroyed when empty?
/***************************************************************************/
void SoundGroup::SetDestroyWhenEmpty(
bool destroy)
{
	destroyWhenEmpty = destroy;
}

/***************************************************************************/
// Should this group be destroyed when empty?
/***************************************************************************/
bool SoundGroup::DestroyWhenEmpty()
{
	return destroyWhenEmpty;
}
