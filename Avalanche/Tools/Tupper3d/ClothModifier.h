////////////////////////////////////////////////////////////////////////////
//
// ClothModifier
//
// Class to handle the Cloth Modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ClothModifier.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 2:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * Cloth Modifier
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_CLOTH_MODIFIER_H
#define TUPPER3D_CLOTH_MODIFIER_H

class TupModifier;

class TupClothModifier
{
	// Private Default constructor so no one can use it
	TupClothModifier(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum ClothModifierFlags
	{
		FLAGS_ENABLED			= 1<<0,
	};

	TupClothModifier(TupperwareAggregate *pObject);
	TupClothModifier(TupModifier &modifierBase);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddFlags(int value);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetFlags(int &value);

private:
// add methods
	TupperwareScalar *AddIntScalar(int value,TupClothModifier::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetIntScalar(int &value,TupClothModifier::ChildrenAtoms index);
};

#endif
