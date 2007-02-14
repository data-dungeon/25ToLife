/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for emitter projectiles!
/////////////////////////////////////////////////////////////////////////////
#ifndef EMITPROJ_H
#define EMITPROJ_H

#include "weapons/aoeproj.h"

// Forwards
class EmitterProjectile;

// The object factory
class EmitterProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	EmitterProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class EmitterProjectileProperties : public AreaOfEffectProjectileProperties
{
public:
	EmitterProjectileProperties(ProjectileCreator &creator, int index);
	~EmitterProjectileProperties() {}

	// For parsing the data
	virtual void RegisterContext(const char *context);

	// Handle explosions!
	virtual void HandleExplodeFX(ActorHandle owner, const SMunitionExplodeFX &fx);

	bool FindGround(const Vector3 &pos, float maxD, Vector3 &ground);
	SFXParticleEmitter *InsideOf(const char *name, const Vector3 &pos);
 	bool MergeSFX(SFXParticleEmitter *mergeWith, const Vector3 &pos);

	// Reset due to a checkpoint
	virtual void CheckpointReset() const;

	// Emitter data
	char						m_emitter[24];
	float						m_ttl;

	// Trigger data
	char						m_triggerMessage[32];
	Vector3Packed			m_triggerSize;
	float						m_triggerDuration;
};

// The mighty class
class EmitterProjectile : public AreaOfEffectProjectile
{
public:
	EmitterProjectile(const EmitterProjectileProperties &properties);
	~EmitterProjectile() {}

protected:
	// Alias our special properties
	const EmitterProjectileProperties &m_emitProps;
};

#endif
