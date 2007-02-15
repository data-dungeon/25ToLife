////////////////////////////////////////////////////////////////////////////
//
// ApplyVCModifier
//
// Class to handle the apply vertex color modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: applyvcmodifier.cpp $
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 1/28/03    Time: 12:08p
 * Updated in $/Avalanche/tools/Tupper3d
 * vertex alpha added to apply vc modifier
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 1:08p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Apply Vertex Color Modifiers to Derived objects
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/03/02    Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier (pass 1)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/02/02    Time: 4:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Apply VC Modifier complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:33p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifiers
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "applyvcmodifier.h"
#include "modifier.h"

char *TupApplyVCModifier::m_keyNames[TupApplyVCModifier::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"ColorVerts", // COLOR_VERTS
	"ColorIndices", // COLOR_INDICES
	"AlphaVerts", // ALPHA_VERTS
	"AlphaIndices", // ALPHA_INDICES
	"Flags", // FLAGS
};

TupApplyVCModifier::TupApplyVCModifier(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupApplyVCModifier::TupApplyVCModifier(TupModifier &modifierBase)
{
	m_pObject = modifierBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupApplyVCModifier::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupApplyVCModifier::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareScalar *TupApplyVCModifier::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupApplyVCModifier::AddColorVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts*3,TupApplyVCModifier::COLOR_VERTS);
}

TupperwareList *TupApplyVCModifier::AddColorIndices(const int *pIndices,int numIndices)
{
	return AddIntList(pIndices,numIndices,TupApplyVCModifier::COLOR_INDICES);
}

TupperwareList *TupApplyVCModifier::AddAlphaVerts(const float *pVerts,int numVerts)
{
	return AddFloatList(pVerts,numVerts,TupApplyVCModifier::ALPHA_VERTS);
}

TupperwareList *TupApplyVCModifier::AddAlphaIndices(const int *pIndices,int numIndices)
{
	return AddIntList(pIndices,numIndices,TupApplyVCModifier::ALPHA_INDICES);
}

TupperwareScalar *TupApplyVCModifier::AddFlags(int value)
{
	return AddIntScalar(value,TupApplyVCModifier::FLAGS);
}

TupperwareScalar *TupApplyVCModifier::GetName(char **name)
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

TupperwareList *TupApplyVCModifier::GetColorVerts(float **pVerts,int &numVerts)
{
	TupperwareList *list = GetFloatList(pVerts,numVerts,TupApplyVCModifier::COLOR_VERTS);
	numVerts/=3;
	return list;
}

TupperwareList *TupApplyVCModifier::GetColorIndices(int **pVerts,int &numIndices)
{
	TupperwareList *list = GetIntList(pVerts,numIndices,TupApplyVCModifier::COLOR_INDICES);
	return list;
}

TupperwareList *TupApplyVCModifier::GetAlphaVerts(float **pVerts,int &numVerts)
{
	TupperwareList *list = GetFloatList(pVerts,numVerts,TupApplyVCModifier::ALPHA_VERTS);
	return list;
}

TupperwareList *TupApplyVCModifier::GetAlphaIndices(int **pVerts,int &numIndices)
{
	TupperwareList *list = GetIntList(pVerts,numIndices,TupApplyVCModifier::ALPHA_INDICES);
	return list;
}

TupperwareScalar *TupApplyVCModifier::GetFlags(int &value)
{
	return GetIntScalar(value,TupApplyVCModifier::FLAGS);
}


TupperwareList *TupApplyVCModifier::AddFloatList(const float *pFloats,int numFloats,TupApplyVCModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupApplyVCModifier::AddIntList(const int *pInts,int numInts,TupApplyVCModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupApplyVCModifier::GetFloatList(float **pFloats,int &numFloats,TupApplyVCModifier::ChildrenAtoms index)
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

TupperwareList *TupApplyVCModifier::GetIntList(int **pInts,int &numInts,TupApplyVCModifier::ChildrenAtoms index)
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

TupperwareScalar *TupApplyVCModifier::AddIntScalar(int value,TupApplyVCModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupApplyVCModifier::GetIntScalar(int &value,TupApplyVCModifier::ChildrenAtoms index)
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
