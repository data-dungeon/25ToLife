/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a supersonic projectile
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Interfaces/ViewIF.h"
#include "weapons/seethru.h"
#include "weapons/flashproj.h"

// Create this
FlashBangProjectileCreator g_flashBangCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
FlashBangProjectileCreator::FlashBangProjectileCreator() :
ProjectileCreator("flashproj")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Projectile *FlashBangProjectileCreator::CreateInstance(
const VarObjProperties<Projectile> &properties)
{
	return new FlashBangProjectile((const FlashBangProjectileProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties *FlashBangProjectileCreator::NewProperties(
int index)
{
	return new FlashBangProjectileProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FlashBangProjectileProperties::FlashBangProjectileProperties(
ProjectileCreator &creator,
int index) :
AreaOfEffectProjectileProperties(creator, index)
{
	m_flashIntensity = 0.0f;
	m_fullBlind = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void FlashBangProjectileProperties::RegisterContext(
const char *context)
{
	// Call the parent
	AreaOfEffectProjectileProperties::RegisterContext(context);

	g_console.CreateVar(context, "flashIntensity", m_flashIntensity);
	g_console.CreateVar(context, "fullBlind",  m_fullBlind);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FlashBangProjectile::FlashBangProjectile(
const FlashBangProjectileProperties &properties) :
AreaOfEffectProjectile(properties),
m_flashProps(properties)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FlashBangProjectile::~FlashBangProjectile()
{
}

/////////////////////////////////////////////////////////////////////////////
// Send the hit
/////////////////////////////////////////////////////////////////////////////
void FlashBangProjectile::SendHit(
Weapon *weapon,
CActor *victim,
const Vector3 &pos,
u8 material,
const Vector3 *intersection,
float factor)
{
	// Let this pass through
	AreaOfEffectProjectile::SendHit(weapon, victim, pos, material, intersection, factor);

	// This shouldn't happen
	ASSERT(victim);
	if (!victim)
		return;

	// Is the thing in front  of them?
	Vector3 victimEye;
	Vector3 victimLookDir;
	ViewIF *viewIF = (ViewIF *)victim->GetInterface(ViewIF::GetID());
	if (viewIF)
	{
		viewIF->GetEyepoint(victimEye);
		viewIF->GetDirection(victimLookDir);
	}
	else
	{
		victimEye = victim->GetFocusInWorld(); //brian removed this -> GetBodyInWorld() + Vector3(0.0f, 2.0f, 0.0f);
		victimLookDir = victim->GetBodyToWorld().GetRow2();
	}

	// Do the mighty dot
	Vector3 toFlash = pos - victimEye;
	if (Vector3::Dot(toFlash, victimLookDir) < 0.0f)
		return;

	// Tweaky!
	float distance = toFlash.Length();
	Vector3 toFlashDir = toFlash / distance;
	Vector3 startScan = pos + toFlashDir * -0.3f;

	// Now do an intersection to the eye
	SeeThru::LookResult result;
	if (SeeThru::Look(startScan, victimEye, victim, result, true))
		 return;

	// figure out the falloff
	float falloff;
	if (distance < m_flashProps.m_fullBlind)
		falloff = 0.0f;
	else
		falloff = (distance - m_flashProps.m_fullBlind) / (m_flashProps.m_damageRadius - m_flashProps.m_fullBlind);

	// Ooops
	falloff = Math::Min(falloff, 0.9f);

	// They are blind!
	SMunitionFlash flash;
	flash.d_munitionID = m_flashProps.GetIndex();
	flash.d_pos = pos;
	flash.d_intensity = m_flashProps.m_flashIntensity * (1.0f - falloff);
	g_messageDispatcher.SendMessageToActor(MUNITION_FLASH, &flash, m_owner, victim->GetHandle());
}





















