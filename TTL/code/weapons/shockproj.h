/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for fire generators
/////////////////////////////////////////////////////////////////////////////
#ifndef SHOCKPROJ_H
#define SHOCKPROJ_H

#include "weapons/aoeproj.h"

// Forwards
class ShockProjectile;

// The object factory
class ShockProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	ShockProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class ShockProjectileProperties : public AreaOfEffectProjectileProperties
{
public:
	ShockProjectileProperties(ProjectileCreator &creator, int index);
	~ShockProjectileProperties() {}

	// For parsing the data
	void RegisterContext(const char *context);

	// Shock data
	float						m_shockDuration;
};

// The mighty class
class ShockProjectile : public AreaOfEffectProjectile
{
public:
	ShockProjectile(const ShockProjectileProperties &properties);
	~ShockProjectile();

protected:
	// Send the hit
 	void SendHit(Weapon *weapon, CActor *victim, const Vector3 &pos, u8 material, const Vector3 *intersection  = NULL, float factor = 1.0f);

	// Alias our special properties
	const ShockProjectileProperties &m_shockProps;
};

#endif
