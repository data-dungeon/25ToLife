////////////////////////////////////////////////////////////////////////////
//
// ImageUtilities
//
// Classes to handle format convertion & manipulation of TupImageData objects
//
////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "ImageUtilities.h"
#include "PaletteGenerator.h"
#include "CRC.h"

void ImageUtilities::ConvertToFormat(TupImageData &tupImageData,int newFormat)
{
	int numFrames = tupImageData.GetNumImageDataFrames();
	for (int frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		assert(pFrameAgg);
		TupImageDataFrame tupImageDataFrame(pFrameAgg);

		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);

		// only do if the format is different
		if ((formatFlags&TupImageDataFrame::FORMAT_MASK)!=newFormat)
		{
			switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
			{
				case TupImageDataFrame::RGBA_8888:
					{
						switch (newFormat)
						{
							case TupImageDataFrame::RGBA_8888:
								assert(false); // should not be
								break;
							case TupImageDataFrame::RGB_888:
								Convert8888to888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_565:
								Convert8888to565(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_555:
								Convert8888to555(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGBA_1555:
								Convert8888to1555(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_8:
								Convert8888to8(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_4:
								Convert8888to4(tupImageDataFrame);
								break;
						}
					}
					break;
				case TupImageDataFrame::RGB_888:
					{
						switch (newFormat)
						{
							case TupImageDataFrame::RGBA_8888:
								Convert888to8888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_888:
								assert(false); // should not be
								break;
							case TupImageDataFrame::RGB_565:
								Convert888to8888(tupImageDataFrame);
								Convert8888to565(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_555:
								Convert888to8888(tupImageDataFrame);
								Convert8888to555(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGBA_1555:
								Convert888to8888(tupImageDataFrame);
								Convert8888to1555(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_8:
								Convert888to8888(tupImageDataFrame);
								Convert8888to8(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_4:
								Convert888to8888(tupImageDataFrame);
								Convert8888to4(tupImageDataFrame);
								break;
						}
					}
					break;
				case TupImageDataFrame::RGB_565:
					{
						switch (newFormat)
						{
							case TupImageDataFrame::RGBA_8888:
								Convert565to8888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_888:
								Convert565to8888(tupImageDataFrame);
								Convert8888to888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_565:
								assert(false); // should not be
								break;
							case TupImageDataFrame::RGB_555:
								Convert565to8888(tupImageDataFrame);
								Convert8888to555(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGBA_1555:
								Convert565to8888(tupImageDataFrame);
								Convert8888to1555(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_8:
								Convert565to8888(tupImageDataFrame);
								Convert8888to8(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_4:
								Convert565to8888(tupImageDataFrame);
								Convert8888to4(tupImageDataFrame);
								break;
						}
					}
					break;
				case TupImageDataFrame::RGB_555:
					{
						switch (newFormat)
						{
							case TupImageDataFrame::RGBA_8888:
								Convert555to8888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_888:
								Convert555to8888(tupImageDataFrame);
								Convert8888to888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_565:
								Convert555to8888(tupImageDataFrame);
								Convert8888to565(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_555:
								assert(false); // should not be
								break;
							case TupImageDataFrame::RGBA_1555:
								Convert555to8888(tupImageDataFrame);
								Convert8888to1555(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_8:
								Convert555to8888(tupImageDataFrame);
								Convert8888to8(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_4:
								Convert555to8888(tupImageDataFrame);
								Convert8888to4(tupImageDataFrame);
								break;
						}
					}
					break;
				case TupImageDataFrame::RGBA_1555:
					{
						switch (newFormat)
						{
							case TupImageDataFrame::RGBA_8888:
								Convert1555to8888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_888:
								Convert1555to8888(tupImageDataFrame);
								Convert8888to888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_565:
								Convert1555to8888(tupImageDataFrame);
								Convert8888to565(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_555:
								Convert1555to8888(tupImageDataFrame);
								Convert8888to555(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGBA_1555:
								assert(false); // should not be
								break;
							case TupImageDataFrame::INDEXED_8:
								Convert1555to8888(tupImageDataFrame);
								Convert8888to8(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_4:
								Convert1555to8888(tupImageDataFrame);
								Convert8888to4(tupImageDataFrame);
								break;
						}
					}
					break;
				case TupImageDataFrame::INDEXED_8:
					{
						switch (newFormat)
						{
							case TupImageDataFrame::RGBA_8888:
								Convert8to8888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_888:
								Convert8to8888(tupImageDataFrame);
								Convert8888to888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_565:
								Convert8to8888(tupImageDataFrame);
								Convert8888to565(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_555:
								Convert8to8888(tupImageDataFrame);
								Convert8888to555(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGBA_1555:
								Convert8to8888(tupImageDataFrame);
								Convert8888to1555(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_8:
								assert(false); // should not be
								break;
							case TupImageDataFrame::INDEXED_4:
								Convert8to8888(tupImageDataFrame);
								Convert8888to4(tupImageDataFrame);
								break;
						}
					}
					break;
				case TupImageDataFrame::INDEXED_4:
					{
						switch (newFormat)
						{
							case TupImageDataFrame::RGBA_8888:
								Convert4to8888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_888:
								Convert4to8888(tupImageDataFrame);
								Convert8888to888(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_565:
								Convert4to8888(tupImageDataFrame);
								Convert8888to565(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGB_555:
								Convert4to8888(tupImageDataFrame);
								Convert8888to555(tupImageDataFrame);
								break;
							case TupImageDataFrame::RGBA_1555:
								Convert4to8888(tupImageDataFrame);
								Convert8888to1555(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_8:
								Convert4to8888(tupImageDataFrame);
								Convert8888to8(tupImageDataFrame);
								break;
							case TupImageDataFrame::INDEXED_4:
								assert(false); // should not be
								break;
						}
					}
					break;
			}
		}
	}
}

void ImageUtilities::Scale(TupImageData &tupImageData,int newWidth,int newHeight)
{
	int numFrames = tupImageData.GetNumImageDataFrames();
	for (int frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		assert(pFrameAgg);
		TupImageDataFrame tupImageDataFrame(pFrameAgg);

		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);

		int numMaps = tupImageDataFrame.GetNumImageDataMaps();
		assert(numMaps==1); // don't handle mip maps...

		switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
				Scale8888(tupImageDataFrame,newWidth,newHeight,0,0);
				break;
			case TupImageDataFrame::RGB_888:
				Convert888to8888(tupImageDataFrame);
				Scale8888(tupImageDataFrame,newWidth,newHeight,0,0);
				Convert8888to888(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_565:
				Convert565to8888(tupImageDataFrame);
				Scale8888(tupImageDataFrame,newWidth,newHeight,0,0);
				Convert8888to565(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_555:
				Convert555to8888(tupImageDataFrame);
				Scale8888(tupImageDataFrame,newWidth,newHeight,0,0);
				Convert8888to555(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGBA_1555:
				Convert1555to8888(tupImageDataFrame);
				Scale8888(tupImageDataFrame,newWidth,newHeight,0,0);
				Convert8888to1555(tupImageDataFrame);
				break;
			case TupImageDataFrame::INDEXED_8:
				Convert8to8888(tupImageDataFrame);
				Scale8888(tupImageDataFrame,newWidth,newHeight,0,0);
				Convert8888to8(tupImageDataFrame);
				break;
			case TupImageDataFrame::INDEXED_4:
				Convert4to8888(tupImageDataFrame);
				Scale8888(tupImageDataFrame,newWidth,newHeight,0,0);
				Convert8888to4(tupImageDataFrame);
				break;
		}
	}
}

void ImageUtilities::Scale8888(TupImageDataFrame &tupImageDataFrame,int newWidth,int newHeight,int sourceMapIndex,int destMapIndex)
{
	TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(sourceMapIndex);
	assert(pMapAgg); // source better be there
	TupImageDataMap tupSourceImageDataMap(pMapAgg);

	// get dest map (create if not there)
	pMapAgg = tupImageDataFrame.GetImageDataMap(destMapIndex);
	if (!pMapAgg)
	{
		pMapAgg = tupImageDataFrame.AddImageDataMap(destMapIndex);
	}
	TupImageDataMap tupDestImageDataMap(pMapAgg);

	int i,x,y;

	int width;
	int height;

	tupSourceImageDataMap.GetWidth(width);
	tupSourceImageDataMap.GetHeight(height);

	// don't bother if we are not changing the size
	if (width==newWidth && height==newHeight && sourceMapIndex == destMapIndex)
		return;

	BYTE *pData;
	int numSourceBytes;
	tupSourceImageDataMap.GetData((void **)&pData,numSourceBytes);
	assert(numSourceBytes==(width*height)<<2);

	double widthFactor = (double) (width - 1) / (double) (newWidth - 1);
	double heightFactor = (double) (height - 1) / (double) (newHeight - 1);

	int newSize = (newWidth*newHeight)<<2;
	BYTE *pNewData = new BYTE[newSize];

	for (x = 0; x < newWidth; x++)
	{
		for (y = 0; y < newHeight; y++)
		{
			double widthFraction = ((double)x) * widthFactor;
			int left = (int) widthFraction;
			if (left<0)
			{
				left = 0;
				widthFraction = 0.0;
			}
			int right = left + 1;
			if (right >= width)
			{
				right = width - 1;
				left = right - 1;
				if (left < 0)
					left = 0;
				widthFraction = (double) right;
			}
			widthFraction -= (double) left;

			double heightFraction = ((double)y) * heightFactor;
			int top = (int) heightFraction;
			if (top < 0)
			{
				top = 0;
				heightFraction = 0.0;
			}
			int bottom = top +1;
			if ( bottom >= height)
			{
				bottom = height-1;
				top = bottom-1;
				if (top < 0)
					top = 0;
				heightFraction = (double) bottom;
			}
			heightFraction -= (double) top;

			double topLeftWeight = (1.0 - widthFraction) * (1.0 - heightFraction);
			double topRightWeight = widthFraction * (1.0 - heightFraction);
			double bottomLeftWeight = (1.0 - widthFraction) * heightFraction;
			double bottomRightWeight = widthFraction * heightFraction;

			BYTE *pTarget = (pNewData + (((y * newWidth) + x)<<2));
			BYTE *pTopLeft = (pData + (((top * width) + left)<<2));
			BYTE *pTopRight = (pData + (((top * width) + right)<<2));
			BYTE *pBottomLeft = (pData + (((bottom * width) + left)<<2));
			BYTE *pBottomRight = (pData + (((bottom * width) + right)<<2));

			for (i = 0; i < 4; i++)
			{
				pTarget[i] = (BYTE) ( 
						(topLeftWeight * (double) pTopLeft[i]) +
						(topRightWeight * (double) pTopRight[i]) +
						(bottomLeftWeight * (double) pBottomLeft[i]) +
						(bottomRightWeight * (double) pBottomRight[i]) +
						0.5);
			}
		}
	}
	tupDestImageDataMap.AddWidth(newWidth);
	tupDestImageDataMap.AddHeight(newHeight);
	tupDestImageDataMap.AddData(pNewData,newSize);
	delete pNewData;
}


void ImageUtilities::Convert888to8888(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u8 *pData;
		int numBytes;

		// convert 3 bytes per pixel to 4
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes/3;
		assert((numPixels*3)==numBytes);
		u32 *pNewData = new u32[numPixels];

		int destIndex,srcIndex;
		for (destIndex=0,srcIndex=0;destIndex<numPixels;destIndex++,srcIndex+=3)
		{
			pNewData[destIndex] = pData[srcIndex+0] | ((u32)pData[srcIndex+1]<<8) | ((u32)pData[srcIndex+2]<<16) | 0xff000000;
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<2);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGBA_8888);
}

void ImageUtilities::Convert565to8888(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u16 *pData;
		int numBytes;

		// convert 2 bytes per pixel to 4
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>1;
		assert((numPixels<<1)==numBytes);
		u32 *pNewData = new u32[numPixels];

		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
		{
			// 84218421842184218421842184218421
			//                 rrrrrggggggbbbbb to 
			// 11111111bbbbb000gggggg00rrrrr000
			pNewData[pixelIndex] = ((((u32)(pData[pixelIndex]&0x001f))<<19) | // blue
								   (((u32)(pData[pixelIndex]&0x07e0))<<5) | // green 
								   (((u32)(pData[pixelIndex]&0xf800))>>8) | // red
								   0xff000000); // alpha
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<2);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGBA_8888);
}

void ImageUtilities::Convert555to8888(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u16 *pData;
		int numBytes;

		// convert 2 bytes per pixel to 4
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>1;
		assert((numPixels<<1)==numBytes);
		u32 *pNewData = new u32[numPixels];

		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
		{
			// 84218421842184218421842184218421
			//                 0rrrrrgggggbbbbb to 
			// 11111111bbbbb000ggggg000rrrrr000
			pNewData[pixelIndex] = ((((u32)(pData[pixelIndex]&0x001f))<<19) | // blue
								   (((u32)(pData[pixelIndex]&0x03e0))<<6) | // green 
								   (((u32)(pData[pixelIndex]&0x7c00))>>7) | // red
								   0xff000000); // alpha
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<2);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGBA_8888);
}

void ImageUtilities::Convert1555to8888(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u16 *pData;
		int numBytes;

		// convert 2 bytes per pixel to 4
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>1;
		assert((numPixels<<1)==numBytes);
		u32 *pNewData = new u32[numPixels];

		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
		{
			// 84218421842184218421842184218421
			//                 arrrrrgggggbbbbb to 
			// 11111111bbbbb000ggggg000rrrrr000 or
			// 00000000bbbbb000ggggg000rrrrr000
			u32 alphaMask = pData[pixelIndex]&0x8000 ? 0xff000000 : 0;
			pNewData[pixelIndex] = ((((u32)(pData[pixelIndex]&0x001f))<<19) | // blue
								   (((u32)(pData[pixelIndex]&0x03e0))<<6) | // green 
								   (((u32)(pData[pixelIndex]&0x7c00))>>7) | // red
								   alphaMask); // alpha
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<2);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGBA_8888);
}

void ImageUtilities::Convert8to8888(TupImageDataFrame &tupImageDataFrame)
{
	TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
	assert(pPaletteAgg);
	TupImageDataPalette tupImageDataPalette(pPaletteAgg);
	u32 *pPaletteData;
	int numPaletteBytes;
	tupImageDataPalette.GetData((void **)&pPaletteData,numPaletteBytes);
	assert(numPaletteBytes==1024);
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u8 *pData;
		int numBytes;

		// convert 1 bytes per pixel to 4
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes;
		u32 *pNewData = new u32[numPixels];

		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
		{
			pNewData[pixelIndex] = pPaletteData[pData[pixelIndex]];
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<2);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGBA_8888);
	tupImageDataFrame.ClearImageDataPalette(); // no more palette
}

void ImageUtilities::Convert4to8888(TupImageDataFrame &tupImageDataFrame)
{
	TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
	assert(pPaletteAgg);
	TupImageDataPalette tupImageDataPalette(pPaletteAgg);
	u32 *pPaletteData;
	int numPaletteBytes;
	tupImageDataPalette.GetData((void **)&pPaletteData,numPaletteBytes);
	assert(numPaletteBytes==64);

	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u8 *pData;
		int numBytes;

		// convert 2 pixels per byte to 4 bytes per pixel
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes<<1;
		u32 *pNewData = new u32[numPixels];

		int destIndex,srcIndex;
		for (destIndex=0,srcIndex=0;destIndex<numPixels;destIndex+=2,srcIndex++)
		{
			pNewData[destIndex] = pPaletteData[pData[srcIndex]&0xf];
			pNewData[destIndex+1] = pPaletteData[pData[srcIndex]>>4];
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<2);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGBA_8888);
	tupImageDataFrame.ClearImageDataPalette(); // no more palette
}

void ImageUtilities::Convert8888to888(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u32 *pData;
		int numBytes;

		// convert 4 bytes per pixel to 3
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);
		u8 *pNewData = new u8[numPixels*3];

		int destIndex,srcIndex;
		for (destIndex=0,srcIndex=0;destIndex<numPixels;destIndex+=3,srcIndex++)
		{
			pNewData[destIndex] = (u8)(pData[srcIndex]&0xff); // blue
			pNewData[destIndex+1] = (u8)((pData[srcIndex]>>8)&0xff); // green
			pNewData[destIndex+2] = (u8)((pData[srcIndex]>>16)&0xff); // red
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels*3);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	formatFlags &= ~(TupImageDataFrame::HAS_ALPHA|TupImageDataFrame::HAS_8BITALPHA); // new image has no alpha
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGB_888);
}

void ImageUtilities::Convert8888to565(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u32 *pData;
		int numBytes;

		// convert 4 bytes per pixel to 2
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);
		u16 *pNewData = new u16[numPixels];

		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
		{
			// 84218421842184218421842184218421
			// xxxxxxxxbbbbb000gggggg00rrrrr000 to
			//                 rrrrrggggggbbbbb
			pNewData[pixelIndex] = (u16)(((pData[pixelIndex]&0x000000f8)<<8) | // red
										 ((pData[pixelIndex]&0x0000fc00)>>5) | // green 
										 ((pData[pixelIndex]&0x00f80000)>>19));  // blue
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<1);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	formatFlags &= ~(TupImageDataFrame::HAS_ALPHA|TupImageDataFrame::HAS_8BITALPHA); // new image has no alpha
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGB_565);
}

void ImageUtilities::Convert8888to555(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u32 *pData;
		int numBytes;

		// convert 4 bytes per pixel to 2
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);
		u16 *pNewData = new u16[numPixels];

		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
		{
			// 84218421842184218421842184218421
			// xxxxxxxxbbbbb000gggggg00rrrrr000 to
			//                 0rrrrrgggggbbbbb
			pNewData[pixelIndex] = (u16)(((pData[pixelIndex]&0x000000f8)<<7) | // red
										 ((pData[pixelIndex]&0x0000f800)>>6) | // green 
										 ((pData[pixelIndex]&0x00f80000)>>19));  // blue
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<1);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	formatFlags &= ~(TupImageDataFrame::HAS_ALPHA|TupImageDataFrame::HAS_8BITALPHA); // new image has no alpha
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGB_555);
}

void ImageUtilities::Convert8888to1555(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u32 *pData;
		int numBytes;

		// convert 4 bytes per pixel to 2
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);
		u16 *pNewData = new u16[numPixels];

		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
		{
			// 84218421842184218421842184218421
			// aaaaaaaabbbbb000gggggg00rrrrr000 to
			//                 arrrrrgggggbbbbb
			u16 alphaMask = pData[pixelIndex]&0xff000000 ? 0x8000 : 0;
			pNewData[pixelIndex] = (u16)(((pData[pixelIndex]&0x000000f8)<<7) | // red
										 ((pData[pixelIndex]&0x0000f800)>>6) | // green 
										 ((pData[pixelIndex]&0x00f80000)>>19) |  // blue
										 alphaMask); // alpha
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels<<1);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	formatFlags &= ~(TupImageDataFrame::HAS_8BITALPHA); // new image has no alpha
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::RGBA_1555);
}

void ImageUtilities::Convert8888to8(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	// generate a palette from the first map
	PaletteGenerator::RgbaColor newPalette[256];
	memset(newPalette,0,sizeof(PaletteGenerator::RgbaColor)*256);
	if (numMaps)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(0);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u32 *pData;
		int numBytes;

		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);

		PaletteGenerator paletteGenerator;
		paletteGenerator.InitRGBStats();

		paletteGenerator.GetRGBStats(pData,numPixels,1);
		paletteGenerator.GeneratePalette(newPalette,256,1.0f);
	}

	Keeper<u32> colorKeeper(4096);
	TupArray<u8> colorIndexArray;

	int paletteColorIndex;
	// make the palette easier to access
	int paletteRed[256];
	int paletteGreen[256];
	int paletteBlue[256];
	int paletteAlpha[256];
	for (paletteColorIndex=0;paletteColorIndex<256;paletteColorIndex++)
	{
		paletteRed[paletteColorIndex] = newPalette[paletteColorIndex].r;
		paletteGreen[paletteColorIndex] = newPalette[paletteColorIndex].g;
		paletteBlue[paletteColorIndex] = newPalette[paletteColorIndex].b;
		paletteAlpha[paletteColorIndex] = newPalette[paletteColorIndex].a;
	}

	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u32 *pData;
		int numBytes;

		// convert 4 bytes per pixel to 1
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);
		u8 *pNewData = new u8[numPixels];

		// remap values to new palette
		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
		{
			u32 colorValue = pData[pixelIndex];

			int oldKeeperSize = colorKeeper.GetSize();
			int keeperIndex = colorKeeper.Add(colorValue);
			// if it was not added to the keeper (previous color found)
			if (keeperIndex!=oldKeeperSize)
			{
				pNewData[pixelIndex] = colorIndexArray[keeperIndex]; // get the color index	
			}
			// if we could not find the color in the keeper
			else
			{
				int dataRed = colorValue&0xff;
				int dataGreen = (colorValue>>8)&0xff;
				int dataBlue = (colorValue>>16)&0xff;
				int dataAlpha = (colorValue>>24)&0xff;
				int bestDelta;
				int bestIndex=-1;
				for (paletteColorIndex=0;paletteColorIndex<256;paletteColorIndex++)
				{
					// calculate delta
					int delta = ((paletteRed[paletteColorIndex]-dataRed)*(paletteRed[paletteColorIndex]-dataRed)<<1)+ 
								((paletteGreen[paletteColorIndex]-dataGreen)*(paletteGreen[paletteColorIndex]-dataGreen)<<2)+ 
								((paletteBlue[paletteColorIndex]-dataBlue)*(paletteBlue[paletteColorIndex]-dataBlue)<<1)+
								((paletteAlpha[paletteColorIndex]-dataAlpha)*(paletteAlpha[paletteColorIndex]-dataAlpha)>>2);
					if (bestIndex==-1 || delta < bestDelta)
					{
						bestDelta = delta;
						bestIndex = paletteColorIndex;
					}
				}
				u8 colorIndex = (u8)bestIndex;
				pNewData[pixelIndex] = colorIndex;
				colorIndexArray.Add(colorIndex);
			}
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::INDEXED_8);
	// add in the palette
	TupperwareAggregate *tupImagePalette = tupImageDataFrame.AddImageDataPalette();
	TupImageDataPalette tupImageDataPalette(tupImagePalette);
	tupImageDataPalette.AddData(newPalette,1024);
}

void ImageUtilities::Convert8888to4(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	// generate a palette from the first map
	PaletteGenerator::RgbaColor newPalette[16];
	memset(newPalette,0,sizeof(PaletteGenerator::RgbaColor)*16);
	if (numMaps)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(0);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u32 *pData;
		int numBytes;

		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);

		PaletteGenerator paletteGenerator;
		paletteGenerator.InitRGBStats();

		paletteGenerator.GetRGBStats(pData,numPixels,1);
		paletteGenerator.GeneratePalette(newPalette,16,1.0f);
	}

	Keeper<u32> colorKeeper(4096);
	TupArray<u8> colorIndexArray;

	int paletteColorIndex;
	// make the palette easier to access
	int paletteRed[16];
	int paletteGreen[16];
	int paletteBlue[16];
	int paletteAlpha[16];
	for (paletteColorIndex=0;paletteColorIndex<16;paletteColorIndex++)
	{
		paletteRed[paletteColorIndex] = newPalette[paletteColorIndex].r;
		paletteGreen[paletteColorIndex] = newPalette[paletteColorIndex].g;
		paletteBlue[paletteColorIndex] = newPalette[paletteColorIndex].b;
		paletteAlpha[paletteColorIndex] = newPalette[paletteColorIndex].a;
	}

	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		u32 *pData;
		int numBytes;

		// convert 4 bytes per pixel to 2 pixels per byte
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);
		u8 *pNewData = new u8[numPixels>>1];
		int width;
		tupImageDataMap.GetWidth(width);
		assert((width&1)==0); // make sure the width is even number of pixels

		// remap values to new palette
		for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex+=2)
		{
			for (int subPixelIndex=0;subPixelIndex<2;subPixelIndex++)
			{
				u32 colorValue = pData[pixelIndex+subPixelIndex];

				int oldKeeperSize = colorKeeper.GetSize();
				int keeperIndex = colorKeeper.Add(colorValue);
				// if it was not added to the keeper (previous color found)
				if (keeperIndex!=oldKeeperSize)
				{
					if (subPixelIndex==0)
						pNewData[pixelIndex>>1] = colorIndexArray[keeperIndex]; // get the color index	
					else
						pNewData[pixelIndex>>1] |= colorIndexArray[keeperIndex]<<4; // get the color index	
				}
				// if we could not find the color in the keeper
				else
				{
					int dataRed = colorValue&0xff;
					int dataGreen = (colorValue>>8)&0xff;
					int dataBlue = (colorValue>>16)&0xff;
					int dataAlpha = (colorValue>>24)&0xff;
					int bestDelta;
					int bestIndex=-1;
					for (paletteColorIndex=0;paletteColorIndex<16;paletteColorIndex++)
					{
						// calculate delta
						int delta = ((paletteRed[paletteColorIndex]-dataRed)*(paletteRed[paletteColorIndex]-dataRed)<<1)+ 
									((paletteGreen[paletteColorIndex]-dataGreen)*(paletteGreen[paletteColorIndex]-dataGreen)<<2)+ 
									((paletteBlue[paletteColorIndex]-dataBlue)*(paletteBlue[paletteColorIndex]-dataBlue)<<1)+
									((paletteAlpha[paletteColorIndex]-dataAlpha)*(paletteAlpha[paletteColorIndex]-dataAlpha)>>2);
						if (bestIndex==-1 || delta < bestDelta)
						{
							bestDelta = delta;
							bestIndex = paletteColorIndex;
						}
					}
					u8 colorIndex = (u8)bestIndex;
					if (subPixelIndex==0)
						pNewData[pixelIndex>>1] = colorIndex; // get the color index	
					else
						pNewData[pixelIndex>>1] |= colorIndex<<4; // get the color index	
					colorIndexArray.Add(colorIndex);
				}
			}
		}
		// replace data with new data
		tupImageDataMap.AddData(pNewData,numPixels>>1);
		delete pNewData;
	}
	// set the new format
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	tupImageDataFrame.AddFormatFlags((formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::INDEXED_4);
	// add in the palette
	TupperwareAggregate *tupImagePalette = tupImageDataFrame.AddImageDataPalette();
	TupImageDataPalette tupImageDataPalette(tupImagePalette);
	tupImageDataPalette.AddData(newPalette,64);
}

// bilinear version
unsigned long ImageUtilities::GetPixelColor(TupImageDataFrame &tupImageDataFrame,int sourceMapIndex,float x,float y,bool bClampX,bool bClampY)
{
	TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(sourceMapIndex);
	assert(pImageDataMapAgg);

	TupImageDataMap tupImageDataMap(pImageDataMapAgg);

	int width;
	int height;

	tupImageDataMap.GetWidth(width);
	tupImageDataMap.GetHeight(height);

	// make coordinates >=0 && <=1.0
	if (x>1.0f)
	{
		if (bClampX)
			x = 1.0f;
		else
			x = x - (float((int) x));
	}
	if (x<0.0f)
	{
		if (bClampX)
			x = 0.0f;
		else
			x = 1.0f - (x - (float((int) x)));
	}

	if (y>1.0f)
	{
		if (bClampY)
			y = 1.0f;
		else
			y = y - (float((int) y));
	}
	if (y<0.0f)
	{
		if (bClampY)
			y = 0.0f;
		else
			y = 1.0f - (y - (float((int) y)));
	}
	double fImageX = (double)x*((double)width);
	int left = (int) fImageX;
	double xFraction = fImageX-(double)left;
	int right = left+1;
	if (right>=width)
	{
		right = width-1;
		left = right -1;
		if (left<0)
			left = 0;
		xFraction = 1.0f;
	}
	double fImageY = y*((double)height);
	int top = (int) fImageY;
	double yFraction = fImageY-(double)top;
	int bottom = top+1;
	if (bottom>=height)
	{
		bottom = height-1;
		top = bottom -1;
		if (top<0)
			top = 0;
		yFraction = 1.0f;
	}

	// calculate weights for each based on the fractions
	double topLeftWeight = (1.0 - xFraction) * (1.0 - yFraction);
	double topRightWeight = xFraction * (1.0 - yFraction);
	double bottomLeftWeight = (1.0 - xFraction) * yFraction;
	double bottomRightWeight = xFraction * yFraction;

	// get the 4 pixel colors to use
	u32 resultColor;
	u32 topLeftPixel = GetPixelColor(tupImageDataFrame,sourceMapIndex,left,top);
	u32 topRightPixel = GetPixelColor(tupImageDataFrame,sourceMapIndex,right,top);
	u32 bottomLeftPixel = GetPixelColor(tupImageDataFrame,sourceMapIndex,left,bottom);
	u32 bottomRightPixel = GetPixelColor(tupImageDataFrame,sourceMapIndex,right,bottom);

	u8 *pTarget = (u8 *)&resultColor;
	u8 *pTopLeft = (u8 *)&topLeftPixel;
	u8 *pTopRight = (u8 *)&topRightPixel;
	u8 *pBottomLeft = (u8 *)&bottomLeftPixel;
	u8 *pBottomRight = (u8 *)&bottomRightPixel;

	// go through each series of bytes and compute the color based on the weights and the colors
	for (int i = 0; i < 4; i++)
	{
		pTarget[i] = (u8) ( 
				(topLeftWeight * (double) pTopLeft[i]) +
				(topRightWeight * (double) pTopRight[i]) +
				(bottomLeftWeight * (double) pBottomLeft[i]) +
				(bottomRightWeight * (double) pBottomRight[i]) +
				0.5);
	}
	return resultColor;
}

unsigned long ImageUtilities::GetPixelColor(TupImageDataFrame &tupImageDataFrame,int sourceMapIndex,int x,int y)
{
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);

	TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(sourceMapIndex);
	assert(pImageDataMapAgg);

	TupImageDataMap tupImageDataMap(pImageDataMapAgg);

	int width;
	int height;

	tupImageDataMap.GetWidth(width);
	tupImageDataMap.GetHeight(height);

	assert(x>=0 && x<width && y>=0 && y<height);

	void *pVoidData;
	int numSourceBytes;
	tupImageDataMap.GetData(&pVoidData,numSourceBytes);

	u32 resultColor;

	switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::RGBA_8888:
			{
				unsigned long *pData = (unsigned long *) pVoidData;
				resultColor = pData[(width*y)+x];
			}
			break;
		case TupImageDataFrame::RGB_888:
			{
				u8 *pData = (u8 *) pVoidData;
				int pixelIndex = ((width*y)+x)*3;
				resultColor = pData[pixelIndex+0] | // red
							  ((u32)pData[pixelIndex+1]<<8) | // green
							  ((u32)pData[pixelIndex+2]<<16) | // blue
							  0xff000000; // alpha
			}
			break;
		case TupImageDataFrame::RGB_565:
			{
				// 84218421842184218421842184218421
				//                 0rrrrrgggggbbbbb to 
				// 11111111bbbbb000ggggg000rrrrr000
				u16 *pData = (u16 *) pVoidData;
				int pixelIndex = ((width*y)+x);
				resultColor = ((((u32)(pData[pixelIndex]&0x001f))<<19) | // blue
									   (((u32)(pData[pixelIndex]&0x03e0))<<6) | // green 
									   (((u32)(pData[pixelIndex]&0x7c00))>>7) | // red
									   0xff000000); // alpha
			}
			break;
		case TupImageDataFrame::RGB_555:
			{
				// 84218421842184218421842184218421
				//                 0rrrrrgggggbbbbb to 
				// 11111111bbbbb000ggggg000rrrrr000
				u16 *pData = (u16 *) pVoidData;
				int pixelIndex = ((width*y)+x);
				resultColor = ((((u32)(pData[pixelIndex]&0x001f))<<19) | // blue
									   (((u32)(pData[pixelIndex]&0x03e0))<<6) | // green 
									   (((u32)(pData[pixelIndex]&0x7c00))>>7) | // red
									   0xff000000); // alpha
			}
			break;
		case TupImageDataFrame::RGBA_1555:
			{
				// 84218421842184218421842184218421
				//                 arrrrrgggggbbbbb to 
				// 11111111bbbbb000ggggg000rrrrr000 or
				// 00000000bbbbb000ggggg000rrrrr000
				u16 *pData = (u16 *) pVoidData;
				int pixelIndex = ((width*y)+x);
				u32 alphaMask = pData[pixelIndex]&0x8000 ? 0xff000000 : 0;
				resultColor = ((((u32)(pData[pixelIndex]&0x001f))<<19) | // blue
									   (((u32)(pData[pixelIndex]&0x03e0))<<6) | // green 
									   (((u32)(pData[pixelIndex]&0x7c00))>>7) | // red
									   alphaMask); // alpha
			}
			break;
		case TupImageDataFrame::INDEXED_8:
			{
				TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
				assert(pPaletteAgg);
				TupImageDataPalette tupImageDataPalette(pPaletteAgg);
				u32 *pPaletteData;
				int numPaletteBytes;
				tupImageDataPalette.GetData((void **)&pPaletteData,numPaletteBytes);
				assert(numPaletteBytes==1024);

				u8 *pData = (u8 *) pVoidData;
				int pixelIndex = ((width*y)+x);
				resultColor = pPaletteData[pData[pixelIndex]];
			}
			break;
		case TupImageDataFrame::INDEXED_4:
			{
				TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
				assert(pPaletteAgg);
				TupImageDataPalette tupImageDataPalette(pPaletteAgg);
				u32 *pPaletteData;
				int numPaletteBytes;
				tupImageDataPalette.GetData((void **)&pPaletteData,numPaletteBytes);
				assert(numPaletteBytes==64);

				u8 *pData = (u8 *) pVoidData;
				int pixelIndex = ((width*y)+x)>>1;
				if (x&1)
				{
					resultColor = pPaletteData[pData[pixelIndex]>>4];
				}
				else
				{
					resultColor = pPaletteData[pData[pixelIndex]&0xf];
				}
				resultColor = pPaletteData[pData[pixelIndex]];
			}
			break;
	}
	return resultColor;
}

