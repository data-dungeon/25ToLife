/*
**
**  File:   SurfaceLookup.cpp
**  Date:   February 23, 2001
**  By:     Bryant Collard
**  Desc:   Lookup class for surface to surface pairings.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/SurfaceLookup.cpp $
**  $Revision: 5 $
**      $Date: 11/21/02 12:25p $
**    $Author: Dross $
**
*/

// Precompiled header
#include "stage/StagePCH.h"

DeclareSingleton(CSurfaceLookup);

CSurfaceLookup::CSurfaceLookup()
{
	d_surfaceType = NULL;
	d_numRow = 0;
	d_numCol = 0;
	d_indexMap = NULL;
	d_reverseIndexMap = NULL;
}

CSurfaceLookup::~CSurfaceLookup()
{
	Clear();
}

bool CSurfaceLookup::Init(CSurfaceType* i_surfaceType)
{
	if ((d_surfaceType != NULL) || (i_surfaceType == NULL))
		return(false);

	d_indexMap = new int[i_surfaceType->GetNumEntries()];
	if (d_indexMap == NULL)
		return(false);
	d_reverseIndexMap = new int[i_surfaceType->GetNumEntries()];
	if (d_reverseIndexMap == NULL)
	{
		delete[] d_indexMap;
		return(false);
	}

	int dynamicCount = 0;
	int staticCount = i_surfaceType->GetNumDynamicEntries();
	for (int i = 0; i < i_surfaceType->GetNumEntries(); i++)
	{
		if (i_surfaceType->IsDynamic(i))
		{
			if (dynamicCount >= i_surfaceType->GetNumDynamicEntries())
			{
				delete[] d_indexMap;
				delete[] d_reverseIndexMap;
				d_indexMap = NULL;
				d_reverseIndexMap = NULL;
				return(false);
			}
			d_indexMap[i] = dynamicCount;
			d_reverseIndexMap[dynamicCount] = i;
			dynamicCount++;
		}
		else
		{
			if (staticCount >= i_surfaceType->GetNumEntries())
			{
				delete[] d_indexMap;
				delete[] d_reverseIndexMap;
				d_indexMap = NULL;
				d_reverseIndexMap = NULL;
				return(false);
			}
			d_indexMap[i] = staticCount;
			d_reverseIndexMap[staticCount] = i;
			staticCount++;
		}
	}
	if ((dynamicCount != i_surfaceType->GetNumDynamicEntries()) ||
			(staticCount != i_surfaceType->GetNumEntries()))
	{
		delete[] d_indexMap;
		delete[] d_reverseIndexMap;
		d_indexMap = NULL;
		d_reverseIndexMap = NULL;
		return(false);
	}

	d_surfaceType = i_surfaceType;
	d_numRow = d_surfaceType->GetNumDynamicEntries();
	d_numCol = d_surfaceType->GetNumEntries();
	d_size = (d_numRow * ((d_numCol << 1) + 1 - d_numRow)) >> 1;

	return(true);
}

void CSurfaceLookup::Clear(void)
{
	CTableLookup::Clear();
	delete[] d_indexMap;
	delete[] d_reverseIndexMap;
	d_surfaceType = NULL;
	d_numRow = 0;
	d_numCol = 0;
	d_indexMap = NULL;
	d_reverseIndexMap = NULL;
}

void CSurfaceLookup::InitTables(CSurfaceType* i_surfaceType)
{
	if (Init(i_surfaceType))
		CTableLookup::InitTables();
}

int CSurfaceLookup::GetRow(const char* i_label)
{
	if (d_surfaceType == NULL)
		return(-1);

	return(d_surfaceType->GetIndex(i_label));
}

int CSurfaceLookup::GetCol(const char* i_label)
{
	if (d_surfaceType == NULL)
		return(-1);

	return(d_surfaceType->GetIndex(i_label));
}

int CSurfaceLookup::GetEntry(int i_row, int i_col)
{
	if ((i_row < 0) || (i_row >= d_numCol) ||
			(i_col < 0) || (i_col >= d_numCol))
		return(-1);

	i_row = d_indexMap[i_row];
	i_col = d_indexMap[i_col];

	if (i_row > i_col)
	{
		int temp = i_row;
		i_row = i_col;
		i_col = temp;
	}

	if (i_row >= d_numRow)
		return(-1);

	return(i_col + ((i_row * ((d_numCol << 1) - i_row - 1)) >> 1));
}

int CSurfaceLookup::GetValidEntry(int i_row, int i_col)
{
	ASSERT((d_numRow > 0) && (d_numCol > 0));

	if ((i_row < 0) || (i_row >= d_numCol))
		i_row = 0;

	if ((i_col < 0) || (i_col >= d_numCol))
		i_col = 0;

	i_row = d_indexMap[i_row];
	i_col = d_indexMap[i_col];

	if (i_row > i_col)
	{
		int temp = i_row;
		i_row = i_col;
		i_col = temp;
	}

	ASSERT(i_row < d_numRow);

	return(i_col + ((i_row * ((d_numCol << 1) - i_row - 1)) >> 1));
}

bool CSurfaceLookup::GetLabels(int i_entry, char* &o_row, char* &o_col)
{
	if ((d_surfaceType == NULL) || (i_entry < 0))
		return(false);

	int col = 0;
	int row;
	for (row = 0; row < d_numRow; row++)
	{
		col += d_numCol - row;
		if (i_entry < col)
			break;
	}

	if (row >= d_numRow)
		return(false);

	col = i_entry + d_numCol - col;

	o_row = d_surfaceType->GetName(d_reverseIndexMap[row]);
	o_col = d_surfaceType->GetName(d_reverseIndexMap[col]);

	return(true);
}

/*
 *	To enable CRigidBodyTable to spoof watch. Should go away.
 */
char* CSurfaceLookup::GetMappedRow(int i_row)
{
	return(d_surfaceType->GetName(d_reverseIndexMap[i_row]));
}

char* CSurfaceLookup::GetMappedCol(int i_col)
{
	return(d_surfaceType->GetName(d_reverseIndexMap[i_col]));
}

int CSurfaceLookup::GetMappedEntry(int i_row, int i_col)
{
	if (i_row > i_col)
	{
		int temp = i_row;
		i_row = i_col;
		i_col = temp;
	}

	if (i_row >= d_numRow)
		return(-1);

	return(i_col + ((i_row * ((d_numCol << 1) - i_row - 1)) >> 1));
}
