////////////////////////////////////////////////////////////////////////////
//
// StandardMaterial
//
// Class to handle a standard material (has no sub materials)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: standardmaterial.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 9:39a
 * Updated in $/Avalanche/tools/Tupper3d
 * Changed from value shading types to a string.  Made the map now have a
 * mapname instead of reading from a table.  The name of the map aggregate
 * is the slot number instead.
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 11:35a
 * Updated in $/Avalanche/tools/Tupper3d
 * changed shinniness values to match max (glossiness)
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
#include "standardmaterial.h"
#include "material.h"

char *TupStandardMaterial::m_keyNames[TupStandardMaterial::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"AmbientColor", // AMBIENT_COLOR
	"DiffuseColor", // DIFFUSE_COLOR
	"SpecularColor", // SPECULAR_COLOR
	"FilterColor", // FILTER COLOR
	"Glossiness", // GLOSSINESS
	"SpecularLevel", // SPECULAR_LEVEL
	"SpecularSoften", // SPECULAR_SOFTEN
	"Transparency", // TRANSPARENCY
	"WireSize", // WIRE_SIZE
	"IndexOfRefraction", // INDEX_OF_REFRACTION
	"ReflectionDimIntensity", // REFLECTION_DIM_INTENSITY
	"ReflectionDimMultiplier", // REFLECTION_DIM_MULTIPLIER
	"Opacity", // OPACITY
	"OpacityFalloff", // OPACITY_FALLOFF
	"TransparencyType", // TRANSPARENCY_TYPE
	"ShadingTypeName", // SHADING_TYPE_NAME
	"SelfIllumination", // SELF_ILLUMINATION
	"Flags", // FLAGS
	"Maps", // MAPS
};

TupStandardMaterial::TupStandardMaterial(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupStandardMaterial::TupStandardMaterial(TupMaterial &materialBase)
{
	m_pObject = materialBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupStandardMaterial::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupStandardMaterial::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupStandardMaterial::AddAmbientColor(const float *pColor)
{
	return AddFloatList(pColor,3,TupStandardMaterial::AMBIENT_COLOR);
}

TupperwareList *TupStandardMaterial::AddDiffuseColor(const float *pColor)
{
	return AddFloatList(pColor,3,TupStandardMaterial::DIFFUSE_COLOR);
}

TupperwareList *TupStandardMaterial::AddSpecularColor(const float *pColor)
{
	return AddFloatList(pColor,3,TupStandardMaterial::SPECULAR_COLOR);
}

TupperwareList *TupStandardMaterial::AddFilterColor(const float *pColor)
{
	return AddFloatList(pColor,3,TupStandardMaterial::FILTER_COLOR);
}

TupperwareScalar *TupStandardMaterial::AddGlossiness(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::GLOSSINESS);
}

TupperwareScalar *TupStandardMaterial::AddSpecularLevel(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::SPECULAR_LEVEL);
}

TupperwareScalar *TupStandardMaterial::AddSpecularSoften(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::SPECULAR_SOFTEN);
}

TupperwareScalar *TupStandardMaterial::AddTransparency(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::TRANSPARENCY);
}

TupperwareScalar *TupStandardMaterial::AddWireSize(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::WIRE_SIZE);
}

TupperwareScalar *TupStandardMaterial::AddIndexOfRefraction(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::INDEX_OF_REFRACTION);
}

TupperwareScalar *TupStandardMaterial::AddReflectionDimIntensity(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::REFLECTION_DIM_INTENSITY);
}

TupperwareScalar *TupStandardMaterial::AddReflectionDimMultiplier(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::REFLECTION_DIM_MULTIPLIER);
}

TupperwareScalar *TupStandardMaterial::AddOpacity(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::OPACITY);
}

TupperwareScalar *TupStandardMaterial::AddOpacityFalloff(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::OPACITY_FALLOFF);
}

TupperwareScalar *TupStandardMaterial::AddTransparencyType(int value)
{
	return AddIntScalar(value,TupStandardMaterial::TRANSPARENCY_TYPE);
}

TupperwareScalar *TupStandardMaterial::AddShadingTypeName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[SHADING_TYPE_NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[SHADING_TYPE_NAME],NULL,name);
	m_cachedAtoms[SHADING_TYPE_NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupStandardMaterial::AddSelfIllumination(float value)
{
	return AddFloatScalar(value,TupStandardMaterial::SELF_ILLUMINATION);
}

TupperwareScalar *TupStandardMaterial::AddFlags(int value)
{
	return AddIntScalar(value,TupStandardMaterial::FLAGS);
}

TupperwareAggregate *TupStandardMaterial::AddMap(int channelNum)
{
	TupperwareAtom *pAtom = m_cachedAtoms[MAPS];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[MAPS] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[MAPS]);
	}
	TupperwareAggregate *pTextureMapsAgg = (TupperwareAggregate *)pAtom;
	char channelString[20];
	itoa(channelNum,channelString,10);
	TupperwareAtom *pMapAtom = pTextureMapsAgg->FindAtomByKeyAndName("Map",channelString);
	if (pMapAtom)
	{
		pTextureMapsAgg->DeleteAtom(pMapAtom);
	}
	return pTextureMapsAgg->AddAggregate("Map",channelString);

}

