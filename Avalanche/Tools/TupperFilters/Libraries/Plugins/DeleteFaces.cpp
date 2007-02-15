////////////////////////////////////////////////////////////////////////////
//
// DeleteFaces
//
// Class to handle the deleting of faces from the filter chain data
//
////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "DeleteFaces.h"
#include "AnnotateHelper.h"
#include "Bounding.h"

DeleteFaceManager::DeleteFaceManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool) : 
m_tupNodePool(tupNodePool), m_tupObjectPool(tupObjectPool), m_tupModifierPool(tupModifierPool)
{
}

// function to delete the faces underneath the desired node
// the faceTagArray is an array indicating which groups the faces will belong to
// the faceTagValue is the entry that will be group that will be kept (all others will be deleted)
void DeleteFaceManager::DeleteNodeFaces(TupArray<int> &faceTagArray,int faceTagValue,int nodeRef)
{
	TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeRef);
	assert(pNodeAgg);
	TupNode tupNode(pNodeAgg);
	int objectRef;
	tupNode.GetObjectReference(objectRef);
	assert(objectRef!=-1);
	TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(m_tupObjectPool,objectRef);
	assert(pBaseMeshAgg);
	TupTriMesh tupBaseMesh(pBaseMeshAgg);
	DeleteObjectFaces(faceTagArray,faceTagValue,objectRef,tupBaseMesh);
	Bounding::AddNodeBounding(tupNode,m_tupObjectPool);
}

// function to delete the faces from the desired object and the others within the pipeline
void DeleteFaceManager::DeleteObjectFaces(TupArray<int> &faceTagArray,int faceTagValue,int objectRef,TupTriMesh &tupBaseMesh)
{
	TupperwareAggregate *pTupObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
	assert(pTupObjectAgg);
	TupObject tupObject(pTupObjectAgg);

	switch(tupObject.GetObjectType())
	{
	case TupObject::DERIVED:
		{
			TupDerived tupDerived(tupObject);
			int numModifierSlots = tupDerived.GetNumModifierSlots();
			// call on all the modifiers
			for (int modifierIndex=0;modifierIndex<numModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
					DeleteModifierFaces(faceTagArray,faceTagValue,modifierReference,pModifierContextAgg,tupBaseMesh);
				}
			}
			int nextObjectRef;
			tupDerived.GetObjectReference(nextObjectRef);
			// call on the next object in the pipeline
			DeleteObjectFaces(faceTagArray,faceTagValue,nextObjectRef,tupBaseMesh);
		}
		break;
	case TupObject::TRI_MESH:
		{
			DeleteMeshFaces(faceTagArray,faceTagValue,tupBaseMesh);
		}
		break;
	default:
		assert(false);
	}
}

