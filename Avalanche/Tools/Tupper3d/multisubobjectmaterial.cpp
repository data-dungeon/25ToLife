////////////////////////////////////////////////////////////////////////////
//
// MultiSubObjectMaterial
//
// Class to handle a multi sub object material
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: multisubobjectmaterial.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/13/01   Time: 10:56a
 * Updated in $/Avalanche/tools/Tupper3d
 * More Materials
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:18p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "multisubobjectmaterial.h"
#include "material.h"

char *TupMultiSubObjectMaterial::m_keyNames[TupMultiSubObjectMaterial::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"SubMaterials", // SUB_MATERIALS
};

TupMultiSubObjectMaterial::TupMultiSubObjectMaterial(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupMultiSubObjectMaterial::TupMultiSubObjectMaterial(TupMaterial &materialBase)
{
	m_pObject = materialBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupMultiSubObjectMaterial::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupMultiSubObjectMaterial::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareAggregate *TupMultiSubObjectMaterial::AddSubMaterial(int index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[SUB_MATERIALS];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[SUB_MATERIALS] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[SUB_MATERIALS]);
	}
	TupperwareAggregate *pSubMatsAgg = (TupperwareAggregate *)pAtom;

	char indexString[20];
	itoa(index,indexString,10);
	TupperwareAtom *pSubMatAtom = pSubMatsAgg->FindAtomByKeyAndName("SubMaterial",indexString);
	if (pSubMatAtom)
	{
		pSubMatsAgg->DeleteAtom(pSubMatAtom);
	}
	return pSubMatsAgg->AddAggregate("SubMaterial",indexString);

}

TupperwareScalar *TupMultiSubObjectMaterial::GetName(char **name)
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

int TupMultiSubObjectMaterial::GetNumSubMaterials(void)
{
	int total = 0;
	TupperwareAtom *pSubMaterialsAtom = m_cachedAtoms[SUB_MATERIALS];
	// if right type
	if (pSubMaterialsAtom && pSubMaterialsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pSubMaterialsAgg = (TupperwareAggregate *)pSubMaterialsAtom;
		// make sure it has children
		if (pSubMaterialsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pSubMaterialsAgg->FindByKey("SubMaterial"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pSubMaterialAtom = i.GetAsAtom();
				if (pSubMaterialAtom && pSubMaterialAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					total++;
				}
			}
		}
	}
	return total;
}

TupperwareAggregate *TupMultiSubObjectMaterial::GetSubMaterial(int index)
{
	int count = 0;
	TupperwareAtom *pSubMaterialsAtom = m_cachedAtoms[SUB_MATERIALS];
	// if right type
	if (pSubMaterialsAtom && pSubMaterialsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pSubMaterialsAgg = (TupperwareAggregate *)pSubMaterialsAtom;
		// make sure it has children
		if (pSubMaterialsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pSubMaterialsAgg->FindByKey("SubMaterial"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pSubMaterialAtom = i.GetAsAtom();
				if (pSubMaterialAtom && pSubMaterialAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					if (index == count)
						return (TupperwareAggregate *)pSubMaterialAtom;
					count++;
				}
			}
		}
	}
	return NULL;
}

