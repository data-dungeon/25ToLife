////////////////////////////////////////////////////////////////////////////
//
// ZupToYupConverter
//
// Handle the conversion from a zup coordinate system to yup
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ZupToYupConverter.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * coordinate system converters
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"

#include "scene.h"
#include "node.h"
#include "nodepool.h"
#include "transform.h"
#include "staticmatrix.h"
#include "object.h"
#include "objectpool.h"
#include "trimesh.h"
#include "polymesh.h"
#include "beziershape.h"
#include "bezierspline.h"
#include "splineshape.h"
#include "spline.h"
#include "derived.h"
#include "derivedmodifier.h"
#include "modifier.h"
#include "modifierpool.h"
#include "skinmodifier.h"
#include "skinmodifierbone.h"
#include "skinmodifiercontext.h"

#include "TupVertex.h"
#include "TupMatrix.h"

#include "ZupToYupConverter.h"

bool TupZupToYupConverter::ConvertScene(TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool)
{
	TupZupToYupConverter Converter(tupScene,tupNodePool,tupObjectPool,tupModifierPool);
	return Converter.TransformScene();
}
	
TupZupToYupConverter::TupZupToYupConverter(TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool) :
	m_tupScene(tupScene),
	m_tupNodePool(tupNodePool),
	m_tupObjectPool(tupObjectPool),
	m_tupModifierPool(tupModifierPool)
{
}

bool TupZupToYupConverter::TransformScene(void)
{
	bool bResult;
	// do the objects
	int numObjects = m_tupObjectPool.GetNumObjects();
	for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectIndex);
		// if slot is empty then continue
		if (!pObjectAgg)
			continue;
		bResult = TransformObject(pObjectAgg);
		if (!bResult)
			return false;
	}

	// do the nodes
	int numNodes = m_tupNodePool.GetNumNodes();
	for (int nodeIndex = 0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);
		// if slot is empty then continue
		if (!pNodeAgg)
			continue;
		bResult = TransformNode(pNodeAgg);
		if (!bResult)
			return false;
	}

	// set to yup
	int unitFlags = TupScene::UP_AXIS_Y | TupScene::LINEAR_UNIT_METER | TupScene::ANGULAR_UNIT_RADIANS | TupScene::TIME_UNIT_UNKNOWN;
	TupperwareScalar *pUnitsScalar = m_tupScene.GetUnitFlags(unitFlags);
	if (pUnitsScalar)
	{
		unitFlags = (unitFlags & ~TupScene::UP_AXIS_MASK) | TupScene::UP_AXIS_Y;
	}

	m_tupScene.AddUnitFlags(unitFlags);

	return true;
}

