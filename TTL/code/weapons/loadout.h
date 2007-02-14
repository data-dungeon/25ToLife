/////////////////////////////////////////////////////////////////////////////
// The weapon loadout class!
/////////////////////////////////////////////////////////////////////////////
#ifndef LOADOUT_H
#define LOADOUT_H

#include "weapons/ttlweapon.h"

class WeaponInventory;
class ParameterSet;

// A loadout is just a properties string
class WeaponLoadout
{
public:
	// An empty loadout
	WeaponLoadout();

	// Create a loadout string from an existing weapon inventory
	WeaponLoadout(WeaponInventory &inventory);

	// Create a loadout from a weapon string
	WeaponLoadout(const char *paramString);

	// Create a loadout from a parameter set
	WeaponLoadout(ParameterSet &param);

	// Clear this loadout
	void Clear();

	// Add/appeend a weapon
	void AddWeapon(const char *weaponName, int index);
	void AppendWeapon(const char *weaponName);

	// Add ammo
	bool AddAmmo(const char *type, int amount);
	bool AddAmmo(TTLWeapon::Slot slot, int amount);

	// Set ammo
	bool SetAmmo(const char *type, int amount);
	bool SetAmmo(TTLWeapon::Slot slot, int amount);

	// Full ammo!
	bool FullAmmo(const char *type);
	bool FullAmmo(TTLWeapon::Slot slot);
	bool FullAmmo();

	// Access the info in this loadout (not complete)

	// Return highest indexed weapon in here
	int Weapons();

	// Return the weapon with this index (could be NULL)
	const char *GetWeapon(int index);

	// Fill out a weapon inventory from this loadout
	// Returns weapons added
	int Build(WeaponInventory &inventory, bool reset = false);

	// Using an arbitrary propeties string, build this inventory
	// Returns weapons added
	static int Build(ParameterSet &param, WeaponInventory &inventory, bool reset = false);

	// build a random inventory
	// Returns weapons added
	static int BuildRandom(WeaponInventory &inventory);

	// build an indexed inventory
	// Returns weapons added
	static int BuildDebug(WeaponInventory &inventory, int index);

	// Show information about the inventory
	static void DebugDump(WeaponInventory &inventory);

	// Get at the parameter set directly
	ParameterSet &GetParameterSet()
		{ return m_param; }

	// you must call delete [] on this return value
	const char *GetParameterString()
		{ return m_param.GetInitializerString(); }

private:
	ParameterSet	m_param;
	int				m_onWeapon;

	// utilties
	static bool ApplyAmmo(TTLWeapon &weapon, const char *type, const char *amount);

	// A quick variable maker
	static const char *WeaponParam(int index);
	static char s_varname[64];

	// Parse a weapon
	static void ParseWeaponAmmo(const char *key, char *weapon, int &ammoCode);
	static void BuildWeaponAmmo(const char *weapon, int ammoCode, char *key);
};

#endif
