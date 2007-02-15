////////////////////////////////////////////////////////////////////////////
//
// DuplicateFace
//
// Class to handle the deleting of faces from the filter chain data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DuplicateFaces.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:40a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/28/03    Time: 12:10p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * vertex alpha added to apply vc modifier
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/18/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * wrong reference bug
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/02    Time: 3:11p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Duplicate Faces & Multitex Start
 */

////////////////////////////////////////////////////////////////////////////

#include "MultiTexPch.h"
#include "DuplicateFaces.h"

DuplicateFaceManager::DuplicateFaceManager(TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool) : 
	m_tupObjectPool(tupObjectPool), m_tupModifierPool(tupModifierPool)
{
}

// function to duplicated faces for a mesh and associated parent derived object
// the faceNumCopiesArray indicates how many copies each face will need
// the newFacesArray will be filled in to contain the indexes of the new faces created
void DuplicateFaceManager::DuplicateFaces(int objectRef,const TupArray<int> &faceNumCopiesArray,TupArray< TupArray<int> > &newFacesArray)
{
	TupperwareAggregate *pTupObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
	assert(pTupObjectAgg);
	TupObject tupObject(pTupObjectAgg);
	assert(tupObject.GetObjectType() == TupObject::TRI_MESH); // this better be a trimesh
	TupTriMesh tupBaseMesh(pTupObjectAgg);

	// build a list of derived objects that we need to fix
	int numObjects = m_tupObjectPool.GetNumObjects();
	int objectIndex;
	TupArray<int> parentDerivedObjectsArray;
	for (objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		// don't count the mesh
		if (objectIndex==objectRef)
			continue;
		TupperwareAggregate *pTupObjectAgg = m_tupObjectPool.GetObjectAggregate(objectIndex);
		if (!pTupObjectAgg)
			continue;
		TupObject tupObject(pTupObjectAgg);
		// only try on derived objects
		if (tupObject.GetObjectType() == TupObject::DERIVED)
		{
			// go down to base
			int baseObjectRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectIndex);
			// if the base is me then add
			if (baseObjectRef == objectRef)
				parentDerivedObjectsArray.Add(objectIndex);
		}
	}
	int numParentObjects = parentDerivedObjectsArray.GetSize();
	int parentObjectIndex;
	for (parentObjectIndex=0;parentObjectIndex<numParentObjects;parentObjectIndex++)
	{
		int parentObjectRef = parentDerivedObjectsArray[parentObjectIndex];
		TupperwareAggregate *pTupObjectAgg = m_tupObjectPool.GetObjectAggregate(parentObjectRef);

		TupDerived tupDerived(pTupObjectAgg);
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
				DuplicateModifierFaces(faceNumCopiesArray,modifierReference,pModifierContextAgg,tupBaseMesh);
			}
		}
	}
	// do the base mesh
	DuplicateMeshFaces(faceNumCopiesArray,newFacesArray,tupBaseMesh);
}

