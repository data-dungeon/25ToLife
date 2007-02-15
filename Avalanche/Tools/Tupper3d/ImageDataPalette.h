////////////////////////////////////////////////////////////////////////////
//
// ImageDataPalette
//
// Class to handle a palette for the image data frame
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageDataPalette.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/14/02    Time: 11:21a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the ImageData Classes to old bitmap data
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_IMAGE_DATA_PALETTE_H
#define TUPPER3D_IMAGE_DATA_PALETTE_H

class TupImageDataPalette
{
	// Private Default constructor so no one can use it
	TupImageDataPalette(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the atoms

	enum ChildrenAtoms 
	{
		FLAGS,
		DATA,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

public:
	TupImageDataPalette(TupperwareAggregate *pObject);

// add methods
	TupperwareScalar *AddFlags(int flags);
	TupperwareScalar *AddData(const void *pData,int numBytes);

// get methods
	TupperwareScalar *GetFlags(int &flags);
	TupperwareScalar *GetData(void **pData,int &numBytes);

private:
	TupperwareScalar *AddIntScalar(int value,TupImageDataPalette::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupImageDataPalette::ChildrenAtoms index);
};

#endif
