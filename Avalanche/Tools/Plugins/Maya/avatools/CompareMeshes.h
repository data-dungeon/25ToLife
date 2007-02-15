////////////////////////////////////////////////////////////////////////////
//
// CompareMeshes
//
// Class to handle comparing meshes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CompareMeshes.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * compare meshes does not care about normals
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/25/03    Time: 2:03p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * compare meshes start
 */

#ifndef COMPARE_MESH_MANAGER
#define COMPARE_MESH_MANAGER

//#define COMPARE_NORMALS

class CompareMeshesManager
{
	Keeper<int> m_uniqueObjectPtrs; // keep track of the unique objects
	
	class UniqueObjectData
	{
	public:
		MDagPath m_dagPath; // path to mesh
		unsigned long m_meshCRC;
		unsigned long m_vertexColorCRC;

		int m_parentMeshIndex; // index in m_uniqueObjectRefs to parent mesh or -1 if no parent
		int m_parentVertexColorIndex; // index in m_uniqueObjectRefs to parent vertex color mesh or -1 if no parent

		// mesh information
		TupArray<Vector> m_positionVertices;
		TupArray<int> m_positionIndices;

#ifdef COMPARE_NORMALS
		Keeper<Vector> m_normalVertices;
		TupArray<int> m_normalIndices;

		Keeper<Vector> m_faceNormalVertices;
		TupArray<int> m_faceNormalIndices;
#endif

		TupArray<TupArray<Vector> > m_UVVertices;
		TupArray<TupArray<int> > m_UVIndices;

		// vertex color information
		bool m_bHasVertexColors;
		Keeper<Vector> m_vertexColorVertices;
		TupArray<int> m_vertexColorIndices;
		Keeper<float> m_vertexAlphaVertices;
		TupArray<int> m_vertexAlphaIndices;

		UniqueObjectData(void);

		void InitMeshData(void);
		bool QuickCompare(MDagPath mDagPath);
		void FlushMesh(void);
		void FlushVertexColors(void);

	};
	
	// for each unique object we store a bunch of other refs
	TupArray<UniqueObjectData> m_uniqueObjectDataArray;

public:
	void AddSourceMesh(MDagPath mDagPath);
	bool CheckMesh(MDagPath mDagPath,bool bCheckVertexColors);

private:
	static bool CompareMeshes(UniqueObjectData &mesh1,UniqueObjectData &mesh2);
	static bool CompareVertexColors(UniqueObjectData &mesh1,UniqueObjectData &mesh2);

	inline static bool CompareArrays(const TupArray<TupArray<int> > &s1,const TupArray<TupArray<int> > &s2);
	inline static bool CompareArrays(const TupArray<TupArray<Vector> > &s1,const TupArray<TupArray<Vector> > &s2);
	inline static bool CompareArrays(const TupArray<int> &s1,const TupArray<int> &s2);
	inline static bool CompareArrays(const TupArray<Vector> &s1,const TupArray<Vector> &s2);
	inline static bool CompareArrays(const TupArray<float> &s1,const TupArray<float> &s2);

};

#endif
