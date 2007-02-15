////////////////////////////////////////////////////////////////////////////
//
// ClothModifierContext
//
// Class to handle the Cloth modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ClothModifierContext.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 7/22/02    Time: 4:21p
 * Updated in $/Avalanche/tools/Tupper3d
 * Changed connections to 1 list instead of 2
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 2:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * Cloth Modifier
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "ClothModifierContext.h"

char *TupClothModifierContext::m_keyNames[TupClothModifierContext::NUM_CHILDREN_ATOMS] = 
{
	"PointFlags", // POINT_FLAGS
	"ConnectionPoints", // CONNECTION_POINTS
};

TupClothModifierContext::TupClothModifierContext(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupClothModifierContext::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupClothModifierContext::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareList *TupClothModifierContext::AddPointFlags(const int *pValues,int numPoints)
{
	return AddIntList(pValues,numPoints,TupClothModifierContext::POINT_FLAGS);
}

// connections are two ints per connection
TupperwareList *TupClothModifierContext::AddConnectionPoints(const int *pValues,int numConnections)
{
	return AddIntList(pValues,numConnections<<1,TupClothModifierContext::CONNECTION_POINTS);
}

// get methods

TupperwareList *TupClothModifierContext::GetPointFlags(int **pValues,int &numPoints)
{
	return GetIntList(pValues,numPoints,TupClothModifierContext::POINT_FLAGS);
}

// connections are two ints per connection
TupperwareList *TupClothModifierContext::GetConnectionPoints(int **pValues,int &numConnections)
{
	int numInts;
	TupperwareList *pList = GetIntList(pValues,numInts,TupClothModifierContext::CONNECTION_POINTS);
	numConnections = numInts>>1;
	return pList;
}

TupperwareList *TupClothModifierContext::AddIntList(const int *pInts,int numInts,TupClothModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}


TupperwareList *TupClothModifierContext::GetIntList(int **pInts,int &numInts,TupClothModifierContext::ChildrenAtoms index)
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

