/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a trigger projectile
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Effects/sfx/sfxtrigger.h"
#include "Effects/sfx/sfxsys.h"
#include "weapons/triggerproj.h"

// Create this
TriggerProjectileCreator g_triggerCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
TriggerProjectileCreator::TriggerProjectileCreator() :
ProjectileCreator("triggeraoe")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Projectile *TriggerProjectileCreator::CreateInstance(
const VarObjProperties<Projectile> &properties)
{
	return new TriggerProjectile((const TriggerProjectileProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties *TriggerProjectileCreator::NewProperties(
int index)
{
	return new TriggerProjectileProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TriggerProjectileProperties::TriggerProjectileProperties(
ProjectileCreator &creator,
int index) :
AreaOfEffectProjectileProperties(creator, index)
{
	m_message[0] = '\0';
	m_size.Clear();
	m_duration = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void TriggerProjectileProperties::RegisterContext(
const char *context)
{
	// Call the parent
	AreaOfEffectProjectileProperties::RegisterContext(context);

	g_console.CreateVar(context, "message", m_message, sizeof(m_message) - 1);
	g_console.CreateVar(context, "size", &m_size);
	g_console.CreateVar(context, "duration",  m_duration);
}

/////////////////////////////////////////////////////////////////////////////
// Handle explosions!
/////////////////////////////////////////////////////////////////////////////
void TriggerProjectileProperties::HandleExplodeFX(
ActorHandle owner,
const SMunitionExplodeFX &fx)
{
	if (m_message[0])
	{
		SFXTriggerBox *trigger = SFXTriggerBox::Create(m_message, fx.d_pos, m_size, m_duration);
		ASSERT(trigger);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Reset due to a checkpoint
/////////////////////////////////////////////////////////////////////////////
void TriggerProjectileProperties::CheckpointReset() const
{
	TTLProjectileProperties::CheckpointReset();

	g_sfx.DeleteAllByClass("trigger");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TriggerProjectile::TriggerProjectile(
const TriggerProjectileProperties &properties) :
AreaOfEffectProjectile(properties),
m_tProps(properties)
{
}

/////////////////////////////////////////////////////////////////////////////
// Fire!
/////////////////////////////////////////////////////////////////////////////
bool TriggerProjectile::Fire(
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

	return true;
}













