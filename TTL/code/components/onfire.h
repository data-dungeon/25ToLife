/////////////////////////////////////////////////////////////////////////////
// Oh shit, we are on fire!
/////////////////////////////////////////////////////////////////////////////
#ifndef ONFIRE_H
#define ONFIRE_H

#include "Components/Component.h"
#include "Effects/sfx/sfxlight.h"

class OnFire : public CActorComponent
{
public:
	// Pass in a list (terminated with a NULL string)
	// List must be static
	OnFire(CCompActor &actor);
	~OnFire();

	// Our interface support
	const char *ComponentName()
		{ return "OnFire"; }

	// Reset!
	void Reset();

	// How are we burning?
	enum BurnType
	{
		BURNTYPE_NONE,
		BURNTYPE_FIRE,
		BURNTYPE_STUNNED,
		BURNTYPE_SHOCK,
		BURNTYPES
	};

	// Are we on fire?
	BurnType Burning()
		{ return m_burnType; }

	// The update
	void EndUpdate(float i_deltaSec);

	// the animation
	void RunAnimation();

private:
	BurnType		m_burnType;
	Vector3		m_velocity;
	SFXLight		*m_light;
	SoundHandle	m_sound;

	struct BurningBone
	{
		BoneID	bone;
		t_Handle	emitter;
		Vector3	pos;
	};
	BurningBone *m_bone;
	int m_bones;
	bool CreateBones();
	void UpdateBones();
	void StartBurn(BurnType type);
	void QuenchBurn();

	// The bone sets
	static const char *s_boneNameSP[];
	static const int s_bonesSP;
	static const char *s_boneNameMP[];
	static const int s_bonesMP;

	struct Effect
	{
		const char *emitterName;
		const char *soundName;
		bool			wantLight;
		Vector3		lightColor;
		float			lightIntensity;
		float			lightAttenuation;
	};
	static const Effect s_effect[BURNTYPES];
};

#endif
