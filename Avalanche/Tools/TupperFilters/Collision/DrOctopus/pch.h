////////////////////////////////////////////////////////////////////////////
//
// DrOctopusPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////
#ifndef DROCTOPUS_PCH_H
#define DROCTOPUS_PCH_H

// defines for the attribute block headers
#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK	"END OBJECTATTRIBUTES"

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define DISABLE_ASSERTS
#include <Layers/Assert/Assert.h>
// Don't use engine asserts
#undef ASSERT
#undef ASSERTS
#undef ASSERTF
#define ASSERT assert
#define ASSERTS(a, s) assert(a)
#define ASSERTF(a, f) assert(a)

#include <Math/Vector.h>
#include <Math/Matrix.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <Tupperware/keeper.h>

// TupperAttrib
#include <tupperattrib/keyvalue.h>
#include <tupperattrib/keyvaluecontext.h>
#include <tupperattrib/keyselection.h>
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
#include <TupperFilters/Libraries/Plugins/AnnotateHelper.h>
#include <TupperFilters/Libraries/Plugins/FaceCollision.h>


// Game headers
#include <Platform/BaseType.h>

#include <collide/xzrect.h>
#include <collide/tsector.h>
#include <collide/tritest.h>
#include <collide/cpsys.h>
#include <collide/cdbhdr.h>

#include <geomutil/chord.h>

#include <toolhelper/dynstr.h>
#include <toolhelper/writepad.h>
#include <container/hashlist.h>
#include <container/dynlist.h>

// DrOctopus global headers
#include "collinst.h"
#include "stats.h"
#include "droctopus.h"
#include "trisoup.h"
#include "potfactory.h"
#include "factoryot.h"

#endif
