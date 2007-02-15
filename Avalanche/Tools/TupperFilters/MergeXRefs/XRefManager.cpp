////////////////////////////////////////////////////////////////////////////
//
// XRefHolder
//
// Class to handle keeping track of data for another scene (used by xrefs)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: XRefManager.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 9:37a
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Modifier access using slots
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 10:31a
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Check for database version.
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 6/04/02    Time: 3:25p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Xref Paths
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/28/02    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Moved MergeXRefs from annotate filter to its own filter
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 4:24p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Rearranged tupper3d usage on process tup class
 * Merge xref complete (does not do materials, maps, images)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/02    Time: 4:15p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Merge XRef Phase A
 */

////////////////////////////////////////////////////////////////////////////

#include "MergeXRefsPch.h"
#include "XRefManager.h"
#include "XRefHolder.h"

// XRefManager methods
XRefManager::XRefManager(FilterDataManager &dataManager,
						 TupScene &tupScene,
						 TupNodePool &tupNodePool,
						 TupObjectPool &tupObjPool,
						 TupModifierPool &tupModifierPool,
						 TupMaterialPool &tupMaterialPool,
						 TupMapPool &tupMapPool,
						 TupImagePool &tupImagePool) :
	m_dataManager(dataManager),
	m_tupScene(tupScene),
	m_tupNodePool(tupNodePool),
	m_tupObjPool(tupObjPool),
	m_tupModifierPool(tupModifierPool),
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_tupImagePool(tupImagePool)
{
	int numScenes = m_dataManager.GetNumOtherScenes();
	if (numScenes)
	{
		m_holderArray.SetSize(numScenes);
		for (int sceneIndex=0;sceneIndex<numScenes;sceneIndex++)
		{
			TupperwareAggregate *pSceneAgg = m_dataManager.GetOtherSceneRoot(sceneIndex);
			assert(pSceneAgg);
			m_holderArray[sceneIndex] = new XRefHolder(pSceneAgg,m_dataManager.GetOtherSceneFilename(sceneIndex));
		}
	}
}

XRefManager::~XRefManager()
{
	int numScenes = m_holderArray.GetSize();
	for (int sceneIndex=0;sceneIndex<numScenes;sceneIndex++)
	{
		delete m_holderArray[sceneIndex];
	}
}

int XRefManager::AddScene(const char *pFilename)
{
	int sceneIndex = m_dataManager.GetOtherSceneIndex(pFilename);
	TupperwareAggregate *pSceneAgg = m_dataManager.GetOtherSceneRoot(sceneIndex);

	if (!pSceneAgg)
		throw(PluginException(ERROR_FATAL,"Cannot load scene file %s",pFilename));

	// keep the holder array in sync with the scenes in the data manager
	int oldSize = m_holderArray.GetSize();
	int newSize = m_dataManager.GetNumOtherScenes();
	assert(oldSize<=newSize);
	if (oldSize<newSize)
	{
		for (int fixIndex = oldSize;fixIndex<newSize;fixIndex++)
		{
			TupperwareAggregate *pFixAgg = m_dataManager.GetOtherSceneRoot(fixIndex);
			XRefHolder *pNewXRefHolder = new XRefHolder(pFixAgg,pFilename);
			m_holderArray.Add(pNewXRefHolder);
		}
	}
	return sceneIndex;
}

// check if the scene is within the manager and return the index if so
// if not return -1
int XRefManager::GetSceneIndex(const char *pFilename) const
{
	TupString relativePath,fullPath,name,ext;
	// get full path of filename
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,pFilename,m_dataManager.GetInputFilename());

	int numScenes = m_dataManager.GetNumOtherScenes();
	for (int sceneIndex=0;sceneIndex<numScenes;sceneIndex++)
	{
		if (!stricmp(fullPath,m_dataManager.GetOtherSceneFilename(sceneIndex)))
		{
			return sceneIndex;
		}
	}
	return -1;
}

int XRefManager::GetNumScenes(void) const
{
	int holderSize = m_holderArray.GetSize();
	int dataManagerSize = m_dataManager.GetNumOtherScenes();
	assert(holderSize==dataManagerSize);
	return holderSize;
}

TupperwareAggregate *XRefManager::GetSceneAgg(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return m_holderArray[sceneIndex]->GetSceneAgg();
}

