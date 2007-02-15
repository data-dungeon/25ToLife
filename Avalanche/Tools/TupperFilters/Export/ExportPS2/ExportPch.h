////////////////////////////////////////////////////////////////////////////
//
// ExportPCPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportPch.h $
 * 
 * *****************  Version 42  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:26a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 41  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 10:39a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * moved mesh connectivity to tupper3d
 * 
 * *****************  Version 40  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 2:57p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 39  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 38  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 37  *****************
 * User: Dwain Skinner Date: 2/26/03    Time: 6:15p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 36  *****************
 * User: Dwain Skinner Date: 2/26/03    Time: 11:01a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 35  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 3:59p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * added stage faces helper class
 * 
 * *****************  Version 34  *****************
 * User: Adam Clayton Date: 10/02/02   Time: 9:33a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * changed uv transform code
 * 
 * *****************  Version 33  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 10:04a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * moved build texture contexts into plugin library
 * 
 * *****************  Version 32  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Multi-texturing first pass complete
 * 
 * *****************  Version 31  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 30  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 2:43p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * added composite maps
 * 
 * *****************  Version 29  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 2:08p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 10:32a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 27  *****************
 * User: Adam Clayton Date: 7/09/02    Time: 3:51p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth SOM part 2
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 4:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth SOM start
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 7/01/02    Time: 2:45p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * pragma warning disable
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 6/03/02    Time: 3:28p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * First Pass at bone export
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 5/10/02    Time: 2:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * PS2_EXPORT define changed to EXPORT_PS2 
 * LIghting checked on meshes
 * can handle node with no material
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 5/06/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * DMA Som Code put in
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * added test for include in ps2 export
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 9:10a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Environmental instances are not added to scene tree
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * First pass of instance flags accessing node attribute data
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:32p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Surface Type strings now read from node attribute tree
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/03/02    Time: 2:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Paths Exporting
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * hybrid configuration
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:56p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed to use Annotate filter
 * Added boundign and annotate helper references
 * Removed export pass
 * Added Dependencies
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/06/02    Time: 4:44p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Vertex Color Support (hold vc modifier too)
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:57p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Apply Vertex Color Modifiers to Derived objects
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:26p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Added TriStrip header
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Added Node Pool and Node Referencing
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/18/02    Time: 1:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixes for Juju
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/14/02    Time: 2:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exporting 32 bit textures.
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Start of Tupperchain filter for the PC 
 * Currently got as far to output the SOM data (should not work yet)
 */

#ifndef __EXPORT_PCH__H
#define __EXPORT_PCH__H

#ifdef _HYBRID
#define ASSERT assert
#endif

#include <Math/Vector.h>
#include <Math/Matrix.h>

// defines for the attribute block headers
#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK	"END OBJECTATTRIBUTES"

#pragma warning (disable:4786)

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <io.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/TupArray.h>
#include <tupperware/TupString.h>
#include <tupperware/TupSort.h>
#include <Tupperware/keeper.h>
// TupperAttrib
#include <tupperattrib/keyvalue.h>
#include <tupperattrib/keyvaluecontext.h>
#include <tupperattrib/keyselection.h>
#include <tupperattrib/configroot.h>
#include <tupperattrib/configchecklist.h>
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
#include <Tupper3d/bezierspline.h>
#include <Tupper3d/splineshape.h>
#include <Tupper3d/spline.h>
#include <Tupper3d/light.h>

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
#include <Tupper3d/compositemap.h>
#include <Tupper3d/compositemapsubmap.h>
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
#include <Tupper3d/clothmodifiercontext.h>
#include <Tupper3d/trimeshconnectivity.h>
#include <Tupper3d/trimeshconnectivityparts.h>
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
#include <TupperFilters/Libraries/Plugins/builduvtransforms.h>
#include <TupperFilters/Libraries/Plugins/applyuvtransforms.h>
#include <TupperFilters/Libraries/Plugins/uvtransforms.h>
#include <TupperFilters/Libraries/Plugins/buildtexturecontext.h>
#include <TupperFilters/Libraries/Plugins/StageFaces.h>
#include <TupperFilters/Libraries/Plugins/TokenizeString.h>
// DBLUtils library
#include <TupperFilters/Libraries/DBLUtils/DBLUtil.h>
#include <TupperFilters/Libraries/DBLUtils/DBLWrite.h>
#include <TupperFilters/Libraries/DBLUtils/DBLSwap.h>
#include <TupperFilters/Libraries/DBLUtils/DBLDebug.h>
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
#include <Game/Database/MeshSOM.h>
#include <Game/Database/VisRegion.h>
#include <Game/Misc/Bounds.h>
#include <Game/Managers/ParticleDB.h>
#include <Game/Database/lightset.h>
#include <Game/Managers/TextureMgr/TextureDbl.h>

#ifdef EXPORT_PS2
//#include <PS2DMAExport.h>
#endif

//#include "LightChunk.h"
//#include "NamedPointChunk.h"
//#include "Sectors.h"

#endif
