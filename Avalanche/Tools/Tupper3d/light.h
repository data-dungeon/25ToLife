////////////////////////////////////////////////////////////////////////////
//
// Light
//
// Class for lights
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: light.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 3:18p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 8  *****************
 * User: Dwain Skinner Date: 3/05/03    Time: 9:19a
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/06/03    Time: 4:07p
 * Updated in $/Avalanche/tools/Tupper3d
 * export reflection maps
 * export lights
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

#ifndef TUPPER3D_LIGHT_H
#define TUPPER3D_LIGHT_H

class TupObject;

class TupLight
{
	// Private Default constructor so no one can use it
	TupLight(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		LIGHT_TYPE,
		LIGHT_COLOR,
		MULTIPLIER,
		AFFECT_SURFACES_CONTRAST,
		AFFECT_SURFACES_SOFTEN_DIFF_EDGE,
		PROJECTOR_MAP_REFERENCE,
		LIGHT_CONE_HOTSPOT,
		LIGHT_CONE_FALLOFF,
		LIGHT_CONE_RECTANGLE_ASPECT_RATIO,
		TARGET_DISTANCE,
		NEAR_ATTENUATION_START,
		NEAR_ATTENUATION_END,
		FAR_ATTENUATION_START,
		FAR_ATTENUATION_END,
		ATTENUATION_DECAY_TYPE,
		ATTENUATION_DECAY_START,
		SHADOW_TYPE,
		SHADOW_COLOR,
		SHADOW_DENSITY,
		SHADOW_MAP_REFERENCE,
		ATMOSPHERE_SHADOWS_OPACITY,
		ATMOSPHERE_SHADOWS_COLOR_AMOUNT,
		SHADOW_MAP_BIAS,
		SHADOW_MAP_SIZE,
		SHADOW_MAP_SAMPLE_RANGE,
		RAY_TRACE_BIAS,
		RAY_TRACE_MAX_DEPTH,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum LightTypes
	{
		LIGHT_OMNI,
		LIGHT_TARGET_SPOT,
		LIGHT_FREE_SPOT,
		LIGHT_TARGET_DIRECT,
		LIGHT_FREE_DIRECT,
		LIGHT_AMBIENT,
		LIGHT_AREA,
		TOTAL_LIGHT_TYPES,
	};

	static char *LightNames[TOTAL_LIGHT_TYPES];

	enum LightFlags
	{
		FLAGS_ENABLE													= 1<<0,
		FLAGS_CAST_SHADOWS											= 1<<1,
		FLAGS_AFFECT_SURFACES_DIFFUSE								= 1<<2,
		FLAGS_AFFECT_SURFACES_SPECULAR							= 1<<3,
		FLAGS_AFFECT_SURFACES_AMBIENT_ONLY						= 1<<4,
		FLAGS_SHOW_LIGHT_CONE										= 1<<5,
		FLAGS_LIGHT_CONE_OVERSHOOT									= 1<<6,
		FLAGS_LIGHT_CONE_CIRCLE										= 1<<7,
		FLAGS_PROJECTOR_MAP_ENABLE									= 1<<8,
		FLAGS_NEAR_ATTENUATION_USE									= 1<<9,
		FLAGS_NEAR_ATTENUATION_SHOW								= 1<<10,
		FLAGS_FAR_ATTENUATION_USE									= 1<<11,
		FLAGS_FAR_ATTENUATION_SHOW									= 1<<12,
		FLAGS_ATTENUATION_DECAY_SHOW								= 1<<13,
		FLAGS_OBJECT_SHADOWS_USE_GLOBAL_SETTINGS				= 1<<14,
		FLAGS_OBJECT_SHADOWS_MAP_ENABLE							= 1<<15,
		FLAGS_OBJECT_SHADOWS_LIGHT_AFFECTS_SHADOW_COLOR		= 1<<16,
		FLAGS_ATMOSPHERE_SHADOWS_ON								= 1<<17,
		FLAGS_SHADOW_MAP_ABSOLUTE_MAP_BIAS						= 1<<18,
	};

	enum AttenuationDecayType
	{
		ATTENUATION_DECAY_NONE,
		ATTENUATION_DECAY_INVERSE,
		ATTENUATION_DECAY_INVERSE_SQUARED,
	};

	enum ShadowType
	{
		SHADOW_TYPE_SHADOW_MAP,
		SHADOW_TYPE_RAY_TRACED,
	};


	TupLight(TupperwareAggregate *pObject);
	TupLight(TupObject &objectBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddLightType(int value);
	TupperwareList *AddLightColor(const float *value);
	TupperwareScalar *AddMultiplier(float value);
	TupperwareScalar *AddAffectSurfacesContrast(float value);
	TupperwareScalar *AddAffectSurfacesSoftenDiffEdge(float value);
	TupperwareScalar *AddProjectorMapReference(int value);
	TupperwareScalar *AddLightConeHotspot(float value);
	TupperwareScalar *AddLightConeFalloff(float value);
	TupperwareScalar *AddLightConeRectangleAspectRatio(float value);
	TupperwareScalar *AddTargetDistance(float value);
	TupperwareScalar *AddNearAttenuationStart(float value);
	TupperwareScalar *AddNearAttenuationEnd(float value);
	TupperwareScalar *AddFarAttenuationStart(float value);
	TupperwareScalar *AddFarAttenuationEnd(float value);
	TupperwareScalar *AddAttenuationDecayType(int value);
	TupperwareScalar *AddAttenuationDecayStart(float value);
	TupperwareScalar *AddShadowType(int value);
	TupperwareList *AddShadowColor(const float *value);
	TupperwareScalar *AddShadowDensity(float value);
	TupperwareScalar *AddShadowMapReference(int value);
	TupperwareScalar *AddAtmosphereShadowsOpacity(float value);
	TupperwareScalar *AddAtmosphereShadowsColorAmount(float value);
	TupperwareScalar *AddShadowMapBias(float value);
	TupperwareScalar *AddShadowMapSize(int value);
	TupperwareScalar *AddShadowMapSampleRange(float value);
	TupperwareScalar *AddRayTraceBias(float value);
	TupperwareScalar *AddRayMaxDepth(int value);
	TupperwareScalar *AddFlags(int value);

// get methods
	TupperwareScalar *GetLightType(int &value);
	TupperwareList *GetLightColor(float **value);
	TupperwareScalar *GetMultiplier(float &value);
	TupperwareScalar *GetAffectSurfacesContrast(float &value);
	TupperwareScalar *GetAffectSurfacesSoftenDiffEdge(float &value);
	TupperwareScalar *GetProjectorMapReference(int &value);
	TupperwareScalar *GetLightConeHotspot(float &value);
	TupperwareScalar *GetLightConeFalloff(float &value);
	TupperwareScalar *GetLightConeRectangleAspectRatio(float &value);
	TupperwareScalar *GetTargetDistance(float &value);
	TupperwareScalar *GetNearAttenuationStart(float &value);
	TupperwareScalar *GetNearAttenuationEnd(float &value);
	TupperwareScalar *GetFarAttenuationStart(float &value);
	TupperwareScalar *GetFarAttenuationEnd(float &value);
	TupperwareScalar *GetAttenuationDecayType(int &value);
	TupperwareScalar *GetAttenuationDecayStart(float &value);
	TupperwareScalar *GetShadowType(int &value);
	TupperwareList *GetShadowColor(float **value);
	TupperwareScalar *GetShadowDensity(float &value);
	TupperwareScalar *GetShadowMapReference(int &value);
	TupperwareScalar *GetAtmosphereShadowsOpacity(float &value);
	TupperwareScalar *GetAtmosphereShadowsColorAmount(float &value);
	TupperwareScalar *GetShadowMapBias(float &value);
	TupperwareScalar *GetShadowMapSize(int &value);
	TupperwareScalar *GetShadowMapSampleRange(float &value);
	TupperwareScalar *GetRayTraceBias(float &value);
	TupperwareScalar *GetRayMaxDepth(int &value);
	TupperwareScalar *GetFlags(int &value);
	const char *GetLightTypeName( LightTypes type );

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupLight::ChildrenAtoms index);
	TupperwareScalar *AddFloatScalar(float value,TupLight::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupLight::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupLight::ChildrenAtoms index);
	TupperwareScalar *GetFloatScalar(float &value,TupLight::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupLight::ChildrenAtoms index);
};

#endif
