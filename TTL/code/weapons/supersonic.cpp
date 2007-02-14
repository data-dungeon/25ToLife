/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a supersonic projectile
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/supersonic.h"
#include "EngineHelper/sfx.h"
#include "Environ/Munition.h"
#include "weapons/penetrate.h"
#include "weapons/debugshot.h"
#include "avatar/pstat.h"
#include "weapons/ttlweapstats.h"

// Create this
SupersonicProjectileCreator g_supersonicCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
SupersonicProjectileCreator::SupersonicProjectileCreator() :
ProjectileCreator("supersonic")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Projectile *SupersonicProjectileCreator::CreateInstance(
const VarObjProperties<Projectile> &properties)
{
	return new SupersonicProjectile((const SupersonicProjectileProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties *SupersonicProjectileCreator::NewProperties(
int index)
{
	return new SupersonicProjectileProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SupersonicProjectileProperties::SupersonicProjectileProperties(
ProjectileCreator &creator,
int index) :
TTLProjectileProperties(creator, index)
{
	m_energy = 0.0f;
	m_maxRange = 0.0f;

	m_tracerName[0] = '\0';
	m_tracer = NULL;

	m_whizSound[0] = '\0';
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SupersonicProjectileProperties::~SupersonicProjectileProperties()
{
	delete m_tracer;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void SupersonicProjectileProperties::RegisterContext(
const char *context)
{
	// Call the parent
	TTLProjectileProperties::RegisterContext(context);

	// Do our shiznizzy
	g_console.CreateVar(context, "energy", m_energy, CONVAR_REAL);
	g_console.CreateVar(context, "maxRange", m_maxRange, CONVAR_REAL);

	g_console.CreateVar(context, "tracer", m_tracerName, sizeof(m_tracerName) - 1);

	g_console.CreateVar(context, "whiz", m_whizSound, sizeof(m_whizSound) - 1);
}

/////////////////////////////////////////////////////////////////////////////
// Setup the tracer info
/////////////////////////////////////////////////////////////////////////////
void SupersonicProjectileProperties::PostLoadSetup()
{
	TTLProjectileProperties::PostLoadSetup();

	if (m_tracerName[0])
		m_tracer = g_tracerSetLibrary.CreateInstance(m_tracerName);
}

/////////////////////////////////////////////////////////////////////////////
// Handle munition impacts
/////////////////////////////////////////////////////////////////////////////
void SupersonicProjectileProperties::MunitionImpactFX(
const CMunitionImpact &impact)
{
	// Run the tracer
	if (m_tracer)
		m_tracer->CreateChord(impact.d_shotOrigin, impact.d_intersection);

	// This is sorta temprorary
	if (m_whizSound[0])
	{
		// Get the ear
		Vector3 ear = g_cameraDirector.CurrentCamera().Eye();
		Vector3 dir = impact.d_intersection - impact.d_shotOrigin;

		// Are these on opposite sides of the camera
		float originDot = Vector3::Dot(impact.d_shotOrigin - ear, dir);
		float hitDot = Vector3::Dot(impact.d_intersection - ear, dir);
		if (!Math::SameSign(originDot, hitDot))
		{
			// How far is this bullet from the eye?
			Segment chord(impact.d_shotOrigin, impact.d_intersection);
			Vector3 closestPoint = chord.ClosestPoint(ear);
			float dSquared = (ear - closestPoint).LengthSquared();
			if (dSquared < Math::Square(3.0f))
				g_ttlMunition.PlaySFX(m_whizSound, closestPoint, 1.0f, 0.0f, 1.0f);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SupersonicProjectile::SupersonicProjectile(
const SupersonicProjectileProperties &properties) :
TTLProjectile(properties),
m_ssProps(properties)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SupersonicProjectile::~SupersonicProjectile()
{
}

/////////////////////////////////////////////////////////////////////////////
// Fire!
/////////////////////////////////////////////////////////////////////////////
bool SupersonicProjectile::Fire(
Weapon *weapon,
const Vector3 &origin,
const Vector3 &dir)
{
	ASSERT(m_properties.m_spawnCount > 0 && m_properties.m_spawnCount <= HitSalvo::MaxHits());

	// Our hit data
	HitSalvo salvo;

	// Set the weapon
	salvo.SetWeapon(weapon);

	// Detect a hit
	bool gotHit = false;
	float d = Range();
	if (m_properties.m_spawnCount == 1)
		gotHit = RayCast(salvo, origin, dir, m_ssProps.m_energy, d);
	else
	{
		// Generate a perpendicular vector
		const Vector3 perp = MakeOrthogonal(dir);

		// How many are we going to spawn?
		int spawn = Math::Min(m_properties.m_spawnCount, HitSalvo::MaxHits());

		// We are going to rotate around
		float rotation = 0.0f;
		const float perShot = Math::TwoPi / (float)(spawn - 1);

		// Figure out some contraints for rnage
		const float minAngle = m_properties.m_spawnAngle * 0.6f;
		const float maxAngle = m_properties.m_spawnAngle;
		const float maxRot = perShot * 0.4f;
		const float minRot = -maxRot;

		// Spawn them!
		for (int s = 0; s < spawn; s++, rotation += perShot)
		{
			// Make a random vector
			Vector3 spray = dir;
			if (s != 0)
			{
				const float angle = g_random.InRange(minAngle, maxAngle);
				spray = RotatePointAroundAxis(spray, perp, angle);

				const float rotVariance = g_random.InRange(minRot, maxRot);
				spray = RotatePointAroundAxis(spray, dir, rotation + rotVariance);
			}

			// Detect a hit
			if (RayCast(salvo, origin, spray, m_ssProps.m_energy, d))
				gotHit = true;
		}
	}

	// Ignore random firing caused by the avatar's death.
	PlayerStats* pStats = (PlayerStats*)(CActor::FromHandle(m_owner))->GetInterface(PlayerStats::GetID());
	if (pStats && pStats->IsAlive())
	{
		// Weapon stats - avatar used weapon.
		TTLWeaponStats* stats = TTLWeaponStats::GetComponent(m_owner);
		if (stats)
			stats->IncrementUsed(m_properties.m_spawnCount);
	}

	// Let the hits go
	salvo.Send(m_owner);
	return gotHit;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SupersonicProjectile::SetupEntity(
CLineEntity &entity,
const Vector3 &origin,
const Vector3 &dir,
float d)
{
	entity.Line().SetOriginBasis(origin, dir);
	entity.Line().SetMaxBoundary(d);
	entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	entity.SetIgnoreEntity(CActor::FromHandle(m_owner));
	entity.SetPositiveExclude(NO_WEAPON_HIT_CATEGORY);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool SupersonicProjectile::RayCast(
HitSalvo &salvo,
Vector3 shotOrigin,
Vector3 shotDir,
float energy,
float d,
int level,
bool noDeflect)
{
	// Create the line
	CLineEntity entity;
	SetupEntity(entity, shotOrigin, shotDir, d);

	// About to do a test
	salvo.IncrementTest();

	// Do the test!
	g_collisionSystem.ResetStack();
	CLineQuery test;
	if (!test.Test(entity, CStageEntity::USE_COMPLEX))
	{
		if (salvo.Tests() == 1)
		{
			Vector3 origin = ShotOrigin(level, salvo, shotOrigin);
			Vector3 miss = ShotEnd(salvo, origin, shotDir, d);
			HandleNonIntersection(salvo, origin, miss, level == 0);
		}
		DebugShot::Line(entity.Line().ConstOrigin(), entity.Line().ConstOrigin() + entity.Line().ConstBasis() * 100.0f);
		return false;
	}

	// Debugging
	DebugShot::Line(entity.Line().ConstOrigin(), test.intersection);
	DebugShot::Hit(test.intersection);

	// What did we hit?
	HandleIntersection(salvo, ShotOrigin(level, salvo, shotOrigin), test, energy, level == 0);

	// This is seutp to be recursive
	if (!noDeflect && Deflect(test, shotOrigin, shotDir, energy, d, noDeflect))
		RayCast(salvo, shotOrigin, shotDir, energy, d, level + 1, noDeflect);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SupersonicProjectile::HandleIntersection(
HitSalvo &salvo,
const Vector3 &shotOrigin,
CLineQuery &test,
float energy,
bool deflected)
{
	ASSERT(!Math::Zero((shotOrigin - test.intersection).LengthSquared()));

	// always goes to the world
	CMunitionImpactSurface impact;

	impact.d_weaponID = salvo.GetWeapon() ? salvo.GetWeapon()->GetID() : -1;
	impact.d_firstShot = (salvo.Tests() == 1);
	impact.d_deflected = deflected;
	impact.d_munitionID = m_properties.GetIndex();
	impact.d_shotOrigin = shotOrigin;
	impact.SetQuery(test);
	impact.d_energy = energy;
	impact.d_damage = m_properties.m_damage * energy / m_ssProps.m_energy;

	salvo.Add(impact);

	DebugShot::Impact(impact);
	DebugShot::RegisterHit(impact.d_intersection, impact.Direction());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SupersonicProjectile::HandleNonIntersection(
HitSalvo &salvo,
const Vector3 &shotOrigin,
const Vector3 &shotEnd,
bool deflected)
{
	ASSERT(!Math::Zero((shotOrigin - shotEnd).LengthSquared()));

	// always goes to the world
	CMunitionImpactSurface impact;

	impact.d_weaponID = salvo.GetWeapon() ? salvo.GetWeapon()->GetID() : -1;
	impact.d_firstShot = (salvo.Tests() == 1);
	impact.d_deflected = deflected;
	impact.d_munitionID = m_properties.GetIndex();
	impact.d_shotOrigin = shotOrigin;
	impact.d_victim = NULL;
	impact.d_intersection = shotEnd;
	impact.d_surfaceNormal = (shotEnd - shotOrigin).Normalized();
	impact.d_surfaceMaterial = g_ttlMunition.NothingMaterial();
	impact.d_energy = m_ssProps.m_energy;
	impact.d_damage = m_properties.m_damage;

	salvo.Add(impact);
}

/////////////////////////////////////////////////////////////////////////////
// Handle deflections from material penetrations
/////////////////////////////////////////////////////////////////////////////
bool SupersonicProjectile::Deflect(
CLineQuery &test,
Vector3 &shotOrigin,
Vector3 &shotDir,
float &energy,
float &d,
bool &noDeflect)
{
	ASSERT(m_ssProps.m_surfaceType >= 0);
//	ASSERT(test.material >= 0);

	// What is needed to go through here?
	SPenetrationData *penetrate = g_penetrationTable.GetEntry(m_ssProps.m_surfaceType, test.material);
	if (!penetrate)
		return false;

	// Anything to do?
	if (penetrate->d_energy < 0.0f)
		return false;
	if (penetrate->d_percentBlocked == 100 || g_random.UpToExcludeHigh(100) <= penetrate->d_percentBlocked)
		return false;

	// What is the entergy?
	if (energy - penetrate->d_energy <= 0.0f)
		return false;

	// Hot damn, we are in!
	energy -= penetrate->d_energy;
	d -= test.d;
	d *= energy / m_ssProps.m_energy;

	// Is this a ricochet?
	if (penetrate->d_percentRicochet != 0 && g_random.UpToExcludeHigh(100) <= penetrate->d_percentRicochet)
	{
		shotOrigin = test.intersection + test.normal * 0.005f;
		shotDir = g_random.InCone(test.normal, Math::Deg2Rad(-penetrate->d_deflection));
		noDeflect = true;
	}
	else // Just a penetration
	{
		if (Math::Zero(penetrate->d_deflection))
			shotOrigin = test.intersection - test.normal * 0.01f;
		else if (penetrate->d_deflection > 0.0f)
		{
			shotOrigin = test.intersection - test.normal * 0.01f;
			shotDir = g_random.InCone(shotDir, Math::Deg2Rad(penetrate->d_deflection));
		}
	}

	// We don't handle this yet
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 SupersonicProjectile::ShotOrigin(
int level,
HitSalvo &salvo,
const Vector3 &shotOrigin)
{
#if 1
	return salvo.GetWeapon()->GetAimSim().AimOrigin();
#else
	// Is this a deflected projectile?
	if (level != 0 || !salvo.GetWeapon())
		return shotOrigin;
	else // Give the real muzzle
	{
		Vector3 muzzleEnd = salvo.GetWeapon()->GetAimSim().GetMuzzleEnd();
		Vector3 muzzleDir = salvo.GetWeapon()->GetAimSim().GetMuzzleDir();
		return (muzzleEnd - muzzleDir * 0.3f);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 SupersonicProjectile::ShotEnd(
HitSalvo &salvo,
const Vector3 &shotOrigin,
const Vector3 &shotDir,
float d)
{
	if (!salvo.GetWeapon())
		return shotOrigin + shotDir * d;
	else
		return salvo.GetWeapon()->GetAimSim().PointOnTrajectory(d);
}