void DeleteFaceManager::DeleteMeshFaces(TupArray<int> &faceTagArray,int faceTagValue,TupTriMesh &tupMesh)
{
	// get mesh data
	int numPositionVerts, numNormalVerts, tempValue;
	Vector3 *pPositionVerts, *pNormalVerts;
	int *pPositionIndices, *pNormalIndices;
	int *pMaterialIndices,*pFaceFlags;
	int numFaces;
	Vector3 *pFaceNormalVerts;
	int *pFaceNormalIndices;
	int *pFaceShadingGroups;
	int numFaceNormalVerts;
	int *pNextStageFaces=NULL;

	tupMesh.GetFaceFlags( &pFaceFlags, numFaces );
	tupMesh.GetPositionVerts((float **) &pPositionVerts, numPositionVerts );
	tupMesh.GetNormalVerts((float **) &pNormalVerts, numNormalVerts );
	tupMesh.GetPositionIndices( &pPositionIndices, tempValue );
	tupMesh.GetNormalIndices( &pNormalIndices, tempValue );
	tupMesh.GetFaceMaterialIndices(&pMaterialIndices,tempValue);
	tupMesh.GetFaceNormalVerts((float **) &pFaceNormalVerts,numFaceNormalVerts);
	tupMesh.GetFaceNormalIndices(&pFaceNormalIndices,tempValue);
	tupMesh.GetFaceShadingGroups(&pFaceShadingGroups,tempValue);

	// need to break this up too
	TupperwareList *pNextStageFaceIndicesList = tupMesh.GetBaseAggregate()->FindListByKey("CombineMeshes_NextStageFaceIndices");
	if (pNextStageFaceIndicesList)
	{
		pNextStageFaces = pNextStageFaceIndicesList->GetAsInt();
	}

	assert(numFaces==faceTagArray.GetSize()); // these better be the same size
	TupArray<int> positionVertsRemapArray;
	TupArray<int> normalVertsRemapArray;
	TupArray<int> faceNormalVertsRemapArray;
	TupArray<int> faceRemapArray;
	positionVertsRemapArray.SetSize(numPositionVerts);
	normalVertsRemapArray.SetSize(numNormalVerts);
	faceNormalVertsRemapArray.SetSize(numFaceNormalVerts);
	faceRemapArray.SetSize(numFaces);

	int positionVertIndex;
	int normalVertIndex;
	int faceNormalVertIndex;
	int faceIndex;

	// reset remaps to null
	for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
		positionVertsRemapArray[positionVertIndex] = -1;
	for (normalVertIndex=0;normalVertIndex<numNormalVerts;normalVertIndex++)
		normalVertsRemapArray[normalVertIndex] = -1;
	for (faceNormalVertIndex=0;faceNormalVertIndex<numFaceNormalVerts;faceNormalVertIndex++)
		faceNormalVertsRemapArray[faceNormalVertIndex] = -1;
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		faceRemapArray[faceIndex] = -1;

	// count how many faces that we will have
	// as well as the number of position and normal verts
	int newFaceCount=0;
	int newPositionVertCount=0;
	int newNormalVertCount=0;
	int newFaceNormalVertCount=0;

	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;
			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				positionVertIndex = pPositionIndices[baseFaceIndex+facePointIndex];
				// if this vert has not been used yet
				if (positionVertsRemapArray[positionVertIndex]==-1)
				{
					positionVertsRemapArray[positionVertIndex]=newPositionVertCount;
					newPositionVertCount++;
				}
				normalVertIndex = pNormalIndices[baseFaceIndex+facePointIndex];
				// if this normal has not been used yet
				if (normalVertsRemapArray[normalVertIndex]==-1)
				{
					normalVertsRemapArray[normalVertIndex]=newNormalVertCount;
					newNormalVertCount++;
				}
			}
			faceNormalVertIndex = pFaceNormalIndices[faceIndex];
			// if this vert has not been used yet
			if (faceNormalVertsRemapArray[faceNormalVertIndex]==-1)
			{
				faceNormalVertsRemapArray[faceNormalVertIndex]=newFaceNormalVertCount;
				newFaceNormalVertCount++;
			}
			faceRemapArray[faceIndex] = newFaceCount;
			newFaceCount++;
		}
	}

	assert(newFaceCount); // we better have at least one face
	assert(newPositionVertCount);
	assert(newNormalVertCount);
	assert(newFaceNormalVertCount);

	TupArray<int> newFaceFlags;
	TupArray<int> newFaceMaterialIndices;
	TupArray<int> newPositionIndices;
	TupArray<int> newNormalIndices;
	TupArray<Vector3> newPositionVerts;
	TupArray<Vector3> newNormalVerts;
	TupArray<Vector3> newFaceNormalVerts;
	TupArray<int> newFaceNormalIndices;
	TupArray<int> newFaceShadingGroups;
	TupArray<int> newNextStageFaces;

	newFaceFlags.SetSize(newFaceCount);
	newFaceMaterialIndices.SetSize(newFaceCount);
	newPositionIndices.SetSize((newFaceCount<<1)+newFaceCount);
	newNormalIndices.SetSize((newFaceCount<<1)+newFaceCount);
	newPositionVerts.SetSize(newPositionVertCount);
	newNormalVerts.SetSize(newNormalVertCount);
	newFaceNormalVerts.SetSize(newFaceNormalVertCount);
	newFaceNormalIndices.SetSize(newFaceCount);
	newFaceShadingGroups.SetSize(newFaceCount);
	if (pNextStageFaces)
		newNextStageFaces.SetSize(newFaceCount);

	newFaceCount=0;
	// build the face flags, material indices, position indices, and normal indices
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			// get the face flags and zero out the stage value (set up later)
			newFaceFlags[newFaceCount] = (pFaceFlags[faceIndex] & ~TupTriMesh::FLAGS_FACE_STAGE_MASK);
			newFaceMaterialIndices[newFaceCount] = pMaterialIndices[faceIndex];
			newFaceShadingGroups[newFaceCount] = pFaceShadingGroups[faceIndex];
			// face normals
			int newFaceNormalVertIndex = faceNormalVertsRemapArray[pFaceNormalIndices[faceIndex]];
			assert(newFaceNormalVertIndex!=-1);
			newFaceNormalIndices[newFaceCount] = newFaceNormalVertIndex;
			// position and vertex normals
			int baseFaceIndex = (faceIndex<<1)+faceIndex;
			int newBaseFaceIndex = (newFaceCount<<1)+newFaceCount;
			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				int newPositionVertIndex = positionVertsRemapArray[pPositionIndices[baseFaceIndex+facePointIndex]];
				assert(newPositionVertIndex!=-1);
				newPositionIndices[newBaseFaceIndex+facePointIndex] = newPositionVertIndex;

				int newNormalVertIndex = normalVertsRemapArray[pNormalIndices[baseFaceIndex+facePointIndex]];
				assert(newNormalVertIndex!=-1);
				newNormalIndices[newBaseFaceIndex+facePointIndex] = newNormalVertIndex;
			}
			if (pNextStageFaces)
			{
				newNextStageFaces[newFaceCount]=-1; // init to no stages (set up later)
			}
			newFaceCount++;
		}
	}

	// if any staged faces to setup
	if (pNextStageFaces)
	{
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			// if original face has stages and is the first face in the list
			if (((pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_STAGE_MASK)==0) && (pNextStageFaces[faceIndex]!=-1))
			{
				int stageValue = 0;
				int stageFaceIndex = faceIndex;
				int prevFaceToSet = -1;
				// loop through all faces in stages and set flags as well as next faces
				while(stageFaceIndex!=-1)
				{
					// if in new mesh
					int newFaceIndex = faceRemapArray[stageFaceIndex];
					if (newFaceIndex!=-1)
					{
						// set correct stage
						newFaceFlags[newFaceIndex] |= (stageValue<<TupTriMesh::FLAGS_FACE_STAGE_SHIFT);
						stageValue++; // increment for next pass
						// set prev to point to me
						if (prevFaceToSet!=-1)
						{
							newNextStageFaces[prevFaceToSet] = newFaceIndex;
						}
						prevFaceToSet = newFaceIndex;
					}
					stageFaceIndex = pNextStageFaces[stageFaceIndex];
				}
			}
		}
	}

	// build the position verts
	for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
	{
		int newIndex = positionVertsRemapArray[positionVertIndex];
		// if it is used in the new data
		if (newIndex!=-1)
		{
			newPositionVerts[newIndex] = pPositionVerts[positionVertIndex];
		}
	}

	// build the normal verts
	for (normalVertIndex=0;normalVertIndex<numNormalVerts;normalVertIndex++)
	{
		int newIndex = normalVertsRemapArray[normalVertIndex];
		// if it is used in the new data
		if (newIndex!=-1)
		{
			newNormalVerts[newIndex] = pNormalVerts[normalVertIndex];
		}
	}

	// build the face normal verts
	for (faceNormalVertIndex=0;faceNormalVertIndex<numFaceNormalVerts;faceNormalVertIndex++)
	{
		int newIndex = faceNormalVertsRemapArray[faceNormalVertIndex];
		// if it is used in the new data
		if (newIndex!=-1)
		{
			newFaceNormalVerts[newIndex] = pFaceNormalVerts[faceNormalVertIndex];
		}
	}

	// replace the data with the new stuff
	tupMesh.AddFaceFlags( newFaceFlags.GetData(), newFaceCount);
	tupMesh.AddFaceShadingGroups( newFaceShadingGroups.GetData(), newFaceCount);
	tupMesh.AddFaceMaterialIndices( newFaceMaterialIndices.GetData(), newFaceCount );
	tupMesh.AddPositionVerts((float *) newPositionVerts.GetData(), newPositionVertCount);
	tupMesh.AddNormalVerts((float *) newNormalVerts.GetData(), newNormalVertCount );
	tupMesh.AddPositionIndices( newPositionIndices.GetData(), newFaceCount );
	tupMesh.AddNormalIndices( newNormalIndices.GetData(), newFaceCount );
	tupMesh.AddFaceNormalVerts((float *) newFaceNormalVerts.GetData(), newFaceNormalVertCount );
	tupMesh.AddFaceNormalIndices( newFaceNormalIndices.GetData(), newFaceCount );

	// replace with new next stage list
	if (pNextStageFaceIndicesList)
	{
		pNextStageFaceIndicesList->SetAsInt(newNextStageFaces);
	}


	// do the meshmaps
	int numMeshMaps = tupMesh.GetNumMeshMaps();
	int meshMapIndex;
	for (meshMapIndex = 0; meshMapIndex < numMeshMaps; meshMapIndex++ )
	{
		DeleteMeshMapFaces(faceTagArray,faceTagValue,tupMesh.GetMeshMap(meshMapIndex));
	}
	// compute new bounding information for mesh
	Bounding::AddObjectBounding(tupMesh);
}