TupScene &XRefManager::GetScene(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return  *m_holderArray[sceneIndex]->GetScene();
}

TupNodePool &XRefManager::GetNodePool(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return  *m_holderArray[sceneIndex]->GetNodePool();
}

TupObjectPool &XRefManager::GetObjectPool(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return  *m_holderArray[sceneIndex]->GetObjectPool();
}

TupModifierPool &XRefManager::GetModifierPool(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return  *m_holderArray[sceneIndex]->GetModifierPool();
}

TupMaterialPool &XRefManager::GetMaterialPool(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return  *m_holderArray[sceneIndex]->GetMaterialPool();
}

TupMapPool &XRefManager::GetMapPool(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return  *m_holderArray[sceneIndex]->GetMapPool();
}

TupImagePool &XRefManager::GetImagePool(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return  *m_holderArray[sceneIndex]->GetImagePool();
}

int XRefManager::GetRootNodeReference(int sceneIndex)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	int rootNodeReference;
	m_holderArray[sceneIndex]->GetScene()->GetRootNodeReference(rootNodeReference);
	return rootNodeReference;
}

int XRefManager::GetRemappedNode(int sceneIndex,int nXRefReference)
{
	int remappedRef = FindRemap(sceneIndex,nXRefReference,RefRemap::REMAP_NODE);
	if (remappedRef!=-1)
		return remappedRef;
	return CopyNodeFromScene(sceneIndex,nXRefReference);
}

int XRefManager::GetRemappedObject(int sceneIndex,int nXRefReference)
{
	int remappedRef = FindRemap(sceneIndex,nXRefReference,RefRemap::REMAP_OBJECT);
	if (remappedRef!=-1)
		return remappedRef;
	return CopyObjectFromScene(sceneIndex,nXRefReference);
}

int XRefManager::GetRemappedModifier(int sceneIndex,int nXRefReference)
{
	int remappedRef = FindRemap(sceneIndex,nXRefReference,RefRemap::REMAP_MODIFIER);
	if (remappedRef!=-1)
		return remappedRef;
	return CopyModifierFromScene(sceneIndex,nXRefReference);
}

int XRefManager::GetRemappedMaterial(int sceneIndex,int nXRefReference)
{
	int remappedRef = FindRemap(sceneIndex,nXRefReference,RefRemap::REMAP_MATERIAL);
	if (remappedRef!=-1)
		return remappedRef;
	return CopyMaterialFromScene(sceneIndex,nXRefReference);
}

int XRefManager::GetRemappedMap(int sceneIndex,int nXRefReference)
{
	int remappedRef = FindRemap(sceneIndex,nXRefReference,RefRemap::REMAP_MAP);
	if (remappedRef!=-1)
		return remappedRef;
	return CopyMapFromScene(sceneIndex,nXRefReference);
}

int XRefManager::GetRemappedImage(int sceneIndex,int nXRefReference)
{
	return FindRemap(sceneIndex,nXRefReference,RefRemap::REMAP_IMAGE);
	int remappedRef = FindRemap(sceneIndex,nXRefReference,RefRemap::REMAP_IMAGE);
	if (remappedRef!=-1)
		return remappedRef;
	return CopyImageFromScene(sceneIndex,nXRefReference);
}

int XRefManager::FindRemap(int sceneIndex,int nXRefReference,RefRemap::RemapType type) const
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	return m_holderArray[sceneIndex]->GetRemapManager()->Find(nXRefReference,type);
}

void XRefManager::AddRemap(int sceneIndex,int nXRefReference,int nSceneReference,RefRemap::RemapType type)
{
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	m_holderArray[sceneIndex]->GetRemapManager()->Add(nXRefReference,nSceneReference,type);
}

