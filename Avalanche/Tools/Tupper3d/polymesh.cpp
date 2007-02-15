////////////////////////////////////////////////////////////////////////////
//
// PolyMesh
//
// Class to handle polygon meshes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: polymesh.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * multi sub object material converter
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/28/03    Time: 3:10p
 * Updated in $/Avalanche/tools/Tupper3d
 * polymesh and start of object converters
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "polymesh.h"
#include "object.h"

char *TupPolyMesh::m_keyNames[TupPolyMesh::NUM_CHILDREN_ATOMS] = 
{
	"PositionVerts", // POSITION_VERTS
	"PositionIndices", // POSITION_INDICES
	"NormalVerts", // NORMAL_VERTS
	"NormalIndices", // NORMAL_INDICES
	"FaceVertexCounts", // FACE_VERTEX_COUNTS
	"FaceFlags", // FACE_FLAGS
	"FaceMaterialIndices", // FACE_MATERIAL_INDICES
	"FaceNormalVerts", // FACE_NORMAL_VERTS
	"FaceNormalIndices", // FACE_NORMAL_INDICES
	"MeshMaps", // MESH_MAPS
	"FaceTriangleCounts", // FACE_TRIANGLE_COUNTS
	"TrianglePointIndices", // TRIANGLE_POINT_INDICES
	"FaceMaterialRefs", // FACE_MATERIAL_REFS
};

TupPolyMesh::TupPolyMesh(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupPolyMesh::TupPolyMesh(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupPolyMesh::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupPolyMesh::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareList *TupPolyMesh::AddPositionVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupPolyMesh::POSITION_VERTS);
}

TupperwareList *TupPolyMesh::AddPositionIndices(const int *pIndices,int numIndices)
{
	return AddIntList(pIndices,numIndices,TupPolyMesh::POSITION_INDICES);
}

TupperwareList *TupPolyMesh::AddNormalVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupPolyMesh::NORMAL_VERTS);
}

TupperwareList *TupPolyMesh::AddNormalIndices(const int *pIndices,int numIndices)
{
	return AddIntList(pIndices,numIndices,TupPolyMesh::NORMAL_INDICES);
}

TupperwareList *TupPolyMesh::AddFaceVertexCounts(const int *pCounts,int numFaces)
{
	return AddIntList(pCounts,numFaces,TupPolyMesh::FACE_VERTEX_COUNTS);
}

TupperwareList *TupPolyMesh::AddFaceNormalVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupPolyMesh::FACE_NORMAL_VERTS);
}

TupperwareList *TupPolyMesh::AddFaceNormalIndices(const int *pIndices,int numFaces)
{
	return AddIntList(pIndices,numFaces,TupPolyMesh::FACE_NORMAL_INDICES);
}

TupperwareList *TupPolyMesh::AddFaceFlags(const int *pFaceFlags,int numFaces)
{
	return AddIntList(pFaceFlags,numFaces,TupPolyMesh::FACE_FLAGS);
}

TupperwareList *TupPolyMesh::AddFaceMaterialIndices(const int *pFaceMaterialIndices,int numFaces)
{
	return AddIntList(pFaceMaterialIndices,numFaces,TupPolyMesh::FACE_MATERIAL_INDICES);
}

TupperwareList *TupPolyMesh::AddFaceTriangleCounts(const int *pCounts,int numFaces)
{
	return AddIntList(pCounts,numFaces,TupPolyMesh::FACE_TRIANGLE_COUNTS);
}

TupperwareList *TupPolyMesh::AddTrianglePointIndices(const int *pIndices,int numIndices)
{
	return AddIntList(pIndices,numIndices,TupPolyMesh::TRIANGLE_POINT_INDICES);
}

TupperwareList *TupPolyMesh::AddFaceMaterialRefs(const int *pFaceMaterialRefs,int numFaces)
{
	return AddIntList(pFaceMaterialRefs,numFaces,TupPolyMesh::FACE_MATERIAL_REFS);
}

TupperwareAggregate *TupPolyMesh::AddMeshMap(int channelNum)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MESH_MAPS];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[MESH_MAPS] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[MESH_MAPS]);
	}
	TupperwareAggregate *pMeshMapsAgg = (TupperwareAggregate *)pAtom;

	char channelString[20];
	itoa(channelNum,channelString,10);
	TupperwareAtom *pMapAtom = pMeshMapsAgg->FindAtomByKeyAndName("MeshMap",channelString);
	if (pMapAtom)
	{
		pMeshMapsAgg->DeleteAtom(pMapAtom);
	}
	return pMeshMapsAgg->AddAggregate("MeshMap",channelString);
}


TupperwareList *TupPolyMesh::GetPositionVerts(float **pVerts,int &numVerts)
{
	TupperwareList *pList = GetFloatList(pVerts,numVerts,TupPolyMesh::POSITION_VERTS);
	// need to divide since there are 3 entrys per vertex
	numVerts/=3;
	return pList;
}

