//-----------------------------------------------------------------------------
//	File:		ColorSetManager.cpp
//	Created:	November 16, 2004
//				Copyright (c) 2002-2004, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The ColorSet handles keeping track of the color sets
//
//-----------------------------------------------------------------------------

#include "ApplyColorsPCH.h"
#include "ColorSetManager.h"
#include "ColorSet.h"

ColorSetManager::ColorSetManager(void)
{
}

void ColorSetManager::Init(TupperwareAggregate *pRootAgg)
{
	m_colorSets.RemoveAll();

	TupArray<TupperwareAggregate *> setPointers;
	for( TupperwareIterator tupIterator = pRootAgg->FindByKey("COLOR_SET"); !tupIterator.Finished(); tupIterator.Next() )
	{
		setPointers.Add(tupIterator.GetAsAggregate());
	}
	m_colorSets.SetSize(setPointers.GetSize());
	for (int index=0;index<setPointers.GetSize();index++)
	{
		m_colorSets[index].Init(setPointers[index]);
	}
}

int ColorSetManager::GetNumColorSets(void) const
{
	return m_colorSets.GetSize();
}

const ColorSet &ColorSetManager::GetColorSet(int index) const
{
	return m_colorSets[index];
}
