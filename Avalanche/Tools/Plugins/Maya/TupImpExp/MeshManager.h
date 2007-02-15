////////////////////////////////////////////////////////////////////////////
//
// MeshManager
//
// Class to handle keeping track of the unique meshes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MeshManager.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix map channel on uv set chooser
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/30/03    Time: 12:45p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * polygon export support
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 2:17p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * took out normal comparisons
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/22/03    Time: 10:51a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * impovement on handling boned models
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 */

#ifndef MESH_MANAGER
#define MESH_MANAGER

class MeshManager
{
public:
	// method for having this class call through to the material manager
	class AddShadersInterface
	{
	public:
		virtual int AddShaders(const MDagPath &shaderPath,const MObjectArray &shaderArray,MIntArray &shaderIndices) = 0;
	};

private:
	Keeper<int> m_uniqueObjectPtrs; // keep track of the unique objects
	
	class UniqueObjectData
	{
	public:
		MDagPath m_dagPath; // path to mesh
		MDagPath m_PBDagPath; // path to the pre bound mesh
		unsigned long m_meshCRC;
		unsigned long m_vertexColorCRC;

		int m_parentMeshIndex; // index in m_uniqueObjectRefs to parent mesh or -1 if no parent
		int m_parentVertexColorIndex; // index in m_uniqueObjectRefs to parent vertex color mesh or -1 if no parent

		int m_meshObjectRef; // the reference in the object pool for the mesh
		int m_vertexColorObjectRef; // the reference in the object pool for the vertex color derived object or -1
		int m_meshMaterialRef; // the reference in the material pool for the mesh

		int m_shaderId;

		// mesh information
		TupArray<Vector3> m_positionVertices;
		TupArray<int> m_positionIndices;

		Keeper<Vector3> m_normalVertices;
		TupArray<int> m_normalIndices;

		Keeper<Vector3> m_faceNormalVertices;
		TupArray<int> m_faceNormalIndices;
		TupArray<int> m_faceFlags;
#ifndef POLYGON_EXPORT
		TupArray<int> m_faceShadingGroups;
#endif
		TupArray<int> m_faceMaterialIndices;

		TupArray<TupArray<Vector3> > m_UVVertices;
		TupArray<TupArray<int> > m_UVIndices;
		TupArray<TupString> m_UVNames;

		// vertex color information
		bool m_bHasVertexColors;
		Keeper<Vector3> m_vertexColorVertices;
		TupArray<int> m_vertexColorIndices;
		Keeper<float> m_vertexAlphaVertices;
		TupArray<int> m_vertexAlphaIndices;

#ifdef POLYGON_EXPORT
		TupArray<int> m_faceVertexCounts;
		TupArray<int> m_faceTriangleCounts;
		TupArray<int> m_trianglePointIndices;
#endif

		UniqueObjectData(void);

		void InitMeshData(AddShadersInterface *pAddShadersInterface);
		void FlushMesh(void);
		void FlushVertexColors(void);

	};
	
	// for each unique object we store a bunch of other refs
	TupArray<UniqueObjectData> m_uniqueObjectDataArray;

	AddShadersInterface *m_pAddShadersInterface;


public:
	int AddMesh(MDagPath mDagPath,MDagPath mPBDagPath);

	void SetAddShadersInterface(AddShadersInterface *pAddShadersInterface) { m_pAddShadersInterface = pAddShadersInterface; }

	int GetParentMeshIndex(int index) const { return m_uniqueObjectDataArray[index].m_parentMeshIndex; }
	int GetParentVertexColorIndex(int index) const { return m_uniqueObjectDataArray[index].m_parentVertexColorIndex; }

	int GetMeshObjectRef(int index) const { return m_uniqueObjectDataArray[index].m_meshObjectRef; }
	int GetVertexColorObjectRef(int index) const { return m_uniqueObjectDataArray[index].m_vertexColorObjectRef; }
	int GetMeshMaterialRef(int index) const { return m_uniqueObjectDataArray[index].m_meshMaterialRef; }

	void SetMeshObjectRef(int index,int ref) { m_uniqueObjectDataArray[index].m_meshObjectRef = ref; }
	void SetVertexColorObjectRef(int index,int ref) { m_uniqueObjectDataArray[index].m_vertexColorObjectRef = ref; }
	void SetMeshMaterialRef(int index,int ref) { m_uniqueObjectDataArray[index].m_meshMaterialRef = ref; }