void DuplicateFaceManager::DuplicateMeshFaces(const TupArray<int> &faceNumCopiesArray,TupArray< TupArray<int> > &newFacesArray,TupTriMesh &tupTriMesh)
{
	// get mesh data
	int tempValue;
	int *pPositionIndices, *pNormalIndices;
	int *pMaterialIndices,*pFaceFlags;
	int numFaces;
	int *pFaceNormalIndices;
	int *pFaceShadingGroups;

	tupTriMesh.GetFaceFlags( &pFaceFlags, numFaces );
	tupTriMesh.GetPositionIndices( &pPositionIndices, tempValue );
	tupTriMesh.GetNormalIndices( &pNormalIndices, tempValue );
	tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,tempValue);
	tupTriMesh.GetFaceNormalIndices(&pFaceNormalIndices,tempValue);
	tupTriMesh.GetFaceShadingGroups(&pFaceShadingGroups,tempValue);

	assert(numFaces==faceNumCopiesArray.GetSize()); // these better be the same size

	// count how many faces that we will have
	// as well as the number of position and normal verts
	int newNumFaces=numFaces;
	int faceIndex;

	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		int numCopies = faceNumCopiesArray[faceIndex];
		if (numCopies>0)
		{
			newNumFaces+=numCopies;
		}
	}
	// did we add any faces?
	if (newNumFaces>numFaces)
	{
		// arrays to hold new face info
		TupArray<int> newFaceFlags;
		TupArray<int> newFaceMaterialIndices;
		TupArray<int> newPositionIndices;
		TupArray<int> newNormalIndices;
		TupArray<int> newFaceNormalIndices;
		TupArray<int> newFaceShadingGroups;

		// set the sizes
		newFaceFlags.SetSize(newNumFaces);
		newFaceMaterialIndices.SetSize(newNumFaces);
		newPositionIndices.SetSize((newNumFaces<<1)+newNumFaces);
		newNormalIndices.SetSize((newNumFaces<<1)+newNumFaces);
		newFaceNormalIndices.SetSize(newNumFaces);
		newFaceShadingGroups.SetSize(newNumFaces);

		// copy the old data
		memcpy(newFaceFlags.GetData(),pFaceFlags,numFaces*sizeof(int));
		memcpy(newFaceMaterialIndices.GetData(),pMaterialIndices,numFaces*sizeof(int));
		memcpy(newPositionIndices.GetData(),pPositionIndices,numFaces*sizeof(int)*3);
		memcpy(newNormalIndices.GetData(),pNormalIndices,numFaces*sizeof(int)*3);
		memcpy(newFaceNormalIndices.GetData(),pFaceNormalIndices,numFaces*sizeof(int));
		memcpy(newFaceShadingGroups.GetData(),pFaceShadingGroups,numFaces*sizeof(int));

		// set up the array to hold the new face numbers
		newFacesArray.RemoveAll();
		newFacesArray.SetSize(numFaces);

		int newFaceCount=numFaces;
		// set up the new faces
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			int numCopies = faceNumCopiesArray[faceIndex];
			if (numCopies>0)
			{
				newFacesArray[faceIndex].SetSize(numCopies);
				for (int copyIndex=0;copyIndex<numCopies;copyIndex++)
				{
					// set the new face indices 
					newFacesArray[faceIndex][copyIndex] = newFaceCount;

					newFaceFlags[newFaceCount] = newFaceFlags[faceIndex];
					newFaceMaterialIndices[newFaceCount] = newFaceMaterialIndices[faceIndex];
					newFaceNormalIndices[newFaceCount] = newFaceNormalIndices[faceIndex];
					newFaceShadingGroups[newFaceCount] = newFaceShadingGroups[faceIndex];

					int baseFaceIndex = (faceIndex<<1)+faceIndex;
					int newBaseFaceIndex = (newFaceCount<<1)+newFaceCount;
					for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
					{
						newPositionIndices[newBaseFaceIndex+facePointIndex] = newPositionIndices[baseFaceIndex+facePointIndex];
						newNormalIndices[newBaseFaceIndex+facePointIndex] = newNormalIndices[baseFaceIndex+facePointIndex];
					}
					newFaceCount++;
				}
			}
		}
		assert(newFaceCount==newNumFaces);

		// replace the data with the new stuff
		tupTriMesh.AddFaceFlags( newFaceFlags.GetData(), newNumFaces);
		tupTriMesh.AddFaceShadingGroups( newFaceShadingGroups.GetData(), newNumFaces);
		tupTriMesh.AddFaceMaterialIndices( newFaceMaterialIndices.GetData(), newNumFaces );
		tupTriMesh.AddPositionIndices( newPositionIndices.GetData(), newNumFaces );
		tupTriMesh.AddNormalIndices( newNormalIndices.GetData(), newNumFaces );
		tupTriMesh.AddFaceNormalIndices( newFaceNormalIndices.GetData(), newNumFaces );

		// do the meshmaps
		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
		int meshMapIndex;
		for (meshMapIndex = 0; meshMapIndex < numMeshMaps; meshMapIndex++ )
		{
			TupTriMeshMap  tupTriMeshMap( tupTriMesh.GetMeshMap( meshMapIndex ) );

			int *pMapIndices;
			tupTriMeshMap.GetMapIndices( &pMapIndices, tempValue );

			TupArray<int> newMapIndices;

			newMapIndices.SetSize((newNumFaces<<1)+newNumFaces);
			memcpy(newMapIndices.GetData(),pMapIndices,numFaces*sizeof(int)*3);

			newFaceCount=numFaces;
			// set up the new map faces
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int numCopies = faceNumCopiesArray[faceIndex];
				if (numCopies>0)
				{
					newFacesArray[faceIndex].SetSize(numCopies);
					for (int copyIndex=0;copyIndex<numCopies;copyIndex++)
					{
						int baseFaceIndex = (faceIndex<<1)+faceIndex;
						int newBaseFaceIndex = (newFaceCount<<1)+newFaceCount;
						for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
						{
							newMapIndices[newBaseFaceIndex+facePointIndex] = newMapIndices[baseFaceIndex+facePointIndex];
						}
						newFaceCount++;
					}
				}
			}
			assert(newFaceCount==newNumFaces);
			tupTriMeshMap.AddMapIndices( newMapIndices.GetData(), newNumFaces );
		}
	}
}