// calculate a CRC on the image data
u32 ImageUtilities::GetCRC(TupImageData &tupImageData)
{
	CRC CRCGenerator;
	int numFrames = tupImageData.GetNumImageDataFrames();
	CRCGenerator.AnalyzeData(numFrames);
	for (int frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		assert(pFrameAgg);
		TupImageDataFrame tupImageDataFrame(pFrameAgg);
		u32 CRCFrame = GetCRC(tupImageDataFrame);
		CRCGenerator.AnalyzeData(CRCFrame);
	}
	return CRCGenerator.GetResult();
}

// calculate a CRC on the image data
u32 ImageUtilities::GetCRC(TupImageDataFrame &tupImageDataFrame)
{
	CRC CRCGenerator;

	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);

	CRCGenerator.AnalyzeData(formatFlags);

	TupperwareAggregate *pImageDataPalette = tupImageDataFrame.GetImageDataPalette();
	if (pImageDataPalette)
	{
		TupImageDataPalette tupImageDataPalette(pImageDataPalette);
		void *pPaletteData;
		int numPaletteBytes;
		tupImageDataPalette.GetData(&pPaletteData,numPaletteBytes);
		CRCGenerator.AnalyzeData(pPaletteData,numPaletteBytes);
	}

	int numMaps = tupImageDataFrame.GetNumImageDataMaps();

	CRCGenerator.AnalyzeData(numMaps);

	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		int width;
		tupImageDataMap.GetWidth(width);
		int height;
		tupImageDataMap.GetHeight(height);
		CRCGenerator.AnalyzeData(width);
		CRCGenerator.AnalyzeData(height);
		void *pImageData;
		int numImageBytes;
		tupImageDataMap.GetData(&pImageData,numImageBytes);
		CRCGenerator.AnalyzeData(pImageData,numImageBytes);
	}
	return CRCGenerator.GetResult();
}


