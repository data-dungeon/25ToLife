////////////////////////////////////////////////////////////////////////////
//
// ExportTransform
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportTransform.cpp $
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 7/02/03    Time: 3:30p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * make y up code working correctly (doing z up for now)
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 7/01/03    Time: 4:24p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * map attributes
 * y up 
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vector3 issues
 * assert issues
 * add audio clips
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 11:54a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * translation fix
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:50p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * added more data for matrix components
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/06/03    Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export reflection maps
 * export lights
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 12/12/02   Time: 8:55a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix mesh instance matrix to use delta matrix from root bone
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone matrix output correct
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 3:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone pose exporting 
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 4:21p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of materials, maps, images, and attributes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * node, mesh output working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/06/02   Time: 3:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of mesh stuff
 */

#include "TupImpExpPch.h"
#include "TupExport.h"
#include "SkinManager.h"
#ifndef TEMPLATE_SPECIALIZATION
unsigned int GetHashValue(const MDagPath& entry)
#else
template<> unsigned int GetHashValue<MDagPath> (const MDagPath& entry)
#endif
{
	TupString tempString = entry.fullPathName().asChar();
	return GetHashValue(tempString);
}

static const float SCALEFACTOR = .01f;

static const float *GetMayaVector(MVector vector)
{
	static float data[3];
	data[0] = (float) vector.x*SCALEFACTOR;
	data[1] = (float) vector.y*SCALEFACTOR;
	data[2] = (float) vector.z*SCALEFACTOR;
	return data;
}

static const float *GetMayaPoint(MPoint point)
{
	static float data[4];
	data[0] = (float) point.x*SCALEFACTOR;
	data[1] = (float) point.y*SCALEFACTOR;
	data[2] = (float) point.z*SCALEFACTOR;
	data[3] = (float) point.w;
	return data;
}

static const float *GetMayaQuat(MQuaternion quat)
{
	static float data[4];
	double tempDoubles[4];
	quat.get(tempDoubles);
	data[0] = (float) tempDoubles[0];
	data[1] = (float) tempDoubles[1];
	data[2] = (float) tempDoubles[2];
	data[3] = (float) tempDoubles[3];
	return data;
}

