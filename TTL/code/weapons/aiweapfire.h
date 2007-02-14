/////////////////////////////////////////////////////////////////////////////
// AI Weapon fire controller
/////////////////////////////////////////////////////////////////////////////
#ifndef AIWEAPFIRE_H
#define AIWEAPFIRE_H

#include "weapons/ttlweapon.h"
#include "weapons/melee.h"
#include "weapons/lobbed.h"
#include "weapons/firearm.h"
#include "weapons/aiweaptarget.h"

class AIWeaponFire
{
public:
	AIWeaponFire(Weapon &forWeapon) {}
	virtual ~AIWeaponFire() {}

	// Request a fire
	virtual bool Fire(int times = 1, float deviance = 0.0f, float rateOfAttack = 0.0f) = 0;

	// How many shots have been fired in this salvo?
	virtual int ShotsFired() = 0;

	// Abort firing (returns true if actually was firing)
	virtual bool AbortFire() = 0;

	// Call this once per frame
	virtual void Update(CCompActor &shooter, AIWeaponTarget &target) = 0;

	// Is this event done?
	virtual bool Done() = 0;

protected:
	// The name says it all
	bool InFrontAndInRange(TTLWeapon &weapon, CCompActor &shooter, AIWeaponTarget &target, float deviance, float rangeMultiplier = 1.0f );

	// What time is it?
	inline float Now();
};

class AIWeaponFireMelee : public AIWeaponFire
{
public:
	AIWeaponFireMelee(Melee &forWeapon);
	~AIWeaponFireMelee();

	// Request a fire
	bool Fire(int times, float deviance, float rateOfAttack);

	// How many shots have been fired in this salvo?
	int ShotsFired();

	// Abort firing (returns true if actually was firing)
	bool AbortFire();

	// Call this once per frame
	void Update(CCompActor &shooter, AIWeaponTarget &target);

	// Is this event done?
	bool Done();

private:
	Melee			&m_weapon;
	int			m_timesToAttack, m_attacksSoFar;
	float 		m_deviance;
	float 		m_rateOfAttack, m_nextAttack;

	// Could this weapon even hit?
	bool CouldHit(CCompActor &shooter, AIWeaponTarget &target);
};

class AIWeaponFireLobbed : public AIWeaponFire
{
public:
	AIWeaponFireLobbed(Lobbed &forWeapon);
	~AIWeaponFireLobbed();

	// Request a fire
	bool Fire(int times = 1, float deviance = 0.0f, float rateOfAttack = 0.0f);

	// How many shots have been fired in this salvo?
	int ShotsFired();

	// Abort firing (returns true if actually was firing)
	bool AbortFire();

	// Call this once per frame
	void Update(CCompActor &shooter, AIWeaponTarget &target);

	// Is this event done?
	bool Done();

private:
	Lobbed		&m_weapon;
	int			m_throw, m_thrown;
	float 		m_deviance;
	float 		m_throwRate, m_nextThrow;

	// Could this weapon even hit?
	bool CouldHit(CCompActor &shooter, AIWeaponTarget &target);
	bool ComputeThrow(CCompActor &shooter, AIWeaponTarget &target, Vector3 &launchDir, float &strength);
	float OptimizeLaunchAngle(const Vector3 &shooter, const Vector3 &target);
	bool MakeLaunchDir(const Vector3 &toTarget, float theta, Vector3 &launchDir);
};

class AIWeaponFireFirearm : public AIWeaponFire
{
public:
	AIWeaponFireFirearm(Firearm &forWeapon);
	~AIWeaponFireFirearm();

	// Request a fire
	bool Fire(int times = 1, float deviance = 0.0f, float rateOfAttack = 0.0f);

	// How many shots have been fired in this salvo?
	int ShotsFired();

	// Abort firing (returns true if actually was firing)
	bool AbortFire();

	// Call this once per frame
	void Update(CCompActor &shooter, AIWeaponTarget &target);

	// Is this event done?
	bool Done();

private:
	Firearm		&m_weapon;
	bool			m_auto, m_subsonic;
	bool 			m_triggerDown;
	int			m_roundsWhenDone, m_roundsAtStart;
	float			m_deviance;
	float 		m_rateOfFire, m_nextFire;

	// Could this weapon even hit?
	bool CouldHit(CCompActor &shooter, AIWeaponTarget &target, float &d);
	Vector3 ComputeSubsonicAim(const Vector3 &targetPosition, float d);

	bool PullTrigger();
	bool ReleaseTrigger();
};

#endif
