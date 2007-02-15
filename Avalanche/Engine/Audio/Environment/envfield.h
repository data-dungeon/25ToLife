/***************************************************************************/
// An environmental sound field
/***************************************************************************/
#ifndef ENVFIELD_H
#define ENVFIELD_H

#include "CollideHelper/zone.h"
#include "Audio/Environment/ambsound.h"

class EnvField : public ZoneOBB
{
public:
	EnvField(int id, OBB &obb,
				SoundDriver::ENVEFFECT effectID,
				float effectVolume, float effectTaper,
				AmbientSound &ambientSound);
	~EnvField() {}

	bool Active()
		{ return true; }

	// How big are we
	float Size() const 
		{ return size; }

	// We just expose everything
	SoundDriver::ENVEFFECT Effect() const
		{ return effect; }
	float EffectVolume() const
		{ return volume; }
	float Volume(const Vector3 &forPoint) const;

	// Get at the ambient sound
	const AmbientSound &Ambient() const
		{ return ambient; }
	void SetAmbient(AmbientSound &ambientSound)
		{ ambient = ambientSound; }
	void ClearAmbient()
		{ ambient.Clear(); }

protected:
	// The volume of our obb
	float							size;

	// The effects field
	SoundDriver::ENVEFFECT	effect;
	float							volume;
	bool							hasTaper;
	float							taperMin, invTaperRange;

	// The ambient parameters
	AmbientSound				ambient;
};

#endif
