/***************************************************************************/
// A sound listener
/***************************************************************************/
#include "Audio/AudioPCH.h"

// Weighting factor for the velocity filter
#define V_WEIGHT 0.99f

// How far can the camera jump in a single frame without causing a discontinuity?
#define CAMERA_DISCONTINUITY_D				30.0f
#define CAMERA_DISCONTINUITY_D_SQUARED		(CAMERA_DISCONTINUITY_D * CAMERA_DISCONTINUITY_D)

/***************************************************************************/
/***************************************************************************/
SoundListener::SoundListener(
const Cartesian4x4 &camera) :
worldToListener(camera)
{
	lastPos.Clear();
	lastTime = 0.0f;
	velocity.Clear();
	speed = 0.0f;
}

/***************************************************************************/
// Update
/***************************************************************************/
void SoundListener::Update()
{
	// Yerk the pos out of the camera matrix
	Vector3 temp;
	temp = worldToListener.GetTranslation();
	Vector3 pos;
	pos = temp.InvBasisTransform(worldToListener);
	pos.Negate();

	// Get the current time
	float time = g_timer.GetEndSec();

	// What is ds?
	Vector3 ds;
	ds = pos - lastPos;

	// Have we experienced a jump?
	if (ds.LengthSquared() > CAMERA_DISCONTINUITY_D_SQUARED)
	{
		velocity.Clear();
		speed = 0.0f;
	}
	else
	{
		// Must have a dt != 0.0f to update
		float dt = time - lastTime;
		if (!Math::Zero(dt))
		{
			// Compute instantaneous velocity
			Vector3 vNow;
			vNow = ds / dt;

			// Filter it into the current velocity
			velocity = (V_WEIGHT * vNow) + ((1.0f - V_WEIGHT) * velocity);

			// Figure out the speed
			speed = velocity.Length();
		}
	}

	// Save these for the next call
	lastPos = pos;
	lastTime = time;
}

