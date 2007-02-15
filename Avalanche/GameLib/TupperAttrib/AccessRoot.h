////////////////////////////////////////////////////////////////////////////
//
// AccessRoot
//
// Class to deal accessing the attributes from the game engine side
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AccessRoot.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:27p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added access classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERATTRIB_ACCESS_ROOT_H
#define TUPPERATTRIB_ACCESS_ROOT_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAccessRoot
{
	// Private Default constructor so no one can use it
	TupAccessRoot(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		INSTANCE_ATTRIBUTES,
		PATH_ATTRIBUTES,
		NODE_ATTRIBUTE_TREE,
		PATH_POINT_ATTRIBUTE_TREE,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	TupAccessRoot(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
  
  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareAggregate *AddInstanceAttributes(void);
	TupperwareAggregate *AddPathAttributes(void);
	TupperwareAggregate *AddNodeAttributeTree(TupperwareAggregate *pTreeAgg);
	TupperwareAggregate *AddPathPointAttributeTree(TupperwareAggregate *pTreeAgg);

// get methods
	TupperwareAggregate *GetInstanceAttributes(void);
	TupperwareAggregate *GetPathAttributes(void);
	TupperwareAggregate *GetNodeAttributeTree(void);
	TupperwareAggregate *GetPathPointAttributeTree(void);

private:
// add methods
	TupperwareAggregate *AddAggregate(TupAccessRoot::ChildrenAtoms index);

// get methods
	TupperwareAggregate *GetAggregate(TupAccessRoot::ChildrenAtoms index);
};

#endif