// copy methods
// these methods will copy an referenced element from the xref scene into the main scene
int XRefManager::CopyNodeFromScene(int sceneIndex,int nXRefReference)
{
	// get the source node aggregate
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	TupperwareAggregate *pSourceNodeAgg = m_holderArray[sceneIndex]->GetNodePool()->GetNode(nXRefReference);
	assert(pSourceNodeAgg);

	// create a new entry in the node pool
	int newNodeIndex = m_tupNodePool.AddNode();
	TupperwareAggregate *pNewNodeAgg = m_tupNodePool.GetNode(newNodeIndex);
	assert(pNewNodeAgg);

	// copy stuff from the source node to the new one
	for (TupperwareIterator iter = pSourceNodeAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		pNewNodeAgg->Copy(iter.GetAsAtom());
	}

	// add index into the remap manager
	AddRemap(sceneIndex,nXRefReference,newNodeIndex,RefRemap::REMAP_NODE);

	// resolve any references used by the new node
	TupNode tupNode(pNewNodeAgg);

	// do the object reference
	int objectRef;
	TupperwareScalar *pObjectRefScalar = tupNode.GetObjectReference(objectRef);
	assert(pObjectRefScalar);
	assert(objectRef!=-1);
	// get object in our scene (may copy it in)
	int newObjectRef = GetRemappedObject(sceneIndex,objectRef);
	// change scalar value to new reference
	pObjectRefScalar->SetAsInt(newObjectRef); 

	// do the material reference
	int materialRef;
	TupperwareScalar *pMaterialRefScalar = tupNode.GetMaterialReference(materialRef);
	// if we have a valid material
	if (pMaterialRefScalar && materialRef!=-1)
	{
		// get object in our scene (may copy it in)
		int newMaterialRef = -1; //GetRemappedMaterial(sceneIndex,materialRef);
		// change scalar value to new reference
		pMaterialRefScalar->SetAsInt(newMaterialRef); 
	}

	// do the parent node reference
	int parentNodeRef;
	TupperwareScalar *pParentNodeRefScalar = tupNode.GetParentNodeReference(parentNodeRef);
	assert(pParentNodeRefScalar);
	assert(parentNodeRef!=-1); // this better have a parent (we don't do root nodes)
	// check if our parent is the root node
	int rootNodeReference = GetRootNodeReference(sceneIndex);
	// if our parent is the root node then make our new parent the root node of the new scene
	if (parentNodeRef == rootNodeReference)
	{
		int newParentNodeRef;
		m_tupScene.GetRootNodeReference(newParentNodeRef);
		assert(newParentNodeRef!=-1);
		pParentNodeRefScalar->SetAsInt(newParentNodeRef);
		// add me as a child of the root
		TupperwareAggregate *pRootNodeAgg = m_tupNodePool.GetNode(newParentNodeRef);
		assert(pRootNodeAgg);
		TupNode tupRootNode(pRootNodeAgg);
		tupRootNode.AddChildNodeReference(newNodeIndex);
	}
	// if our parent is not the root node then get the parent node (may copy it in)
	else
	{
		// get object in our scene (may copy it in)
		int newParentNodeRef = GetRemappedNode(sceneIndex,parentNodeRef);
		assert(newParentNodeRef!=-1);
		// change scalar value to new reference
		pParentNodeRefScalar->SetAsInt(newParentNodeRef); 
	}

	// do the children node references
	int numChildNodes = tupNode.GetNumChildNodes();
	for (int childIndex=0;childIndex<numChildNodes;childIndex++)
	{
		int childNodeRef;
		TupperwareScalar *pChildNodeRefScalar = tupNode.GetChildNodeReference(childIndex,childNodeRef);
		assert(pChildNodeRefScalar);
		assert(childNodeRef!=-1);
		// get child reference (may copy in)
		int newChildNodeRef = GetRemappedNode(sceneIndex,childNodeRef);
		assert(newChildNodeRef!=-1);
		pChildNodeRefScalar->SetAsInt(newChildNodeRef);
	}
	return newNodeIndex;
}

