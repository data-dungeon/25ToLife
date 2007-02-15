//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _snapMeshesManager
#define _snapMeshesManager

class SnapMeshesManager
{
	class MeshData
	{
	public:
		MDagPath m_transformDagPath;
		MDagPath m_meshDagPath;
		int m_meshChildIndex;
		class MeshMap
		{
		public:
			TupString m_uvSetName;
			TupArray<TupVertex3d> m_verts;
			TupArray<int> m_indices;
		};
		TupArray<TupVertex3d> m_positionVerts;
		TupArray<int> m_positionIndices;
		TupArray<TupVertex3d> m_normalVerts;
		TupArray<int> m_normalIndices;
		TupArray<int> m_faceVertexCounts;
		TupArray<int> m_facePointOffsets;
		TupArray<MeshMap> m_meshMaps;
		Keeper<TupVertex3d> m_vertexColorVerts;
		TupArray<int> m_vertexColorIndices;
		Keeper<float> m_vertexAlphaVerts;
		TupArray<int> m_vertexAlphaIndices;
		TupString m_faceAttributeVerts;
		TupArray<int> m_faceAttributeIndices;
		TupMatrix4x4d m_localToWorldMatrix;
		TupString m_mayaObjectName;
		TupString m_nodeName;
		MIntArray m_shaderIndices;
		MObjectArray m_shaderVerts;
		TupArray<int> m_facesToSelect;
	};

	float m_positionTolerance;
	float m_positionToleranceSq;

	MeshData m_baseMeshData;
	TupArray<MeshData> m_subMeshDataArray;

public:
	SnapMeshesManager(void);

	void SetPositionTolerance(float positionTolerance);

	MStatus AddInstancesForSnap(const MSelectionList &selectionList); // build data based on the instances (last one is the base)
	MStatus AddInstancesForCopy(const MSelectionList &selectionList); // build data for the entire selection

	MStatus ReorderFaces(void);

	MStatus AddToScene(void); 
	MStatus SnapInstancesToBase(void);

private:
	MStatus GetMeshData(MeshData &meshData,const MDagPath &transformDagPath,const MDagPath &meshDatPath,int meshChildIndex);
	MStatus SnapMeshToBase(const MeshData &baseMeshData,MeshData &subMeshData);
	MStatus CreateMeshFromSub(MeshData &subMeshData);
	void ConvertMayaMatrix(const MMatrix &srcMatrix,TupMatrix4x4d &dstMatrix);
	void ConvertMayaMatrix(const TupMatrix4x4d &dstMatrix,MMatrix &srcMatrix);
	void AddNote(MObject object,const TupString &noteString);
	MStatus AddFacesToSelection(MeshData &smeshData,MSelectionList &selectionList);
	bool FindMesh(const MDagPath &selectedPath,MDagPath &transformPath,MDagPath &meshPath,int &childIndex);
	MStatus DeleteIntermediateObjects(MDagPath &transformPath);
	MStatus ReorderFaces(MeshData &meshData);
};

#endif