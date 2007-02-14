/////////////////////////////////////////////////////////////////////////////
// Base weapon class for the game
/////////////////////////////////////////////////////////////////////////////
#ifndef TTLWEAPON_H
#define TTLWEAPON_H

#include "game/animation/attachp.h"
#include "Components/weapons/weaplib.h"
#include "weapons/weaponanim.h"
#include "weapons/aiming.h"
#include "weapons/ttlprojectile.h"

class ZoomSight;

// Our properties
class TTLWeaponProperties : public WeaponProperties
{
public:
	// Weee
	TTLWeaponProperties(WeaponCreator &creator, int index);
	virtual ~TTLWeaponProperties();

	// For parsing the data
	void RegisterContext(const char *context);

	// After the load
	void PostLoadSetup();

	// The slot (0-3 right now)
	int				m_slot;

	// The restriction code
	char				m_restrict[8];

	// Stuff for animations
	char				m_holdType[12];
	char				m_reloadType[12];

	// OUr aiming info
	char				m_aimingName[16];
	Aiming			*m_aiming;
	float AimTurningFactor() const
		{ return m_aiming ? m_aiming->AimTurningFactor() : 1.0f; }
	float MovementFactor() const
		{ return m_aiming ? m_aiming->MovementFactor() : 1.0f; }
	float VibrateIntensity() const
		{ return m_aiming ? m_aiming->VibrateIntensity() : 0.0f; }
	float VibrateDuration() const
		{ return m_aiming ? m_aiming->VibrateDuration() : 0.0f; }

	// These must be implemented by your subclass
	const WeaponAimProps *GetWeaponAimProps() const
		{ return m_aiming ? m_aiming->GetWeaponAimProps() : NULL; }
	int GetStances() const
		{ return m_aiming ? m_aiming->GetStances() : 0; }

	// Our info
	WeaponClass		m_weaponClass;
	WeaponType		m_weaponType;
	WeaponSubType	m_weaponSubType;
	WeaponHold		m_weaponHold;
	WeaponReload	m_weaponReload;

	// A flag of restrictions for this weapon
	enum RestrictCode
	{
		ALLOWCOPS = 0x0001,
		ALLOWGANGSTER = 0x0002,
		ALLOWMULTIPLAYER = 0x0004,
		NPCONLY = 0x0008,
		SUPERWEAPON = 0x0010,
		TYPEDEFAULT = 0x0100,
		SLOTDEFAULT = 0x0200,
	};
	int				m_weaponRestrict;
	bool Test(RestrictCode code) const
		{ return ((m_weaponRestrict & code) != 0); }
};

// The actual implementation
class TTLWeapon : public Weapon
{
public:
	TTLWeapon(const TTLWeaponProperties &properties);
	virtual ~TTLWeapon();

	// Get at some info
	WeaponClass	GetWeaponClass() const
		{ return m_ttlProps.m_weaponClass; }
	WeaponType GetWeaponType() const
		{ return m_ttlProps.m_weaponType; }
	WeaponSubType GetWeaponSubType() const
		{ return m_ttlProps.m_weaponSubType; }
	WeaponHold GetWeaponHold() const
		{ return m_ttlProps.m_weaponHold; }
	WeaponReload GetWeaponReload() const
		{ return m_ttlProps.m_weaponReload; }

	// Monkey with the aim
	virtual void SetAim(const Vector3 &eye, const Vector3 &dir);
	virtual void SetAim(const Vector3 &target, float deviance = 0.0f);
	virtual void AimCameraShake(bool enable)
		{ m_cameraShakeAim = enable; }

	// Or, in a more fiendly way, what is the class of this weapon
	enum Slot
	{
		MELEE = 0,
		SECONDARY = 1,
		PRIMARY = 2,
		LOBBED = 3,
		SLOTS // Don't use this as a slot index!
	};
	Slot GetSlot()
		{ return (Slot)m_ttlProps.m_slot; }
	static const int MaxSlots()
		{ return SLOTS; }
	static bool SlotInRange(int slot)
		{ return (slot >= MELEE && slot <= LOBBED); }
	static const char *SlotName(Slot slot)
		{ return s_slotName[slot]; }

