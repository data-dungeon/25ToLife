////////////////////////////////////////////////////////////////////////////
//
// MakeYUpPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MakeYUpPch.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:05p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * convert over to tupper3d coordinate system classes
 * keep old code just in case
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:35a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 6/10/02    Time: 9:26a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * transform bezier shapes
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/29/02    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Transform skin modifier
 * Changed to use node pools
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Added Node Pool and Node Referencing
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * skeleton filter chain complete
 */

#ifndef __MAKE_YUP_PCH__H
#define __MAKE_YUP_PCH__H

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <tupperware/aggregate.h>

#include <Tupper3d/scene.h>
#include <Tupper3d/nodepool.h>
#include <Tupper3d/objectpool.h>
#include <Tupper3d/modifierpool.h>
#include <Tupper3d/CoordinateSystemConverter.h>

#include <Tupper3d/node.h>
#include <Tupper3d/transform.h>
#include <Tupper3d/staticmatrix.h>
#include <Tupper3d/object.h>
#include <Tupper3d/trimesh.h>
#include <Tupper3d/beziershape.h>
#include <Tupper3d/spline.h>
#include <Tupper3d/derived.h>
#include <Tupper3d/derivedmodifier.h>
#include <Tupper3d/modifier.h>
#include <Tupper3d/skinmodifier.h>
#include <Tupper3d/skinmodifierbone.h>
#include <Tupper3d/skinmodifiercontext.h>

#include "TupperFilters/Libraries/Plugins/FilterPluginDesc.h"
#include "TupperFilters/Libraries/Plugins/FilterPlugin.h"
#include "TupperFilters/Libraries/Plugins/Options.h"
#include "TupperFilters/Libraries/Plugins/DataManager.h"
#include "TupperFilters/Libraries/Plugins/PluginError.h"
#include "TupperFilters/Libraries/Plugins/PluginLog.h"

#include <Math/Matrix.h>

#endif
