////////////////////////////////////////////////////////////////////////////
//
// MultiSubObjectMaterialConverter
//
// Convert from using multisubobjectmaterials to using material refs for each face
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MultiSubObjectMaterialConverter.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * multi sub object material converter
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "MultiSubObjectMaterialConverter.h"
#include "ObjectHelper.h"
#include "NodePool.h"
#include "ObjectPool.h"
#include "materialpool.h"
#include "node.h"
#include "object.h"
#include "trimesh.h"
#include "polyMesh.h"
#include "Material.h"
#include "multisubobjectmaterial.h"
#include "multisubobjectmaterialsubmaterial.h"

TupMultiSubObjectMaterialConverter::TupMultiSubObjectMaterialConverter(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool) :
	m_tupNodePool(tupNodePool),
	m_tupObjectPool(tupObjectPool),
	m_tupMaterialPool(tupMaterialPool)
{
}

bool TupMultiSubObjectMaterialConverter::ConvertScene(void)
{
	int numNodes = m_tupNodePool.GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);
		int materialRef;
		TupperwareScalar *pMaterialRefScalar = tupNode.GetMaterialReference(materialRef);
		if (pMaterialRefScalar)
		{
			TupArray<int> materialRefs;

			// get the material refs used for the mesh
			if (materialRef!=-1)
			{
				TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
				TupMaterial tupMaterial(pMaterialAgg);
				if (tupMaterial.GetMaterialType()==TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
				{
					TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
					int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
					materialRefs.SetSize(numSubMaterials);
					for (int subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
					{
						TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
						TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
						int subMaterialRef;
						tupSubMaterial.GetMaterialReference(subMaterialRef);
						materialRefs[subMaterialIndex] = subMaterialRef;
					}
				}
				else
				{
					materialRefs.Add(materialRef); // add main ref (only one material for this mesh
				}
			}
			else
			{
				materialRefs.Add(materialRef); // add -1
			}
			int numFaceMaterials = materialRefs.GetSize();

			// no more material ref on the node 
			pNodeAgg->DeleteAtom(pMaterialRefScalar);

			int objectRef;
			tupNode.GetObjectReference(objectRef);
			if (objectRef==-1)
				continue;
			int baseObjectRef = TupObjectHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);
			TupperwareAggregate *pBaseObjectAgg = m_tupObjectPool.GetObjectAggregate(baseObjectRef);
			TupObject tupObject(pBaseObjectAgg);
			switch (tupObject.GetObjectType())
			{
				case TupObject::TRI_MESH:
					{
						TupTriMesh tupTriMesh(pBaseObjectAgg);
						int *pFaceMaterialIndices;
						int numFaces;
						TupperwareList *pFaceMaterialIndicesList = tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);
						// if this is here we need to convert to refs
						if (pFaceMaterialIndicesList)
						{
							TupArray<int> faceMaterialRefs;
							faceMaterialRefs.SetSize(numFaces);
							for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
							{
								int faceMaterialIndex = pFaceMaterialIndices[faceIndex];
								if (faceMaterialIndex<0 || faceMaterialIndex>=numFaceMaterials)
								{
									faceMaterialIndex = 0;
								}
								faceMaterialRefs[faceIndex] = materialRefs[faceMaterialIndex];
							}
							tupTriMesh.AddFaceMaterialRefs(faceMaterialRefs.GetData(),numFaces);
							pBaseObjectAgg->DeleteAtom(pFaceMaterialIndicesList);
						}
					}
					break;
				case TupObject::POLY_MESH:
					{
						TupPolyMesh tupPolyMesh(pBaseObjectAgg);
						int *pFaceMaterialIndices;
						int numFaces;
						TupperwareList *pFaceMaterialIndicesList = tupPolyMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);
						// if this is here we need to convert to refs
						if (pFaceMaterialIndicesList)
						{
							TupArray<int> faceMaterialRefs;
							faceMaterialRefs.SetSize(numFaces);
							for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
							{
								int faceMaterialIndex = pFaceMaterialIndices[faceIndex];
								if (faceMaterialIndex<0 || faceMaterialIndex>=numFaceMaterials)
								{
									faceMaterialIndex = 0;
								}
								faceMaterialRefs[faceIndex] = materialRefs[faceMaterialIndex];
							}
							tupPolyMesh.AddFaceMaterialRefs(faceMaterialRefs.GetData(),numFaces);
							pBaseObjectAgg->DeleteAtom(pFaceMaterialIndicesList);
						}
					}
					break;
			}
		}
	}
	// delete all multisubobject materials
	int numMaterials = m_tupMaterialPool.GetNumMaterials();
	for (int materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialIndex);
		if (!pMaterialAgg)
			continue;
		TupMaterial tupMaterial(pMaterialAgg);
		if (tupMaterial.GetMaterialType()==TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
		{
			m_tupMaterialPool.DeleteMaterial(materialIndex);
		}
	}
	return true;
}
