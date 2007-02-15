/***************************************************************************/
// Simple utility class to compute the volume and pan of a sound based
// on a 3D listener and soundsource
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// Compute pan & volume for a sound given a listner matrix
// pan is computed and returned between [-1..1]
// volume is computed and return between [0..1] for a SetVolume command
// Returns true if the sound is audible 
/***************************************************************************/
bool PositionalSound::ComputePanVolume(
const Graphics4x4 &listener,
Vector3CRef soundPos,
float &pan,
float &volume,
float panFactor,
float distanceOfFullVolume,
float distanceOfNoVolume)
{
	ASSERT(distanceOfFullVolume <= distanceOfNoVolume);

	// Put us into listener space
	Vector3 emitter = soundPos * listener;

	// How far away is the sound (holy crap, square root!!!!)
	float distance = emitter.Length();

	// Out of range?
	bool audible = (distance <= distanceOfNoVolume);
	if (audible)
	{
		// Compute the volume & pan
		volume = ComputeVolume(distance, distanceOfFullVolume, distanceOfNoVolume);
		pan = ComputePan(emitter, panFactor);
	}
	else
	{
		volume = 0.0f;
		pan = 0.0f;
	}

	// sound is audible?
	return audible;
}

/***************************************************************************/
// Compute the pan and volume for a directed sound source.  A directed
// sound source has a position, unit direction, and a dot product cone.
// The dot product code gives the cosine of the cone half-angle.
/***************************************************************************/
bool PositionalSound::ComputePanVolume(
const Graphics4x4 &listener,
Vector3CRef soundPos,
Vector3CRef soundDir,
float dotProductCone,
float &pan,
float &volume,
float panFactor,
float distanceOfFullVolume,
float distanceOfNoVolume)
{
	ASSERT(distanceOfFullVolume <= distanceOfNoVolume);

	// Convert into camera space
	Vector3 emitter = soundPos * listener;

	// Normalize the direction to the emitter
	float distance = emitter.Length();

	// outside of audible?
	bool audible = (distance <= distanceOfNoVolume);

	// What is the listener dir (listener is at origin)
	Vector3 listenerDir = emitter / distance;

	// Get the emission direction in listener space
	Vector3 emitterDir = soundDir.BasisTransform(listener);

	// Are we in the cone?
	float dot = Vector3::Dot(emitterDir, listenerDir);
	if (dot < dotProductCone)
		audible = false;

	// No edge attenuation (if you are in the cone, you get full volume)
	// could do some though (secondary cone maybe)

	// Compute the volume & pan
	if (audible)
	{
		volume = ComputeVolume(distance, distanceOfFullVolume, distanceOfNoVolume);
		pan = ComputePan(emitter, panFactor);
	}
	else
	{
		volume = 0.0f;
		pan = 0.0f;
	}

	// sound is audible?
	return audible;
}

/***************************************************************************/
// Compute the volume given distance from listenter
// Assumes sound is LESS THAN distanceOfNoVolume away
/***************************************************************************/
inline float PositionalSound::ComputeVolume(
float distance,
float distanceOfFullVolume,
float distanceOfNoVolume)
{
	ASSERT(distanceOfFullVolume <= distanceOfNoVolume);

	// Less than min distance
	float volume;
	if (distance <= distanceOfFullVolume)
		volume = 1.0f;
	else // scale it linearly
		volume = Math::Clamp(1.0f - (distance - distanceOfFullVolume) / (distanceOfNoVolume - distanceOfFullVolume));

	// sock it to 'em
	return volume;
}

/***************************************************************************/
// Compute the pan (given the emitter in listener space)
/***************************************************************************/
inline float PositionalSound::ComputePan(
Vector3CRef emitter,
float panFactor)
{
	// What is the pan
	// ANOTHER SQUARE ROOT w/a DIVIDE!!!

	float panDot;
	float const headHalfSize = 0.15f;
	if (Math::Abs(emitter.X()) < headHalfSize && Math::Abs(emitter.Z()) < headHalfSize) //Check for divide by zero
		panDot = 0.0f;
	else
		panDot= emitter.x() /  emitter.LengthXZ();

	return Math::Clamp(panDot * panFactor, -1.0f, 1.0f);
}

