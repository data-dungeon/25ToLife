/////////////////////////////////////////////////////////////////////////////
// TTL Weapon inventory
/////////////////////////////////////////////////////////////////////////////
#ifndef TTLWEAPINV_H
#define TTLWEAPINV_H

#include "GameHelper/Message.h"
#include "Environ/Munition.h"
#include "Components/weapons/weapinv.h"

class Lobbed;

class TTLWeaponInventory : public WeaponInventory
{
public:
	TTLWeaponInventory(CCompActor &owner);
	virtual ~TTLWeaponInventory() {}

	// Component support
	const char *ComponentName(void)
		{ return "WeaponInventory"; }
	void Initialize();

	// Override these! (no selecting slot 3)
	bool SelectNext();
	bool SelectPrevious();
	bool SelectBest();

	// Override these! (no selecting slot 3)
	int GetNextIndex();
	int GetPreviousIndex();

	// Select a weapon (valid 0 - 2) (selecting 3 will FAIL)
	bool Select(int index);

	// Reset all the weapons
	void Reset();

	// Rearm all the weapons
	void ReArm();

	// Do this once per frame
	void RunAnimation();

	// Get the current lobbed weapon (left hand), could be NULL
	Lobbed *GetLobbed();

private:
	// Handle a weapon being fired
	void WeaponFired(const CMunitionFired &fire);

	// Message handling
	REF_SEND_MESSAGE_HANDLER(WeaponFireHandler, TTLWeaponInventory) m_weaponFireHandler;
};

#endif
