////////////////////////////////////////////////////////////////////////////
//
// FaceAttributesModifierContext
//
// Class to handle the Face Attributes modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: faceattributesmodifiercontext.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/Tupper3d
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/19/02    Time: 3:09p
 * Updated in $/Avalanche/tools/Tupper3d
 * Face Attributes
 */
////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "faceattributesmodifiercontext.h"

char *TupFaceAttributesModifierContext::m_keyNames[TupFaceAttributesModifierContext::NUM_CHILDREN_ATOMS] = 
{
	"FaceAttributeVerts", // FACE_ATTRIBUTE_VERTS
	"FaceAttributeIndices", // FACE_ATTRIBUTE_INDICES
};

TupFaceAttributesModifierContext::TupFaceAttributesModifierContext(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupFaceAttributesModifierContext::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupFaceAttributesModifierContext::GetBaseAggregate(void)
{
	return m_pObject;
}


TupperwareList *TupFaceAttributesModifierContext::AddFaceAttributeVerts(const char **pValues,int numVerts)
{
	return AddStringList(pValues,numVerts,TupFaceAttributesModifierContext::FACE_ATTRIBUTE_VERTS);
}

TupperwareList *TupFaceAttributesModifierContext::AddFaceAttributeVerts(const TupArray<TupString> &strings)
{
	return AddStringList(strings,TupFaceAttributesModifierContext::FACE_ATTRIBUTE_VERTS);
}

TupperwareList *TupFaceAttributesModifierContext::AddFaceAttributeIndices(const int *pValues,int numIndices)
{
	return AddIntList(pValues,numIndices,TupFaceAttributesModifierContext::FACE_ATTRIBUTE_INDICES);
}

// get methods

TupperwareList *TupFaceAttributesModifierContext::GetFaceAttributeVerts(char ***pValues,int &numVerts)
{
	return GetStringList(pValues,numVerts,TupFaceAttributesModifierContext::FACE_ATTRIBUTE_VERTS);
}

TupperwareList *TupFaceAttributesModifierContext::GetFaceAttributeVerts(TupArray<TupString> &strings)
{
	return GetStringList(strings,TupFaceAttributesModifierContext::FACE_ATTRIBUTE_VERTS);
}

TupperwareList *TupFaceAttributesModifierContext::GetFaceAttributeIndices(int **pValues,int &numIndices)
{
	return GetIntList(pValues,numIndices,TupFaceAttributesModifierContext::FACE_ATTRIBUTE_INDICES);
}





TupperwareList *TupFaceAttributesModifierContext::AddStringList(const char **pStrings,int numStrings,TupFaceAttributesModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListString(m_keyNames[index],NULL,pStrings,numStrings);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupFaceAttributesModifierContext::AddStringList(const TupArray<TupString> &strings,TupFaceAttributesModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListString(m_keyNames[index],NULL,strings);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupFaceAttributesModifierContext::AddIntList(const int *pInts,int numInts,TupFaceAttributesModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupFaceAttributesModifierContext::GetStringList(char ***pStrings,int &numStrings,TupFaceAttributesModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_STRING)
		{
			numStrings = pList->GetLength();
			*pStrings = pList->GetAsString();
			return pList;
		}
	}
	// if we could not get the data then return a null pointer and init the count to 0
	*pStrings = NULL;
	numStrings = 0;
	return NULL;
}

TupperwareList *TupFaceAttributesModifierContext::GetStringList(TupArray<TupString> &strings,TupFaceAttributesModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_STRING)
		{
			pList->GetAsString(strings);
			return pList;
		}
	}
	// if we could not get the data then return a null pointer and init the count to 0
	strings.RemoveAll();
	return NULL;
}

TupperwareList *TupFaceAttributesModifierContext::GetIntList(int **pInts,int &numInts,TupFaceAttributesModifierContext::ChildrenAtoms index)
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

