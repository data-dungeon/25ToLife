/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a Melee weapon
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "Game/Managers/SwooshMgr.h"
#include "EngineHelper/timer.h"
#include "Effects/widget.h"
#include "Math/sfilter.h"
#include "Math/fastrand.h"
#include "Environ/Munition.h"
#include "weapons/melee.h"
#include "weapons/meleeproj.h"
#include "weapons/ttlweapstats.h"

// Create this
MeleeCreator g_meleeCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
MeleeCreator::MeleeCreator() :
WeaponCreator("melee")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Weapon *MeleeCreator::CreateInstance(
const VarObjProperties<Weapon> &properties)
{
	return new Melee((const MeleeProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
// Subclass this to provide new
/////////////////////////////////////////////////////////////////////////////
WeaponProperties *MeleeCreator::NewProperties(
int index)
{
	return new MeleeProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
// Weee
/////////////////////////////////////////////////////////////////////////////
MeleeProperties::MeleeProperties(
WeaponCreator &creator,
int index) :
TTLWeaponProperties(creator, index)
{
 	m_idleEmitter[0] = '\0';

	m_rateOfAttack = 0.0f;

	m_swoosh = false;
	m_swooshLength = 0.0f;
	m_swooshColor.Set(0.0f, 0.0f, 0.0f, 0.0f);
	m_swooshMaxAlpha = 0.0f;
	m_swooshLifetime = 0.0f;
	m_swooshAdditive = false;
	m_swooshPointMultiply = 1.0;
	m_swooshSharp = false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MeleeProperties::~MeleeProperties()
{
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void MeleeProperties::RegisterContext(
const char *context)
{
	// Call the parent
	TTLWeaponProperties::RegisterContext(context);

	g_console.CreateVar(context, "idleEmitter", m_idleEmitter, sizeof(m_idleEmitter) - 1);

	m_attack.RegisterContext(context, "attack");

	g_console.CreateVar(context, "rateOfAttack", m_rateOfAttack);

	g_console.CreateVar(context, "swoosh", m_swoosh);
	g_console.CreateVar(context, "swooshLength", m_swooshLength);
	g_console.CreateVar(context, "swooshColor", &m_swooshColor);
	g_console.CreateVar(context, "swooshMaxAlpha", m_swooshMaxAlpha);
	g_console.CreateVar(context, "swooshLifetime", m_swooshLifetime);
	g_console.CreateVar(context, "swooshPointMultiply", m_swooshPointMultiply);
	g_console.CreateVar(context, "swooshAdditive", m_swooshAdditive);
	g_console.CreateVar(context, "swooshSharp", m_swooshSharp);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Melee::Melee(
const MeleeProperties &properties) :
TTLWeapon(properties),
m_mProps(properties)
{
	// We require a projectile
	ASSERT(m_projectile);

	// No swoosh
	m_swoosh = INVALID_HANDLE;

	// Get ready to go
	Reset();
}

/////////////////////////////////////////////////////////////////////////////
// Clean up
/////////////////////////////////////////////////////////////////////////////
Melee::~Melee()
{
	DestroySwoosh();
}

/////////////////////////////////////////////////////////////////////////////
// Set the owner
/////////////////////////////////////////////////////////////////////////////
void Melee::SetOwner(
CActor *owner)
{
	// CAll the parent
	TTLWeapon::SetOwner(owner);

	// Init our handler
	if (owner)
	{
		m_lethalStartHandler.Initialize("MeleeStart", MESSAGE_HANDLE_IF_MASTER, owner->GetMessageOwner(), this);
		m_lethalEndHandler.Initialize("MeleeEnd", MESSAGE_HANDLE_IF_MASTER, owner->GetMessageOwner(), this);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Reset the state of this weapon
/////////////////////////////////////////////////////////////////////////////
void Melee::Reset()
{
	// Call the parent
	TTLWeapon::Reset();

	// Ready!
	m_state = READY;

	m_nextAttack = 0.0f;

	m_lethal = false;

	// Weee
	DeactivateSwoosh();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Melee::State Melee::GetState() const
{
	return m_state;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float Melee::MaxRange()
{
	ASSERT(m_projectile);
	if (!m_projectile)
		return 0.0f;

	const MeleeProjectileProperties &props = (const MeleeProjectileProperties &)m_projectile->GetProperties();
	return props.m_range;
}

/////////////////////////////////////////////////////////////////////////////
// Request a fire (returns true if it worked)
/////////////////////////////////////////////////////////////////////////////
bool Melee::PullTrigger()
{
	// Can we fire?
	if (m_state != READY)
		return false;

	// Wee!
	m_state = FIRING;

	// State
	m_lethal = false;

	// Setup the next time we can attack
	m_nextAttack = Now() + m_mProps.m_rateOfAttack;

	// We are go
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Melee::ReleaseTrigger()
{
	// Are we even firing?
	return (m_state == FIRING);
}

/////////////////////////////////////////////////////////////////////////////
// AI simplified fire
/////////////////////////////////////////////////////////////////////////////
bool Melee::AIFire()
{
	if (!PullTrigger())
		return false;
	g_messageDispatcher.SendMessageToActor("BeginAttack", NULL, m_owner, m_owner);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Request a reload
/////////////////////////////////////////////////////////////////////////////
bool Melee::Reload()
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Make gun not ready for a time
/////////////////////////////////////////////////////////////////////////////
bool Melee::Unready(
float time)
{
	m_state = NOTREADY;
	m_nextAttack = Now() + time;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Activate/deactivate this weapon
/////////////////////////////////////////////////////////////////////////////
void Melee::Activate()
{
	// Call the parent
	TTLWeapon::Activate();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Melee::Deactivate()
{
	// Call the parent
	TTLWeapon::Deactivate();

	// Make sure this is gone
	DeactivateSwoosh();
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame
/////////////////////////////////////////////////////////////////////////////
void Melee::Update()
{
	// Let the parent update
	TTLWeapon::Update();

	// Do!
	UpdateSwoosh();

	// Are we lethal?
	if (m_lethal)
	{
#ifdef SPINEMELEE
		bool hit = m_projectile->Fire(this, GetWeaponInWorld(0), m_aimSim.TrueAimDir());
#else
		bool hit = m_projectile->Fire(this, GetBoneInWorld(0, BONE_MUZZLE), GetBoneDir(0, BONE_MUZZLE));
#endif
		if (hit)
		{
			g_messageDispatcher.SendMessageToActor("UpdateSecondaryAnimation", 0, m_owner, m_owner);
			m_lethal = false;
		}
	}

	// Update to the ready state
	if (Now() >= m_nextAttack)
	{
		m_state = READY;
		m_lethal = false;
		DeactivateSwoosh();
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Melee::LethalStartHandler::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	// We we sure this is for us?
	ASSERT(i_sender == d_ref->m_owner);
	if (i_sender == d_ref->m_owner)
	{
		if (d_ref->m_state == FIRING)
		{
			d_ref->m_lethal = true;
			d_ref->ActivateSwoosh();

			// Weapon stats - avatar used weapon.
			// BET: Melee needs to be tracked here since it only gets into
			// MeleeProjectile::Fire if you actually hit something.
			TTLWeaponStats* stats = TTLWeaponStats::GetComponent(d_ref->m_owner);
			if (stats)
				stats->IncrementUsed();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Melee::LethalEndHandler::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	// We we sure this is for us?
	ASSERT(i_sender == d_ref->m_owner);
	if (i_sender == d_ref->m_owner)
	{
		if (d_ref->m_state == FIRING)
			d_ref->m_lethal = false;
		d_ref->DeactivateSwoosh();
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Melee::CreateSwoosh()
{
	// Do we want one?
	if (!m_mProps.m_swoosh)
		return false;

	// Make sure this is gone
	DestroySwoosh();

	// Setup everything
	ts_bRGBA color;
	color.R = (u8)Math::Clamp(m_mProps.m_swooshColor.X() * 255.0f, 0.0f, 255.0f);
	color.G = (u8)Math::Clamp(m_mProps.m_swooshColor.Y() * 255.0f, 0.0f, 255.0f);
	color.B = (u8)Math::Clamp(m_mProps.m_swooshColor.Z() * 255.0f, 0.0f, 255.0f);
	color.A = (u8)Math::Clamp(m_mProps.m_swooshColor.W() * 255.0f, 0.0f, 255.0f);

	// Make it
 	m_swoosh = SwooshMgr__CreateSwoosh(&m_swooshStart, &m_swooshEnd,
													color, m_mProps.m_swooshMaxAlpha,
													m_mProps.m_swooshLifetime,
													m_mProps.m_swooshAdditive,
													m_mProps.m_swooshPointMultiply);
	Swoosh__SetSharpTail(m_swoosh, m_mProps.m_swooshSharp);
	UpdateSwoosh();
	return(m_swoosh != INVALID_HANDLE);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Melee::DestroySwoosh()
{
	if (m_swoosh != INVALID_HANDLE)
	{
		Swoosh__Kill(m_swoosh);
		m_swoosh = INVALID_HANDLE;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Melee::ActivateSwoosh()
{
	// Do the attack stuff
	Vector3 normal = GetBoneDir(0, BONE_BREECH);
	m_mProps.m_attack.Do(m_owner, GetBoneInWorld(0, BONE_BREECH), &normal);

	// Create it!
	if (m_swoosh == INVALID_HANDLE)
		CreateSwoosh();

	if (m_swoosh != INVALID_HANDLE)
		Swoosh__Activate(m_swoosh);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Melee::DeactivateSwoosh()
{
	if (m_swoosh != INVALID_HANDLE)
		Swoosh__Deactivate(m_swoosh);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Melee::UpdateSwoosh()
{
	if (m_swoosh != INVALID_HANDLE)
	{
		// Update the points
		m_swooshStart = GetBoneInWorld(0, BONE_MUZZLE);
		m_swooshEnd = m_swooshStart + GetBoneDir(0, BONE_MUZZLE) * m_mProps.m_swooshLength;

#ifdef _DEBUG
		DrawUtility::Line(m_swooshStart, m_swooshEnd, DrawUtility::BLUE);
#endif
	}
}
