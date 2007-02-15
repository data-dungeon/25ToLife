////////////////////////////////////////////////////////////////////////////
//
// VertexPaintModifier
//
// Class to handle the vertex paint modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: vertexpaintmodifier.cpp $
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
 * User: Adam Clayton Date: 3/05/02    Time: 1:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Apply Vertex Color Modifiers to Derived objects
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/03/02    Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier (pass 1)
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "vertexpaintmodifier.h"
#include "modifier.h"

char *TupVertexPaintModifier::m_keyNames[TupVertexPaintModifier::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Flags", // FLAGS
};

TupVertexPaintModifier::TupVertexPaintModifier(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupVertexPaintModifier::TupVertexPaintModifier(TupModifier &modifierBase)
{
	m_pObject = modifierBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupVertexPaintModifier::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupVertexPaintModifier::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareScalar *TupVertexPaintModifier::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupVertexPaintModifier::AddFlags(int value)
{
	return AddIntScalar(value,TupVertexPaintModifier::FLAGS);
}

TupperwareScalar *TupVertexPaintModifier::GetName(char **name)
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

TupperwareScalar *TupVertexPaintModifier::GetFlags(int &value)
{
	return GetIntScalar(value,TupVertexPaintModifier::FLAGS);
}

TupperwareScalar *TupVertexPaintModifier::AddIntScalar(int value,TupVertexPaintModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupVertexPaintModifier::GetIntScalar(int &value,TupVertexPaintModifier::ChildrenAtoms index)
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
