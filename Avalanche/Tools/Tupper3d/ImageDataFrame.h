////////////////////////////////////////////////////////////////////////////
//
// ImageDataFrame
//
// Class to handle a single frame (may have more than one bitmap i.e. mip maps)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageDataFrame.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 1/24/03    Time: 5:30p
 * Updated in $/Avalanche/tools/Tupper3d
 * added 8 bit has alpha flag
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 7/25/02    Time: 4:45p
 * Updated in $/Avalanche/tools/Tupper3d
 * added clear palette function
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 6/04/02    Time: 2:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * changed to 1555
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/08/02    Time: 3:07p
 * Updated in $/Avalanche/tools/Tupper3d
 * added hasalpha flag
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/07/02    Time: 4:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added has alpha flag
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/12/02    Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * More Flags
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/14/02    Time: 11:21a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the ImageData Classes to old bitmap data
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_IMAGE_DATA_FRAME_H
#define TUPPER3D_IMAGE_DATA_FRAME_H

class TupImageDataFrame
{
	// Private Default constructor so no one can use it
	TupImageDataFrame(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the atoms

	enum ChildrenAtoms 
	{
		FORMAT_FLAGS,
		AUXILARY_FLAGS,
		IMAGE_DATA_PALETTE,
		IMAGE_DATA_MAPS,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum FormatFlags
	{
		FORMAT_MASK			= 15<<0,
		RGBA_8888			= 0<<0,
		RGB_888				= 1<<0,
		RGB_555				= 2<<0,
		RGB_565				= 3<<0,
		RGBA_1555			= 4<<0,
		INDEXED_8			= 5<<0,
		INDEXED_4			= 6<<0,

		HAS_ALPHA			= 1<<4,
		HAS_8BITALPHA		= 1<<5,
	};

	TupImageDataFrame(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareScalar *AddFormatFlags(int formatFlags);
	TupperwareScalar *AddAuxilaryFlags(int auxilaryFlags);
	TupperwareAggregate *AddImageDataPalette(void);
	TupperwareAggregate *AddImageDataMap(int mapIndex);

// get methods
	TupperwareScalar *GetFormatFlags(int &formatFlags);
	TupperwareScalar *GetAuxilaryFlags(int &auxilaryFlags);
	TupperwareAggregate *GetImageDataPalette(void);
	int GetNumImageDataMaps(void);
	TupperwareAggregate *GetImageDataMap(int mapIndex);

// clear methods
	void ClearImageDataPalette(void);

private:
	TupperwareScalar *AddIntScalar(int value,TupImageDataFrame::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupImageDataFrame::ChildrenAtoms index);
};

#endif
