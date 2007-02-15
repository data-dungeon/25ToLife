////////////////////////////////////////////////////////////////////////////
//
// ProcessXRefs
//
// Class to handle processing the xrefs
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ProcessXrefs.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 9/27/02    Time: 12:22p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * better log support
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * File Dependency checks
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 6/11/02    Time: 12:36p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * nobones remap when merging
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/28/02    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Moved MergeXRefs from annotate filter to its own filter
 */

////////////////////////////////////////////////////////////////////////////

#include "MergeXRefsPch.h"
#include "ProcessXRefs.h"
#include "XRefManager.h"
#include "XRefHolder.h"

ProcessXRefs::ProcessXRefs(const FilterOptions &options,FilterDataManager &dataManager,bool bProcessStep) : m_options(options), m_dataManager(dataManager)
{
	m_pSceneAgg = m_dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);

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

	m_pMapPoolAgg = m_pTupScene->GetMapPool();
	assert(m_pMapPoolAgg);
	m_pTupMapPool = new TupMapPool( m_pMapPoolAgg );

	m_pImagePoolAgg = m_pTupScene->GetImagePool();
	assert(m_pImagePoolAgg);
	m_pTupImagePool = new TupImagePool( m_pImagePoolAgg );

	m_pTupConfigRoot = new TupConfigRoot;
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = m_pTupScene->GetNodeAttributeTree();
	if (pSceneNodeAttributeTreeAgg)
	{
		m_pTupConfigRoot->BuildConfigTree(pSceneNodeAttributeTreeAgg);
	}
	else if (bProcessStep)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}
}

ProcessXRefs::~ProcessXRefs()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupRootNode;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;
	delete m_pTupConfigRoot;
}


void ProcessXRefs::Process(void)
{
	// bring in the xrefs (this should be first)
	MergeXRefs();
	if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
}

void ProcessXRefs::MergeXRefs(void)
{
	// set up the manager
	XRefManager xRefManager(m_dataManager,
							*m_pTupScene,
							*m_pTupNodePool,
							*m_pTupObjectPool,
							*m_pTupModifierPool,
							*m_pTupMaterialPool,
							*m_pTupMapPool,
							*m_pTupImagePool);

	int objectsMerged = 0;
	while (1)
	{
		bool foundXRef = false;
		for (int objectIndex=0;objectIndex<m_pTupObjectPool->GetNumObjects();objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

			// continue if aggregate is 0
			if (!pObjectAgg)
				continue;

			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType()==TupObject::XREF)
			{
				TupXRef tupXRef(pObjectAgg);
				char *pFilename;
				tupXRef.GetFilename(&pFilename);
				char *pNodename;
				tupXRef.GetNodeName(&pNodename);
				TupString nodeName(pNodename);
				bool bNoBonesFound = false;
				// change the name if no bones is found
				int bonesFoundIndex = nodeName.Find("nobones");
				if (bonesFoundIndex != -1)
				{
					bNoBonesFound = true;
					nodeName = nodeName.Left(bonesFoundIndex);
				}

				int sceneIndex = xRefManager.AddScene(pFilename);
				TupNodePool &tupOtherNodePool = xRefManager.GetNodePool(sceneIndex);

				int numOtherNodes = tupOtherNodePool.GetNumNodes();
				for (int otherNodeIndex = 0;otherNodeIndex<numOtherNodes;otherNodeIndex++)
				{
					TupperwareAggregate *pOtherNode = tupOtherNodePool.GetNode(otherNodeIndex);

					// continue if aggregate is 0
					if (!pOtherNode)
						continue;

					TupNode tupOtherNode(pOtherNode);
					char *pOtherNodeName;
					tupOtherNode.GetName(&pOtherNodeName);
					// check if the name in the xref matches this node
					if (!strcmp(nodeName,pOtherNodeName))
					{
						objectsMerged++;
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging XRef Node %s from File %s",(const char *)nodeName,pFilename);
// don't do the probigation of attributes at this time
#if 0
						// get the attributes and apply to the nodes in the scene that use this xref
						char *pNodeAttributes;
						tupOtherNode.GetUserProperties(&pNodeAttributes);
						if (pNodeAttributes)
						{
							ApplyXRefAttributes(objectIndex,pNodeAttributes);
						}
#endif
						// get object that the xref is pointing to 
						int otherObjectRef;
						tupOtherNode.GetObjectReference(otherObjectRef);
						assert(otherObjectRef!=-1);
						// bring into the scene (under a new reference)
						int otherObjectRemappedRef = xRefManager.GetRemappedObject(sceneIndex,otherObjectRef);
						assert(otherObjectRemappedRef!=-1);
						// remap all objects and nodes in the scene that point to me to the new one
						RemapObjectRefs(objectIndex,otherObjectRemappedRef);
						// delete the xref from the pool
						m_pTupObjectPool->DeleteObjectAggregate(objectIndex);
						foundXRef = true;
						break;
					}
				}
				if (!foundXRef)
				{
					if (bNoBonesFound)
					{
						throw(PluginException(ERROR_FATAL,"Cannot find bone node %s from nobones node %s in file %s",pNodename,(const char *)nodeName),pFilename);
					}
					else
					{
						throw(PluginException(ERROR_FATAL,"Cannot find node %s in file %s",pNodename,pFilename));
					}
				}
			}
		}
		// continue looping until there are no more xrefs
		if (!foundXRef)
			break;
	}
	if (objectsMerged)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merged %d objects into scene",objectsMerged);
		int numOtherScenes = m_dataManager.GetNumOtherScenes();
		int otherSceneIndex;
		for (otherSceneIndex=0;otherSceneIndex<numOtherScenes;otherSceneIndex++)
		{
			const char *pSceneFilename = m_dataManager.GetOtherSceneFilename(otherSceneIndex);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Used Scene %s for XRefs",pSceneFilename);
		}
	}
}

