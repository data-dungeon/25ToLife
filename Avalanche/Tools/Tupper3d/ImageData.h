////////////////////////////////////////////////////////////////////////////
//
// ImageData
//
// Class to handle image data (actual bitmaps)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageData.h $
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

#ifndef TUPPER3D_IMAGE_DATA_H
#define TUPPER3D_IMAGE_DATA_H

class TupImageData
{
	// Private Default constructor so no one can use it
	TupImageData(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the atoms

public:
	TupImageData(TupperwareAggregate *pObject);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareAggregate *AddImageDataFrame(int frameNum);

// get methods
	int GetNumImageDataFrames(void);
	TupperwareAggregate *GetImageDataFrame(int index);
};

#endif
