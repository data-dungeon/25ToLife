////////////////////////////////////////////////////////////////////////////
//
// BlendMaterial
//
// Class to handle the blend material (blends two materials together with a mask)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: blendmaterial.h $
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

#ifndef TUPPER3D_BLEND_MATERIAL_H
#define TUPPER3D_BLEND_MATERIAL_H

class TupMaterial;

class TupBlendMaterial
{
	// Private Default constructor so no one can use it
	TupBlendMaterial(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		MATERIAL_1_REFERENCE,
		MATERIAL_2_REFERENCE,
		MASK_REFERENCE,
		FLAGS,
		MIX_AMOUNT,
		UPPER_TRANSITION_ZONE,
		LOWER_TRANSITION_ZONE,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum Flags
	{
		MATERIAL_1_ENABLE		= 1<<0,
		MATERIAL_2_ENABLE		= 1<<1,
		MASK_ENABLE				= 1<<2,
		USE_CURVE				= 1<<3,
	};

	TupBlendMaterial(TupperwareAggregate *pObject);
	TupBlendMaterial(TupMaterial &materialBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddMaterial1Reference(int value);
	TupperwareScalar *AddMaterial2Reference(int value);
	TupperwareScalar *AddMaskReference(int value);
	TupperwareScalar *AddFlags(int value);
	TupperwareScalar *AddMixAmount(float value);
	TupperwareScalar *AddUpperTransitionZone(float value);
	TupperwareScalar *AddLowerTransitionZone(float value);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetMaterial1Reference(int &value);
	TupperwareScalar *GetMaterial2Reference(int &value);
	TupperwareScalar *GetMaskReference(int &value);
	TupperwareScalar *GetFlags(int &value);
	TupperwareScalar *GetMixAmount(float &value);
	TupperwareScalar *GetUpperTransitionZone(float &value);
	TupperwareScalar *GetLowerTransitionZone(float &value);

private:
// add methods
	TupperwareScalar *AddFloatScalar(float value,TupBlendMaterial::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupBlendMaterial::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupBlendMaterial::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupBlendMaterial::ChildrenAtoms index);
};

#endif
