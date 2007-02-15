////////////////////////////////////////////////////////////////////////////
//
// Map
//
// Handle the types of maps
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: map.h $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/30/02    Time: 9:40a
 * Updated in $/Avalanche/tools/Tupper3d
 * Map Name access on TupMap
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/13/02    Time: 1:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Mix Map
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/31/01   Time: 3:37p
 * Updated in $/Avalanche/tools/Tupper3d
 * modifiers pass 1
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

#ifndef TUPPER3D_MAP_H
#define TUPPER3D_MAP_H

class TupMap
{
	// Private Default constructor so no one can use it
	TupMap(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		MAP_TYPE,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum MapTypes
	{
		BITMAP_MAP,
		COMPOSITE_MAP,
		MIX_MAP,
		UNKNOWN,
		LAST_ENTRY,
	};
	static char *m_typeNames[LAST_ENTRY]; // names for the children atoms

	TupMap(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	void SetMapType(TupMap::MapTypes type);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareAggregate *GetBaseAggregate(void);
	TupMap::MapTypes GetMapType(void);
};


#endif
