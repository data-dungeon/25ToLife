/***************************************************************************/
// Simple utility class to compute doppler effects
/***************************************************************************/
#ifndef DOPPLER_H
#define DOPPLER_H

#include "Math/Vector.h"

// Default speed of sound, in m/s
#define REAL_SPEEDOFSOUND		343.0f
#define DEFAULT_SPEEDOFSOUND	REAL_SPEEDOFSOUND

class Doppler
{
public:
	// Return the modified frequency with a stationary source, and
	// a moving detector
	static float SourceRestDetectorMoving(float frequency,
													  Vector3CRef sourcePos,
													  Vector3CRef detectorPos, Vector3CRef detectorV,
													  float sos = DEFAULT_SPEEDOFSOUND);

	// Return the modified frequency with a moving source, and a detector
	// at rest
	static float SourceMovingDetectorRest(float frequency,
													  Vector3CRef sourcePos, Vector3CRef sourceV,
													  Vector3CRef detectorPos,
													  float sos = DEFAULT_SPEEDOFSOUND);

	// Both the source and detector are moving
	static float SourceAndDetectorMoving(float frequency,
													 Vector3CRef sourcePos, Vector3CRef sourceV,
													 Vector3CRef detectorPos, Vector3CRef detectorV,
													 float sos = DEFAULT_SPEEDOFSOUND);

	// Both the source and detector are moving, quicker computation that only holds
	// if |sourceV| and |detectorV| are << sos
	static float QuickSourceAndDetectorMoving(float frequency,
			 			 									Vector3CRef sourcePos, Vector3CRef sourceV,
															Vector3CRef detectorPos, Vector3CRef detectorV,
															float sos = DEFAULT_SPEEDOFSOUND);

	// Compute who's moving, who's at rest and do the right thing!
	static float Compute(float frequency,
								 Vector3CRef sourcePos, Vector3CRef sourceV,
								 Vector3CRef detectorPos, Vector3CRef detectorV,
								 float sos = DEFAULT_SPEEDOFSOUND);

private:
	// Return the speed along an axis
	inline static float AxisSpeed(Vector3CRef axis, Vector3CRef v)
		{ return Vector3::Dot(axis.Normalized(), v); }
};

#endif
