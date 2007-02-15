////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// SkinModifierContextVertex
//
// Class to handle a vertex within the skin modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifiercontextvertex.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "skinmodifiercontextvertex.h"

char *TupSkinModifierContextVertex::m_keyNames[TupSkinModifierContextVertex::NUM_CHILDREN_ATOMS] = 
{
	"BONES", // BONES
	"WEIGHTS", // WEIGHTS
};

TupSkinModifierContextVertex::TupSkinModifierContextVertex(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupSkinModifierContextVertex::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupSkinModifierContextVertex::GetBaseAggregate(void)
{
	return m_pObject;
}

// add methods
TupperwareList *TupSkinModifierContextVertex::AddBones(const int *pValues,int numBones)
{
	return AddIntList(pValues,numBones,TupSkinModifierContextVertex::BONES);
}

TupperwareList *TupSkinModifierContextVertex::AddWeights(const float *pValues,int numBones)
{
	return AddFloatList(pValues,numBones,TupSkinModifierContextVertex::WEIGHTS);
}

// get methods
TupperwareList *TupSkinModifierContextVertex::GetBones(int **pValues,int &numBones)
{
	return GetIntList(pValues,numBones,TupSkinModifierContextVertex::BONES);
}

TupperwareList *TupSkinModifierContextVertex::GetWeights(float **pValues,int &numBones)
{
	return GetFloatList(pValues,numBones,TupSkinModifierContextVertex::WEIGHTS);
}

// Private Methods
TupperwareList *TupSkinModifierContextVertex::AddFloatList(const float *pFloats,int numFloats,TupSkinModifierContextVertex::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupSkinModifierContextVertex::AddIntList(const int *pInts,int numInts,TupSkinModifierContextVertex::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareList *TupSkinModifierContextVertex::GetFloatList(float **pFloats,int &numFloats,TupSkinModifierContextVertex::ChildrenAtoms index)
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

TupperwareList *TupSkinModifierContextVertex::GetIntList(int **pInts,int &numInts,TupSkinModifierContextVertex::ChildrenAtoms index)
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

