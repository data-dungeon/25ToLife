////////////////////////////////////////////////////////////////////////////
//
// SkinModifierContext
//
// Class for handling a context for a skin modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifiercontext.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/29/02    Time: 2:59p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added correct matrix lists
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "skinmodifiercontext.h"

char *TupSkinModifierContext::m_keyNames[TupSkinModifierContext::NUM_CHILDREN_ATOMS] = 
{
	"VertexPool", // VERTEX_POOL
	"BaseTM", // BASE_TM
};

TupSkinModifierContext::TupSkinModifierContext(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupSkinModifierContext::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupSkinModifierContext::GetBaseAggregate(void)
{
	return m_pObject;
}

// Add Methods
TupperwareAggregate *TupSkinModifierContext::AddVertexPool(void)
{
	return AddAggregate(TupSkinModifierContext::VERTEX_POOL);
}

TupperwareList *TupSkinModifierContext::AddBaseTM(const float *pValue)
{
	return AddFloatList(pValue,16,TupSkinModifierContext::BASE_TM);
}


// Get Methods
TupperwareAggregate *TupSkinModifierContext::GetVertexPool(void)
{
	return GetAggregate(TupSkinModifierContext::VERTEX_POOL);
}

TupperwareList *TupSkinModifierContext::GetBaseTM(float **pValue)
{
	int numValues;
	TupperwareList *pList = GetFloatList(pValue,numValues,TupSkinModifierContext::BASE_TM);
	return pList;
}

// private methods

TupperwareAggregate *TupSkinModifierContext::AddAggregate(TupSkinModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareAggregate *TupSkinModifierContext::GetAggregate(TupSkinModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}

TupperwareList *TupSkinModifierContext::AddFloatList(const float *pFloats,int numFloats,TupSkinModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupSkinModifierContext::GetFloatList(float **pFloats,int &numFloats,TupSkinModifierContext::ChildrenAtoms index)
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

