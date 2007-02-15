////////////////////////////////////////////////////////////////////////////
//
// ImportNode
//
// Tupperware import
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportTransform.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/13/03    Time: 4:52p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import nodes and polymeshes
 */

#include "TupImpExpPch.h"
#include "TupImport.h"
#include "JointManager.h"

void TupImport::ConvertToMayaMatrix(const Graphics4x4 &srcMatrix,MMatrix &dstMatrix)
{
	const float SCALEFACTOR = 100.0f;
	
	dstMatrix.matrix[0][0] = srcMatrix.r00;
	dstMatrix.matrix[0][1] = srcMatrix.r01;
	dstMatrix.matrix[0][2] = srcMatrix.r02;
	
	dstMatrix.matrix[1][0] = srcMatrix.r10;
	dstMatrix.matrix[1][1] = srcMatrix.r11;
	dstMatrix.matrix[1][2] = srcMatrix.r12;
	
	dstMatrix.matrix[2][0] = srcMatrix.r20;
	dstMatrix.matrix[2][1] = srcMatrix.r21;
	dstMatrix.matrix[2][2] = srcMatrix.r22;
	
	dstMatrix.matrix[3][0] = srcMatrix.tx*SCALEFACTOR;
	dstMatrix.matrix[3][1] = srcMatrix.ty*SCALEFACTOR;
	dstMatrix.matrix[3][2] = srcMatrix.tz*SCALEFACTOR;
	
	dstMatrix.matrix[0][3] = dstMatrix.matrix[1][3] = dstMatrix.matrix[2][3] = 0.0;
	dstMatrix.matrix[3][3] = 1.0;
}

void TupImport::ConvertBoneToMayaMatrix(const Graphics4x4 &srcMatrix,MMatrix &dstMatrix)
{
	Graphics4x4 srcMatrixTemp;
	Graphics4x4 rotateY;
	rotateY.SetYRotation(Math::Pi/2.0f);
	srcMatrixTemp = rotateY * srcMatrix;
	ConvertToMayaMatrix(srcMatrixTemp,dstMatrix);
}

void TupImport::ConvertLightToMayaMatrix(const Graphics4x4 &srcMatrix,MMatrix &dstMatrix)
{
	Graphics4x4 srcMatrixTemp;
	Graphics4x4 rotateX;
	rotateX.SetXRotation(-Math::Pi/2.0f);
	srcMatrixTemp = rotateX * srcMatrix;
	ConvertToMayaMatrix(srcMatrixTemp,dstMatrix);
}

void TupImport::ImportTransform(int nodeRef,int parentNodeRef,TupNode &tupNode,TupNode &tupParentNode,MObject transformObject)
{
	MMatrix parentMatrix;
	GetMatrix(parentNodeRef,tupParentNode,parentMatrix);
	MMatrix myMatrix;
	GetMatrix(nodeRef,tupNode,myMatrix);

	MMatrix inverseParentMatrix = parentMatrix.inverse();

	myMatrix = myMatrix*inverseParentMatrix; //*myMatrix;

	int objectRef;
	tupNode.GetObjectReference(objectRef);
	TupObject::ObjectTypes objectType = GetObjectType(objectRef);

	switch (objectType)
	{
		case TupObject::BONE:
			{
				MFnIkJoint mfnJoint(transformObject);
				MTransformationMatrix localMatrix(myMatrix);

				MVector translation(myMatrix(3,0),myMatrix(3,1),myMatrix(3,2));
				mfnJoint.setTranslation(translation,MSpace::kObject);

				MQuaternion orientation;
				localMatrix.getRotationQuaternion(orientation.x,orientation.y,orientation.z,orientation.w);
				mfnJoint.setOrientation(orientation);
			}
			break;
		default:
			{
				MFnTransform mfnTransform(transformObject);
				MTransformationMatrix localMatrix(myMatrix);
				mfnTransform.set(localMatrix);
			}
	}

}

TupObject::ObjectTypes TupImport::GetObjectType(int objectRef)
{
	// default to poly mesh
	TupObject::ObjectTypes objectType = TupObject::POLY_MESH;

	if (objectRef!=-1)
	{
		int baseObjectRef = TupObjectHelper::FindBaseObjectRef(*m_pObjectPool,objectRef);
		assert(baseObjectRef!=-1);
		TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(baseObjectRef);
		assert(pObjectAgg);

		TupObject tupObject(pObjectAgg);
		objectType = tupObject.GetObjectType();
	}
	return objectType;
}

void TupImport::GetMatrix(int nodeRef,TupNode &tupNode,MMatrix &matrix)
{
	int objectRef;
	tupNode.GetObjectReference(objectRef);
	TupObject::ObjectTypes objectType = GetObjectType(objectRef);

	Graphics4x4 *pMatrix = NULL;

	// get initial matrix from the node
	TupperwareAggregate *pTransformAgg = tupNode.GetTransform();
	if (pTransformAgg)
	{
		TupTransform tupTransform(pTransformAgg);
		if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
		{
			TupStaticMatrix tupStaticMatrix(pTransformAgg);
			tupStaticMatrix.GetMatrix((float**) &pMatrix);
			ConvertToMayaMatrix(*pMatrix,matrix);
		}
	}

	// any special behavior for certain object types
	switch (objectType)
	{
		case TupObject::BONE:
			{
				int skinIndex;
				int boneInSkinIndex;
				if (m_pJointManager->BoneInSkin(nodeRef,skinIndex,boneInSkinIndex))
				{
					const JointManager::BoneConnections &boneConnections = m_pJointManager->GetBoneConnections(skinIndex,boneInSkinIndex);
					ConvertBoneToMayaMatrix(boneConnections.m_boneBindingPoseMatrix,matrix);
				}
				else
				{
					ConvertBoneToMayaMatrix(*pMatrix,matrix);
				}
			}
			break;
		case TupObject::LIGHT:
			{
				ConvertLightToMayaMatrix(*pMatrix,matrix);
			}
			break;
	}
}

