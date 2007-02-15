/***************************************************************************/
// 2D implemenation for a sound group
/***************************************************************************/
#ifndef SGROUP2D_H
#define SGROUP2D_H

#include "Audio/group/sgroup.h"

class SoundGroup2D : public SoundGroup
{
public:
	TYPE Type() const;

	// Allocate a new sound
	SoundChannel *NewChannel(SoundEmitter &emitter, SoundDef &def);

	// Free a sound
	void DeleteChannel(SoundChannel *channel);

	// Size down if possible
	void Shrink()
		{ channelPool.Shrink(); }

private:
	// Only the sound system can create us
	SoundGroup2D(const char *groupName,
						SoundDriver &soundDriver,
				  		int howManyChannels,
						Reclaim &reclaimMethod);

	// The storage mechanism for groups
	Pool<SoundChannel2D>	channelPool;

public:
	// this has to be public for list management
	~SoundGroup2D();

	// The sound system creates us
	friend class Audio;
};

#endif