void DeleteFaceManager::DeleteMeshMapFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pMeshMapAgg)
{
	TupTriMeshMap  meshMap(pMeshMapAgg);

	int numMapVerts;
	Vector3 *pMapVerts;
	int *pMapIndices;
	int numFaces;
	meshMap.GetMapVerts((float **) &pMapVerts, numMapVerts );
	meshMap.GetMapIndices( &pMapIndices, numFaces );
	assert(numFaces==faceTagArray.GetSize());

	TupArray<int> mapVertsRemapArray;
	mapVertsRemapArray.SetSize(numMapVerts);

	int mapVertIndex;

	// reset remaps to null
	for (mapVertIndex=0;mapVertIndex<numMapVerts;mapVertIndex++)
	mapVertsRemapArray[mapVertIndex] = -1;

	int newFaceCount=0;
	int faceIndex;
	int newMapVertCount=0;
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;
			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				mapVertIndex = pMapIndices[baseFaceIndex+facePointIndex];
				// if this vert has not been used yet
				if (mapVertsRemapArray[mapVertIndex]==-1)
				{
					mapVertsRemapArray[mapVertIndex]=newMapVertCount;
					newMapVertCount++;
				}
			}
			newFaceCount++;
		}
	}

	TupArray<int> newMapIndices;
	TupArray<Vector3> newMapVerts;

	newMapIndices.SetSize((newFaceCount<<1)+newFaceCount);
	newMapVerts.SetSize(newMapVertCount);

	newFaceCount=0;
	// build the map indices
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;
			int newBaseFaceIndex = (newFaceCount<<1)+newFaceCount;
			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				int newMapVertIndex = mapVertsRemapArray[pMapIndices[baseFaceIndex+facePointIndex]];
				assert(newMapVertIndex!=-1);
				newMapIndices[newBaseFaceIndex+facePointIndex] = newMapVertIndex;
			}
			newFaceCount++;
		}
	}
	// build the normal verts
	for (mapVertIndex=0;mapVertIndex<numMapVerts;mapVertIndex++)
	{
		int newIndex = mapVertsRemapArray[mapVertIndex];
		// if it is used in the new data
		if (newIndex!=-1)
		{
			newMapVerts[newIndex] = pMapVerts[mapVertIndex];
		}
	}
	meshMap.AddMapVerts((float *) newMapVerts.GetData(), newMapVertCount );
	meshMap.AddMapIndices( newMapIndices.GetData(), newFaceCount );
}