int XRefManager::CopyObjectFromScene(int sceneIndex,int nXRefReference)
{
	// get the source object aggregate
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	TupperwareAggregate *pSourceObjectAgg = m_holderArray[sceneIndex]->GetObjectPool()->GetObjectAggregate(nXRefReference);
	assert(pSourceObjectAgg);

	// create a new entry in the object pool
	int newObjectIndex = m_tupObjPool.AddObjectAggregate();
	TupperwareAggregate *pNewObjectAgg = m_tupObjPool.GetObjectAggregate(newObjectIndex);
	assert(pNewObjectAgg);

	// copy stuff from the source object to the new one
	for (TupperwareIterator iter = pSourceObjectAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		pNewObjectAgg->Copy(iter.GetAsAtom());
	}

	// add index into the remap manager
	AddRemap(sceneIndex,nXRefReference,newObjectIndex,RefRemap::REMAP_OBJECT);

	// resolve any references used by the new object
	TupObject tupObject(pNewObjectAgg);

	int objectType = tupObject.GetObjectType();
	switch (objectType)
	{
		// handle derived objects (only one that references other items for now)
		case TupObject::DERIVED:
		{
			TupDerived tupDerived(pNewObjectAgg);
			int numModifierSlots = tupDerived.GetNumModifierSlots();
			for (int modifierIndex=0;modifierIndex<numModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pModifierAgg);
					int modifierRef;
					TupperwareScalar *pModifierRefScalar = tupDerivedModifier.GetModifierReference(modifierRef);
					assert(pModifierRefScalar);
					assert(modifierRef!=-1);
					// get modifier in our scene (may copy it in)
					int newModifierRef = GetRemappedModifier(sceneIndex,modifierRef);
					assert(newModifierRef!=-1);
					// change scalar value to new reference
					pModifierRefScalar->SetAsInt(newModifierRef); 
				}
			}
			// to the object that I am pointing to
			int derivedObjectRef;
			TupperwareScalar *pDerivedObjectRefScalar = tupDerived.GetObjectReference(derivedObjectRef);
			assert(derivedObjectRef!=-1);
			assert(pDerivedObjectRefScalar);
			int newDerivedObjectRef = GetRemappedObject(sceneIndex,derivedObjectRef);
			assert(newDerivedObjectRef!=-1);
			pDerivedObjectRefScalar->SetAsInt(newDerivedObjectRef);
		}
		break;
	}
	return newObjectIndex;
}

int XRefManager::CopyModifierFromScene(int sceneIndex,int nXRefReference)
{
	// get the source modifier aggregate
	int holderSize = m_holderArray.GetSize();
	assert(sceneIndex<holderSize);
	TupperwareAggregate *pSourceModifierAgg = m_holderArray[sceneIndex]->GetModifierPool()->GetModifier(nXRefReference);
	assert(pSourceModifierAgg);

	// create a new entry in the modifier pool
	int newModifierIndex = m_tupModifierPool.AddModifier();
	TupperwareAggregate *pNewModifierAgg = m_tupModifierPool.GetModifier(newModifierIndex);
	assert(pNewModifierAgg);

	// copy stuff from the source modifier to the new one
	for (TupperwareIterator iter = pSourceModifierAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		pNewModifierAgg->Copy(iter.GetAsAtom());
	}

	// add index into the remap manager
	AddRemap(sceneIndex,nXRefReference,newModifierIndex,RefRemap::REMAP_MODIFIER);

	// resolve any references used by the new modifier
	TupModifier tupModifier(pNewModifierAgg);

	int modifierType = tupModifier.GetModifierType();
	switch (modifierType)
	{
		// handle the skin modifier (only one that references other items for now)
		case TupModifier::SKIN_MODIFIER:
		{
			TupSkinModifier tupSkinModifier(pNewModifierAgg);
			int numBones = tupSkinModifier.GetNumBones();
			for (int boneIndex=0;boneIndex<numBones;boneIndex++)
			{
				TupperwareAggregate *pModifierBoneAgg = tupSkinModifier.GetBone(boneIndex);
				assert(pModifierBoneAgg);
				TupSkinModifierBone tupSkinModifierBone(pModifierBoneAgg);
				int boneNodeRef;
				TupperwareScalar *pBoneNodeRefScalar = tupSkinModifierBone.GetNodeReference(boneNodeRef);
				assert(pBoneNodeRefScalar);
				assert(boneNodeRef!=-1);
				// get modifier in our scene (may copy it in)
				int newBoneNodeRef = GetRemappedNode(sceneIndex,boneNodeRef);
				assert(newBoneNodeRef!=-1);
				// change scalar value to new reference
				pBoneNodeRefScalar->SetAsInt(newBoneNodeRef); 
			}
		}
		break;
	}
	return newModifierIndex;
}

int XRefManager::CopyMaterialFromScene(int sceneIndex,int nXRefReference)
{
	return -1;
}

int XRefManager::CopyMapFromScene(int sceneIndex,int nXRefReference)
{
	return -1;
}

int XRefManager::CopyImageFromScene(int sceneIndex,int nXRefReference)
{
	return -1;
}

