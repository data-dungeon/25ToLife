/***************************************************************************/
// Simple utility class to compute doppler effects
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// Return the modified frequency with a stationary source, and
// a moving detector
/***************************************************************************/
float Doppler::SourceRestDetectorMoving(
float frequency,
Vector3CRef sourcePos,
Vector3CRef detectorPos,
Vector3CRef detectorV,
float sos)
{
	// What is the detector's speed towards the source?
	float axisSpeed = AxisSpeed(sourcePos - detectorPos, detectorV);

	// Faster than the speed of sound?
	if (Math::Abs(axisSpeed) >= sos)
		return frequency;
	else
		return (frequency * (sos + axisSpeed) / sos);
}

/***************************************************************************/
// Return the modified frequency with a moving source, and a detector
// at rest
/***************************************************************************/
float Doppler::SourceMovingDetectorRest(
float frequency,
Vector3CRef sourcePos,
Vector3CRef sourceV,
Vector3CRef detectorPos,
float sos)
{
	// What is the source's speed towards the detector?
	float axisSpeed = AxisSpeed(detectorPos - sourcePos, sourceV);

	// Faster than the speed of sound?
	if (Math::Abs(axisSpeed) >= sos)
		return frequency;
	else
		return (frequency * sos / (sos - axisSpeed));
}

/***************************************************************************/
// Both the source and detector are moving
/***************************************************************************/
float Doppler::SourceAndDetectorMoving(
float frequency,
Vector3CRef sourcePos,
Vector3CRef sourceV,
Vector3CRef detectorPos,
Vector3CRef detectorV,
float sos)
{
	// What is the detector's speed towards the source?
	float detectorAxisSpeed = AxisSpeed(sourcePos - detectorPos, detectorV);

	// Faster than the speed of sound?
	if (Math::Abs(detectorAxisSpeed) >= sos)
		return frequency;

	// What is the source's speed towards the detector?
	float sourceAxisSpeed = AxisSpeed(detectorPos - sourcePos, sourceV);

	// Faster than the speed of sound?
	if (Math::Abs(sourceAxisSpeed) >= sos)
		return frequency;

	// Compute the biggie
	return (frequency * (sos + detectorAxisSpeed) / (sos - sourceAxisSpeed));
}

/***************************************************************************/
// Both the source and detector are moving, quicker computation that only holds
// if |sourceV| and |detectorV| are << sos
/***************************************************************************/
float Doppler::QuickSourceAndDetectorMoving(
float frequency,
Vector3CRef sourcePos,
Vector3CRef sourceV,
Vector3CRef detectorPos,
Vector3CRef detectorV,
float sos)
{
	// what is the closing/parting speed?
	Vector3 d = (detectorPos - sourcePos).Normalized();
	float speed = Vector3::Dot(d, sourceV) - Vector3::Dot(d, detectorV);

	// Faster than the speed of sound?
	if (Math::Abs(speed) >= sos)
		return frequency;
	else
		return (frequency * (1.0f + speed / sos));
}

/***************************************************************************/
// Compute who's moving, who's at rest and do the right thing!
/***************************************************************************/
float Doppler::Compute(
float frequency,
Vector3CRef sourcePos,
Vector3CRef sourceV,
Vector3CRef detectorPos,
Vector3CRef detectorV,
float sos)
{
	bool sourceAtRest = Math::Zero(sourceV.LengthSquared());
	bool detectorAtRest = Math::Zero(detectorV.LengthSquared());

	if (sourceAtRest && detectorAtRest)
		return frequency;
	else if (sourceAtRest)
		return SourceRestDetectorMoving(frequency, sourcePos, detectorPos, detectorV, sos);
	else if (detectorAtRest)
		return SourceMovingDetectorRest(frequency, sourcePos, sourceV, detectorPos, sos);
	else
		return QuickSourceAndDetectorMoving(frequency, sourcePos, sourceV, detectorPos, detectorV, sos);
}

