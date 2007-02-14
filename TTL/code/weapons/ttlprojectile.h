/////////////////////////////////////////////////////////////////////////////
// Projectile base for the game
/////////////////////////////////////////////////////////////////////////////
#ifndef TTLPROJECTILE_H
#define TTLPROJECTILE_H

#include "Components/weapons/projlib.h"
#include "Environ/Munition.h"

class TTLProjectileProperties : public ProjectileProperties
{
public:
	TTLProjectileProperties(ProjectileCreator &creator, int index);
	virtual ~TTLProjectileProperties() {}

	// Register our context
	void RegisterContext(const char *context);

	// After the load
	void PostLoadSetup();

	// Handle munition impacts
	virtual void MunitionImpactFX(const CMunitionImpact &impact) {}

	// Handle explosions!
	virtual void HandleExplodeFX(ActorHandle owner, const SMunitionExplodeFX &fx) {}

	// Reset due to a checkpoint
	virtual void CheckpointReset() const {}

	// our data
	char		m_ammoType[16];

	// Is this projectile explosive?
	virtual bool Explosive() const
		{ return false; }

	// Is this a stunnable weapon?
	virtual bool Stunner() const
		{ return false; }

	virtual bool Melee() const
		{ return false;  }
};

// The mighty class
class TTLProjectile : public Projectile
{
public:
	TTLProjectile(const TTLProjectileProperties &properties);
	virtual ~TTLProjectile() {}

	// What type of ammo are we?
	const char *GetAmmoType() const
		{ return m_ttlProps.m_ammoType; }

	// Fire requires as many as you know
	virtual bool Fire(Weapon *weapon, const Vector3 &origin, const Vector3 &dir) = 0;

	// Dry fire runs the effects without the damage
	virtual bool DryFire(const Vector3 &origin,	const Vector3 &dir)
		{ return false; }

private:
	const TTLProjectileProperties		&m_ttlProps;
};

#endif
