////////////////////////////////////////////////////////////////////////////
//
// PaletteGenerator
//
// Class to handle creating a palette from a set of colors
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: PaletteGenerator.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 7/26/02    Time: 12:57p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Fixed various bugs with byte orders, etc
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/25/02    Time: 4:43p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * First phase for texture animation
 * Analysis tools, utilities and palette generator
 */

#include "PluginsPCH.h"
#include "PaletteGenerator.h"
#include "PluginLog.h"

static int __cdecl PaletteCompare(const void *aPass,const void *bPass);
static void RGBToHSV(float r,float g,float b,float &h,float &s,float &v);
static void GetGammaTbl (float gamma,unsigned char *lumTbl);

PaletteGenerator::PaletteGenerator (void)
{
	m_pHashTable = NULL;
	m_pHashHeader = NULL;
	m_pRGBStat = NULL;
	m_bRGBStatFull = false;
}

PaletteGenerator::~PaletteGenerator (void)
{
	delete m_pHashTable;
	delete m_pHashHeader;
	delete m_pRGBStat;
}

void PaletteGenerator::InitRGBStats (void)
{
	if (m_pHashTable)
		return;	//return if already allocated

	m_pHashTable = new RgbHash[MAX_HASH_VALUES];
	m_pHashHeader = new unsigned long[NUM_HASH_BINS];
	m_pRGBStat = new RgbStat[MAX_HASH_VALUES];

	ResetRGBStats();
}

void PaletteGenerator::ResetRGBStats (void)
{
unsigned int i;

	for (i=0; i<NUM_HASH_BINS; i++)
		m_pHashHeader[i] = 0;

	for (i=0; i<MAX_HASH_VALUES; i++)
		m_pRGBStat[i].freqency = 0;

	m_rgbStatIndex = 0;
	m_hashIndex = 1;
	m_bRGBStatFull = false;
}

int PaletteGenerator::GetRGBStatColors (void)
{
	return((int)m_rgbStatIndex);
}

bool PaletteGenerator::GetRGBStats(unsigned long *data,int size,int weight)
{
	if ( m_bRGBStatFull ) 
		return true;

	for (int i=0;i<size;i++)
	{
		int r = data[i]&0xff;
		int g = (data[i]>>8)&0xff;
		int b = (data[i]>>16)&0xff;
		int a = (data[i]>>24)&0xff;
		//get hash index
		//hash value is 14 bits
		int hashValue = ((r<<6) | (g<<4) | (b<<2) | (a))&HASH_MASK;
		
		//Search using hash table
		int found=0;
		unsigned int entry=m_pHashHeader[hashValue];
		unsigned int foundEntry;
		if (entry!=0 )
		{
			do
			{
				if ((m_pRGBStat[m_pHashTable[entry].index].r==r) &&
					(m_pRGBStat[m_pHashTable[entry].index].g==g) &&
					(m_pRGBStat[m_pHashTable[entry].index].b==b) &&
					(m_pRGBStat[m_pHashTable[entry].index].a==a) )
				{
					m_pRGBStat[m_pHashTable[entry].index].freqency+=weight;
					if (m_pRGBStat[m_pHashTable[entry].index].freqency>10000)
						m_pRGBStat[m_pHashTable[entry].index].freqency = 10000;
					found++;
				}
				foundEntry = entry;
				entry = m_pHashTable[entry].next;
			}
			while ( entry && !found );

			if ( !found )
				found--;	//need to allocate new element of current node
		}
		if ( found <= 0 )
		{
			if ( (m_rgbStatIndex<MAX_HASH_VALUES-1) )	//overflow???
			{
				m_pHashTable[m_hashIndex].next = 0;	//terminate list
				m_pRGBStat[m_rgbStatIndex].freqency = weight;
				if (m_pRGBStat[m_rgbStatIndex].freqency>10000)
					m_pRGBStat[m_rgbStatIndex].freqency = 10000;
				m_pRGBStat[m_rgbStatIndex].r = (unsigned char) r;
				m_pRGBStat[m_rgbStatIndex].g = (unsigned char) g;
				m_pRGBStat[m_rgbStatIndex].b = (unsigned char) b;
				m_pRGBStat[m_rgbStatIndex].a = (unsigned char) a;
				m_pHashTable[m_hashIndex].index = m_rgbStatIndex++;

				if ( found == 0 )
					m_pHashHeader[hashValue] = m_hashIndex++;
				else
					m_pHashTable[foundEntry].next = m_hashIndex++;
			}
			else	//hash table full
			{
				m_bRGBStatFull = true;
				return true;
			}
		}
	}
	return false;
}

