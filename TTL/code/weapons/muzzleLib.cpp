/////////////////////////////////////////////////////////////////////////////
// Manage a list of weapons with a fast way of instantiating them.
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/muzzleLib.h"

// Our singleton
DeclareSingleton(MuzzleFlashLibrary);

// Our array of creation functions
VarObjCreator<MuzzleFlash> *VarObjLibrary<MuzzleFlash>::m_creatorList = NULL;

// Our only creator right now
MuzzleFlashCreator g_muzzleFlashCreator;
