/***************************************************************************/
// A sound listener
/***************************************************************************/
#ifndef LISTENER_H
#define LISTENER_H

#include "platform/BaseType.h"
#include "Math/Matrix.h"

// Forwards
class SoundDriver;

class SoundListener
{
public:
	// Update given the camera and current time
	virtual void Update();

	// Get the transformation matrix
	const Cartesian4x4 &WorldToListener()
		{ return worldToListener; }

	// What is our position in the world?
	inline Vector3 &Position()
		{ return lastPos; }

	// What is our velocity
	inline Vector3 &Velocity()
		{ return velocity; }

	// What is our speed?
	inline float Speed()
		{ return speed; }

	// Put this world point into listener space
	inline Vector3 TransformPosition(Vector3CRef posInWorld)
		{ return (posInWorld * worldToListener); }

	// Give a distance squared from the listener
	inline float DistanceSquared(Vector3CRef p)
		{ return ((p - lastPos).LengthSquared()); }

protected:
	const Cartesian4x4			&worldToListener;
	Vector3							lastPos;
	float								lastTime;
	Vector3							velocity;
	float								speed;

	// Only the sounddriver can operate on us
	SoundListener(const Cartesian4x4 &camera);
	virtual ~SoundListener() {}

	friend class SoundDriver;
};

#endif
