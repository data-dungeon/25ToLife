/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for trigger based effects
/////////////////////////////////////////////////////////////////////////////
#ifndef TRIGGERPROJ_H
#define TRIGGERPROJ_H

#include "weapons/aoeproj.h"

// Forwards
class TriggerProjectile;

// The object factory
class TriggerProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	TriggerProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class TriggerProjectileProperties : public AreaOfEffectProjectileProperties
{
public:
	TriggerProjectileProperties(ProjectileCreator &creator, int index);
	~TriggerProjectileProperties() {}

	// For parsing the data
	virtual void RegisterContext(const char *context);

	// Handle explosions!
	virtual void HandleExplodeFX(ActorHandle owner, const SMunitionExplodeFX &fx);

	// Reset due to a checkpoint
	virtual void CheckpointReset() const;

	// Trigger data
	char						m_message[32];
	Vector3Packed			m_size;
	float						m_duration;
};

// The mighty class
class TriggerProjectile : public AreaOfEffectProjectile
{
public:
	TriggerProjectile(const TriggerProjectileProperties &properties);
	~TriggerProjectile() {}

	// Fire!
	bool Fire(Weapon *weapon, const Vector3 &origin, const Vector3 &dir);

protected:
	// Alias our special properties
	const TriggerProjectileProperties &m_tProps;
};

#endif
