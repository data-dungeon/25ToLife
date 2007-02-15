////////////////////////////////////////////////////////////////////////////
//
// MultiTexMesh
//
// Class to handle setting up the meshes for multi texturing
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MultiTexMesh.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:40a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 2:57p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * add next stage face list for use in ps2 multitexture code
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 10/01/02   Time: 9:47a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * build uv manager needs the image pool (for image rotation)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/18/02    Time: 4:34p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * rebuild tupmesh cache
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 10:01a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Multi-texturing first pass complete
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/04/02    Time: 7:57a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * calculate used materials change
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/03/02    Time: 2:06p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * start of hookup phase
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/28/02    Time: 4:38p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Material Remap Skeleton & MultiTexMesh done
 */

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "MultiTexPch.h"
#include "MultiTexMesh.h"
#include "MaterialRemap.h"
#include "DuplicateFaces.h"
#include "VertexAlpha.h"

MultiTexMeshManager::MultiTexMeshManager(const FilterOptions &options,FilterDataManager &dataManager) : m_options(options), m_dataManager(dataManager)
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
}

MultiTexMeshManager::~MultiTexMeshManager()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupRootNode;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;
}

void MultiTexMeshManager::Process(void)
{
	int numMaterials = m_pTupMaterialPool->GetNumMaterials();
	// if there are no materials then bail
	if (!numMaterials)
		return;

	TupArray<int> usedMaterials;
	CalculateUsedMaterials(usedMaterials);
	TupArray<MaterialRemapEntry> materialRemapEntryArray;
	materialRemapEntryArray.SetSize(numMaterials);

	MaterialRemapManager materialRemapManager(*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool);
	// set up the material remaps
	int materialIndex;
	for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		// do only used ones
		if (usedMaterials[materialIndex])
		{
			MaterialRemapEntry &materialRemapEntry = materialRemapEntryArray[materialIndex];
			materialRemapManager.SetupMaterialRemap(materialIndex,materialRemapEntry);
		}
	}

	// this object is in charge of duplicating the faces
	DuplicateFaceManager duplicateFaceManager(*m_pTupObjectPool,*m_pTupModifierPool);

	// alter the meshes to match the remapped materials
	int numObjects = m_pTupObjectPool->GetNumObjects();
	int objectIndex;
	for (objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		// deal only with trimeshes
		if (tupObject.GetObjectType() == TupObject::TRI_MESH)
		{
			TupTriMesh tupTriMesh(pObjectAgg);

			int tempValue;
			int *pMaterialIndices,*pFaceFlags;
			int numFaces;

			tupTriMesh.GetFaceFlags( &pFaceFlags, numFaces );
			tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,tempValue);

			TupArray<int> duplicateFaceArray;
			duplicateFaceArray.SetSize(numFaces);
			int faceIndex;
			// init duplicateFaceArray to 0 copies
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				duplicateFaceArray[faceIndex] = 0;
			}
			bool bNeedDuplicateFaces = false;

			TupperwareScalar *pMaterialRefScalar = pObjectAgg->FindScalarByKey("Annotate_MaterialRef");
			assert(pMaterialRefScalar);
			int materialRef = pMaterialRefScalar->GetAsInt();
			if (materialRef!=-1)
			{
				TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
				TupMaterial tupMaterial(pMaterialAgg);
				TupArray<int> newSubMaterialRefs; // keep track of the new materials needed for creating the new material
				TupArray<MaterialRefAlpha> applyAlphaEntries; // keep track of the active alpha entries
				bool bNeedNewMaterial = false; // if we need to create a new material
				if (tupMaterial.GetMaterialType() == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
				{
					TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
					int subMaterialIndex;
					int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
					assert(numSubMaterials); // there better be one (this may need to throw a fatal error)
					TupArray<int> subMaterialRefArray; // the current material reference for this sub material
					TupArray<int> subMaterialNumLayersArray; // how many layers are needed for this sub material
					TupArray<int> subMaterialNewIndexArray; // new index within new multi-sub object material
					TupArray<int> usedSubMaterialsArray;

					subMaterialRefArray.SetSize(numSubMaterials);
					subMaterialNumLayersArray.SetSize(numSubMaterials);
					subMaterialNewIndexArray.SetSize(numSubMaterials);
					usedSubMaterialsArray.SetSize(numSubMaterials);
					// initialize these arrays to default values
					for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
					{
						usedSubMaterialsArray[subMaterialIndex]=0; // start with not used
					}
					// mark which materials are actually used on this mesh...
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						int faceMaterialIndex = pMaterialIndices[faceIndex];
						assert(faceMaterialIndex<numSubMaterials); // this better fit
						usedSubMaterialsArray[faceMaterialIndex]++;
					}

					int newIndex = 0; // the index of each new material due to remap
					int numDuplicateFaces = 0; // keep track to see if I need to duplicate any faces
					// set up the data for the sub materials for later looping through the faces
					for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
					{
						// make sure the material is actually used
						if (usedSubMaterialsArray[subMaterialIndex])
						{
							TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
							TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
							int subMaterialRef;
							tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(subMaterialRef);
							// if this is a valid material
							if (subMaterialRef!=-1)
							{
								subMaterialRefArray[subMaterialIndex] = subMaterialRef;
								MaterialRemapEntry &materialRemapEntry = materialRemapEntryArray[subMaterialRef];
								int numLayers = materialRemapEntry.m_layerArray.GetSize();
								subMaterialNumLayersArray[subMaterialIndex] = numLayers;
								subMaterialNewIndexArray[subMaterialIndex] = newIndex;
								// keep track of the material refs for each new index
								for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
								{
									newSubMaterialRefs.Add(materialRemapEntry.m_layerArray[layerIndex].m_materialRef);
									if (materialRemapEntry.m_layerArray[layerIndex].m_alphaEntries.GetSize())
									{
										// add alpha entries to array
										MaterialRefAlpha materialRefAlpha;
										materialRefAlpha.m_faceMaterialIndex = newIndex+layerIndex;
										materialRefAlpha.m_alphaEntries =  materialRemapEntry.m_layerArray[layerIndex].m_alphaEntries;
										applyAlphaEntries.Add(materialRefAlpha);
									}
								}
								newIndex+=numLayers; // allocate space in the material indices for all the layers
							}
							else
							{
								// a material ref of -1 (no material)
								subMaterialRefArray[subMaterialIndex] = -1;
								subMaterialNumLayersArray[subMaterialIndex] = 1;
								subMaterialNewIndexArray[subMaterialIndex] = newIndex;
								newIndex++;
								int noMaterialRef = -1;
								newSubMaterialRefs.Add(noMaterialRef);
							}
						}
						else
						{
							// this material is not used on the mesh
							subMaterialRefArray[subMaterialIndex] = -1;
							subMaterialNumLayersArray[subMaterialIndex] = 0;
							subMaterialNewIndexArray[subMaterialIndex] = -1;
						}
					}
					int newNumSubMaterials = newSubMaterialRefs.GetSize();
					// go through the faces and set up the duplicate face values
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						// our material index better fit 
						int subMaterialIndex = pMaterialIndices[faceIndex];
						assert(subMaterialIndex<numSubMaterials);
						int numLayers = subMaterialNumLayersArray[subMaterialIndex];
						// better be at least one layer
						assert(numLayers>0);
						duplicateFaceArray[faceIndex] = numLayers-1; // set up number of copies (any beyond 1)
						numDuplicateFaces += numLayers-1;
					}
					// if I need to duplicate any faces
					if (numDuplicateFaces)
					{
						bNeedNewMaterial = true;
						// duplicate the faces of the mesh
						TupArray<TupArray<int> > newFacesArray;
						duplicateFaceManager.DuplicateFaces(objectIndex,duplicateFaceArray,newFacesArray);
						// the mesh needs to be resynced
						tupTriMesh.BuildAtomCache();

						int numOldFaces = numFaces;
						tupTriMesh.GetFaceFlags( &pFaceFlags, numFaces );
						tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,tempValue);

						// setup array to keep track of the next stage for each face
						TupArray<int> nextStageFaceIndices;
						nextStageFaceIndices.SetSize(numFaces);
						
						// setup the new face material indices as well as the stage portion of the flags
						for (faceIndex=0;faceIndex<numOldFaces;faceIndex++)
						{
							// our material index better fit 
							int subMaterialIndex = pMaterialIndices[faceIndex];
							assert(subMaterialIndex<numSubMaterials);
							int numLayers = subMaterialNumLayersArray[subMaterialIndex];
							TupArray<int> &newFaces = newFacesArray[faceIndex];
							assert(numLayers-1 == newFaces.GetSize());
							// do the first face
							pMaterialIndices[faceIndex] = subMaterialNewIndexArray[subMaterialIndex];
							assert(pMaterialIndices[faceIndex]<newNumSubMaterials); // this better fit
							// this face is at stage 0
							pFaceFlags[faceIndex] &= ~TupTriMesh::FLAGS_FACE_STAGE_MASK;
							// keep track of face to write the new stage into
							int prevFace = faceIndex; 
							// go through other faces that were duplicated from this one
							for (int layerIndex=1;layerIndex<numLayers;layerIndex++)
							{
								int dupFaceIndex = newFaces[layerIndex-1];
								assert(dupFaceIndex<numFaces);
								// make the previous face point to me
								nextStageFaceIndices[prevFace] = dupFaceIndex;
								// use sub materials located after the first face in layers
								pMaterialIndices[dupFaceIndex] = subMaterialNewIndexArray[subMaterialIndex]+layerIndex;
								assert(pMaterialIndices[dupFaceIndex]<newNumSubMaterials); // this better fit
								// set stage to be the layer index 
								pFaceFlags[dupFaceIndex] = (pFaceFlags[dupFaceIndex] & ~TupTriMesh::FLAGS_FACE_STAGE_MASK) | 
															(layerIndex<<TupTriMesh::FLAGS_FACE_STAGE_SHIFT);
								// we are now the previous face
								prevFace = dupFaceIndex;
							}
							// last face of layers gets a -1
							nextStageFaceIndices[prevFace] = -1;
						}
						// add this array to the tup data (for use in other filters)
						pObjectAgg->AddListInt("MultiTex_NextStageFaceIndices",NULL,nextStageFaceIndices);
					}
					// if we don't need to duplicate any faces
					// we still need to check what happened to the new material arrays
					else
					{
						if (newNumSubMaterials==numSubMaterials)
						{
							// check if this list is exactly the same (if not we need to change the face material indices)
							// and create a new material
							for (int subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
							{
								if (subMaterialNewIndexArray[subMaterialIndex]!=subMaterialIndex)
								{
									// if we found a mismatch
									bNeedNewMaterial = true;
									break;
								}
							}
						}
						// if the size is different we need to remap the faces and create a new material
						else
						{
							bNeedNewMaterial = true;
						}
						// if we still need the new material
						if (bNeedNewMaterial)
						{
							for (faceIndex=0;faceIndex<numFaces;faceIndex++)
							{
								// our material index better fit 
								int subMaterialIndex = pMaterialIndices[faceIndex];
								assert(subMaterialIndex<numSubMaterials);
								int newSubMaterialIndex = subMaterialNewIndexArray[subMaterialIndex];
								pMaterialIndices[faceIndex] = newSubMaterialIndex;
								assert(newSubMaterialIndex<newNumSubMaterials);
								// this face is at stage 0
//								pFaceFlags[faceIndex] &= ~TupTriMesh::FLAGS_FACE_STAGE_MASK;
							}
						}
					}
				}
				else
				{
					int numLayers;
					int numDuplicateFaces = 0; // keep track to see if I need to duplicate any faces
					// set up the data for the sub materials for later looping through the faces
					MaterialRemapEntry &materialRemapEntry = materialRemapEntryArray[materialRef];
					numLayers = materialRemapEntry.m_layerArray.GetSize();
					if (numLayers==1)
					{
						// if this does not match then we need a new material
						if (materialRemapEntry.m_layerArray[0].m_materialRef!=materialRef)
						{
							bNeedNewMaterial = true;
						}
					}
					else // if more than one layer then we need one too
					{
						bNeedNewMaterial = true;
					}
					// keep track of the material refs for each new index
					for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
					{
						newSubMaterialRefs.Add(materialRemapEntry.m_layerArray[layerIndex].m_materialRef);
						if (materialRemapEntry.m_layerArray[layerIndex].m_alphaEntries.GetSize())
						{
							// add alpha entries to array
							MaterialRefAlpha materialRefAlpha;
							materialRefAlpha.m_faceMaterialIndex = layerIndex;
							materialRefAlpha.m_alphaEntries =  materialRemapEntry.m_layerArray[layerIndex].m_alphaEntries;
							applyAlphaEntries.Add(materialRefAlpha);
						}
					}
					// go through the faces and set up the duplicate face values
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						duplicateFaceArray[faceIndex] = numLayers-1; // set up number of copies (any beyond 1)
						numDuplicateFaces += numLayers-1;
					}
					// if I need to duplicate any faces
					if (numDuplicateFaces)
					{
						// duplicate the faces of the mesh
						TupArray<TupArray<int> > newFacesArray;
						duplicateFaceManager.DuplicateFaces(objectIndex,duplicateFaceArray,newFacesArray);
						// the mesh needs to be resynced
						tupTriMesh.BuildAtomCache();

						int numOldFaces = numFaces;
						int newNumSubMaterials = newSubMaterialRefs.GetSize();
						tupTriMesh.GetFaceFlags( &pFaceFlags, numFaces );
						tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,tempValue);
						// setup the new face material indices as well as the stage portion of the flags
						for (faceIndex=0;faceIndex<numOldFaces;faceIndex++)
						{
							TupArray<int> &newFaces = newFacesArray[faceIndex];
							assert(numLayers-1 == newFaces.GetSize());
							pMaterialIndices[faceIndex] = 0;
							// this face is at stage 0
							pFaceFlags[faceIndex] &= ~TupTriMesh::FLAGS_FACE_STAGE_MASK;
							// go through other faces that were duplicated from this one
							for (int layerIndex=1;layerIndex<numLayers;layerIndex++)
							{
								int dupFaceIndex = newFaces[layerIndex-1];
								assert(dupFaceIndex<numFaces);
								// use sub materials located after the first face in layers
								pMaterialIndices[dupFaceIndex] = layerIndex;
								assert(pMaterialIndices[dupFaceIndex]<newNumSubMaterials); // this better fit
								// set stage to be the layer index 
								pFaceFlags[dupFaceIndex] = (pFaceFlags[dupFaceIndex] & ~TupTriMesh::FLAGS_FACE_STAGE_MASK) | 
															(layerIndex<<TupTriMesh::FLAGS_FACE_STAGE_SHIFT);
							}
						}
					}
				}
				// if a new material needs to be created
				if (bNeedNewMaterial)
				{
					int newMaterialRef = CreateMultiSubMaterial(materialRef,newSubMaterialRefs);
					// change material reference scalar to point to the new material
					pMaterialRefScalar->SetAsInt(newMaterialRef);
					// make all the nodes that point to this mesh to point to the new material as well
					RemapNodeMaterialRefs(objectIndex,materialRef,newMaterialRef);
					materialRef = newMaterialRef;
				}
				// if I need to apply some vertex alpha to the mesh
				if (applyAlphaEntries.GetSize())
				{
					// set up the manager (also caches off the vertex alpha values)
					VertexAlphaManager vertexAlphaManager(*m_pTupImagePool,m_dataManager.GetImageManager(),tupTriMesh);
					// do the vertex alpha
					vertexAlphaManager.ApplyVertexAlpha(applyAlphaEntries);
					// place on the mesh
					vertexAlphaManager.AddAlphaToMesh();
				}
			}
		}
	}
}

