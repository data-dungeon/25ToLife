////////////////////////////////////////////////////////////////////////////
//
// PaletteGenerator
//
// Class to handle creating a palette from a set of colors
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: PaletteGenerator.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/25/02    Time: 4:43p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * First phase for texture animation
 * Analysis tools, utilities and palette generator
 */

#ifndef TUPPERFILTERS_LIB_PLUGINS_PALETTE_GENERATOR_H
#define TUPPERFILTERS_LIB_PLUGINS_PALETTE_GENERATOR_H

static const int MEAN_CUT = 1;	//1=meanCut, 0=medianCut
static const int USE_VARIANCE = 1;	//1=variance, 0=spread

static const int MAX_PALETTE_SIZE = 256;
static const int COLOR_INDEX_MIN = 0;
static const int COLOR_INDEX_MAX = 255;
static const int TOTAL_COLORS = 256;

static const int NUM_HASH_BINS = 0x4000;
static const int HASH_MASK = 0x3fff;

static const int MAX_HASH_VALUES = 2000000;

static const int NUM_COLOR_AXIS = 4;

/***************************************/
/***** Generate Palette Definition *****/
/***************************************/
class PaletteGenerator
{
public:
	struct RgbStat
	{
		long freqency;
		unsigned char r,g,b,a;
	};

	struct RgbHash
	{
		unsigned long index;
		unsigned long next;
	};

	struct RgbVar
	{
		float v[4];
	};

	struct RgbaColor
	{   
		unsigned char	r;
		unsigned char	g;
		unsigned char	b;
		unsigned char	a;
	};

private:
	RgbStat *m_pRGBStat;
	unsigned long m_rgbStatIndex,m_hashIndex;
	RgbHash *m_pHashTable;
	unsigned long *m_pHashHeader;
	bool m_bRGBStatFull;

	void GetPalVar(RgbStat *pStat,unsigned long *pIndexHdr,RgbHash *pIndx,unsigned int node,RgbVar *pVar);
	void RgbStatGamma(RgbStat *pStatIn,int nColors,float gamma,RgbStat *pStatOut);

public:
	PaletteGenerator (void);
	~PaletteGenerator (void);

	//Routines to accumulate RGB stats
	void InitRGBStats (void);
	void ResetRGBStats (void);
	int GetRGBStatColors (void);
	bool GetRGBStats (unsigned long *pData,int size,int weight);
						 
	void GeneratePalette(RgbaColor *pPal,unsigned int palLen,float palGamma);
};

#endif	

