/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for flash bangs
/////////////////////////////////////////////////////////////////////////////
#ifndef FLASHBANG_H
#define FLASHBANG_H

#include "weapons/aoeproj.h"

// Forwards
class FlashBangProjectile;

// The object factory
class FlashBangProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	FlashBangProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class FlashBangProjectileProperties : public AreaOfEffectProjectileProperties
{
public:
	FlashBangProjectileProperties(ProjectileCreator &creator, int index);
	~FlashBangProjectileProperties() {}

	// For parsing the data
	void RegisterContext(const char *context);

	// FlashBang data
	float						m_flashIntensity;
	float						m_fullBlind;
};

// The mighty class
class FlashBangProjectile : public AreaOfEffectProjectile
{
public:
	FlashBangProjectile(const FlashBangProjectileProperties &properties);
	~FlashBangProjectile();

protected:
	// Send the hit
 	void SendHit(Weapon *weapon, CActor *actor, const Vector3 &pos, u8 material, const Vector3 *intersection  = NULL, float factor = 1.0f);

	// Alias our special properties
	const FlashBangProjectileProperties &m_flashProps;
};

#endif
