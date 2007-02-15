/***************************************************************************/
// SoundGroup3D
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// Only the sound system can create us
/***************************************************************************/
SoundGroup3D::SoundGroup3D(
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
SoundGroup3D::~SoundGroup3D()
{
	KillChannels();
}

/***************************************************************************/
// The basic 3D implementation
/***************************************************************************/
SoundGroup::TYPE SoundGroup3D::Type() const
{
	return GROUP3D;
}

/***************************************************************************/
// Allocate a new sound
/***************************************************************************/
SoundChannel *SoundGroup3D::NewChannel(
SoundEmitter &emitter,
SoundDef &def)
{
	// Allocate the buffer
	void *buffer = channelPool.NewPtr();
	if (!buffer)
		return NULL;

	// This can't fail now
	return new (buffer) SoundChannel3D(emitter, def);
}

/***************************************************************************/
// Free a sound
/***************************************************************************/
void SoundGroup3D::DeleteChannel(
SoundChannel *channel)
{
	if (channel)
		channelPool.Delete((SoundChannel3D *)channel);
}