void DeleteFaceManager::DeleteModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,int modifierRef,TupperwareAggregate *pTupModifierContext,TupTriMesh &tupBaseMesh)
{
	TupperwareAggregate *pTupModifierAgg = m_tupModifierPool.GetModifier(modifierRef);
	assert(pTupModifierAgg);
	TupModifier tupModifier(pTupModifierAgg);

	switch(tupModifier.GetModifierType())
	{
	case TupModifier::APPLY_VC_MODIFIER:
		DeleteApplyVCModifierFaces(faceTagArray,faceTagValue,pTupModifierContext,tupModifier,tupBaseMesh);
		break;
	case TupModifier::VERTEX_PAINT_MODIFIER:
		DeleteVertexPaintModifierFaces(faceTagArray,faceTagValue,pTupModifierContext,tupModifier,tupBaseMesh);
		break;
	case TupModifier::HOLD_VC_MODIFIER:
		DeleteHoldVCModifierFaces(faceTagArray,faceTagValue,pTupModifierContext,tupModifier,tupBaseMesh);
		break;
	case TupModifier::FACE_ATTRIBUTE_MODIFIER:
		DeleteFaceAttributeModifierFaces(faceTagArray,faceTagValue,pTupModifierContext,tupModifier,tupBaseMesh);
		break;
	case TupModifier::SKIN_MODIFIER:
		DeleteSkinModifierFaces(faceTagArray,faceTagValue,pTupModifierContext,tupModifier,tupBaseMesh);
		break;
	case TupModifier::CLOTH_MODIFIER:
		DeleteClothModifierFaces(faceTagArray,faceTagValue,pTupModifierContext,tupModifier,tupBaseMesh);
		break;
	default:
		assert(false);
	}

}

