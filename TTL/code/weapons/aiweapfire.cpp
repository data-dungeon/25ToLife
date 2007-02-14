/////////////////////////////////////////////////////////////////////////////
// AI Weapon fire controller
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/seethru.h"
#include "weapons/aiweapfire.h"

/////////////////////////////////////////////////////////////////////////////
// The name says it all
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFire::InFrontAndInRange(
TTLWeapon &weapon,
CCompActor &shooter,
AIWeaponTarget &target,
float deviance,
float rangeMultiplier)
{
	// Form a vector to the target
	Vector3 d = target.PositionInWorld() - shooter.GetFocusInWorld();

#if 0
	// What is the actor's facing?
	Vector3 dir = shooter.GetBodyToWorld().GetRow2();

	// Is the target in front
	if (Vector3::Dot(d, dir) < 0.0f)
		return false;
#endif

	// Is the target in range?
	float dSquared = d.LengthSquared();
	float range = (rangeMultiplier < 0.0f) ? -rangeMultiplier : weapon.MaxRange() * rangeMultiplier;
	return (dSquared <= Math::Square(range));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline float AIWeaponFire::Now()
{
	return g_timer.GetEndSec();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AIWeaponFireMelee::AIWeaponFireMelee(Melee &forWeapon) :
AIWeaponFire(forWeapon),
m_weapon(forWeapon)
{
	m_timesToAttack = 0;
	m_attacksSoFar = 0;
	m_deviance = 0.0f;
	m_rateOfAttack = 0.0f;
	m_nextAttack = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AIWeaponFireMelee::~AIWeaponFireMelee()
{
}

/////////////////////////////////////////////////////////////////////////////
// Request a fire
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireMelee::Fire(
int times,
float deviance,
float rateOfAttack)
{
	ASSERT(times > 0);

	// Abort!
	AbortFire();

	// Set it up
	m_timesToAttack = times;
	m_attacksSoFar = 0;
	m_deviance = deviance;
	m_rateOfAttack = !Math::Zero(rateOfAttack) ? rateOfAttack : m_weapon.RateOfAttack() * 0.5f;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// How many shots have been fired in this salvo?
/////////////////////////////////////////////////////////////////////////////
int AIWeaponFireMelee::ShotsFired()
{
	return m_attacksSoFar;
}

/////////////////////////////////////////////////////////////////////////////
// Abort firing (returns true if actually was firing)
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireMelee::AbortFire()
{
	bool wasFiring = m_timesToAttack > 0;
	m_timesToAttack = 0;
	return wasFiring;
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame
/////////////////////////////////////////////////////////////////////////////
void AIWeaponFireMelee::Update(
CCompActor &shooter,
AIWeaponTarget &target)
{
	// Duh
	if (Done())
		return;

	if (m_timesToAttack > 0 && Now() > m_nextAttack)
	{
		if (CouldHit(shooter, target))
		{
			m_weapon.SetAim(target.PositionInWorld(), Math::Deg2Rad(m_deviance));
			if (m_weapon.AIFire())
			{
				--m_timesToAttack;
				++m_attacksSoFar;
			}
		}

		// When to we attack next?
		m_nextAttack = Now() + m_rateOfAttack;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Is this event done?
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireMelee::Done()
{
	return (m_timesToAttack <= 0);
}

/////////////////////////////////////////////////////////////////////////////
// Could this weapon even hit?
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireMelee::CouldHit(
CCompActor &shooter,
AIWeaponTarget &target)
{
	const float rangeMultiplier = -2.5f;
	return InFrontAndInRange(m_weapon, shooter, target, m_deviance, rangeMultiplier);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AIWeaponFireLobbed::AIWeaponFireLobbed(
Lobbed &forWeapon) :
AIWeaponFire(forWeapon),
m_weapon(forWeapon)
{
	m_throw = 0;
	m_thrown = 0;
	m_deviance = 0.0f;
	m_throwRate = 0.0f;
	m_nextThrow = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AIWeaponFireLobbed::~AIWeaponFireLobbed()
{
}

/////////////////////////////////////////////////////////////////////////////
// Request a fire
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireLobbed::Fire(
int times,
float deviance,
float rateOfAttack)
{
	ASSERT(times > 0);

	// Abort!
	AbortFire();

	// Set it up
	m_throw = times;
	m_thrown = 0;
	m_deviance = deviance;
	m_throwRate = !Math::Zero(rateOfAttack) ? rateOfAttack : m_weapon.RateOfAttack() * 0.5f;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// How many shots have been fired in this salvo?
/////////////////////////////////////////////////////////////////////////////
int AIWeaponFireLobbed::ShotsFired()
{
	return m_thrown;
}

/////////////////////////////////////////////////////////////////////////////
// Abort firing (returns true if actually was firing)
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireLobbed::AbortFire()
{
	bool wasFiring = (m_throw > 0);
	m_throw = 0;
	return wasFiring;
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame
/////////////////////////////////////////////////////////////////////////////
void AIWeaponFireLobbed::Update(
CCompActor &shooter,
AIWeaponTarget &target)
{
	// Duh
	if (Done())
		return;

	if (m_throw > 0 && Now() > m_nextThrow)
	{
		if (CouldHit(shooter, target))
		{
			m_weapon.SetAim(target.PositionInWorld(), Math::Deg2Rad(m_deviance));

			float strength;
			Vector3 launchDir;
			if (ComputeThrow(shooter, target, launchDir, strength) && m_weapon.AIFire(launchDir, strength))
			{
				--m_throw;
				++m_thrown;
			}
		}

		// When to we attack next?
		m_nextThrow = Now() + m_throwRate;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Is this event done?
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireLobbed::Done()
{
	return (m_throw <= 0);
}

/////////////////////////////////////////////////////////////////////////////
// Could this weapon even hit?
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireLobbed::CouldHit(
CCompActor &shooter,
AIWeaponTarget &target)
{
	return InFrontAndInRange(m_weapon, shooter, target, m_deviance);
}

/////////////////////////////////////////////////////////////////////////////
// Compute the strength
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireLobbed::ComputeThrow(
CCompActor &shooter,
AIWeaponTarget &target,
Vector3 &launchDir,
float &strength)
{
	// Where is the launch coming from?
	Vector3 launchPos = m_weapon.AIGetLaunchPos();

	// Figure out a launch angle
	float launchAngle = OptimizeLaunchAngle(launchPos, target.PositionInWorldSimple());

	// Defaults
	float launchV = 0.0f;
	float leadTime = 0.0f;
	int homeIn = 1;
#ifdef LOBBED_LEAD_PREDICT
	homeIn = Math::Zero(target.VelocityInWorld().LengthSquared()) ? 1 : 3;
#endif
	// Lead predict
	while (homeIn > 0)
	{
		// Where is the target?
		Vector3 targetPos = target.PositionInWorldSimple(leadTime);

		// Figure out the 2d quantities
		Vector3 d = targetPos - launchPos;
		float yt = d.Y();
		float xt = Math::Sqrt(Math::Square(d.X()) + Math::Square(d.Z()));

		// Make a launch dir
		if (!MakeLaunchDir(d, launchAngle, launchDir))
			return false;

		// what should the launch velocity be?
		launchV = xt / (Math::Cos(launchAngle) * Math::Sqrt(2.0f / 9.81f * (xt * Math::Tan(launchAngle) - yt)));

		// Refine based on time of flight
		leadTime = xt / (launchV * Math::Cos(launchAngle));

#if 0
		TRACE("%x Computed launch angle, speed, time [%0.2f, %0.2f, %0.2f]\n", this, Math::Rad2Deg(launchAngle), launchV, leadTime);
#endif
		--homeIn;
	}

	// Convert strength
	float maxSpeed = m_weapon.AIGetMaxLaunchSpeed();
	strength = Math::Clamp(launchV, 0.0f, maxSpeed) / maxSpeed;

	// Now for the kicker, apply deviance
#if 0
	if (!Math::Zero(m_deviance))
		launchDir = Random::InCone(launchDir, Math::Deg2Rad(m_deviance));
#else
	strength -= m_deviance;
#endif

#if 0
	TRACE("%x launching angle, speed, time, strength [%0.2f, %0.2f, %0.2f, %0.2f]\n", this, Math::Rad2Deg(launchAngle), launchV, leadTime, strength);
#endif

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AIWeaponFireLobbed::OptimizeLaunchAngle(
const Vector3 &shooter,
const Vector3 &target)
{
	// What is the difference between targers?
	Vector3 d = target - shooter;

	// If there is a slight height difference, use a better angle
	if (d.Y() < -3.0f)
		return Math::Deg2Rad(15.0f);
	else if (d.Y() > 3.0f)
		return Math::Deg2Rad(60.0f);

	// Default to 45
	return Math::Deg2Rad(30.0f);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireLobbed::MakeLaunchDir(
const Vector3 &toTarget,
float theta,
Vector3 &launchDir)
{
	launchDir = toTarget;
	launchDir.y(0.0f);
	float length = launchDir.Length();
	if (Math::Zero(length))
		return false;

	launchDir /= length;
	Vector3 right = Vector3::Cross(launchDir, Vector3(0.0f, 1.0f, 0.0f));
	launchDir = RotatePointAroundAxis(launchDir, right, theta);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AIWeaponFireFirearm::AIWeaponFireFirearm(
Firearm &forWeapon) :
AIWeaponFire(forWeapon),
m_weapon(forWeapon)
{
	m_auto = (m_weapon.GetFiringMode() == Firearm::AUTOMATIC);
	TTLWeaponProperties &props = (TTLWeaponProperties &)m_weapon.GetProperties();
	m_subsonic = (props.m_weaponType == WEAPONTYPE_LAUNCHER);
	m_triggerDown = false;
	m_roundsWhenDone = m_roundsAtStart = m_weapon.TotalRounds();
	m_deviance = 0.0f;
	m_rateOfFire = 0.0f;
	m_nextFire = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AIWeaponFireFirearm::~AIWeaponFireFirearm()
{
}

/////////////////////////////////////////////////////////////////////////////
// Request a fire
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireFirearm::Fire(
int times,
float deviance,
float rateOfAttack)
{
	ASSERT(times > 0);

	// Abort!
	AbortFire();

	// Set it up
	m_roundsWhenDone = m_weapon.TotalRoundsFired() + times;
	m_roundsAtStart = m_weapon.TotalRoundsFired();
	m_deviance = deviance;
	m_rateOfFire = !Math::Zero(rateOfAttack) ? rateOfAttack : m_weapon.RateOfAttack() * 0.5f;
	m_triggerDown = false;

	// Apply the deviance
	//m_weapon.GetAimingSimulation().SetAIMinCone(deviance);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// How many shots have been fired in this salvo?
/////////////////////////////////////////////////////////////////////////////
int AIWeaponFireFirearm::ShotsFired()
{
	return (m_weapon.TotalRoundsFired() - m_roundsAtStart);
}

/////////////////////////////////////////////////////////////////////////////
// Abort firing (returns true if actually was firing)
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireFirearm::AbortFire()
{
	bool wasFiring = (m_weapon.TotalRoundsFired() < m_roundsWhenDone);
	m_roundsWhenDone = m_weapon.TotalRoundsFired();
	ReleaseTrigger();
	return wasFiring;
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame
/////////////////////////////////////////////////////////////////////////////
void AIWeaponFireFirearm::Update(
CCompActor &shooter,
AIWeaponTarget &target)
{
	// special case, the gun is empty
	if (m_weapon.RoundsLoaded() == 0)  //edit from TotalRounds(), by brian (????)
		AbortFire();

	// Duh
	if (Done())
		ReleaseTrigger();
	else if (Now() > m_nextFire)
	{
		float d;
		if (CouldHit(shooter, target, d))
		{
			if (!m_subsonic)
				m_weapon.SetAim(target.PositionInWorld(), Math::Deg2Rad(m_deviance));
			else
				m_weapon.SetAim(ComputeSubsonicAim(target.PositionInWorld(), d), Math::Deg2Rad(m_deviance));

			ASSERT(m_weapon.GetAimSim().GetAiming());
			PullTrigger();
		}

		// When to we attack next?
		m_nextFire = Now() + m_rateOfFire;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Is this event done?
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireFirearm::Done()
{
	return (m_roundsWhenDone <= m_weapon.TotalRoundsFired());
}

/////////////////////////////////////////////////////////////////////////////
// Could this weapon even hit?
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireFirearm::CouldHit(
CCompActor &shooter,
AIWeaponTarget &target,
float &d)
{
	if (!m_subsonic)
		return InFrontAndInRange(m_weapon, shooter, target, m_deviance);
	else
	{
		// Form a vector to the target
		const Vector3 shooterPos = shooter.GetFocusInWorld() + Vector3(0.0f, -0.2f, 0.0f);
		Vector3 p = target.PositionInWorld() - shooterPos;
		d = p.Length();
		const float safeRange = 3.0f;
		if (d < safeRange)
			return false;

		// Setup a chord check
		const Vector3 dir = p / d;
		const Vector3 end = shooterPos + dir * (d - 2.5f);

		SeeThru::LookResult result;
		bool hit = SeeThru::Look(shooterPos, end, &shooter, result);
		return !hit;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 AIWeaponFireFirearm::ComputeSubsonicAim(
const Vector3 &targetPosition,
float d)
{
	// We gotta aim over them
	Vector3 aimAt = targetPosition;
	const float maxDistance = 100.0f;
	const float maxUp = 1.75f;
	const float yBoost = Math::Clamp(d, 0.0f, maxDistance) / maxDistance * maxUp;
	aimAt.Y(aimAt.Y() + yBoost);
	return aimAt;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireFirearm::PullTrigger()
{
	if (!m_auto && m_triggerDown)
	{
		m_weapon.ReleaseTrigger();
		m_triggerDown = false;
	}

	if (!m_triggerDown)
		m_triggerDown =  m_weapon.PullTrigger();
	return m_triggerDown;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponFireFirearm::ReleaseTrigger()
{
	if (!m_triggerDown)
		return false;

	m_weapon.ReleaseTrigger();
	m_triggerDown = false;
	return true;
}







