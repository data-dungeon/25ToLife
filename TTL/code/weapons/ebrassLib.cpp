/////////////////////////////////////////////////////////////////////////////
// Manage a list of weapons with a fast way of instantiating them.
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/ebrassLib.h"

// Our singleton
DeclareSingleton(EjectingBrassLibrary);

// Our array of creation functions
VarObjCreator<EjectingBrass> *VarObjLibrary<EjectingBrass>::m_creatorList = NULL;

// Our only creator right now
EjectingBrassCreator g_ejectingBrassCreator;
