////////////////////////////////////////////////////////////////////////////
//
// HoldVCModifier
//
// Class to handle the hold vertex color modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: holdvcmodifier.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/06/02    Time: 3:03p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added support for the HoldVC Modifier
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "holdvcmodifier.h"
#include "modifier.h"

char *TupHoldVCModifier::m_keyNames[TupHoldVCModifier::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"ColorVerts", // COLOR_VERTS
	"ColorIndices", // COLOR_INDICES
	"Flags", // FLAGS
};

TupHoldVCModifier::TupHoldVCModifier(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupHoldVCModifier::TupHoldVCModifier(TupModifier &modifierBase)
{
	m_pObject = modifierBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupHoldVCModifier::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupHoldVCModifier::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareScalar *TupHoldVCModifier::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupHoldVCModifier::AddColorVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupHoldVCModifier::COLOR_VERTS);
}

TupperwareList *TupHoldVCModifier::AddColorIndices(const int *pIndices,int numFaces)
{
	return AddIntList(pIndices,numFaces*3,TupHoldVCModifier::COLOR_INDICES);
}

TupperwareScalar *TupHoldVCModifier::AddFlags(int value)
{
	return AddIntScalar(value,TupHoldVCModifier::FLAGS);
}

TupperwareScalar *TupHoldVCModifier::GetName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
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

TupperwareList *TupHoldVCModifier::GetColorVerts(float **pVerts,int &numVerts)
{
	TupperwareList *list = GetFloatList(pVerts,numVerts,TupHoldVCModifier::COLOR_VERTS);
	numVerts/=3;
	return list;
}

TupperwareList *TupHoldVCModifier::GetColorIndices(int **pVerts,int &numFaces)
{
	TupperwareList *list = GetIntList(pVerts,numFaces,TupHoldVCModifier::COLOR_INDICES);
	numFaces/=3;
	return list;
}

TupperwareScalar *TupHoldVCModifier::GetFlags(int &value)
{
	return GetIntScalar(value,TupHoldVCModifier::FLAGS);
}


TupperwareList *TupHoldVCModifier::AddFloatList(const float *pFloats,int numFloats,TupHoldVCModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupHoldVCModifier::AddIntList(const int *pInts,int numInts,TupHoldVCModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupHoldVCModifier::GetFloatList(float **pFloats,int &numFloats,TupHoldVCModifier::ChildrenAtoms index)
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

TupperwareList *TupHoldVCModifier::GetIntList(int **pInts,int &numInts,TupHoldVCModifier::ChildrenAtoms index)
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

TupperwareScalar *TupHoldVCModifier::AddIntScalar(int value,TupHoldVCModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupHoldVCModifier::GetIntScalar(int &value,TupHoldVCModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = scalar->GetAsInt();
			return scalar;
		}
	}
	value = 0;
	return NULL;
}
