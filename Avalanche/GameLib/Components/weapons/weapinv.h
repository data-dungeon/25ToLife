/////////////////////////////////////////////////////////////////////////////
// Weapon inventory
/////////////////////////////////////////////////////////////////////////////
#ifndef WEAPINV_H
#define WEAPINV_H

#include "Components/Component.h"
#include "container/dynlist.h"
#include "Components/weapons/weapon.h"

class WeaponInventory : public CActorComponent
{
public:
	WeaponInventory(CCompActor &owner);
	virtual ~WeaponInventory();

	// Component support
	virtual const char *ComponentName(void)
		{ return "WeaponInventory"; }
	virtual void Initialize(void);

	// Set the maximum number of weapons. If this returns false
	// there are weapons beyond maxWeapons in the list.
	virtual bool SetMaxWeapons(int maxWeapons);

	// Manage the selection
	virtual Weapon *GetSelected();
	int GetSelectedIndex(); // -1 for no selected
	virtual bool SelectNext();
	virtual bool SelectPrevious();
	bool SelectNoWeapon();
	virtual bool Select(int index);
	bool Select(const char *name);
	Weapon *Find(const char *name);

	// Add a weapon
	bool Append(Weapon *weapon);
	bool Add(Weapon *weapon, int index);

	// Handle removing
	bool Remove(int index);
	void RemoveAll();

	// Direct management
	int Count()
		{ return m_size; }
	Weapon *Get(int index)
		{ return InRange(index) ? m_list[index] : NULL; }
	const char *GetName(int index)
		{ return InRange(index) ? m_list[index]->Name() : NULL; }
	const char *GetDisplayName(int index)
		{ return InRange(index) ? m_list[index]->DisplayName() : NULL; }
	int GetSelectedSlot()
		{ return m_current; }

	// Reset all the weapons
	virtual void Reset();

	// Do this once per frame
	virtual void RunAnimation();

private:
	// the array of weapons
	Weapon								**m_list;
	int									m_size;
	int									m_maxSize;
	bool Realloc(int newSize);
	void Free();
	void Clear();

	// Currently selected weapon
	int									m_current;
	inline bool InRange(int index)
		{ return (index >= 0 && index < m_size); }
};

#endif
