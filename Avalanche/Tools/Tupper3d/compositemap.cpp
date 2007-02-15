////////////////////////////////////////////////////////////////////////////
//
// CompositeMap
//
// Class to handle the composite map
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: compositemap.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/18/02    Time: 3:54p
 * Updated in $/Avalanche/tools/Tupper3d
 * oops
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
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "compositemap.h"
#include "map.h"

char *TupCompositeMap::m_keyNames[TupCompositeMap::NUM_CHILDREN_ATOMS] = 
{
		"Name", // NAME
		"SubMaps", // SUB_MAPS
};

TupCompositeMap::TupCompositeMap(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupCompositeMap::TupCompositeMap(TupMap &mapBase)
{
	m_pObject = mapBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupCompositeMap::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupCompositeMap::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareAggregate *TupCompositeMap::AddSubMap(int index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[SUB_MAPS];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[SUB_MAPS] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[SUB_MAPS]);
	}
	TupperwareAggregate *pSubMapsAgg = (TupperwareAggregate *)pAtom;

	char indexString[20];
	itoa(index,indexString,10);
	TupperwareAtom *pSubMapAtom = pSubMapsAgg->FindAtomByKeyAndName("SubMap",indexString);
	if (pSubMapAtom)
	{
		pSubMapsAgg->DeleteAtom(pSubMapAtom);
	}
	return pSubMapsAgg->AddAggregate("SubMap",indexString);

}

TupperwareScalar *TupCompositeMap::GetName(char **name)
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

int TupCompositeMap::GetNumSubMaps(void)
{
	int total = 0;
	TupperwareAtom *pSubMapsAtom = m_cachedAtoms[SUB_MAPS];
	// if right type
	if (pSubMapsAtom && pSubMapsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pSubMapsAgg = (TupperwareAggregate *)pSubMapsAtom;
		// make sure it has children
		if (pSubMapsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pSubMapsAgg->FindByKey("SubMap"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pSubMapAtom = i.GetAsAtom();
				if (pSubMapAtom && pSubMapAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					total++;
				}
			}
		}
	}
	return total;
}

TupperwareAggregate *TupCompositeMap::GetSubMap(int index)
{
	int count = 0;
	TupperwareAtom *pSubMapsAtom = m_cachedAtoms[SUB_MAPS];
	// if right type
	if (pSubMapsAtom && pSubMapsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pSubMapsAgg = (TupperwareAggregate *)pSubMapsAtom;
		// make sure it has children
		if (pSubMapsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pSubMapsAgg->FindByKey("SubMap"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pSubMapAtom = i.GetAsAtom();
				if (pSubMapAtom && pSubMapAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					if (index == count)
						return (TupperwareAggregate *)pSubMapAtom;
					count++;
				}
			}
		}
	}
	return NULL;
}
