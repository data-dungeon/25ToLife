/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for supersonic
/////////////////////////////////////////////////////////////////////////////
#ifndef SUPERSONIC_H
#define SUPERSONIC_H

#include "weapons/ttlprojectile.h"
#include "weapons/tracerset.h"
#include "weapons/hitsalvo.h"

// Forwards
class SupersonicProjectile;

// The object factory
class SupersonicProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	SupersonicProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

// The properties
class SupersonicProjectileProperties : public TTLProjectileProperties
{
public:
	SupersonicProjectileProperties(ProjectileCreator &creator, int index);
	virtual ~SupersonicProjectileProperties();

	// For parsing the data
	virtual void RegisterContext(const char *context);

	// After the load
	void PostLoadSetup();

	// Handle munition impacts
	void MunitionImpactFX(const CMunitionImpact &impact);

	// For supersonic
	float 			m_energy;
	float 			m_maxRange;

	// Tracer info
	char				m_tracerName[PROJSTRING_SIZE];

	// Our tracer creator
	TracerSet		*m_tracer;

	// The bullet whiz sound!
	char				m_whizSound[PROJSTRING_SIZE];
};

// The mighty class
class SupersonicProjectile : public TTLProjectile
{
public:
	SupersonicProjectile(const SupersonicProjectileProperties &properties);
	virtual ~SupersonicProjectile();

	// Fire!
	bool Fire(Weapon *weapon, const Vector3 &origin, const Vector3 &dir);

protected:
	// Alias our special properties
	const SupersonicProjectileProperties &m_ssProps;

	// Length
	float Range()
		{ return (m_ssProps.m_maxRange <= 0.0f) ? 120.0f : m_ssProps.m_maxRange; }

	// Setup the initial vector
	void SetupEntity(CLineEntity &entity, const Vector3 &origin, const Vector3 &dir, float d);

	// Cast a ray
	bool RayCast(HitSalvo &salvo, Vector3 shotOrigin, Vector3 shotDir, float energy, float d, int level = 0, bool noDeflect = false);

	// Handle an intersection
	virtual void HandleIntersection(HitSalvo &salvo, const Vector3 &shotOrigin, CLineQuery &test, float energy, bool deflected);

	// Handle a total miss
	virtual void HandleNonIntersection(HitSalvo &salvo, const Vector3 &shotOrigin, const Vector3 &shotEnd, bool deflected);

	// Deflect a ray based on material
	bool Deflect(CLineQuery &test, Vector3 &shotOrigin, Vector3 &shotDir, float &energy, float &d, bool &noDeflect);

	// Utility for figuring out shot start/end
	Vector3 ShotOrigin(int level, HitSalvo &salvo, const Vector3 &shotOrigin);
	Vector3 ShotEnd(HitSalvo &salvo, const Vector3 &shotOrigin,  const Vector3 &shotDir, float d);
};

#endif
