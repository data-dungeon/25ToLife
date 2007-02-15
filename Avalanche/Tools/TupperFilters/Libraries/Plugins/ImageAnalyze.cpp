////////////////////////////////////////////////////////////////////////////
//
// ImageAnalyze
//
// Classes to handle analyzing the frames of an TupImageData object and collect stats
//
////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "ImageAnalyze.h"

void ImageAnalyze::AddImageData(TupImageData &tupImageData)
{
	int totalFrames = tupImageData.GetNumImageDataFrames();
	for (int frameIndex = 0;frameIndex<totalFrames;frameIndex++)
	{
		ImageFrameStats imageFrameStats;

		TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		assert(pFrameAgg);
		TupImageDataFrame tupImageDataFrame(pFrameAgg);
		tupImageDataFrame.GetFormatFlags(imageFrameStats.m_formatFlags);

		TupperwareAggregate *pMapAgg = tupImageDataFrame.GetImageDataMap(0);
		assert(pMapAgg);
		TupImageDataMap tupImageDataMap(pMapAgg);
		tupImageDataMap.GetWidth(imageFrameStats.m_width);
		tupImageDataMap.GetHeight(imageFrameStats.m_height);

		switch (imageFrameStats.m_formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
				AnalyzeImage8888(tupImageDataMap,imageFrameStats);
				break;
			case TupImageDataFrame::RGB_888:
				AnalyzeImage888(tupImageDataMap,imageFrameStats);
				break;
			case TupImageDataFrame::RGB_555:
				AnalyzeImage555(tupImageDataMap,imageFrameStats);
				break;
			case TupImageDataFrame::RGB_565:
				AnalyzeImage565(tupImageDataMap,imageFrameStats);
				break;
			case TupImageDataFrame::RGBA_1555:
				AnalyzeImage1555(tupImageDataMap,imageFrameStats);
				break;
			case TupImageDataFrame::INDEXED_8:
				{
					TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
					assert(pPaletteAgg);
					TupImageDataPalette tupImageDataPalette(pPaletteAgg);
					AnalyzeImage8(tupImageDataMap,tupImageDataPalette,imageFrameStats);
				}
				break;
			case TupImageDataFrame::INDEXED_4:
				{
					TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
					assert(pPaletteAgg);
					TupImageDataPalette tupImageDataPalette(pPaletteAgg);
					AnalyzeImage4(tupImageDataMap,tupImageDataPalette,imageFrameStats);
				}
				break;
		}
		m_frameStats.Add(imageFrameStats);
	}
}

void ImageAnalyze::AnalyzeImage8888(TupImageDataMap &tupImageDataMap,ImageAnalyze::ImageFrameStats &imageFrameStats)
{
	Keeper<u32> colorPalette(4096);
	Keeper<BYTE> alphaPalette;

	u32 *pData;
	int numBytes;
	tupImageDataMap.GetData((void **)&pData,numBytes);
	int numPixels = numBytes>>2; // 4 bytes per pixel

	imageFrameStats.m_minAlphaValue = 0xff;
	imageFrameStats.m_maxAlphaValue = 0x00;

	// find out how many colors and alpha values are used
	while (numPixels)
	{
		colorPalette.Add(*pData);
		BYTE alphaValue = (BYTE)(*pData>>24);
		if (alphaValue<imageFrameStats.m_minAlphaValue)
			imageFrameStats.m_minAlphaValue = alphaValue;
		if (alphaValue>imageFrameStats.m_maxAlphaValue)
			imageFrameStats.m_maxAlphaValue = alphaValue;
		alphaPalette.Add(alphaValue);
		pData++;
		numPixels--;
	}
	imageFrameStats.m_colorsUsed = colorPalette.GetSize();
	imageFrameStats.m_alphaUsed = alphaPalette.GetSize();
}

void ImageAnalyze::AnalyzeImage888(TupImageDataMap &tupImageDataMap,ImageAnalyze::ImageFrameStats &imageFrameStats)
{
	Keeper<u32> colorPalette(4096);

	BYTE *pData;
	int numBytes;
	tupImageDataMap.GetData((void **)&pData,numBytes);

	imageFrameStats.m_minAlphaValue = 0xff;
	imageFrameStats.m_maxAlphaValue = 0xff;

	// find out how many colors and alpha values are used
	while (numBytes)
	{
		u32 color = (u32)pData[0] | ((u32)pData[1]<<8) | ((u32)pData[2]<<16);
		colorPalette.Add(color);
		pData+=3;
		numBytes-=3;
	}
	imageFrameStats.m_colorsUsed = colorPalette.GetSize();
	imageFrameStats.m_alphaUsed = 0;
}

