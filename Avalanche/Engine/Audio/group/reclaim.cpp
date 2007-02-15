/***************************************************************************/
// A reclaim class scans a list of sounds and returns the best sound to
// kill.
/***************************************************************************/
#include "Audio/AudioPCH.h"

// Declare one of each of our objects
static ReclaimNone 		reclaimNone;
static ReclaimOldest 	reclaimOldest;
static ReclaimAny			reclaimAny;
static ReclaimVolume		reclaimVolume;
static ReclaimDistance  reclaimDistance;

// Make the array of objects
Reclaim *Reclaim::reclaim[METHODS] =
{
	&reclaimNone,
	&reclaimOldest,
	&reclaimAny,
	&reclaimVolume,
	&reclaimDistance
};

/***************************************************************************/
// Get a reclaim method object
/***************************************************************************/
Reclaim &Reclaim::Get(
METHOD reclaimMethod)
{
	ASSERT(reclaimMethod < METHODS);
	return *reclaim[reclaimMethod];
}

/***************************************************************************/
/***************************************************************************/
SoundChannel *Reclaim::FirstActiveChannel(
SoundChannel *head)
{
	// Skip to the first channel that has a voice
	while (head && (!head->HasVoice() || head->Locked()))
		head = head->next;

	return head;
}

/***************************************************************************/
/***************************************************************************/
SoundChannel *Reclaim::NextActiveChannel(
SoundChannel *channel)
{
	if (!channel)
		return NULL;
	else
		return FirstActiveChannel(channel->next);
}

/***************************************************************************/
// Perform a safe upcast
/***************************************************************************/
SoundChannel3D *Reclaim::Upcast3D(
SoundChannel *channel)
{
	// Null is okay
	if (!channel)
		return NULL;

	// Make sure our upcast is legal
	ASSERT(channel->Type() == SoundChannel::CHANNEL3D);
	if (channel->Type() != SoundChannel::CHANNEL3D)
		return NULL;

	return (SoundChannel3D *)channel;
}

/***************************************************************************/
// How many channles have voices
/***************************************************************************/
int Reclaim::ActiveChannels(
SoundChannel *head)
{
	int legal = 0;

	SoundChannel *channel = FirstActiveChannel(head);
	while (channel)
	{
		++legal;
		channel = NextActiveChannel(channel);
	}

	return legal;
}

/***************************************************************************/
// Reclaim NONE
/***************************************************************************/
SoundChannel *ReclaimNone::Best(
SoundDriver &driver,
SoundChannel *head,
SoundChannel *forWho)
{
	// No reclaiming here
 	return NULL;
}

/***************************************************************************/
// Reclaim the oldest
/***************************************************************************/
SoundChannel *ReclaimOldest::Best(
SoundDriver &driver,
SoundChannel *head,
SoundChannel *forWho)
{
	// Pick the head as the oldest
	SoundChannel *oldest = FirstActiveChannel(head);

	// If we get to the forWho before Oldest, it is not older
	SoundChannel *channel = head;
	while (channel)
	{
		// found forWho first
		if (channel == forWho)
			return NULL;
		else if (channel == oldest)
			return oldest;

		// next
		channel = channel->next;
	}

	// Should never get here....
	ASSERT(false);
	return NULL;
}

/***************************************************************************/
// Reclaim any (pointless)
/***************************************************************************/
SoundChannel *ReclaimAny::Best(
SoundDriver &driver,
SoundChannel *head,
SoundChannel *forWho)
{
	// How many items are there?
	int active = ActiveChannels(head);

	// No active channels?
	if (!active)
		return NULL;

	// Pick a random unlocked channel
	int who = rand() % active;
	SoundChannel *channel = FirstActiveChannel(head);
	while (channel)
	{
		if (who-- == 0)
			return channel;

		channel = NextActiveChannel(channel);
	}

	// Should never get here
 	return NULL;
}

/***************************************************************************/
// Reclaim the quietest
/***************************************************************************/
SoundChannel *ReclaimVolume::Best(
SoundDriver &driver,
SoundChannel *head,
SoundChannel *forWho)
{
	// Assume it's the first
	SoundChannel *best = FirstActiveChannel(head);

	// No active channels
	if (!best)
		return NULL;

	// Scan the list for a better sound
	SoundChannel *channel = NextActiveChannel(best);
	while (channel)
	{
		// Which one is better?
		if (Better(channel, best))
			best = channel;

		// next
		channel = NextActiveChannel(channel);
	}

	// Is this one better?
	if (!forWho || Better(best, forWho))
		return best;
	else
		return NULL;
}

/***************************************************************************/
// Is a better than b?
/***************************************************************************/
bool ReclaimVolume::Better(
SoundChannel *a,
SoundChannel *b)
{
	// duh
	ASSERT(a);
	ASSERT(b);

	// What are the volumes?
	const float aVolume = a->GetVolume();
	const float bVolume = b->GetVolume();

	// Are the of equal volume? Pick the one that will end sooner
	if (Math::Equal(aVolume, bVolume))
		return (a->GetTimeOfCompletion() < b->GetTimeOfCompletion());
	else // If it's less then pick it
		return (aVolume < bVolume);
}

/***************************************************************************/
// Reclaim the farthest (3D only)
/***************************************************************************/
SoundChannel *ReclaimDistance::Best(
SoundDriver &driver,
SoundChannel *head,
SoundChannel *forWho)
{
	// Assume nothing
	SoundChannel3D *farthest = NULL;
	float maxD = 0.0f;

	// Scan the list for a farther sound
	SoundChannel3D *channel = Upcast3D(FirstActiveChannel(head));
	while (channel)
	{
		// How far away is this one?
		float d = driver.DistanceSquaredClosestListener(channel->Position());

		if (Better(channel, d, farthest, maxD))
		{
			farthest = channel;
			maxD = d;
		}

		// Next
		channel = Upcast3D(NextActiveChannel(channel));
	}

	// One last check
	if (!forWho)
		return farthest;

	// Is this better
	float d = driver.DistanceSquaredClosestListener(Upcast3D(forWho)->Position());
	if (Better(farthest, maxD, forWho, d))
 		return farthest;
	else
		return NULL;
}

/***************************************************************************/
// Is a better than b?
/***************************************************************************/
bool ReclaimDistance::Better(
SoundChannel *a,
float aDist,
SoundChannel *b,
float bDist)
{
	ASSERT(a);

	// Simple check
	if (!b)
		return true;

	// Farther?
	return (aDist > bDist);
}

