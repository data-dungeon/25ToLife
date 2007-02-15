////////////////////////////////////////////////////////////////////////////
//
// MakeYUpFilter
//
// Class to handle the Make YUp Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MakeYUp.cpp $
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:05p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * convert over to tupper3d coordinate system classes
 * keep old code just in case
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:35a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:05a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * avoid running if already y up
 * 
 * *****************  Version 14  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 9:37a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Modifier access using slots
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 6/10/02    Time: 9:26a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * transform bezier shapes
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 5/29/02    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Transform skin modifier
 * Changed to use node pools
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 2:28p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Oops on the normal
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Abort checks within system
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:57p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Changed plugin class
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/01/02    Time: 12:05p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Rich Edit Control for the Log.  Added additional flags for font styles
 * and colors.
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Added Node Pool and Node Referencing
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Fixed Exception Problem
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/10/02    Time: 2:52p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * const functions
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * skeleton filter chain complete
 */

////////////////////////////////////////////////////////////////////////////

#include "MakeYUpPch.h"
#include "MakeYUp.h"


int MakeYUpFilter::GetTotalPasses(void)	const
{
	return 1;
}

void MakeYUpFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	// set up log
	PluginLog::SetLog(&m_log);

	m_pSceneAgg = dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);

	int unitFlags;
	TupperwareScalar *pUnitsScalar = m_pTupScene->GetUnitFlags(unitFlags);
	if (pUnitsScalar && ((unitFlags & TupScene::UP_AXIS_MASK) == TupScene::UP_AXIS_Y))
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"File already in Y up Format.  MakeYUpFilter not converting data.");
		delete m_pTupScene;
	}
	else
	{
		m_pNodePoolAgg = m_pTupScene->GetNodePool();
		assert(m_pNodePoolAgg);
		m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);
		
		m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
		assert(m_pObjectPoolAgg);
		m_pTupObjectPool = new TupObjectPool( m_pObjectPoolAgg );
		
		m_pModifierPoolAgg = m_pTupScene->GetModifierPool();
		assert(m_pModifierPoolAgg);
		m_pTupModifierPool = new TupModifierPool( m_pModifierPoolAgg );

		try
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Converting from ZUp to YUp");
#ifdef USE_TUPPER3D
			TupCoordinateSystemConverter converter(*m_pTupScene,*m_pTupNodePool,*m_pTupObjectPool,*m_pTupModifierPool);
			bool bResult = converter.ConvertScene(TupScene::UP_AXIS_Y);
			if (!bResult)
				throw(PluginException(ERROR_FATAL,"Error Converting scene..."));
#else
			TransformScene();
#endif
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"MakeYUp - Process\r\n  %s",(const char *)(e)));
		}
		delete m_pTupScene;
		delete m_pTupNodePool;
		delete m_pTupObjectPool;
		delete m_pTupModifierPool;
	}
}

#ifndef USE_TUPPER3D

void MakeYUpFilter::TransformScene(void)
{
	// do the objects
	int numObjects = m_pTupObjectPool->GetNumObjects();
	for (int i=0;i<numObjects;i++)
	{
		if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(i);
		// if slot is empty then continue
		if (!pObjectAgg)
			continue;
		TransformObject(pObjectAgg);
	}

	// do the nodes
	int numNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex = 0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		// if slot is empty then continue
		if (!pNodeAgg)
			continue;
		TransformNode(pNodeAgg);
	}
}