void ImageAnalyze::AnalyzeImage555(TupImageDataMap &tupImageDataMap,ImageAnalyze::ImageFrameStats &imageFrameStats)
{
	Keeper<u16> colorPalette(1024);

	u16 *pData;
	int numBytes;
	tupImageDataMap.GetData((void **)&pData,numBytes);
	int numPixels = numBytes>>1; // 2 bytes per pixel

	imageFrameStats.m_minAlphaValue = 0xff;
	imageFrameStats.m_maxAlphaValue = 0xff;

	// find out how many colors and alpha values are used
	while (numPixels)
	{
		colorPalette.Add(*pData);
		pData++;
		numPixels--;
	}
	imageFrameStats.m_colorsUsed = colorPalette.GetSize();
	imageFrameStats.m_alphaUsed = 0;
}

void ImageAnalyze::AnalyzeImage565(TupImageDataMap &tupImageDataMap,ImageAnalyze::ImageFrameStats &imageFrameStats)
{
	Keeper<u16> colorPalette(1024);

	u16 *pData;
	int numBytes;
	tupImageDataMap.GetData((void **)&pData,numBytes);
	int numPixels = numBytes>>1; // 2 bytes per pixel

	imageFrameStats.m_minAlphaValue = 0xff;
	imageFrameStats.m_maxAlphaValue = 0xff;

	// find out how many colors and alpha values are used
	while (numPixels)
	{
		colorPalette.Add(*pData);
		pData++;
		numPixels--;
	}
	imageFrameStats.m_colorsUsed = colorPalette.GetSize();
	imageFrameStats.m_alphaUsed = 0;
}

void ImageAnalyze::AnalyzeImage1555(TupImageDataMap &tupImageDataMap,ImageAnalyze::ImageFrameStats &imageFrameStats)
{
	Keeper<u16> colorPalette(1024);
	Keeper<u16> alphaPalette;

	u16 *pData;
	int numBytes;
	tupImageDataMap.GetData((void **)&pData,numBytes);
	int numPixels = numBytes>>1; // 2 bytes per pixel

	imageFrameStats.m_minAlphaValue = 0xff;
	imageFrameStats.m_maxAlphaValue = 0x00;

	// find out how many colors and alpha values are used
	while (numPixels)
	{
		colorPalette.Add(*pData);
		alphaPalette.Add((*pData)&0x8000);
		BYTE alphaValue = ((*pData)&0x8000) ? 0xff : 0x00;
		if (alphaValue<imageFrameStats.m_minAlphaValue)
			imageFrameStats.m_minAlphaValue = alphaValue;
		if (alphaValue>imageFrameStats.m_maxAlphaValue)
			imageFrameStats.m_maxAlphaValue = alphaValue;

		pData++;
		numPixels--;
	}
	imageFrameStats.m_colorsUsed = colorPalette.GetSize();
	imageFrameStats.m_alphaUsed = alphaPalette.GetSize();
}

void ImageAnalyze::AnalyzeImage8(TupImageDataMap &tupImageDataMap,TupImageDataPalette &tupImageDataPalette,ImageAnalyze::ImageFrameStats &imageFrameStats)
{
	Keeper<BYTE> colorPalette;
	Keeper<BYTE> alphaPalette;

	BYTE *pData;
	int numBytes;
	tupImageDataMap.GetData((void **)&pData,numBytes);
	u32 *pPaletteData;
	int numPaletteBytes;
	tupImageDataPalette.GetData((void **)&pPaletteData,numPaletteBytes);
	assert(numPaletteBytes==1024);

	imageFrameStats.m_minAlphaValue = 0xff;
	imageFrameStats.m_maxAlphaValue = 0x00;

	// find out how many colors and alpha values are used
	while (numBytes)
	{
		colorPalette.Add(*pData);
		BYTE alphaValue = (BYTE)(pPaletteData[*pData]>>24);
		if (alphaValue<imageFrameStats.m_minAlphaValue)
			imageFrameStats.m_minAlphaValue = alphaValue;
		if (alphaValue>imageFrameStats.m_maxAlphaValue)
			imageFrameStats.m_maxAlphaValue = alphaValue;

		alphaPalette.Add(alphaValue);
		pData++;
		numBytes--;
	}
	imageFrameStats.m_colorsUsed = colorPalette.GetSize();
	imageFrameStats.m_alphaUsed = alphaPalette.GetSize();
}

