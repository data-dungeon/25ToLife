////////////////////////////////////////////////////////////////////////////
//
// TriMeshToPolyMeshConverter
//
// Handle the conversion from a trimesh to a polymesh
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_TRI_MESH_TO_POLY_MESH_CONVERTER_H
#define TUPPER3D_TRI_MESH_TO_POLY_MESH_CONVERTER_H

class TupObjectPool;
class TupModifierPool;

template<class TYPE>
class TupVertex3;
typedef TupVertex3<float> TupVertex3f;

class TupTriMeshToPolyMeshConverter
{
	enum EdgeVisibility
	{
		EDGE_VISIBILITY_MASK		= 7,
		EDGE_VISIBILITY_NONE		= 0,
		EDGE_VISIBILITY_ALL		= 7,
		EDGE_VISIBILITY_0			= 1,
		EDGE_VISIBILITY_1			= 2,
		EDGE_VISIBILITY_0_1		= 3,
		EDGE_VISIBILITY_2			= 4,
		EDGE_VISIBILITY_0_2		= 5,
		EDGE_VISIBILITY_1_2		= 6,
	};

	class TriMeshMap
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
	
	TupTriMeshToPolyMeshConverter(TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);
	bool ConvertData(int objectRef,const TupArray<int> &derivedObjectRefs);
	bool ConvertModifier(TupperwareAggregate *pDerivedModifierAgg,int numFaces,const TupArray<int> polyTriFaceIndices,const TupArray<int> polyPointTriFaceIndices,const TupArray<int> polyPointTriFacePointIndices);
};

#endif
