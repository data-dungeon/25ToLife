/////////////////////////////////////////////////////////////////////////////
// Manage a list of projectile types with a fast way of instantiating them.
// Please note, at this point in the data desingn a projectile is NOT AN
// ACTOR, rather a description of functionality that may eventually spawn
// actors.
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"
#include "Components/weapons/projlib.h"

// Our singleton
ProjectileLibrary *g_projectileLibPtr = NULL;

// Our array of creation functions
VarObjCreator<Projectile>	*VarObjLibrary<Projectile>::m_creatorList = NULL;
