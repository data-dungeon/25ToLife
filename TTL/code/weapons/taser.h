/////////////////////////////////////////////////////////////////////////////
// Stuff for the taser
/////////////////////////////////////////////////////////////////////////////
#ifndef TASER_H
#define TASER_H

#include "weapons/supersonic.h"

// Forwards
class TaserProjectile;

// The object factory
class TaserProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	TaserProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class TaserProjectileProperties : public SupersonicProjectileProperties
{
public:
	TaserProjectileProperties(ProjectileCreator &creator, int index);
	~TaserProjectileProperties() {}

	// For parsing the data
	void RegisterContext(const char *context);

	bool Stunner() const
		{ return (m_stunDuration > 0.0f); }

	// For Taser
	float 			m_stunDuration;
};

// The mighty class
class TaserProjectile : public SupersonicProjectile
{
public:
	TaserProjectile(const TaserProjectileProperties &properties);
	~TaserProjectile() {}

protected:
	// Alias our special properties
	const TaserProjectileProperties &m_taserProps;

	// Handle an intersection
	void HandleIntersection(HitSalvo &salvo, const Vector3 &shotOrigin, CLineQuery &test, float energy, bool deflected);
};

#endif
