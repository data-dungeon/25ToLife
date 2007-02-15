////////////////////////////////////////////////////////////////////////////
//
// StdAfx.h
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: StdAfx.h $
 * 
 * *****************  Version 6  *****************
 * User: Tony Jones   Date: 6/24/03    Time: 1:11p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/06/03    Time: 4:25p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/29/03    Time: 12:54p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/25/03    Time: 2:03p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * compare meshes start
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/24/03    Time: 4:20p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * support for precompiled header
 * mtools now in a library
 */

#ifndef STDAFX_H
#define	STDAFX_H

// defines for the attribute block headers
#define BEGIN_OBJECT_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_OBJECT_BLOCK	"END OBJECTATTRIBUTES"
#define BEGIN_FACE_BLOCK	"BEGIN FACEATTRIBUTES"
#define END_FACE_BLOCK		"END FACEATTRIBUTES"

#include <iostream>
#include <ostream>
#include <stdio.h>
#include <direct.h>
#include <string>
#include <vector>
#include <memory.h>
#include <algorithm>
#include <math.h>

#define REQUIRE_IOSTREAM

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
#include <maya/MItMeshFaceVertex.h>
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
#include <maya/MEulerRotation.h>
#include <maya/MFnTransform.h>
#include <maya/MFnWeightGeometryFilter.h>
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
#include <maya/MFnIkJoint.h>
#include <maya/MQuaternion.h>
#include <maya/MMatrix.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/mRenderUtil.h>
#include <maya/MFnCharacter.h>
#include <maya/MFnClip.h>
#include <maya/MDGModifier.h>

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

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MItMeshEdge.h>
#include <maya/M3dView.h>
#include <maya/MFnCamera.h>

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
#include <tupper3d/TupVertex.h>
#include <tupper3d/TupMatrix.h>

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

// TupperAttrib
#include <tupperattrib/keyvalue.h>

// Math
//#include <Math/Matrix.h>

// Support

#include "HDoublearray.h"
#include "macros.h"
#include "intmap.h"
#include "vector.h"
#include "vector2.h"
#include "vectormap.h"
#include "mtools.h"



#endif
