///////////////////////////////////////////////////////////////////////////////
// SpecialFX distortions!
////////////////`///////////////////////////////////////////////////////////////
#ifndef SFXDISTORT_H
#define SFXDISTORT_H

#include "Effects/worldspacedistort.h"
#include "Effects/sfx/specialfx.h"

class SFXDistort : public SpecialFXSticky
{
public:
	// Auto destructs after ttl has expired
	SFXDistort(SpecialFXSystem &sfx, const char *name, float ttl);
	~SFXDistort();

	// give back the subclass
	const char *Class()
		{ return "distort"; }

	// High priority lights will reclaim a low
	enum Priority
	{
		SFXDISTORT_LOW_PRIORITY,
		SFXDISTORT_HIGH_PRIORITY
	};

	// Set this up for the light
	struct Params
	{
		Priority									priority;
		bool										hasNormal;
		Vector3									normal;

		float										frequency, amplitude;
		WorldSpaceDistort::DistortParams distort;
	};
	static void ParamDefaults(Params &params, float size, float attack = 0.0f, float sustain = 0.0f, float decay = 0.0f);

	// These create and add! (negative ttl means you delete)
	static SFXDistort *Create(const char *name, const Vector3 &pos, const Params &params);
	static SFXDistort *Create(const char *name, CActor &actor, const char *bone, const Params &params);
	static void Destroy(SFXDistort *sfx);

protected:
	// Create the emitter
	bool Create(const Params &params);

	// Reclaim a light!
	Priority GetPriority()
		{ return m_priority; }
	bool Reclaim();

	// Our update!
	void Update(float now, float deltaT);

	// The distortion!
	Priority				m_priority;
	bool					m_hasNormal;
	Vector3				m_normal;
	int 					m_handle;

	// Get the camera normal
	Vector3 FaceTheScreen(const Vector3 &pos);

	static const int 	s_maxDistortions;
	static int			s_distortions;
};

#endif


