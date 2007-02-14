/////////////////////////////////////////////////////////////////////////////
// Car bomb is used to generate a projectile-like explosion
/////////////////////////////////////////////////////////////////////////////
#ifndef CARBOMB_H
#define CARBOMB_H

#include "EngineHelper/Singleton.h"
#include "weapons/aoeproj.h"
#include "Components/weapons/weapdep.h"

class CarBomb
{
public:
	CarBomb();
	~CarBomb();

	void Explode(CActor *instigator, CActor *generator, const Vector3 &pos);

private:
	AreaOfEffectProjectile	*m_carBomb;
};

class CarBombDependency : public WeaponDependency
{
public:
	CarBombDependency() {}

	void InitializeOnce() {}
	void TerminateOnce() {}

	void InitializeEvery();
	void TerminateEvery();

	WeaponDependency::Phase WantPhase()
		{ return WeaponDependency::POSTINIT; }
};

DefineSingleton(CarBomb)
#define g_carBomb GetSingleton(CarBomb)

#endif
