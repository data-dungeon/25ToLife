////////////////////////////////////////////////////////////////////////////
//
// Light
//
// Class for lights
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: light.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 3:18p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 7  *****************
 * User: Dwain Skinner Date: 3/05/03    Time: 9:19a
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/21/01   Time: 8:37a
 * Updated in $/Avalanche/tools/Tupper3d
 * Lights
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/19/01   Time: 2:10p
 * Updated in $/Avalanche/tools/Tupper3d
 * Lights & Cameras
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "light.h"
#include "object.h"

char *TupLight::LightNames[ TOTAL_LIGHT_TYPES ] = 
{	"Omni", "Target Spot", "Free Spot", "Target Direct", "Free Direct", "Ambient", "Area" };


char *TupLight::m_keyNames[TupLight::NUM_CHILDREN_ATOMS] = 
{
	"LightType", // LIGHT_TYPE
	"LightColor", // LIGHT_COLOR
	"Multiplier", // MULTIPLIER
	"AffectSurfacesContrast", // AFFECT_SURFACES_CONTRAST
	"AffectSurfacesSoftenDiffEdge", // AFFECT_SURFACES_SOFTEN_DIFF_EDGE
	"ProjectorMapReference", // PROJECTOR_MAP_REFERENCE
	"LightConeHotspot", // LIGHT_CONE_HOTSPOT
	"LightConeFalloff", // LIGHT_CONE_FALLOFF
	"LightConeRectangleAspectRatio", // LIGHT_CONE_RECTANGLE_ASPECT_RATIO
	"TargetDistance", // TARGET_DISTANCE
	"NearAttenuationStart", // NEAR_ATTENUATION_START
	"NearAttenuationEnd", // NEAR_ATTENUATION_END
	"FarAttenuationStart", // FAR_ATTENUATION_START
	"FarAttenuationEnd", // FAR_ATTENUATION_END
	"AttenuationDecayType", // ATTENUATION_DECAY_TYPE
	"AttenuationDecayStart", // ATTENUATION_DECAY_START
	"ShadowType", // SHADOW_TYPE
	"ShadowColor", // SHADOW_COLOR
	"ShadowDensity", // SHADOW_DENSITY
	"ShadowMapReference", // SHADOW_MAP_REFERENCE
	"AtmosphereShadowsOpacity", // ATMOSPHERE_SHADOWS_OPACITY
	"AtmosphereShadowsColorAmount", // ATMOSPHERE_SHADOWS_COLOR_AMOUNT
	"ShadowMapBias", // SHADOW_MAP_BIAS
	"ShadowMapSize", // SHADOW_MAP_SIZE
	"ShadowMapSampleRange", // SHADOW_MAP_SAMPLE_RANGE
	"RayTraceBias", // RAY_TRACE_BIAS
	"RayTraceMaxDepth", // RAY_TRACE_MAX_DEPTH
	"Flags", // FLAGS
};

const char *TupLight::GetLightTypeName( LightTypes type )
{
	return( LightNames[ type ] );
}