// calculates which materials (not multi-sub object materials) were actually assigned to a 
// mesh or multi - sub object material
void MultiTexMeshManager::CalculateUsedMaterials(TupArray<int> &usedMaterials)
{
	int numMaterials = m_pTupMaterialPool->GetNumMaterials();
	usedMaterials.SetSize(numMaterials);
	int materialIndex;
	for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		usedMaterials[materialIndex] = 0;
	}

	int numObjects = m_pTupObjectPool->GetNumObjects();
	int objectIndex;
	for (objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		// deal only with trimeshes
		if (tupObject.GetObjectType() == TupObject::TRI_MESH)
		{
			TupTriMesh tupTriMesh(pObjectAgg);

			int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
			if (materialRef!=-1)
			{
				int *pMaterialIndices;
				int numFaces,faceIndex;
				tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,numFaces);

				TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
				TupMaterial tupMaterial(pMaterialAgg);
				if (tupMaterial.GetMaterialType() == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
				{
					TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
					int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
					if (numSubMaterials)
					{
						TupArray<int> subMaterialRefs;
						subMaterialRefs.SetSize(numSubMaterials);
						TupArray<int> usedSubMaterials;
						usedSubMaterials.SetSize(numSubMaterials);
						int subMaterialIndex;
						for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
						{
							usedSubMaterials[subMaterialIndex] = 0;

							TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
							TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
							int subMaterialRef;
							tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(subMaterialRef);
							subMaterialRefs[subMaterialIndex] = subMaterialRef;
						}
						for (faceIndex=0;faceIndex<numFaces;faceIndex++)
						{
							usedSubMaterials[pMaterialIndices[faceIndex]]++; // mark how many faces used the sub material
						}
						for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
						{
							if (usedSubMaterials[subMaterialIndex] && subMaterialRefs[subMaterialIndex]!=-1)
							{
								usedMaterials[subMaterialRefs[subMaterialIndex]]+=usedSubMaterials[subMaterialIndex];
							}
						}
					}
				}
				else
				{
					usedMaterials[materialRef]+=numFaces; // mark how many faces used the material
				}
			}
		}
	}
}

