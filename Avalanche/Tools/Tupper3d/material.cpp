////////////////////////////////////////////////////////////////////////////
//
// Material
//
// Class to handle the material types
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: material.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/30/02    Time: 9:45a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added material name access to TupMaterial
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
#include "material.h"

char *TupMaterial::m_typeNames[TupMaterial::LAST_ENTRY] = 
{
	"Standard", // STANDARD_MATERIAL
	"MultiSubObject", // MULTI_SUB_OBJECT_MATERIAL
	"Blend", // BLEND_MATERIAL
	"Composite", // COMPOSITE_MATERIAL
	"DoubleSided", // DOUBLE_SIDED_MATERIAL
	"Unknown", // UNKNOWN
};

char *TupMaterial::m_keyNames[TupMaterial::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"MaterialType", // MATERIAL_TYPE
};


TupMaterial::TupMaterial(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupMaterial::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupMaterial::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

void TupMaterial::SetMaterialType(TupMaterial::MaterialTypes type)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MATERIAL_TYPE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	m_pObject->AddScalarString(m_keyNames[MATERIAL_TYPE],NULL,m_typeNames[type]);
}

TupperwareAggregate *TupMaterial::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareScalar *TupMaterial::GetName(char **name)
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

TupMaterial::MaterialTypes TupMaterial::GetMaterialType(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MATERIAL_TYPE];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			for (int i=0;i<TupMaterial::UNKNOWN;i++)
			{
				if (!strcmp(m_typeNames[i],pScalar->GetAsString()))
				{
					return (TupMaterial::MaterialTypes)i;
				}
			}
		}
	}
	return TupMaterial::UNKNOWN;
}