TupLight::TupLight(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupLight::TupLight(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}


void TupLight::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupLight::AddLightType(int value)
{
	return AddIntScalar(value,TupLight::LIGHT_TYPE);
}

TupperwareList *TupLight::AddLightColor(const float *value)
{
	return AddFloatList(value,3,TupLight::LIGHT_COLOR);
}

TupperwareScalar *TupLight::AddMultiplier(float value)
{
	return AddFloatScalar(value,TupLight::MULTIPLIER);
}

TupperwareScalar *TupLight::AddAffectSurfacesContrast(float value)
{
	return AddFloatScalar(value,TupLight::AFFECT_SURFACES_CONTRAST);
}

TupperwareScalar *TupLight::AddAffectSurfacesSoftenDiffEdge(float value)
{
	return AddFloatScalar(value,TupLight::AFFECT_SURFACES_SOFTEN_DIFF_EDGE);
}

TupperwareScalar *TupLight::AddProjectorMapReference(int value)
{
	return AddIntScalar(value,TupLight::PROJECTOR_MAP_REFERENCE);
}

TupperwareScalar *TupLight::AddLightConeHotspot(float value)
{
	return AddFloatScalar(value,TupLight::LIGHT_CONE_HOTSPOT);
}

TupperwareScalar *TupLight::AddLightConeFalloff(float value)
{
	return AddFloatScalar(value,TupLight::LIGHT_CONE_FALLOFF);
}

TupperwareScalar *TupLight::AddLightConeRectangleAspectRatio(float value)
{
	return AddFloatScalar(value,TupLight::LIGHT_CONE_RECTANGLE_ASPECT_RATIO);
}

TupperwareScalar *TupLight::AddTargetDistance(float value)
{
	return AddFloatScalar(value,TupLight::TARGET_DISTANCE);
}

TupperwareScalar *TupLight::AddNearAttenuationStart(float value)
{
	return AddFloatScalar(value,TupLight::NEAR_ATTENUATION_START);
}

TupperwareScalar *TupLight::AddNearAttenuationEnd(float value)
{
	return AddFloatScalar(value,TupLight::NEAR_ATTENUATION_END);
}

TupperwareScalar *TupLight::AddFarAttenuationStart(float value)
{
	return AddFloatScalar(value,TupLight::FAR_ATTENUATION_START);
}

TupperwareScalar *TupLight::AddFarAttenuationEnd(float value)
{
	return AddFloatScalar(value,TupLight::FAR_ATTENUATION_END);
}

TupperwareScalar *TupLight::AddAttenuationDecayType(int value)
{
	return AddIntScalar(value,TupLight::ATTENUATION_DECAY_TYPE);
}

TupperwareScalar *TupLight::AddAttenuationDecayStart(float value)
{
	return AddFloatScalar(value,TupLight::ATTENUATION_DECAY_START);
}

TupperwareScalar *TupLight::AddShadowType(int value)
{
	return AddIntScalar(value,TupLight::SHADOW_TYPE);
}

TupperwareList *TupLight::AddShadowColor(const float *value)
{
	return AddFloatList(value,3,TupLight::SHADOW_COLOR);
}

TupperwareScalar *TupLight::AddShadowDensity(float value)
{
	return AddFloatScalar(value,TupLight::SHADOW_DENSITY);
}

TupperwareScalar *TupLight::AddShadowMapReference(int value)
{
	return AddIntScalar(value,TupLight::SHADOW_MAP_REFERENCE);
}

TupperwareScalar *TupLight::AddAtmosphereShadowsOpacity(float value)
{
	return AddFloatScalar(value,TupLight::ATMOSPHERE_SHADOWS_OPACITY);
}

TupperwareScalar *TupLight::AddAtmosphereShadowsColorAmount(float value)
{
	return AddFloatScalar(value,TupLight::ATMOSPHERE_SHADOWS_COLOR_AMOUNT);
}

TupperwareScalar *TupLight::AddShadowMapBias(float value)
{
	return AddFloatScalar(value,TupLight::SHADOW_MAP_BIAS);
}

TupperwareScalar *TupLight::AddShadowMapSize(int value)
{
	return AddIntScalar(value,TupLight::SHADOW_MAP_SIZE);
}

TupperwareScalar *TupLight::AddShadowMapSampleRange(float value)
{
	return AddFloatScalar(value,TupLight::SHADOW_MAP_SAMPLE_RANGE);
}

TupperwareScalar *TupLight::AddRayTraceBias(float value)
{
	return AddFloatScalar(value,TupLight::RAY_TRACE_BIAS);
}

TupperwareScalar *TupLight::AddRayMaxDepth(int value)
{
	return AddIntScalar(value,TupLight::RAY_TRACE_MAX_DEPTH);
}

TupperwareScalar *TupLight::AddFlags(int value)
{
	return AddIntScalar(value,TupLight::FLAGS);
}

// get methods

TupperwareScalar *TupLight::GetLightType(int &value)
{
	return GetIntScalar(value,TupLight::LIGHT_TYPE);
}

TupperwareList *TupLight::GetLightColor(float **value)
{
	int num;
	return GetFloatList(value,num,TupLight::LIGHT_COLOR);
}

TupperwareScalar *TupLight::GetMultiplier(float &value)
{
	return GetFloatScalar(value,TupLight::MULTIPLIER);
}

TupperwareScalar *TupLight::GetAffectSurfacesContrast(float &value)
{
	return GetFloatScalar(value,TupLight::AFFECT_SURFACES_CONTRAST);
}

TupperwareScalar *TupLight::GetAffectSurfacesSoftenDiffEdge(float &value)
{
	return GetFloatScalar(value,TupLight::AFFECT_SURFACES_SOFTEN_DIFF_EDGE);
}

TupperwareScalar *TupLight::GetProjectorMapReference(int &value)
{
	return GetIntScalar(value,TupLight::PROJECTOR_MAP_REFERENCE);
}

TupperwareScalar *TupLight::GetLightConeHotspot(float &value)
{
	return GetFloatScalar(value,TupLight::LIGHT_CONE_HOTSPOT);
}

TupperwareScalar *TupLight::GetLightConeFalloff(float &value)
{
	return GetFloatScalar(value,TupLight::LIGHT_CONE_FALLOFF);
}

TupperwareScalar *TupLight::GetLightConeRectangleAspectRatio(float &value)
{
	return GetFloatScalar(value,TupLight::LIGHT_CONE_RECTANGLE_ASPECT_RATIO);
}

TupperwareScalar *TupLight::GetTargetDistance(float &value)
{
	return GetFloatScalar(value,TupLight::TARGET_DISTANCE);
}

TupperwareScalar *TupLight::GetNearAttenuationStart(float &value)
{
	return GetFloatScalar(value,TupLight::NEAR_ATTENUATION_START);
}

TupperwareScalar *TupLight::GetNearAttenuationEnd(float &value)
{
	return GetFloatScalar(value,TupLight::NEAR_ATTENUATION_END);
}

TupperwareScalar *TupLight::GetFarAttenuationStart(float &value)
{
	return GetFloatScalar(value,TupLight::FAR_ATTENUATION_START);
}

TupperwareScalar *TupLight::GetFarAttenuationEnd(float &value)
{
	return GetFloatScalar(value,TupLight::FAR_ATTENUATION_END);
}

TupperwareScalar *TupLight::GetAttenuationDecayType(int &value)
{
	return GetIntScalar(value,TupLight::ATTENUATION_DECAY_TYPE);
}

TupperwareScalar *TupLight::GetAttenuationDecayStart(float &value)
{
	return GetFloatScalar(value,TupLight::ATTENUATION_DECAY_START);
}

TupperwareScalar *TupLight::GetShadowType(int &value)
{
	return GetIntScalar(value,TupLight::SHADOW_TYPE);
}

TupperwareList *TupLight::GetShadowColor(float **value)
{
	int num;
	return GetFloatList(value,num,TupLight::SHADOW_COLOR);
}

TupperwareScalar *TupLight::GetShadowDensity(float &value)
{
	return GetFloatScalar(value,TupLight::SHADOW_DENSITY);
}

TupperwareScalar *TupLight::GetShadowMapReference(int &value)
{
	return GetIntScalar(value,TupLight::SHADOW_MAP_REFERENCE);
}

TupperwareScalar *TupLight::GetAtmosphereShadowsOpacity(float &value)
{
	return GetFloatScalar(value,TupLight::ATMOSPHERE_SHADOWS_OPACITY);
}

TupperwareScalar *TupLight::GetAtmosphereShadowsColorAmount(float &value)
{
	return GetFloatScalar(value,TupLight::ATMOSPHERE_SHADOWS_COLOR_AMOUNT);
}

TupperwareScalar *TupLight::GetShadowMapBias(float &value)
{
	return GetFloatScalar(value,TupLight::SHADOW_MAP_BIAS);
}

TupperwareScalar *TupLight::GetShadowMapSize(int &value)
{
	return GetIntScalar(value,TupLight::SHADOW_MAP_SIZE);
}

TupperwareScalar *TupLight::GetShadowMapSampleRange(float &value)
{
	return GetFloatScalar(value,TupLight::SHADOW_MAP_SAMPLE_RANGE);
}

TupperwareScalar *TupLight::GetRayTraceBias(float &value)
{
	return GetFloatScalar(value,TupLight::RAY_TRACE_BIAS);
}

TupperwareScalar *TupLight::GetRayMaxDepth(int &value)
{
	return GetIntScalar(value,TupLight::RAY_TRACE_MAX_DEPTH);
}

TupperwareScalar *TupLight::GetFlags(int &value)
{
	return GetIntScalar(value,TupLight::FLAGS);
}


// private methods							

TupperwareScalar *TupLight::AddFloatScalar(float value,TupLight::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupLight::GetFloatScalar(float &value,TupLight::ChildrenAtoms index)
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

TupperwareScalar *TupLight::AddIntScalar(int value,TupLight::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupLight::GetIntScalar(int &value,TupLight::ChildrenAtoms index)
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


TupperwareList *TupLight::AddFloatList(const float *pFloats,int numFloats,TupLight::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupLight::GetFloatList(float **pFloats,int &numFloats,TupLight::ChildrenAtoms index)
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
