//-----------------------------------------------------------------------------
//	File:		ColorSetManager.h
//	Created:	November 16, 2004
//				Copyright (c) 2002-2004, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The ColorSet handles keeping track of the color sets
//
//-----------------------------------------------------------------------------

#ifndef __COLORSETMANAGER__
#define __COLORSETMANAGER__

#include "ColorSet.h"

class ColorSetManager
{
	TupArray<ColorSet> m_colorSets;

public:

	ColorSetManager(void);
	void Init(TupperwareAggregate *pRootAgg);

	int GetNumColorSets(void) const;
	const ColorSet &GetColorSet(int index) const;
};

#endif