void DuplicateFaceManager::DuplicateModifierFaces(const TupArray<int> &faceNumCopiesArray,int modifierRef,TupperwareAggregate *pTupModifierContext,TupTriMesh &tupBaseMesh)
{
	TupperwareAggregate *pTupModifierAgg = m_tupModifierPool.GetModifier(modifierRef);
	assert(pTupModifierAgg);
	TupModifier tupModifier(pTupModifierAgg);

	switch(tupModifier.GetModifierType())
	{
		case TupModifier::APPLY_VC_MODIFIER:
			DuplicateApplyVCModifierFaces(faceNumCopiesArray,pTupModifierContext,tupModifier,tupBaseMesh);
			break;
		case TupModifier::VERTEX_PAINT_MODIFIER:
			DuplicateVertexPaintModifierFaces(faceNumCopiesArray,pTupModifierContext,tupModifier,tupBaseMesh);
			break;
		case TupModifier::HOLD_VC_MODIFIER:
			DuplicateHoldVCModifierFaces(faceNumCopiesArray,pTupModifierContext,tupModifier,tupBaseMesh);
			break;
		case TupModifier::FACE_ATTRIBUTE_MODIFIER:
			DuplicateFaceAttributeModifierFaces(faceNumCopiesArray,pTupModifierContext,tupModifier,tupBaseMesh);
			break;
		case TupModifier::SKIN_MODIFIER:
			DuplicateSkinModifierFaces(faceNumCopiesArray,pTupModifierContext,tupModifier,tupBaseMesh);
			break;
		case TupModifier::CLOTH_MODIFIER:
			DuplicateClothModifierFaces(faceNumCopiesArray,pTupModifierContext,tupModifier,tupBaseMesh);
			break;
		default:
			assert(false);
	}

}

void DuplicateFaceManager::DuplicateApplyVCModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupApplyVCModifier tupApplyVCModifier(tupModifier);
	int *pColorIndices;
	int numColorIndices;
	tupApplyVCModifier.GetColorIndices(&pColorIndices,numColorIndices);
	int flags;
	tupApplyVCModifier.GetFlags(flags);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	int *pAlphaIndices = NULL;
	int numAlphaIndices = 0;
	float *pModifierAlphaVerts = NULL;
	int numModifierAlphaVerts = 0;
	if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
	{
		tupApplyVCModifier.GetAlphaIndices(&pAlphaIndices,numAlphaIndices);
		tupApplyVCModifier.GetAlphaVerts((float **)&pModifierAlphaVerts,numModifierAlphaVerts);
	}
	

	// if this flag is set the indices are in sync with the position verts
	if (flags & TupApplyVCModifier::FLAGS_MIXED_COLORS)
	{
		// this one deals with the verts which do not need any modification
	}
	// indices are per point per face
	else
	{
		int newNumFaces=numFaces;
		int faceIndex;

		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			int numCopies = faceNumCopiesArray[faceIndex];
			if (numCopies>0)
			{
				newNumFaces+=numCopies;
			}
		}

		TupArray<int> newColorIndices;

		newColorIndices.SetSize((newNumFaces<<1)+newNumFaces);

		// copy the indices from the source that I need (handle numColorIndices value different than numfaces*3)
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;

			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				if (baseFaceIndex+facePointIndex<numColorIndices)
				{
					newColorIndices[baseFaceIndex+facePointIndex] = pColorIndices[baseFaceIndex+facePointIndex];
				}
				else
				{
					newColorIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
				}
			}
		}

		int newFaceCount=numFaces;
		// set up the new face data
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			int numCopies = faceNumCopiesArray[faceIndex];
			if (numCopies>0)
			{
				for (int copyIndex=0;copyIndex<numCopies;copyIndex++)
				{
					int baseFaceIndex = (faceIndex<<1)+faceIndex;
					int newBaseFaceIndex = (newFaceCount<<1)+newFaceCount;
					for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
					{
						newColorIndices[newBaseFaceIndex+facePointIndex] = newColorIndices[baseFaceIndex+facePointIndex];
					}
					newFaceCount++;
				}
			}
		}
		assert(newFaceCount==newNumFaces);
		tupApplyVCModifier.AddColorIndices(newColorIndices.GetData(),(newNumFaces<<1)+newNumFaces);

		// do the alpha if there
		if (pAlphaIndices)
		{
			TupArray<int> newAlphaIndices;
			
			newAlphaIndices.SetSize((newNumFaces<<1)+newNumFaces);
			
			// copy the indices from the source that I need (handle numAlphaIndices value different than numfaces*3)
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int baseFaceIndex = (faceIndex<<1)+faceIndex;
				
				for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
				{
					if (baseFaceIndex+facePointIndex<numAlphaIndices)
					{
						newAlphaIndices[baseFaceIndex+facePointIndex] = pAlphaIndices[baseFaceIndex+facePointIndex];
					}
					else
					{
						newAlphaIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
					}
				}
			}
			
			int newFaceCount=numFaces;
			// set up the new face data
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int numCopies = faceNumCopiesArray[faceIndex];
				if (numCopies>0)
				{
					for (int copyIndex=0;copyIndex<numCopies;copyIndex++)
					{
						int baseFaceIndex = (faceIndex<<1)+faceIndex;
						int newBaseFaceIndex = (newFaceCount<<1)+newFaceCount;
						for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
						{
							newAlphaIndices[newBaseFaceIndex+facePointIndex] = newAlphaIndices[baseFaceIndex+facePointIndex];
						}
						newFaceCount++;
					}
				}
			}
			assert(newFaceCount==newNumFaces);
			tupApplyVCModifier.AddAlphaIndices(newAlphaIndices.GetData(),(newNumFaces<<1)+newNumFaces);
		}
	}
}

