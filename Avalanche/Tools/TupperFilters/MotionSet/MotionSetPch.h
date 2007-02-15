////////////////////////////////////////////////////////////////////////////
//
// MotionSetPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MOTION_SET_PCH__H
#define __MOTION_SET_PCH__H

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

// plugin library
#include <TupperFilters/Libraries/Plugins/FilterPluginDesc.h>
#include <TupperFilters/Libraries/Plugins/FilterPlugin.h>
#include <TupperFilters/Libraries/Plugins/Options.h>
#include <TupperFilters/Libraries/Plugins/DataManager.h>
#include <TupperFilters/Libraries/Plugins/filepath.h>
#include <TupperFilters/Libraries/Plugins/filepatharray.h>
#include <TupperFilters/Libraries/Plugins/PluginError.h>
#include <TupperFilters/Libraries/Plugins/PluginLog.h>
#include <TupperFilters/Libraries/Plugins/TokenizeString.h>

// within this project
#include "GenMath.h"
#include "hashtime.h"
#include "HashVector2.h"
#include "HashVector3.h"
#include "HashMatrix3x3.h"
#include "bezier2.h"

extern const char *g_pMotionSetDate;
extern const char *g_pMotionSetTime;
extern const char *g_pMotionSetVersion;

#endif
