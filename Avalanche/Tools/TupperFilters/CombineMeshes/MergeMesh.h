////////////////////////////////////////////////////////////////////////////
//
// MergeMesh
//
// Class to handle the merging of meshes
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef MERGE_MESH_H
#define MERGE_MESH_H

class MergeMeshesManager
{
	class MeshData
	{
	public:
		int m_nodeRef;
		class MeshMap
		{
		public:
			int m_mapChannel;
			Keeper<TupVertex3f> m_verts;
			TupArray<int> m_indices;
		};
		TupArray<TupVertex3f>	m_positionVerts;
		TupArray<int> m_positionIndices;
		Keeper<TupVertex3f> m_normalVerts;
		TupArray<int> m_normalIndices;
		Keeper<TupVertex3f> m_faceNormalVerts;
		TupArray<int> m_faceNormalIndices;
		TupArray<int> m_faceFlags;
		Keeper<int> m_faceMaterialRefVerts;
		TupArray<int> m_faceMaterialRefIndices;
		TupArray<int> m_faceShadingGroups;
		TupArray<MeshMap> m_meshMaps;
		Keeper<int> m_mapChannelToMeshMapIndexKeeper;
		Keeper<TupVertex3f> m_vertexColorVerts;
		TupArray<int> m_vertexColorIndices;
		Keeper<float> m_vertexAlphaVerts;
		TupArray<int> m_vertexAlphaIndices;
		Keeper<TupString> m_faceAttributeVerts;
		TupArray<int> m_faceAttributeIndices;
		TupMatrix4x4f m_localToWorldMatrix;
		TupString m_objectAttributes;
		TupArray<int> m_nextStageFaceArray;
		TupArray<int> m_prevStageFaceArray;
		TupString m_mayaObjectName;
		TupString m_nodeName;
	};

	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;
	TupMaterialPool &m_tupMaterialPool;
	TupConfigRoot &m_tupNodeConfigRoot;
	TupConfigRoot &m_tupFaceConfigRoot;

	float m_positionTolerance;
	float m_positionToleranceSq;

	MeshData m_baseMeshData;
	TupArray<MeshData> m_subMeshDataArray;

	class AddedCombinedMeshes
	{
	public:
		unsigned long m_CRC;
		TupString m_nodeName;
		TupString m_mayaObjectName;
		int m_objectRef;
		int m_materialRef;

		Keeper<unsigned long> m_vertexColorCRCs;
		TupArray<int> m_vertexColorRefs; 
	};

	Keeper<unsigned long> m_addedMeshCRCs;
	TupArray<AddedCombinedMeshes> m_addedCombinedMeshes;

public:
	MergeMeshesManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,
		TupModifierPool &tupModifierPool,TupMaterialPool &tupMaterialPool,
		TupConfigRoot &tupNodeConfigRoot,TupConfigRoot &tupFaceConfigRoot);

	void SetPositionTolerance(float positionTolerance);

	void AddInstances(const TupArray<int> &instanceRefs); // build data based on the instances (first one is the base)
	void AddToScene(int parentNodeRef); // add instance, mesh, modifiers, materials to scene
private:
	void GetMeshData(int nodeRef,MeshData &meshData);
	void GetMaterialRefs(TupNode &tupNode,TupArray<int> &materialRefs);
	void MergeMeshToBase(MeshData &baseMeshData,MeshData &subMeshData);
	int CreateMeshFromBase(void);
	int AddVertexColorModifier(int objectRef);
	int AddFaceAttributeModifier(int objectRef);
	int CreateMaterialFromRefs(void);
	void SetFaceFlagsForStages(void);
	void CopyCollisionAttributesToFaces(MeshData &meshData);
	void RemoveCollisionAttributeFromInstance(MeshData &meshData);
	unsigned long GetMeshCRC(const MeshData &meshData);
	unsigned long GetVertexColorCRC(const MeshData &meshData);
};

#endif
