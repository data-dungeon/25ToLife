/***************************************************************************/
// 2D SoundEmitter
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// set up
/***************************************************************************/
SoundEmitter2D::SoundEmitter2D(
const char *name,
SoundDriver &soundDriver,
SoundGroup &soundGroup) :
SoundEmitter(name, soundDriver, soundGroup)
{
}

/***************************************************************************/
/***************************************************************************/
SoundEmitter2D::~SoundEmitter2D()
{
}

/***************************************************************************/
// Set the pan of a sound [-1..1]
/***************************************************************************/
bool SoundEmitter2D::SetPan(
SoundHandle handle,
float pan)
{
	// Resolve the channel
	SoundChannel2D *channel = ResolveHandle2D(handle);
	if (!channel)
		return false;

	channel->SetPan(pan);
	return true;
}

/***************************************************************************/
// Set the wet/dry mix
/***************************************************************************/
bool SoundEmitter2D::SetWetDry(
SoundHandle handle, 
float wet, 
float dry)
{
	// Resolve the channel
	SoundChannel2D *channel = ResolveHandle2D(handle);
	if (!channel)
		return false;

	channel->SetWetDry(wet, dry);
	return true;
}

/***************************************************************************/
// A utility to resolve a handle to a channel
/***************************************************************************/
SoundChannel2D *SoundEmitter2D::ResolveHandle2D(
SoundHandle handle)
{
	// get the channel
	SoundChannel *channel = ResolveHandle(handle);
	if (!channel)
		return NULL;

	// Do the upcast
	return (SoundChannel2D *)channel;
}