void ProcessXRefs::ApplyXRefAttributes(int xrefObjectRef,const char *pXRefNodeAttributes)
{
	assert(pXRefNodeAttributes);

	TupKeyValue xRefKeyValue(pXRefNodeAttributes,BEGIN_BLOCK,END_BLOCK);

	int numNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		int objectRef;
		tupNode.GetObjectReference(objectRef);
		assert(objectRef!=-1);
		// find the base object for this node
		int baseRef = AnnotateHelper::FindBaseObjectRef(*m_pTupObjectPool,objectRef);
		// if it points to my xref then great
		if (baseRef == xrefObjectRef)
		{
			char *pNodeAttributes;
			tupNode.GetUserProperties(&pNodeAttributes);
			if (pNodeAttributes)
			{
				// start with a copy of the xref attributes
				TupKeyValue tempKeyVal(xRefKeyValue);
				TupKeyValue nodeKeyValue(pNodeAttributes,BEGIN_BLOCK,END_BLOCK);
				// merge the node keys on top of the xref keys
				tempKeyVal.MergeKeyValues(nodeKeyValue);
				// create a string that holds the attributes as well as the extra stuff
				TupString newAttributeString = tempKeyVal.BuildString(pXRefNodeAttributes,pNodeAttributes,BEGIN_BLOCK,END_BLOCK);
				tupNode.AddUserProperties(newAttributeString);
			}
			// we don't have any on this node
			else
			{
				tupNode.AddUserProperties(pXRefNodeAttributes); // copy the xref one to the node
			}
		}
	}
}

void ProcessXRefs::RemapObjectRefs(int xrefObjectRef,int newObjectRef)
{
	// remap nodes that are pointing to xrefObjectRef to newObjectRef
	int numNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		int objectRef;
		tupNode.GetObjectReference(objectRef);
		if (objectRef == xrefObjectRef)
		{
			tupNode.AddObjectReference(newObjectRef);
		}
	}

	// remap derived objects that are pointing to xrefObjectRef to newObjectRef
	int numObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			// try the one I point to
			TupDerived tupDerived(pObjectAgg);
			int childObjectRef;
			tupDerived.GetObjectReference(childObjectRef);
			assert(childObjectRef>=0);
			if (childObjectRef==xrefObjectRef)
			{
				tupDerived.AddObjectReference(newObjectRef);
			}
		}
	}
}

