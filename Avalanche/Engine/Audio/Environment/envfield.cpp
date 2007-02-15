/***************************************************************************/
// An environmental sound field
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
/***************************************************************************/
EnvField::EnvField(
int id,
OBB &obb,
SoundDriver::ENVEFFECT effectID,
float effectVolume,
float effectTaper,
AmbientSound &ambientSound) :
ZoneOBB(id, obb)
{
	effect = effectID;
	volume = effectVolume;

	SetAmbient(ambientSound);

	const AABB &aabb = obb.GetConstAABB();
	size = aabb.maxV.x() * aabb.maxV.y() * aabb.maxV.z();

	float minorAxis = Math::Min(aabb.maxV.x(), aabb.maxV.y(), aabb.maxV.z());
	float majorAxis = aabb.maxV.Length();
	taperMin = minorAxis + (majorAxis - minorAxis) * (1.0f - effectTaper);
	if (Math::Zero(majorAxis - taperMin))
		hasTaper = false;
	else
	{
		invTaperRange = 1.0f / (majorAxis - taperMin);
		hasTaper = true;
	}
}

/***************************************************************************/
/***************************************************************************/
float EnvField::Volume(
const Vector3 &forPoint) const
{
	// Simple case
	if (!hasTaper)
		return volume;

	// Where is this inside the obb?
	float d = (forPoint - *obb.Center()).Length();

	// Compute the volume
	float taperVolume = 1.0f - Math::Clamp((d - taperMin) * invTaperRange);
	return taperVolume * volume;
}

