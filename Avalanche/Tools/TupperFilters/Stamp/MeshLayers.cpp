////////////////////////////////////////////////////////////////////////////
//
// MeshLayers.cpp
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MeshLayers.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:49a
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:29p
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////


#include "StampPch.h"
#include "MeshLayers.h"
#include "MaterialMaps.h"

MeshLayersManager::MeshLayersManager(TupTriMesh &tupTriMesh,MaterialMapsManager &materialMapsManager) : 
	m_bFaceLayersAllocated(false),
	m_materialMapsManager(materialMapsManager),
	m_tupTriMesh(tupTriMesh)
{
	// get the number of faces
	int *pFaceFlags;
	m_tupTriMesh.GetFaceFlags(&pFaceFlags,m_numFaces);
}

MeshLayersManager::~MeshLayersManager()
{
}

void MeshLayersManager::AddLayerToFace(int faceIndex,const FaceLayer &faceLayer)
{
	if (!m_bFaceLayersAllocated)
	{
		m_faceLayers.SetSize(m_numFaces);
		m_bFaceLayersAllocated = true;
	}
	m_faceLayers[faceIndex].Add(faceLayer);
}

bool MeshLayersManager::ApplyLayersToMesh(void)
{
	if (!m_bFaceLayersAllocated)
	{
		return false;
	}

	int startChannel = m_materialMapsManager.GetStartChannel();

	int numMeshMaps = 0;
	int faceIndex;
	for (faceIndex=0;faceIndex<m_numFaces;faceIndex++)
	{
		int size = m_faceLayers[faceIndex].GetSize();
		if (size>numMeshMaps)
			numMeshMaps = size;
	}

	TupArray<MeshMap> meshMaps;
	meshMaps.SetSize(numMeshMaps);

	int meshMapIndex;
	for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		MeshMap &meshMap = meshMaps[meshMapIndex];
		Vector3 initVert;
		initVert.Set(0.0f,0.0f,0.0f);
		meshMap.m_verts.Add(initVert);
		int numIndices = m_numFaces+(m_numFaces<<1);
		meshMap.m_indices.SetSize(numIndices);
		memset(meshMap.m_indices.GetData(),0,numIndices*sizeof(int));
	}

	int *pFaceMaterialIndices;
	int temp;
	m_tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,temp);

	int facePointIndex=0;
	for (faceIndex=0;faceIndex<m_numFaces;faceIndex++,facePointIndex+=3)
	{
		TupArray<FaceLayer> &faceLayers = m_faceLayers[faceIndex];
		int numLayers = faceLayers.GetSize();
		// sort based on distance
		TupSort<FaceLayer>::SortAscending(faceLayers.GetData(),numLayers);

		TupArray<MaterialMapsManager::ImageLayer> imageLayers;
		imageLayers.SetSize(numLayers);

		for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
		{
			FaceLayer &faceLayer = faceLayers[layerIndex];
			MeshMap &meshMap = meshMaps[layerIndex];
			for (int point=0;point<3;point++)
			{
				meshMap.m_indices[facePointIndex+point] = meshMap.m_verts.Add(faceLayer.m_uvCoords[point]);
			}
			MaterialMapsManager::ImageLayer &imageLayer = imageLayers[layerIndex];
			imageLayer.m_channel = startChannel+layerIndex;
			imageLayer.m_imageRef = faceLayer.m_imageRef;
			imageLayer.m_opacity = faceLayer.m_opacity;
			imageLayer.m_blendMode = faceLayer.m_blendMode;
		}
		// get the remapped face index
		pFaceMaterialIndices[faceIndex] = m_materialMapsManager.GetMaterialIndex(pFaceMaterialIndices[faceIndex],imageLayers);
	}
	return true;
}

