#define REQUIRE_IOSTREAM

#include <assert.h>

#include <maya/MStatus.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFileIO.h>
#include <maya/MItDag.h>
#include <maya/MDagModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <maya/MFnTransform.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MDagPathArray.h>
#include <maya/MStringArray.h>
#include <maya/MVector.h>
#include <maya/MEulerrotation.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnSet.h>
#include <maya/MFnMesh.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnGeometryFilter.h>
#include <maya/MItMeshVertex.h>
#include <maya/MFnIkJoint.h>
#include <maya/MQuaternion.h>
#include <maya/MMatrix.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/mRenderUtil.h>
#include <maya/MPointArray.h>
#include <maya/MFnCharacter.h>
#include <maya/MFnClip.h>
#include <maya/MTime.h>
#include <maya/MItDependencyNodes.h>
#include <maya/M3dView.h>

#include <iostream>

#include <Tupperware/TupString.h>

