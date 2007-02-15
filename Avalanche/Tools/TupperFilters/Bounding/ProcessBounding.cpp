////////////////////////////////////////////////////////////////////////////
//
// ProcessStamps.h
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ProcessBounding.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:08a
 * Updated in $/Avalanche/tools/TupperFilters/Bounding
 * moved tupMesh to tupTriMesh
 * 
 * *****************  Version 3  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:54p
 * Updated in $/Avalanche/tools/TupperFilters/Bounding
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/Bounding
 * new bounding filter for use in annotate and break large meshes
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/31/03    Time: 3:14p
 * Created in $/Avalanche/tools/TupperFilters/Bounding
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_STAMPS_H
#define PROCESS_STAMPS_H

#include "BoundingPch.h"
#include "ProcessBounding.h"

ProcessBoundingManager::ProcessBoundingManager(const FilterOptions &options,FilterDataManager &dataManager) : m_options(options), m_dataManager(dataManager) 
{
	m_pSceneAgg = m_dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);
	
	m_pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(m_pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);
	
	m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(m_pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool( m_pObjectPoolAgg );
	
	m_pTupNodeConfigRoot = new TupConfigRoot;
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = m_pTupScene->GetNodeAttributeTree();
	if (!pSceneNodeAttributeTreeAgg)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}
	m_pTupNodeConfigRoot->BuildConfigTree(pSceneNodeAttributeTreeAgg);

	m_pBoundingBoxMin = new Vector3;
	m_pBoundingBoxMax = new Vector3;
	m_pBoundingBoxMin->Set( MAXFLOAT32, MAXFLOAT32, MAXFLOAT32 );
	m_pBoundingBoxMax->Set( -MAXFLOAT32, -MAXFLOAT32, -MAXFLOAT32 );
}

ProcessBoundingManager::~ProcessBoundingManager()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupObjectPool;
	delete m_pTupNodeConfigRoot;

	delete m_pBoundingBoxMin;
	delete m_pBoundingBoxMax;

}

void ProcessBoundingManager::Process(void)
{
	AddObjectBounding();
	AddNodeBounding();

	m_pSceneAgg->AddListFloat("Bounding_BoundingBoxMin",NULL,(float *)m_pBoundingBoxMin,3);
	m_pSceneAgg->AddListFloat("Bounding_BoundingBoxMax",NULL,(float *)m_pBoundingBoxMax,3);
}

void ProcessBoundingManager::AddObjectBounding(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			TupTriMesh tupTriMesh(pObjectAgg);
			Bounding::AddObjectBounding(tupTriMesh);
		}
	}
}

// this also adds bounding boxes to the world bounding box
void ProcessBoundingManager::AddNodeBounding(void)
{
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		// check if this node is an environmental instance
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many named points we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
		TupKeyValueContext tupKeyValueContext;

		const int INSTANCE_TYPE = 0;
		const int ENV_TYPE = 1;
		tupKeyValueContext.AddKey("instance_type");
		tupKeyValueContext.AddKey("env_type");

		if (!Bounding::AddNodeBounding(tupNode,*m_pTupObjectPool))
			continue;

		Vector3 *pBoundingBoxMin = (Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
		Vector3 *pBoundingBoxMax = (Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();

		// add to world bounding box
		Bounding::AddToBoundingBox(*m_pBoundingBoxMin,*m_pBoundingBoxMax,*pBoundingBoxMin);
		Bounding::AddToBoundingBox(*m_pBoundingBoxMin,*m_pBoundingBoxMax,*pBoundingBoxMax);

		// warn if environmental object's hot spots are outside of bounding box
		m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
		// if set to something other than normal then skip
		if (tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"normal")!=0)
			continue;

		// check if environmental instance is outside of bounding
		if (tupKeyValueContext.ValueValidAt(ENV_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(ENV_TYPE),"None")==0)
			continue;

		TupperwareAggregate *pTransformAgg = tupNode.GetTransform();

		Graphics4x4 mLocalToWorld;
		TupTransform tupTransform(pTransformAgg);
		if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
		{
			TupStaticMatrix tupStaticMatrix(pTransformAgg);
			float *pMatrix;
			tupStaticMatrix.GetMatrix(&pMatrix);
			
			mLocalToWorld = Graphics4x4(
				pMatrix[0], pMatrix[1], pMatrix[2],
				pMatrix[4], pMatrix[5], pMatrix[6],
				pMatrix[8], pMatrix[9], pMatrix[10],
				pMatrix[12], pMatrix[13], pMatrix[14]);      // translation
		}
		else
		{
			mLocalToWorld.Identity(); // if we don't have a static matrix set to identity
		}
		Vector3 hotSpotVert;
		hotSpotVert = mLocalToWorld.GetTranslation( );
		Vector3 adjustment;
		adjustment.Set(10.0f,10.0f,10.0f); // 10 meters

		Vector3 boundingBoxAdjustedMin = (*pBoundingBoxMin) - adjustment;
		Vector3 boundingBoxAdjustedMax = (*pBoundingBoxMax) + adjustment;
		if (!Bounding::ContainsPoint(boundingBoxAdjustedMin,boundingBoxAdjustedMax,hotSpotVert))
		{
			char *pName;
			tupNode.GetName(&pName);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Instance Hotspot for %s is not with 10 meters of bounding box.",pName);
		}
	}
}