// returns the total number of used colors as well as an array (size 256 or 16) with counts for each color in the palette
int ImageUtilities::GetUsedColors(TupImageDataFrame &tupImageDataFrame,TupArray<int> &usedColors)
{
	int totalColors=0;
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::INDEXED_8:
			{
				usedColors.SetSize(256);
				int paletteIndex;
				// all are not used right now
				for (paletteIndex=0;paletteIndex<256;paletteIndex++)
				{
					usedColors[paletteIndex] = 0;
				}
				int numMaps = tupImageDataFrame.GetNumImageDataMaps();
				int mapIndex;
				for (mapIndex=0;mapIndex<numMaps;mapIndex++)
				{
					TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
					TupImageDataMap tupImageDataMap(pImageDataMapAgg);
					BYTE *pData;
					int numSourceBytes;
					tupImageDataMap.GetData((void **)&pData,numSourceBytes);
					int pixelIndex;
					for (pixelIndex=0;pixelIndex<numSourceBytes;pixelIndex++)
					{
						usedColors[pData[pixelIndex]]++;
					}
				}
				for (paletteIndex=0;paletteIndex<256;paletteIndex++)
				{
					if (usedColors[paletteIndex])
					{
						totalColors++;
					}
				}
			}
			break;
		case TupImageDataFrame::INDEXED_4:
			{
				usedColors.SetSize(16);
				int paletteIndex;
				// all are not used right now
				for (paletteIndex=0;paletteIndex<16;paletteIndex++)
				{
					usedColors[paletteIndex] = 0;
				}
				int numMaps = tupImageDataFrame.GetNumImageDataMaps();
				int mapIndex;
				for (mapIndex=0;mapIndex<numMaps;mapIndex++)
				{
					TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
					TupImageDataMap tupImageDataMap(pImageDataMapAgg);
					BYTE *pData;
					int numSourceBytes;
					tupImageDataMap.GetData((void **)&pData,numSourceBytes);
					int pixelIndex;
					for (pixelIndex=0;pixelIndex<numSourceBytes;pixelIndex++)
					{
						usedColors[pData[pixelIndex]>>4]++;
						usedColors[pData[pixelIndex]&0x0f]++;
					}
				}
				for (paletteIndex=0;paletteIndex<16;paletteIndex++)
				{
					if (usedColors[paletteIndex])
					{
						totalColors++;
					}
				}
			}
			break;
		default:
			assert(false); // should not be
			break;
	}
	return totalColors;
}

