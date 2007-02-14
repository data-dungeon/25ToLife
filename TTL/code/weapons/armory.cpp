/////////////////////////////////////////////////////////////////////////////
// The armory is an API for enumerating and creating loadouts
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Components/weapons/weaplib.h"
#include "Components/weapons/weapinv.h"
#include "weapons/firearm.h"
#include "weapons/melee.h"
#include "weapons/lobbed.h"
#include "environ/ActorCreator.h"
#include "environ/ConfigMgr.h"
#include "weapons/armory.h"

DeclareSingleton(Armory);

/////////////////////////////////////////////////////////////////////////////
// Wee
/////////////////////////////////////////////////////////////////////////////
Armory::Armory()
{
	// Create the array
	m_slot = new DynamicArray<Description>[MaxSlots()];
	ASSERT(m_slot);
	if (!m_slot)
		return;

	// Build it!
	for (int w = 0; w < g_weaponLib.Types(); w++)
	{
		// Get the properties
		TTLWeaponProperties *props = (TTLWeaponProperties *)g_weaponLib.GetProperties(w);
		ASSERT(props);

		// Put it into the right list
		int slot = props->m_slot;
		ASSERT(slot >= 0 && slot < MaxSlots());
		if (slot < 0 && slot >= MaxSlots())
			continue;

		// Build the description
		Description desc(m_slot[slot].Count(), props);

		// Add it
		m_slot[slot].Add(desc, desc.Index());
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Armory::~Armory()
{
	delete [] m_slot;
}

/////////////////////////////////////////////////////////////////////////////
// How many of each weapon are there?
/////////////////////////////////////////////////////////////////////////////
int Armory::Items(
TTLWeapon::Slot slot)
{
	if (!m_slot)
		return 0;

	return m_slot[slot].Count();
}

/////////////////////////////////////////////////////////////////////////////
// Get a description (returns NULL if index is out of range)
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *Armory::Get(
TTLWeapon::Slot slot,
int index)
{
	// Make sure we are cool
	if (!m_slot || index < 0 || index >= m_slot[slot].Count())
		return NULL;

	return m_slot[slot].Get(index);
}

/////////////////////////////////////////////////////////////////////////////
// Get a description (returns NULL if index is out of range)
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *Armory::Find(
const char *name)
{
	for (int slot = 0; slot < MaxSlots(); slot++)
	{
		for (int i = 0; i < m_slot[slot].Count(); i++)
		{
			const Description *desc = m_slot[slot].Get(i);
			ASSERT(desc);
			if (strcmpi(name, desc->LoadoutName()) == 0)
				return desc;
		}
	}

	// Not found
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// This is dumb
/////////////////////////////////////////////////////////////////////////////
void Armory::Description::DataMineAmmo()
{
	// Init to not knowing anything
	m_maxAmmo = -1;
	m_ammoPerClip = -1;
	if (!m_props)
		return;

	if (m_props->m_weaponClass == WEAPONCLASS_FIREARM)
	{
		FirearmProperties *faProps = (FirearmProperties *)m_props;
		m_maxAmmo = faProps->m_totalRounds;
		if (faProps->m_clipBased)
			m_ammoPerClip = faProps->m_magazineSize;
	}
	else if (m_props->m_weaponClass == WEAPONCLASS_LOBBED)
	{
		LobbedProperties *lProps = (LobbedProperties *)m_props;
		m_maxAmmo = lProps->m_totalAmmo;
	}
	else if (m_props->m_weaponClass == WEAPONCLASS_MELEE)
	{
		// use defaults
	}
	else
	{
		ASSERT(false);
	}
}