void DeleteFaceManager::DeleteApplyVCModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupApplyVCModifier tupApplyVCModifier(tupModifier);
	Vector3 *pColorVerts;
	int *pColorIndices;
	int numColorVerts;
	int numColorIndices;
	tupApplyVCModifier.GetColorVerts((float **)&pColorVerts,numColorVerts);
	tupApplyVCModifier.GetColorIndices(&pColorIndices,numColorIndices);
	int flags;
	tupApplyVCModifier.GetFlags(flags);

	int *pAlphaIndices = NULL;
	int numAlphaIndices = 0;
	float *pAlphaVerts = NULL;
	int numAlphaVerts = 0;
	if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
	{
		tupApplyVCModifier.GetAlphaIndices(&pAlphaIndices,numAlphaIndices);
		tupApplyVCModifier.GetAlphaVerts((float **)&pAlphaVerts,numAlphaVerts);
	}

	int numPositionVerts;
	int numPositionFaces;
	Vector3 *pPositionVerts;
	int *pPositionIndices;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
	assert(numPositionFaces==faceTagArray.GetSize());

	// if this flag is set the indices are in sync with the position verts
	if (flags & TupApplyVCModifier::FLAGS_MIXED_COLORS)
	{
		TupArray<int> positionVertsRemapArray;
		positionVertsRemapArray.SetSize(numPositionVerts);
		int positionVertIndex;

		for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
			positionVertsRemapArray[positionVertIndex] = -1;

		// count how many faces that we will have
		// as well as the number of position verts
		int newFaceCount=0;
		int newPositionVertCount=0;
		int faceIndex;

		for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
		{
			if (faceTagArray[faceIndex]==faceTagValue)
			{
				int baseFaceIndex = (faceIndex<<1)+faceIndex;
				for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
				{
					positionVertIndex = pPositionIndices[baseFaceIndex+facePointIndex];
					// if this vert has not been used yet
					if (positionVertsRemapArray[positionVertIndex]==-1)
					{
						positionVertsRemapArray[positionVertIndex]=newPositionVertCount;
						newPositionVertCount++;
					}
				}
				newFaceCount++;
			}
		}
		// the indices are the same size as the new position vert count
		// this is used to map to the color pool

		TupArray<int> newColorIndices;
		newColorIndices.SetSize(newPositionVertCount);
		Keeper<Vector3> newColorVerts;
		Vector3 whiteColor;
		whiteColor.Set(1.0f,1.0f,1.0f);
		newColorVerts.Add(whiteColor);

		for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
		{
			int newIndex = positionVertsRemapArray[positionVertIndex];
			if (newIndex!=-1)
			{
				if (positionVertIndex<numColorIndices)
				{
					int newColorVertIndex = pColorIndices[positionVertIndex];
					newColorIndices[newIndex] = newColorVerts.Add(pColorVerts[newColorVertIndex]);
				}
				else
				{
					newColorIndices[newIndex] = 0;
				}
			}
		}
		// replace data
		tupApplyVCModifier.AddColorVerts((float *)newColorVerts.GetData(),newColorVerts.GetSize());
		tupApplyVCModifier.AddColorIndices(newColorIndices.GetData(),newPositionVertCount);

		// the indices are the same size as the new position vert count
		// this is used to map to the color pool

		if (pAlphaVerts)
		{
			TupArray<int> newAlphaIndices;
			newAlphaIndices.SetSize(newPositionVertCount);
			Keeper<float> newAlphaVerts;
			float opaque = 1.0f;
			newAlphaVerts.Add(opaque);

			for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
			{
				int newIndex = positionVertsRemapArray[positionVertIndex];
				if (newIndex!=-1)
				{
					if (positionVertIndex<numAlphaIndices)
					{
						int newAlphaVertIndex = pAlphaIndices[positionVertIndex];
						newAlphaIndices[newIndex] = newAlphaVerts.Add(pAlphaVerts[newAlphaVertIndex]);
					}
					else
					{
						newAlphaIndices[newIndex] = 0;
					}
				}
			}
			// replace data
			tupApplyVCModifier.AddAlphaVerts(newAlphaVerts.GetData(),newAlphaVerts.GetSize());
			tupApplyVCModifier.AddAlphaIndices(newAlphaIndices.GetData(),newPositionVertCount);
		}

	}
	// indices are per point per face
	else
	{
		// count how many faces that we will have
		// as well as the number of color verts
		int newFaceCount=0;
		int faceIndex;

		for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
		{
			if (faceTagArray[faceIndex]==faceTagValue)
			{
				newFaceCount++;
			}
		}

		TupArray<int> newColorIndices;
		Keeper<Vector3> newColorVerts;
		Vector3 whiteColor;
		whiteColor.Set(1.0f,1.0f,1.0f);
		newColorVerts.Add(whiteColor);

		newColorIndices.SetSize((newFaceCount<<1)+newFaceCount);
		newFaceCount=0;
		for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
		{
			if (faceTagArray[faceIndex]==faceTagValue)
			{
				int baseFaceIndex = (faceIndex<<1)+faceIndex;
				int newFaceIndex = (newFaceCount<<1)+newFaceCount;
				for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
				{
					if (baseFaceIndex+facePointIndex<numColorIndices) // valid lookup into modifier data
						newColorIndices[newFaceIndex+facePointIndex] = newColorVerts.Add(pColorVerts[pColorIndices[baseFaceIndex+facePointIndex]]);
					else
						newColorIndices[newFaceIndex+facePointIndex] = 0;
				}
				newFaceCount++;
			}
		}
		// replace data
		tupApplyVCModifier.AddColorVerts((float *)newColorVerts.GetData(),newColorVerts.GetSize());
		tupApplyVCModifier.AddColorIndices(newColorIndices.GetData(),(newFaceCount<<1)+newFaceCount);

		if (pAlphaVerts)
		{
			TupArray<int> newAlphaIndices;
			newAlphaIndices.SetSize((newFaceCount<<1)+newFaceCount);
			Keeper<float> newAlphaVerts;
			float opaque = 1.0f;
			newAlphaVerts.Add(opaque);

			newAlphaIndices.SetSize((newFaceCount<<1)+newFaceCount);
			newFaceCount=0;
			for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
			{
				if (faceTagArray[faceIndex]==faceTagValue)
				{
					int baseFaceIndex = (faceIndex<<1)+faceIndex;
					int newFaceIndex = (newFaceCount<<1)+newFaceCount;
					for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
					{
						if (baseFaceIndex+facePointIndex<numColorIndices) // valid lookup into modifier data
							newAlphaIndices[newFaceIndex+facePointIndex] = newAlphaVerts.Add(pAlphaVerts[pAlphaIndices[baseFaceIndex+facePointIndex]]);
						else
							newAlphaIndices[newFaceIndex+facePointIndex] = 0;
					}
					newFaceCount++;
				}
			}
			// replace data
			tupApplyVCModifier.AddAlphaVerts(newAlphaVerts.GetData(),newAlphaVerts.GetSize());
			tupApplyVCModifier.AddAlphaIndices(newAlphaIndices.GetData(),(newFaceCount<<1)+newFaceCount);
		}
	}
}