bool ImageUtilities::MergePalettes(TupImageDataFrame &frame1,TupArray<int> &usedColors1,
						  TupImageDataFrame &frame2,TupArray<int> &usedColors2,
						  TupArray<int> &remapTo1Array,int &colorsAdded,bool bTestOnly)
{
	int totalColors=0;
	int formatFlags1;
	frame1.GetFormatFlags(formatFlags1);
	int formatFlags2;
	frame2.GetFormatFlags(formatFlags2);
	int numPaletteColors;
	assert((formatFlags1 & TupImageDataFrame::FORMAT_MASK)==(formatFlags2 & TupImageDataFrame::FORMAT_MASK));

	switch(formatFlags1 & TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::INDEXED_8:
			{
				numPaletteColors = 256;
			}
			break;
		case TupImageDataFrame::INDEXED_4:
			{
				numPaletteColors = 16;
			}
			break;
		default:
			assert(false);
			return false;
	}
	assert(usedColors1.GetSize()==numPaletteColors);
	assert(usedColors2.GetSize()==numPaletteColors);
	TupperwareAggregate *pPalette1Agg = frame1.GetImageDataPalette();
	TupImageDataPalette tupPalette1(pPalette1Agg);
	u32 *pPaletteData1;
	int numSourceBytes1;
	tupPalette1.GetData((void **)&pPaletteData1,numSourceBytes1);
	assert(numSourceBytes1==numPaletteColors<<2);
	TupperwareAggregate *pPalette2Agg = frame2.GetImageDataPalette();
	TupImageDataPalette tupPalette2(pPalette2Agg);
	u32 *pPaletteData2;
	int numSourceBytes2;
	tupPalette2.GetData((void **)&pPaletteData2,numSourceBytes2);
	assert(numSourceBytes2==numPaletteColors<<2);

	Keeper<u32> uniqueColors;
	TupArray<int> uniqueColorToPaletteArray;
	int paletteIndex;
	// go through palette 1 and add to the unique colors
	// and set up the indexes from the unique color array to palette1 entries
	int numUsedColors1 = 0;
	for (paletteIndex=0;paletteIndex<numPaletteColors;paletteIndex++)
	{
		if (usedColors1[paletteIndex])
		{
			int beforeSize = uniqueColors.GetSize();
			int uniqueIndex = uniqueColors.Add(pPaletteData1[paletteIndex]);
			int afterSize = uniqueColors.GetSize();
			// if it was just added
			if (beforeSize!=afterSize)
			{
				// keep track of which colors in palette 1 the unique colors came from
				uniqueColorToPaletteArray.Add(paletteIndex);
			}
			numUsedColors1++; // count how many colors are used
		}
	}
	int numUniqueColorsPalette1 = uniqueColors.GetSize();

	int unusedColorIndex1=0;
	// find an unused color
	while (unusedColorIndex1<numPaletteColors)
	{
		if (!usedColors1[unusedColorIndex1])
		{
			break;
		}
		unusedColorIndex1++;
	}

	// this is how many colors we need to add
	if (!bTestOnly)
		remapTo1Array.SetSize(numPaletteColors);
	colorsAdded = 0;

	// go through palette 2 and add to the unique colors
	for (paletteIndex=0;paletteIndex<numPaletteColors;paletteIndex++)
	{
		if (usedColors2[paletteIndex])
		{
			int beforeSize = uniqueColors.GetSize();
			int uniqueIndex = uniqueColors.Add(pPaletteData2[paletteIndex]);
			int afterSize = uniqueColors.GetSize();
			// if it was just added
			if (beforeSize!=afterSize)
			{
				colorsAdded++;
				// if I need a free color and there isn't one then quit
				if (unusedColorIndex1==numPaletteColors)
					return false; // can't do
				// keep track of which colors in palette 2 the unique colors remap to in palette1
				uniqueColorToPaletteArray.Add(unusedColorIndex1);
				if (!bTestOnly)
				{
					pPaletteData1[unusedColorIndex1] = pPaletteData2[paletteIndex]; // add to palette 1
				}
				// find a new unused color
				unusedColorIndex1++;
				while (unusedColorIndex1<numPaletteColors)
				{
					if (!usedColors1[unusedColorIndex1])
					{
						break;
					}
					unusedColorIndex1++;
				}
			}
			if (!bTestOnly)
			{
				int palette1Index = uniqueColorToPaletteArray[uniqueIndex];
				remapTo1Array[paletteIndex] = palette1Index;
				usedColors1[palette1Index] += usedColors2[paletteIndex];
			}
		}
		else
		{
			if (!bTestOnly)
			{
				remapTo1Array[paletteIndex] = -1;
			}
		}
	}
	return true;
}

