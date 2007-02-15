////////////////////////////////////////////////////////////////////////////
//
// UnknownModifier
//
// Class to handle the unknown modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: unknownmodifier.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * Get BaseAggregate
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/04/02    Time: 1:23p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier
 * Unknown Modifier
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "unknownmodifier.h"
#include "modifier.h"

char *TupUnknownModifier::m_keyNames[TupUnknownModifier::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Flags", // FLAGS
};

TupUnknownModifier::TupUnknownModifier(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupUnknownModifier::TupUnknownModifier(TupModifier &modifierBase)
{
	m_pObject = modifierBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupUnknownModifier::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupUnknownModifier::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareScalar *TupUnknownModifier::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupUnknownModifier::AddFlags(int value)
{
	return AddIntScalar(value,TupUnknownModifier::FLAGS);
}

TupperwareScalar *TupUnknownModifier::GetName(char **name)
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

TupperwareScalar *TupUnknownModifier::GetFlags(int &value)
{
	return GetIntScalar(value,TupUnknownModifier::FLAGS);
}

TupperwareScalar *TupUnknownModifier::AddIntScalar(int value,TupUnknownModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupUnknownModifier::GetIntScalar(int &value,TupUnknownModifier::ChildrenAtoms index)
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