void DeleteFaceManager::DeleteVertexPaintModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupVertexPaintModifierContext tupVertexPaintModifierContext(pTupModifierContext);
	Vector3 *pModifierColorData;
	float *pModifierMixFactorData;
	int *pModifierFaceIndexData;
	int *pModifierPointIndexData;
	int numModifierColors;
	for (int channel = 0;channel<3;channel++)
	{
		switch (channel)
		{
		case 0:
			tupVertexPaintModifierContext.GetVertexColorChannelColorData((float **)&pModifierColorData,numModifierColors);
			tupVertexPaintModifierContext.GetVertexColorChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
			tupVertexPaintModifierContext.GetVertexColorChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
			tupVertexPaintModifierContext.GetVertexColorChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
			break;
		case 1:
			tupVertexPaintModifierContext.GetAlphaChannelColorData((float **)&pModifierColorData,numModifierColors);
			tupVertexPaintModifierContext.GetAlphaChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
			tupVertexPaintModifierContext.GetAlphaChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
			tupVertexPaintModifierContext.GetAlphaChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
			break;
		case 2:
			tupVertexPaintModifierContext.GetIlluminationChannelColorData((float **)&pModifierColorData,numModifierColors);
			tupVertexPaintModifierContext.GetIlluminationChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
			tupVertexPaintModifierContext.GetIlluminationChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
			tupVertexPaintModifierContext.GetIlluminationChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
			break;
		}
		if (numModifierColors)
		{
			int numPositionVerts;
			int numPositionFaces;
			Vector3 *pPositionVerts;
			int *pPositionIndices;
			tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
			tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
			assert(numPositionFaces==faceTagArray.GetSize());

			TupArray<int> positionVertsRemapArray;
			TupArray<int> validFaceForPositionArray;
			TupArray<int> validPointForPositionArray;
			positionVertsRemapArray.SetSize(numPositionVerts);
			validFaceForPositionArray.SetSize(numPositionVerts);
			validPointForPositionArray.SetSize(numPositionVerts);
			int positionVertIndex;

			for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
			{
				positionVertsRemapArray[positionVertIndex] = -1;
				validFaceForPositionArray[positionVertIndex] = -1;
				validPointForPositionArray[positionVertIndex] = -1;
			}

			// count how many faces that we will have
			// as well as the number of position verts
			int faceIndex;
			int newFaceCount=0;
			int newPositionVertCount=0;

			for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
			{
				if (faceTagArray[faceIndex]==faceTagValue)
				{
					int baseFaceIndex = (faceIndex<<1)+faceIndex;
					for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
					{
						positionVertIndex = pPositionIndices[baseFaceIndex+facePointIndex];
						// if this vert has not been used yet
						if (positionVertsRemapArray[positionVertIndex]==-1)
						{
							positionVertsRemapArray[positionVertIndex]=newPositionVertCount;
							// also save this new face index and point index for the position (use when referencing the position later)
							validFaceForPositionArray[positionVertIndex] = newFaceCount;
							validPointForPositionArray[positionVertIndex] = facePointIndex;
							newPositionVertCount++;
						}
					}
					newFaceCount++;
				}
			}
			TupArray<Vector3> newColorArray;
			TupArray<float> newMixFactorArray;
			TupArray<int> newFaceIndexArray;
			TupArray<int> newPointIndexArray;

			// fill the arrays with entries that are // go through the list of modifier colors and add to the table in sync with the position pool
			for (int colorIndex = 0;colorIndex<numModifierColors;colorIndex++)
			{
				Vector3 &color = pModifierColorData[colorIndex];
				float mixFactor = pModifierMixFactorData[colorIndex];
				int faceIndex = pModifierFaceIndexData[colorIndex];
				int pointIndex = pModifierPointIndexData[colorIndex];
				assert(pointIndex>=0 && pointIndex<=2);

				int facePointIndex = (faceIndex<<1)+faceIndex+pointIndex;
				if (faceIndex<numPositionFaces)
				{
					int posVertexIndex = pPositionIndices[facePointIndex];
					// is this pointing to a remapped position point?
					if (positionVertsRemapArray[posVertexIndex]!=-1)
					{
						// this is the face and point to use
						int newFaceIndex = validFaceForPositionArray[posVertexIndex];
						int newPointIndex = validPointForPositionArray[posVertexIndex];

						newColorArray.Add(color);
						newMixFactorArray.Add(mixFactor);
						newFaceIndexArray.Add(newFaceIndex);
						newPointIndexArray.Add(newPointIndex);
					}
				}
			}
			switch (channel)
			{
			case 0:
				tupVertexPaintModifierContext.AddVertexColorChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
				tupVertexPaintModifierContext.AddVertexColorChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
				tupVertexPaintModifierContext.AddVertexColorChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
				tupVertexPaintModifierContext.AddVertexColorChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
				break;
			case 1:
				tupVertexPaintModifierContext.AddAlphaChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
				tupVertexPaintModifierContext.AddAlphaChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
				tupVertexPaintModifierContext.AddAlphaChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
				tupVertexPaintModifierContext.AddAlphaChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
				break;
			case 2:
				tupVertexPaintModifierContext.AddIlluminationChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
				tupVertexPaintModifierContext.AddIlluminationChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
				tupVertexPaintModifierContext.AddIlluminationChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
				tupVertexPaintModifierContext.AddIlluminationChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
				break;
			}
		}
	}
}