void ImageUtilities::RemapPalettedFrame(TupImageDataFrame &tupImageDataFrame,TupArray<int> &remapArray)
{
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::INDEXED_8:
			{
				assert(remapArray.GetSize()==256);
				int numMaps = tupImageDataFrame.GetNumImageDataMaps();
				int mapIndex;
				for (mapIndex=0;mapIndex<numMaps;mapIndex++)
				{
					TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
					TupImageDataMap tupImageDataMap(pImageDataMapAgg);
					BYTE *pData;
					int numSourceBytes;
					tupImageDataMap.GetData((void **)&pData,numSourceBytes);
					int pixelIndex;
					for (pixelIndex=0;pixelIndex<numSourceBytes;pixelIndex++)
					{
						int remappedData = remapArray[pData[pixelIndex]];
						assert(remappedData!=-1);
						pData[pixelIndex] = remappedData;
					}
				}
			}
			break;
		case TupImageDataFrame::INDEXED_4:
			{
				assert(remapArray.GetSize()==16);
				int numMaps = tupImageDataFrame.GetNumImageDataMaps();
				int mapIndex;
				for (mapIndex=0;mapIndex<numMaps;mapIndex++)
				{
					TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
					TupImageDataMap tupImageDataMap(pImageDataMapAgg);
					BYTE *pData;
					int numSourceBytes;
					tupImageDataMap.GetData((void **)&pData,numSourceBytes);
					int pixelIndex;
					for (pixelIndex=0;pixelIndex<numSourceBytes;pixelIndex++)
					{
						int remappedData1 = remapArray[pData[pixelIndex]>>4];
						assert(remappedData1!=-1);
						int remappedData2 = remapArray[pData[pixelIndex]&0x0f];
						assert(remappedData2!=-1);
						pData[pixelIndex] = (remappedData1<<4) | (remappedData2);
					}
				}
			}
			break;
		default:
			assert(false);
			break;
	}
}

