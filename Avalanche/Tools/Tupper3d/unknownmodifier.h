////////////////////////////////////////////////////////////////////////////
//
// UnknownModifier
//
// Class to handle an unknown modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: unknownmodifier.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * Get BaseAggregate
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/04/02    Time: 1:23p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier
 * Unknown Modifier
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_UNKNOWN_MODIFIER_H
#define TUPPER3D_UNKNOWN_MODIFIER_H

class TupModifier;

class TupUnknownModifier
{
	// Private Default constructor so no one can use it
	TupUnknownModifier(void);

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
	enum UnknownModifierFlags
	{
		FLAGS_ENABLED			= 1<<0,
	};

	TupUnknownModifier(TupperwareAggregate *pObject);
	TupUnknownModifier(TupModifier &modifierBase);

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
	TupperwareScalar *AddIntScalar(int value,TupUnknownModifier::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetIntScalar(int &value,TupUnknownModifier::ChildrenAtoms index);
};

#endif
