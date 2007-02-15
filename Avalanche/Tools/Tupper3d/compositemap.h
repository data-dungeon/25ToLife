////////////////////////////////////////////////////////////////////////////
//
// CompositeMap
//
// Class to handle the composite map
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: compositemap.h $
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
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_COMPOSITE_MAP_H
#define TUPPER3D_COMPOSITE_MAP_H

class TupMap;

class TupCompositeMap
{
	// Private Default constructor so no one can use it
	TupCompositeMap(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		SUB_MAPS,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupCompositeMap(TupperwareAggregate *pObject);
	TupCompositeMap(TupMap &mapBase);

	void BuildAtomCache(void);

// add methods

	TupperwareScalar *AddName(const char *name);
	TupperwareAggregate *AddSubMap(int index);

// get methods

	TupperwareScalar *GetName(char **name);
	int GetNumSubMaps(void);
	TupperwareAggregate *GetSubMap(int index);
};

#endif
