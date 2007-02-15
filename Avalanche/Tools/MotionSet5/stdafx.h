// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <iostream>
#include <tchar.h>
#include <math.h>
#include <stdarg.h>
#include <conio.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/TupArray.h>
#include <tupperware/TupString.h>
#include <tupperware/TupSort.h>
#include <Tupperware/keeper.h>

// within this project
#include "GenMath.h"
#include "hashtime.h"
#include "HashVector2.h"
#include "HashVector3.h"
#include "HashMatrix3x3.h"
#include "bezier2.h"

#include "Exception.h"

// Tupper3d
#include <tupper3d/scene.h>
#include <tupper3d/node.h>
#include <tupper3d/nodepool.h>
#include <tupper3d/transform.h>
#include <tupper3d/staticmatrix.h>
#include <tupper3d/object.h>
#include <tupper3d/objectpool.h>
#include <tupper3d/AnimCurve.h>
#include <tupper3d/AnimCurvePool.h>
#include <tupper3d/AnimSetPool.h>
#include <tupper3d/AnimSet.h>
#include <tupper3d/AnimChannelPool.h>
#include <tupper3d/AnimChannel.h>
#include <tupper3d/AnimClipPool.h>
#include <tupper3d/AnimInstancedClip.h>
#include <tupper3d/AnimSourceClip.h>
#include <tupper3d/AnimClipBlendPool.h>
#include <tupper3d/AnimClipBlend.h>
#include <tupper3d/AnimGlobals.h>


//#define INTENSE_OUTPUT

extern const char *g_pMotionSet5Date;
extern const char *g_pMotionSet5Time;
extern const char *g_pMotionSet5Version;
