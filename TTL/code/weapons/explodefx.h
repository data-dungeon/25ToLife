/////////////////////////////////////////////////////////////////////////////
// World space distort information for explosions
/////////////////////////////////////////////////////////////////////////////
#ifndef EXPLODEFX_H
#define EXPLODEFX_H

#include "Components/weapons/weaplib.h"
#include "Components/weapons/weapdep.h"

class ExplodeFX;
class ExplodeFXCreator;

// Our properties
class ExplodeFXProperties : public VarObjProperties<ExplodeFX>
{
public:
	// Weee
	ExplodeFXProperties(ExplodeFXCreator &creator, int index);
	~ExplodeFXProperties() {}

	// For parsing the data
	void RegisterContext(const char *context);

	// After the load
 	void PostLoadSetup();

	// sound
	char						m_sound1[32];
	float						m_sound1Pitch;
	char						m_sound2[32];
	float						m_sound2Pitch;
	
	// Particle
	char						m_particle[32];
	float						m_particleScale;

	// Should this shake the avatar's controller
	bool 						m_avatarShake;

	// Dynamic light
	bool						m_wantLight;
	Vector3Packed 			m_lightColor;
	float						m_lightAttenuation;
	float						m_lightIntensity;
	float						m_lightDuration;

	// Distortion settings
	bool						m_wantDistort;
	float						m_distortInnerRadius;
	float						m_distortOuterRadius;
	float						m_distortAttack;
	float						m_distortSustain;
	float						m_distortDecay;
	float						m_distortFrequency;
	float						m_distortAmplitude;
	int						m_distortSlices;
	int						m_distortRings;
	float						m_distortAspectRatio;
	bool						m_distortUseOmega;
	float						m_distortSignalScale;
	Vector4					m_distortInsideColor;
	Vector4					m_distortOuterColor;
	Vector4					m_distortMidColor;
	float						m_distortSpiralStrength;
	float						m_distortSpiralMaxAge;
	float						m_distortSpiralInitialAngle;
	float						m_distortFixedSpiralAngle;
	float						m_distortConicDepth;

	bool Go(const Vector3 &pos, const Vector3 &dir) const;
};

// The actual implementation
class ExplodeFX
{
public:
	ExplodeFX(const ExplodeFXProperties &properties) : m_properties(properties) {}
	~ExplodeFX() {}

	// Trigger the FX
	bool Trigger(ActorHandle from, int projectileType, const Vector3 &pos, const Vector3 &dir);

protected:
	// The Firearms propers
	const ExplodeFXProperties	&m_properties;
};

// The object factory
class ExplodeFXCreator : public VarObjCreator<ExplodeFX>
{
public:
	// Hello
	ExplodeFXCreator() :
		VarObjCreator<ExplodeFX>("expfx") {}

	// You must provide this for the type
	ExplodeFX *CreateInstance(const VarObjProperties<ExplodeFX> &properties)
		{ return new ExplodeFX((const ExplodeFXProperties &)properties); }

protected:
	// Subclass this to provide new
	VarObjProperties<ExplodeFX> *NewProperties(int index)
		{ return new ExplodeFXProperties(*this, index); }
};

// The manager of acive ExplodeFXs
class ExplodeFXLibrary : public VarObjLibrary<ExplodeFX>
{
public:
	// Initialize all the known ExplodeFXs
	ExplodeFXLibrary() {}
	~ExplodeFXLibrary() {}

	// Run an explosion
	void HandleTrigger(const SMunitionExplodeFX &fx);

protected:
	// What is the path
	const char *Path()
		{ return "weapons\\"; }

	// What is the context name
	const char *Context()
		{ return "expfx"; }
};

class ExplodeFXDependency : public WeaponDependency
{
public:
	ExplodeFXDependency() {}

	void InitializeOnce();
	void TerminateOnce();

	WeaponDependency::Phase WantPhase()
		{ return WeaponDependency::PREINIT; }
};

// ExplodeFX singleton
DefineSingleton(ExplodeFXLibrary)
#define g_explodeFXLibrary GetSingleton(ExplodeFXLibrary)

#endif