TupperwareScalar *TupStandardMaterial::GetName(char **name)
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

TupperwareList *TupStandardMaterial::GetAmbientColor(float **pColor)
{
	int total;
	return GetFloatList(pColor,total,TupStandardMaterial::AMBIENT_COLOR);
}

TupperwareList *TupStandardMaterial::GetDiffuseColor(float **pColor)
{
	int total;
	return GetFloatList(pColor,total,TupStandardMaterial::DIFFUSE_COLOR);
}

TupperwareList *TupStandardMaterial::GetSpecularColor(float **pColor)
{
	int total;
	return GetFloatList(pColor,total,TupStandardMaterial::SPECULAR_COLOR);
}

TupperwareList *TupStandardMaterial::GetFilterColor(float **pColor)
{
	int total;
	return GetFloatList(pColor,total,TupStandardMaterial::FILTER_COLOR);
}

TupperwareScalar *TupStandardMaterial::GetGlossiness(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::GLOSSINESS);
}

TupperwareScalar *TupStandardMaterial::GetSpecularLevel(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::SPECULAR_LEVEL);
}

TupperwareScalar *TupStandardMaterial::GetSpecularSoften(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::SPECULAR_SOFTEN);
}

TupperwareScalar *TupStandardMaterial::GetTransparency(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::TRANSPARENCY);
}

TupperwareScalar *TupStandardMaterial::GetWireSize(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::WIRE_SIZE);
}

TupperwareScalar *TupStandardMaterial::GetIndexOfRefraction(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::INDEX_OF_REFRACTION);
}

TupperwareScalar *TupStandardMaterial::GetReflectionDimIntensity(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::REFLECTION_DIM_INTENSITY);
}

TupperwareScalar *TupStandardMaterial::GetReflectionDimMultiplier(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::REFLECTION_DIM_MULTIPLIER);
}

TupperwareScalar *TupStandardMaterial::GetOpacity(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::OPACITY);
}

TupperwareScalar *TupStandardMaterial::GetOpacityFalloff(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::OPACITY_FALLOFF);
}

TupperwareScalar *TupStandardMaterial::GetTransparencyType(int &value)
{
	return GetIntScalar(value,TupStandardMaterial::TRANSPARENCY_TYPE);
}

TupperwareScalar *TupStandardMaterial::GetShadingTypeName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[SHADING_TYPE_NAME];
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

TupperwareScalar *TupStandardMaterial::GetSelfIllumination(float &value)
{
	return GetFloatScalar(value,TupStandardMaterial::SELF_ILLUMINATION);
}

TupperwareScalar *TupStandardMaterial::GetFlags(int &value)
{
	return GetIntScalar(value,TupStandardMaterial::FLAGS);
}


int TupStandardMaterial::GetNumMaps(void)
{
	int total = 0;
	TupperwareAtom *pTextureMapsAtom = m_cachedAtoms[MAPS];
	// if right type
	if (pTextureMapsAtom && pTextureMapsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pTextureMapsAgg = (TupperwareAggregate *)pTextureMapsAtom;
		// make sure it has children
		if (pTextureMapsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pTextureMapsAgg->FindByKey("Map"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pTextureAtom = i.GetAsAtom();
				if (pTextureAtom && pTextureAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					total++;
				}
			}
		}
	}
	return total;
}

TupperwareAggregate *TupStandardMaterial::GetMap(int textureIndex)
{
	int count = 0;
	TupperwareAtom *pTextureMapsAtom = m_cachedAtoms[MAPS];
	// if right type
	if (pTextureMapsAtom && pTextureMapsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pTextureMapsAgg = (TupperwareAggregate *)pTextureMapsAtom;
		// make sure it has children
		if (pTextureMapsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pTextureMapsAgg->FindByKey("Map"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pTextureAtom = i.GetAsAtom();
				if (pTextureAtom && pTextureAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					if (textureIndex == count)
						return (TupperwareAggregate *)pTextureAtom;
					count++;
				}
			}
		}
	}
	return NULL;
}

// private methods							

TupperwareScalar *TupStandardMaterial::AddFloatScalar(float value,TupStandardMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupStandardMaterial::GetFloatScalar(float &value,TupStandardMaterial::ChildrenAtoms index)
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

TupperwareScalar *TupStandardMaterial::AddIntScalar(int value,TupStandardMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupStandardMaterial::GetIntScalar(int &value,TupStandardMaterial::ChildrenAtoms index)
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


TupperwareList *TupStandardMaterial::AddFloatList(const float *pFloats,int numFloats,TupStandardMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupStandardMaterial::GetFloatList(float **pFloats,int &numFloats,TupStandardMaterial::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			*pFloats = pList->GetAsFloat();
			numFloats = pList->GetLength();
			return pList;
		}
	}
	*pFloats = NULL;
	numFloats = 0;
	return NULL;
}