void ImageUtilities::CopyPalette(TupImageDataFrame &dstFrame,TupImageDataFrame &srcFrame)
{
	TupperwareAggregate *pDstPaletteAgg = dstFrame.GetImageDataPalette();
	TupImageDataPalette tupDstPalette(pDstPaletteAgg);
	u32 *pDstPaletteData;
	int numDstSourceBytes;
	tupDstPalette.GetData((void **)&pDstPaletteData,numDstSourceBytes);

	TupperwareAggregate *pSrcPaletteAgg = srcFrame.GetImageDataPalette();
	TupImageDataPalette tupSrcPalette(pSrcPaletteAgg);
	u32 *pSrcPaletteData;
	int numSrcSourceBytes;
	tupSrcPalette.GetData((void **)&pSrcPaletteData,numSrcSourceBytes);
	assert(numDstSourceBytes==numSrcSourceBytes);
	memcpy(pDstPaletteData,pSrcPaletteData,numSrcSourceBytes);
}

int ImageUtilities::GetUsedColors(const TupArray<int> &usedColors)
{
	int count = 0;
	int size = usedColors.GetSize();
	for (int index=0;index<size;index++)
	{
		if (usedColors[index])
			count++;
	}
	return count;
}

void ImageUtilities::Reverse(TupImageData &tupImageData)
{
	int numFrames = tupImageData.GetNumImageDataFrames();
	for (int frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		assert(pFrameAgg);
		TupImageDataFrame tupImageDataFrame(pFrameAgg);

		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);

		switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
				Reverse32(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_888:
				Reverse24(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_565:
			case TupImageDataFrame::RGB_555:
			case TupImageDataFrame::RGBA_1555:
				Reverse16(tupImageDataFrame);
				break;
			case TupImageDataFrame::INDEXED_8:
				Reverse8(tupImageDataFrame);
				break;
			case TupImageDataFrame::INDEXED_4:
				Reverse4(tupImageDataFrame);
				break;
		}
	}
}

