////////////////////////////////////////////////////////////////////////////
//
// TriMeshMap
//
// Class to handle meshmaps for the triangle meshes
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "trimeshmap.h"

char *TupTriMeshMap::m_keyNames[TupTriMeshMap::NUM_CHILDREN_ATOMS] = 
{
	"MapName", // MAP_NAME
	"MapFlags", // MAP_FLAGS
	"MapVerts", // MAP_VERTS
	"MapIndices", //MAP_INDICES
};

TupTriMeshMap::TupTriMeshMap(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupTriMeshMap::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareList *TupTriMeshMap::AddFloatList(const float *pFloats,int numFloats,TupTriMeshMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupTriMeshMap::AddIntList(const int *pInts,int numInts,TupTriMeshMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareScalar *TupTriMeshMap::AddMapName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[MAP_NAME],NULL,name);
	m_cachedAtoms[MAP_NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupTriMeshMap::AddMapFlags(int mapFlags)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[MAP_FLAGS],NULL,mapFlags);
	m_cachedAtoms[MAP_FLAGS] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupTriMeshMap::AddMapVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupTriMeshMap::MAP_VERTS);
}

TupperwareList *TupTriMeshMap::AddMapIndices(const int *pIndices,int numFaces)
{
	return AddIntList(pIndices,numFaces*3,TupTriMeshMap::MAP_INDICES);
}

TupperwareScalar *TupTriMeshMap::GetMapName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_NAME];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*name = pScalar->GetAsString();
			return pScalar;
		}
	}
	*name = NULL;
	return NULL;
}

int TupTriMeshMap::GetMapChannel(void)
{
	const char *pName = m_pObject->GetName();
	assert(pName); // this should always have a name
	return atoi(pName);
}

TupperwareScalar *TupTriMeshMap::GetMapFlags(int &mapFlags)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_FLAGS];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			mapFlags = pScalar->GetAsInt();
			return pScalar;
		}
	}
	// if we could not get the data then return a null pointer and init the flags to 0
	mapFlags = 0;
	return NULL;
}

TupperwareList *TupTriMeshMap::GetFloatList(float **pFloats,int &numFloats,TupTriMeshMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			numFloats = pList->GetLength();
			*pFloats = pList->GetAsFloat();
			return pList;
		}
	}
	// if we could not get the data then return a null pointer and init the count to 0
	*pFloats = NULL;
	numFloats = 0;
	return NULL;
}

TupperwareList *TupTriMeshMap::GetIntList(int **pInts,int &numInts,TupTriMeshMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_INT)
		{
			numInts = pList->GetLength();
			*pInts = pList->GetAsInt();
			return pList;
		}
	}
	// if we could not get the data then return a null pointer and init the count to 0
	*pInts = NULL;
	numInts = 0;
	return NULL;
}

TupperwareList *TupTriMeshMap::GetMapVerts(float **pVerts,int &numVerts)
{
	TupperwareList *list = GetFloatList(pVerts,numVerts,TupTriMeshMap::MAP_VERTS);
	// need to divide since there are 3 entrys per vertex
	numVerts/=3;
	return list;
}

TupperwareList *TupTriMeshMap::GetMapIndices(int **pVerts,int &numFaces)
{
	TupperwareList *list = GetIntList(pVerts,numFaces,TupTriMeshMap::MAP_INDICES);
	// need to divide since there are 3 entrys per face
	numFaces/=3;
	return list;
}
