////////////////////////////////////////////////////////////////////////////
//
// BuildImageList
//
// Class for adding bitmaps to image lists
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BuildImageList.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/01/03    Time: 12:57p
 * Updated in $/Avalanche/tools/AssetManager
 * final version of indicator bitmap
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/31/03    Time: 1:31p
 * Updated in $/Avalanche/tools/AssetManager
 * new indicator icons
 */

#ifndef ASSET_MANAGER_BUILD_IMAGE_LIST_H
#define ASSET_MANAGER_BUILD_IMAGE_LIST_H

class BuildImageList
{

public:
	static int Construct(CImageList &imageList,int width,int height,DWORD flags,COLORREF bkColor,int resourceID);
};

#endif
