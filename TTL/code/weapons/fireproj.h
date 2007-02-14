/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for fire generators
/////////////////////////////////////////////////////////////////////////////
#ifndef FIREBANG_H
#define FIREBANG_H

#include "weapons/aoeproj.h"

// Forwards
class FireProjectile;

// The object factory
class FireProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	FireProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class FireProjectileProperties : public AreaOfEffectProjectileProperties
{
public:
	FireProjectileProperties(ProjectileCreator &creator, int index);
	~FireProjectileProperties() {}

	// For parsing the data
	void RegisterContext(const char *context);

	// Fire data
	float						m_fireDuration;
	float						m_fireDamage;
};

// The mighty class
class FireProjectile : public AreaOfEffectProjectile
{
public:
	FireProjectile(const FireProjectileProperties &properties);
	~FireProjectile();

protected:
	// Send the hit
 	void SendHit(Weapon *weapon, CActor *victim, const Vector3 &pos, u8 material, const Vector3 *intersection  = NULL, float factor = 1.0f);

	// Alias our special properties
	const FireProjectileProperties &m_fireProps;
};

#endif