TupperwareList *TupPolyMesh::GetPositionIndices(int **pIndices,int &numIndices)
{
	return GetIntList(pIndices,numIndices,TupPolyMesh::POSITION_INDICES);
}

TupperwareList *TupPolyMesh::GetNormalVerts(float **pVerts,int &numVerts)
{
	TupperwareList *pList = GetFloatList(pVerts,numVerts,TupPolyMesh::NORMAL_VERTS);
	// need to divide since there are 3 entrys per vertex
	numVerts/=3;
	return pList;
}

TupperwareList *TupPolyMesh::GetNormalIndices(int **pIndices,int &numIndices)
{
	return GetIntList(pIndices,numIndices,TupPolyMesh::NORMAL_INDICES);
}

TupperwareList *TupPolyMesh::GetFaceVertexCounts(int **pCounts,int &numFaces)
{
	return GetIntList(pCounts,numFaces,TupPolyMesh::FACE_VERTEX_COUNTS);
}

TupperwareList *TupPolyMesh::GetFaceNormalVerts(float **pVerts,int &numVerts)
{
	TupperwareList *pList = GetFloatList(pVerts,numVerts,TupPolyMesh::FACE_NORMAL_VERTS);
	// need to divide since there are 3 entrys per vertex
	numVerts/=3;
	return pList;
}

TupperwareList *TupPolyMesh::GetFaceNormalIndices(int **pIndices,int &numFaces)
{
	return GetIntList(pIndices,numFaces,TupPolyMesh::FACE_NORMAL_INDICES);
}

TupperwareList *TupPolyMesh::GetFaceFlags(int **pFaceFlags,int &numFaces)
{
	return GetIntList(pFaceFlags,numFaces,TupPolyMesh::FACE_FLAGS);
}

TupperwareList *TupPolyMesh::GetFaceMaterialIndices(int **pFaceMaterialIndices,int &numFaces)
{
	return GetIntList(pFaceMaterialIndices,numFaces,TupPolyMesh::FACE_MATERIAL_INDICES);
}

TupperwareList *TupPolyMesh::GetFaceTriangleCounts(int **pCounts,int &numFaces)
{
	return GetIntList(pCounts,numFaces,TupPolyMesh::FACE_TRIANGLE_COUNTS);
}

TupperwareList *TupPolyMesh::GetTrianglePointIndices(int **pIndices,int &numIndices)
{
	return GetIntList(pIndices,numIndices,TupPolyMesh::TRIANGLE_POINT_INDICES);
}

TupperwareList *TupPolyMesh::GetFaceMaterialRefs(int **pFaceMaterialRefs,int &numFaces)
{
	return GetIntList(pFaceMaterialRefs,numFaces,TupPolyMesh::FACE_MATERIAL_REFS);
}

int TupPolyMesh::GetNumMeshMaps(void)
{
	int total = 0;
	TupperwareAtom *pMeshMapsAtom = m_cachedAtoms[MESH_MAPS];
	// if right type
	if (pMeshMapsAtom && pMeshMapsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pMeshMapsAgg = (TupperwareAggregate *)pMeshMapsAtom;
		// make sure it has children
		if (pMeshMapsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pMeshMapsAgg->FindByKey("MeshMap"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pMapAtom = i.GetAsAtom();
				if (pMapAtom && pMapAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					total++;
				}
			}
		}
	}
	return total;
}

TupperwareAggregate *TupPolyMesh::GetMeshMap(int mapIndex)
{
	int count = 0;
	TupperwareAtom *pMeshMapsAtom = m_cachedAtoms[MESH_MAPS];
	// if right type
	if (pMeshMapsAtom && pMeshMapsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pMeshMapsAgg = (TupperwareAggregate *)pMeshMapsAtom;
		// make sure it has children
		if (pMeshMapsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pMeshMapsAgg->FindByKey("MeshMap"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pMapAtom = i.GetAsAtom();
				if (pMapAtom && pMapAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					if (mapIndex == count)
						return (TupperwareAggregate *)pMapAtom;
					count++;
				}
			}
		}
	}
	return NULL;
}

// Private Methods
TupperwareList *TupPolyMesh::AddFloatList(const float *pFloats,int numFloats,TupPolyMesh::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupPolyMesh::AddIntList(const int *pInts,int numInts,TupPolyMesh::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareList *TupPolyMesh::GetFloatList(float **pFloats,int &numFloats,TupPolyMesh::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			*pFloats = pList->GetAsFloat();
			numFloats = pList->GetLength();
			return pList;
		}
	}
	*pFloats = NULL;
	numFloats = 0;
	return NULL;
}

TupperwareList *TupPolyMesh::GetIntList(int **pInts,int &numInts,TupPolyMesh::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_INT)
		{
			*pInts = pList->GetAsInt();
			numInts = pList->GetLength();
			return pList;
		}
	}
	*pInts = NULL;
	numInts = 0;
	return NULL;
}
