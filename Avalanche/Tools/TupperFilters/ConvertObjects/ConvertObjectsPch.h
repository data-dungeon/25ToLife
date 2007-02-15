////////////////////////////////////////////////////////////////////////////
//
// ConvertObjectsPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ConvertObjectsPch.h $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 7/29/03    Time: 2:58p
 * Created in $/Avalanche/tools/TupperFilters/ConvertObjects
 */

#ifndef __CONVERT_OBJECTS_PCH__H
#define __CONVERT_OBJECTS_PCH__H

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <tupperware/aggregate.h>
#include <tupperware/scalar.h>

#include <Tupper3d/scene.h>
#include <Tupper3d/objectpool.h>
#include <Tupper3d/modifierpool.h>
#include <Tupper3d/Object.h>
#include <Tupper3d/ObjectConverter.h>
#include <Tupper3d/PolyMesh.h>
#include <Tupper3d/TriMesh.h>
#include <Tupper3d/TupVertex.h>

#include "TupperFilters/Libraries/Plugins/FilterPluginDesc.h"
#include "TupperFilters/Libraries/Plugins/FilterPlugin.h"
#include "TupperFilters/Libraries/Plugins/Options.h"
#include "TupperFilters/Libraries/Plugins/DataManager.h"
#include "TupperFilters/Libraries/Plugins/PluginError.h"
#include "TupperFilters/Libraries/Plugins/PluginLog.h"

#endif
