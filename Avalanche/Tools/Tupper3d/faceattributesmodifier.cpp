////////////////////////////////////////////////////////////////////////////
//
// FaceAttributesModifier
//
// Class to handle the Face Attributes modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: faceattributesmodifier.cpp $
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
 * User: Adam Clayton Date: 3/19/02    Time: 3:09p
 * Updated in $/Avalanche/tools/Tupper3d
 * Face Attributes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "faceattributesmodifier.h"
#include "modifier.h"

char *TupFaceAttributesModifier::m_keyNames[TupFaceAttributesModifier::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Flags", // FLAGS
};

TupFaceAttributesModifier::TupFaceAttributesModifier(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupFaceAttributesModifier::TupFaceAttributesModifier(TupModifier &modifierBase)
{
	m_pObject = modifierBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupFaceAttributesModifier::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupFaceAttributesModifier::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareScalar *TupFaceAttributesModifier::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupFaceAttributesModifier::AddFlags(int value)
{
	return AddIntScalar(value,TupFaceAttributesModifier::FLAGS);
}

TupperwareScalar *TupFaceAttributesModifier::GetName(char **name)
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

TupperwareScalar *TupFaceAttributesModifier::GetFlags(int &value)
{
	return GetIntScalar(value,TupFaceAttributesModifier::FLAGS);
}

TupperwareScalar *TupFaceAttributesModifier::AddIntScalar(int value,TupFaceAttributesModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupFaceAttributesModifier::GetIntScalar(int &value,TupFaceAttributesModifier::ChildrenAtoms index)
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
