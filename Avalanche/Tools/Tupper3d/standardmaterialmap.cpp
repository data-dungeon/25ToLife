////////////////////////////////////////////////////////////////////////////
//
// StandardMaterialMap
//
// Class to handle maps that the standard material uses
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: standardmaterialmap.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 9:39a
 * Updated in $/Avalanche/tools/Tupper3d
 * Changed from value shading types to a string.  Made the map now have a
 * mapname instead of reading from a table.  The name of the map aggregate
 * is the slot number instead.
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "standardmaterialmap.h"

char *TupStandardMaterialMap::m_keyNames[TupStandardMaterialMap::NUM_CHILDREN_ATOMS] = 
{
	"Amount", // AMOUNT
	"Flags", // FLAGS
	"MapReference", // MAP_REFERENCE
	"MapName", // MAP_NAME
};

TupStandardMaterialMap::TupStandardMaterialMap(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupStandardMaterialMap::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupStandardMaterialMap::AddMapName(const char *mapName)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[MAP_NAME],NULL,mapName);
	m_cachedAtoms[MAP_NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupStandardMaterialMap::AddMapReference(int value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_REFERENCE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[MAP_REFERENCE],NULL,value);
	m_cachedAtoms[MAP_REFERENCE] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupStandardMaterialMap::AddFlags(int value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[FLAGS],NULL,value);
	m_cachedAtoms[FLAGS] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupStandardMaterialMap::AddAmount(float value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[AMOUNT];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[AMOUNT],NULL,value);
	m_cachedAtoms[AMOUNT] = (TupperwareAtom *)scalar;
	return scalar;
}

int TupStandardMaterialMap::GetMapChannel(void)
{
	const char *pName = m_pObject->GetName();
	assert(pName); // this should always have a name
	return atoi(pName);
}

TupperwareScalar *TupStandardMaterialMap::GetMapName(char **mapName)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_NAME];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			*mapName = pScalar->GetAsString();
			return pScalar;
		}
	}
	// if we could not get the data then return a null pointer and init the flags to 0
	*mapName = NULL;
	return NULL;
}

TupperwareScalar *TupStandardMaterialMap::GetMapReference(int &value)
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

TupperwareScalar *TupStandardMaterialMap::GetFlags(int &value)
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

TupperwareScalar *TupStandardMaterialMap::GetAmount(float &value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[AMOUNT];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
		{
			value = pScalar->GetAsFloat();
			return pScalar;
		}
	}
	// if we could not get the data then return a null pointer and init the flags to 0
	value = 0;
	return NULL;
}