void DuplicateFaceManager::DuplicateVertexPaintModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	// this does not need to do anything since adding new faces does not alter what this does
	// we already fixed any rogue data
}

void DuplicateFaceManager::DuplicateHoldVCModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupHoldVCModifier tupHoldVCModifier(tupModifier);
	int *pColorIndices;
	int numColorIndices;
	tupHoldVCModifier.GetColorIndices(&pColorIndices,numColorIndices);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	int newNumFaces=numFaces;
	int faceIndex;

	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		int numCopies = faceNumCopiesArray[faceIndex];
		if (numCopies>0)
		{
			newNumFaces+=numCopies;
		}
	}

	TupArray<int> newColorIndices;

	newColorIndices.SetSize((newNumFaces<<1)+newNumFaces);

	// copy the indices from the source that I need (handle numColorIndices value different than numfaces*3)
	for (faceIndex=0;faceIndex<numFaces;faceIndex)
	{
		int baseFaceIndex = (faceIndex<<1)+faceIndex;

		for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
		{
			if (baseFaceIndex+facePointIndex<numColorIndices)
			{
				newColorIndices[baseFaceIndex+facePointIndex] = pColorIndices[baseFaceIndex+facePointIndex];
			}
			else
			{
				newColorIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
			}
		}
	}

	int newFaceCount=numFaces;
	// set up the new face data
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		int numCopies = faceNumCopiesArray[faceIndex];
		if (numCopies>0)
		{
			for (int copyIndex=0;copyIndex<numCopies;copyIndex++)
			{
				int baseFaceIndex = (faceIndex<<1)+faceIndex;
				int newBaseFaceIndex = (newFaceCount<<1)+newFaceCount;
				for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
				{
					newColorIndices[newBaseFaceIndex+facePointIndex] = newColorIndices[baseFaceIndex+facePointIndex];
				}
				newFaceCount++;
			}
		}
	}
	assert(newFaceCount==newNumFaces);
	tupHoldVCModifier.AddColorIndices(newColorIndices.GetData(),(newNumFaces<<1)+newNumFaces);
}

void DuplicateFaceManager::DuplicateFaceAttributeModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pTupModifierContext);
	int *pAttributeIndices;
	int numAttributeIndices;
	tupFaceAttributesModifierContext.GetFaceAttributeIndices(&pAttributeIndices,numAttributeIndices);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	assert(numAttributeIndices==numFaces);

	int newNumFaces=numFaces;
	int faceIndex;

	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		int numCopies = faceNumCopiesArray[faceIndex];
		if (numCopies>0)
		{
			newNumFaces+=numCopies;
		}
	}

	TupArray<int> newAttributeIndices;

	newAttributeIndices.SetSize(newNumFaces);
	// copy the indices from the source
	memcpy(newAttributeIndices.GetData(),pAttributeIndices,numFaces*sizeof(int));

	int newFaceCount=numFaces;
	// set up the new face data
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		int numCopies = faceNumCopiesArray[faceIndex];
		if (numCopies>0)
		{
			for (int copyIndex=0;copyIndex<numCopies;copyIndex++)
			{
				newAttributeIndices[newFaceCount] = newAttributeIndices[faceIndex];
				newFaceCount++;
			}
		}
	}
	assert(newFaceCount==newNumFaces);
	tupFaceAttributesModifierContext.AddFaceAttributeIndices(newAttributeIndices.GetData(),newNumFaces);
}

void DuplicateFaceManager::DuplicateSkinModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	// this does not need to do anything since adding new faces does not alter what this does
	// we already fixed any rogue data
}

void DuplicateFaceManager::DuplicateClothModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	// this does not need to do anything since adding new faces does not alter what this does
	// we already fixed any rogue data
}

