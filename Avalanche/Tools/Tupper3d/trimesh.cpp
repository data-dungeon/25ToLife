////////////////////////////////////////////////////////////////////////////
//
// TriMesh
//
// Class to handle triangle meshes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: trimesh.cpp $
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * multi sub object material converter
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 10:56a
 * Updated in $/Avalanche/tools/Tupper3d
 * move mesh to trimesh
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 11:22a
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Triangle Strip classes
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:12p
 * Updated in $/Avalanche/tools/Tupper3d
 * Remove Bounding Box 
 * Moved to process step in ExportPC filter
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 3:00p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 2:14p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Bounding Box and Bounding Sphere
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 1/11/02    Time: 4:21p
 * Updated in $/Avalanche/tools/Tupper3d
 * Fixed division problems with the numfaces on functions
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * fixed get vertex functions to return the number of vertices rather than
 * the number of floats
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:30p
 * Updated in $/Avalanche/tools/Tupper3d
 * spelled indices correctly
 * 
 * *****************  Version 6  *****************
 * User: Shon         Date: 12/18/01   Time: 3:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 5  *****************
 * User: Shon         Date: 12/18/01   Time: 3:39p
 * Updated in $/Avalanche/tools/Tupper3d
 * The Get*Verts functions were dividing the number of verts by 3 - this
 * should not happen since the numVerts variable is expecting the vert
 * count.
 * Also, the GetFaceFlags was dividing the number of flag values by 3 - we
 * only have 1 flag value per face, so this division was not needed.
 * 
 * *****************  Version 4  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "trimesh.h"
#include "object.h"

char *TupTriMesh::m_keyNames[TupTriMesh::NUM_CHILDREN_ATOMS] = 
{
	"PositionVerts", // POSITION_VERTS
	"PositionIndices", // POSITION_INDICES
	"NormalVerts", // NORMAL_VERTS
	"NormalIndices", // NORMAL_INDICES
	"FaceFlags", // FACE_FLAGS
	"FaceMaterialIndices", // FACE_MATERIAL_INDICES
	"FaceShadingGroups", // FACE_SHADING_GROUPS
	"FaceNormalVerts", // FACE_NORMAL_VERTS
	"FaceNormalIndices", // FACE_NORMAL_INDICES
	"MeshMaps", // MESH_MAPS
	"TriangleStrips", // TRIANGLE_STRIPS
	"FaceMaterialRefs", // FACE_MATERIAL_REFS
};

TupTriMesh::TupTriMesh(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupTriMesh::TupTriMesh(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupTriMesh::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupTriMesh::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareList *TupTriMesh::AddPositionVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupTriMesh::POSITION_VERTS);
}

TupperwareList *TupTriMesh::AddPositionIndices(const int *pIndices,int numFaces)
{
	return AddIntList(pIndices,numFaces*3,TupTriMesh::POSITION_INDICES);
}

TupperwareList *TupTriMesh::AddNormalVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupTriMesh::NORMAL_VERTS);
}

TupperwareList *TupTriMesh::AddNormalIndices(const int *pIndices,int numFaces)
{
	return AddIntList(pIndices,numFaces*3,TupTriMesh::NORMAL_INDICES);
}

TupperwareList *TupTriMesh::AddFaceNormalVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupTriMesh::FACE_NORMAL_VERTS);
}

TupperwareList *TupTriMesh::AddFaceNormalIndices(const int *pIndices,int numFaces)
{
	return AddIntList(pIndices,numFaces,TupTriMesh::FACE_NORMAL_INDICES);
}

TupperwareList *TupTriMesh::AddFaceFlags(const int *pFaceFlags,int numFaces)
{
	return AddIntList(pFaceFlags,numFaces,TupTriMesh::FACE_FLAGS);
}

TupperwareList *TupTriMesh::AddFaceMaterialIndices(const int *pFaceMaterialIndices,int numFaces)
{
	return AddIntList(pFaceMaterialIndices,numFaces,TupTriMesh::FACE_MATERIAL_INDICES);
}

TupperwareList *TupTriMesh::AddFaceShadingGroups(const int *pFaceShadingGroups,int numFaces)
{
	return AddIntList(pFaceShadingGroups,numFaces,TupTriMesh::FACE_SHADING_GROUPS);
}

