////////////////////////////////////////////////////////////////////////////
//
// MixMap
//
// Class to handle the Mix Map (blends two materials together with a mix amount or image)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: mixmap.h $
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

#ifndef TUPPER3D_MIX_MAP_H
#define TUPPER3D_MIX_MAP_H

class TupMap;

class TupMixMap
{
	// Private Default constructor so no one can use it
	TupMixMap(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		MAP_1_REFERENCE,
		MAP_2_REFERENCE,
		MIX_MAP_REFERENCE,
		FLAGS,
		MIX_AMOUNT,
		UPPER_TRANSITION_ZONE,
		LOWER_TRANSITION_ZONE,
		OUTPUT_AMOUNT,
		OUTPUT_RGB_OFFSET,
		OUTPUT_RGB_LEVEL,
		OUTPUT_BUMP_AMOUNT,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum Flags
	{
		MAP_1_ENABLE		= 1<<0,
		MAP_2_ENABLE		= 1<<1,
		MIX_MAP_ENABLE		= 1<<2,
		USE_CURVE			= 1<<3,
		TUP_OUTPUT_INVERT					= 1<<4,
		TUP_OUTPUT_CLAMP					= 1<<5,
		TUP_OUTPUT_ALPHA_FROM_RGB_INTENSITY	= 1<<6,
		TUP_OUTPUT_ENABLE_COLOR_MAP			= 1<<7,
	};

	TupMixMap(TupperwareAggregate *pObject);
	TupMixMap(TupMap &mapBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddMap1Reference(int value);
	TupperwareScalar *AddMap2Reference(int value);
	TupperwareScalar *AddMixMapReference(int value);
	TupperwareScalar *AddFlags(int value);
	TupperwareScalar *AddMixAmount(float value);
	TupperwareScalar *AddUpperTransitionZone(float value);
	TupperwareScalar *AddLowerTransitionZone(float value);
	TupperwareScalar *AddOutputAmount(float value);
	TupperwareScalar *AddOutputRGBOffset(float value);
	TupperwareScalar *AddOutputRGBLevel(float value);
	TupperwareScalar *AddOutputBumpAmount(float value);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetMap1Reference(int &value);
	TupperwareScalar *GetMap2Reference(int &value);
	TupperwareScalar *GetMixMapReference(int &value);
	TupperwareScalar *GetFlags(int &value);
	TupperwareScalar *GetMixAmount(float &value);
	TupperwareScalar *GetUpperTransitionZone(float &value);
	TupperwareScalar *GetLowerTransitionZone(float &value);
	TupperwareScalar *GetOutputAmount(float &value);
	TupperwareScalar *GetOutputRGBOffset(float &value);
	TupperwareScalar *GetOutputRGBLevel(float &value);
	TupperwareScalar *GetOutputBumpAmount(float &value);

private:
// add methods
	TupperwareScalar *AddFloatScalar(float value,TupMixMap::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupMixMap::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupMixMap::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupMixMap::ChildrenAtoms index);
};

#endif
