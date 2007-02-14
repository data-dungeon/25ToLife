/////////////////////////////////////////////////////////////////////////////
// The armory is an API for enumerating and creating loadouts
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/armory.h"
#include "weapons/loadout.h"

char WeaponLoadout::s_varname[] = "";

/////////////////////////////////////////////////////////////////////////////
// Wee!
/////////////////////////////////////////////////////////////////////////////
WeaponLoadout::WeaponLoadout()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////
// Create a loadout string from an existing weapon inventory
/////////////////////////////////////////////////////////////////////////////
WeaponLoadout::WeaponLoadout(
WeaponInventory &inventory)
{
	Clear();

	// Build it
	for (int w = 0; w < inventory.Count(); w++)
	{
		TTLWeapon *weapon = (TTLWeapon *)inventory.Get(w);
		if (weapon)
		{
			char key[256];
			BuildWeaponAmmo(weapon->Name(), weapon->CodifyAmmo(), key);
			AppendWeapon(key);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Create a loadout from a weapon string
/////////////////////////////////////////////////////////////////////////////
WeaponLoadout::WeaponLoadout(
const char *paramString)
{
	Clear();
	m_param.SetFromString(paramString);
}

/////////////////////////////////////////////////////////////////////////////
// Create a loadout from a parameter set
/////////////////////////////////////////////////////////////////////////////
WeaponLoadout::WeaponLoadout(
ParameterSet &param)
{
	Clear();
	m_param.SetFromParameterSet(param);
}

/////////////////////////////////////////////////////////////////////////////
// Clear this loadout
/////////////////////////////////////////////////////////////////////////////
void WeaponLoadout::Clear()
{
	m_param.Clear();
	m_onWeapon = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Add an item
/////////////////////////////////////////////////////////////////////////////
void WeaponLoadout::AddWeapon(
const char *weaponName,
int index)
{
	if (weaponName)
	{
		m_param.Set(WeaponParam(index), weaponName);
		if (index >= m_onWeapon)
			m_onWeapon = index + 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Append an item
/////////////////////////////////////////////////////////////////////////////
void WeaponLoadout::AppendWeapon(
const char *weaponName)
{
	if (weaponName)
		m_param.Set(WeaponParam(m_onWeapon++), weaponName);
}

/////////////////////////////////////////////////////////////////////////////
// Add ammo
/////////////////////////////////////////////////////////////////////////////
bool WeaponLoadout::AddAmmo(
const char *type,
int amount)
{
	char value[64];
	sprintf(value, "+%d", amount);
	m_param.Set(type, value);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponLoadout::AddAmmo(
TTLWeapon::Slot slot,
int amount)
{
	char var[64];
	sprintf(var, "ammoSlot%d", (int)slot);

	char value[64];
	sprintf(value, "+%d", amount);
	m_param.Set(var, value);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Set ammo
/////////////////////////////////////////////////////////////////////////////
bool WeaponLoadout::SetAmmo(
const char *type,
int amount)
{
	char value[64];
	sprintf(value, "%d", amount);
	m_param.Set(type, value);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponLoadout::SetAmmo(
TTLWeapon::Slot slot,
int amount)
{
	char var[64];
	sprintf(var, "ammoSlot%d", (int)slot);

	char value[64];
	sprintf(value, "%d", amount);

	m_param.Set(var, value);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Set ammo
/////////////////////////////////////////////////////////////////////////////
bool WeaponLoadout::FullAmmo(
const char *type)
{
	m_param.Set(type, "*");
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponLoadout::FullAmmo(
TTLWeapon::Slot slot)
{
	char var[64];
	sprintf(var, "ammoSlot%d", (int)slot);
	m_param.Set(var, "*");
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponLoadout::FullAmmo()
{
	for (int i = 0; i < TTLWeapon::MaxSlots(); i++)
		FullAmmo((TTLWeapon::Slot)i);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Return highest indexed weapon in here
/////////////////////////////////////////////////////////////////////////////
int WeaponLoadout::Weapons()
{
	for (int i = 10; i > 0; i--)
	{
		if (m_param.GetString(WeaponParam(i - 1)))
			return i;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Return the weapon with this index (could be NULL)
/////////////////////////////////////////////////////////////////////////////
const char *WeaponLoadout::GetWeapon(int index)
{
	return m_param.GetString(WeaponParam(index));
}

/////////////////////////////////////////////////////////////////////////////
// Fill out a weapon inventory from
/////////////////////////////////////////////////////////////////////////////
int WeaponLoadout::Build(
WeaponInventory &inventory,
bool reset)
{
	return Build(m_param, inventory, reset);
}

/////////////////////////////////////////////////////////////////////////////
// Using an arbitrary propeties string, build this inventory
// Returns weapons added
/////////////////////////////////////////////////////////////////////////////
int WeaponLoadout::Build(
ParameterSet &param,
WeaponInventory &inventory,
bool reset)
{
	// Setup the inventory
	if (reset)
	{
		inventory.SetMaxWeapons(TTLWeapon::MaxSlots());
		inventory.RemoveAll();
	}

	// Look for all weapons
	int weaponsAdded = 0;
	for (int i = 0; i < 10; i++)
	{
		char *key = param.GetString(WeaponParam(i));
		if (!key)
			continue;

		char weaponName[64];
		int ammoCode;
		ParseWeaponAmmo(key, weaponName, ammoCode);

		if (weaponName[0])
		{
			TTLWeapon *weapon = (TTLWeapon *)g_weaponLib.CreateInstance(weaponName);
			if (!weapon)
				continue;

			// Slot
			int slot = weapon->GetSlot();

			// Kill the old one
			if (inventory.Get(slot))
				inventory.Remove(slot);

			// Apply the ammo
			weapon->ApplyAmmoCode(ammoCode);

			// Add the new one
			if (!inventory.Add(weapon, slot))
				delete weapon;
			else
				++weaponsAdded;
		}
	}

#if 0
	// Now do all the specific ammo
	for (int a = 0; a < AMMOTYPES; a++)
	{
		// What are we looking for?
		const char *type = TTLProjectileProperties::AmmoTypeToString((AmmoType)a);
		if (!type || !type[0])
			continue;

		const char *value = param.GetString(type);
		if (!value || !value[0])
			continue;

		// Scan the weapons
		for (int w = 0; w < TTLWeapon::MaxSlots(); w++)
		{
			TTLWeapon *weapon = (TTLWeapon *)inventory.Get(w);
			if (weapon && weapon->GetAmmoType() == (AmmoType)a)
				ApplyAmmo(*weapon, (AmmoType)a, value);
		}
	}
#endif

	// Now do all the slot ammo
	for (int s = 0; s < TTLWeapon::MaxSlots(); s++)
	{
		// What are we looking for?
		char var[64];
		sprintf(var, "ammoSlot%d", s);
		const char *value = param.GetString(var);
		if (!value || !value[0])
			continue;

		TTLWeapon *weapon = (TTLWeapon *)inventory.Get(s);
		if (weapon)
			ApplyAmmo(*weapon, weapon->GetAmmoType(), value);
	}

	if (reset)
	{
		inventory.Reset();
		inventory.Select(TTLWeapon::PRIMARY);
	}

	return weaponsAdded;
}

/////////////////////////////////////////////////////////////////////////////
// Using an arbitrary propeties string, build this inventory
// Returns weapons added
/////////////////////////////////////////////////////////////////////////////
int WeaponLoadout::BuildRandom(
WeaponInventory &inventory)
{
	// Setup the inventory
	inventory.SetMaxWeapons(TTLWeapon::MaxSlots());
	inventory.RemoveAll();

	// Look for all weapons
	int weaponsAdded = 0;
	for (int i = 0; i < TTLWeapon::MaxSlots(); i++)
	{
		TTLWeapon::Slot slot = (TTLWeapon::Slot)i;
		int itemsForSlot = g_armory.Items(slot);
		const Armory::Description *desc = g_armory.Get(slot, g_random.UpToExcludeHigh(itemsForSlot));
		if (desc)
		{
			TTLWeapon *weapon = (TTLWeapon *)g_weaponLib.CreateInstance(desc->LoadoutName());
			ASSERT(weapon);
			if (weapon && !inventory.Add(weapon, weapon->GetSlot()))
				delete weapon;
			else
			{
				weapon->FullAmmo(weapon->GetAmmoType());
				++weaponsAdded;
			}
		}
	}

	// Select the primary weapon
	inventory.Reset();
	inventory.Select(TTLWeapon::PRIMARY);

	return weaponsAdded;
}

/////////////////////////////////////////////////////////////////////////////
// Returns weapons added
/////////////////////////////////////////////////////////////////////////////
int WeaponLoadout::BuildDebug(
WeaponInventory &inventory,
int index)
{
	// Setup the inventory
	inventory.SetMaxWeapons(TTLWeapon::MaxSlots());
	inventory.RemoveAll();

	// Look for all weapons
	int weaponsAdded = 0;
	for (int i = 0; i < TTLWeapon::MaxSlots(); i++)
	{
		TTLWeapon::Slot slot = (TTLWeapon::Slot)i;
		int itemsForSlot = g_armory.Items(slot);
		const Armory::Description *desc = g_armory.Get(slot, index % itemsForSlot);
		if (desc)
		{
			TTLWeapon *weapon = (TTLWeapon *)g_weaponLib.CreateInstance(desc->LoadoutName());
			ASSERT(weapon);
			if (weapon && !inventory.Add(weapon, weapon->GetSlot()))
				delete weapon;
			else
			{
				weapon->FullAmmo(weapon->GetAmmoType());
				++weaponsAdded;
			}
		}
	}

	// Select the primary weapon
	inventory.Reset();
	inventory.Select(TTLWeapon::PRIMARY);

	return weaponsAdded;
}

/////////////////////////////////////////////////////////////////////////////
// Show information about the inventory
/////////////////////////////////////////////////////////////////////////////
void WeaponLoadout::DebugDump(
WeaponInventory &inventory)
{
	for (int i = 0; i < TTLWeapon::MaxSlots(); i++)
	{
		TTLWeapon *weapon = (TTLWeapon *)inventory.Get(i);
		if (!weapon)
		{
			TRACE("Slot %d - Empty\n", i);
		}
		else
		{
			TRACE("Slot %d - %s/%s model: %s ammo: %d/%d\n", i, weapon->Name(), weapon->DisplayName(), weapon->GetModel(0).name, weapon->Ammo(), weapon->MaxAmmo());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// utilties
/////////////////////////////////////////////////////////////////////////////
bool WeaponLoadout::ApplyAmmo(
TTLWeapon &weapon,
const char *type,
const char *amount)
{
	// This weapon wants this ammo
	if (amount[0] == '*')
		return weapon.FullAmmo(type);
	else if (amount[0] == '+')
		return weapon.AddAmmo(type, atoi(&amount[1]));
	else
		return weapon.SetAmmo(type, atoi(amount));
}

/////////////////////////////////////////////////////////////////////////////
// utilties
/////////////////////////////////////////////////////////////////////////////
const char *WeaponLoadout::WeaponParam(
int index)
{
	sprintf(s_varname, "Weapon%d", index);
	return s_varname;
}

/////////////////////////////////////////////////////////////////////////////
// Parse a weapon
/////////////////////////////////////////////////////////////////////////////
void WeaponLoadout::ParseWeaponAmmo(
const char *key,
char *weapon,
int &ammoCode)
{
	while (*key && *key != '.')
		*weapon++ = *key++;
	*weapon = '\0';

	if (*key == '.')
		++key;

	char ammoString[64];
	char *ammoPtr = &ammoString[0];
	while (*key)
		*ammoPtr++ = *key++;
	*ammoPtr = '\0';

	if (ammoString[0] == '\0')
		ammoCode = -1;
	else
		ammoCode = atoi(ammoString);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WeaponLoadout::BuildWeaponAmmo(
const char *weapon,
int ammoCode,
char *key)
{
	if (ammoCode == -1)
		strcpy(key, weapon);
	else
		sprintf(key, "%s.%d", weapon, ammoCode);
}


