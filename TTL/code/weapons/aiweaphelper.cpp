/////////////////////////////////////////////////////////////////////////////
// AI Weapon helper class!
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/aiweaphelper.h"

/////////////////////////////////////////////////////////////////////////////
// We will work your inventory! (don't call things directly)
/////////////////////////////////////////////////////////////////////////////
AIWeaponHelper::AIWeaponHelper() :
m_fireEvent(false),
m_firePool(4, 4)
{
	// We know nothing yet
	m_inventory = NULL;
	m_shooter = NULL;

	m_target = NULL;
	m_targetType = AITARGET_NONE;

	m_fireGap = 0.0f;
	ClearGapTimer();
	m_fire = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AIWeaponHelper::~AIWeaponHelper()
{
	ClearTarget();
	ClearFireQueue();
	delete m_fire;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize stuff
// We will work your inventory! (don't call things directly)
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::Initialize(
WeaponInventory &inventory,
CCompActor &shooter)
{
	m_inventory = &inventory;
	m_shooter = &shooter;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int AIWeaponHelper::Slots()
{
	ASSERT(m_inventory);
	return m_inventory->Count();
}

/////////////////////////////////////////////////////////////////////////////
// Select/get a weapon
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::SelectWeapon(
int slot)
{
	ASSERT(m_inventory);
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	// Duh
	int selected = m_inventory->GetSelectedSlot();
	if ((slot == -1 && selected < 0) || slot == selected)
		return true;

	// Turn off firing!
	ClearFireQueue();
	delete m_fire;
	m_fire = NULL;
	ClearGapTimer();

	// Slect the new weapon
	if (slot == -1)
		return m_inventory->SelectNoWeapon();
	else
		return m_inventory->Select(slot);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int AIWeaponHelper::SelectedWeapon()
{
	ASSERT(m_inventory);
	return m_inventory->GetSelectedSlot();
}

/////////////////////////////////////////////////////////////////////////////
// Get info about the weapon. If slot = -1, use the current
/////////////////////////////////////////////////////////////////////////////
TTLWeapon *AIWeaponHelper::GetWeapon(
int slot)
{
	ASSERT(m_inventory);
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	if (slot == -1)
		return (TTLWeapon *)m_inventory->GetSelected();
	else
		return (TTLWeapon *)m_inventory->Get(slot);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AIWeaponHelper::GetRange(
int slot)
{
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	// Wee!
	TTLWeapon *weapon = GetWeapon(slot);
	return weapon ? weapon->MaxRange() : 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AIWeaponHelper::GetRateOfAttack(
int slot)
{
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	// Wee!
	TTLWeapon *weapon = GetWeapon(slot);
	return weapon ? weapon->RateOfAttack() : -1.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::HasAmmo(
int slot)
{
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	// Wee!
	TTLWeapon *weapon = GetWeapon(slot);
	return weapon ? weapon->HasAmmo() : false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AIWeaponHelper::AmmoRatio(
int slot)
{
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	// Wee!
	TTLWeapon *weapon = GetWeapon(slot);
	return weapon ? weapon->AmmoRatio() : 1.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::CheatForMoreAmmo(
int slot)
{
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

#if 0
	// Only works for firearms right now
	if (GetClass(slot) != WEAPONCLASS_FIREARM)
		return false;

	// Wee!
	Firearm *weapon = (Firearm *)GetWeapon(slot);
	weapon->AddAmmo(weapon->GetAmmoType(), weapon->ClipBased() ? weapon->ClipCapacity() : weapon->MagazineCapacity());
#else
	TTLWeapon *weapon = GetWeapon(slot);
	if (weapon)
		weapon->FullAmmo(weapon->GetAmmoType());
#endif
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponClass AIWeaponHelper::GetClass(
int slot)
{
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	// Get the weapon
	TTLWeapon *weapon = GetWeapon(slot);
	if (!weapon)
		return WEAPONCLASS_NONE;

	// give them back the class
	return weapon->GetWeaponClass();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponType AIWeaponHelper::GetType(
int slot)
{
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	// Get the weapon
	TTLWeapon *weapon = GetWeapon(slot);
	if (!weapon)
		return WEAPONTYPE_NONE;
	return weapon->GetWeaponType();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponSubType AIWeaponHelper::GetSubType(
int slot)
{
	ASSERT(slot == -1 || TTLWeapon::SlotInRange(slot));

	// Get the weapon
	TTLWeapon *weapon = GetWeapon(slot);
	if (!weapon)
		return WEAPONSUBTYPE_NONE;
	return weapon->GetWeaponSubType();
}

/////////////////////////////////////////////////////////////////////////////
// Select the "best" weapon (meaning it exists and has ammo)
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::SelectBest()
{
	ASSERT(m_inventory);
	if (!m_inventory)
		return;

	int current = SelectedWeapon();

	// Select the best weapon
	TTLWeapon::Slot tryWeapon[4] = { TTLWeapon::PRIMARY, TTLWeapon::SECONDARY, TTLWeapon::LOBBED, TTLWeapon::MELEE };
	int t;
	for (t = 0; t < 4; t++)
	{
		m_inventory->Select(tryWeapon[t]);
		Weapon *weapon = m_inventory->GetSelected();
		if (weapon)
			break;
	}

	if (t != current)
		SetAim(false);
}

/////////////////////////////////////////////////////////////////////////////
// Set the current target
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::SetTarget(
const Vector3 &target)
{
	if (m_targetType == AITARGET_POINT && m_target)
		((AIWeaponTargetPosition *)m_target)->Set(target);
	else
	{
		delete m_target;
		m_target = new AIWeaponTargetPosition(target);
		m_targetType = AITARGET_POINT;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::SetTarget(
CActor &actor)
{
	if (m_targetType == AITARGET_ACTOR && m_target)
		((AIWeaponTargetActor *)m_target)->Set(actor);
	else
	{
		delete m_target;
		m_target = new AIWeaponTargetActor(actor);
		m_targetType = AITARGET_ACTOR;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::SetTarget(
CActor &actor,
const Vector3 &offset)
{
	if (m_targetType == AITARGET_ACTOR && m_target)
		((AIWeaponTargetActor *)m_target)->Set(actor, offset);
	else
	{
		delete m_target;
		m_target = new AIWeaponTargetActor(actor, offset);
		m_targetType = AITARGET_ACTOR;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::ClearTarget()
{
	delete m_target;
	m_target = NULL;
	m_targetType = AITARGET_NONE;
	SetAim(false);
}

/////////////////////////////////////////////////////////////////////////////
// Could this weapon even hit based on range?
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::InRange()
{
	ASSERT(m_shooter);

	// Do we have a target?
	if (!m_target)
		return false;

	float dSquared = (m_shooter->GetBodyInWorld() - m_target->PositionInWorld()).LengthSquared();
	float weaponRange = GetRange();
	return (dSquared < Math::Square(weaponRange));
}

/////////////////////////////////////////////////////////////////////////////
// Setup a delay between firing
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::SetFireGap(
float gapTime)
{
	if (gapTime < 0.0f)
		return false;

	m_fireGap = gapTime;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Fire the selected weapon!
// Deviance if 0 will fire *exactly* at target (assuming viewIF is right)
//		whereas 1.0 will miss by a lot
// if rateOfAttack is zero it will use the rate of attack that is in the weapon
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::Fire(
int times,
float deviance,
float rateOfAttack)
{
	ASSERT(times > 0);
	if (times == 0)
		return false;

	FireEvent *event = NewFireEvent();
	if (!event)
		return false;

	// Set it up!
	event->times =  times;
	event->deviance = deviance;
	event->rateOfAttack = rateOfAttack;

	// Queue it!
	return QueueFireEvent(event);
}

/////////////////////////////////////////////////////////////////////////////
// How many shots have been fired in this salvo?
/////////////////////////////////////////////////////////////////////////////
int AIWeaponHelper::ShotsFired()
{
	return (m_fire ? m_fire->ShotsFired() : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Abort firing (returns true is actually was firing)
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::AbortFire()
{
	SetAim(false);
	bool ret = (m_fire ? m_fire->AbortFire() : false);
	SetGapTimer();
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame to manage firing, etc.
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::Update()
{
	// Update the target
	if (m_target)
		m_target->Update();

	// Do we need to reset the gap timer?
	if ((!m_fire || m_fire->Done()) && m_nextFireEvent < 0.0f)
	{
		SetAim(false);
		SetGapTimer();
	}

	// Is there anything to do here?
	if (m_nextFireEvent >= 0.0f && g_timer.GetEndSec() > m_nextFireEvent)
	{
		SetAim(true);
		FireEvent *event = NextFireEvent();
		if (event)
		{
			// Create the fire!
			CreateWeaponFire(*event);
			DeleteFireEvent(event);

			// Special case, stop processing until the current event
			// is done
			m_nextFireEvent = -1.0f;
		}
	}

	// Update the firing weapon
	if (m_fire && m_shooter && m_target)
		m_fire->Update(*m_shooter, *m_target);
}

/////////////////////////////////////////////////////////////////////////////
// Set Aiming
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::SetAim(bool aim)
{	
	if (m_inventory)
	{
		TTLWeapon *weapon = (TTLWeapon *)m_inventory->GetSelected();
		if (weapon)
			weapon->GetAimSim().SetAiming(aim);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Manage the queued fire events
/////////////////////////////////////////////////////////////////////////////
int AIWeaponHelper::FireQueued()
{
	return m_fireEvent.Items();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::DeleteFireEvent(
FireEvent *fireEvent)
{
	if (fireEvent)
	{
		m_fireEvent.Unlink(fireEvent);
		m_firePool.Delete(fireEvent);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::QueueFireEvent(
FireEvent *fireEvent)
{
	if (!fireEvent)
		return false;

	m_fireEvent.AddTail(fireEvent);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::ClearFireQueue()
{
	// Abort the current fire
	AbortFire();

	// Empty the queue
	FireEvent *event = m_fireEvent.Head();
	while (event)
	{
		FireEvent *next = event->next;
		m_fireEvent.Unlink(event);
		m_firePool.Delete(event);
		event = next;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AIWeaponHelper::CreateWeaponFire(
const FireEvent &fireEvent)
{
	// Create a new fire
	if (!m_fire)
	{
		switch (GetClass())
		{
			case WEAPONCLASS_MELEE:
				m_fire = new AIWeaponFireMelee((Melee &)*GetWeapon());
				ASSERT(m_fire);
				break;

			case WEAPONCLASS_FIREARM:
				m_fire = new AIWeaponFireFirearm((Firearm &)*GetWeapon());
				ASSERT(m_fire);
				break;

			case WEAPONCLASS_LOBBED:
				m_fire = new AIWeaponFireLobbed((Lobbed &)*GetWeapon());
				ASSERT(m_fire);
				break;
		}
	}

	// Did it work?
	if (!m_fire)
		return false;

	// Start it going!
	return m_fire->Fire(fireEvent.times, fireEvent.deviance, fireEvent.rateOfAttack);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::SetGapTimer()
{
	m_nextFireEvent = g_timer.GetEndSec() + m_fireGap;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AIWeaponHelper::ClearGapTimer()
{
	m_nextFireEvent = 0.0f;
}



