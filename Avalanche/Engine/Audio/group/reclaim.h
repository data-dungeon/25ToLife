/***************************************************************************/
// A reclaim class scans a list of sounds and returns the best sound to
// kill.
/***************************************************************************/
#ifndef RECLAIM_H
#define RECLAIM_H

#include "container/dllist.h"

class SoundChannel;

class Reclaim
{
public:
	Reclaim() {}

	// The known reclaim methods
	enum METHOD
	{
		NONE,			// Don't reclaim voices
		OLDEST,		// Reclaim oldest voice
		ANY,			// Reclaim any voice
		VOLUME,		// Reclaim quietest
		DISTANCE,	// Reclaim farthest (3D only or ASSERT)

		METHODS
	};

	// Get a reclaim method object
	static Reclaim &Get(METHOD reclaimMethod);

 	// Return the best sound to kill from the list (but don't kill it)
	virtual SoundChannel *Best(SoundDriver &driver, SoundChannel *head, SoundChannel *forWho) = 0;

protected:
	// Is this a used channel to work on?
	SoundChannel *FirstActiveChannel(SoundChannel *head);
	SoundChannel *NextActiveChannel(SoundChannel *channel);
	int ActiveChannels(SoundChannel *head);

	// Perform a safe upcast
	SoundChannel3D *Upcast3D(SoundChannel *channel);

private:
	static Reclaim *reclaim[METHODS];
};

// Don't reclaim
class ReclaimNone : public Reclaim
{
public:
	SoundChannel *Best(SoundDriver &driver, SoundChannel *head, SoundChannel *forWho);
};

// Reclaim the oldest
class ReclaimOldest : public Reclaim
{
public:
	SoundChannel *Best(SoundDriver &driver, SoundChannel *head, SoundChannel *forWho);
};

// Reclaim any (pointless)
class ReclaimAny : public Reclaim
{
public:
	SoundChannel *Best(SoundDriver &driver, SoundChannel *head, SoundChannel *forWho);
};

// Reclaim the quietest
class ReclaimVolume : public Reclaim
{
public:
	SoundChannel *Best(SoundDriver &driver, SoundChannel *head, SoundChannel *forWho);

private:
	// Is a better than b?
	bool Better(SoundChannel *a, SoundChannel *b);
};

// Reclaim the farthest (3D only)
class ReclaimDistance : public Reclaim
{
public:
	SoundChannel *Best(SoundDriver &driver, SoundChannel *head, SoundChannel *forWho);

private:
	// Is a better than b?
	bool Better(SoundChannel *a, float aDist, SoundChannel *b, float bDist);
};

#endif
