////////////////////////////////////////////////////////////////////////////
//
// ImageAnalyze
//
// Classes to handle analyzing the frames of an TupImageData object and collect stats
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_IMAGE_ANALYZE_H
#define TUPPERFILTERS_LIB_PLUGINS_IMAGE_ANALYZE_H

/* system includes */
class ImageAnalyze
{
	struct ImageFrameStats
	{
		int m_formatFlags;
		int m_colorsUsed;
		int m_alphaUsed;
		BYTE m_minAlphaValue;
		BYTE m_maxAlphaValue;
		int m_width;
		int m_height;
	};

	TupArray<ImageFrameStats> m_frameStats;

public:

	struct Stats
	{
		int m_numMaps;
		int m_maxColorsUsed;
		int m_minColorsUsed;
		int m_maxAlphaUsed;
		int m_minAlphaUsed;
		BYTE m_minAlphaValueUsed;
		BYTE m_maxAlphaValueUsed;
		int m_maxWidthUsed;
		int m_minWidthUsed;
		int m_maxHeightUsed;
		int m_minHeightUsed;
	};

	void AddImageData(TupImageData &tupImageData);
	void CalculateStats(Stats &stats);

private:
	void AnalyzeImage8888(TupImageDataMap &tupImageDataMap,ImageFrameStats &imageFrameStats);
	void AnalyzeImage888(TupImageDataMap &tupImageDataMap,ImageFrameStats &imageFrameStats);
	void AnalyzeImage555(TupImageDataMap &tupImageDataMap,ImageFrameStats &imageFrameStats);
	void AnalyzeImage565(TupImageDataMap &tupImageDataMap,ImageFrameStats &imageFrameStats);
	void AnalyzeImage1555(TupImageDataMap &tupImageDataMap,ImageFrameStats &imageFrameStats);
	void AnalyzeImage8(TupImageDataMap &tupImageDataMap,TupImageDataPalette &tupImageDataPalette,ImageFrameStats &imageFrameStats);
	void AnalyzeImage4(TupImageDataMap &tupImageDataMap,TupImageDataPalette &tupImageDataPalette,ImageFrameStats &imageFrameStats);
};

#endif

