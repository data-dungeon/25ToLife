/* this module is for determining the appropriated weapon, when to use what, it will use the regular weapon inventory
to manage the inventory.  So, this is where the bot will look at his status, enemy status, inventory, and choose to throw a nade, etc.*/

#ifndef AI_INVENTORY_H
#define AI_INVENTORY_H

#include "Components/Component.h"

class AiInventory : public CActorComponent
{
public:
	AiInventory( CCompActor &i_actor );
	~AiInventory();
	const char *ComponentName( void ) { return "AiInventory"; }
	void Initialize(void);

	int GetWeaponSelected(void);
	int GetWeaponSelectedType(void);
	bool AttemptToSwitchWeaponType(int weaponType);
	void FireSelectedWeapon(void);

	typedef enum
	{
		WEAPON_SHORT_RANGE, //pistols, rifles, etc.
		WEAPON_LONG_RANGE,	//sniper rifles (things you will want to switch from if close combat)
		WEAPON_GERNADE,		//pipe bomb
		WEAPON_SUPRESSOR,	//flash bangs
		WEAPON_NON_LETHAL	//tasers
	} WEAPON_TYPES;

private:

};

#endif // AI_INVENTORY_H
