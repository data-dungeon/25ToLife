/***************************************************************************/
// SoundGroup2D
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// Only the sound system can create us
/***************************************************************************/
SoundGroup2D::SoundGroup2D(
const char *groupName,
SoundDriver &soundDriver,
int howManyChannels,
Reclaim &reclaimMethod) :
SoundGroup(groupName, soundDriver, howManyChannels, reclaimMethod),
channelPool(howManyChannels * 2, 8)
{
}

/***************************************************************************/
// Make sure our channels are all gone
/***************************************************************************/
SoundGroup2D::~SoundGroup2D()
{
	KillChannels();
}

/***************************************************************************/
// The basic 2D implementation
/***************************************************************************/
SoundGroup::TYPE SoundGroup2D::Type() const
{
	return GROUP2D;
}

/***************************************************************************/
// Allocate a new sound
/***************************************************************************/
SoundChannel *SoundGroup2D::NewChannel(
SoundEmitter &emitter,
SoundDef &def)
{
	// Allocate the buffer
	void *buffer = channelPool.NewPtr();
	if (!buffer)
		return NULL;

	// This can't fail now
	return new (buffer) SoundChannel2D(emitter, def);
}

/***************************************************************************/
// Free a sound
/***************************************************************************/
void SoundGroup2D::DeleteChannel(SoundChannel *channel)
{
	if (channel)
		channelPool.Delete((SoundChannel2D *)channel);
}

