//
// NavigatePCH_SN.cpp
//
// Faster compiles for SN, since we don't have precompiled headers.
//

#include "NavigatePCH.h"

#if defined(PS2) && defined(SN)

#include "BackAndForthPath.cpp"
#include "CircuitPath.cpp"
#include "OneWayPath.cpp"
#include "NavAgents.cpp"
#include "NavigateScriptFunc.cpp"
#include "NavIterators.cpp"
#include "NavManager.cpp"
#include "NavMesh.cpp"
#include "NavPath.cpp"
#include "NavTree.cpp"

#endif // defined(PS2) && defined(SN)
