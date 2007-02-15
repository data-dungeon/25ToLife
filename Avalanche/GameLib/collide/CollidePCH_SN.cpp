//============================================================================
//=
//= CollidePCH_SN.cpp - Precompiled header CPP
//=
//============================================================================

// codewarrior flag
#ifdef CW
#define CW_PCH
#endif

#include "CollidePCH.h"

#if defined(PS2) && defined(SN)

#include "AabbToAabb.cpp"
#include "AabbToObb.cpp"
#include "AabbToTerrain.cpp"
#include "collsys.cpp"
#include "cube.cpp"
#include "EntityList.cpp"
#include "Event.cpp"
#include "Line.cpp"
#include "LineToAabb.cpp"
#include "LineToObb.cpp"
#include "LineToSphere.cpp"
#include "LineToTerrain.cpp"
#include "obb.cpp"
#include "ObbToObb.cpp"
#include "ObbToTerrain.cpp"
#include "ObjectLayer.cpp"
#include "ObjectQT.cpp"
#include "octtree.cpp"
#include "otnode.cpp"
#include "QueryEntity.cpp"
#include "SphereToObb.cpp"
#include "SphereToTerrain.cpp"
#include "Stack.cpp"
#include "StaticEntity.cpp"
#include "tclayer.cpp"
#include "tcllist.cpp"
#include "Terrain.cpp"
#include "terraintri.cpp"
#include "tpdb.cpp"
#include "tperim.cpp"
#include "TriangleList.cpp"
#include "tritest.cpp"
#include "tsuspect.cpp"
#include "xzrect.cpp"

#endif // defined(PS2) && defined(SN)
