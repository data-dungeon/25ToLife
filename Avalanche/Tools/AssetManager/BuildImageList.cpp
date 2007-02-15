////////////////////////////////////////////////////////////////////////////
//
// BuildImageList
//
// Class for adding bitmaps to image lists
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BuildImageList.cpp $
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

#include "StdAfx.h"
#include "BuildImageList.h"

int BuildImageList::Construct(CImageList &imageList,int elementWidth,int elementHeight,DWORD flags,COLORREF bkColor,int resourceID)
{
	HDC hDC = ::GetDC(NULL);

	BYTE bkRed = GetRValue(bkColor);
	BYTE bkGreen = GetGValue(bkColor);
	BYTE bkBlue = GetBValue(bkColor);

	PLAnyPicDecoder decoder;
	PLWinBmp bigImage;
	decoder.MakeBmpFromResource(AfxGetInstanceHandle(),resourceID,&bigImage,32);

	int bigImageWidth = bigImage.GetWidth();
	int bigImageHeight = bigImage.GetHeight();
	int bigImageSize = bigImageWidth*bigImageHeight*4;
	
	int x,y;
	BYTE ** pLineArray = bigImage.GetLineArray();
	BYTE *pBigImageData = new BYTE[bigImageSize];
	BYTE *pDest = pBigImageData;
	BYTE *pLine;

	// combine the bitmap (using alpha) with the background color
	for (y=0; y<bigImageHeight; y++)
	{
		pLine = pLineArray[y];
		
		// Iterate through the pixels
		for (x=0; x<bigImageWidth; x++)
		{
			float alphaFactor = ((float)pLine[PL_RGBA_ALPHA])/((float)0xff);
			pDest[PL_RGBA_RED] = (BYTE)((pLine[PL_RGBA_RED] * alphaFactor)+(bkRed*(1.0f-alphaFactor)));
			pDest[PL_RGBA_GREEN] = (BYTE)((pLine[PL_RGBA_GREEN] * alphaFactor)+(bkGreen*(1.0f-alphaFactor)));
			pDest[PL_RGBA_BLUE] = (BYTE)((pLine[PL_RGBA_BLUE] * alphaFactor)+(bkBlue*(1.0f-alphaFactor)));
			pDest[PL_RGBA_ALPHA] = 0;
			pDest+=4;
			pLine+=4;
		}
	}

	int numColumns = bigImageWidth/elementWidth;
	int numRows = bigImageHeight/elementHeight;
	int numElements = numColumns*numRows;

	imageList.Create(elementWidth,elementHeight, flags, numElements, 1);

	// zero the memory for the bitmap info
	BITMAPINFO smallBmi;  // bitmap header
	ZeroMemory(&smallBmi, sizeof(BITMAPINFO));
	
	// setup bitmap info 
	// set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas.
	smallBmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	smallBmi.bmiHeader.biWidth = elementWidth;
	smallBmi.bmiHeader.biHeight = elementHeight;
	smallBmi.bmiHeader.biPlanes = 1;
	smallBmi.bmiHeader.biBitCount = 32;         // four 8-bit components
	smallBmi.bmiHeader.biCompression = BI_RGB;
	smallBmi.bmiHeader.biSizeImage = 0;

	int elementImageSize = elementWidth*elementHeight*4;

	BYTE *pElementData = new BYTE[elementImageSize];

	int r,c;
	int startX,startY;

	for (r=0,startY=0;r<numRows;r++,startY+=elementHeight)
	{
		for (c=0,startX=0;c<numColumns;c++,startX+=elementWidth)
		{
			for (y=0; y<elementHeight; y++)
			{
				BYTE *pSource = &pBigImageData[(((startY+y)*bigImageWidth)+startX)*4];
				BYTE *pDest = &pElementData[((elementHeight-(y+1))*elementWidth)*4];

				// Iterate through the pixels
				for (x=0; x<elementWidth; x++)
				{
					pDest[PL_RGBA_RED] = pSource[PL_RGBA_RED];
					pDest[PL_RGBA_GREEN] = pSource[PL_RGBA_GREEN];
					pDest[PL_RGBA_BLUE] = pSource[PL_RGBA_BLUE];
					pDest[PL_RGBA_ALPHA] = 0;
					pDest+=4;
					pSource+=4;
				}
			}
			HBITMAP hBitmap = ::CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)&smallBmi, (LONG)CBM_INIT, pElementData, (LPBITMAPINFO)&smallBmi,  DIB_RGB_COLORS ); 
			CBitmap tempBitmap;
			tempBitmap.Attach(hBitmap);
			imageList.Add(&tempBitmap, (CBitmap*) NULL);
		}
	}
	delete pElementData;
	delete pBigImageData;
	return numElements;
}

