////////////////////////////////////////////////////////////////////////////
//
// DoubleSidedMaterial
//
// Class to handle the double sided material 
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: doublesidedmaterial.cpp $
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
 * User: Adam Clayton Date: 12/13/01   Time: 10:56a
 * Updated in $/Avalanche/tools/Tupper3d
 * More Materials
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "doublesidedmaterial.h"
#include "material.h"

char *TupDoubleSidedMaterial::m_keyNames[TupDoubleSidedMaterial::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"FrontMaterialReference", // FRONT_MATERIAL_REFERENCE
	"BackMaterialReference", // BACK_MATERIAL_REFERENCE
	"Flags", // FLAGS
	"Translucency", // TRANSLUCENCY
};

TupDoubleSidedMaterial::TupDoubleSidedMaterial(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupDoubleSidedMaterial::TupDoubleSidedMaterial(TupMaterial &materialBase)
{
	m_pObject = materialBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupDoubleSidedMaterial::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupDoubleSidedMaterial::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupDoubleSidedMaterial::AddFrontMaterialReference(int value)
{
	return AddIntScalar(value,TupDoubleSidedMaterial::FRONT_MATERIAL_REFERENCE);
}

TupperwareScalar *TupDoubleSidedMaterial::AddBackMaterialReference(int value)
{
	return AddIntScalar(value,TupDoubleSidedMaterial::BACK_MATERIAL_REFERENCE);
}

TupperwareScalar *TupDoubleSidedMaterial::AddFlags(int value)
{
	return AddIntScalar(value,TupDoubleSidedMaterial::FLAGS);
}

TupperwareScalar *TupDoubleSidedMaterial::AddTranslucency(float value)
{
	return AddFloatScalar(value,TupDoubleSidedMaterial::TRANSLUCENCY);
}

TupperwareScalar *TupDoubleSidedMaterial::GetName(char **name)
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

TupperwareScalar *TupDoubleSidedMaterial::GetFrontMaterialReference(int &value)
{
	return GetIntScalar(value,TupDoubleSidedMaterial::FRONT_MATERIAL_REFERENCE);
}

TupperwareScalar *TupDoubleSidedMaterial::GetBackMaterialReference(int &value)
{
	return GetIntScalar(value,TupDoubleSidedMaterial::BACK_MATERIAL_REFERENCE);
}

TupperwareScalar *TupDoubleSidedMaterial::GetFlags(int &value)
{
	return GetIntScalar(value,TupDoubleSidedMaterial::FLAGS);
}

TupperwareScalar *TupDoubleSidedMaterial::GetTranslucency(float &value)
{
	return GetFloatScalar(value,TupDoubleSidedMaterial::TRANSLUCENCY);
}

// private methods

TupperwareScalar *TupDoubleSidedMaterial::AddFloatScalar(float value,TupDoubleSidedMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupDoubleSidedMaterial::GetFloatScalar(float &value,TupDoubleSidedMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
		{
			value = scalar->GetAsFloat();
			return scalar;
		}
	}
	value = 0;
	return NULL;
}

TupperwareScalar *TupDoubleSidedMaterial::AddIntScalar(int value,TupDoubleSidedMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupDoubleSidedMaterial::GetIntScalar(int &value,TupDoubleSidedMaterial::ChildrenAtoms index)
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

