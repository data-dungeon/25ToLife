///////////////////////////////////////////////////////////////////////////////
// SpecialFX light!
////////////////`///////////////////////////////////////////////////////////////
#ifndef SFXLIGHT_H
#define SFXLIGHT_H

#include "Game/Database/lightset.h"
#include "Effects/sfx/specialfx.h"

class SFXLight : public SpecialFXSticky
{
public:
	// Auto destructs after ttl has expired
	SFXLight(SpecialFXSystem &sfx, const char *name, float ttl);
	~SFXLight();

	// give back the subclass
	const char *Class()
		{ return "light"; }

	// High priority lights will reclaim a low
	enum Priority
	{
		SFXLIGHT_LOW_PRIORITY,
		SFXLIGHT_HIGH_PRIORITY
	};

	// Set this up for the light
	struct Params
	{
		Priority							priority;
		CLightRecord::LightTypes  	type;
		Vector3	  						color;
		float								attenuation;
		float								intensity;
		float								ttl;
	};

	// Create the emitter
	bool CreateLight(const Params &params);

	// These create and add! (negative ttl means you delete)
	static SFXLight *Create(const char *name, const Vector3 &pos, const Params &params);
	static SFXLight *Create(const char *name, CActor &actor, const char *bone, const Params &params);
	static void Destroy(SFXLight *light);

protected:
	// Reclaim a light!
	Priority GetPriority()
		{ return m_priority; }
	bool Reclaim();

	// Our update!
	void Update(float now, float deltaT);

	// The light!
	Priority			m_priority;
	CLightRecord	*m_light;
	bool				m_inLightSet;

	static const int 	s_maxLights;
	static int			s_lights;
};

#endif

