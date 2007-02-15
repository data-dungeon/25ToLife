////////////////////////////////////////////////////////////////////////////
//
// TestMeshes
//
// Class to handle the testing of meshes to determine which instances can be broken up
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "BreakLargeMeshesPch.h"
#include "BreakLargeMeshes.h"
#include "TestMeshes.h"

TestMeshesManager::TestMeshesManager(TupNodePool &tupNodePool,
												 TupObjectPool &tupObjectPool,
												 TupMaterialPool &tupMaterialPool,
												 TupMapPool &tupMapPool,
												 TupImagePool &tupImagePool,
												 TupModifierPool &tupModifierPool,
												 int tupRootNodeRef,
												 TupConfigRoot &tupConfigRoot,
												 BreakUpSettings &settings) :
	m_tupNodePool(tupNodePool), 
	m_tupObjectPool(tupObjectPool), 
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_tupImagePool(tupImagePool),
	m_tupModifierPool(tupModifierPool), 
	m_tupRootNodeRef(tupRootNodeRef), 
	m_tupConfigRoot(tupConfigRoot), 
	m_settings(settings)
{
}

bool TestMeshesManager::TestAndMarkNodes(int pass)
{
	bool bSomeToDo = false;

	int totalObjects = m_tupObjectPool.GetNumObjects();
	TupArray<int> nodeBaseMeshReferenceCountArray;
	nodeBaseMeshReferenceCountArray.SetSize(totalObjects);
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
		nodeBaseMeshReferenceCountArray[objectIndex] = 0;

	int totalNodes = m_tupNodePool.GetNumNodes();
	int nodeIndex;
	// this pass will count how many nodes reference each base mesh
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		if (nodeIndex==m_tupRootNodeRef) // don't do the root node
			continue;

		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		pNodeAgg->DeleteAtomsByKey("BreakLargeMeshes_BreakUpFlag");

		TupNode tupNode(pNodeAgg);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef==-1)
			continue;

		assert(objectRef<totalObjects);
		int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);
		TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(baseRef);
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			nodeBaseMeshReferenceCountArray[baseRef]++; // a node is referencing this mesh
		}
	}
	// this pass will set which nodes are eligible to be broken up
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef==-1)
			continue;

		assert(objectRef<totalObjects);
		int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);
		// 0-not a mesh
		// 1-is a mesh and I am the only one pointing to it
		// >1 - is a mesh and more than one node is pointing to it
		if (nodeBaseMeshReferenceCountArray[baseRef]==1)
		{
			// get how the attributes like to be broken up
			int result = TestAttributes(tupNode,pass == VISIBILITY_SET);
			if (result>0)
			{
				// get all the modifiers that this node is using and see if any are a problem
				TupArray<int> modifierReferenceArray;
				TupArray<TupperwareAggregate *> modifierContextReferenceArray;
				AnnotateHelper::FindModifiers(m_tupObjectPool,objectRef,modifierReferenceArray,modifierContextReferenceArray);
				if (TestModifiers(modifierReferenceArray,modifierContextReferenceArray))
				{
					bool bAddBreakUpFlag = false;
					if (pass == GRID)
					{
						// test size to see if we need to cut on the grid
						Vector3 boundingBoxMin = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
						Vector3 boundingBoxMax = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();
						Vector3 size = boundingBoxMax - boundingBoxMin;
						if (size.x() > m_settings.m_sizeThreshold && size.z() > m_settings.m_sizeThreshold)
						{
							bAddBreakUpFlag = true;
						}
					}
					else if (pass == TEXTURE_USAGE)
					{
						TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(baseRef);
						TupTriMesh tupTriMesh(pObjectAgg);

						Keeper<int> usedMaterials;
						Keeper<int> usedMaps;
						Keeper<int> usedImages;

						AnnotateHelper::CalculateUsedMatsMapsAndImages(tupTriMesh,
							m_tupMaterialPool,m_tupMapPool,m_tupImagePool,
							usedMaterials,usedMaps,usedImages);
						int totalImageSize = 0;
						for (int index=0;index<usedImages.GetSize();index++)
						{
							int imageRef = usedImages[index];
							TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageRef);
							totalImageSize += pImageAgg->FindScalarByKey("Annotate_ImageSize")->GetAsInt();
							totalImageSize += pImageAgg->FindScalarByKey("Annotate_ImageSizeMipMaps")->GetAsInt();
						}
						if (totalImageSize > (m_settings.m_cutOnTextureUsageLimit * 1024))
						{
							bAddBreakUpFlag = true;
						}
					}
					else
					{
						bAddBreakUpFlag = true; // other passes
					}
					if (bAddBreakUpFlag)
					{
						// store flag indicating if this node needs to be broken up
						pNodeAgg->AddScalarInt("BreakLargeMeshes_BreakUpFlag",NULL,result);
						bSomeToDo = true;
					}
				}
			}
		}
	}
	return bSomeToDo;
}

