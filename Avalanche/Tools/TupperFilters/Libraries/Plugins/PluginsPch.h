////////////////////////////////////////////////////////////////////////////
//
// PluginsPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_PLUGINS_PCH_H
#define TUPPERFILTERS_LIB_PLUGINS_PLUGINS_PCH_H

#pragma warning(disable:4786)

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include <Math/Vector.h>
#include <Math/Matrix.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/tuparray.h>
#include <tupperware/tupstring.h>
#include <tupperware/keeper.h>

// TupperAttrib
#include <tupperattrib/keyvalue.h>
#include <tupperattrib/keyselection.h>
#include <tupperattrib/keyvaluecontext.h>
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
#include <Tupper3d/blendmaterial.h>
#include <Tupper3d/compositematerial.h>
#include <Tupper3d/compositematerialsubmaterial.h>
#include <Tupper3d/doublesidedmaterial.h>
// map
#include <Tupper3d/map.h>
#include <Tupper3d/mappool.h>
#include <Tupper3d/bitmapmap.h>
#include <Tupper3d/compositemap.h>
#include <Tupper3d/compositemapsubmap.h>
#include <Tupper3d/mixmap.h>
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
#include <Tupper3d/faceattributesmodifier.h>
#include <Tupper3d/faceattributesmodifiercontext.h>
#include <Tupper3d/skinmodifiercontext.h>
#include <Tupper3d/skinmodifiercontextvertexpool.h>
#include <Tupper3d/clothmodifier.h>
#include <Tupper3d/clothmodifiercontext.h>
// image data (for image reader)
#include <Tupper3d/imageData.h>
#include <Tupper3d/imageDataFrame.h>
#include <Tupper3d/imageDataMap.h>
#include <Tupper3d/imageDataPalette.h>

#include <Platform/CompilerControl.h>
#include <Platform/BaseType.h>
#include <Game/Database/SOM.h>
#include <Display/Common/RenderState.h>
#include <Display/Common/TextureState.h>

#endif