void DeleteFaceManager::DeleteHoldVCModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupHoldVCModifier tupHoldVCModifier(tupModifier);
	Vector3 *pColorVerts;
	int *pColorIndices;
	int numColorVerts;
	int numColorIndices;
	tupHoldVCModifier.GetColorVerts((float **)&pColorVerts,numColorVerts);
	tupHoldVCModifier.GetColorIndices(&pColorIndices,numColorIndices);
	int flags;
	tupHoldVCModifier.GetFlags(flags);

	int numPositionVerts;
	int numPositionFaces;
	Vector3 *pPositionVerts;
	int *pPositionIndices;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
	assert(numPositionFaces==faceTagArray.GetSize());

	// count how many faces that we will have
	// as well as the number of color verts
	int newFaceCount=0;
	int faceIndex;

	for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			newFaceCount++;
		}
	}

	TupArray<int> newColorIndices;
	Keeper<Vector3> newColorVerts;
	Vector3 whiteColor;
	whiteColor.Set(1.0f,1.0f,1.0f);
	newColorVerts.Add(whiteColor);

	newColorIndices.SetSize((newFaceCount<<1)+newFaceCount);
	newFaceCount=0;
	for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;
			int newFaceIndex = (newFaceCount<<1)+newFaceCount;
			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				if (baseFaceIndex+facePointIndex<numColorIndices) // valid lookup into modifier data
					newColorIndices[newFaceIndex+facePointIndex] = newColorVerts.Add(pColorVerts[pColorIndices[baseFaceIndex+facePointIndex]]);
				else
					newColorIndices[newFaceIndex+facePointIndex] = 0;
			}
			newFaceCount++;
		}
	}
	// replace data
	tupHoldVCModifier.AddColorVerts((float *)newColorVerts.GetData(),newColorVerts.GetSize());
	tupHoldVCModifier.AddColorIndices(newColorIndices.GetData(),(newFaceCount<<1)+newFaceCount);
}

void DeleteFaceManager::DeleteFaceAttributeModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pTupModifierContext);
	TupArray<TupString> attributeVerts;
	int *pAttributeIndices;
	int numAttributeIndices;
	tupFaceAttributesModifierContext.GetFaceAttributeVerts(attributeVerts);
	tupFaceAttributesModifierContext.GetFaceAttributeIndices(&pAttributeIndices,numAttributeIndices);

	int numPositionVerts;
	int numPositionFaces;
	Vector3 *pPositionVerts;
	int *pPositionIndices;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
	assert(numPositionFaces==faceTagArray.GetSize());

	Keeper<TupString> newAttributeVerts;
	TupArray<int> newAttributeIndices;
	// count how many faces that we will have
	int newFaceCount=0;
	int faceIndex;

	for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			if (faceIndex<numAttributeIndices)
			{
				int newIndex = newAttributeVerts.Add(attributeVerts[pAttributeIndices[faceIndex]]);
				newAttributeIndices.Add(newIndex);
			}
			else
			{
				TupString emptyString("");
				int newIndex = newAttributeVerts.Add(emptyString);
				newAttributeIndices.Add(newIndex);
			}
			newFaceCount++;
		}
	}
	// replace data
	tupFaceAttributesModifierContext.AddFaceAttributeVerts(newAttributeVerts.GetArray());
	tupFaceAttributesModifierContext.AddFaceAttributeIndices(newAttributeIndices.GetData(),newAttributeIndices.GetSize());
}

void DeleteFaceManager::DeleteSkinModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupSkinModifierContext tupSkinModifierContext(pTupModifierContext);
	TupperwareAggregate *tupVertexPoolAgg = tupSkinModifierContext.GetVertexPool();
	TupperwareAggregate *pNewTupVertexPoolAgg = new TupperwareAggregate;
	TupSkinModifierContextVertexPool tupVertexPool(tupVertexPoolAgg);
	TupSkinModifierContextVertexPool tupNewVertexPool(pNewTupVertexPoolAgg);
	int numVertexPoolEntries = tupVertexPool.GetNumVertices();

	int numPositionVerts;
	int numPositionFaces;
	Vector3 *pPositionVerts;
	int *pPositionIndices;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
	assert(numPositionFaces==faceTagArray.GetSize());

	TupArray<int> positionVertsRemapArray;
	positionVertsRemapArray.SetSize(numPositionVerts);
	int positionVertIndex;

	for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
		positionVertsRemapArray[positionVertIndex] = -1;

	// count how many faces that we will have
	// as well as the number of position verts
	int newFaceCount=0;
	int newPositionVertCount=0;
	int faceIndex;

	for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;
			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				positionVertIndex = pPositionIndices[baseFaceIndex+facePointIndex];
				// if this vert has not been used yet
				if (positionVertsRemapArray[positionVertIndex]==-1)
				{
					positionVertsRemapArray[positionVertIndex]=newPositionVertCount;
					newPositionVertCount++;
				}
			}
			newFaceCount++;
		}
	}

	for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
	{
		int newIndex = positionVertsRemapArray[positionVertIndex];
		if (newIndex!=-1)
		{
			if (positionVertIndex<numVertexPoolEntries)
			{
				TupperwareAggregate *tupVertexPoolEntryAgg = tupVertexPool.GetVertex(positionVertIndex);
				TupperwareAggregate *tupNewVertexPoolEntryAgg = tupNewVertexPool.AddVertex(newIndex);
				// copy stuff to new vertex
				for (TupperwareIterator iter = tupVertexPoolEntryAgg->FindAll(); !iter.Finished(); iter.Next()) 
				{
					tupNewVertexPoolEntryAgg->Copy(iter.GetAsAtom());
				}
			}
		}
	}
	// replace data
	TupperwareAggregate *pAddedVertexPool = tupSkinModifierContext.AddVertexPool();
	for (TupperwareIterator iter = pNewTupVertexPoolAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		pAddedVertexPool->Copy(iter.GetAsAtom());
	}
	delete pNewTupVertexPoolAgg;
}