int MultiTexMeshManager::CreateMultiSubMaterial(int oldMaterialRef,TupArray<int> &subMaterialRefs)
{
	TupperwareAggregate *pOldMaterialAgg = m_pTupMaterialPool->GetMaterial(oldMaterialRef);
	TupperwareScalar *pOldMaterialNameScalar = pOldMaterialAgg->FindScalarByKey("Name");
	assert(pOldMaterialNameScalar);
	char *pOldMaterialName = pOldMaterialNameScalar->GetAsString();

	int newMaterialRef = m_pTupMaterialPool->AddMaterial();
	TupperwareAggregate *pMaterialAggregate = m_pTupMaterialPool->GetMaterial(newMaterialRef);
	TupMaterial tupMaterial(pMaterialAggregate);
	tupMaterial.SetMaterialType(TupMaterial::MULTI_SUB_OBJECT_MATERIAL);
	TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAggregate);

	tupMultiSubObjectMaterial.AddName(pOldMaterialName);
	int numSubMaterials = subMaterialRefs.GetSize();
	assert(numSubMaterials);
	for (int subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
	{
		TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(subMaterialIndex);
		TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubMaterialAgg);
		tupMultiSubObjectMaterialSubMaterial.AddMaterialReference(subMaterialRefs[subMaterialIndex]);
		// always enabled
		tupMultiSubObjectMaterialSubMaterial.AddFlags(TupMultiSubObjectMaterialSubMaterial::ENABLED);
		// we ignore the slot name for now
	}
	return newMaterialRef;
}

// will remap all nodes that point to the object ref to the new material
void MultiTexMeshManager::RemapNodeMaterialRefs(int objectRef,int oldMaterialRef,int newMaterialRef)
{
	int numNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		int nodeObjectRef;
		tupNode.GetObjectReference(nodeObjectRef);
		if (nodeObjectRef!=-1)
		{
			int baseObjectRef = AnnotateHelper::FindBaseObjectRef(*m_pTupObjectPool,nodeObjectRef);
			// if the base matches the object reference
			if (baseObjectRef == objectRef)
			{
				int materialRef;
				tupNode.GetMaterialReference(materialRef);
				assert(materialRef==oldMaterialRef);
				tupNode.AddMaterialReference(newMaterialRef); // set to new material
			}
		}
	}
}
