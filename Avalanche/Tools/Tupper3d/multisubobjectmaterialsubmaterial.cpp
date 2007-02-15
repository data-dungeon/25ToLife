////////////////////////////////////////////////////////////////////////////
//
// MultiSubObjectMaterialSubMaterial
//
// Class to handle materials that a multi sub object material points to
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: multisubobjectmaterialsubmaterial.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:18p
 * Updated in $/Avalanche/tools/Tupper3d
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "multisubobjectmaterialsubmaterial.h"

char *TupMultiSubObjectMaterialSubMaterial::m_keyNames[TupMultiSubObjectMaterialSubMaterial::NUM_CHILDREN_ATOMS] = 
{
	"SlotName", // SLOT_NAME,
	"Flags", // FLAGS
	"MaterialReference", // MATERIAL_REFERENCE
};

TupMultiSubObjectMaterialSubMaterial::TupMultiSubObjectMaterialSubMaterial(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupMultiSubObjectMaterialSubMaterial::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupMultiSubObjectMaterialSubMaterial::AddSlotName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[SLOT_NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[SLOT_NAME],NULL,name);
	m_cachedAtoms[SLOT_NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupMultiSubObjectMaterialSubMaterial::AddFlags(int value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[FLAGS],NULL,value);
	m_cachedAtoms[FLAGS] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupMultiSubObjectMaterialSubMaterial::AddMaterialReference(int materialIndex)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MATERIAL_REFERENCE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[MATERIAL_REFERENCE],NULL,materialIndex);
	m_cachedAtoms[MATERIAL_REFERENCE] = (TupperwareAtom *)scalar;
	return scalar;
}

int TupMultiSubObjectMaterialSubMaterial::GetSubMaterialIndex(void)
{
	const char *pName = m_pObject->GetName();
	assert(pName); // this should always have a name
	return atoi(pName);
}

TupperwareScalar *TupMultiSubObjectMaterialSubMaterial::GetSlotName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[SLOT_NAME];
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

TupperwareScalar *TupMultiSubObjectMaterialSubMaterial::GetFlags(int &value)
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

TupperwareScalar *TupMultiSubObjectMaterialSubMaterial::GetMaterialReference(int &materialIndex)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MATERIAL_REFERENCE];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			materialIndex = pScalar->GetAsInt();
			return pScalar;
		}
	}
	// if we could not get the data then return a null pointer and init the flags to 0
	materialIndex = 0;
	return NULL;
}