void TupExport::AddTransform(TupNode &tupNode,MDagPath mDagPath)
{
	MMatrix parentTM = GetParentBoneTM(mDagPath);

	MMatrix myTM = mDagPath.inclusiveMatrix();
	MMatrix tm = parentTM*myTM;
	
	Graphics4x4 matrix;

	if (mDagPath.apiType() == MFn::kJoint)
		ConvertMayaBoneMatrix(tm,matrix);
	else // if (mDagPath.apiType() == MFn::kTransform)
	{
		if (mDagPath.hasFn(MFn::kLight))
		{
			ConvertMayaLightMatrix(tm,matrix);
		}
		else
		{
			ConvertMayaMatrix(tm,matrix);
		}
	}

	TupperwareAggregate *pTransformAgg = tupNode.AddTransform();
	TupTransform tupTransform(pTransformAgg);
	tupTransform.SetTransformType(TupTransform::STATIC_MATRIX);
	TupStaticMatrix tupStaticMatrix(pTransformAgg);
	tupStaticMatrix.AddMatrix(&matrix.r00);
	int flags = 0;

	MFnTransform mfnTransform(mDagPath.node());

	Vector3 tempVector;
	MVector mayaVector;

	// the MFnTransform.translation function does not return correct results when scaling or rotating
	// so I am outputting the translation value from the matrix itself

	MMatrix localTM = mfnTransform.transformationMatrix(); 
	tempVector.x( (float) localTM(3,0) * SCALEFACTOR);
	tempVector.y( (float) localTM(3,1) * SCALEFACTOR);
	tempVector.z( (float) localTM(3,2) * SCALEFACTOR);
	tupStaticMatrix.AddTranslation((float *) &tempVector);

	MTransformationMatrix::RotationOrder rotationOrder;
	double tempDoubles[3];
	mfnTransform.getRotation(tempDoubles,rotationOrder,MSpace::kObject);
	tempVector.x( (float)tempDoubles[0] );
	tempVector.y( (float)tempDoubles[1] );
	tempVector.z( (float)tempDoubles[2] );
	tupStaticMatrix.AddRotation((float*) &tempVector);
	switch (rotationOrder)
	{
		case MTransformationMatrix::RotationOrder::kXYZ:
			flags |= TupStaticMatrix::FLAGS_ROTATION_ORDER_XYZ;
			break;
		case MTransformationMatrix::RotationOrder::kYZX:
			flags |= TupStaticMatrix::FLAGS_ROTATION_ORDER_YZX;
			break;
		case MTransformationMatrix::RotationOrder::kZXY:
			flags |= TupStaticMatrix::FLAGS_ROTATION_ORDER_ZXY;
			break;
		case MTransformationMatrix::RotationOrder::kXZY:
			flags |= TupStaticMatrix::FLAGS_ROTATION_ORDER_XZY;
			break;
		case MTransformationMatrix::RotationOrder::kYXZ:
			flags |= TupStaticMatrix::FLAGS_ROTATION_ORDER_YXZ;
			break;
		case MTransformationMatrix::RotationOrder::kZYX:
			flags |= TupStaticMatrix::FLAGS_ROTATION_ORDER_ZYX;
			break;
		case MTransformationMatrix::RotationOrder::kLast:
		case MTransformationMatrix::RotationOrder::kInvalid:
			flags |= TupStaticMatrix::FLAGS_ROTATION_ORDER_UNKNOWN;
			break;
	}
	mfnTransform.getScale(tempDoubles);
	tempVector.x( (float)tempDoubles[0] );
	tempVector.y( (float)tempDoubles[1] );
	tempVector.z( (float)tempDoubles[2] );
	tupStaticMatrix.AddScale((float*) &tempVector);

	mfnTransform.getShear(tempDoubles);
	tempVector.x( (float)tempDoubles[0] ); 
	tempVector.y( (float)tempDoubles[1] );
	tempVector.z( (float)tempDoubles[2] );
	tupStaticMatrix.AddShear((float*) &tempVector);

	tupStaticMatrix.AddScalePivot(GetMayaPoint(mfnTransform.scalePivot(MSpace::kObject)));
	tupStaticMatrix.AddRotationPivot(GetMayaPoint(mfnTransform.rotatePivot(MSpace::kObject)));
	tupStaticMatrix.AddScalePivotTranslation(GetMayaVector(mfnTransform.scalePivotTranslation(MSpace::kObject)));
	tupStaticMatrix.AddRotationPivotTranslation(GetMayaVector(mfnTransform.rotatePivotTranslation(MSpace::kObject)));
	tupStaticMatrix.AddRotationOrientation(GetMayaQuat(mfnTransform.rotateOrientation(MSpace::kObject)));

	MFnDagNode dagNode(mDagPath);
	MPlug attrPlug;
	MStatus status;
	attrPlug = dagNode.findPlug("inheritsTransform",&status);
	if (status)
	{
		bool boolValue;
		attrPlug.getValue(boolValue);
		if (boolValue)
			flags |= TupStaticMatrix::FLAGS_INHERIT_TRANSFORM;
	}

	tupStaticMatrix.AddFlags(flags);
}

