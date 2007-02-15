////////////////////////////////////////////////////////////////////////////
//
// AccessPath
//
// Class to deal accessing the path attributes from the game engine side
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AccessPath.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:27p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added access classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERATTRIB_ACCESS_PATH_H
#define TUPPERATTRIB_ACCESS_PATH_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwarePoolValueString;

class TupAccessPathPool
{
	// Private Default constructor so no one can use it
	TupAccessPathPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pPathPool; // the pool that knows about all the child atoms

public:
	TupAccessPathPool(TupperwareAggregate *pObject); // use this one
	~TupAccessPathPool();

// add methods
	int AddPath(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddPath(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetPath(int index); // return the aggregate using a integer index
	int GetNumPaths(void);
};

class TupAccessPath
{
	// Private Default constructor so no one can use it
	TupAccessPath(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		ATTRIBUTES,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	TupAccessPath(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
  
  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddAttributes(const char *attributesString);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetAttributes(char **attributesString);
};

#endif
