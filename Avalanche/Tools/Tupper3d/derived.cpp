////////////////////////////////////////////////////////////////////////////
//
// Derived
//
// Class for derived objects (objects that hold modifiers)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: derived.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 9:36a
 * Updated in $/Avalanche/tools/Tupper3d
 * Modifier access using slots
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * xrefs and derived objects
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "derived.h"
#include "object.h"

char *TupDerived::m_keyNames[TupDerived::NUM_CHILDREN_ATOMS] = 
{
	"Modifiers", // MODIFIERS
	"ObjectReference", //OBJECT_REFERENCE
};

TupDerived::TupDerived(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupDerived::TupDerived(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupDerived::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupDerived::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareAggregate *TupDerived::AddModifier(int index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MODIFIERS];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[MODIFIERS] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[MODIFIERS]);
	}
	TupperwareAggregate *pModifiersAgg = (TupperwareAggregate *)pAtom;

	char indexString[20];
	itoa(index,indexString,10);
	TupperwareAtom *pModifierAtom = pModifiersAgg->FindAtomByKeyAndName("Modifier",indexString);
	if (pModifierAtom)
	{
		pModifiersAgg->DeleteAtom(pModifierAtom);
	}
	return pModifiersAgg->AddAggregate("Modifier",indexString);
}

TupperwareScalar *TupDerived::AddObjectReference(int objectReference)
{
	TupperwareAtom *pAtom = m_cachedAtoms[OBJECT_REFERENCE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[OBJECT_REFERENCE],NULL,objectReference);
	m_cachedAtoms[OBJECT_REFERENCE] = (TupperwareAtom *)scalar;
	return scalar;
}


// returns the number of slots occupied used by the modifiers (some may be empty)
int TupDerived::GetNumModifierSlots(void)
{
	int maxIndex = -1;
	TupperwareAtom *pModifiersAtom = m_cachedAtoms[MODIFIERS];
	// if right type
	if (pModifiersAtom && pModifiersAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pModifiersAgg = (TupperwareAggregate *)pModifiersAtom;
		// make sure it has children
		if (pModifiersAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pModifiersAgg->FindByKey("Modifier"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pModifierAtom = i.GetAsAtom();
				if (pModifierAtom && pModifierAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					const char *pModifierName = pModifierAtom->GetName();
					int modifierSlot = atoi(pModifierAtom->GetName());
					if (modifierSlot>maxIndex)
						maxIndex = modifierSlot;
				}
			}
		}
	}
	return maxIndex+1;
}

// returns the modifier in slot# (if slot is empty then return NULL)
TupperwareAggregate *TupDerived::GetModifierBySlot(int slotIndex)
{
	int count = 0;
	TupperwareAtom *pModifiersAtom = m_cachedAtoms[MODIFIERS];
	// if right type
	if (pModifiersAtom && pModifiersAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pModifiersAgg = (TupperwareAggregate *)pModifiersAtom;
		char indexString[20];
		itoa(slotIndex,indexString,10);
		return pModifiersAgg->FindAggregateByKeyAndName("Modifier",indexString);
	}
	return NULL;
}

TupperwareScalar *TupDerived::GetObjectReference(int &objectReference)
{
	TupperwareAtom *pAtom = m_cachedAtoms[OBJECT_REFERENCE];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_INT)
		{
			objectReference = pScalar->GetAsInt();
			return pScalar;
		}
	}
	objectReference = -1;
	return NULL;
}