void TupExport::ConvertMayaMatrix(const MMatrix &srcMatrix,Graphics4x4 &dstMatrix)
{
	const float *pSrcMatrix = (const float *)&srcMatrix;

	dstMatrix.r00 = (float)srcMatrix.matrix[0][0];
	dstMatrix.r01 = (float)srcMatrix.matrix[0][1];
	dstMatrix.r02 = (float)srcMatrix.matrix[0][2];

	dstMatrix.r10 = (float)srcMatrix.matrix[1][0];
	dstMatrix.r11 = (float)srcMatrix.matrix[1][1];
	dstMatrix.r12 = (float)srcMatrix.matrix[1][2];

	dstMatrix.r20 = (float)srcMatrix.matrix[2][0];
	dstMatrix.r21 = (float)srcMatrix.matrix[2][1];
	dstMatrix.r22 = (float)srcMatrix.matrix[2][2];

	dstMatrix.tx  = (float)srcMatrix.matrix[3][0]*SCALEFACTOR;
	dstMatrix.ty  = (float)srcMatrix.matrix[3][1]*SCALEFACTOR;
	dstMatrix.tz  = (float)srcMatrix.matrix[3][2]*SCALEFACTOR;

	dstMatrix.u0  = dstMatrix.u1 = dstMatrix.u2 = 0.0f;
	dstMatrix.w = 1.0f;

#ifdef MAKE_Z_UP
	Graphics4x4 matrixYtoZ,matrixZtoY;

	matrixYtoZ.SetXRotation(-Math::Pi/2.0f);
	matrixZtoY.SetXRotation(Math::Pi/2.0f);

	// start with local(yup)->parent(yup) matrix
	// get local(yup)->parent(zup) matrix
	dstMatrix = matrixYtoZ*dstMatrix; 
	// get local(yup)->parent(yup) matrix
	dstMatrix *= matrixZtoY;
#endif
}

void TupExport::ConvertMayaBoneMatrix(const MMatrix &srcMatrix,Graphics4x4 &dstMatrix)
{
	const float SCALEFACTOR = .01f;

	const float *pSrcMatrix = (const float *)&srcMatrix;

	dstMatrix.r00 = (float)srcMatrix.matrix[0][0];
	dstMatrix.r01 = (float)srcMatrix.matrix[0][1];
	dstMatrix.r02 = (float)srcMatrix.matrix[0][2];

	dstMatrix.r10 = (float)srcMatrix.matrix[1][0];
	dstMatrix.r11 = (float)srcMatrix.matrix[1][1];
	dstMatrix.r12 = (float)srcMatrix.matrix[1][2];

	dstMatrix.r20 = (float)srcMatrix.matrix[2][0];
	dstMatrix.r21 = (float)srcMatrix.matrix[2][1];
	dstMatrix.r22 = (float)srcMatrix.matrix[2][2];

	dstMatrix.tx  = (float)srcMatrix.matrix[3][0]*SCALEFACTOR;
	dstMatrix.ty  = (float)srcMatrix.matrix[3][1]*SCALEFACTOR;
	dstMatrix.tz  = (float)srcMatrix.matrix[3][2]*SCALEFACTOR;

	dstMatrix.u0  = dstMatrix.u1 = dstMatrix.u2 = 0.0f;
	dstMatrix.w = 1.0f;

#ifdef MAKE_Z_UP
	Graphics4x4 matrixYtoZ,matrixZtoY;

	matrixYtoZ.SetXRotation(-Math::Pi/2.0f);
	matrixZtoY.SetXRotation(Math::Pi/2.0f);

	// start with local(yup)->parent(yup) matrix
	// get local(yup)->parent(zup) matrix
	dstMatrix = matrixYtoZ*dstMatrix; 
	// get local(yup)->parent(yup) matrix
	dstMatrix *= matrixZtoY;

	Graphics4x4 rotateZ;
	rotateZ.SetZRotation(-Math::Pi/2.0f);
	dstMatrix = rotateZ * dstMatrix;
#else
	Graphics4x4 rotateY;
	rotateY.SetYRotation(-Math::Pi/2.0f);
	dstMatrix = rotateY * dstMatrix;
#endif
}