//| Y |   | .299  .587  .114 |   | R |
//| I | = | .701 -.587 -.114 | * | G |
//| Q |   |-.299 -.587  .886 |   | B |
int __cdecl PaletteCompare(const void *aPass,const void *bPass)
{
	float ha,sa,va;
	float hb,sb,vb;

	PaletteGenerator::RgbaColor *a = (PaletteGenerator::RgbaColor *)aPass;
	PaletteGenerator::RgbaColor *b = (PaletteGenerator::RgbaColor *)bPass;

	RGBToHSV((float)a->r,(float)a->g,(float)a->b,ha,sa,va);
	RGBToHSV((float)b->r,(float)b->g,(float)b->b,hb,sb,vb);

	int ai = (int)((ha*100.0f+sa)*256.0f+va);
	int bi = (int)((hb*100.0f+sb)*256.0f+vb);

	return( bi - ai);
}

void RGBToHSV(float r,float g,float b,float &h,float &s,float &v)
{
	float max = r>g ? r : g;
	max = max>b ? max : b;
	float min = r<g ? r : g;
	min = min<b ? min : b;
	// intensity is max one
	v = max;
	// calculate saturation
	if (max!=0)
		s = (max-min)/max;
	else
		s = 0.0f;
	if (!s)
	{
		h=0.0f;
	}
	else
	{
		float delta = max-min;
		if (r==max)
			h=(g-b)/delta;
		else if (g==max)
			h = 2 + (b-r)/delta;
		else if (b=max)
			h= 4 + (r-g)/delta;
		h*=60;
		if (h<0)
			h+=360;
	}
}

//Utilizes the mean-cut algorithm
//NOTE:  rgbStats must have been accumulated
//Note:  pointer 'm_pHashTable' must point to valid memory

