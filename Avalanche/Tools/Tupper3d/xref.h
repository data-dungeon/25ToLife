////////////////////////////////////////////////////////////////////////////
//
// XRef
//
// Class for xrefs
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: xref.h $
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
 * User: Adam Clayton Date: 12/27/01   Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * xrefs and derived objects
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_XREF_H
#define TUPPER3D_XREF_H

class TupObject;

class TupXRef
{
	// Private Default constructor so no one can use it
	TupXRef(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		FILENAME,
		NODE_NAME,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupXRef(TupperwareAggregate *pObject);
	TupXRef(TupObject &objectBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddFilename(const char *name);
	TupperwareScalar *AddNodeName(const char *name);

// get methods
	TupperwareAggregate *GetBaseAggregate(void) ;
	TupperwareScalar *GetFilename(char **name);
	TupperwareScalar *GetNodeName(char **name);
};

#endif