void TupExport::ConvertMayaLightMatrix(const MMatrix &srcMatrix,Graphics4x4 &dstMatrix)
{
	const float SCALEFACTOR = .01f;
	
	const float *pSrcMatrix = (const float *)&srcMatrix;
	
	dstMatrix.r00 = (float)srcMatrix.matrix[0][0];
	dstMatrix.r01 = (float)srcMatrix.matrix[0][1];
	dstMatrix.r02 = (float)srcMatrix.matrix[0][2];
	
	dstMatrix.r10 = (float)srcMatrix.matrix[1][0];
	dstMatrix.r11 = (float)srcMatrix.matrix[1][1];
	dstMatrix.r12 = (float)srcMatrix.matrix[1][2];
	
	dstMatrix.r20 = (float)srcMatrix.matrix[2][0];
	dstMatrix.r21 = (float)srcMatrix.matrix[2][1];
	dstMatrix.r22 = (float)srcMatrix.matrix[2][2];
	
	dstMatrix.tx  = (float)srcMatrix.matrix[3][0]*SCALEFACTOR;
	dstMatrix.ty  = (float)srcMatrix.matrix[3][1]*SCALEFACTOR;
	dstMatrix.tz  = (float)srcMatrix.matrix[3][2]*SCALEFACTOR;
	
	dstMatrix.u0  = dstMatrix.u1 = dstMatrix.u2 = 0.0f;
	dstMatrix.w = 1.0f;

#ifdef MAKE_Z_UP
	Graphics4x4 matrixYtoZ,matrixZtoY;
	
	matrixYtoZ.SetXRotation(-Math::Pi/2.0f);
	matrixZtoY.SetXRotation(Math::Pi/2.0f);
	
	// start with local(yup)->parent(yup) matrix
	// get local(yup)->parent(zup) matrix
	dstMatrix = matrixYtoZ*dstMatrix; 
	// get local(zup)->parent(zup) matrix
	dstMatrix *= matrixZtoY;

	Graphics4x4 rotateX;
	rotateX.SetXRotation(Math::Pi/2.0f);
	dstMatrix = rotateX * dstMatrix;
#else
	Graphics4x4 rotateX;
	rotateX.SetXRotation(Math::Pi/2.0f);
	dstMatrix = rotateX * dstMatrix;
#endif
}


MMatrix TupExport::GetParentBoneTM(MDagPath mDagPath)
{
	MMatrix parentTM; // start with idenity

	// see if I have a mesh child that is bound to a set of bones
	if (mDagPath.apiType() == MFn::kTransform)
	{
		int numChildren = mDagPath.childCount();
		for (int childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MObject childObject = mDagPath.child(childIndex);
			MDagPath childPath = mDagPath;
			childPath.push(childObject);

			if (childPath.apiType() == MFn::kMesh)
			{
				MFnDependencyNode fnChildNode(childPath.node());
				int objectPtr = (int)MayaHelper::GetMPtrBase(fnChildNode);

				// check if this mesh has any bones attached to it
				int skinIndex = m_pSkinManager->GetSkinIndex(objectPtr);
				if (skinIndex!=-1)
				{
					const SkinManager::BoneConnections &boneConnections = m_pSkinManager->GetBoneConnections(skinIndex,0);
					MDagPath mParentJointDagPath = (m_pNodePoolDagPaths->GetArray())[boneConnections.m_boneNodeRef];
					while (1) 
					{
						MDagPath mTestParentDagPath = mParentJointDagPath;
						mTestParentDagPath.pop();
						if (mTestParentDagPath.apiType() != MFn::kJoint)
							break;
						mParentJointDagPath = mTestParentDagPath;
					}
					MMatrix jointTM = mParentJointDagPath.inclusiveMatrix();
					MMatrix jointBindPoseTM = GetBindPoseMatrixForJoint(mParentJointDagPath.node());
					// get matrix delta between joint actual matrix and the bind pose
					parentTM = jointBindPoseTM.inverse() * jointTM;
				}
			}
		}
	}
	return parentTM;
}
