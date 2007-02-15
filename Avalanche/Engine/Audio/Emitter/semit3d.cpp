/***************************************************************************/
// 3D SoundEmitter
/***************************************************************************/
#include "Audio/AudioPCH.h"


/***************************************************************************/
// set up
/***************************************************************************/
SoundEmitter3D::SoundEmitter3D(
const char *name,
SoundDriver &soundDriver,
SoundGroup &soundGroup) :
SoundEmitter(name, soundDriver, soundGroup)
{
	// No transform
	ClearTransform();

	// No emitter velocity
	WatchEmitterVelocity(NULL);
}

/***************************************************************************/
/***************************************************************************/
SoundEmitter3D::~SoundEmitter3D()
{
}

/***************************************************************************/
// Setup all the basic 3D parameters
/***************************************************************************/
void SoundEmitter3D::Setup(
SoundEmitter3DParams &emitterParams)
{
	params = emitterParams;
}

/***************************************************************************/
/***************************************************************************/
SoundEmitter3DParams &SoundEmitter3D::GetParams()
{
	return params;
}

/***************************************************************************/
// This sets a transform you must manually update
/***************************************************************************/
void SoundEmitter3D::SetTransform(
const DirCos3x3 &dirCos,
Vector3CRef pos)
{
	transform.type = TRANSFORM_LOCAL;
	transform.dirCos = dirCos;
	transform.pos = pos;
}

/***************************************************************************/
// This reads the transform from another location BE CAREFUL, this
// will read from these ASYNCHORNOUSLY from your sound calls.
/***************************************************************************/
void SoundEmitter3D::WatchTransform(
const DirCos3x3 *dirCos,
const Vector3 *pos)
{
	ASSERT(pos != NULL && dirCos != NULL);

	transform.type = TRANSFORM_PTR;
	transform.ptrDirCos = dirCos;
	transform.ptrPos = pos;
}

/***************************************************************************/
// Clear the current transform (put everything back into world coords)
/***************************************************************************/
void SoundEmitter3D::ClearTransform()
{
	transform.type = TRANSFORM_NONE;
}

/***************************************************************************/
// Watch a velocity for this reference frame
// Will read from ptr ASYNCHORNOUSLY from your sound calls.
// Set to NULL to clear
/***************************************************************************/
void SoundEmitter3D::WatchEmitterVelocity(
const Vector3 *emitterVelocity)
{
	velocity = emitterVelocity;
}

/***************************************************************************/
// Set the position of this sound
/***************************************************************************/
bool SoundEmitter3D::SetPosition(
SoundHandle handle,
Vector3CRef pos)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->SetPosition(pos);
	return true;
}

/***************************************************************************/
// Watch this position vector
// Will read from ptr ASYNCHORNOUSLY from your sound calls.
/***************************************************************************/
bool SoundEmitter3D::WatchPosition(
SoundHandle handle,
const Vector3 *pos)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->WatchPosition(pos);
	return true;
}

/***************************************************************************/
// Set the velocity of this sound
/***************************************************************************/
bool SoundEmitter3D::SetVelocity(
SoundHandle handle,
Vector3CRef velocity)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->SetVelocity(velocity);
	return true;
}

/***************************************************************************/
// Watch the velocity of this sound
// Will read from ptr ASYNCHORNOUSLY from your sound calls.
/***************************************************************************/
bool SoundEmitter3D::WatchVelocity(
SoundHandle handle,
const Vector3 *velocity)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->WatchVelocity(velocity);
	return true;
}

/***************************************************************************/
// Watch the velocity of this sound
// Will read from ptr ASYNCHORNOUSLY from your sound calls.
/***************************************************************************/
bool SoundEmitter3D::Detach(
SoundHandle handle)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->Detach();
	return true;
}

/***************************************************************************/
// Set a verify cookie
/***************************************************************************/
bool SoundEmitter3D::SetCookie(
SoundHandle handle,
void *cookie)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->SetCookie(cookie);
	return true;
}

/***************************************************************************/
// Set the ambience [0..1], returns false if sound has expired
/***************************************************************************/
bool SoundEmitter3D::SetAmbience(
SoundHandle handle,
float ambience)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->SetAmbience(ambience);
	return true;
}

/***************************************************************************/
// Set the presence [0..1], returns false if sound has expired
/***************************************************************************/
bool SoundEmitter3D::SetPresence(
SoundHandle handle,
float presence)
{
	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->SetPresence(presence);
	return true;
}

/***************************************************************************/
// Override the audible distance
/***************************************************************************/
bool SoundEmitter3D::SetAudibleDistance(
SoundHandle handle,
float audibleDistance)
{
  	// Resolve the channel
	SoundChannel3D *channel = ResolveHandle3D(handle);
	if (!channel)
		return false;

	channel->SetAudibleDistance(audibleDistance);
	return true;
}

/***************************************************************************/
// Run every frame or so
/***************************************************************************/
void SoundEmitter3D::Daemon()
{
	// Call up the chain
	SoundEmitter::Daemon();
}

/***************************************************************************/
// Mark the emitter for death
// (will allow all non-looped sounds to finish)
/***************************************************************************/
bool SoundEmitter3D::MarkForDeath()
{
	// Walk our sounds and detach them
	SoundChannel *channel = group.FindFirst(this);
	while (channel)
	{
		// Detach it!
		ASSERT(channel->Type() == SoundChannel::CHANNEL3D);
		SoundChannel3D *channel3D = (SoundChannel3D *)channel;
		channel3D->Detach();

		// next!
		channel = group.FindNext(this, channel);
	}

	// Now detach our information
	velocity = NULL;
	if (transform.type == TRANSFORM_PTR)
	{
		transform.type = TRANSFORM_LOCAL;
		transform.dirCos = *transform.ptrDirCos;
		transform.pos = *transform.ptrPos;
	}

	return SoundEmitter::MarkForDeath();
}

/***************************************************************************/
// A utility to resolve a handle to a channel
/***************************************************************************/
SoundChannel3D *SoundEmitter3D::ResolveHandle3D(
SoundHandle handle)
{
	// get the channel
	SoundChannel *channel = ResolveHandle(handle);
	if (!channel)
		return NULL;

	// Valdiate the upcast
	ASSERT(channel->Type() == SoundChannel::CHANNEL3D);
	if (channel->Type() != SoundChannel::CHANNEL3D)
		return NULL;

	// Do the upcast
	return (SoundChannel3D *)channel;
}

