////////////////////////////////////////////////////////////////////////////
//
// Locator
//
// Class for Locators
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: locator.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_LOCATOR_H
#define TUPPER3D_LOCATOR_H

class TupObject;

class TupLocator
{
	// Private Default constructor so no one can use it
	TupLocator(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupLocator(TupperwareAggregate *pObject);
	TupLocator(TupObject &objectBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddFlags(int value);
	
	// get methods
	TupperwareScalar *GetFlags(int &value);
	
private:
	// add methods
	TupperwareScalar *AddFloatScalar(float value,TupLocator::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupLocator::ChildrenAtoms index);
	
	// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupLocator::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupLocator::ChildrenAtoms index);
};

#endif