bool TupZupToYupConverter::TransformObject(TupperwareAggregate *pObjectAgg)
{
	int i;
	TupVertex3f *pArray;
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType() == TupObject::TRI_MESH)
	{
		TupTriMesh tupTriMesh(pObjectAgg);
		// do positions
		int numPositionVerts;
		float *pPositionVerts;
		tupTriMesh.GetPositionVerts(&pPositionVerts,numPositionVerts);
		pArray = (TupVertex3f *)pPositionVerts;
		for (i=0;i<numPositionVerts;i++)
		{
			TupVertex3f &vector = pArray[i];
			float temp = vector.y;
			vector.y = vector.z;
			vector.z = -temp;
		}
		// do normals
		int numNormalVerts;
		float *pNormalVerts;
		tupTriMesh.GetNormalVerts(&pNormalVerts,numNormalVerts);
		pArray = (TupVertex3f *)pNormalVerts;
		for (i=0;i<numNormalVerts;i++)
		{
			TupVertex3f &vector = pArray[i];
			float temp = vector.y;
			vector.y = vector.z;
			vector.z = -temp;
		}
		// do face normals
		int numFaceNormalVerts;
		float *pFaceNormalVerts;
		tupTriMesh.GetFaceNormalVerts(&pFaceNormalVerts,numFaceNormalVerts);
		pArray = (TupVertex3f *)pFaceNormalVerts;
		for (i=0;i<numFaceNormalVerts;i++)
		{
			TupVertex3f &vector = pArray[i];
			float temp = vector.y;
			vector.y = vector.z;
			vector.z = -temp;
		}
	}
	else if (tupObject.GetObjectType() == TupObject::POLY_MESH)
	{
		TupPolyMesh tupPolyMesh(pObjectAgg);
		// do positions
		int numPositionVerts;
		float *pPositionVerts;
		tupPolyMesh.GetPositionVerts(&pPositionVerts,numPositionVerts);
		pArray = (TupVertex3f *)pPositionVerts;
		for (i=0;i<numPositionVerts;i++)
		{
			TupVertex3f &vector = pArray[i];
			float temp = vector.y;
			vector.y = vector.z;
			vector.z = -temp;
		}
		// do normals
		int numNormalVerts;
		float *pNormalVerts;
		tupPolyMesh.GetNormalVerts(&pNormalVerts,numNormalVerts);
		pArray = (TupVertex3f *)pNormalVerts;
		for (i=0;i<numNormalVerts;i++)
		{
			TupVertex3f &vector = pArray[i];
			float temp = vector.y;
			vector.y = vector.z;
			vector.z = -temp;
		}
		// do face normals
		int numFaceNormalVerts;
		float *pFaceNormalVerts;
		tupPolyMesh.GetFaceNormalVerts(&pFaceNormalVerts,numFaceNormalVerts);
		pArray = (TupVertex3f *)pFaceNormalVerts;
		for (i=0;i<numFaceNormalVerts;i++)
		{
			TupVertex3f &vector = pArray[i];
			float temp = vector.y;
			vector.y = vector.z;
			vector.z = -temp;
		}
	}
	else if (tupObject.GetObjectType() == TupObject::BEZIER_SHAPE)
	{
		TupBezierShape tupBezierShape(pObjectAgg);
		int totalSplines = tupBezierShape.GetNumSplines();
		for (int splineIndex=0;splineIndex<totalSplines;splineIndex++)
		{

			TupperwareAggregate *pSplineAgg = tupBezierShape.GetSpline(splineIndex);
			assert(pSplineAgg);
			TupBezierSpline tupBezierSpline(pSplineAgg);

			int numPoints;
			TupVertex3f *pPointLocations;
			TupVertex3f *pInVectors;
			TupVertex3f *pOutVectors;

			tupBezierSpline.GetPointLocations((float **)&pPointLocations,numPoints);
			for (i=0;i<numPoints;i++)
			{
				TupVertex3f &vector = pPointLocations[i];
				float temp = vector.y;
				vector.y = vector.z;
				vector.z = -temp;
			}

			tupBezierSpline.GetInVectors((float **)&pInVectors,numPoints);
			for (i=0;i<numPoints;i++)
			{
				TupVertex3f &vector = pInVectors[i];
				float temp = vector.y;
				vector.y = vector.z;
				vector.z = -temp;
			}

			tupBezierSpline.GetOutVectors((float **)&pOutVectors,numPoints);
			for (i=0;i<numPoints;i++)
			{
				TupVertex3f &vector = pOutVectors[i];
				float temp = vector.y;
				vector.y = vector.z;
				vector.z = -temp;
			}
		}
	}
	else if (tupObject.GetObjectType() == TupObject::SPLINE_SHAPE)
	{
		TupSplineShape tupSplineShape(pObjectAgg);
		int totalSplines = tupSplineShape.GetNumSplines();
		for (int splineIndex=0;splineIndex<totalSplines;splineIndex++)
		{
			TupperwareAggregate *pSplineAgg = tupSplineShape.GetSpline(splineIndex);
			assert(pSplineAgg);
			TupSpline tupSpline(pSplineAgg);

			int numPoints;
			TupVertex3f *pControlPoints;

			tupSpline.GetControlPoints((float **)&pControlPoints,numPoints);
			for (i=0;i<numPoints;i++)
			{
				TupVertex3f &vector = pControlPoints[i];
				float temp = vector.y;
				vector.y = vector.z;
				vector.z = -temp;
			}
		}
	}
	else if (tupObject.GetObjectType() == TupObject::DERIVED)
	{
		TupDerived tupDerived(pObjectAgg);
		int totalModifierSlots = tupDerived.GetNumModifierSlots();
		// if there is any modifiers
		if (totalModifierSlots)
		{
			// loop through the modifiers and see if any deal with the vertex colors
			for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					bool bResult = TransformModifier(pDerivedModifierAgg);
					if (!bResult)
						return false;
				}
			}
		}
	}
	return true;
}

