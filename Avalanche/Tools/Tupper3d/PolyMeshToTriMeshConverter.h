////////////////////////////////////////////////////////////////////////////
//
// PolyMeshToTriMeshConverter
//
// Handle the conversion from a polymesh to a trimesh
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_POLY_MESH_TO_TRI_MESH_CONVERTER_H
#define TUPPER3D_POLY_MESH_TO_TRI_MESH_CONVERTER_H

class TupObjectPool;
class TupModifierPool;

template<class TYPE>
class TupVertex3;
typedef TupVertex3<float> TupVertex3f;

class TupPolyMesh;

class TupPolyMeshToTriMeshConverter
{
	class PolyMeshMap
	{
	public:
		char *m_pMapName;
		int m_mapChannel;
		int m_mapFlags;
		int *m_pMapIndices;
		int m_numIndices;
		TupVertex3f *m_pMapVerts;
		int m_numVerts;
	};

public:
	static bool Convert(int objectRef,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,TupArray<TupArray<int> > &derivedObjectsRefs);

private:
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;

	TupPolyMeshToTriMeshConverter(TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);
	bool ConvertData(int objectRef,const TupArray<int> &derivedObjectRefs);
	bool ConvertModifier(TupperwareAggregate *pDerivedModifierAgg,int numFaces,int numTriangles,int *pFaceVertexCounts,int *pFaceTriangleCounts,int *pTrianglePointIndices);
	bool BuildTriangleData(TupPolyMesh &tupPolyMesh);
};

#endif
