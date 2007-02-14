/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a Melee projectile
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "EngineHelper/sfx.h"
#include "Environ/Munition.h"
#include "weapons/meleeproj.h"

// Create this
MeleeProjectileCreator g_MeleeCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
MeleeProjectileCreator::MeleeProjectileCreator() :
ProjectileCreator("melee")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Projectile *MeleeProjectileCreator::CreateInstance(
const VarObjProperties<Projectile> &properties)
{
	return new MeleeProjectile((const MeleeProjectileProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties *MeleeProjectileCreator::NewProperties(
int index)
{
	return new MeleeProjectileProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MeleeProjectileProperties::MeleeProjectileProperties(
ProjectileCreator &creator,
int index) :
TTLProjectileProperties(creator, index)
{
	m_useBone[0] = '\0';
	m_range = 0.0f;
	m_material[0] = '\0';
	m_stunDuration = 0.0f;
	m_tearGasStrength = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void MeleeProjectileProperties::RegisterContext(
const char *context)
{
	// Call the parent
	TTLProjectileProperties::RegisterContext(context);

	// Do our shiznizzy
	g_console.CreateVar(context, "useBone", m_useBone, sizeof(m_useBone) - 1);
	g_console.CreateVar(context, "range", m_range);
	g_console.CreateVar(context, "material", m_material, sizeof(m_material) - 1);
	m_landedHit.RegisterContext(context, "landedHit");
	g_console.CreateVar(context, "stunDuration", m_stunDuration);
	g_console.CreateVar(context, "tearGasStrength", m_tearGasStrength);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MeleeProjectile::MeleeProjectile(
const MeleeProjectileProperties &properties) :
TTLProjectile(properties),
m_mProps(properties)
{
	m_bone = INVALID_BONEID;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MeleeProjectile::~MeleeProjectile()
{
}

/////////////////////////////////////////////////////////////////////////////
// Fire!
/////////////////////////////////////////////////////////////////////////////
bool MeleeProjectile::Fire(
Weapon *weapon,
const Vector3 &origin,
const Vector3 &dir)
{
#ifdef SPINEMELEE
	// Resolve the bone
	if (m_bone == INVALID_BONEID && m_mProps.m_useBone[0])
		boneGetBoneID(m_owner->Instance()->Body(), &m_bone, (char *)m_mProps.m_useBone);

	// What is the check?
	Vector3 start, realDir;
	if (m_bone == INVALID_BONEID)
	{
		start = origin;
		realDir = m_owner->GetBodyToWorld().GetRow2();
	}
	else
	{
		// Compute the hard stuff
		Graphics4x4 boneToModel;
	 	boneGetCurrentBoneToModel(m_owner->Instance()->Body(), m_bone, &boneToModel);
		Graphics4x4 boneToWorld = boneToModel * m_owner->Instance()->Matrix();
		start = boneToWorld.GetRow3();
		realDir = -boneToWorld.GetRow1();
	}

	// Twizzeak
	const float moveBoneOut = 0.25f;
	start += realDir * moveBoneOut;
#endif

	// Setup the test object
	CLineEntity test;
#ifdef SPINEMELEE
	test.Line().SetOriginBasis(start, realDir);
#else
	const float backupD = 0.1f;
	Vector3 backUp = origin - dir * backupD;
	test.Line().SetOriginBasis(backUp, dir);
#endif
	test.Line().SetMaxBoundary(m_mProps.m_range);
	test.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	test.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	test.SetIgnoreEntity(CActor::FromHandle(m_owner));
	test.SetPositiveExclude(NO_WEAPON_HIT_CATEGORY);

#ifdef _DEBUG
	DrawUtility::Line(test.Line().ConstOrigin(), test.Line().ConstOrigin() + test.Line().ConstBasis() * m_mProps.m_range, DrawUtility::YELLOW);
#endif

	// Test!
	CLineQuery query;
	if (!query.Test(test, CStageEntity::USE_COMPLEX))
		return false;

#ifdef _DEBUG
	DrawUtility::Point(query.intersection, DrawUtility::CYAN);
#endif

	// Handle the intersection, always back up because the test could have started
	// inside the actor being hit (leading to a "directionless" impact)
	const Vector3 safeOrigin = test.Line().ConstOrigin() - test.Line().ConstBasis() * 0.1f;

	// Do the attack particle
	m_mProps.m_landedHit.Do(m_owner, query.intersection, &dir);

	// Hanlde the intersection
	HandleIntersection(weapon, safeOrigin, query);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MeleeProjectile::HandleIntersection(
Weapon *weapon,
const Vector3 &shotOrigin,
CLineQuery &test)
{
	CMunitionImpactSurface impact;

	impact.d_weaponID = weapon ? weapon->GetID() : -1;
	impact.d_firstShot = true;
	impact.d_deflected = false;
	impact.d_munitionID = m_properties.GetIndex();
	impact.d_shotOrigin = shotOrigin;
	impact.SetQuery(test);
	impact.d_energy = 1000.0f;
	impact.d_damage = m_properties.m_damage;

	// Wee!
	g_ttlMunition.SendMunitionImpact(m_owner, impact);

	if (test.actor)
	{
		if (!Math::Zero(m_mProps.m_stunDuration))
		{
			float duration = m_mProps.m_stunDuration;
			g_messageDispatcher.SendMessageToActor("MunitionStun", CMessageDispatcher::FloatToData(duration), m_owner, test.actor->GetHandle());
		}

		if (!Math::Zero(m_mProps.m_tearGasStrength))
		{
			float strength = m_mProps.m_tearGasStrength;
			g_messageDispatcher.SendMessageToActor("InTearGas", CMessageDispatcher::FloatToData(strength), m_owner, test.actor->GetHandle());
		}
	}
}

