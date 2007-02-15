////////////////////////////////////////////////////////////////////////////
//
// CompositeMaterialSubMaterial
//
// Class to handle materials that a composite material points to
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: compositematerialsubmaterial.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/13/01   Time: 10:56a
 * Updated in $/Avalanche/tools/Tupper3d
 * More Materials
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_COMPOSITE_MATERIAL_SUB_MATERIAL_H
#define TUPPER3D_COMPOSITE_MATERIAL_SUB_MATERIAL_H

class TupCompositeMaterialSubMaterial
{
	// Private Default constructor so no one can use it
	TupCompositeMaterialSubMaterial(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		FLAGS,
		MIX_AMOUNT,
		MATERIAL_REFERENCE,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum Flags
	{
		ENABLED					= 1<<0,
		MIXING_MODE_MASK		= 3<<1,

		MIXING_MODE_ADDITIVE	= 0<<1,
		MIXING_MODE_SUBTRACTIVE = 1<<1,
		MIXING_MODE_MIXED		= 2<<1,
	};

	TupCompositeMaterialSubMaterial(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddMaterialReference(int materialIndex);
	TupperwareScalar *AddMixAmount(float value);
	TupperwareScalar *AddFlags(int value);

// get methods
	int GetSubMaterialIndex(void);
	TupperwareScalar *GetMaterialReference(int &materialIndex);
	TupperwareScalar *GetMixAmount(float &value);
	TupperwareScalar *GetFlags(int &value);
};

#endif
