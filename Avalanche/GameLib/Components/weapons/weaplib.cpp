/////////////////////////////////////////////////////////////////////////////
// Manage a list of weapons with a fast way of instantiating them.
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"
#include "Components/weapons/weaplib.h"
#include "Components/weapons/projlib.h"
#include "Components/weapons/sightlib.h"
#include "Components/weapons/weapdep.h"

// Our singleton
DeclareSingleton(WeaponLibrary);

// Our array of creation functions
VarObjCreator<Weapon> *VarObjLibrary<Weapon>::m_creatorList = NULL;

#define WEAPON_VAR_LIB "var/weapons/weapons.lib"

/////////////////////////////////////////////////////////////////////////////
// ALWAYS USE THESE TO MANAGE THE SINGLETON
/////////////////////////////////////////////////////////////////////////////
bool WeaponLibrary::Load()
{
	if (ValidSingleton(WeaponLibrary))
		return true;

	NewSingleton(WeaponLibrary);
	if (!ValidSingleton(WeaponLibrary))
		return false;

	// Load the weapon var lib for fast loading
	g_console.LoadLibrary( WEAPON_VAR_LIB );

	// Wee!
	WeaponDependency::InitializeAllOnce(WeaponDependency::PREINIT);

	// Create the projectile library
	ASSERT(!g_projectileLibPtr);
	g_projectileLibPtr = new ProjectileLibrary;
	ASSERT(g_projectileLibPtr);
	g_projectileLibPtr->InitializeOnce();

	// Create the aimsight library
	ASSERT(!g_aimSightLibPtr);
	g_aimSightLibPtr = new AimSightLibrary;
	ASSERT(g_aimSightLibPtr);
	g_aimSightLibPtr->InitializeOnce();

	// Setup the aiming stuff
	NewSingleton(AimingSimProps);

	// Initialize ourselves now
	g_weaponLib.InitializeOnce();

	// Wee!
	WeaponDependency::InitializeAllOnce(WeaponDependency::POSTINIT);

	// Unload lib
	g_console.UnloadLibrary( WEAPON_VAR_LIB );

	// Set it up!
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponLibrary::Unload()
{
	if (!ValidSingleton(WeaponLibrary))
		return false;

	// Wee!
	WeaponDependency::TerminateAllOnce(WeaponDependency::POSTINIT);

	// Setup the aiming stuff
	DeleteSingleton(AimingSimProps);

	// Get ride of the sights
	if (g_aimSightLibPtr)
	{
		g_aimSightLibPtr->TerminateOnce();
		delete g_aimSightLibPtr;
		g_aimSightLibPtr = NULL;
	}

	// Get rid of the projectiles
	if (g_projectileLibPtr)
	{
		g_projectileLibPtr->TerminateOnce();
		delete g_projectileLibPtr;
		g_projectileLibPtr = NULL;
	}

	// Wee!
	WeaponDependency::TerminateAllOnce(WeaponDependency::PREINIT);

	DeleteSingleton(WeaponLibrary);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Use this around a game
/////////////////////////////////////////////////////////////////////////////
bool WeaponLibrary::PrepareForCombat()
{
	// Wee!
	WeaponDependency::InitializeAllEvery(WeaponDependency::PREINIT);

	// Let them be lazy
	if (!ValidSingleton(WeaponLibrary))
		WeaponLibrary::Load();
	if (!ValidSingleton(WeaponLibrary))
		return false;
	
	// Do all the every's
	g_projectileLib.InitializeEvery();

	// Create the aimsight library
	g_aimSightLib.InitializeEvery();

	// Initialize ourselves now
	g_weaponLib.InitializeEvery();

	// Wee!
	WeaponDependency::InitializeAllEvery(WeaponDependency::POSTINIT);
	
	// We are ready for combat
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponLibrary::DoneCombat()
{
	// Wee!
	WeaponDependency::TerminateAllEvery(WeaponDependency::POSTINIT);

	// Ooops
	if (!ValidSingleton(WeaponLibrary))
		return false;
	
	// Do all the every's
	g_projectileLib.TerminateEvery();

	// Create the aimsight library
	g_aimSightLib.TerminateEvery();

	// Initialize ourselves now
	g_weaponLib.TerminateEvery();
	
	// Wee!
	WeaponDependency::TerminateAllEvery(WeaponDependency::PREINIT);

	// We are clean
	return true;
}

