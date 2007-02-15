////////////////////////////////////////////////////////////////////////////
//
// MeshLayers.h
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MeshLayers.h $
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

#ifndef MESH_LAYERS_H
#define MESH_LAYERS_H

class MaterialMapsManager;

class MeshLayersManager
{
	MaterialMapsManager &m_materialMapsManager;

	class MeshMap
	{
	public:
		TupArray<int> m_indices;
		Keeper<Vector3> m_verts;
	};

public:
	class FaceLayer
	{
	public:
		Vector3 m_uvCoords[3];
		float m_opacity;
		float m_distance;
		int m_imageRef;
		int m_blendMode;

		bool operator==(const FaceLayer& other) const { return m_distance==other.m_distance; }
		bool operator<=(const FaceLayer& other) const { return m_distance<=other.m_distance; }
		bool operator>=(const FaceLayer& other) const { return m_distance>=other.m_distance; }
		bool operator<(const FaceLayer& other) const { return m_distance<other.m_distance; }
		bool operator>(const FaceLayer& other) const { return m_distance>other.m_distance; }

	};

	MeshLayersManager(TupTriMesh &tupMesh,MaterialMapsManager &materialMapsManager);
	~MeshLayersManager();

	void AddLayerToFace(int faceIndex,const FaceLayer &faceLayer);
	bool ApplyLayersToMesh(void);

private:
	int m_numFaces;
	bool m_bFaceLayersAllocated;
	TupArray<TupArray<FaceLayer> > m_faceLayers;
	TupTriMesh &m_tupTriMesh;
};

#endif
