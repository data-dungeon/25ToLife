////////////////////////////////////////////////////////////////////////////
//
// CompositeMaterialSubMaterial
//
// Class to handle materials that a composite material points to
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: compositematerialsubmaterial.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/13/01   Time: 10:56a
 * Updated in $/Avalanche/tools/Tupper3d
 * More Materials
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "compositematerialsubmaterial.h"

char *TupCompositeMaterialSubMaterial::m_keyNames[TupCompositeMaterialSubMaterial::NUM_CHILDREN_ATOMS] = 
{
	"Flags", // FLAGS
	"MixAmount", // MIX_AMOUNT
	"MaterialReference", // MATERIAL_REFERENCE
};

TupCompositeMaterialSubMaterial::TupCompositeMaterialSubMaterial(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupCompositeMaterialSubMaterial::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupCompositeMaterialSubMaterial::AddFlags(int value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[FLAGS],NULL,value);
	m_cachedAtoms[FLAGS] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupCompositeMaterialSubMaterial::AddMixAmount(float value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MIX_AMOUNT];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[MIX_AMOUNT],NULL,value);
	m_cachedAtoms[MIX_AMOUNT] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupCompositeMaterialSubMaterial::AddMaterialReference(int materialIndex)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MATERIAL_REFERENCE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[MATERIAL_REFERENCE],NULL,materialIndex);
	m_cachedAtoms[MATERIAL_REFERENCE] = (TupperwareAtom *)scalar;
	return scalar;
}

int TupCompositeMaterialSubMaterial::GetSubMaterialIndex(void)
{
	const char *pName = m_pObject->GetName();
	assert(pName); // this should always have a name
	return atoi(pName);
}


TupperwareScalar *TupCompositeMaterialSubMaterial::GetFlags(int &value)
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

TupperwareScalar *TupCompositeMaterialSubMaterial::GetMixAmount(float &value)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MIX_AMOUNT];
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

TupperwareScalar *TupCompositeMaterialSubMaterial::GetMaterialReference(int &materialIndex)
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
