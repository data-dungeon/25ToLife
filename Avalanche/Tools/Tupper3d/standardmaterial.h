////////////////////////////////////////////////////////////////////////////
//
// StandardMaterial
//
// Class to handle a standard material (has no sub materials)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: standardmaterial.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 9:39a
 * Updated in $/Avalanche/tools/Tupper3d
 * Changed from value shading types to a string.  Made the map now have a
 * mapname instead of reading from a table.  The name of the map aggregate
 * is the slot number instead.
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 11:35a
 * Updated in $/Avalanche/tools/Tupper3d
 * changed shinniness values to match max (glossiness)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
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

#ifndef TUPPER3D_STANDARD_MATERIAL_H
#define TUPPER3D_STANDARD_MATERIAL_H

class TupMaterial;

class TupStandardMaterial
{
	// Private Default constructor so no one can use it
	TupStandardMaterial(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		AMBIENT_COLOR,
		DIFFUSE_COLOR,
		SPECULAR_COLOR,
		FILTER_COLOR,
		GLOSSINESS,
		SPECULAR_LEVEL,
		SPECULAR_SOFTEN,
		TRANSPARENCY,
		WIRE_SIZE,
		INDEX_OF_REFRACTION,
		REFLECTION_DIM_INTENSITY,
		REFLECTION_DIM_MULTIPLIER,
		OPACITY,
		OPACITY_FALLOFF,
		TRANSPARENCY_TYPE,
		SHADING_TYPE_NAME,
		SELF_ILLUMINATION,
		FLAGS,
		MAPS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum TransparencyType
	{
		TRANS_SUBTRACTIVE,
		TRANS_ADDITIVE,
		TRANS_FILTER,
	};

	enum MaterialFlags
	{
		FLAGS_SOFTEN						= 1<<0,
		FLAGS_FACE_MAPPING					= 1<<1,
		FLAGS_TWO_SIDED						= 1<<2,
		FLAGS_WIRE							= 1<<3,
		FLAGS_WIRE_UNITS					= 1<<4,
		FLAGS_FALLOFF_OUT					= 1<<5,
		FLAGS_AMBIENT_DIFFUSE_TEXTURE_LOCK	= 1<<6,
		FLAGS_AMBIENT_DIFFUSE_COLOR_LOCK	= 1<<7,
		FLAGS_DIFFUSE_SPECULAR_COLOR_LOCK	= 1<<8,
		FLAGS_SELF_ILLUMINATION_COLOR		= 1<<9,
		REFLECTION_DIMMING_APPLY			= 1<<10,
	};

	TupStandardMaterial(TupperwareAggregate *pObject);
	TupStandardMaterial(TupMaterial &materialBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareList *AddAmbientColor(const float *pColor);
	TupperwareList *AddDiffuseColor(const float *pColor);
	TupperwareList *AddSpecularColor(const float *pColor);
	TupperwareList *AddFilterColor(const float *pColor);
	TupperwareScalar *AddGlossiness(float value);
	TupperwareScalar *AddSpecularLevel(float value);
	TupperwareScalar *AddSpecularSoften(float value);
	TupperwareScalar *AddTransparency(float value);
	TupperwareScalar *AddWireSize(float value);
	TupperwareScalar *AddIndexOfRefraction(float value);
	TupperwareScalar *AddReflectionDimIntensity(float value);
	TupperwareScalar *AddReflectionDimMultiplier(float value);
	TupperwareScalar *AddOpacity(float value);
	TupperwareScalar *AddOpacityFalloff(float value);
	TupperwareScalar *AddTransparencyType(int value);
	TupperwareScalar *AddShadingTypeName(const char *name);
	TupperwareScalar *AddSelfIllumination(float value);
	TupperwareScalar *AddFlags(int value);
	TupperwareAggregate *AddMap(int channelNum);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareList *GetAmbientColor(float **pColor);
	TupperwareList *GetDiffuseColor(float **pColor);
	TupperwareList *GetSpecularColor(float **pColor);
	TupperwareList *GetFilterColor(float **pColor);
	TupperwareScalar *GetGlossiness(float &value);
	TupperwareScalar *GetSpecularLevel(float &value);
	TupperwareScalar *GetSpecularSoften(float &value);
	TupperwareScalar *GetTransparency(float &value);
	TupperwareScalar *GetWireSize(float &value);
	TupperwareScalar *GetIndexOfRefraction(float &value);
	TupperwareScalar *GetReflectionDimIntensity(float &value);
	TupperwareScalar *GetReflectionDimMultiplier(float &value);
	TupperwareScalar *GetOpacity(float &value);
	TupperwareScalar *GetOpacityFalloff(float &value);
	TupperwareScalar *GetTransparencyType(int &value);
	TupperwareScalar *GetShadingTypeName(char **name);
	TupperwareScalar *GetSelfIllumination(float &value);
	TupperwareScalar *GetFlags(int &value);
	int GetNumMaps(void);
	TupperwareAggregate *GetMap(int index);

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupStandardMaterial::ChildrenAtoms index);
	TupperwareScalar *AddFloatScalar(float value,TupStandardMaterial::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupStandardMaterial::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupStandardMaterial::ChildrenAtoms index);
	TupperwareScalar *GetFloatScalar(float &value,TupStandardMaterial::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupStandardMaterial::ChildrenAtoms index);
};

#endif