void ImageAnalyze::AnalyzeImage4(TupImageDataMap &tupImageDataMap,TupImageDataPalette &tupImageDataPalette,ImageAnalyze::ImageFrameStats &imageFrameStats)
{
	Keeper<BYTE> colorPalette;
	Keeper<BYTE> alphaPalette;

	BYTE *pData;
	int numBytes;
	tupImageDataMap.GetData((void **)&pData,numBytes);
	u32 *pPaletteData;
	int numPaletteBytes;
	tupImageDataPalette.GetData((void **)&pPaletteData,numPaletteBytes);
	assert(numPaletteBytes==64);

	imageFrameStats.m_minAlphaValue = 0xff;
	imageFrameStats.m_maxAlphaValue = 0x00;

	// find out how many colors and alpha values are used
	while (numBytes)
	{
		colorPalette.Add((*pData)&0xf);
		BYTE alphaValue;
		alphaValue = (BYTE)(pPaletteData[(*pData)&0xf]>>24);

		if (alphaValue<imageFrameStats.m_minAlphaValue)
			imageFrameStats.m_minAlphaValue = alphaValue;
		if (alphaValue>imageFrameStats.m_maxAlphaValue)
			imageFrameStats.m_maxAlphaValue = alphaValue;
		alphaPalette.Add(alphaValue);
		colorPalette.Add((*pData)>>4);
		alphaValue = (BYTE)(pPaletteData[(*pData)>>4]>>24);
		if (alphaValue<imageFrameStats.m_minAlphaValue)
			imageFrameStats.m_minAlphaValue = alphaValue;
		if (alphaValue>imageFrameStats.m_maxAlphaValue)
			imageFrameStats.m_maxAlphaValue = alphaValue;
		alphaPalette.Add(alphaValue);
		pData++;
		numBytes--;
	}
	imageFrameStats.m_colorsUsed = colorPalette.GetSize();
	imageFrameStats.m_alphaUsed = alphaPalette.GetSize();
}

	struct FinalStats
	{
		int m_numMaps;
		int m_maxColorsUsed;
		int m_minColorsUsed;
		int m_maxAlphaUsed;
		int m_minAlphaUsed;
		int m_maxWidthUsed;
		int m_minWidthUsed;
		int m_maxHeightUsed;
		int m_minHeightUsed;
	};

void ImageAnalyze::CalculateStats(ImageAnalyze::Stats &stats)
{
	stats.m_numMaps = m_frameStats.GetSize();

	stats.m_maxColorsUsed = -1;
	stats.m_minColorsUsed = -1;
	stats.m_maxAlphaUsed = -1;
	stats.m_minAlphaUsed = -1;
	stats.m_maxAlphaValueUsed = 0x00;
	stats.m_minAlphaValueUsed = 0xff;
	stats.m_maxWidthUsed = -1;
	stats.m_minWidthUsed = -1;
	stats.m_maxHeightUsed = -1;
	stats.m_minHeightUsed = -1;

	for (int mapIndex=0;mapIndex<stats.m_numMaps;mapIndex++)
	{
		const ImageFrameStats &frameStats = m_frameStats[mapIndex];
		if (stats.m_maxColorsUsed==-1 || stats.m_maxColorsUsed<frameStats.m_colorsUsed)
			stats.m_maxColorsUsed = frameStats.m_colorsUsed;
		if (stats.m_minColorsUsed==-1 || stats.m_minColorsUsed>frameStats.m_colorsUsed)
			stats.m_minColorsUsed = frameStats.m_colorsUsed;
		if (stats.m_maxAlphaUsed==-1 || stats.m_maxAlphaUsed<frameStats.m_alphaUsed)
			stats.m_maxAlphaUsed = frameStats.m_alphaUsed;
		if (stats.m_minAlphaUsed==-1 || stats.m_minAlphaUsed>frameStats.m_alphaUsed)
			stats.m_minAlphaUsed = frameStats.m_alphaUsed;
		if (stats.m_maxAlphaValueUsed<frameStats.m_maxAlphaValue)
			stats.m_maxAlphaValueUsed = frameStats.m_maxAlphaValue;
		if (stats.m_minAlphaValueUsed>frameStats.m_minAlphaValue)
			stats.m_minAlphaValueUsed = frameStats.m_minAlphaValue;
		if (stats.m_maxWidthUsed==-1 || stats.m_maxWidthUsed<frameStats.m_width)
			stats.m_maxWidthUsed = frameStats.m_width;
		if (stats.m_minWidthUsed==-1 || stats.m_minWidthUsed>frameStats.m_width)
			stats.m_minWidthUsed = frameStats.m_width;
		if (stats.m_maxHeightUsed==-1 || stats.m_maxHeightUsed<frameStats.m_height)
			stats.m_maxHeightUsed = frameStats.m_height;
		if (stats.m_minHeightUsed==-1 || stats.m_minHeightUsed>frameStats.m_height)
			stats.m_minHeightUsed = frameStats.m_height;

	}
	
}

