////////////////////////////////////////////////////////////////////////////
//
// DerivedModifier
//
// Class to handle the modifier information within a derived object
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: derivedmodifier.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:55p
 * Updated in $/Avalanche/tools/Tupper3d
 * Modifier Index
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * xrefs and derived objects
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_DERIVED_MODIFIER_H
#define TUPPER3D_DERIVED_MODIFIER_H

class TupDerivedModifier
{
	// Private Default constructor so no one can use it
	TupDerivedModifier(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		MODIFIER_REFERENCE,
		MODIFIER_CONTEXT,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	TupDerivedModifier(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddModifierReference(int modifierReference);
	TupperwareAggregate *AddModifierContext(void);

// get methods
	int GetModifierIndex(void);
	TupperwareScalar *GetModifierReference(int &modifierReference);
	TupperwareAggregate *GetModifierContext(void);
};

#endif
