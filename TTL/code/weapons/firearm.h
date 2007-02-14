/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a Firearm
/////////////////////////////////////////////////////////////////////////////
#ifndef FIREARM_H
#define FIREARM_H

#include "weapons/ttlweapon.h"
#include "weapons/muzzleLib.h"
#include "weapons/ebrassLib.h"

// The creator
class FirearmCreator : public WeaponCreator
{
public:
	// Do the work
	FirearmCreator();

	// Create
	Weapon *CreateInstance(const VarObjProperties<Weapon> &properties);

	// We have our own initialize and terminate
	void InitializeOnce();
	void TerminateOnce();

	void InitializeEvery();
	void TerminateEvery();

protected:
	// Subclass this to provide new
	WeaponProperties *NewProperties(int index);
};

// Our properties
class FirearmProperties : public TTLWeaponProperties
{
public:
	// Weee
	FirearmProperties(WeaponCreator &creator, int index);
	~FirearmProperties();

	// For parsing the data
	void RegisterContext(const char *context);

	// Projectile name
	char 				m_displayProjectile[WEAPONSTRING_SIZE];

	// What is the firing sound mod?
	char 				m_fireSound[WEAPONSTRING_SIZE];
	float				m_firePitch;
	char 				m_emptySound[WEAPONSTRING_SIZE];

	// Start of reload sound
	char				m_reloadStart[WEAPONSTRING_SIZE];
	char				m_reloadEach[WEAPONSTRING_SIZE];
	char				m_reloadEnd[WEAPONSTRING_SIZE];

	// What is the firing effect
	char 				m_muzzleFlash[WEAPONSTRING_SIZE];
	char				m_ejectingBrass[WEAPONSTRING_SIZE];

	// What is the rate of fire (projectiles per second)
	bool				m_automatic;
	float 			m_rateOfFire;

	// After each fire, play this sound
	char				m_nextRoundSound[WEAPONSTRING_SIZE];
	float				m_nextRoundSoundDelay;

	// How big is the burst (0 = single shot, size -1 = fully auto)
	int 				m_burstCount;
	float				m_burstRate;

	// Is this weapon clip based (like a pistol)
	// or loose ammo like a shotgun
	bool				m_clipBased;

	// Total capacity of bullets weapon can hold
	int				m_magazineSize;
	int				m_totalRounds;

	// Reaload time in seconds
	float 			m_reloadTime;
};

// The actual implementation
class Firearm : public TTLWeapon
{
public:
	Firearm(const FirearmProperties &properties);
	~Firearm();

	// What is our display projectile
	const char *DisplayProjectile() const
		{ return m_faProps.m_displayProjectile; }

	// If out of ammo, does it reload?
 	bool SetAutoReload(bool autoReload);

	// Set the firing mode (returns false if unallowed)
	enum FiringMode
	{
		SEMIAUTOMATIC,
		AUTOMATIC
	};
	bool SetFiringMode(FiringMode newMode);
	FiringMode GetFiringMode() const;

	// Does this weapon use clip based ammo?
 	bool ClipBased() const;

	// How many rounds are in a clip
	int ClipCapacity() const;

	// How many rounds fit in the magazine
	int MagazineCapacity() const;

	// How many rounds are left in the gun
	int RoundsLoaded() const;

	// What is the total number of rounds left (not counting what is in gun)
	int TotalRounds() const;

	// How many rounds have been fired since the creation of this gun
	int TotalRoundsFired() const;

	// Has ammo?
	int Ammo()
		{ return m_totalAmmo + m_ammoInGun; }
	int MaxAmmo()
		{ return m_faProps.m_totalRounds; }

	// Give gun some ammo!
#ifdef DIRECTX_PC
	bool SetAmmo(const char *type, int amount, bool fillClip = false);
#else
	bool SetAmmo(const char *type, int amount);
#endif
	bool FullAmmo(const char *type);

	// A single integer code for ammo
	// Per-class specific BUT -1 means full, 0 means none
	int CodifyAmmo();
	void ApplyAmmoCode(int ammoCode);
	void GetSplitAmmo(int& inBelt, int& inClip);
	void SetSplitAmmo(int inBelt, int inClip);

	// Reset the state of this weapon
	void Reset();

	// The states of the gun
	State GetState() const;

	// Request a fire (returns true if it worked)
	bool PullTrigger();
	bool ReleaseTrigger();

	// Dry fire for network
	bool DryFire(const CMunitionFired &fire, bool selected);

	// Request a reload
	bool Reload();

	// Cancel the current state (be it firing, reloading, etc)
	bool Cancel();

	// How far into the reload are we (negative means not reloading)
	float ReloadedRatio() const;

	// Make gun not ready for a time
	bool Unready(float time);

	// Activate/deactivate this weapon
	void Activate();
	void Deactivate();

	// Other weapon info
	float MaxRange();
	float RateOfAttack();

	// Call this once per frame
	void Update();

	// Enable disable the off-hand weapon
	bool HasOffHand() const
		{ return (Models() > 1); }
	bool OffHandEnabled() const
		{ return !m_offHandDisabled; }
	bool EnableOffHand(bool enable);

	bool ReloadEnabled() const
		{ return (m_reloadEnabled); }
	void EnableReload(bool enable)
		{ m_reloadEnabled = enable; }

	// What is the currently active model?
	int GetActiveModel()
		{ return m_useModel; }

#ifdef DIRECTX_PC
	// virtual functions inhereted from weapon/ttlweapon
	virtual void Hide();
	virtual void Show();
#endif

protected:
	// The Firearms propers
	const FirearmProperties	&m_faProps;

	// What is our firing mode
	FiringMode				m_firingMode;

	// Our current state
	State 					m_state;
	float						m_readyAtTime;
	float						m_reloadUpdateTime;
	float						m_reloadStartTime;
	bool						m_triggerDown;
	void SetState(State newState);

#ifdef DIRECTX_PC
	bool					m_showMuzzleFlash;
#endif

	// Ammo status
	int						m_totalAmmo;
	int						m_ammoInGun;
	int						m_totalRoundsFired;
	int						m_magazineSize; // could be 1/2 if offhand is disabled
	bool FullyLoaded();
	bool AbortReload();
	void UpdateReload();

	// How many bullets we are firing at this moment
	int						m_burstCount;
	int						m_useModel;
	float						m_nextShot;
	float 					m_nextRoundSoundDelay;
	bool PrepFire();
	virtual void UpdateFire();
	void NextModel();

	// Offhand support
	bool						m_offHandDisabled;

	// Reload support
	bool						m_reloadEnabled;

	// Muzzle flash support
	MuzzleFlash				*m_muzzleFlash[WEAPON_MAX_MODELS];
	void CreateMuzzleFlash(int forModel, bool doLight = true);
	void HideMuzzleFlash();
	void UpdateMuzzleFlash(int forModel, float deltaT);

	// Eject brass
	EjectingBrass *m_ejectingBrass;
	void EjectBrass(int forModel);

	// Do we auto-reload?
	bool						m_autoReload;
};

#endif
