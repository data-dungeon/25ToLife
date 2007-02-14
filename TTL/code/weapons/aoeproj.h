/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for area of effect
/////////////////////////////////////////////////////////////////////////////
#ifndef AOEPROJ_H
#define AOEPROJ_H

#include "weapons/ttlweapon.h"
#include "weapons/ttlprojectile.h"
#include "weapons/explodefx.h"
#include "weapons/seethru.h"

// Forwards
class AreaOfEffectProjectile;

// The object factory
class AreaOfEffectProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	AreaOfEffectProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class AreaOfEffectProjectileProperties : public TTLProjectileProperties
{
public:
	AreaOfEffectProjectileProperties(ProjectileCreator &creator, int index);
	virtual ~AreaOfEffectProjectileProperties();

	// For parsing the data
	virtual void RegisterContext(const char *context);

	// After the load
	virtual void PostLoadSetup();

	// Is this projectile explosive?
	virtual bool Explosive() const
		{ return true; }

	// Damage info
	float						m_damageRadius;
	float						m_fullDamageRadius;
	bool						m_scanHit;

	float						m_deafnessDuration;

	// Material
	char						m_material[PROJSTRING_SIZE];

	// The explosion special fx
	char						m_explodeFXName[PROJSTRING_SIZE];

	// Our explosion FX
	ExplodeFX				*m_explodeFX;
};

// The mighty class
class AreaOfEffectProjectile : public TTLProjectile
{
public:
	AreaOfEffectProjectile(const AreaOfEffectProjectileProperties &properties);
	virtual ~AreaOfEffectProjectile() {}

	// Setup the generator
	void SetGenerator(CActor *actor)
		{ m_generator = CActor::ToHandle(actor); }

	// Fire!
	virtual bool Fire(Weapon *weapon, const Vector3 &origin, const Vector3 &dir);

protected:
	// Alias our special properties
	const AreaOfEffectProjectileProperties &m_aoeProps;

	// What actor is generating us?
	ActorHandle			m_generator;

	// Do the hits
	bool SimpleHit(Weapon *weapon, const Vector3 &pos, const Vector3 &dir);
	bool ComplexHit(Weapon *weapon, const Vector3 &pos, const Vector3 &dir);

	// Send the hit
	virtual void SendHit(Weapon *weapon, CActor *victim, const Vector3 &pos, u8 material, const Vector3 *intersection  = NULL, float factor = 1.0f);

	// Compute damage
	float ComputeDamageFactor(float distance);

	// Tell the world
	bool ImpactWorld(Weapon *weapon, const Vector3 &pos, const Vector3 &normal);

	// Get a bone in world
	Vector3 GetBoneInWorld(CActor &forWho, const char *boneName);

	// Test a point in space for clear
	bool InView(const Vector3 &eye, const Vector3 &target, CActor *ignore)
		{ SeeThru::LookResult result; return !SeeThru::Look(eye, target, ignore, result, false); }
	bool TestCombatant(Weapon *weapon, const Vector3 &pos, CActor *victim, u8 material);
};

#endif
