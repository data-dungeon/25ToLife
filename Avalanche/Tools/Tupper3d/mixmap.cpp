////////////////////////////////////////////////////////////////////////////
//
// MixMap
//
// Class to handle the Mix Map (blends two materials together with a mix amount or image)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: mixmap.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/13/02    Time: 3:21p
 * Updated in $/Avalanche/tools/Tupper3d
 * more options
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/13/02    Time: 1:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Mix Map
 */

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "mixmap.h"
#include "map.h"

char *TupMixMap::m_keyNames[TupMixMap::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Map1Reference", // MAP_1_REFERENCE
	"Map2Reference", // MAP_2_REFERENCE
	"MixMapReference", // MIX_MAP_REFERENCE
	"Flags", // FLAGS
	"MixAmount", // MIX_AMOUNT
	"UpperTransitionZone", // UPPER_TRANSITION_ZONE
	"LowerTransitionZone", // LOWER_TRANSITION_ZONE
	"OutputAmount", // OUTPUT_AMOUNT
	"OutputRGBOffset", // OUTPUT_RGB_OFFSET
	"OutputRGBLevel", // OUTPUT_RGB_LEVEL
	"OutputBumpAmount", // OUTPUT_BUMP_AMOUNT
};

TupMixMap::TupMixMap(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupMixMap::TupMixMap(TupMap &mapBase)
{
	m_pObject = mapBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupMixMap::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupMixMap::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupMixMap::AddMap1Reference(int value)
{
	return AddIntScalar(value,TupMixMap::MAP_1_REFERENCE);
}

TupperwareScalar *TupMixMap::AddMap2Reference(int value)
{
	return AddIntScalar(value,TupMixMap::MAP_2_REFERENCE);
}

TupperwareScalar *TupMixMap::AddMixMapReference(int value)
{
	return AddIntScalar(value,TupMixMap::MIX_MAP_REFERENCE);
}

TupperwareScalar *TupMixMap::AddFlags(int value)
{
	return AddIntScalar(value,TupMixMap::FLAGS);
}

TupperwareScalar *TupMixMap::AddMixAmount(float value)
{
	return AddFloatScalar(value,TupMixMap::MIX_AMOUNT);
}

TupperwareScalar *TupMixMap::AddUpperTransitionZone(float value)
{
	return AddFloatScalar(value,TupMixMap::UPPER_TRANSITION_ZONE);
}

TupperwareScalar *TupMixMap::AddLowerTransitionZone(float value)
{
	return AddFloatScalar(value,TupMixMap::LOWER_TRANSITION_ZONE);
}

TupperwareScalar *TupMixMap::AddOutputAmount(float value)
{
	return AddFloatScalar(value,TupMixMap::OUTPUT_AMOUNT);
}

TupperwareScalar *TupMixMap::AddOutputRGBOffset(float value)
{
	return AddFloatScalar(value,TupMixMap::OUTPUT_RGB_OFFSET);
}

TupperwareScalar *TupMixMap::AddOutputRGBLevel(float value)
{
	return AddFloatScalar(value,TupMixMap::OUTPUT_RGB_LEVEL);
}

TupperwareScalar *TupMixMap::AddOutputBumpAmount(float value)
{
	return AddFloatScalar(value,TupMixMap::OUTPUT_BUMP_AMOUNT);
}

TupperwareScalar *TupMixMap::GetName(char **name)
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

TupperwareScalar *TupMixMap::GetMap1Reference(int &value)
{
	return GetIntScalar(value,TupMixMap::MAP_1_REFERENCE);
}

TupperwareScalar *TupMixMap::GetMap2Reference(int &value)
{
	return GetIntScalar(value,TupMixMap::MAP_2_REFERENCE);
}

TupperwareScalar *TupMixMap::GetMixMapReference(int &value)
{
	return GetIntScalar(value,TupMixMap::MIX_MAP_REFERENCE);
}

TupperwareScalar *TupMixMap::GetFlags(int &value)
{
	return GetIntScalar(value,TupMixMap::FLAGS);
}

TupperwareScalar *TupMixMap::GetMixAmount(float &value)
{
	return GetFloatScalar(value,TupMixMap::MIX_AMOUNT);
}

TupperwareScalar *TupMixMap::GetUpperTransitionZone(float &value)
{
	return GetFloatScalar(value,TupMixMap::UPPER_TRANSITION_ZONE);
}

TupperwareScalar *TupMixMap::GetLowerTransitionZone(float &value)
{
	return GetFloatScalar(value,TupMixMap::LOWER_TRANSITION_ZONE);
}

TupperwareScalar *TupMixMap::GetOutputAmount(float &value)
{
	return GetFloatScalar(value,TupMixMap::OUTPUT_AMOUNT);
}

TupperwareScalar *TupMixMap::GetOutputRGBOffset(float &value)
{
	return GetFloatScalar(value,TupMixMap::OUTPUT_RGB_OFFSET);
}

TupperwareScalar *TupMixMap::GetOutputRGBLevel(float &value)
{
	return GetFloatScalar(value,TupMixMap::OUTPUT_RGB_LEVEL);
}

TupperwareScalar *TupMixMap::GetOutputBumpAmount(float &value)
{
	return GetFloatScalar(value,TupMixMap::OUTPUT_BUMP_AMOUNT);
}

// private methods

TupperwareScalar *TupMixMap::AddFloatScalar(float value,TupMixMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupMixMap::GetFloatScalar(float &value,TupMixMap::ChildrenAtoms index)
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

TupperwareScalar *TupMixMap::AddIntScalar(int value,TupMixMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupMixMap::GetIntScalar(int &value,TupMixMap::ChildrenAtoms index)
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