void MakeYUpFilter::TransformObject(TupperwareAggregate *pObjectAgg)
{
	int i;
	Vector3 *pArray;
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType() == TupObject::TRI_MESH)
	{
		TupTriMesh tupTriMesh(pObjectAgg);
		// do positions
		int numPositionVerts;
		float *pPositionVerts;
		tupTriMesh.GetPositionVerts(&pPositionVerts,numPositionVerts);
		pArray = (Vector3 *)pPositionVerts;
		for (i=0;i<numPositionVerts;i++)
		{
			Vector3 &vector = pArray[i];
			float temp = vector.y( );
			vector.y( vector.z( ) );
			vector.z( -temp );
		}
		// do normals
		int numNormalVerts;
		float *pNormalVerts;
		tupTriMesh.GetNormalVerts(&pNormalVerts,numNormalVerts);
		pArray = (Vector3 *)pNormalVerts;
		for (i=0;i<numNormalVerts;i++)
		{
			Vector3 &vector = pArray[i];
			float temp = vector.y( );
			vector.y( vector.z( ) );
			vector.z( -temp );
		}
		// do face normals
		int numFaceNormalVerts;
		float *pFaceNormalVerts;
		tupTriMesh.GetFaceNormalVerts(&pFaceNormalVerts,numFaceNormalVerts);
		pArray = (Vector3 *)pFaceNormalVerts;
		for (i=0;i<numFaceNormalVerts;i++)
		{
			Vector3 &vector = pArray[i];
			float temp = vector.y( );
			vector.y( vector.z( ) );
			vector.z( -temp );
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
			TupSpline tupSpline(pSplineAgg);

			int numPoints;
			Vector3 *pPointLocations;
			Vector3 *pInVectors;
			Vector3 *pOutVectors;

			tupSpline.GetPointLocations((float **)&pPointLocations,numPoints);
			for (i=0;i<numPoints;i++)
			{
				Vector3 &vector = pPointLocations[i];
				float temp = vector.y( );
				vector.y( vector.z( ) );
				vector.z( -temp );
			}

			tupSpline.GetInVectors((float **)&pInVectors,numPoints);
			for (i=0;i<numPoints;i++)
			{
				Vector3 &vector = pInVectors[i];
				float temp = vector.y( );
				vector.y( vector.z( ) );
				vector.z( -temp );
			}

			tupSpline.GetOutVectors((float **)&pOutVectors,numPoints);
			for (i=0;i<numPoints;i++)
			{
				Vector3 &vector = pOutVectors[i];
				float temp = vector.y( );
				vector.y( vector.z( ) );
				vector.z( -temp );
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
					TransformModifier(pDerivedModifierAgg);
				}
			}
		}
	}
}

void MakeYUpFilter::TransformModifier(TupperwareAggregate *pDerivedModifierAgg)
{
	TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
	int modifierReference;
	tupDerivedModifier.GetModifierReference(modifierReference);
	TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
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
				Graphics4x4 *pBaseMatrix = (Graphics4x4 *)pFloatList;

				// convert matrix from z up to y up
				Graphics4x4 matrixYtoZ,matrixZtoY;

				matrixYtoZ.SetXRotation(-Math::Pi/2.0f);
				matrixZtoY.SetXRotation(Math::Pi/2.0f);

				// start with local(zup)->parent(zup) matrix
				// get local(zup)->parent(yup) matrix
				*pBaseMatrix = matrixZtoY*(*pBaseMatrix); 
				// get local(yup)->parent(yup) matrix
				*pBaseMatrix *= matrixYtoZ;

				int nBonesRefs = tupSkinModifier.GetNumBones();

				for (int boneIndex=0;boneIndex<nBonesRefs;boneIndex++)
				{
					TupperwareAggregate *pBoneAgg = tupSkinModifier.GetBone(boneIndex);
					TupSkinModifierBone tupBone(pBoneAgg);

					tupBone.GetBoneInitObjectTM(&pFloatList);
					Graphics4x4 *pBoneMatrix = (Graphics4x4 *)pFloatList;

					// start with local(zup)->parent(zup) matrix
					// get local(zup)->parent(yup) matrix
					*pBoneMatrix = matrixZtoY*(*pBoneMatrix); 
					// get local(yup)->parent(yup) matrix
					*pBoneMatrix *= matrixYtoZ;
				}
			}
	}
}

void MakeYUpFilter::TransformNode(TupperwareAggregate *pNodeAgg)
{
	if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

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
			Graphics4x4 *pMatrix = (Graphics4x4 *)pFloatList;
			// convert matrix from z up to y up
			Graphics4x4 matrixYtoZ,matrixZtoY;

			matrixYtoZ.SetXRotation(-Math::Pi/2.0f);
			matrixZtoY.SetXRotation(Math::Pi/2.0f);

			// start with local(zup)->parent(zup) matrix
			// get local(zup)->parent(yup) matrix
			*pMatrix = matrixZtoY*(*pMatrix); 
			// get local(yup)->parent(yup) matrix
			*pMatrix *= matrixYtoZ;
		}
	}
}

#endif