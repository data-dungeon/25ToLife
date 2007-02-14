/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a firearm
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "EngineHelper/timer.h"
#include "Math/sfilter.h"
#include "Math/fastrand.h"
#include "avatar/avatarfx.h"
#include "weapons/ttlprojectile.h"
#include "weapons/mlsight.h"
#include "weapons/firearm.h"
#include "weapons/supersonic.h"

// Create this
FirearmCreator g_firearmCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
FirearmCreator::FirearmCreator() :
WeaponCreator("firearm")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Weapon *FirearmCreator::CreateInstance(
const VarObjProperties<Weapon> &properties)
{
	return new Firearm((const FirearmProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
// We have our own initialize and terminate
/////////////////////////////////////////////////////////////////////////////
void FirearmCreator::InitializeOnce()
{
	WeaponCreator::InitializeOnce();

	NewSingleton(MuzzleFlashLibrary);
	g_muzzleFlashLib.InitializeOnce();

	NewSingleton(EjectingBrassLibrary);
	g_ejectingBrassLib.InitializeOnce();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void FirearmCreator::TerminateOnce()
{
	g_ejectingBrassLib.TerminateOnce();
	DeleteSingleton(EjectingBrassLibrary);

	g_muzzleFlashLib.TerminateOnce();
	DeleteSingleton(MuzzleFlashLibrary);

	WeaponCreator::TerminateOnce();
}

/////////////////////////////////////////////////////////////////////////////
// We have our own initialize and terminate
/////////////////////////////////////////////////////////////////////////////
void FirearmCreator::InitializeEvery()
{
	WeaponCreator::InitializeEvery();
	g_muzzleFlashLib.InitializeEvery();
	g_ejectingBrassLib.InitializeEvery();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void FirearmCreator::TerminateEvery()
{
	g_ejectingBrassLib.TerminateEvery();
	g_muzzleFlashLib.TerminateEvery();
	WeaponCreator::TerminateEvery();
}

/////////////////////////////////////////////////////////////////////////////
// Subclass this to provide new
/////////////////////////////////////////////////////////////////////////////
WeaponProperties *FirearmCreator::NewProperties(
int index)
{
	return new FirearmProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
// Weee
/////////////////////////////////////////////////////////////////////////////
FirearmProperties::FirearmProperties(
WeaponCreator &creator,
int index) : TTLWeaponProperties(creator, index)
{
	m_displayProjectile[0] = '\0';

	m_fireSound[0] = '\0';
	m_firePitch = 0.0f;

	m_emptySound[0] = '\0';

	m_reloadStart[0] = '\0';
	m_reloadEach[0] = '\0';
	m_reloadEnd[0] = '\0';

	m_muzzleFlash[0] = '\0';
	m_ejectingBrass[0] = '\0';

	m_automatic = false;
	m_rateOfFire = 0.0f;
	m_burstCount = 0;
	m_burstRate = 0.0f;

	m_nextRoundSound[0] = '\0';
	m_nextRoundSoundDelay = 0.0f;

	m_clipBased = false;
	m_magazineSize = 0;
	m_totalRounds = 0;

	m_reloadTime = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FirearmProperties::~FirearmProperties()
{
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void FirearmProperties::RegisterContext(
const char *context)
{
	// Call the parent
	TTLWeaponProperties::RegisterContext(context);

	g_console.CreateVar(context, "displayProj", m_displayProjectile, sizeof(m_displayProjectile) - 1);

	g_console.CreateVar(context, "fireSound", m_fireSound, sizeof(m_fireSound) - 1);
	g_console.CreateVar(context, "firePitch", m_firePitch, CONVAR_REAL);

	g_console.CreateVar(context, "emptySound", m_emptySound, sizeof(m_emptySound) - 1);

	g_console.CreateVar(context, "reloadStartSound", m_reloadStart, sizeof(m_reloadStart) - 1);
	g_console.CreateVar(context, "reloadEachSound", m_reloadEach, sizeof(m_reloadEach) - 1);
	g_console.CreateVar(context, "reloadEndSound", m_reloadEnd, sizeof(m_reloadEnd) - 1);

	g_console.CreateVar(context, "muzzleFlash", m_muzzleFlash, sizeof(m_muzzleFlash) - 1);
	g_console.CreateVar(context, "ejectingBrass", m_ejectingBrass, sizeof(m_ejectingBrass) - 1);

	g_console.CreateVar(context, "automatic", m_automatic);
	g_console.CreateVar(context, "rateOfFire", m_rateOfFire, CONVAR_REAL);
	g_console.CreateVar(context, "burstCount", m_burstCount);
	g_console.CreateVar(context, "burstRate", m_burstRate, CONVAR_REAL);

	g_console.CreateVar(context, "nextRoundSound", m_nextRoundSound, sizeof(m_nextRoundSound) - 1);
	g_console.CreateVar(context, "nextRoundSoundDelay", m_nextRoundSoundDelay, CONVAR_REAL);

	g_console.CreateVar(context, "clipBased", m_clipBased);
	g_console.CreateVar(context, "magazineSize", m_magazineSize);
	g_console.CreateVar(context, "totalRounds", m_totalRounds);

	g_console.CreateVar(context, "reloadTime", m_reloadTime, CONVAR_REAL);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Firearm::Firearm(
const FirearmProperties &properties) :
TTLWeapon(properties),
m_faProps(properties)
{
	// We must have a projectile to work
	ASSERT(m_projectile);

	// Always auto reload by default
	SetAutoReload(true);

	// Create the ejecting brass
	if (!m_faProps.m_ejectingBrass[0])
		m_ejectingBrass = NULL;
	else
		m_ejectingBrass = g_ejectingBrassLib.CreateInstance(m_faProps.m_ejectingBrass);

	// Try create muzzle flashes
	for (int w = 0; w < WEAPON_MAX_MODELS; w++)
	{
		if (w < Models() && m_faProps.m_muzzleFlash[0])
			m_muzzleFlash[w] = g_muzzleFlashLib.CreateInstance(m_faProps.m_muzzleFlash);
		else
			m_muzzleFlash[w] = NULL;
	}

#ifdef DIRECTX_PC
	// set our muzzle flash bool
	m_showMuzzleFlash = true;
#endif

	// No fire sound
	m_fireHandle = NULL;

	// Set the initial ammo
	m_totalAmmo = 0;
	m_ammoInGun = 0;

	// Reload enabled
	m_reloadEnabled = true;

	// Get ready to go
	Reset();
}

/////////////////////////////////////////////////////////////////////////////
// Clean up
/////////////////////////////////////////////////////////////////////////////
Firearm::~Firearm()
{
	for (int i = 0; i < WEAPON_MAX_MODELS; i++)
		delete m_muzzleFlash[i];
	delete m_ejectingBrass;
}

/////////////////////////////////////////////////////////////////////////////
// Reset the state of this weapon
/////////////////////////////////////////////////////////////////////////////
void Firearm::Reset()
{
	// Call the parent
	TTLWeapon::Reset();

	// Reset total rounds fired
	m_totalRoundsFired = 0;

	// Clear the times
	m_readyAtTime = 0.0f;
	m_reloadUpdateTime = 0.0f;
	m_reloadStartTime = 0.0f;

	// Init the firing
	m_useModel = 0;
	m_burstCount = 0;
	m_nextRoundSoundDelay = 0.0f;
	m_nextShot = 0.0f;
	m_offHandDisabled = false;
	m_magazineSize = m_faProps.m_magazineSize;

	// Ready our state
	m_state = READY;

	if (m_aimSight)
		m_aimSight->Reset();

	// Setup the firing mode
	m_firingMode = m_faProps.m_automatic ? AUTOMATIC : SEMIAUTOMATIC;

	m_triggerDown = false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float Firearm::MaxRange()
{
	ASSERT(m_projectile);
	if (!m_projectile)
		return 0.0f;

	const SupersonicProjectileProperties &props = (SupersonicProjectileProperties &)m_projectile->GetProperties();
	return Math::Zero(props.m_maxRange) ? 100.0f : props.m_maxRange;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float Firearm::RateOfAttack()
{
	return m_faProps.m_rateOfFire;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifdef DIRECTX_PC
bool Firearm::SetAmmo(
const char *type,
int amount,
bool fillClip)
#else
bool Firearm::SetAmmo(
const char *type,
int amount)
#endif
{
	// Duh
	if (strcmpi(type, GetAmmoType()) != 0)
		return false;

	// Give it to them!
#ifdef DIRECTX_PC
	if (fillClip)
	{
		int inventoryRounds = m_faProps.m_totalRounds - m_magazineSize;
		if (amount > inventoryRounds)
			m_ammoInGun = amount - inventoryRounds;
	}
#endif
	m_totalAmmo = Math::Clamp(amount, 0, m_faProps.m_totalRounds - m_magazineSize);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Firearm::FullAmmo(const char *type)
{
	// Duh
	if (strcmpi(type, GetAmmoType()) != 0)
		return false;

	m_totalAmmo = m_faProps.m_totalRounds - m_magazineSize;
	m_ammoInGun = m_magazineSize;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Using strings, do the ammo
/////////////////////////////////////////////////////////////////////////////
int Firearm::CodifyAmmo()
{
	return m_totalAmmo + (m_ammoInGun << 16);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::ApplyAmmoCode(int ammoCode)
{
	if (ammoCode == -1)
	{
		m_totalAmmo = m_faProps.m_totalRounds - m_magazineSize;
		m_ammoInGun = m_magazineSize;
	}
	else
	{
		m_totalAmmo = Math::Clamp(ammoCode & 0xffff, 0, m_faProps.m_totalRounds - m_magazineSize);
		m_ammoInGun = Math::Clamp(ammoCode >> 16, 0, m_magazineSize);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::GetSplitAmmo(int& inBelt, int& inClip)
{
	inBelt = m_totalAmmo;
	inClip = m_ammoInGun;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::SetSplitAmmo(int inBelt, int inClip)
{
	if (inBelt == -1)
		m_totalAmmo = m_faProps.m_totalRounds - m_magazineSize;
	else
		m_totalAmmo = Math::Clamp(inBelt, 0, m_faProps.m_totalRounds - m_magazineSize);

	if (inClip == -1)
		m_ammoInGun = m_magazineSize;
	else
		m_ammoInGun = Math::Clamp(inClip, 0, m_magazineSize);
}

/////////////////////////////////////////////////////////////////////////////
// Does this gun autoreaload
/////////////////////////////////////////////////////////////////////////////
bool Firearm::SetAutoReload(
bool autoReload)
{
	m_autoReload = autoReload;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Firearm::SetFiringMode(
FiringMode newMode)
{
	// Can't put a non-auto gun into auto
	if (newMode == AUTOMATIC && !m_faProps.m_automatic)
		return false;

	// If we got here, it worked
	m_firingMode = newMode;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Firearm::FiringMode Firearm::GetFiringMode() const
{
	return m_firingMode;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Firearm::State Firearm::GetState() const
{
	return m_state;
}

/////////////////////////////////////////////////////////////////////////////
// Does this weapon use clip based ammo?
/////////////////////////////////////////////////////////////////////////////
bool Firearm::ClipBased() const
{
	return m_faProps.m_clipBased;
}

/////////////////////////////////////////////////////////////////////////////
// How many rounds are in a clip
/////////////////////////////////////////////////////////////////////////////
int Firearm::ClipCapacity() const
{
	if (ClipBased())
		return m_magazineSize;
	else
		return 1;
}

/////////////////////////////////////////////////////////////////////////////
// How many rounds fit in the magazine
/////////////////////////////////////////////////////////////////////////////
int Firearm::MagazineCapacity() const
{
	if (ClipBased())
		return 1;
	else
		return m_magazineSize;
}

/////////////////////////////////////////////////////////////////////////////
// How many rounds are left in the gun
/////////////////////////////////////////////////////////////////////////////
int Firearm::RoundsLoaded() const
{
	return m_ammoInGun;
}

/////////////////////////////////////////////////////////////////////////////
// What is the toal number of rounds left
/////////////////////////////////////////////////////////////////////////////
int Firearm::TotalRounds() const
{
	return m_totalAmmo;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int Firearm::TotalRoundsFired() const
{
	return m_totalRoundsFired;
}

/////////////////////////////////////////////////////////////////////////////
// Request a fire (returns true if it worked)
/////////////////////////////////////////////////////////////////////////////
bool Firearm::PullTrigger()
{
	// Can we abort the reload?
	if (GetState() == RELOADING)
		AbortReload();

	// Can we even fire?
	if (GetState() != READY)
		return false;

	// Can we fire yet?
	if (g_timer.GetEndSec() < m_nextShot)
		return false;

	// prepare tp fire
	if (!PrepFire())
		return false;

	// Ready to to the first shot
	m_nextShot = 0.0f;

	// We are firing!
	SetState(FIRING);
	m_triggerDown = true;
	//	UpdateFire();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Firearm::ReleaseTrigger()
{
	// Are we even firing?
	if (GetState() != FIRING)
		return false;

	// Trigger is no longer down
	m_triggerDown = false;

	// Are we still firing?
	if (m_burstCount > 0 || !Math::Zero(m_nextRoundSoundDelay))
		return false;

	// We are ready
	SetState(READY);

	// Force an update
	Update();

	// Done
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Firearm::DryFire(
const CMunitionFired &fire,
bool selected)
{
	// We don't do work if we are not selected
	if (!selected)
		return false;

	// Leave us alone
	CActor *actorOwner = CActor::FromHandle(m_owner);
	if (!actorOwner || actorOwner->IsNetMaster())
		return true;

	ASSERT(fire.d_weaponID == GetID());

	// Are we in front of the camera
	float dSquared;
	bool inFront = InFront(fire.d_shotOrigin, dSquared);

	// tell the projectile
	ASSERT(m_projectile);
	if (m_projectile)
		((TTLProjectile *)m_projectile)->DryFire(fire.d_shotOrigin, fire.Direction());

	// Do the flash
	const float lightD = 50.0f * 50.0f;
#ifdef DIRECTX_PC
	if (inFront && m_showMuzzleFlash)
#else
	if (inFront)
#endif
		CreateMuzzleFlash(m_useModel, dSquared < lightD);

	const float brassEject = 20.0f * 20.0f;
	if (inFront && dSquared < brassEject)
		EjectBrass(m_useModel);

	// Play the sound
	FireSound(m_faProps.m_fireSound, m_faProps.m_firePitch, m_useModel);

	// Update the model
	NextModel();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Request a reload
/////////////////////////////////////////////////////////////////////////////
bool Firearm::Reload()
{
	// Can we even reload?
	if (!ReloadEnabled() || !TotalRounds() || FullyLoaded() || GetState() == RELOADING)
		return false;

	// Play the sound
	PlaySound(m_faProps.m_reloadStart, MAGAZINE);

	// When we be done reloading the next round/clip
	m_reloadUpdateTime = g_timer.GetEndSec() + m_faProps.m_reloadTime;
	m_reloadStartTime = g_timer.GetEndSec();
	SetState(RELOADING);

	g_messageDispatcher.SendMessageToActor("Reload", NULL, m_owner, m_owner);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Cancel the current state (be it firing, reloading, etc)
/////////////////////////////////////////////////////////////////////////////
bool Firearm::Cancel()
{
	// Make sure we turn this off
	ReleaseTrigger();

	// If we are reloading, we now aren't
	if (m_state == RELOADING)
		m_state = READY;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float Firearm::ReloadedRatio() const
{
	if (GetState() != RELOADING)
		return -1.0f;

	float reloadTime;
	if (ClipBased())
		reloadTime = m_faProps.m_reloadTime;
	else
	{
		if (m_magazineSize == 1)
			reloadTime = m_faProps.m_reloadTime;
		else
			reloadTime = m_faProps.m_reloadTime * (m_magazineSize + 1);
	}

	float timeElapsed = g_timer.GetEndSec() - m_reloadStartTime;
	return Math::Clamp(timeElapsed / reloadTime);
}

/////////////////////////////////////////////////////////////////////////////
// Make gun not ready for a time
/////////////////////////////////////////////////////////////////////////////
bool Firearm::Unready(
float time)
{
	// Interrupt firing (if we can)
	if (GetState() == FIRING)
		ReleaseTrigger();

	// Do it
	m_readyAtTime = g_timer.GetEndSec() + time;
	Update();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Activate/deactivate this weapon
/////////////////////////////////////////////////////////////////////////////
void Firearm::Activate()
{
	// Call the parent
	TTLWeapon::Activate();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::Deactivate()
{
	// Make sure we turn this off
	ReleaseTrigger();

	// Call the parent
	TTLWeapon::Deactivate();

	// make sure we turn this off
	HideMuzzleFlash();

	// If we are reloading, we now aren't
	if (m_state == RELOADING)
		m_state = READY;
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame
/////////////////////////////////////////////////////////////////////////////
void Firearm::Update()
{
	// Let the parent update
	TTLWeapon::Update();

	// Handle firing
	if (GetState() == FIRING)
		UpdateFire();

	// Handle muzzle flash
#ifdef DIRECTX_PC
	if (m_showMuzzleFlash)
	{
		for (int i = 0; i < WEAPON_MAX_MODELS; i++)
	 		UpdateMuzzleFlash(i, g_timer.GetFrameSec());
	}
#else
		for (int i = 0; i < WEAPON_MAX_MODELS; i++)
	 		UpdateMuzzleFlash(i, g_timer.GetFrameSec());
#endif

	// Handle auto reloading
	if (GetState() == READY && !m_ammoInGun && m_autoReload && OkayToAutoReload())
		Reload();

	// What time is it!
	float time = g_timer.GetEndSec();

	// Are we reloading?
	if (GetState() == RELOADING && time >= m_reloadUpdateTime)
		UpdateReload();

	// Are we trying to make ourselves not ready?
	if (time < m_readyAtTime && GetState() == READY)
		SetState(NOTREADY);
	else if (time >= m_readyAtTime && GetState() == NOTREADY)
		SetState(READY);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Firearm::EnableOffHand(
bool enable)
{
	// Do we have an off hand?
	if (!HasOffHand())
		return false;

	// If it is the same, don't do any work
	if (m_offHandDisabled == !enable)
		return true;

	CActor *actorOwner = CActor::FromHandle(m_owner);
	if (!actorOwner)
		return false;

	// Switch it up
	m_offHandDisabled = !enable;
	if (m_offHandDisabled)
	{
		// Hide the left hand weapon
		m_useModel = 0;
		m_totalAmmo += (m_ammoInGun + 1) / 2;
		m_ammoInGun /= 2;
		m_magazineSize = m_faProps.m_magazineSize / 2;

		CInstance *inst = actorOwner->Instance(m_faProps.m_model[1].geometry);
		if (inst)
			inst->Hide(true);
	}
	else
	{
		// reenable the left hand weapon
		m_useModel = 1;
		m_magazineSize = m_faProps.m_magazineSize;

		int ammoInGun = m_ammoInGun;
		m_totalAmmo += m_ammoInGun;
		if (ammoInGun * 2 > m_totalAmmo)
			m_ammoInGun = m_totalAmmo;
		else
			m_ammoInGun = ammoInGun * 2;
		m_totalAmmo -= m_ammoInGun;

		CInstance *inst = actorOwner->Instance(m_faProps.m_model[1].geometry);
		if (inst)
			inst->Hide(false);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::SetState(
State newState)
{
	m_state = newState;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Firearm::FullyLoaded()
{
	return (m_ammoInGun == m_magazineSize);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Firearm::AbortReload()
{
	if (GetState() != RELOADING)
		return false;

	if (!ClipBased() && RoundsLoaded() > 0)
	{
		PlaySound(m_faProps.m_reloadEnd, MAGAZINE);
		SetState(READY);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::UpdateReload()
{
	// Uh oh, can't reload with no rounds left
	if (!TotalRounds() || FullyLoaded())
	{
		PlaySound(m_faProps.m_reloadEnd, MAGAZINE);
		SetState(READY);
		return;
	}

	// Handle a clip based gun
	if (ClipBased())
	{
		m_totalAmmo += m_ammoInGun;
		int loaded = min(m_magazineSize, m_totalAmmo);
		m_totalAmmo -= loaded;
		m_ammoInGun = loaded;

		// Done reloading
		PlaySound(m_faProps.m_reloadEnd, MAGAZINE);
		SetState(READY);
	}
	else
	{
		// Handle a magazine based gun one round at a time
		m_totalAmmo -= 1;
		m_ammoInGun += 1;
		PlaySound(m_faProps.m_reloadEach, MAGAZINE);
		if (m_magazineSize > 1)
 			m_reloadUpdateTime = g_timer.GetEndSec() + m_faProps.m_reloadTime;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Request a fire (returns true if it worked)
/////////////////////////////////////////////////////////////////////////////
bool Firearm::PrepFire()
{
	// No bullets in the gun?
	if (!RoundsLoaded())
	{
		PlaySound(m_faProps.m_emptySound, BREACH);
		m_readyAtTime = g_timer.GetEndSec() + m_faProps.m_rateOfFire / 3.0f;
		SetState(NOTREADY);
		return false;
	}

	// Are we bursting?
	m_burstCount = Math::Min(m_faProps.m_burstCount, m_ammoInGun);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::UpdateFire()
{
	// What time is it!
	float time = g_timer.GetEndSec();

	// Play a reload sound
	if (!Math::Zero(m_nextRoundSoundDelay) && time > m_nextRoundSoundDelay)
	{
		PlaySound(m_faProps.m_nextRoundSound, MAGAZINE);
		m_nextRoundSoundDelay = 0.0f;
	}

	// Can we fire yet?
	if (time < m_nextShot)
		return;

	// Done with the burst?
	if (m_burstCount == 0)
	{
		// Handle automatic
		if (m_firingMode == AUTOMATIC && m_triggerDown)
			PrepFire();
		if (m_burstCount == 0)
		{
			if (Math::Zero(m_nextRoundSoundDelay))
				SetState(READY);
			return;
		}
	}

	// Get the true aim
	Vector3 aimOrigin = m_aimSim.AimOrigin();
	Vector3 aimDir = m_aimSim.AimDir();

	// Fire a shot
	m_projectile->Fire(this, aimOrigin, aimDir);

	// Recoil
	m_aimSim.Recoil();

	// spawn effects
	bool sightLight = (m_aimSight && strcmp(m_aimSight->Class(), "maglite") == 0 && ((MagliteSight *)m_aimSight)->DynamicLightOn());

#ifdef DIRECTX_PC
	if(m_showMuzzleFlash) CreateMuzzleFlash(m_useModel, !sightLight);
#else
	CreateMuzzleFlash(m_useModel, !sightLight);
#endif

	EjectBrass(m_useModel);

	// Tell the world
	CActor *actorOwner = CActor::FromHandle(m_owner);
	if (actorOwner)
		g_avatarFX.NotifyWeaponFire(*actorOwner, m_faProps.VibrateDuration(), m_faProps.VibrateIntensity());

	// Play the sound
	FireSound(m_faProps.m_fireSound, m_faProps.m_firePitch, m_useModel);

	// Update the model
	NextModel();

	// Decrement ammo
	if (m_burstCount > 0)
		--m_burstCount;

	if (!Cheats::Enabled(Cheats::INFINITEAMMO, m_owner))
		--m_ammoInGun;
	++m_totalRoundsFired;

	// Setup for the next shot
	if (m_burstCount != 0)
		m_nextShot = time + m_faProps.m_burstRate;
	else
	{
		if (HasOffHand() && m_offHandDisabled)
			m_nextShot = time + m_faProps.m_rateOfFire * 2.0f;
		else
			m_nextShot = time + m_faProps.m_rateOfFire;
	}

	if (Math::Zero(m_faProps.m_nextRoundSoundDelay))
		m_nextRoundSoundDelay = 0.0f;
	else
		m_nextRoundSoundDelay = time + m_faProps.m_nextRoundSoundDelay;
}

/////////////////////////////////////////////////////////////////////////////
// Update the model
/////////////////////////////////////////////////////////////////////////////
void Firearm::NextModel()
{
	if (!m_offHandDisabled)
	{
		// Update the model
		if (++m_useModel >= Models())
			m_useModel = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::CreateMuzzleFlash(
int forModel,
bool doLight)
{
	// Nothing to see here
	if (m_muzzleFlash[forModel])
		m_muzzleFlash[forModel]->Start(GetBoneInWorld(forModel, BONE_MUZZLE), GetBoneToWorld(forModel, BONE_MUZZLE), doLight);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::HideMuzzleFlash()
{
	for (int i = 0; i < WEAPON_MAX_MODELS; i++)
	{
		if (m_muzzleFlash[i])
			m_muzzleFlash[i]->Hide();
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Firearm::UpdateMuzzleFlash(
int forModel,
float deltaT)
{
	// Nothing to see here
	if (m_muzzleFlash[forModel])
		m_muzzleFlash[forModel]->Update(deltaT, GetBoneInWorld(forModel, BONE_MUZZLE), GetBoneToWorld(forModel, BONE_MUZZLE));
}

/////////////////////////////////////////////////////////////////////////////
// Eject brass
/////////////////////////////////////////////////////////////////////////////
void Firearm::EjectBrass(
int forModel)
{
	// Anything to do?
	if (!m_ejectingBrass)
		return;

	// Create the dircos
	DirCos3x3 orientation;
	orientation = (Cartesian4x4 &)GetWeaponToWorld(forModel);

	// do it
	CActor *actorOwner = CActor::FromHandle(m_owner);
	if (actorOwner)
		m_ejectingBrass->Eject(actorOwner->GetBodyInWorld(),
										GetBoneInWorld(forModel, BONE_BREECH), GetBoneDir(forModel, BONE_BREECH),
										orientation, GetWeaponVelocity(forModel));
}

#ifdef DIRECTX_PC
// virtual
void Firearm::Hide()
{
	// hide our weapon
	TTLWeapon::Hide();
	// hide our muzzle flash if we haven't
	HideMuzzleFlash();
	// set our state to not show muzzle flash
	// (maybe brass eject too?)
	m_showMuzzleFlash = false;
}

// virtual
void Firearm::Show()
{
	// show our weapon
	TTLWeapon::Show();
	// show our muzzle flash
	// (and maybe brass)
	m_showMuzzleFlash = true;
}
#endif
