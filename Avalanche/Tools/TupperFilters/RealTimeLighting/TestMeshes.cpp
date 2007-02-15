////////////////////////////////////////////////////////////////////////////
//
// TestMeshes
//
// Class to handle the testing of meshes to determine which instances can be broken up
//
////////////////////////////////////////////////////////////////////////////

#include "RealTimeLightingPch.h"
#include "RealTimeLighting.h"
#include "TestMeshes.h"

TestMeshesManager::TestMeshesManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,int tupRootNodeRef,TupConfigRoot &tupConfigRoot,LightManager &lightManager,const RealTimeLightingSettings &settings ) :
	m_tupNodePool(tupNodePool), 
	m_tupObjectPool(tupObjectPool), 
	m_tupModifierPool(tupModifierPool), 
	m_tupRootNodeRef(tupRootNodeRef), 
	m_tupConfigRoot(tupConfigRoot),
	m_lights(lightManager.GetLights()),
	m_settings(settings)
{
}

bool TestMeshesManager::TestAndMarkNodes(void)
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

		pNodeAgg->DeleteAtomsByKey("RealTimeLighting_BreakUpFlag");

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

		// --- see if the whole object has lighting
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey("lightingPoint");
		tupKeyValueContext.AddKey("lightingDirectional");
		tupKeyValueContext.AddKey("lightingAmbient");
		tupKeyValueContext.AddKey("lightingVertex");
		tupKeyValueContext.AddKey("lightingPointBreakupMesh");
		tupKeyValueContext.AddKey("lightingPointBreakupMeshTurnOffUnlit");
		tupKeyValueContext.AddKey("AttenuationScale");
		m_tupConfigRoot.GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );

		bool bPLit = tupKeyValueContext.GetValueAsBool(0);
		bool bDLit = tupKeyValueContext.GetValueAsBool(1);
		bool bALit = tupKeyValueContext.GetValueAsBool(2);
		bool bVLit = tupKeyValueContext.GetValueAsBool(3);
		bool bBreakupMesh = bPLit && tupKeyValueContext.GetValueAsBool(4);
		bool bBreakupMeshTurnOffUnlit = bPLit && tupKeyValueContext.GetValueAsBool(5);
		float lightingAttenuationScale = tupKeyValueContext.GetValueAsFloat(6);

		// mark instance with appropriate lighting
		pNodeAgg->AddScalarInt( "RealTimeLighting_Vertex", NULL, bVLit ? 1 : 0 );
		pNodeAgg->AddScalarInt( "RealTimeLighting_Ambient", NULL, bALit ? 1 : 0 );
		pNodeAgg->AddScalarInt( "RealTimeLighting_Point", NULL, bPLit ? 1 : 0 );
		pNodeAgg->AddScalarInt( "RealTimeLighting_Directional", NULL, bDLit ? 1: 0 );
		pNodeAgg->AddScalarFloat( "RealTimeLighting_AttenuationScale", NULL, lightingAttenuationScale );

		assert(objectRef<totalObjects);
		int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);

		bool bTryBreakup = false;
		// if this instance wants to be point lit and wants to be broken up
		if (bPLit && bBreakupMesh && m_settings.m_bBreakUpMesh)
		{
			// 0-not a mesh
			// 1-is a mesh and I am the only one pointing to it
			// >1 - is a mesh and more than one node is pointing to it
			if (nodeBaseMeshReferenceCountArray[baseRef]==1)
			{
				if (TestAttributes(tupNode)) // make sure the attributes are happy
				{
					// get all the modifiers that this node is using and see if any are a problem
					TupArray<int> modifierReferenceArray;
					TupArray<TupperwareAggregate *> modifierContextReferenceArray;
					AnnotateHelper::FindModifiers(m_tupObjectPool,objectRef,modifierReferenceArray,modifierContextReferenceArray);
					if (TestModifiers(modifierReferenceArray,modifierContextReferenceArray))
					{
						pNodeAgg->AddScalarInt( "RealTimeLighting_BreakUpFlag", NULL, 1 );
						pNodeAgg->AddScalarInt( "RealTimeLighting_BreakUpFlagTurnOffUnlit", NULL, bBreakupMeshTurnOffUnlit ? 1 : 0 );
						bTryBreakup = true;
						bSomeToDo = true;
					}
				}
			}
		}
		// if instance points to a mesh and wishes to be assigned some lights
		if (nodeBaseMeshReferenceCountArray[baseRef]>=1 && 
			 !bTryBreakup && 
			 m_settings.m_bAssignLightsToInstance)
		{
			// node is needing to be broken up
			TupVertex3f boundingBoxMin = *(TupVertex3f *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
			TupVertex3f boundingBoxMax = *(TupVertex3f *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();

			TupVertex3f boundingBoxCenter = (boundingBoxMin + boundingBoxMax) * 0.5f;

			TupArray<int> bestLights;
			TupArray<float> bestLightFactors;
			bestLights.SetSize(m_settings.m_maxAssignedPointLights);
			bestLightFactors.SetSize(m_settings.m_maxAssignedPointLights);
			int bestLightIndex;
			for (bestLightIndex=0;bestLightIndex<m_settings.m_maxAssignedPointLights;bestLightIndex++)
			{
				bestLights[bestLightIndex] = -1;
			}

			// find the best n lights for the face
			for (int lightIndex=0;lightIndex<m_lights.GetSize();lightIndex++)
			{
				const LightManager::Light &light = m_lights[lightIndex];

				// if we are terrian check if the affects terrain flag set on light
				if (nodeBaseMeshReferenceCountArray[baseRef]==1)
				{
					// if this light does not affect terrain then skip
					if (!light.m_bAffectsTerrain)
						continue;
				}
				// if an instance (more than one instance to a mesh)
				// make sure the lights affects this 
				else if (!light.m_bAffectsStaticInstances)
				{
					continue;
				}

				TupVertex3f lightVector = light.m_worldPosition-boundingBoxCenter;
				float bestLightFactor = lightVector.LengthSq();

				// keep the list of best lights sorted where the highest light is first and so on
				for (bestLightIndex=0;bestLightIndex<m_settings.m_maxAssignedPointLights;bestLightIndex++)
				{
					// if slot empty
					if (bestLights[bestLightIndex] == -1)
					{
						bestLights[bestLightIndex] = lightIndex;
						bestLightFactors[bestLightIndex] = bestLightFactor;
						break;
					}
					// if better than slot
					else if (bestLightFactors[bestLightIndex]<bestLightFactor)
					{
						// move other lights down
						for (int moveLightIndex=m_settings.m_maxAssignedPointLights-2;moveLightIndex>=bestLightIndex;moveLightIndex--)
						{
							bestLights[moveLightIndex+1] = bestLights[moveLightIndex];
							bestLightFactors[moveLightIndex+1] = bestLightFactors[moveLightIndex];
						}
						bestLights[bestLightIndex] = lightIndex;
						bestLightFactors[bestLightIndex] = bestLightFactor;
						break;
					}
				}
			}

			// build an array for the actual light indices for the light group
			TupArray<int> lightsUsedInGroup;
			for (lightIndex=0;lightIndex<m_settings.m_maxAssignedPointLights;lightIndex++)
			{
				if (bestLights[lightIndex]!=-1)
				{
					lightsUsedInGroup.Add(m_lights[bestLights[lightIndex]].m_lightIndex);
				}
				else
				{
					break;
				}
			}
			// tell everyone which lights are needed for this group
			pNodeAgg->AddListInt("RealTimeLighting_Lights",NULL,lightsUsedInGroup);
		}
	}
	return bSomeToDo;
}

bool TestMeshesManager::TestAttributes(TupNode &tupNode)
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
	tupKeyValueContext.AddKey("no_break_mesh");
	const int INSTANCE_TYPE = 0;
	const int ENV_TYPE = 1;
	const int INSTANCE_NO_FAR_CLIP_KEY = 2;
	const int NO_BREAK_MESH = 3;

	// get all key/value pairs for the instance
	m_tupConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
	// if the env_type is not normal instance
	if (tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"normal"))
	{
		return false;
	}
	// if the env_type is not none (meaning a environmental object)
	if (tupKeyValueContext.ValueValidAt(ENV_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(ENV_TYPE),"None"))
	{
		return false;
	}
	// if the far clip is turned off
	if (tupKeyValueContext.ValueValidAt(INSTANCE_NO_FAR_CLIP_KEY) && tupKeyValueContext.GetValueAsBool(INSTANCE_NO_FAR_CLIP_KEY))
	{
		return false;
	}
	// if the no break mesh is set
	if (tupKeyValueContext.ValueValidAt(NO_BREAK_MESH) && tupKeyValueContext.GetValueAsBool(NO_BREAK_MESH))
	{
		return false;
	}

	return true;
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