void PaletteGenerator::GeneratePalette(RgbaColor *pal,unsigned int palLen,float palGamma)
{
	RgbVar *rgbVar;
	unsigned int i,j,maxNode,maxAxis,I0,nColors;
	long frqTot,lstLen;
	long lastIndx1,lastIndx2;
	double maxVar,meanY,meanI,meanQ,meanA;
	unsigned char *rgb1;
	RgbStat *stat2,*rgbStat2;

	//Check to see if total unique colors will fit in desired palette

	if ( m_rgbStatIndex<=palLen )
	{
		for (i=0; i<m_rgbStatIndex; i++)
		{
			pal[i].r = m_pRGBStat[i].r;
			pal[i].g = m_pRGBStat[i].g;
			pal[i].b = m_pRGBStat[i].b;
			pal[i].a = m_pRGBStat[i].a;
		}
		nColors = m_rgbStatIndex;
		//sort palette in ascending order according to alpha
		qsort(pal,nColors,sizeof(RgbaColor),PaletteCompare);
		return;
	}

	//Memory Allocations
	rgbVar = new RgbVar[palLen];
	rgbStat2 = new RgbStat[m_rgbStatIndex];

	//Init tree so all used stats included in single main node
	for (i=1; i<NUM_HASH_BINS; i++)
		m_pHashHeader[i] = 0;
	for (i=0; i<m_rgbStatIndex; i++)
		if ( m_pRGBStat[i].freqency > 0 )
		{
			m_pHashHeader[0] = (i+1);
			break;
		}
	if (!m_pHashHeader[0])
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"No usable stat nodes in PaletteGenerator.");
		return;
	}
	for (j=i+1,i=i+2; (i<MAX_HASH_VALUES)&&(i<=m_rgbStatIndex); i++)
	{
		if ( m_pRGBStat[i-1].freqency > 0 )
		{
			m_pHashTable[j].next = i;
			m_pHashTable[i].index= (i-1);
			j = i;	//previous node
		}
	}
	m_pHashTable[j].next = 0;	//terminate the list

	// Gamma correct luminance so brighter areas are not favored
	RgbStatGamma(m_pRGBStat,m_rgbStatIndex,palGamma,rgbStat2);
	//calculate initial variance for main node (node(0))
	GetPalVar(rgbStat2,m_pHashHeader,m_pHashTable,0,&(rgbVar[0]));

	for (nColors=1; nColors<palLen; nColors++)
	{
		//Search all color axis for largest variance
		for (i=0,maxVar=-1.0; i<nColors; i++)
		{
			for (j=0; j<NUM_COLOR_AXIS; j++)
			{
				if (rgbVar[i].v[j] > maxVar )
				{
					maxVar = rgbVar[i].v[j];
					maxNode = i;
					maxAxis = j;
				}
			}
		}
		if ( (I0=m_pHashHeader[maxNode])!=0 )
		{
			//calculate MEAN for axis to be cut
			frqTot=lstLen=0;
			meanY = 0.0;
			do
			{
				rgb1 = &(rgbStat2[m_pHashTable[I0].index].r);
				frqTot += rgbStat2[m_pHashTable[I0].index].freqency;
				meanY += rgb1[maxAxis] * rgbStat2[m_pHashTable[I0].index].freqency;
				lstLen++;
			}
			while ( ((I0=m_pHashTable[I0].next)!=0) );
			meanY /= (double)frqTot;
		}
		else
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"MaxNode undefined in PaletteGenerator.");
			return;
		}
		//check to make sure we can divide axis
		if ( lstLen <= 1 )
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Can no longer subdivide in PaletteGenerator.");
			palLen = nColors;
			break;
		}

		// cut colors on axis with largest variance
		do
		{
			lastIndx2=lastIndx1=-1;
			I0=m_pHashHeader[maxNode];
			do
			{
				rgb1 = &(rgbStat2[m_pHashTable[I0].index].r);
				if ( rgb1[maxAxis] > meanY )
				{
					if ( lastIndx2 < 0 )
						m_pHashHeader[nColors] = I0;	//point to initial node
					else
						m_pHashTable[lastIndx2].next= I0;
					lastIndx2 = I0;
				}
				else
				{
					if ( lastIndx1 < 0 )
						m_pHashHeader[maxNode] = I0; //point to initial node
					else
						m_pHashTable[lastIndx1].next= I0;
					lastIndx1 = I0;
				}
			}
			while ( ((I0=m_pHashTable[I0].next)!=0) );

			if ( lastIndx1>=0 )
				m_pHashTable[lastIndx1].next = 0;
			if (lastIndx2>=0 )
				m_pHashTable[lastIndx2].next = 0;	//terminate both lists
			meanY -= 1.0;
		}
		while ( (lastIndx1<0) || (lastIndx2<0) );

		//Recalculate variance for new & old node
		GetPalVar(rgbStat2,m_pHashHeader,m_pHashTable,nColors,&(rgbVar[nColors]));
		GetPalVar(rgbStat2,m_pHashHeader,m_pHashTable,maxNode,&(rgbVar[maxNode]));
	} 

	/* Find centroids of regions */
	for (nColors=0; nColors<palLen; nColors++)
	{
		frqTot = 0;
		meanY=meanI=meanQ=meanA = 0.0;
		if ( (I0=m_pHashHeader[nColors])!=0 )
		{
			do
			{
				stat2 = &(m_pRGBStat[m_pHashTable[I0].index]);
				if ( (lstLen=stat2->freqency) > 0 )
				{
					frqTot += lstLen;
					meanY += stat2->r * lstLen;
					meanI += stat2->g * lstLen;
					meanQ += stat2->b * lstLen;
					meanA += stat2->a * lstLen;
				}
				else
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Stat frq=%d while finding centroids in PaletteGenerator.",lstLen);
					return;
				}
			}
			while ( ((I0=m_pHashTable[I0].next)!=0) );
		}
		else
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"CNULL node encountered in trying to find centroids in PaletteGenerator.");
			return;
		}

		meanY /= (double)frqTot;
		meanI /= (double)frqTot;
		meanQ /= (double)frqTot;
		meanA /= (double)frqTot;

		//clip colors to fit in range
		if ( meanY > COLOR_INDEX_MAX ) meanY = COLOR_INDEX_MAX;
		else if (meanY < COLOR_INDEX_MIN ) meanY = COLOR_INDEX_MIN;
		if ( meanI > COLOR_INDEX_MAX )	meanI = COLOR_INDEX_MAX;
		else if (meanI < COLOR_INDEX_MIN ) meanI = COLOR_INDEX_MIN;
		if ( meanQ > COLOR_INDEX_MAX )	meanQ = COLOR_INDEX_MAX;
		else if (meanQ < COLOR_INDEX_MIN ) meanQ = COLOR_INDEX_MIN;
		if ( meanA > COLOR_INDEX_MAX )	meanA = COLOR_INDEX_MAX;
		else if (meanA < COLOR_INDEX_MIN ) meanA = COLOR_INDEX_MIN;
		pal[nColors].r = ((unsigned char) (meanY+.5))&0xff;
		pal[nColors].g = ((unsigned char) (meanI+.5))&0xff;
		pal[nColors].b = ((unsigned char) (meanQ+.5))&0xff;
		pal[nColors].a = (unsigned char) (meanA+.5);
	}

	//Free Memory
	delete rgbVar;
	delete rgbStat2;

	//sort palette in ascending order according to alpha
	qsort(pal,nColors,sizeof(RgbaColor),PaletteCompare);

}

