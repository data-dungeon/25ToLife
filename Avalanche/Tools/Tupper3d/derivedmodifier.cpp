////////////////////////////////////////////////////////////////////////////
//
// DerivedModifier
//
// Class to handle the modifier information within a derived object
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: derivedmodifier.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:55p
 * Updated in $/Avalanche/tools/Tupper3d
 * Modifier Index
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * xrefs and derived objects
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "derivedmodifier.h"

char *TupDerivedModifier::m_keyNames[TupDerivedModifier::NUM_CHILDREN_ATOMS] = 
{
	"ModifierReference", // MODIFIER_REFERENCE
	"ModifierContext", // MODIFIER_CONTEXT
};

TupDerivedModifier::TupDerivedModifier(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupDerivedModifier::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupDerivedModifier::AddModifierReference(int modifierReference)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MODIFIER_REFERENCE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[MODIFIER_REFERENCE],NULL,modifierReference);
	m_cachedAtoms[MODIFIER_REFERENCE] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareAggregate *TupDerivedModifier::AddModifierContext(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MODIFIER_CONTEXT];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *aggregate = m_pObject->AddAggregate(m_keyNames[MODIFIER_CONTEXT],NULL);
	m_cachedAtoms[MODIFIER_CONTEXT] = (TupperwareAtom *)aggregate;
	return aggregate;
}

int TupDerivedModifier::GetModifierIndex(void)
{
	const char *pName = m_pObject->GetName();
	assert(pName); // this should always have a name
	return atoi(pName);
}

TupperwareScalar *TupDerivedModifier::GetModifierReference(int &modifierReference)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MODIFIER_REFERENCE];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_INT)
		{
			modifierReference = pScalar->GetAsInt();
			return pScalar;
		}
	}
	modifierReference = -1;
	return NULL;
}

TupperwareAggregate *TupDerivedModifier::GetModifierContext(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MODIFIER_CONTEXT];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}
