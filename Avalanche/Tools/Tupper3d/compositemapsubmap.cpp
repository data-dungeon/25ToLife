////////////////////////////////////////////////////////////////////////////
//
// CompositeSubMap
//
// Class to handle sub maps from the composite map
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: compositemapsubmap.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/17/01   Time: 11:06a
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "compositemapsubmap.h"

char *TupCompositeMapSubMap::m_keyNames[TupCompositeMapSubMap::NUM_CHILDREN_ATOMS] = 
{
	"MapReference", // MAP_REFERENCE
	"Flags", // FLAGS
};

TupCompositeMapSubMap::TupCompositeMapSubMap(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupCompositeMapSubMap::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupCompositeMapSubMap::AddMapReference(int value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_REFERENCE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[MAP_REFERENCE],NULL,value);
	m_cachedAtoms[MAP_REFERENCE] = (TupperwareAtom *)scalar;
	return scalar;
}

int TupCompositeMapSubMap::GetSubMapIndex(void)
{
	const char *pName = m_pObject->GetName();
	assert(pName); // this should always have a name
	return atoi(pName);
}

TupperwareScalar *TupCompositeMapSubMap::AddFlags(int value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[FLAGS],NULL,value);
	m_cachedAtoms[FLAGS] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupCompositeMapSubMap::GetMapReference(int &value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_REFERENCE];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = pScalar->GetAsInt();
			return pScalar;
		}
	}
	// if we could not get the data then return a null pointer and init the flags to 0
	value = 0;
	return NULL;
}

TupperwareScalar *TupCompositeMapSubMap::GetFlags(int &value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = pScalar->GetAsInt();
			return pScalar;
		}
	}
	// if we could not get the data then return a null pointer and init the flags to 0
	value = 0;
	return NULL;
}
