////////////////////////////////////////////////////////////////////////////
//
// TriangleStrip
//
// Class to handle the mesh triangle strips
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: triangleStrip.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Triangle Strip classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "triangleStrip.h"

// TupTriangleStripPool methods
TupTriangleStripPool::TupTriangleStripPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pTriangleStripPool = new TupperwarePoolValueString(pObject,"TriangleStrip");
}

TupTriangleStripPool::~TupTriangleStripPool()
{
	delete m_pTriangleStripPool;
}

int TupTriangleStripPool::AddTriangleStrip( void )
{
	TupperwareAggregate *pAgg = m_pTriangleStripPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupTriangleStripPool::AddTriangleStrip(int index)
{
	return m_pTriangleStripPool->AddAggregate(index);
}

TupperwareAggregate *TupTriangleStripPool::GetTriangleStrip(int index)
{
	return m_pTriangleStripPool->FindAggregate(index);
}

int TupTriangleStripPool::GetNumTriangleStrips(void)
{
	return m_pTriangleStripPool->GetPoolSize();
}


// TupTriangleStrip methods
char *TupTriangleStrip::m_keyNames[TupTriangleStrip::NUM_CHILDREN_ATOMS] = 
{
	"FaceIndices", // FACE_INDICES
	"FacePointIndices", // FACE_POINT_INDICES
};

TupTriangleStrip::TupTriangleStrip(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupTriangleStrip::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareList *TupTriangleStrip::AddIntList(const int *pInts,int numInts,TupTriangleStrip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupTriangleStrip::AddFaceIndices(const int *pFaceIndices,int numFaceIndices)
{
	return AddIntList(pFaceIndices,numFaceIndices,TupTriangleStrip::FACE_INDICES);
}

TupperwareList *TupTriangleStrip::AddFacePointIndices(const int *pFacePointIndices,int numFacePointIndices)
{
	return AddIntList(pFacePointIndices,numFacePointIndices,TupTriangleStrip::FACE_POINT_INDICES);
}

TupperwareList *TupTriangleStrip::GetIntList(int **pInts,int &numInts,TupTriangleStrip::ChildrenAtoms index)
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

TupperwareList *TupTriangleStrip::GetFaceIndices(int **pFaceIndices,int &numFaceIndices)
{
	return GetIntList(pFaceIndices,numFaceIndices,TupTriangleStrip::FACE_INDICES);
}

TupperwareList *TupTriangleStrip::GetFacePointIndices(int **pFacePointIndices,int &numFacePointIndices)
{
	return GetIntList(pFacePointIndices,numFacePointIndices,TupTriangleStrip::FACE_POINT_INDICES);
}

