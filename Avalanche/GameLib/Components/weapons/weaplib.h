/////////////////////////////////////////////////////////////////////////////
// Manage a list of weapon types with a fast way of instantiating them.
/////////////////////////////////////////////////////////////////////////////
#ifndef WEAPONLIB_H
#define WEAPONLIB_H

#include "Components/weapons/volib.h"
#include "Components/weapons/weapon.h"
#include "EngineHelper/Singleton.h"

// This class needs to be subclassed in order to create a Weapon
class WeaponCreator : public VarObjCreator<Weapon>
{
public:
	// Hello
	WeaponCreator(const char *type) :
		VarObjCreator<Weapon>(type) {}

	// You must provide this for the type
	virtual Weapon *CreateInstance(const VarObjProperties<Weapon> &properties) = 0;

protected:
	// Subclass this to provide new
	virtual WeaponProperties *NewProperties(int index) = 0;
};

// The manager of acive Weapons
class WeaponLibrary : public VarObjLibrary<Weapon>
{
public:
	// Initialize all the known Weapons
	WeaponLibrary() {}
	~WeaponLibrary() {}

	// ALWAYS USE THESE TO MANAGE THE SINGLETON
	static bool Load();
	static bool Unload();

	// Use this around a game
	static bool PrepareForCombat();
	static bool DoneCombat();

protected:
	// What is the path
	const char *Path()
		{ return "weapons\\"; }

	// What is the context name
	const char *Context()
		{ return "Weapon"; }
};

// Weapon singleton
DefineSingleton(WeaponLibrary)
#define g_weaponLib GetSingleton(WeaponLibrary)

#endif
