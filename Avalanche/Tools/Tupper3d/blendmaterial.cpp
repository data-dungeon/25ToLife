////////////////////////////////////////////////////////////////////////////
//
// BlendMaterial
//
// Class to handle the blend material 
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: blendmaterial.cpp $
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
 * User: Adam Clayton Date: 12/13/01   Time: 10:55a
 * Updated in $/Avalanche/tools/Tupper3d
 * More Materials
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "blendmaterial.h"
#include "material.h"

char *TupBlendMaterial::m_keyNames[TupBlendMaterial::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Material1Reference", // MATERIAL_1_REFERENCE
	"Material2Reference", // MATERIAL_2_REFERENCE
	"MaskReference", // MASK_REFERENCE
	"Flags", // FLAGS
	"MixAmount", // MIX_AMOUNT
	"UpperTransitionZone", // UPPER_TRANSITION_ZONE
	"LowerTransitionZone", // LOWER_TRANSITION_ZONE
};

TupBlendMaterial::TupBlendMaterial(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupBlendMaterial::TupBlendMaterial(TupMaterial &materialBase)
{
	m_pObject = materialBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupBlendMaterial::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupBlendMaterial::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBlendMaterial::AddMaterial1Reference(int value)
{
	return AddIntScalar(value,TupBlendMaterial::MATERIAL_1_REFERENCE);
}

TupperwareScalar *TupBlendMaterial::AddMaterial2Reference(int value)
{
	return AddIntScalar(value,TupBlendMaterial::MATERIAL_2_REFERENCE);
}

TupperwareScalar *TupBlendMaterial::AddMaskReference(int value)
{
	return AddIntScalar(value,TupBlendMaterial::MASK_REFERENCE);
}

TupperwareScalar *TupBlendMaterial::AddFlags(int value)
{
	return AddIntScalar(value,TupBlendMaterial::FLAGS);
}

TupperwareScalar *TupBlendMaterial::AddMixAmount(float value)
{
	return AddFloatScalar(value,TupBlendMaterial::MIX_AMOUNT);
}

TupperwareScalar *TupBlendMaterial::AddUpperTransitionZone(float value)
{
	return AddFloatScalar(value,TupBlendMaterial::UPPER_TRANSITION_ZONE);
}

TupperwareScalar *TupBlendMaterial::AddLowerTransitionZone(float value)
{
	return AddFloatScalar(value,TupBlendMaterial::LOWER_TRANSITION_ZONE);
}

TupperwareScalar *TupBlendMaterial::GetName(char **name)
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

TupperwareScalar *TupBlendMaterial::GetMaterial1Reference(int &value)
{
	return GetIntScalar(value,TupBlendMaterial::MATERIAL_1_REFERENCE);
}

TupperwareScalar *TupBlendMaterial::GetMaterial2Reference(int &value)
{
	return GetIntScalar(value,TupBlendMaterial::MATERIAL_2_REFERENCE);
}

TupperwareScalar *TupBlendMaterial::GetMaskReference(int &value)
{
	return GetIntScalar(value,TupBlendMaterial::MASK_REFERENCE);
}

TupperwareScalar *TupBlendMaterial::GetFlags(int &value)
{
	return GetIntScalar(value,TupBlendMaterial::FLAGS);
}

TupperwareScalar *TupBlendMaterial::GetMixAmount(float &value)
{
	return GetFloatScalar(value,TupBlendMaterial::MIX_AMOUNT);
}

TupperwareScalar *TupBlendMaterial::GetUpperTransitionZone(float &value)
{
	return GetFloatScalar(value,TupBlendMaterial::UPPER_TRANSITION_ZONE);
}

TupperwareScalar *TupBlendMaterial::GetLowerTransitionZone(float &value)
{
	return GetFloatScalar(value,TupBlendMaterial::LOWER_TRANSITION_ZONE);
}

// private methods

TupperwareScalar *TupBlendMaterial::AddFloatScalar(float value,TupBlendMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBlendMaterial::GetFloatScalar(float &value,TupBlendMaterial::ChildrenAtoms index)
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

TupperwareScalar *TupBlendMaterial::AddIntScalar(int value,TupBlendMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBlendMaterial::GetIntScalar(int &value,TupBlendMaterial::ChildrenAtoms index)
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

