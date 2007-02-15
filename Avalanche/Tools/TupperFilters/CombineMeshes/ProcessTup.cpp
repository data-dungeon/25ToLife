////////////////////////////////////////////////////////////////////////////
//
// ProcessTup
//
// Scan through and add additional data to tupperware tree
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "CombineMeshesPch.h"
#include "ProcessTup.h"
#include "MergeMesh.h"

ProcessTup::ProcessTup(const FilterOptions &options,FilterDataManager &dataManager) : m_options(options), m_dataManager(dataManager)
{
	m_pSceneAgg = m_dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);

	char *pVersionString;
	m_pTupScene->GetVersion(&pVersionString);
	if (!pVersionString)
	{
		throw(PluginException(ERROR_FATAL,"Database Version not available in file %s.\r\nPlease export with updated TupExport plugin.",dataManager.GetInputFilename()));
	}
	// if the version does not match
	else if (strcmp(pVersionString,TupScene::GetSceneVersion()))
	{
		throw(PluginException(ERROR_FATAL,"Database Version in file %s is %s should be %s.\r\nPlease export with updated TupExport plugin.",dataManager.GetInputFilename(),pVersionString,TupScene::GetSceneVersion()));
	}

	m_pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(m_pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);

	m_pTupScene->GetRootNodeReference(m_rootNodeReference);
	assert(m_rootNodeReference>=0);
	m_pRootNodeAgg = m_pTupNodePool->GetNode(m_rootNodeReference);
	assert(m_pRootNodeAgg);
	m_pTupRootNode = new TupNode(m_pRootNodeAgg);

	m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(m_pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool( m_pObjectPoolAgg );

	m_pModifierPoolAgg = m_pTupScene->GetModifierPool();
	assert(m_pModifierPoolAgg);
	m_pTupModifierPool = new TupModifierPool( m_pModifierPoolAgg );

	m_pMaterialPoolAgg = m_pTupScene->GetMaterialPool();
	assert(m_pMaterialPoolAgg);
	m_pTupMaterialPool = new TupMaterialPool( m_pMaterialPoolAgg );

	m_pTupNodeConfigRoot = new TupConfigRoot;
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = m_pTupScene->GetNodeAttributeTree();
	if (pSceneNodeAttributeTreeAgg)
	{
		m_pTupNodeConfigRoot->BuildConfigTree(pSceneNodeAttributeTreeAgg);
	}

	m_pTupFaceConfigRoot = new TupConfigRoot;
	// Face Attribute Tree
	TupperwareAggregate *pSceneFaceAttributeTreeAgg = m_pTupScene->GetFaceAttributeTree();
	if (pSceneFaceAttributeTreeAgg)
	{
		m_pTupFaceConfigRoot->BuildConfigTree(pSceneFaceAttributeTreeAgg);
	}
}

ProcessTup::~ProcessTup()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupRootNode;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupMaterialPool;
	delete m_pTupNodeConfigRoot;
	delete m_pTupFaceConfigRoot;
}