TupperwareList *TupTriMesh::AddFaceMaterialRefs(const int *pFaceMaterialRefs,int numFaces)
{
	return AddIntList(pFaceMaterialRefs,numFaces,TupTriMesh::FACE_MATERIAL_REFS);
}

TupperwareAggregate *TupTriMesh::AddMeshMap(int channelNum)
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

TupperwareAggregate *TupTriMesh::AddTriangleStrips(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[TRIANGLE_STRIPS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	pAtom = m_cachedAtoms[TRIANGLE_STRIPS] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[TRIANGLE_STRIPS]);
	return (TupperwareAggregate *)pAtom;
}

TupperwareList *TupTriMesh::GetPositionVerts(float **pVerts,int &numVerts)
{
	TupperwareList *pList = GetFloatList(pVerts,numVerts,TupTriMesh::POSITION_VERTS);
	// need to divide since there are 3 entrys per vertex
	numVerts/=3;
	return pList;
}

TupperwareList *TupTriMesh::GetPositionIndices(int **pIndices,int &numFaces)
{
	TupperwareList *pList = GetIntList(pIndices,numFaces,TupTriMesh::POSITION_INDICES);
	// need to divide since there are 3 entrys per face
	numFaces/=3;
	return pList;
}

TupperwareList *TupTriMesh::GetNormalVerts(float **pVerts,int &numVerts)
{
	TupperwareList *pList = GetFloatList(pVerts,numVerts,TupTriMesh::NORMAL_VERTS);
	// need to divide since there are 3 entrys per vertex
	numVerts/=3;
	return pList;
}

TupperwareList *TupTriMesh::GetNormalIndices(int **pIndices,int &numFaces)
{
	TupperwareList *pList = GetIntList(pIndices,numFaces,TupTriMesh::NORMAL_INDICES);
	// need to divide since there are 3 entrys per face
	numFaces/=3;
	return pList;
}

TupperwareList *TupTriMesh::GetFaceNormalVerts(float **pVerts,int &numVerts)
{
	TupperwareList *pList = GetFloatList(pVerts,numVerts,TupTriMesh::FACE_NORMAL_VERTS);
	// need to divide since there are 3 entrys per vertex
	numVerts/=3;
	return pList;
}

TupperwareList *TupTriMesh::GetFaceNormalIndices(int **pIndices,int &numFaces)
{
	TupperwareList *pList = GetIntList(pIndices,numFaces,TupTriMesh::FACE_NORMAL_INDICES);
	// don't need to divide due to one entry per face
	return pList;
}

TupperwareList *TupTriMesh::GetFaceFlags(int **pFaceFlags,int &numFaces)
{
	TupperwareList *pList = GetIntList(pFaceFlags,numFaces,TupTriMesh::FACE_FLAGS);
	// don't need to divide due to one entry per face
	return pList;
}

TupperwareList *TupTriMesh::GetFaceMaterialIndices(int **pFaceMaterialIndices,int &numFaces)
{
	TupperwareList *pList = GetIntList(pFaceMaterialIndices,numFaces,TupTriMesh::FACE_MATERIAL_INDICES);
	// don't need to divide due to one entry per face
	return pList;
}

TupperwareList *TupTriMesh::GetFaceShadingGroups(int **pFaceShadingGroups,int &numFaces)
{
	TupperwareList *pList = GetIntList(pFaceShadingGroups,numFaces,TupTriMesh::FACE_SHADING_GROUPS);
	// don't need to divide due to one entry per face
	return pList;
}

TupperwareList *TupTriMesh::GetFaceMaterialRefs(int **pFaceMaterialRefs,int &numFaces)
{
	return GetIntList(pFaceMaterialRefs,numFaces,TupTriMesh::FACE_MATERIAL_REFS);
}

int TupTriMesh::GetNumMeshMaps(void)
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

TupperwareAggregate *TupTriMesh::GetMeshMap(int mapIndex)
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

TupperwareAggregate *TupTriMesh::GetTriangleStrips(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[TRIANGLE_STRIPS];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}

// Private Methods
TupperwareList *TupTriMesh::AddFloatList(const float *pFloats,int numFloats,TupTriMesh::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupTriMesh::AddIntList(const int *pInts,int numInts,TupTriMesh::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareList *TupTriMesh::GetFloatList(float **pFloats,int &numFloats,TupTriMesh::ChildrenAtoms index)
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

TupperwareList *TupTriMesh::GetIntList(int **pInts,int &numInts,TupTriMesh::ChildrenAtoms index)
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