bool TupZupToYupConverter::TransformModifier(TupperwareAggregate *pDerivedModifierAgg)
{
	TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
	int modifierReference;
	tupDerivedModifier.GetModifierReference(modifierReference);
	TupperwareAggregate *pModifierAgg = m_tupModifierPool.GetModifier(modifierReference);
	TupModifier tupModifier(pModifierAgg);
	switch(tupModifier.GetModifierType())
	{
			case TupModifier::SKIN_MODIFIER:
			{
				TupSkinModifier tupSkinModifier(pModifierAgg);

				TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
				TupSkinModifierContext tupSkinModifierContext(pModifierContextAgg);
				float *pFloatList;
				tupSkinModifierContext.GetBaseTM(&pFloatList);
				TupMatrix4x4f *pBaseMatrix = (TupMatrix4x4f *)pFloatList;

				// convert matrix from z up to y up
				TupMatrix4x4f matrixYtoZ,matrixZtoY;

				matrixYtoZ.SetXRotation(-90.0f);
				matrixZtoY.SetXRotation(90.0f);

				// start with local(zup)->parent(zup) matrix
				// get local(zup)->parent(yup) matrix
				(*pBaseMatrix).Multiply(matrixZtoY,*pBaseMatrix);
				// get local(yup)->parent(yup) matrix
				matrixYtoZ.Multiply(*pBaseMatrix,*pBaseMatrix);

				int nBonesRefs = tupSkinModifier.GetNumBones();

				for (int boneIndex=0;boneIndex<nBonesRefs;boneIndex++)
				{
					TupperwareAggregate *pBoneAgg = tupSkinModifier.GetBone(boneIndex);
					TupSkinModifierBone tupBone(pBoneAgg);

					tupBone.GetBoneInitObjectTM(&pFloatList);
					TupMatrix4x4f *pBoneMatrix = (TupMatrix4x4f *)pFloatList;

					// start with local(zup)->parent(zup) matrix
					// get local(zup)->parent(yup) matrix
					(*pBoneMatrix).Multiply(matrixZtoY,*pBoneMatrix);
					// get local(yup)->parent(yup) matrix
					matrixYtoZ.Multiply(*pBoneMatrix,*pBoneMatrix);
				}
			}
	}
	return true;
}

bool TupZupToYupConverter::TransformNode(TupperwareAggregate *pNodeAgg)
{
	TupNode tupNode(pNodeAgg);
	TupperwareAggregate *pTransformAgg = tupNode.GetTransform();
	if (pTransformAgg)
	{
		TupTransform tupTransform(pTransformAgg);
		TupTransform::TransformTypes transformType = tupTransform.GetTransformType();
		if (transformType == TupTransform::STATIC_MATRIX)
		{
			TupStaticMatrix tupStaticMatrix(pTransformAgg);
			float *pFloatList;
			tupStaticMatrix.GetMatrix(&pFloatList);
			TupMatrix4x4f *pMatrix = (TupMatrix4x4f *)pFloatList;
			// convert matrix from z up to y up
			TupMatrix4x4f matrixYtoZ,matrixZtoY;

			matrixYtoZ.SetXRotation(-90.0f);
			matrixZtoY.SetXRotation(90.0f);

			// start with local(zup)->parent(zup) matrix
			// get local(zup)->parent(yup) matrix
			(*pMatrix).Multiply(matrixZtoY,*pMatrix);
			// get local(yup)->parent(yup) matrix
			matrixYtoZ.Multiply(*pMatrix,*pMatrix);
		}
	}
	return true;
}
