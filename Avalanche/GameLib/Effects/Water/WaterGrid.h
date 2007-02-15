//
// Author: Tyler Colbert
// Purpose: This class serves to map arbitrary data sets in a grid across the waters surface.
// Its an attempt at cleaning up a lot of the grids that I use into a cleaner box...
//


#ifdef DEBUG_WATER
#include "Display/Common/RenderMisc.h"
#endif


#ifndef __WATERGRID_H__
#define __WATERGRID_H__

// represents a grid of information across the water...
// this is a two dimensional grid of data spread across the water
// it does not handle looking anything up in the data for you...
template <class PrimaryDataType>
class WaterGrid
{
public:
	void Init(int subdivs, int sizeofElement, bool doMemset = false, int initialValue = 0);
	WaterGrid();
	~WaterGrid();

	int mGridSize;  // # vertices wide/tall the grid is.
	int mGridSizeMinus1; // mGridSize - 1
	int mGridNodes; // total vtx count
	float mInvGridSize; // 1.0f / mGridSize
	float mInvGridSizeMinus1; // 1.0f / mGridSizeMinus1

	PrimaryDataType* mDataBlock;

#ifdef DEBUG_WATER
	typedef ts_bRGBA (Water::*DrawGridColorQueryPtr)(int x, int z);
	void QuickDrawGrid(Graphics4x4* localToWorld, ts_bRGBA *color1);
	void DrawGridWeightedColors(Graphics4x4* localToWorld, ts_bRGBA* color1, ts_bRGBA* color2, bool useMarkers, float scale);
	void DrawGridAsColors(Graphics4x4* localToWorld, bool useMarkers);
#endif

	// this function supports grabbing from one of a series of parallel arrays that
	// may be in the data block and linearly interpolating the value out of it.
	// to work, the following kinds of expressions must be valid:
	// DataType = DataType + DataType
	// DataType = DataType * float
	// where float is in the range [0, 1]
	PrimaryDataType operator()(float x, float z);

	PrimaryDataType& operator()(int x, int z)
	{
		return mDataBlock[x + z * mGridSize];
	}
};

template<class PrimaryDataType>
WaterGrid<PrimaryDataType>::WaterGrid()
{
	mDataBlock = NULL;
}

template<class PrimaryDataType>
WaterGrid<PrimaryDataType>::~WaterGrid()
{
	if(mDataBlock)
		memFree(mDataBlock);
}

template<class PrimaryDataType>
void WaterGrid<PrimaryDataType>::Init(int subdivs, int sizeofElement, bool doMemset, int initialValue)
{
	mGridSizeMinus1 = (1 << subdivs);
	mGridSize = mGridSizeMinus1 + 1;
	mGridNodes = mGridSize * mGridSize;
	mInvGridSize = Math::Inverse((float)mGridSize);
	mInvGridSizeMinus1 = Math::Inverse((float)mGridSizeMinus1);

	mDataBlock = (PrimaryDataType*)memAlloc(sizeofElement * mGridNodes);
	
	if(doMemset)
		memset(mDataBlock, initialValue, sizeofElement * mGridNodes);
}

template<class PrimaryDataType>
PrimaryDataType WaterGrid<PrimaryDataType>::operator()(float x, float z)
{
	// four nearest grid lines
	int gridxl = (int)(x * (mGridSizeMinus1));
	int gridzl = (int)(z * (mGridSizeMinus1));
	int gridxh = gridxl + 1;
	int gridzh = gridzl + 1;

	// where between the lines are we
	float weightxh = x * (mGridSizeMinus1) - gridxl;
	float weightzh = z * (mGridSizeMinus1) - gridzl;
	float weightxl = 1.0f - weightxh;
	float weightzl = 1.0f - weightzh;

	// get the value and linear interpolate it...
	return (PrimaryDataType)(
		mDataBlock[gridxl + gridzl * mGridSize] * (weightxl * weightzl) +
		mDataBlock[gridxh + gridzl * mGridSize] * (weightxh * weightzl) +
		mDataBlock[gridxh + gridzh * mGridSize] * (weightxh * weightzh) +
		mDataBlock[gridxl + gridzh * mGridSize] * (weightxl * weightzh));
}

///////////////////////////////////////////////////////////
// WaterGrid - Debugging helpers
///////////////////////////////////////////////////////////

#ifdef DEBUG_WATER
template<class PrimaryDataType>
void WaterGrid<PrimaryDataType>::QuickDrawGrid(Graphics4x4* localToWorld, ts_bRGBA* color1)
{
	for(int y = 0; y <= mGridSize; y++)
	{
		Vector3 a, b, c, d;
		a.Set(0.0f, 0.0f, y * mInvGridSize);
		b.Set(1.0f, 0.0f, y * mInvGridSize);
		c.Set(y * mInvGridSize, 0.0f, 0.0f);
		d.Set(y * mInvGridSize, 0.0f, 1.0f);
		
		gfxDrawLineMC(localToWorld, &a, &b, color1);
		gfxDrawLineMC(localToWorld, &c, &d, color1);
	}
}
#endif // DEBUG_WATER

#ifdef DEBUG_WATER
template<class PrimaryDataType>
void WaterGrid<PrimaryDataType>::DrawGridWeightedColors(Graphics4x4* localToWorld, ts_bRGBA* color1, ts_bRGBA* color2, bool useMarkers, float scale)
{
	for(int y = 0; y < mGridSize; y++)
	{
		for(int x = 0; x < mGridSize; x++)
		{
			Vector3 a;
			a.Set(
				x * mInvGridSizeMinus1,
				0.0f,
				y * mInvGridSizeMinus1);

			float weight = operator()(x,y) * scale;
			float nweight = 1.0f - weight;
			ts_bRGBA color;
			color.R = (u8)(color1->R * weight + color2->R * nweight);
			color.G = (u8)(color1->G * weight + color2->G * nweight);
			color.B = (u8)(color1->B * weight + color2->B * nweight);
			color.A = (u8)(color1->A * weight + color2->A * nweight);

			if(useMarkers)
				gfxDrawMarkMC(localToWorld, &a, 0.25f, &color);
		}				
	}
}
#endif // DEBUG_WATER

#ifdef DEBUG_WATER
template<class PrimaryDataType>
void WaterGrid<PrimaryDataType>::DrawGridAsColors(Graphics4x4* localToWorld, bool useMarkers)
{
	ASSERT(sizeof(PrimaryDataType) == 4);

	for(int y = 0; y < mGridSize; y++)
	{
		for(int x = 0; x < mGridSize; x++)
		{
			Vector3 a;
			a.Set(x * mInvGridSizeMinus1,
					0.0f,
					y * mInvGridSizeMinus1);

			ts_bRGBA color = *(ts_bRGBA*)&operator()(x, y);

			if(useMarkers)
				gfxDrawMarkMC(localToWorld, &a, 0.25f, &color);
		}
	}	
}
#endif // DEBUG_WATER



#endif // __WATERGRID_H__