	// Other weapon info
	virtual float MaxRange() = 0;
	virtual float RateOfAttack() = 0;

	// -1 means not ammo based (i.e. melee)
	virtual int Ammo() = 0;
	virtual int MaxAmmo() = 0;
	float AmmoRatio()
		{ return (float)Ammo() / (float)MaxAmmo(); }
	virtual bool HasAmmo()
		{ return (Ammo() != 0); }

	// Interacting with ammo
	const char *GetAmmoType();
	virtual bool AddAmmo(const char *type, int amount)
		{ return SetAmmo(type, Ammo() + amount); }
#ifdef DIRECTX_PC
	virtual bool SetAmmo(const char *type, int amount, bool fillClip = false)
		{ return false; }
#else
	virtual bool SetAmmo(const char *type, int amount)
		{ return false; }
#endif
	virtual bool FullAmmo(const char *type)
		{ return SetAmmo(type, MaxAmmo()); }

	// A single integer code for ammo
	// Per-class specific BUT -1 means full, 0 means none
	virtual int CodifyAmmo() = 0;
	virtual void ApplyAmmoCode(int ammoCode) = 0;
	virtual void GetSplitAmmo(int& inBelt, int& inClip) = 0;
	virtual void SetSplitAmmo(int inBelt, int inClip) = 0;

	// Is this a dual wield weapon?
	bool IsDualWield() const
		{ return (Models() > 1); }

	// Set the aim
	ZoomSight *GetZoomSight()
		{ ASSERT(m_aimSight); return (ZoomSight *)m_aimSight; }

	// Activate/deactivate
	virtual void Activate();
	virtual void Deactivate();

	// Handle network traffic
	virtual bool DryFire(const CMunitionFired &fired, bool selected)
		{ return false; }

protected:
	// The Firearms propers
	const TTLWeaponProperties	&m_ttlProps;

	// Are we following camerashake?
	bool	m_cameraShakeAim;

	// Play a sound (you must handle this)
	enum SoundLocation
	{
		MUZZLE,
		BREACH,
		MAGAZINE
	};
 	void PlaySound(const char *name, SoundLocation location, int model = 0);
	SoundHandle m_fireHandle;
	void FireSound(const char *sound, float pitch = 0.0f, int model = 0);

	// Okay to auto reload?
	bool OkayToAutoReload()
		{ return !IsDead(m_owner); }

	// Utilities
	static bool InFront(const Vector3 &pos, float &dSquared);
	static bool IsDead(ActorHandle who);
	static float Now()
		{ return g_timer.GetEndSec(); }

	// the slot names
	static const char *s_slotName[4];
};

// Handy utility classes
class WeaponSoundParticle
{
public:
	// Init stuff
	WeaponSoundParticle();

	// Register the context
	void RegisterContext(const char *context, const char *name);

	// Do the particle/sound
	void Do(ActorHandle actor, const Vector3 &pos, const Vector3 *normal = NULL, float modifier = 1.0f, SoundHandle *handle = NULL) const;
	void DoStationary(const Vector3 &pos, const Vector3 *normal = NULL, float modifier = 1.0f, SoundHandle *handle = NULL) const;

private:
	char	m_sound[32];
	char	m_particle[24];
	float	m_volume;
	float	m_scale;
};

class EmitterWrapper
{
public:
	EmitterWrapper();
	~EmitterWrapper();

	// Create it
	bool Create(CActor *actor, const char *emitter, const char *bone = NULL);
	bool Create(CActor *actor, const char *emitter, const AttachPoints *attachPoints, AttachPointID usePoint);

	// destroy it
	void Destroy();

	// Activate/deactivate
	void Activate();
	void Deactivate();

	// Update
	void Update();

private:
	ActorHandle				m_actor;
	BoneID					m_boneID;
	const AttachPoints 	*m_attachPoints;
	AttachPointID 			m_attachPoint;
	t_Handle					m_handle;
	Vector3					m_pos, m_velocity, m_up;
};

#endif
