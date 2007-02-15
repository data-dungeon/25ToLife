////////////////////////////////////////////////////////////////////////////
//
// TupExportPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupImportPch.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */

#ifndef __TUP_EXPORT_PCH__H
#define __TUP_EXPORT_PCH__H

#pragma warning (disable: 4786)

#include <stdio.h>
#include <direct.h>
#include <string>
#include <vector>

// Max Includes
#include <Max.h>
#include <meshdlib.h>
#include <modstack.h>
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <stdmat.h>
#include <shaders.h>
#include <Simpobj.h>
#include <iskin.h>

// Tupperware
#include <tupperware/aggregate.h>
#include <tupperware/loadsave.h>
#include <tupperware/tuparray.h>
#include <tupperware/tupstring.h>

// Tupper3d
#include <tupper3d/keeper.h>

#include <tupper3d/scene.h>

#include <tupper3d/node.h>
#include <tupper3d/nodepool.h>

#include <tupper3d/transform.h>
#include <tupper3d/staticmatrix.h>

#include <tupper3d/object.h>
#include <tupper3d/objectpool.h>
#include <tupper3d/mesh.h>
#include <tupper3d/meshmap.h>
#include <tupper3d/camera.h>
#include <tupper3d/light.h>
#include <tupper3d/xref.h>
#include <tupper3d/derived.h>
#include <tupper3d/derivedmodifier.h>
#include <tupper3d/beziershape.h>
#include <tupper3d/spline.h>
#include <tupper3d/bone.h>

#include <tupper3d/modifier.h>
#include <tupper3d/modifierpool.h>
#include <tupper3d/applyvcmodifier.h>
#include <tupper3d/vertexpaintmodifier.h>
#include <tupper3d/vertexpaintmodifiercontext.h>
#include <tupper3d/holdvcmodifier.h>
#include <tupper3d/faceattributesmodifier.h>
#include <tupper3d/faceattributesmodifiercontext.h>
#include <tupper3d/skinmodifier.h>
#include <tupper3d/skinmodifierbone.h>
#include <tupper3d/skinmodifiercontext.h>
#include <tupper3d/skinmodifiercontextvertexpool.h>
#include <tupper3d/skinmodifiercontextvertex.h>
#include <tupper3d/clothmodifier.h>
#include <tupper3d/clothmodifiercontext.h>
#include <tupper3d/unknownmodifier.h>

#include <tupper3d/material.h>
#include <tupper3d/materialpool.h>
#include <tupper3d/standardmaterial.h>
#include <tupper3d/standardmaterialmap.h>
#include <tupper3d/multisubobjectmaterial.h>
#include <tupper3d/multisubobjectmaterialsubmaterial.h>
#include <tupper3d/blendmaterial.h>
#include <tupper3d/compositematerial.h>
#include <tupper3d/compositematerialsubmaterial.h>
#include <tupper3d/doublesidedmaterial.h>

#include <tupper3d/map.h>
#include <tupper3d/mappool.h>
#include <tupper3d/bitmapmap.h>
#include <tupper3d/compositemap.h>
#include <tupper3d/compositemapsubmap.h>
#include <tupper3d/mixmap.h>

#include <tupper3d/image.h>
#include <tupper3d/imagepool.h>

// Math
#include <Math/Matrix.h>

extern const char *g_pImporterDate;
extern const char *g_pImporterTime;

#endif
