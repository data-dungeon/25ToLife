/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a melee weapon
/////////////////////////////////////////////////////////////////////////////
#ifndef MELEE_H
#define MELEE_H

#include "weapons/ttlweapon.h"

// The creator
class MeleeCreator : public WeaponCreator
{
public:
	// Do the work
	MeleeCreator();

	// Create
	Weapon *CreateInstance(const VarObjProperties<Weapon> &properties);

protected:
	// Subclass this to provide new
	WeaponProperties *NewProperties(int index);
};

// Our properties
class MeleeProperties : public TTLWeaponProperties
{
public:
	// Weee
	MeleeProperties(WeaponCreator &creator, int index);
	~MeleeProperties();

	// For parsing the data
	void RegisterContext(const char *context);

	// The idle emitter
	char						m_idleEmitter[WEAPONSTRING_SIZE];

	// The sounds and particles
	WeaponSoundParticle	m_attack;

	// How fast can we swing it
	float 					m_rateOfAttack;

	// Stuff for the swoosh
	bool						m_swoosh;
	float						m_swooshLength;
	Vector4					m_swooshColor;
	float						m_swooshMaxAlpha;
	float						m_swooshLifetime;
	bool						m_swooshAdditive;
	float						m_swooshPointMultiply;
	bool						m_swooshSharp;
};

// The actual implementation
class Melee : public TTLWeapon
{
public:
	Melee(const MeleeProperties &properties);
	~Melee();

	// Set the owner
	void SetOwner(CActor *owner);

	// Reset the state of this weapon
	void Reset();

	// The states of the gun
	State GetState() const;

	// Request a fire (returns true if it worked)
	bool PullTrigger();
	bool ReleaseTrigger();

	// AI simplified fire
	bool AIFire();

	// Request a reload
	bool Reload();

	// Make gun not ready for a time
	bool Unready(float time);

	// Activate/deactivate this weapon
	void Activate();
	void Deactivate();

	// Has ammo?
	int Ammo()
		{ return -1; }
	int MaxAmmo()
		{ return -1; }

	// A single integer code for ammo
	// Per-class specific BUT -1 means full, 0 means none
	int CodifyAmmo()
		{ return -1; }
	void ApplyAmmoCode(int ammoCode)  {}
	void GetSplitAmmo(int& inBelt, int& inClip)
		{ inBelt = inClip = -1; }
	void SetSplitAmmo(int inBelt, int inClip) {}

	// Other weapon info
	float MaxRange();
	float RateOfAttack()
		{ return m_mProps.m_rateOfAttack; }

	// Call this once per frame
	void Update();

protected:
	// The Melees propers
	const MeleeProperties	&m_mProps;

	// Our current state
	State							m_state;

	// when can we fire again?
	float							m_nextAttack;

	// Are we lethal?
	bool							m_lethal;

	// Our message handlers
	REF_BASIC_MESSAGE_HANDLER(LethalStartHandler, MESSAGE_VALUE, Melee) m_lethalStartHandler;
	REF_BASIC_MESSAGE_HANDLER(LethalEndHandler, MESSAGE_VALUE, Melee) m_lethalEndHandler;

	// Stuff for swooshing
	t_Handle						m_swoosh;
	Vector3Packed				m_swooshStart, m_swooshEnd;
	bool CreateSwoosh();
	void DestroySwoosh();
	void ActivateSwoosh();
	void DeactivateSwoosh();
	void UpdateSwoosh();
};

#endif