// returns how the attributes want to deal with the breakup using the visibility sets
// 0 - don't do
// 1 - do for grid or smallest vis region
// 2 - do for the largest vis region
int TestMeshesManager::TestAttributes(TupNode &tupNode,bool bVisibilitySet)
{
	// check and set flags based on instance attributes
	// get my attributes from the node
	char *pNodeAttributesString;
	tupNode.GetUserProperties(&pNodeAttributesString);
	TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
	TupKeyValueContext tupKeyValueContext;
	tupKeyValueContext.AddKey("instance_type");
	tupKeyValueContext.AddKey("env_type");
	tupKeyValueContext.AddKey("instance_no_far_clip");
	tupKeyValueContext.AddKey("visibility_set_placement");
	tupKeyValueContext.AddKey("no_break_mesh");
	const int INSTANCE_TYPE = 0;
	const int ENV_TYPE = 1;
	const int INSTANCE_NO_FAR_CLIP_KEY = 2;
	const int VISIBILITY_SET_PLACEMENT = 3;
	const int NO_BREAK_MESH = 4;
	
	// get all key/value pairs for the instance
	m_tupConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
	// if the env_type is not normal instance
	if (tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"normal"))
	{
		return 0;
	}
	// if the env_type is not none (meaning a environmental object)
	if (tupKeyValueContext.ValueValidAt(ENV_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(ENV_TYPE),"None"))
	{
		return 0;
	}
	// if the far clip is turned off
	if (tupKeyValueContext.ValueValidAt(INSTANCE_NO_FAR_CLIP_KEY) && tupKeyValueContext.GetValueAsBool(INSTANCE_NO_FAR_CLIP_KEY))
	{
		return 0;
	}
	// if the no break mesh is set
	if (tupKeyValueContext.ValueValidAt(NO_BREAK_MESH) && tupKeyValueContext.GetValueAsBool(NO_BREAK_MESH))
	{
		return 0;
	}

	// check how it is being placed in a visibility set
	if (bVisibilitySet && tupKeyValueContext.ValueValidAt(VISIBILITY_SET_PLACEMENT))
	{
		if (strcmp(tupKeyValueContext.GetValueAsString(VISIBILITY_SET_PLACEMENT),"inside_smallest")==0)
		{
			return 1;
		}
		else if (strcmp(tupKeyValueContext.GetValueAsString(VISIBILITY_SET_PLACEMENT),"inside_largest")==0)
		{
			return 2;
		}
		else if (strcmp(tupKeyValueContext.GetValueAsString(VISIBILITY_SET_PLACEMENT),"world")==0)
		{
			return 0;
		}
		else if (strcmp(tupKeyValueContext.GetValueAsString(VISIBILITY_SET_PLACEMENT),"specific_set")==0)
		{
			return 0;
		}
	}
	return 1;
}


bool TestMeshesManager::TestModifiers(TupArray<int> &modifierReferenceArray,TupArray<TupperwareAggregate *> &modifierContextReferenceArray)
{
	assert(modifierReferenceArray.GetSize() == modifierContextReferenceArray.GetSize());
	int numModifiers = modifierReferenceArray.GetSize();
	bool foundBadModifier = false;
	for (int modifierIndex=0;modifierIndex<numModifiers;modifierIndex++)
	{
		int modifierReference = modifierReferenceArray[modifierIndex];
		assert(modifierReference!=-1);
		TupperwareAggregate *pModifierAgg = m_tupModifierPool.GetModifier(modifierReference);
		assert(pModifierAgg);
		TupModifier tupModifier(pModifierAgg);
		TupModifier::ModifierTypes modifierType = tupModifier.GetModifierType();
		if (modifierType == TupModifier::CLOTH_MODIFIER ||
			modifierType == TupModifier::SKIN_MODIFIER)
		{
			foundBadModifier = true;
			break;
		}
	}
	return !foundBadModifier; // true if we did not find a bad modifier
}
