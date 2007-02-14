/////////////////////////////////////////////////////////////////////////////
// Free (i.e. weaponless) projectile invocation
/////////////////////////////////////////////////////////////////////////////
#ifndef FREEPROJ_H
#define FREEPROJ_H

#include "EngineHelper/Singleton.h"
#include "weapons/ttlprojectile.h"
#include "Components/weapons/weapdep.h"
#include "container/Darray.h"

class CActor;

class FreeProjectile
{
public:
	FreeProjectile();
	~FreeProjectile();

	// Explosions affect all actors around the event.
	// instigator is the cause of the explosion (i.e. shooter of the car)
	// Generator is the actor generating the explosion that should be ignored in collisons
	bool Explode(const char *projectileName, CActor *instigator, CActor *generator, const Vector3 &pos);

	// Fire shoots the projectile in the specified direction
	bool Fire(const char *projectileName, CActor *shooter, const Vector3 &pos, const Vector3 &dir);

	// Fire specifially targets a victim.
	bool Fire(const char *projectileName, CActor *shooter, CActor *victim, const Vector3 &pos);

private:
	struct Instance
	{
		char 				name[PROJSTRING_SIZE];
		TTLProjectile 	*projectile;
	};
	DynamicArray<Instance>	m_list;

	// Find it!
	Instance *Find(const char *name);
};

class FreeProjectileDependency : public WeaponDependency
{
public:
	FreeProjectileDependency() {}

	void InitializeOnce() {}
	void TerminateOnce() {}

	void InitializeEvery();
	void TerminateEvery();

	WeaponDependency::Phase WantPhase()
		{ return WeaponDependency::POSTINIT; }
};

DefineSingleton(FreeProjectile)
#define g_freeProjectile GetSingleton(FreeProjectile)

#endif
