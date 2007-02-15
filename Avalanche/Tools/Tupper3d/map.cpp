////////////////////////////////////////////////////////////////////////////
//
// Map
//
// Class to handle the texture types
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: map.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/30/02    Time: 9:40a
 * Updated in $/Avalanche/tools/Tupper3d
 * Map Name access on TupMap
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/13/02    Time: 1:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Mix Map
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/31/01   Time: 3:37p
 * Updated in $/Avalanche/tools/Tupper3d
 * modifiers pass 1
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "map.h"

char *TupMap::m_typeNames[TupMap::LAST_ENTRY] = 
{
	"Bitmap", // BITMAP_MAP
	"Composite", // COMPOSITE_MAP
	"Mix", // MIX_MAP
	"Unknown", // UNKNOWN
};

char *TupMap::m_keyNames[TupMap::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"MapType", // MAP_TYPE
};

TupMap::TupMap(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupMap::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupMap::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

void TupMap::SetMapType(TupMap::MapTypes type)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_TYPE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[MAP_TYPE],NULL,m_typeNames[type]);
	m_cachedAtoms[MAP_TYPE] = (TupperwareAtom *)scalar;
}

TupperwareScalar *TupMap::GetName(char **name)
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

TupperwareAggregate *TupMap::GetBaseAggregate(void)
{
	return m_pObject;
}

TupMap::MapTypes TupMap::GetMapType(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAP_TYPE];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			for (int i=0;i<TupMap::UNKNOWN;i++)
			{
				if (!strcmp(m_typeNames[i],pScalar->GetAsString()))
				{
					return (TupMap::MapTypes)i;
				}
			}
		}
	}
	return TupMap::UNKNOWN;
}
