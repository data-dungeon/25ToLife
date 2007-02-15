////////////////////////////////////////////////////////////////////////////
//
// TupImpExpPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupImpExpPch.h $
 * 
 * *****************  Version 30  *****************
 * User: Adam Clayton Date: 10/08/03   Time: 12:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * smooth binding exporting
 * 
 * *****************  Version 29  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting layers
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 9/04/03    Time: 10:50a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * poly mesh to tri mesh now has triangulation routine
 * 
 * *****************  Version 27  *****************
 * User: Adam Clayton Date: 8/19/03    Time: 11:48a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export vertex colors (from apply vc modifier)
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 8/13/03    Time: 4:52p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import nodes and polymeshes
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:59a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 7/30/03    Time: 12:45p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * polygon export support
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 7/24/03    Time: 10:30a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * now doing y up
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 7/02/03    Time: 3:30p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * make y up code working correctly (doing z up for now)
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 7/01/03    Time: 4:24p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * map attributes
 * y up 
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vector3 issues
 * assert issues
 * add audio clips
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * motion path exporting
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 10:34a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * animation globals
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 2:25p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * add clip blends
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 10:35a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export characters and clips
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:50p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * added more data for matrix components
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first phase of animation export
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/17/03    Time: 2:19p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 2/06/03    Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export reflection maps
 * export lights
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 12/18/02   Time: 12:13p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting face attribute modifiers
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 3:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone pose exporting 
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/14/02   Time: 3:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bezier shape
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 4:21p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of materials, maps, images, and attributes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * node, mesh output working
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/06/02   Time: 3:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of mesh stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/29/02   Time: 8:58a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * starting point for tup exporter
 */

#ifndef TUP_IMP_EXP_PCH_H
#define	TUP_IMP_EXP_PCH_H

// defines for the attribute block headers
#define BEGIN_OBJECT_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_OBJECT_BLOCK	"END OBJECTATTRIBUTES"
#define BEGIN_FACE_BLOCK	"BEGIN FACEATTRIBUTES"
#define END_FACE_BLOCK		"END FACEATTRIBUTES"

// blind face data for face attributes
#define BLIND_DATA_ID 956
#define BLIND_DATA_LONG_NAME "FaceAttributes"
#define BLIND_DATA_SHORT_NAME "fa1"
#define BLIND_DATA_FORMAT_NAME "int"


// defines for changing output
//#define MAKE_Z_UP
//#define COMPARE_NORMALS
#define POLYGON_EXPORT

//#define COLOR_TRI_STRIPS_ON_IMPORT
//#define COLOR_FACE_GROUPS

#define ADD_OLD_BEZIER_SPLINE_DATA
//#ifdef IMBED_ATTRIBUTE_CFG
#define VERIFY_FINITE_VALUES


// timeout after certain time
//#define TIMEOUT
//#define TIMEOUT_YEAR 2006
//#define TIMEOUT_MONTH 9
//#define TIMEOUT_DAY 1

// defined for changing input
// indicates if shaders are assigned after skin
#define DEFER_MESH_SHADER_APPLICATION

#define REQUIRE_IOSTREAM
//#include <iostream.h>
#include <stdio.h>
#include <direct.h>
#include <string>
#include <vector>
#include <memory.h>
#include <float.h>
#include <time.h>

// Maya
#include <maya/MString.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItGeometry.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnSet.h>
#include <maya/MDagModifier.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnTransform.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnTransform.h>
#include <maya/MFnWeightGeometryFilter.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MDGModifier.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <maya/MFnTripleIndexedComponent.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnPointLight.h>
#include <maya/MFnSpotLight.h>
#include <maya/MFnAmbientLight.h>
#include <maya/MFnDirectionalLight.h>
#include <maya/MFnAreaLight.h>
#include <maya/MFnIKJoint.h>

// Attributes
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>

#include <maya/MAngle.h>
#include <maya/MTime.h>
#include <maya/MDistance.h>

#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnLight.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MObjectArray.h>
#include <maya/MStringArray.h>
#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MFileIO.h>
#include <maya/MFnData.h>
#include <maya/MFnComponentListData.h>

#include <maya/MFnAnimCurve.h>
#include <maya/MAnimUtil.h>
#include <maya/MFnCamera.h>
#include <maya/MFnCharacter.h>
#include <maya/MFnClip.h>
#include <maya/MQuaternion.h>
#include <maya/MAnimControl.h>

#include <maya/MFnMotionPath.h>

// Tupperware
#include <tupperware/aggregate.h>
#include <tupperware/scalar.h>
#include <tupperware/list.h>
#include <tupperware/loadsave.h>
#include <tupperware/tuparray.h>
#include <tupperware/tupstring.h>
#include <tupperware/hash.h>
#include <tupperware/tupSort.h>
#include <tupperware/keeper.h>

template<> 
unsigned int GetHashValue<MDagPath> (const MDagPath& entry);

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
#include <tupper3d/polymesh.h>
#include <tupper3d/polymeshmap.h>
#include <tupper3d/triangleStrip.h>
#include <tupper3d/camera.h>
#include <tupper3d/light.h>
#include <tupper3d/xref.h>
#include <tupper3d/derived.h>
#include <tupper3d/derivedmodifier.h>
#include <tupper3d/beziershape.h>
#include <tupper3d/bezierspline.h>
#include <tupper3d/splineshape.h>
#include <tupper3d/spline.h>
#include <tupper3d/bone.h>
#include <tupper3d/locator.h>
#include <tupper3d/lookat.h>

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

#include <tupper3d/AnimCurve.h>
#include <tupper3d/AnimCurvePool.h>
#include <tupper3d/AnimSetPool.h>
#include <tupper3d/AnimSet.h>
#include <tupper3d/AnimChannelPool.h>
#include <tupper3d/AnimChannel.h>
#include <tupper3d/AnimClipPool.h>
#include <tupper3d/AnimInstancedClip.h>
#include <tupper3d/AnimSourceClip.h>
#include <tupper3d/AnimClipBlendPool.h>
#include <tupper3d/AnimClipBlend.h>
#include <tupper3d/AnimGlobals.h>

#include <tupper3d/connectionpool.h>
#include <tupper3d/connection.h>
#include <tupper3d/connectionmotionpath.h>

#include <tupper3d/audiopool.h>
#include <tupper3d/audio.h>

#include <Tupper3d/layerpool.h>
#include <Tupper3d/layer.h>

#include <tupper3d/ObjectConverter.h>
#include <tupper3d/CoordinateSystemConverter.h>
#include <tupper3d/MultiSubObjectMaterialConverter.h>
#include <tupper3d/FixRogueModifiers.h>
#include <tupper3d/TupVertex.h>

#include <Tupper3d/ObjectHelper.h>


// TupperAttrib
#include <tupperattrib/keyvalue.h>

// Math
#include <Math/Matrix.h>

// Support
#include "../../libraries/MTools/MTools.h"
#include "filepath.h"
#include "MayaHelper.h"

extern const char *g_pExporterDate;
extern const char *g_pExporterTime;

#endif