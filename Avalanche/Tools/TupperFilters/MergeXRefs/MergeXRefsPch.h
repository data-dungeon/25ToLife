////////////////////////////////////////////////////////////////////////////
//
// MergeXRefsPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MergeXRefsPch.h $
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:36a
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 10  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * File Dependency checks
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/28/02    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Moved MergeXRefs from annotate filter to its own filter
 */

#ifndef __MERGE_XREFS_PCH__H
#define __MERGE_XREFS_PCH__H

#include <Math/Vector.h>
#include <Math/Matrix.h>

// defines for the attribute block headers
#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK	"END OBJECTATTRIBUTES"

#include <windows.h>
#include <stdio.h>
#include <assert.h>
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
#include <Tupper3d/triangleStrip.h>
#include <Tupper3d/derived.h>
#include <Tupper3d/derivedmodifier.h>
#include <Tupper3d/beziershape.h>
#include <Tupper3d/spline.h>
#include <Tupper3d/xref.h>
// node
#include <Tupper3d/node.h>
#include <Tupper3d/nodepool.h>
// transform
#include <Tupper3d/transform.h>
#include <Tupper3d/staticmatrix.h>
// material
#include <Tupper3d/material.h>
#include <Tupper3d/materialpool.h>
#include <Tupper3d/standardmaterial.h>
#include <Tupper3d/standardmaterialmap.h>
#include <Tupper3d/multisubobjectmaterial.h>
#include <Tupper3d/multisubobjectmaterialsubmaterial.h>
// map
#include <Tupper3d/map.h>
#include <Tupper3d/mappool.h>
#include <Tupper3d/bitmapmap.h>
// image
#include <Tupper3d/image.h>
#include <Tupper3d/imagepool.h>
// modifier
#include <Tupper3d/modifier.h>
#include <Tupper3d/modifierpool.h>
#include <Tupper3d/applyvcmodifier.h>
#include <Tupper3d/vertexpaintmodifier.h>
#include <Tupper3d/vertexpaintmodifiercontext.h>
#include <Tupper3d/holdvcmodifier.h>
#include <Tupper3d/skinmodifier.h>
#include <Tupper3d/skinmodifierbone.h>
#include <Tupper3d/skinmodifiercontext.h>
#include <Tupper3d/skinmodifiercontextvertex.h>
#include <Tupper3d/skinmodifiercontextvertexpool.h>
// image data (for image reader)
#include <Tupper3d/imageData.h>
#include <Tupper3d/imageDataFrame.h>
#include <Tupper3d/imageDataMap.h>
#include <Tupper3d/imageDataPalette.h>
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
// DBLUtils library
#include <TupperFilters/Libraries/DBLUtils/DBLUtil.h>
#include <TupperFilters/Libraries/DBLUtils/DBLWrite.h>
#include <TupperFilters/Libraries/DBLUtils/DBLSwap.h>
// DBL game headers
#include <Platform/BaseType.h>
#include <Display/Common/TextureState.h>
#include <Display/Common/RenderState.h>
#include <Game/Animation/Bone.h>
#include <Game/Database/World.h>
#include <Game/Database/Instance.h>
#include <Game/Database/LOD.h>
#include <Game/Database/DBFile.h>
#include <Game/Database/SceneNode.h>
#include <Game/Misc/Bounds.h>
#include <Game/Managers/ParticleDB.h>

#endif
