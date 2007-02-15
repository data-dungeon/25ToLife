/***************************************************************************/
// 3D Sound Emitter
/***************************************************************************/
#ifndef SEMIT3D_H
#define SEMIT3D_H

#include "Math/Vector.h"
#include "Audio/Emitter/semitter.h"
#include "Audio/group/schan3d.h"
#include "Audio/Emitter/e3dparam.h"

class Sound;
class SoundListener;

// 3D Voice group
class SoundEmitter3D : public SoundEmitter
{
public:
	// Return our type
	TYPE Type()
		{ return EMITTER3D; }

	// Setup all the basic 3D parameters
	void Setup(SoundEmitter3DParams &emitterParams);
	SoundEmitter3DParams &GetParams();

	// This sets a transform you must manually update
	void SetTransform(const DirCos3x3 &dirCos, Vector3CRef pos);

	// This reads the transform from another location BE CAREFUL, this
	// will read from these ASYNCHORNOUSLY from your sound calls.
	void WatchTransform(const DirCos3x3 *dirCos, const Vector3 *pos);

	// Clear the current transform (put everything back into world coords)
	void ClearTransform();

	// Watch a velocity for this reference frame
	// Will read from ptr ASYNCHORNOUSLY from your sound calls.
	// Set to NULL to clear
	void WatchEmitterVelocity(const Vector3 *emitterVelocity);

	// Set the position of this sound
	bool SetPosition(SoundHandle handle, Vector3CRef pos);

	// Watch this position vector
	// Will read from ptr ASYNCHORNOUSLY from your sound calls.
	bool WatchPosition(SoundHandle handle, const Vector3 *pos);

	// Set the velocity of this sound
	bool SetVelocity(SoundHandle handle, Vector3CRef velocity);

	// Watch the velocity of this sound
	// Will read from ptr ASYNCHORNOUSLY from your sound calls.
	bool WatchVelocity(SoundHandle handle, const Vector3 *velocity);

	// Detach velocity and position
	bool Detach(SoundHandle handle);

	// Set a verify cookie
	bool SetCookie(SoundHandle handle, void *cookie);

	// Set the ambience [0..1], returns false if sound has expired
	bool SetAmbience(SoundHandle handle, float ambience);

	// Set the presence [0..1], returns false if sound has expired
	bool SetPresence(SoundHandle handle, float presence);

	// Override the audible distance
	bool SetAudibleDistance(SoundHandle handle, float audibleDistance);

	// Run every frame or so
	virtual void Daemon();

	// Mark the emitter for death
	// (will allow all non-looped sounds to finish)
	bool MarkForDeath();

protected:
	// set up
	SoundEmitter3D(const char *name,
						SoundDriver &soundDriver,
						SoundGroup &soundGroup);

	// Our 3D sound parameters
	SoundEmitter3DParams 	params;

	// Our tranform
	enum TRANSFORMTYPE
	{
		TRANSFORM_NONE = 0,
		TRANSFORM_PTR,
		TRANSFORM_LOCAL
	};
	struct Transform
	{
		TRANSFORMTYPE		type;
		DirCos3x3			dirCos;
		const DirCos3x3	*ptrDirCos;
		Vector3				pos;
		const Vector3		*ptrPos;
	};
	Transform					transform;

	// Convert a model space coordinate into world coordinates
	inline Vector3				PosToWorld(Vector3CRef modelPos);
	inline Vector3				VelToWorld(Vector3CRef modelVel);

	// What is our emitter velocity?
	const Vector3				*velocity;

	// Resolve and upcast
	SoundChannel3D *ResolveHandle3D(SoundHandle handle);

	friend class Audio;
	friend class SoundChannel3D;

public:
	// Has to be public for list management
	virtual ~SoundEmitter3D();
};

/***************************************************************************/
// Convert a model space coordinate into world coordinates
/***************************************************************************/
inline Vector3	SoundEmitter3D::PosToWorld(
Vector3CRef modelPos)
{
	if (transform.type == TRANSFORM_NONE)
		return modelPos;
	else if (transform.type == TRANSFORM_PTR)
		return modelPos * *transform.ptrDirCos + *transform.ptrPos;
	else if (transform.type == TRANSFORM_LOCAL)
		return modelPos * transform.dirCos + transform.pos;

	// Unsupported transform type
	ASSERT(false);
	return modelPos;
}

/***************************************************************************/
/***************************************************************************/
inline Vector3	SoundEmitter3D::VelToWorld(
Vector3CRef modelVel)
{
	Vector3 v;

	if (transform.type == TRANSFORM_NONE)
		v = modelVel;
	else if (transform.type == TRANSFORM_PTR)
		v = modelVel / *transform.ptrDirCos;
	else if (transform.type == TRANSFORM_LOCAL)
		v = modelVel / transform.dirCos;

	// Add in the emitter velocity
	if (velocity)
		v += *velocity;

	return v;
}

#endif
