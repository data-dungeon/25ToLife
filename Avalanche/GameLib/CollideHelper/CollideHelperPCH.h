//============================================================================
//=
//= CollideHelperPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __COLLIDE_HELPER_PCH_H
#define __COLLIDE_HELPER_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// engine

#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Assert/Assert.h"

// gamelib

#include "platform/BaseType.h"
#include "Layers/Debug.h"
#include "collide/Cull.h"
#include "collide/Intersection.h"
#include "collide/LineToTerrain.h"
#include "collide/LineToObb.h"
#include "collide/LineToAabb.h"
#include "collide/collsys.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __COLLIDE_HELPER_PCH_H