void DeleteFaceManager::DeleteClothModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupClothModifierContext tupClothModifierContext(pTupModifierContext);

	int numPositionVerts;
	int numPositionFaces;
	Vector3 *pPositionVerts;
	int *pPositionIndices;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
	assert(numPositionFaces==faceTagArray.GetSize());

	int *pClothPointFlags;
	int numClothPoints;
	tupClothModifierContext.GetPointFlags(&pClothPointFlags,numClothPoints);
	int *pConnections;
	int numConnections;
	tupClothModifierContext.GetConnectionPoints(&pConnections,numConnections);

	TupArray<int> positionVertsRemapArray;
	positionVertsRemapArray.SetSize(numPositionVerts);
	int positionVertIndex;

	for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
		positionVertsRemapArray[positionVertIndex] = -1;

	// count how many faces that we will have
	// as well as the number of position verts
	int newFaceCount=0;
	int newPositionVertCount=0;
	int faceIndex;

	for (faceIndex=0;faceIndex<numPositionFaces;faceIndex++)
	{
		if (faceTagArray[faceIndex]==faceTagValue)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;
			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				positionVertIndex = pPositionIndices[baseFaceIndex+facePointIndex];
				// if this vert has not been used yet
				if (positionVertsRemapArray[positionVertIndex]==-1)
				{
					positionVertsRemapArray[positionVertIndex]=newPositionVertCount;
					newPositionVertCount++;
				}
			}
			newFaceCount++;
		}
	}

	TupArray<int> newPointFlags;
	newPointFlags.SetSize(newPositionVertCount);

	for (positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
	{
		int newIndex = positionVertsRemapArray[positionVertIndex];
		if (newIndex!=-1)
		{
			if (positionVertIndex<numClothPoints)
			{
				newPointFlags[newIndex] = pClothPointFlags[positionVertIndex];
			}
		}
	}

	int connectionIndex;
	TupArray<int> newConnections;
	for (connectionIndex=0;connectionIndex<numConnections;connectionIndex++)
	{
		int index1 = pConnections[connectionIndex<<1];
		int index2 = pConnections[(connectionIndex<<1)+1];
		if (index1<numPositionVerts && index2<numPositionVerts && 
			positionVertsRemapArray[index1]!=-1 && positionVertsRemapArray[index2]!=-1)
		{
			newConnections.Add(index1);
			newConnections.Add(index2);
		}
	}
	tupClothModifierContext.AddPointFlags(newPointFlags.GetData(),newPositionVertCount);
	tupClothModifierContext.AddConnectionPoints(newConnections.GetData(),newConnections.GetSize()>>1);
}

// deletes faces from mesh as well as all modifiers linked to this mesh (also can deal with staged faces)
void DeleteFaceManager::DeleteMeshFaces(TupArray<int> &faceTagArray,int faceTagValue,int objectRef,TupTriMesh &tupBaseMesh)
{
	Keeper<int> parentDerivedObjects;
	AnnotateHelper::FindAllParentDerivedObjects(objectRef,m_tupObjectPool,parentDerivedObjects);
	int numParents = parentDerivedObjects.GetSize();
	for (int parentIndex=0;parentIndex<numParents;parentIndex++)
	{
		int objectRef = parentDerivedObjects[parentIndex];
		TupperwareAggregate *pTupObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
		assert(pTupObjectAgg);
		TupObject tupObject(pTupObjectAgg);
		assert(tupObject.GetObjectType()==TupObject::DERIVED);
		TupDerived tupDerived(tupObject);
		int numModifierSlots = tupDerived.GetNumModifierSlots();
		// call on all the modifiers
		for (int modifierIndex=0;modifierIndex<numModifierSlots;modifierIndex++)
		{
			TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
			if (pDerivedModifierAgg)
			{
				TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
				int modifierReference;
				tupDerivedModifier.GetModifierReference(modifierReference);
				TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
				DeleteModifierFaces(faceTagArray,faceTagValue,modifierReference,pModifierContextAgg,tupBaseMesh);
			}
		}
		// do any vertex colors and alphas that may have been propagated
		TupperwareAggregate *pVertexColorMeshMapAgg = pTupObjectAgg->FindAggregateByKeyAndName("Annotate_VertexColorMeshMap","0");
		if (pVertexColorMeshMapAgg)
		{
			DeleteMeshMapFaces(faceTagArray,faceTagValue,pVertexColorMeshMapAgg);
		}

		TupperwareAggregate *pVertexAlphaMeshMapAgg = pTupObjectAgg->FindAggregateByKeyAndName("Annotate_VertexAlphaMeshMap","-2");
		if (pVertexAlphaMeshMapAgg)
		{
			DeleteMeshMapFaces(faceTagArray,faceTagValue,pVertexAlphaMeshMapAgg);
		}
	}
	DeleteMeshFaces(faceTagArray,faceTagValue,tupBaseMesh);
}
