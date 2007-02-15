//============================================================================
//=
//= CollidePCH.h - Precompiled headers
//=
//============================================================================

#ifndef __COLLIDE_PCH_H
#define __COLLIDE_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif

// engine

#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/LayersStruct.h"
#include "Layers/Memory.h"
#include "Layers/Assert/Assert.h"
#include "Game/Database/DBFile.h"

// gamelib

#include "platform/BaseType.h"
#include "EngineHelper/drawutil.h"
#include "geomUtil/planefit.h"
#include "stage/StageEntity.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

#include "collide/StackArray.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __COLLIDE_PCH_H