void ProcessXRefs::GetFileDependencies(FilePathArray &filePathArray)
{
	// set up the manager
	XRefManager xRefManager(m_dataManager,
							*m_pTupScene,
							*m_pTupNodePool,
							*m_pTupObjectPool,
							*m_pTupModifierPool,
							*m_pTupMaterialPool,
							*m_pTupMapPool,
							*m_pTupImagePool);

	for (int objectIndex=0;objectIndex<m_pTupObjectPool->GetNumObjects();objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// continue if aggregate is 0
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::XREF)
		{
			TupXRef tupXRef(pObjectAgg);
			char *pXRefFilename;
			tupXRef.GetFilename(&pXRefFilename);
			char *pXRefNodeName;
			tupXRef.GetNodeName(&pXRefNodeName);
			TupString xRefNodeName(pXRefNodeName);
			bool bNoBonesFound = false;
			// change the name if no bones is found
			int bonesFoundIndex = xRefNodeName.Find("nobones");
			if (bonesFoundIndex != -1)
			{
				bNoBonesFound = true;
				xRefNodeName = xRefNodeName.Left(bonesFoundIndex);
			}
			AddXRefFileDependencies(xRefManager,pXRefFilename,pXRefNodeName,filePathArray);
		}
	}
}

void ProcessXRefs::AddXRefFileDependencies(XRefManager &xRefManager,const char *pFilename,const char *pNodeName,FilePathArray &filePathArray)
{
	int sceneIndex = xRefManager.AddScene(pFilename);
	filePathArray.AddFilePath(m_dataManager.GetOtherSceneFilename(sceneIndex),FilePathArray::FILE_INPUT|FilePathArray::FILE_ACCESS_FATAL);
	TupNodePool &tupOtherNodePool = xRefManager.GetNodePool(sceneIndex);
	TupObjectPool &tupOtherObjectPool = xRefManager.GetObjectPool(sceneIndex);

	bool foundXRef = false;

	int numOtherNodes = tupOtherNodePool.GetNumNodes();
	for (int otherNodeIndex = 0;otherNodeIndex<numOtherNodes;otherNodeIndex++)
	{
		TupperwareAggregate *pOtherNode = tupOtherNodePool.GetNode(otherNodeIndex);

		// continue if aggregate is 0
		if (!pOtherNode)
			continue;

		TupNode tupOtherNode(pOtherNode);
		char *pOtherNodeName;
		tupOtherNode.GetName(&pOtherNodeName);
		// check if the name in the xref matches this node
		if (!strcmp(pNodeName,pOtherNodeName))
		{
			// get object that the xref is pointing to 
			int otherObjectRef;
			tupOtherNode.GetObjectReference(otherObjectRef);
			assert(otherObjectRef!=-1);
			TupperwareAggregate *pBaseXRefAgg = AnnotateHelper::FindBaseXRef(tupOtherObjectPool,otherObjectRef);
			if (pBaseXRefAgg)
			{
				TupXRef tupXRef(pBaseXRefAgg);
				char *pXRefFilename;
				tupXRef.GetFilename(&pXRefFilename);
				char *pXRefNodeName;
				tupXRef.GetNodeName(&pXRefNodeName);
				TupString xRefNodeName(pXRefNodeName);
				bool bNoBonesFound = false;
				// change the name if no bones is found
				int bonesFoundIndex = xRefNodeName.Find("nobones");
				if (bonesFoundIndex != -1)
				{
					bNoBonesFound = true;
					xRefNodeName = xRefNodeName.Left(bonesFoundIndex);
				}
				AddXRefFileDependencies(xRefManager,pXRefFilename,pXRefNodeName,filePathArray);
			}
			foundXRef = true;
			break;
		}
	}
	if (!foundXRef)
	{
		throw(PluginException(ERROR_FATAL,"Cannot find node %s in file %s",pNodeName,pFilename));
	}
}