void ImageUtilities::Reverse32(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		int oldWidth;
		tupImageDataMap.GetWidth(oldWidth);
		int oldHeight;
		tupImageDataMap.GetHeight(oldHeight);
		int newWidth = oldHeight;
		int newHeight = oldWidth;

		u32 *pOldData;
		int numBytes;

		tupImageDataMap.GetData((void **)&pOldData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);
		u32 *pNewData = new u32[numPixels];

		int x,y,yNewOffset,yOldOffset;
		// copy from top left going down and to the right to
		// top left going right and to the bottom
		for (y=0,yNewOffset=0;y<newHeight;y++,yNewOffset+=newWidth)
		{
			for (x=0,yOldOffset=0;x<newWidth;x++,yOldOffset+=oldWidth)
			{
				pNewData[yNewOffset+x] = pOldData[yOldOffset+y];
			}
		}
		tupImageDataMap.AddData((void *)pNewData,numBytes);
		tupImageDataMap.AddWidth(newWidth);
		tupImageDataMap.AddHeight(newHeight);
		delete pNewData;
	}
}

void ImageUtilities::Reverse24(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		int oldWidth;
		tupImageDataMap.GetWidth(oldWidth);
		int oldHeight;
		tupImageDataMap.GetHeight(oldHeight);
		int newWidth = oldHeight;
		int newHeight = oldWidth;

		u8 *pOldData;
		int numBytes;

		tupImageDataMap.GetData((void **)&pOldData,numBytes);
		int numPixels = numBytes/3;
		assert((numPixels*3)==numBytes);
		u8 *pNewData = new u8[numBytes];

		int x,y,yNewOffset,yOldOffset,y3;
		// copy from top left going down and to the right to
		// top left going right and to the bottom
		int newWidth3 = (newWidth<<1)+newWidth;
		int oldWidth3 = (oldWidth<<1)+oldWidth;
		for (y=0,y3=0,yNewOffset=0;y<newHeight;y++,y3+=3,yNewOffset+=newWidth3)
		{
			for (x=0,yOldOffset=0;x<newWidth3;x+=3,yOldOffset+=oldWidth3)
			{
				pNewData[yNewOffset+x] = pOldData[yOldOffset+y3];
				pNewData[yNewOffset+x+1] = pOldData[yOldOffset+y3+1];
				pNewData[yNewOffset+x+2] = pOldData[yOldOffset+y3+2];
			}
		}
		tupImageDataMap.AddData((void *)pNewData,numBytes);
		tupImageDataMap.AddWidth(newWidth);
		tupImageDataMap.AddHeight(newHeight);
		delete pNewData;
	}
}

void ImageUtilities::Reverse16(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		int oldWidth;
		tupImageDataMap.GetWidth(oldWidth);
		int oldHeight;
		tupImageDataMap.GetHeight(oldHeight);
		int newWidth = oldHeight;
		int newHeight = oldWidth;

		u16 *pOldData;
		int numBytes;

		tupImageDataMap.GetData((void **)&pOldData,numBytes);
		int numPixels = numBytes>>1;
		assert((numPixels<<1)==numBytes);
		u16 *pNewData = new u16[numPixels];

		int x,y,yNewOffset,yOldOffset;
		// copy from top left going down and to the right to
		// top left going right and to the bottom
		for (y=0,yNewOffset=0;y<newHeight;y++,yNewOffset+=newWidth)
		{
			for (x=0,yOldOffset=0;x<newWidth;x++,yOldOffset+=oldWidth)
			{
				pNewData[yNewOffset+x] = pOldData[yOldOffset+y];
			}
		}
		tupImageDataMap.AddData((void *)pNewData,numBytes);
		tupImageDataMap.AddWidth(newWidth);
		tupImageDataMap.AddHeight(newHeight);
		delete pNewData;
	}
}

void ImageUtilities::Reverse8(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		int oldWidth;
		tupImageDataMap.GetWidth(oldWidth);
		int oldHeight;
		tupImageDataMap.GetHeight(oldHeight);
		int newWidth = oldHeight;
		int newHeight = oldWidth;

		u8 *pOldData;
		int numBytes;

		tupImageDataMap.GetData((void **)&pOldData,numBytes);
		u8 *pNewData = new u8[numBytes];

		int x,y,yNewOffset,yOldOffset;
		// copy from top left going down and to the right to
		// top left going right and to the bottom
		for (y=0,yNewOffset=0;y<newHeight;y++,yNewOffset+=newWidth)
		{
			for (x=0,yOldOffset=0;x<newWidth;x++,yOldOffset+=oldWidth)
			{
				pNewData[yNewOffset+x] = pOldData[yOldOffset+y];
			}
		}
		tupImageDataMap.AddData((void *)pNewData,numBytes);
		tupImageDataMap.AddWidth(newWidth);
		tupImageDataMap.AddHeight(newHeight);
		delete pNewData;
	}
}

void ImageUtilities::Reverse4(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
		TupImageDataMap tupImageDataMap(pMapAgg);

		int oldWidth;
		tupImageDataMap.GetWidth(oldWidth);
		int oldHeight;
		tupImageDataMap.GetHeight(oldHeight);
		int newWidth = oldHeight;
		int newHeight = oldWidth;

		u8 *pOldData;
		int numBytes;

		tupImageDataMap.GetData((void **)&pOldData,numBytes);
		u8 *pNewData = new u8[numBytes];

		int x,y,yNewOffset,yOldOffset;
		int newWidthHalf=newWidth>>1;
		int oldWidthHalf=oldWidth>>1;
		// copy from top left going down and to the right to
		// top left going right and to the bottom
		for (y=0,yNewOffset=0;y<newHeight;y++,yNewOffset+=newWidthHalf)
		{
			for (x=0,yOldOffset=0;x<newWidthHalf;x++,yOldOffset+=oldWidth)
			{
				// if source is msbits
				if (y&1)
				{
					// both left and right pixels are in the msbits of two bytes in the source
					pNewData[yNewOffset+x] = ((pOldData[yOldOffset+(y>>1)]&0xf0)>>4)|((pOldData[(yOldOffset+oldWidthHalf)+(y>>1)]&0xf0));
				}
				else
				{
					// both left and right pixels are in the lsbits of two bytes in the source
					pNewData[yNewOffset+x] = (pOldData[yOldOffset+(y>>1)]&0x0f)|((pOldData[(yOldOffset+oldWidthHalf)+(y>>1)]&0x0f)<<4);
				}
			}
		}
		tupImageDataMap.AddData((void *)pNewData,numBytes);
		tupImageDataMap.AddWidth(newWidth);
		tupImageDataMap.AddHeight(newHeight);
		delete pNewData;
	}
}

