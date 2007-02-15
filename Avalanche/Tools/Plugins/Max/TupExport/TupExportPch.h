////////////////////////////////////////////////////////////////////////////
//
// TupExportPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupExportPch.h $
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 12:19p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * move tupmesh to tuptrimesh
 * made it compile with changes made to tupper3d in the last year
 * 
 * *****************  Version 27  *****************
 * User: Adam Clayton Date: 8/13/02    Time: 3:19p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * added mix map
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 7/22/02    Time: 9:36a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Cloth Export
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 9:49a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Removed xref update options
 * Added exporter data & time as well as database version to export
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 5/20/02    Time: 9:00a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Fix for tupperware change. Fix for Face attribute change
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:07a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:55p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * add bones & skin
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 4/18/02    Time: 2:11p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Bone Export Complete
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 3/29/02    Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Rough in of bezier shapes stuff
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:20p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * First pass of Face Attributes
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 3/19/02    Time: 4:35p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Face Attributes Phase 1
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 3/06/02    Time: 4:43p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Hold VC Modifier Export Support
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 2:04p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Relative Path Support for Images
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:46a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Added Node Pool and Node Referencing
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 1/04/02    Time: 1:23p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Vertex Paint Modifier
 * Unknown Modifier
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 1/03/02    Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Vertex Paint Modifier (pass 1)
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 1/02/02    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Apply VC Modifier complete
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:33p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs & derived objects (skeleton)
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 12/20/01   Time: 11:10a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Cameras
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 4:08p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Dialog Box working (doesn't do modifiers or xrefs yet)
 * Put transform back in (opps)
 * Export selected working
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 11:34a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Materials, Maps complete - saving all information (except for plugin
 * shaders)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/17/01   Time: 11:07a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Materials & maps exporting (others still to do)
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:39p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Start of standard material exporting
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 3:08p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Use the loadsavemanager
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:50p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Full use of tupper3d for exporting
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Level A  complete
 */

#ifndef __TUP_EXPORT_PCH__H
#define __TUP_EXPORT_PCH__H

#pragma warning (disable: 4786)

#include <stdio.h>
#include <direct.h>
#include <string>
#include <vector>
#include <float.h>

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
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/tuparray.h>
#include <tupperware/tupstring.h>
#include <tupperware/keeper.h>

// Tupper3d

#include <tupper3d/scene.h>

#include <tupper3d/node.h>
#include <tupper3d/nodepool.h>

#include <tupper3d/transform.h>
#include <tupper3d/staticmatrix.h>

#include <tupper3d/object.h>
#include <tupper3d/objectpool.h>
#include <tupper3d/trimesh.h>
#include <tupper3d/trimeshmap.h>
#include <tupper3d/camera.h>
#include <tupper3d/light.h>
#include <tupper3d/xref.h>
#include <tupper3d/derived.h>
#include <tupper3d/derivedmodifier.h>
#include <tupper3d/beziershape.h>
#include <tupper3d/bezierspline.h>
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

extern const char *g_pExporterDate;
extern const char *g_pExporterTime;

#endif
