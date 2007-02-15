////////////////////////////////////////////////////////////////////////////
//
// StandardMaterialMap
//
// Class to the maps that the standard material refers to
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: standardmaterialmap.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 9:39a
 * Updated in $/Avalanche/tools/Tupper3d
 * Changed from value shading types to a string.  Made the map now have a
 * mapname instead of reading from a table.  The name of the map aggregate
 * is the slot number instead.
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_STANDARD_MATERIAL_MAP_H
#define TUPPER3D_STANDARD_MATERIAL_MAP_H

class TupStandardMaterialMap
{
	// Private Default constructor so no one can use it
	TupStandardMaterialMap(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		AMOUNT,
		FLAGS,
		MAP_REFERENCE,
		MAP_NAME,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum Flags
	{
		ENABLED				= 1<<0,
	};

	TupStandardMaterialMap(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddMapName(const char *mapName);
	TupperwareScalar *AddMapReference(int value);
	TupperwareScalar *AddAmount(float value);
	TupperwareScalar *AddFlags(int value);

// get methods
	int GetMapChannel(void);
	TupperwareScalar *GetMapName(char **mapName);
	TupperwareScalar *GetMapReference(int &value);
	TupperwareScalar *GetAmount(float &value);
	TupperwareScalar *GetFlags(int &value);
};

#endif
