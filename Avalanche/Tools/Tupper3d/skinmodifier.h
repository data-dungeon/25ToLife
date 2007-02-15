////////////////////////////////////////////////////////////////////////////
//
// SkinModifier
//
// Class to handle the Skin Modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifier.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_SKIN_MODIFIER_H
#define TUPPER3D_SKIN_MODIFIER_H

class TupModifier;

class TupSkinModifier
{
	// Private Default constructor so no one can use it
	TupSkinModifier(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		FLAGS,
		BONES,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum FaceAttributesModifierFlags
	{
		FLAGS_ENABLED			= 1<<0,
	};

	TupSkinModifier(TupperwareAggregate *pObject);
	TupSkinModifier(TupModifier &modifierBase);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddFlags(int value);
	TupperwareAggregate *AddBone(int index);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetFlags(int &value);
	int GetNumBones(void);
	TupperwareAggregate *GetBone(int index);

private:
// add methods
	TupperwareScalar *AddIntScalar(int value,TupSkinModifier::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetIntScalar(int &value,TupSkinModifier::ChildrenAtoms index);
};

#endif
