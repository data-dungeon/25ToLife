///////////////////////////////////////////////////////////////////////////////
// SpecialFX particle emitter!
///////////////////////////////////////////////////////////////////////////////
#ifndef SFXPEMIT_H
#define SFXPEMIT_H

#include "Effects/sfx/specialfx.h"

class SFXParticleEmitter : public SpecialFXSticky
{
public:
	// Auto destructs after ttl has expired
	SFXParticleEmitter(SpecialFXSystem &sfx, const char *name, float ttl);
	~SFXParticleEmitter();

	// give back the subclass
	const char *Class()
		{ return "pemitter"; }

	// Reset the TTL
	void ResetTTL(float newTTL, bool includeSiblings = true);

	// Create the emitter
	bool CreateEmitter();
	bool DestroyEmitter();

	// These create and add! (negative ttl means ask the emitter)
	static SFXParticleEmitter *Create(const char *emitterName, const Vector3 &pos, float ttl = -1.0f);
	static SFXParticleEmitter *Create(const char *emitterName, CActor &actor, const char *bone = NULL, float ttl = -1.0f);

protected:
	// Our update!
	void Update(float now, float deltaT);

	// Our particle emitter handle
	t_Handle			m_handle;

	// The data
	Vector3			m_pos;
	Vector3			m_velocity;
	static const Vector3 s_up;
};

#endif

