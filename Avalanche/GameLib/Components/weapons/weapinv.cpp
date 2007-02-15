/////////////////////////////////////////////////////////////////////////////
// Weapon inventory
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"
#include "Components/weapons/weapinv.h"
#include "Components/ComponentCreator.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(WeaponInventory, BaseWeaponInventory, COMPONENT_NEUTRAL_PRIORITY);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponInventory::WeaponInventory(CCompActor &owner) :
CActorComponent(owner)
{
	// The list
	m_list = NULL;
	m_size = 0;
	m_maxSize = -1;

	m_current = -1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponInventory::~WeaponInventory()
{ 
	Free(); 
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WeaponInventory::Initialize()
{
}

/////////////////////////////////////////////////////////////////////////////
// Set the maximum number of weapons. If this returns false
// there are weapons beyond maxWeapons in the list.
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::SetMaxWeapons(
int maxWeapons)
{
	// What is the number of used weapons
	int highestIndex = 0;
	for (int i = 0; i < m_size; i++)
	{	
		if (m_list[i])
			highestIndex = i;
	}

	// Too late
	if (highestIndex >= maxWeapons)
		return false;

	// Set it
	m_maxSize = maxWeapons;	
	Realloc(m_maxSize);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Weapon *WeaponInventory::GetSelected()
{
	// Valid selection?
	if (!InRange(m_current))
		return NULL;

	// This is an interesting assert
	ASSERT(!m_list[m_current] || m_list[m_current]->GetOwner() == &GetActor());
	return m_list[m_current];
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int WeaponInventory::GetSelectedIndex()
{
	return m_current;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::SelectNext()
{
	int index = m_current;
	int tries = m_size;
	while (tries-- > 0)
	{
		// who do we select?
		if (++index >= m_size)
			index = 0;

		// Select it	
		Select(index);
		if (GetSelected())
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::SelectPrevious()
{
	int index = m_current;
	int tries = m_size;
	while (tries-- > 0)
	{
		// who do we select?
		if (--index < 0)
			index = m_size - 1;

		// Select it	
		Select(index);
		if (GetSelected())
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::SelectNoWeapon()
{
	Select(-1); 
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::Select(
int index)
{
	// Already selected?
	if (index == m_current)
		return true;

	// Goddamned this is such a hack
	Weapon *weapon = GetSelected();
	if (weapon)
		weapon->Deactivate();

	// Assign the new
	m_current = index;

	weapon = GetSelected();
	if (weapon)
	{
		ASSERT(weapon->GetOwner() == &GetActor());

		// Activate this weapon
		weapon->Activate();
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::Select(
const char *name)
{
	for (int i = 0; i < m_size; i++)
	{
		if (m_list[i] && strcmpi(m_list[i]->Name(), name) == 0)
			return Select(i);
	}
	
	// They are smokin'
	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Weapon *WeaponInventory::Find(const char *name)
{
	for (int i = 0; i < m_size; i++)
	{
		if (m_list[i] && strcmpi(m_list[i]->Name(), name) == 0)
			return m_list[i];
	}
	
	// They are smokin'
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::Append(
Weapon *weapon)
{
	// Duh
	if (!weapon)
		return false;

	// try to find a hole from the back
	int hole = -1;
	for (int search = m_size - 1; search >= 0; search--)
	{
		if (!m_list[search])
			hole = search;
	}

	// Any space?
	if (hole < 0)
	{
		// Must make bigger
		hole = m_size;
		if (!Realloc(hole + 1))
			return false;
	}

	// Do it!
	m_list[hole] = weapon;
	weapon->SetOwner(&GetActor());

	// Do we have a selected weeapon?
	if (!InRange(m_current))
		Select(hole);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::Add(
Weapon *weapon,
int index)
{
	ASSERT(weapon);
	if (!weapon)
		return false;

	// Adding it to the end?
	if (index < 0)
		index = m_size;

	// Is the array big enough
	if (index < m_size)
		Remove(index);
	else if (!Realloc(index + 1))
		return false;

	// Deslect this if we are switching it
	if (m_current == index)
		SelectNoWeapon();

	m_list[index] = weapon;
	weapon->SetOwner(&GetActor());

	// Do we have a selected weeapon?
	if (!InRange(m_current))
		Select(index);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Handle removing
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::Remove(
int index)
{
	if (!InRange(index))
		return false;

	// Anything here
	if (!m_list[index])
		return false;
	
	// Deselect if we are going away
	if (m_current == index)
		SelectNoWeapon();

	// Get rid of it!
	delete m_list[index];
	m_list[index] = NULL;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WeaponInventory::RemoveAll()
{
	SelectNoWeapon();
	Clear();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WeaponInventory::Reset()
{
	for (int w = 0; w < m_size; w++)
	{
		if (m_list[w])
			m_list[w]->Reset();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Do this once per frame
/////////////////////////////////////////////////////////////////////////////
void WeaponInventory::RunAnimation()
{
	Weapon *weapon = GetSelected();
	if (weapon)
		weapon->Update();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponInventory::Realloc(
int newSize)
{
	// Duh
	if (newSize <= m_size)
		return true;

	// going to large?
	if (newSize > m_maxSize)	
		return false;

	// Make the new array
	Weapon **newList = new Weapon*[newSize];
	ASSERT(newList);
	if (!newList)
		return false;

	// Copy and fill w/NULL
	for (int i = 0; i < newSize; i++)
	{
		if (i < m_size)
			newList[i] = m_list[i];
		else
			newList[i] = NULL;
	}

	// Reassign
	delete [] m_list;
	m_list = newList;
	m_size = newSize;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WeaponInventory::Free()
{
	Clear();
	delete [] m_list;
	m_list = NULL;
	m_size = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WeaponInventory::Clear()
{
	for (int i = 0; i < m_size; i++)
	{
		delete m_list[i];
		m_list[i] = NULL;
	}
}