void PaletteGenerator::RgbStatGamma(RgbStat *statIn,int nColors,float gamma,RgbStat *statOut)
{
	unsigned char lumTbl[TOTAL_COLORS];
	int pix,tmp,Yi,i;

	//init luminance remap table
	for (i=0; i<TOTAL_COLORS; i++) lumTbl[i] = (unsigned char) i;

	GetGammaTbl(gamma,lumTbl);

	for (i=0; i<nColors; i++)
	{
		if ( statIn!=statOut )
			statOut[i] = statIn[i];

		Yi = (int)(.299*statIn[i].r+.587*statIn[i].g+.114*statIn[i].b+.5);
		pix = lumTbl[Yi] - Yi;

		tmp = statIn[i].r + pix;
		if ( tmp > COLOR_INDEX_MAX ) tmp=COLOR_INDEX_MAX;
		else if ( tmp < COLOR_INDEX_MIN ) tmp=COLOR_INDEX_MIN;
		statOut[i].r = (unsigned char)tmp & 0xff;

		tmp = statIn[i].g + pix;
		if ( tmp > COLOR_INDEX_MAX ) tmp=COLOR_INDEX_MAX;
		else if ( tmp < COLOR_INDEX_MIN ) tmp=COLOR_INDEX_MIN;
		statOut[i].g = (unsigned char)tmp & 0xff;
		
		tmp = statIn[i].b + pix;
		if ( tmp > COLOR_INDEX_MAX ) tmp=COLOR_INDEX_MAX;
		else if ( tmp < COLOR_INDEX_MIN ) tmp=COLOR_INDEX_MIN;
		statOut[i].b = (unsigned char)tmp & 0xff;
	}  

}

void PaletteGenerator::GetPalVar (RgbStat *stat,unsigned long *indxHdr,RgbHash *indx,unsigned int node,RgbVar *var)
{
	float meanY,meanI,meanQ,meanA;
	RgbStat *stat2;
	unsigned int frqTot,I0;
	unsigned long frq;

	/* calc mean for each color axis */
	frqTot = 0;
	meanY = meanI = meanQ = meanA = 0.0;
	if ( (I0=indxHdr[node])!=0 )
	{
		do
		{
			stat2 = &(stat[indx[I0].index]);
			if ( (frq=stat2->freqency) > 0 )
			{
				frqTot += frq;
				meanY += stat2->r * frq;
				meanI += stat2->g * frq;
				meanQ += stat2->b * frq;
				meanA += stat2->a * frq;
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Stat frq=%d while finding centroids in PaletteGenerator.",frq);
				return;
			}
		}
		while ( ((I0=m_pHashTable[I0].next)!=0) );
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"node undefined in getPalVar");
		return;
	}

	meanY /= (float)frqTot;
	meanI /= (float)frqTot;
	meanQ /= (float)frqTot;
	meanA /= (float)frqTot;

	/* get variance for each color axis */
	var->v[0]=var->v[1]=var->v[2]=var->v[3] = 0.0;
	if ( (I0=indxHdr[node])!=0 )
	{
		do
		{
			stat2 = &(stat[indx[I0].index]);
			if ( (frq=stat2->freqency) > 0 )
			{
				var->v[0] += frq * (stat2->r-meanY) * (stat2->r-meanY);
				var->v[1] += frq * (stat2->g-meanI) * (stat2->g-meanI);
				var->v[2] += frq * (stat2->b-meanQ) * (stat2->b-meanQ);
				var->v[3] += frq * (stat2->a-meanA) * (stat2->a-meanA);
			}
		}
		while ( ((I0=m_pHashTable[I0].next)!=0) );
	}
	var->v[0] /= (float)frqTot;
	var->v[1] /= (float)frqTot;
	var->v[2] /= (float)frqTot;
	var->v[3] /= (float)frqTot;
}

//0.0 < gamma < 1e36  -- gamma==1.0 means no change
void GetGammaTbl (float gamma,unsigned char *lumTbl)
{
	double g;
	float K;
	int i;

	// Iout = (Iin^(1/g))*(Rin^(-1/g))*Rout
	// I=intensity (luminance)
	// g=gamma (1.0 means unchanged gamma)
	// R = input & output range of values

	g = 1.0/gamma;
	K = (float)Math::Pow((float)COLOR_INDEX_MAX,(float)-g)*COLOR_INDEX_MAX;

	for (i=0; i<TOTAL_COLORS; i++)
		lumTbl[i] = (unsigned char)(Math::Pow((float)lumTbl[i],(float)g) * K + .5);

}
