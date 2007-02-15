/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Components/weapons/volib.h"
#include "Environ/Munition.h"

class ProjectileCreator;
class Projectile;

// Size of a projectile string
#define PROJSTRING_SIZE		32

// The projectile specification
class ProjectileProperties : public VarObjProperties<Projectile>
{
public:
	ProjectileProperties(ProjectileCreator &creator, int index);
	virtual ~ProjectileProperties();

	// For parsing the data
	virtual void RegisterContext(const char *context);

	// Init every game run
	void InitializeEvery();

	// Create an instance using these properties
	virtual Projectile *CreateInstance();

	// Our material
	char	m_material[PROJSTRING_SIZE];
	int	m_surfaceType;

	// Damage (could be per second)
	float m_damage;

	// Spawn count (how many does this make?)
	int 	m_spawnCount;
	float	m_spawnAngle;

	// What is the flight sound?
	char 	m_flightSound[PROJSTRING_SIZE];
};

// The actual projectile
class Projectile
{
public:
	Projectile(const ProjectileProperties &properties);
	virtual ~Projectile() {}

	// What is our name?
	const char *Name();

	// Get at the props
	const ProjectileProperties &GetProperties()
		{ return m_properties; }

	// Set the owner
	void SetOwner(CActor *owner)
		{ m_owner = CActor::ToHandle(owner); }
	CActor *GetOwner()
		{ return CActor::FromHandle(m_owner); }

	// Handle activation/deactivation
	virtual void Activate(Weapon &weapon) {}
	virtual void Deactivate(Weapon &weapon) {}
	virtual void Update(Weapon &weapon) {}

	// Fire!!!
	virtual bool Fire(Weapon *weapon, const Vector3 &origin, const Vector3 &dir) = 0;

protected:
	// Pointer to our data
	const ProjectileProperties		&m_properties;
	ActorHandle							m_owner;
};

#endif
