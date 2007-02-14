/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a supersonic projectile
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "EngineHelper/sfx.h"
#include "Effects/sfx/sfxlight.h"
#include "Effects/sfx/sfxdistort.h"
#include "Environ/Munition.h"
#include "CollideHelper/SightEntity.h"
#include "weapons/aoeproj.h"

// Create this
AreaOfEffectProjectileCreator g_areaOfEffectCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
AreaOfEffectProjectileCreator::AreaOfEffectProjectileCreator() :
ProjectileCreator("AreaOfEffect")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Projectile *AreaOfEffectProjectileCreator::CreateInstance(
const VarObjProperties<Projectile> &properties)
{
	return new AreaOfEffectProjectile((const AreaOfEffectProjectileProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties *AreaOfEffectProjectileCreator::NewProperties(
int index)
{
	return new AreaOfEffectProjectileProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AreaOfEffectProjectileProperties::AreaOfEffectProjectileProperties(
ProjectileCreator &creator,
int index) :
TTLProjectileProperties(creator, index)
{
	m_damageRadius = 0.0f;
	m_fullDamageRadius = 0.0f;
	m_damage = 0.0f;
	m_scanHit = false;

	m_deafnessDuration = 0.0f;

	m_material[0] = '\0';

	m_explodeFXName[0] = '\0';
	m_explodeFX = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AreaOfEffectProjectileProperties::~AreaOfEffectProjectileProperties()
{
	delete m_explodeFX;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void AreaOfEffectProjectileProperties::RegisterContext(
const char *context)
{
	// Call the parent
	TTLProjectileProperties::RegisterContext(context);

	g_console.CreateVar(context, "damageRadius", m_damageRadius);
	g_console.CreateVar(context, "fullDamageRadius", m_fullDamageRadius);
	g_console.CreateVar(context, "scanHit", m_scanHit);

	g_console.CreateVar(context, "deafnessDuration", m_deafnessDuration);

	g_console.CreateVar(context, "material", m_material, sizeof(m_material) - 1);

	g_console.CreateVar(context, "explodeFX", m_explodeFXName, sizeof(m_explodeFXName) - 1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AreaOfEffectProjectileProperties::PostLoadSetup()
{
	TTLProjectileProperties::PostLoadSetup();

	// Wee!
	if (m_explodeFXName[0])
	{
		m_explodeFX = g_explodeFXLibrary.CreateInstance(m_explodeFXName);
		ASSERT(m_explodeFX);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AreaOfEffectProjectile::AreaOfEffectProjectile(
const AreaOfEffectProjectileProperties &properties) :
TTLProjectile(properties),
m_aoeProps(properties)
{
	m_generator = (ActorHandle)INVALID_OBJECT_HANDLE;
}

/////////////////////////////////////////////////////////////////////////////
// Fire!
/////////////////////////////////////////////////////////////////////////////
bool AreaOfEffectProjectile::Fire(
Weapon *weapon,
const Vector3 &origin,
const Vector3 &dir)
{
	// Start the fx
	if (m_aoeProps.m_explodeFX)
	{
		ActorHandle sender = weapon ? CActor::ToHandle(weapon->GetOwner()) : (ActorHandle)INVALID_OBJECT_HANDLE;
		m_aoeProps.m_explodeFX->Trigger(sender, m_properties.GetIndex(), origin, dir);
	}

	// Tell the world
	ImpactWorld(weapon, origin, dir);

	// Hit the actors
	if (m_aoeProps.m_scanHit)
		return ComplexHit(weapon, origin, dir);
	else
		return SimpleHit(weapon, origin, dir);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AreaOfEffectProjectile::SimpleHit(
Weapon *weapon,
const Vector3 &pos,
const Vector3 &dir)
{
	// Make the test sphere
	CSphereEntity sphere;
	sphere.Sphere().Set(pos, m_aoeProps.m_damageRadius);
	sphere.SetPositiveInclude(ACTOR_CATEGORY);
	sphere.SetPositiveExclude(NO_WEAPON_HIT_CATEGORY);
	sphere.SetIgnoreEntity(CActor::FromHandle(m_generator));

	// Scan the entire cast
	g_collisionSystem.ResetStack();
	CCollisionList *list = g_collisionSystem.Test(&sphere);
	if (!list)
		return false;

	// Lock the stack
	CCollisionStackLock lock;
	lock.Set(g_collisionSystem.GetStack());

	// Walk the events
	bool hit = false;
	CCollisionEvent *event = list->GetHeadEvent();
	while (event)
	{
		// We a target!
		CStageEntity *target = event->GetTargetEntity();

		// Can we damage this mofo?
		if (target && target->IsA(CActor::ClassType()))
		{
			CActor *victim = (CActor *)target;
			u8 material = victim->Material(event->GetHeadPair());
			SendHit(weapon, victim, pos, material);
			hit = true;
		}

		// next!
		event = event->GetNext();
	}

	// Unlock the stack
	lock.Release();
	return hit;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AreaOfEffectProjectile::ComplexHit(
Weapon *weapon,
const Vector3 &pos,
const Vector3 &dir)
{
	// Make the test sphere
	CSphereEntity sphere;
	sphere.Sphere().Set(pos, m_aoeProps.m_damageRadius * 1.1f);
	sphere.SetPositiveInclude(ACTOR_CATEGORY);
	sphere.SetPositiveExclude(NO_WEAPON_HIT_CATEGORY);
	sphere.SetIgnoreEntity(CActor::FromHandle(m_generator));

#if defined(FOOD) && defined(_DEBUG)
	DrawUtility::QueueSphere(pos, DrawUtility::RED, m_aoeProps.m_damageRadius * 1.1f);
	DrawUtility::QueueSphere(pos, DrawUtility::YELLOW, m_aoeProps.m_fullDamageRadius);
#endif

	// Scan the entire cast
	g_collisionSystem.ResetStack();
	CCollisionList *list = g_collisionSystem.Test(&sphere);
	if (!list)
		return false;

	// Lock the stack
	CCollisionStackLock lock;
	lock.Set(g_collisionSystem.GetStack());

	// Walk the events
	int hitActors = 0;
	CCollisionEvent *event = list->GetHeadEvent();
	while (event)
	{
		// Wee a target!
		CStageEntity *target = event->GetTargetEntity();
		if (target && target->IsA(CActor::ClassType()))
		{
			CActor *victim = (CActor *)target;
			int material = victim->Material(event->GetHeadPair());

			// Can we damage this mofo?
			if (victim->IsA(CCombatant::ClassType()))
				TestCombatant(weapon, pos, victim, material);
			else
				SendHit(weapon, victim, pos, material);
			++hitActors;
		}

		// next!
		event = event->GetNext();
	}

	// Unlock the stack
	lock.Release();

	// Did we get anyone?
	return hitActors > 0;
}

/////////////////////////////////////////////////////////////////////////////
// Send the hit
/////////////////////////////////////////////////////////////////////////////
void AreaOfEffectProjectile::SendHit(
Weapon *weapon,
CActor *victim,
const Vector3 &pos,
u8 material,
const Vector3 *intersection,
float factor)
{
	// This shouldn't happen
	if (!victim)
		return;

  	CMunitionImpactSurface impact;
	impact.d_weaponID = weapon ? weapon->GetID() : -1;
	impact.d_firstShot = true;
	impact.d_deflected = false;
	impact.d_munitionID = m_properties.GetIndex();
	impact.d_victim = victim;
	impact.d_shotOrigin = pos;
	float distance;
	Vector3 dir;
	if (!intersection)
	{
		impact.d_intersection = victim->GetBodyInWorld() + Vector3(0.0f, 0.25f, 0.0f);
		distance = 0.0f;
	}
	else
	{
		impact.d_intersection = *intersection;
		dir = (impact.d_intersection - pos);
		distance = dir.Length();
	}
	if (!Math::Zero(distance))
		impact.d_surfaceNormal = dir / distance;
	else
		impact.d_surfaceNormal.Set(0.0f, 1.0f, 0.0f);
	impact.d_surfaceMaterial = material;
	float damageFactor = ComputeDamageFactor(distance);
	impact.d_damage = factor * damageFactor *  m_aoeProps.m_damage;
	impact.d_energy = 1000.0f;

	g_ttlMunition.SendMunitionImpact(m_owner, impact);

	// Doh!
#if defined(FOOD) && !defined(CDROM)
	//TRACE("Grenade impact (d = %.1f) damage: %.1f\n", distance, impact.d_damage);
#endif

	// Handle deafness
	float deafness = damageFactor * m_aoeProps.m_deafnessDuration;
	if (!Math::Zero(deafness))
		g_messageDispatcher.SendMessageToActor("MunitionDeaf", CMessageDispatcher::FloatToData(deafness), m_owner, victim->GetHandle());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AreaOfEffectProjectile::ComputeDamageFactor(
float distance)
{
	if (distance > m_aoeProps.m_damageRadius)
		return 0.0f;
	else if (distance < m_aoeProps.m_fullDamageRadius)
		return 1.0f;
	else
	{
		float factor = 1.0f - ((distance - m_aoeProps.m_fullDamageRadius) / (m_aoeProps.m_damageRadius - m_aoeProps.m_fullDamageRadius));
		return factor;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Tell the world
/////////////////////////////////////////////////////////////////////////////
bool AreaOfEffectProjectile::ImpactWorld(
Weapon *weapon,
const Vector3 &pos,
const Vector3 &normal)
{
	// Setup a test
	CLineEntity test;
	test.Line().SetOriginBasis(pos, -normal);
	test.Line().SetMaxBoundary(1.0f);
	test.Line().SetMinBoundary(-0.1f);
	test.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	test.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	test.SetPositiveInclude(WORLD_CATEGORY);

	// Test!
	CLineQuery query;
	if (!query.Test(test))
		return false;

	// Woo! hit the world!
  	CMunitionImpactSurface impact;
	impact.d_weaponID = weapon ? weapon->GetID() : -1;
	impact.d_firstShot = true;
	impact.d_deflected = false;
	impact.d_munitionID = m_properties.GetIndex();
	if ((pos - query.intersection).LengthSquared() < 0.001f)
		impact.d_shotOrigin = pos - normal * 0.25f;
	else
		impact.d_shotOrigin = pos;
	impact.SetQuery(query);
	impact.d_damage = m_aoeProps.m_damage;
	impact.d_energy = 1000.0f;

	g_ttlMunition.SendMunitionImpact(m_owner, impact);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector3 AreaOfEffectProjectile::GetBoneInWorld(
CActor &forWho,
const char *boneName)
{
	ASSERT(boneName && *boneName);

	Vector3 pos;
	BoneID boneID;
	int ret = boneGetBoneID(forWho.Instance()->Body(), &boneID, (char *)boneName);
	if (!ret)
		pos = forWho.GetBodyInWorld();
	else
	{
		boneGetStartPosition(forWho.Instance()->Body(), boneID, &pos);
		pos = pos * forWho.Instance()->Matrix();
	}

	return pos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AreaOfEffectProjectile::TestCombatant(
Weapon *weapon,
const Vector3 &pos,
CActor *victim,
u8 material)
{
	// See if it can "see" the target
	struct TestBone
	{
		const char *boneName;
		float factor;
	};
	const TestBone testBone[] =
	{
		{ "spine", 1.0f },
		{ "head", 1.5f },
		{ "leftleg", 1.0f },
		{ "rightleg", 1.0f },
		{ "leftarm", 0.75f },
		{ "rightarm", 0.750f }
	};
	const int testBones = sizeof(testBone) / sizeof(TestBone);

	ASSERT(victim);
	if (!victim)
		return false;

	// Loop through the bones
	for (int t = 0; t < testBones; t++)
	{
		Vector3 bone = GetBoneInWorld(*victim, testBone[t].boneName);

#if defined(FOOD) && defined(_DEBUG)
		DrawUtility::QueueLine(pos, bone, DrawUtility::YELLOW);
#endif

		if (InView(pos, bone, victim))
		{
			SendHit(weapon, victim, pos, material, &bone, testBone[t].factor);
			break;
		}
	}

	return true;
}


























