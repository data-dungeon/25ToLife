/////////////////////////////////////////////////////////////////////////////
// The armory is an API for enumerating and creating loadouts
/////////////////////////////////////////////////////////////////////////////
#ifndef ARMORY_H
#define ARMORY_H

#include "EngineHelper/Singleton.h"
#include "container/Darray.h"
#include "weapons/ttlweapon.h"
#include "weapons/loadout.h"
#include "Components/weapons/weapdep.h"

// What is the current flavor of loadouts this week?
class Armory
{
public:
	// Wee
	Armory();
	~Armory();

	// How many slots are currently supported?
	int MaxSlots()
		{ return TTLWeapon::MaxSlots(); }

	// How many of each weapon are there?
	int Items(TTLWeapon::Slot slot);

	// The weapon description
	class Description
	{
	public:
		Description()
			{ m_index = -1; m_props = NULL; m_maxAmmo = -1; m_ammoPerClip = -1; }
		Description(int index, const TTLWeaponProperties *props)
			{ Setup(index, props); }

		// Set it up
		void Setup(int index, const TTLWeaponProperties *props)
			{ m_index = index; m_props = props; DataMineAmmo(); }

		// The name to use to create this weapon
		int Index() const
			{ return m_index; }
		const char *LoadoutName() const
			{ return m_props->GetName(); }

		// What slot is this for?
		int Slot() const
			{ return m_props->m_slot; }

		WeaponType GetWeaponType() const
			{ return m_props->m_weaponType; }

		// Access the properties directly
		const TTLWeaponProperties &Properties() const
			{ ASSERT(m_props); return *m_props; }

		// Nice stuff for interface
		const char *DisplayName() const
			{ return m_props->m_displayName; }
		const char *Icon() const
			{ return m_props->m_icon;}
		int MaxAmmo() const
			{ return m_maxAmmo; }
		int AmmoPerClip() const
			{ return m_ammoPerClip; }

		// Operator =
		Description &operator=(const Description &d)
			{ m_index = d.m_index; m_props = d.m_props; m_maxAmmo = d.m_maxAmmo; m_ammoPerClip = d.m_ammoPerClip; return *this; }

	private:
		// Index in the array
		int						m_index;

		// The properties we refer to
		const TTLWeaponProperties *m_props;

		// -1 means infinite or not applicable
		int						m_maxAmmo, m_ammoPerClip;
 		void DataMineAmmo();
	};

	// Get a description (returns NULL if index is out of range)
	const Description *Get(TTLWeapon::Slot slot, int index);
	const Description *Find(const char *weapon);

private:
	// Wee!
	DynamicArray<Description>	*m_slot;
};

DefineSingleton(Armory)
#define g_armory GetSingleton(Armory)

#endif