	const TupArray<Vector3> &GetMeshPositionVertices(int index) const { return m_uniqueObjectDataArray[index].m_positionVertices; }
	const TupArray<int> &GetMeshPositionIndices(int index) const { return m_uniqueObjectDataArray[index].m_positionIndices; }
	const TupArray<Vector3> &GetMeshNormalVertices(int index) const { return m_uniqueObjectDataArray[index].m_normalVertices.GetArray(); }
	const TupArray<int> &GetMeshNormalIndices(int index) const { return m_uniqueObjectDataArray[index].m_normalIndices; }
	const TupArray<Vector3> &GetMeshFaceNormalVertices(int index) const { return m_uniqueObjectDataArray[index].m_faceNormalVertices.GetArray(); }
	const TupArray<int> &GetMeshFaceNormalIndices(int index) const { return m_uniqueObjectDataArray[index].m_faceNormalIndices; }
	const TupArray<int> &GetMeshFaceFlags(int index) const { return m_uniqueObjectDataArray[index].m_faceFlags; }
#ifndef POLYGON_EXPORT
	const TupArray<int> &GetMeshFaceShadingGroups(int index) const { return m_uniqueObjectDataArray[index].m_faceShadingGroups; }
#endif
	const TupArray<int> &GetMeshFaceMaterialIndices(int index) const { return m_uniqueObjectDataArray[index].m_faceMaterialIndices; }
	int GetNumUVSets(int index) const { return m_uniqueObjectDataArray[index].m_UVVertices.GetSize(); }
	const char *GetMeshUVName(int index,int uvSetIndex) const { return m_uniqueObjectDataArray[index].m_UVNames[uvSetIndex]; }
	const TupArray<Vector3> &GetMeshUVVertices(int index,int uvSetIndex) const { return m_uniqueObjectDataArray[index].m_UVVertices[uvSetIndex]; }
	const TupArray<int> &GetMeshUVIndices(int index,int uvSetIndex) const { return m_uniqueObjectDataArray[index].m_UVIndices[uvSetIndex]; }
	bool MeshHasVertexColors(int index) const { return m_uniqueObjectDataArray[index].m_bHasVertexColors; }
	const TupArray<Vector3> &GetMeshVertexColorVertices(int index) const { return m_uniqueObjectDataArray[index].m_vertexColorVertices.GetArray(); }
	const TupArray<int> &GetMeshVertexColorIndices(int index) const { return m_uniqueObjectDataArray[index].m_vertexColorIndices; }
	const TupArray<float> &GetMeshVertexAlphaVertices(int index) const { return m_uniqueObjectDataArray[index].m_vertexAlphaVertices.GetArray(); }
	const TupArray<int> &GetMeshVertexAlphaIndices(int index) const { return m_uniqueObjectDataArray[index].m_vertexAlphaIndices; }
#ifdef POLYGON_EXPORT
	const TupArray<int> &GetMeshFaceVertexCounts(int index) const { return m_uniqueObjectDataArray[index].m_faceVertexCounts; }
	const TupArray<int> &GetMeshFaceTriangleCounts(int index) const { return m_uniqueObjectDataArray[index].m_faceTriangleCounts; }
	const TupArray<int> &GetMeshTrianglePointIndices(int index) const { return m_uniqueObjectDataArray[index].m_trianglePointIndices; }
#endif

	int GetShaderID(int index) const { return m_uniqueObjectDataArray[index].m_shaderId; }

private:
	static bool CompareMeshes(UniqueObjectData &mesh1,UniqueObjectData &mesh2);
	static bool CompareVertexColors(UniqueObjectData &mesh1,UniqueObjectData &mesh2);

	inline static bool CompareArrays(const TupArray<TupArray<int> > &s1,const TupArray<TupArray<int> > &s2);
	inline static bool CompareArrays(const TupArray<TupArray<Vector3> > &s1,const TupArray<TupArray<Vector3> > &s2);
	inline static bool CompareArrays(const TupArray<int> &s1,const TupArray<int> &s2);
	inline static bool CompareArrays(const TupArray<Vector3> &s1,const TupArray<Vector3> &s2);
	inline static bool CompareArrays(const TupArray<float> &s1,const TupArray<float> &s2);
	inline static bool CompareArrays(const TupArray<TupString> &s1,const TupArray<TupString> &s2);

};

#endif
