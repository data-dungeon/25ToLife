////////////////////////////////////////////////////////////////////////////
//
// Image
//
// Class to handle image information
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: image.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/08/02    Time: 9:17a
 * Updated in $/Avalanche/tools/Tupper3d
 * getbaseaggregate
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_IMAGE_H
#define TUPPER3D_IMAGE_H

class TupImage
{
	// Private Default constructor so no one can use it
	TupImage(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		FILE_PATH_NAME,
		USER_PROPERTIES,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	TupImage(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddFilePathName(const char *name);
	TupperwareScalar *AddUserProperties(const char *userPropertyString);

// get methods
	TupperwareAggregate *GetBaseAggregate(void);
	TupperwareScalar *GetFilePathName(char **name);
	TupperwareScalar *GetUserProperties(char **userPropertyString);
};

#endif
