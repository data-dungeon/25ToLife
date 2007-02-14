/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for melee weapons
/////////////////////////////////////////////////////////////////////////////
#ifndef MELEEPROJ_H
#define MELEEPROJ_H

#include "weapons/ttlweapon.h"
#include "weapons/ttlprojectile.h"

// Forwards
class MeleeProjectile;

// The object factory
class MeleeProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	MeleeProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class MeleeProjectileProperties : public TTLProjectileProperties
{
public:
	MeleeProjectileProperties(ProjectileCreator &creator, int index);
	~MeleeProjectileProperties() {}

	// For parsing the data
	void RegisterContext(const char *context);

	bool Stunner() const
		{ return (m_stunDuration > 0.0f); }

	bool Melee() const
		{ return Math::Zero(m_stunDuration); }

	char						m_useBone[PROJSTRING_SIZE];
	float						m_range;
	char						m_material[PROJSTRING_SIZE];

	// The sounds and particles
	WeaponSoundParticle	m_landedHit;

	// Non-zero specified stun duration
	float						m_stunDuration;
	float						m_tearGasStrength;
};

// The mighty class
class MeleeProjectile : public TTLProjectile
{
public:
	MeleeProjectile(const MeleeProjectileProperties &properties);
	~MeleeProjectile();

	// Fire!
	bool Fire(Weapon *weapon, const Vector3 &origin, const Vector3 &dir);

protected:
	// Alias our special properties
	const MeleeProjectileProperties &m_mProps;

	// Handle the intersection
	void HandleIntersection(Weapon *weapon, const Vector3 &aim, CLineQuery &test);

	// The bone id
	BoneID				m_bone;
};

#endif
