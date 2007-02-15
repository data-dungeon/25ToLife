/***************************************************************************/
// Simple utility class to compute the volume and pan of a sound based
// on a 3D listener and soundsource
/***************************************************************************/
#ifndef POSITIONAL_H
#define POSITIONAL_H

#include "Math/Matrix.h"

// What is the default pan ratio (0.0 - 1.0), determines how hard
// the sound will be panned
#define DEFAULT_PANFACTOR	0.9f

// What is the distance at which the sound will not be heard
#define DEFAULT_AUDIBLEDISTANCE			50.0f

class PositionalSound
{
public:
	// soundPos is given in listener space!!
	// Compute pan & volume for a sound given a listner matrix
	// pan is computed and returned between [-1..1]
	// volume is computed and return between [0..1] for a SetVolume command
	// Returns true if the sound is audible
	static bool ComputePanVolume(const Graphics4x4 &listener,
											Vector3CRef soundPos,
											float &pan,
											float &volume,
											float panFactor = DEFAULT_PANFACTOR,
											float distanceOfFullVolume = 0.0f,
											float distanceOfNoVolume = DEFAULT_AUDIBLEDISTANCE);

	// Compute the pan and volume for a directed sound source.  A directed
	// sound source has a position, unit direction, and a dot product cone.
	// The dot product code gives the cosine of the cone half-angle.
	// Returns true if the sound is audible
	static bool ComputePanVolume(const Graphics4x4 &listener,
											Vector3CRef soundPos,
											Vector3CRef soundDir,
											float dotProductCone,
											float &pan,
											float &volume,
											float panFactor = DEFAULT_PANFACTOR,
											float distanceOfFullVolume = 0.0f,
											float distanceOfNoVolume = DEFAULT_AUDIBLEDISTANCE);

private:
	// Compute the volume given distance from listenter
	// Assumes sound is LESS THAN distanceOfNoVolume away
	inline static float ComputeVolume(float distance, float distanceOfFullVolume, float distanceOfNoVolume);

	// Compute the pan (given the emitter in listener space)
	inline static float ComputePan(Vector3CRef emitter, float panFactor);
};

#endif