void ImageUtilities::ConvertLuminanceToAlpha(TupImageData &tupImageData)
{
	// start with 32 bit image
	ConvertToFormat(tupImageData,TupImageDataFrame::RGBA_8888);
	int numFrames = tupImageData.GetNumImageDataFrames();
	for (int frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		assert(pFrameAgg);
		TupImageDataFrame tupImageDataFrame(pFrameAgg);
		
		int numMaps = tupImageDataFrame.GetNumImageDataMaps();

		// set up the pallette to be a bunch of white colors with different alpha
		PaletteGenerator::RgbaColor newPalette[256];
		for (int paletteIndex=0;paletteIndex<256;paletteIndex++)
		{
			newPalette[paletteIndex].r = 255;
			newPalette[paletteIndex].g = 255;
			newPalette[paletteIndex].b = 255;
			newPalette[paletteIndex].a = (unsigned char) paletteIndex;
		}

		for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
		{
			TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
			TupImageDataMap tupImageDataMap(pMapAgg);

			u32 *pData;
			int numBytes;

			// convert 4 bytes per pixel to 1
			tupImageDataMap.GetData((void **)&pData,numBytes);
			int numPixels = numBytes>>2;
			assert((numPixels<<2)==numBytes);
			u8 *pNewData = new u8[numPixels];



			// take the remap values to new palette
			for (int pixelIndex=0;pixelIndex<numPixels;pixelIndex++)
			{
				u32 colorValue = pData[pixelIndex];

				int dataRed = colorValue&0xff;
				int dataGreen = (colorValue>>8)&0xff;
				int dataBlue = (colorValue>>16)&0xff;
				// index into palette is the luminance of the color
				pNewData[pixelIndex] = ((u8) ((0.299f * (float) dataRed) + (0.587f * (float) dataGreen) + (0.114f * (float) dataBlue))); 
			}
			// replace data with new data
			tupImageDataMap.AddData(pNewData,numPixels);
			delete pNewData;
		}
		// set the new format
		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);
		formatFlags |= (TupImageDataFrame::HAS_ALPHA | TupImageDataFrame::HAS_8BITALPHA);
		formatFlags = (formatFlags & ~TupImageDataFrame::FORMAT_MASK)|TupImageDataFrame::INDEXED_8;
		tupImageDataFrame.AddFormatFlags(formatFlags);
		// add in the palette
		TupperwareAggregate *tupImagePalette = tupImageDataFrame.AddImageDataPalette();
		TupImageDataPalette tupImageDataPalette(tupImagePalette);
		tupImageDataPalette.AddData(newPalette,1024);
	}
}

int ImageUtilities::BuildMipMaps(TupImageData &tupImageData,int numSubMaps)
{
	TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(0);
	assert(pFrameAgg);
	TupImageDataFrame tupImageDataFrame(pFrameAgg);
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	if (numMaps!=1)
		return 0; // don't do if already done

	TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(0);
	TupImageDataMap tupImageDataMap(pMapAgg);
	int width,height;
	tupImageDataMap.GetWidth(width);
	tupImageDataMap.GetHeight(height);

	int minSize = 8;

	int numGeneratedSubMaps = 0;
	int scaleWidth = width>>1;
	int scaleHeight = height>>1;
	if (numSubMaps) // requested amount
	{
		while (scaleWidth>=minSize && scaleHeight>=minSize && numSubMaps)
		{
			scaleWidth = scaleWidth>>1;
			scaleHeight = scaleHeight>>1;
			numSubMaps--;
			numGeneratedSubMaps++;
		}
	}
	else // do as many as possible
	{
		while (scaleWidth>=minSize && scaleHeight>=minSize)
		{
			scaleWidth = scaleWidth>>1;
			scaleHeight = scaleHeight>>1;
			numGeneratedSubMaps++;
		}
	}
	// if none to do then return
	if (!numGeneratedSubMaps)
		return 0;
	
	int numFrames = tupImageData.GetNumImageDataFrames();
	for (int frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);
		int numMaps = tupImageDataFrame.GetNumImageDataMaps();
		
		switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
		case TupImageDataFrame::RGBA_8888:
			break;
		case TupImageDataFrame::RGB_888:
			Convert888to8888(tupImageDataFrame);
			break;
		case TupImageDataFrame::RGB_565:
			Convert565to8888(tupImageDataFrame);
			break;
		case TupImageDataFrame::RGB_555:
			Convert555to8888(tupImageDataFrame);
			break;
		case TupImageDataFrame::RGBA_1555:
			Convert1555to8888(tupImageDataFrame);
			break;
		case TupImageDataFrame::INDEXED_8:
			Convert8to8888(tupImageDataFrame);
			break;
		case TupImageDataFrame::INDEXED_4:
			Convert4to8888(tupImageDataFrame);
			break;
		}

		scaleWidth = width;
		scaleHeight = height;
		for (int subMapIndex=0;subMapIndex<numGeneratedSubMaps;subMapIndex++)
		{
			tupImageDataFrame.AddImageDataMap(subMapIndex+1);
			scaleWidth = scaleWidth>>1;
			scaleHeight = scaleHeight>>1;
			Scale8888(tupImageDataFrame,scaleWidth,scaleHeight,0,subMapIndex+1);
		}

		switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
				break;
			case TupImageDataFrame::RGB_888:
				Convert8888to888(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_565:
				Convert8888to565(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_555:
				Convert8888to555(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGBA_1555:
				Convert8888to1555(tupImageDataFrame);
				break;
			case TupImageDataFrame::INDEXED_8:
				Convert8888to8(tupImageDataFrame);
				break;
			case TupImageDataFrame::INDEXED_4:
				Convert8888to4(tupImageDataFrame);
				break;
		}
	}
	return numGeneratedSubMaps;
}

void ImageUtilities::MarkAsFixTexture(TupImageData &tupImageData)
{
	int numFrames = tupImageData.GetNumImageDataFrames();
	for (int frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		assert(pFrameAgg);
		TupImageDataFrame tupImageDataFrame(pFrameAgg);

		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);

		switch(formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
				MarkAsFixTexture8888(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_888:
				Convert888to8888(tupImageDataFrame);
				MarkAsFixTexture8888(tupImageDataFrame);
				Convert8888to888(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_565:
				Convert565to8888(tupImageDataFrame);
				MarkAsFixTexture8888(tupImageDataFrame);
				Convert8888to565(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGB_555:
				Convert555to8888(tupImageDataFrame);
				MarkAsFixTexture8888(tupImageDataFrame);
				Convert8888to555(tupImageDataFrame);
				break;
			case TupImageDataFrame::RGBA_1555:
				Convert1555to8888(tupImageDataFrame);
				MarkAsFixTexture8888(tupImageDataFrame);
				Convert8888to1555(tupImageDataFrame);
				break;
			case TupImageDataFrame::INDEXED_8:
				Convert8to8888(tupImageDataFrame);
				MarkAsFixTexture8888(tupImageDataFrame);
				Convert8888to8(tupImageDataFrame);
				break;
			case TupImageDataFrame::INDEXED_4:
				Convert4to8888(tupImageDataFrame);
				MarkAsFixTexture8888(tupImageDataFrame);
				Convert8888to4(tupImageDataFrame);
				break;
		}
	}
}

void ImageUtilities::MarkAsFixTexture8888(TupImageDataFrame &tupImageDataFrame)
{
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);

		assert(pMapAgg); // source better be there
		TupImageDataMap tupImageDataMap(pMapAgg);

		int width;
		int height;

		tupImageDataMap.GetWidth(width);
		tupImageDataMap.GetHeight(height);

		u32 *pData;
		int numBytes;

		// convert 4 bytes per pixel to 2
		tupImageDataMap.GetData((void **)&pData,numBytes);
		int numPixels = numBytes>>2;
		assert((numPixels<<2)==numBytes);

		u32 color = 0xffff00ff;

		int x,y;

		// every other pixel is pink
		int yLine = 0;
		for (y=0;y<height;y++,yLine+=width)
		{
			for (x=0;x<width;x++)
			{

				if (((x^y)&7)==0 || ((x^y)&7)==7)
				{
					pData[yLine+x] = color;
				}
			}
		}
	}
}
