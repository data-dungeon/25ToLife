////////////////////////////////////////////////////////////////////////////
//
// AccessInstance
//
// Class to deal accessing the instance attributes from the game engine side
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AccessInstance.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:27p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added access classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERATTRIB_ACCESS_INSTANCE_H
#define TUPPERATTRIB_ACCESS_INSTANCE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwarePoolValueString;

class TupAccessInstancePool
{
	// Private Default constructor so no one can use it
	TupAccessInstancePool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pInstancePool; // the pool that knows about all the child atoms

public:
	TupAccessInstancePool(TupperwareAggregate *pObject); // use this one
	~TupAccessInstancePool();

// add methods
	int AddInstance(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddInstance(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetInstance(int index); // return the aggregate using a integer index
	int GetNumInstances(void);
};


class TupAccessInstance
{
	// Private Default constructor so no one can use it
	TupAccessInstance(void);

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
	TupAccessInstance(TupperwareAggregate *pObject);

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