void ProcessTup::Process(void)
{
	MergeMeshesManager mergeMeshesManager(*m_pTupNodePool,*m_pTupObjectPool,
													  *m_pTupModifierPool,*m_pTupMaterialPool,
													  *m_pTupNodeConfigRoot,*m_pTupFaceConfigRoot);

	bool bAnyMerged = false;

	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		if (nodeIndex==m_rootNodeReference)
			continue; // don't bother with root node

		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
			continue; // must be a group (no object reference)

		char *pNodeAttributesString;
		tupNode.GetUserProperties( &pNodeAttributesString );

		TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey( "instance_type" );
		tupKeyValueContext.AddKey( "layerPositionTolerance" );

		const int INSTANCE_TYPE = 0;
		const int LAYER_POSITION_TOLERANCE = 1;
		m_pTupNodeConfigRoot->GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );

		if ( tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp( tupKeyValueContext.GetValueAsString(INSTANCE_TYPE), "combine_mesh_group" ) == 0 )
		{
			float positionTolerance=0.01f;
			if ( tupKeyValueContext.ValueValidAt(LAYER_POSITION_TOLERANCE))
			{
				positionTolerance = tupKeyValueContext.GetValueAsFloat(LAYER_POSITION_TOLERANCE);
				TupString toleranceString = tupKeyValueContext.GetValueAsString(LAYER_POSITION_TOLERANCE);
			}
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Setting Position Tolerance to %f",positionTolerance);
			mergeMeshesManager.SetPositionTolerance(positionTolerance);
			int numChildNodes = tupNode.GetNumChildNodes();
			TupArray<int> instancesToCombine;
			TupArray<TupString> instanceNames;

			for (int childIndex=0;childIndex<numChildNodes;childIndex++)
			{
				int childNodeRef;
				tupNode.GetChildNodeReference(childIndex,childNodeRef);
				TupperwareAggregate *pChildAgg = m_pTupNodePool->GetNode(childNodeRef);
				TupNode tupChildNode(pChildAgg);
				char *pChildName;
				tupChildNode.GetName(&pChildName);
				int childObjectRef;
				tupChildNode.GetObjectReference(childObjectRef);
				if (childObjectRef==-1)
					continue;
				// needs to point to a mesh
				TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,childObjectRef);
				if (!pBaseMeshAgg)
					continue;

				char *pChildNodeAttributesString;
				tupChildNode.GetUserProperties( &pChildNodeAttributesString );

				TupKeyValue tupKeyValue( pChildNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
				TupKeyValueContext tupKeyValueContext;
				tupKeyValueContext.AddKey( "instance_type" );
				tupKeyValueContext.AddKey("env_type");
				const int ENV_TYPE = 1;

				m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
				// if set to something other than normal then skip
				if (tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"normal")==0)
				{
					// should not be an environmental object
					if (strcmp(tupKeyValueContext.GetValueAsString(ENV_TYPE),"None")==0)
					{
						instancesToCombine.Add(childNodeRef);
						TupString childName(pChildName);
						instanceNames.Add(childName);
					}
				}
			}
			// if more than one instance in array
			int numInstancesToCombine = instancesToCombine.GetSize();
			if (numInstancesToCombine>1)
			{
				char *pGroupName;
				tupNode.GetName(&pGroupName);
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Combining child instances from node %s",pGroupName);
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Base Instance from node %s",(const char *)instanceNames[0]);
				mergeMeshesManager.AddInstances(instancesToCombine);
				mergeMeshesManager.AddToScene(nodeIndex);
				bAnyMerged = true;
			}
		}
	}
	if (bAnyMerged)
	{
		Keeper<int> usedNodes;
		Keeper<int> usedObjects;
		Keeper<int> usedModifiers;
		AnnotateHelper::CalculateUsedNodesObjectsAndModifiers(m_rootNodeReference,*m_pTupNodePool,*m_pTupObjectPool,*m_pTupModifierPool,usedNodes,usedObjects,usedModifiers);
		int numNodes = m_pTupNodePool->GetNumNodes();
		for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
		{
			TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
			if (pNodeAgg)
			{
				// if not used
				if (usedNodes.GetIndex(nodeIndex)==-1)
				{
					m_pTupNodePool->DeleteNode(nodeIndex);
				}
			}
		}
		int numObjects = m_pTupObjectPool->GetNumObjects();
		for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
			if (pObjectAgg)
			{
				// if not used
				if (usedObjects.GetIndex(objectIndex)==-1)
				{
					m_pTupObjectPool->DeleteObjectAggregate(objectIndex);
				}
			}
		}
		int numModifiers = m_pTupModifierPool->GetNumModifiers();
		for (int modifierIndex=0;modifierIndex<numModifiers;modifierIndex++)
		{
			TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierIndex);
			if (pModifierAgg)
			{
				// if not used
				if (usedModifiers.GetIndex(modifierIndex)==-1)
				{
					m_pTupModifierPool->DeleteModifier(modifierIndex);
				}
			}
		}
	}
}


