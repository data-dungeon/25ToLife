////////////////////////////////////////////////////////////////////////////
//
// OBBWorldPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Pch.h $
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:44a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 13  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 12  *****************
 * User: Food         Date: 3/12/03    Time: 2:04p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 11:33a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * new collision attribute system (using image names)
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * File Dependency checks
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 7/23/02    Time: 2:40p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Keeper usage
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 9:31a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * changed name of array and string files
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Added support for attrib tree
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * hybrid configuration
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 3:53p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * First Pass of Attributes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/14/02    Time: 9:49a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 */

#ifndef _OBB_WORLD_PCH_H
#define _OBB_WORLD_PCH_H

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <Math/Vector.h>
#include <Math/Matrix.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/tupstring.h>
#include <tupperware/tuparray.h>
#include <Tupperware/keeper.h>
// TupperAttrib
#include <tupperattrib/configroot.h>
// scene
#include <Tupper3d/scene.h>
// object
#include <Tupper3d/object.h>
#include <Tupper3d/objectpool.h>
#include <Tupper3d/trimesh.h>
#include <Tupper3d/trimeshmap.h>
// node
#include <Tupper3d/node.h>
#include <Tupper3d/nodepool.h>
// transform
#include <Tupper3d/transform.h>
#include <Tupper3d/staticmatrix.h>
// modifier
#include <Tupper3d/modifierpool.h>
#include <Tupper3d/faceattributesmodifiercontext.h>
// material
#include <Tupper3d/materialpool.h>
// map
#include <Tupper3d/mappool.h>
// image
#include <Tupper3d/imagepool.h>
// 
// plugin library
#include <TupperFilters/Libraries/Plugins/FilterPluginDesc.h>
#include <TupperFilters/Libraries/Plugins/FilterPlugin.h>
#include <TupperFilters/Libraries/Plugins/Options.h>
#include <TupperFilters/Libraries/Plugins/DataManager.h>
#include <TupperFilters/Libraries/Plugins/filepath.h>
#include <TupperFilters/Libraries/Plugins/filepatharray.h>
#include <TupperFilters/Libraries/Plugins/PluginError.h>
#include <TupperFilters/Libraries/Plugins/PluginLog.h>
#include <TupperFilters/Libraries/Plugins/ImageManager.h>
#include <TupperFilters/Libraries/Plugins/bounding.h>
#include <TupperFilters/Libraries/Plugins/annotatehelper.h>
#include <TupperFilters/Libraries/Plugins/FaceCollision.h>

// TupperAttrib
#include <tupperattrib/configroot.h>
#include <tupperattrib/keyvalue.h>
#include <tupperattrib/keyvaluecontext.h>

// Game headers
#include <Platform/BaseType.h>

#include <collide/xzrect.h>
#include <collide/obb.h>

#include <toolhelper/dynstr.h>
#include <toolhelper/vistool.h>

#include <container/dynlist.h>

// OBBWorld global headers
#include "stats.h"

#endif
