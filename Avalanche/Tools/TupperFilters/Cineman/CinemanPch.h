////////////////////////////////////////////////////////////////////////////
//
// CinemanPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CinemanPch.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:12a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 8  *****************
 * User: Nmartin      Date: 7/30/03    Time: 11:29a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process light participant
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 3:07p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * audio clip support
 * 
 * *****************  Version 6  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 4:19p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 11:00a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * animglobals
 * 
 * *****************  Version 4  *****************
 * User: Nmartin      Date: 5/30/03    Time: 3:27p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * working on merging multiple participants into one cut scene data stream
 * (e.g. all particle effects end up as one stream)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 1:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * group transform connecting to boned mesh transform
 * animation from bones connecting to actions for mesh transform
 * added more camera channels for hooking up
 * added instance clip blends
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 3:21p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of participants classes
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:30p
 * Created in $/Avalanche/tools/TupperFilters/Cineman
 */

#ifndef __CINEMAN_PCH__H
#define __CINEMAN_PCH__H

// defines for the attribute block headers
#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK	"END OBJECTATTRIBUTES"

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <Math/Vector.h>
#include <Math/Matrix.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/TupArray.h>
#include <tupperware/TupString.h>
#include <tupperware/TupSort.h>
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
#include <Tupper3d/camera.h>
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
#include <Tupper3d/skinmodifier.h>
#include <Tupper3d/skinmodifierbone.h>
#include <Tupper3d/skinmodifiercontext.h>
#include <Tupper3d/skinmodifiercontextvertex.h>
#include <Tupper3d/skinmodifiercontextvertexpool.h>
#include <Tupper3d/clothmodifier.h>
#include <Tupper3d/clothmodifiercontext.h>

#include <tupper3d/AnimCurve.h>
#include <tupper3d/AnimCurvePool.h>
#include <tupper3d/AnimSetPool.h>
#include <tupper3d/AnimSet.h>
#include <tupper3d/AnimChannelPool.h>
#include <tupper3d/AnimChannel.h>
#include <tupper3d/AnimClipPool.h>
#include <tupper3d/AnimInstancedClip.h>
#include <tupper3d/AnimSourceClip.h>
#include <tupper3d/AnimClipBlend.h>
#include <tupper3d/AnimClipBlendPool.h>

#include <Tupper3d/AnimGlobals.h>

#include <tupper3d/audiopool.h>
#include <tupper3d/audio.h>

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
#include <TupperFilters/Libraries/Plugins/ImageUtilities.h>
#include <TupperFilters/Libraries/Plugins/BuildUVTransforms.h>
#include <TupperFilters/Libraries/Plugins/ApplyUVTransforms.h>
#include <TupperFilters/Libraries/Plugins/UVTransforms.h>

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
//#include <Game/Misc/Actor.h>
#include <Game/Managers/ParticleDB.h>
#include <GeomUtil/AnimCurve.h>

#endif
