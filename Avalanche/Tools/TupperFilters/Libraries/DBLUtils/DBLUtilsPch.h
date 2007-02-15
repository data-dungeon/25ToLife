////////////////////////////////////////////////////////////////////////////
//
// DBLUtilsPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLUtilsPch.h $
 * 
 * *****************  Version 8  *****************
 * User: Dwain Skinner Date: 2/26/03    Time: 7:10p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 7  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * DBL Debug Code
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 3:58p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added Mesh Som classes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * hybrid configuration
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added library for handling the dbl file format (taken from Import
 * Export)
 */

#ifndef __DBL_UTILS_PCH__H
#define __DBL_UTILS_PCH__H

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <Platform/BaseType.h>
#include <Game/Animation/Bone.h>
#include <Game/Database/World.h>
#include <Game/Database/Instance.h>
#include <Game/Database/LOD.h>
#include <Game/Database/DBFile.h>
#include <Game/Database/SceneNode.h>
#include <Game/Misc/Bounds.h>
#include <Game/Managers/ParticleDB.h>
#include <Game/Database/MeshSOM.h>
#include <Game/Database/VisRegion.h>
#include <Game/Database/LightSet.h>
#include "TupperFilters/Libraries/Plugins/PluginError.h"
#include "TupperFilters/Libraries/Plugins/PluginLog.h"

#include "Tupperware/TupArray.h"
#include "Tupperware/TupString.h"


#endif
