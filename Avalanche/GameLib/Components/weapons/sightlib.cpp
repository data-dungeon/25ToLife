/////////////////////////////////////////////////////////////////////////////
// Manage a list of sight types with a fast way of instantiating them.
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"
#include "Components/weapons/sightlib.h"

// Our singleton
AimSightLibrary *g_aimSightLibPtr = NULL;

// Our array of creation functions
VarObjCreator<AimSight>	*VarObjLibrary<AimSight>::m_creatorList = NULL;


