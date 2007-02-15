////////////////////////////////////////////////////////////////////////////
//
// KeySelection
//
// Class to deal with the selection children of a key
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: KeySelection.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

////////////////////////////////////////////////////////////////////////////

#include "TupperAttribPch.h"
#include "KeySelection.h"

TupKeySelection::TupKeySelection(void) : m_defaultIndex(-1)
{
}

// will search through the selection and return the index of the first match
// will return the default index if is not found or the string is null
int TupKeySelection::GetSelectionIndex(const char *pSelectionString) const
{
	if (pSelectionString)
	{
		for (int index = 0;index<m_selectionStrings.GetSize();index++)
		{
			if (!m_selectionStrings[index].Compare(pSelectionString))
			{
				return index;
			}
		}
	}
	return m_defaultIndex;
}

int TupKeySelection::GetDefaultIndex(void) const
{
	return m_defaultIndex;
}

int TupKeySelection::GetNumSelections(void) const
{
	return m_selectionStrings.GetSize();
}

const char *TupKeySelection::GetSelection(int index) const
{
	if (index<m_selectionStrings.GetSize())
	{
		return m_selectionStrings[index];
	}
	return NULL;
}

// will add the string to the selection
// does not check if the string is already in the selection array
// returns the index within the selection array (-1 if the string is NULL)
int TupKeySelection::AddSelection(const char *pSelectionString,bool isDefault)
{
	if (pSelectionString)
	{
		m_selectionStrings.Add(pSelectionString);
		return m_selectionStrings.GetSize()-1;
	}
	return -1;
}

// will add the string to the selection
// does not check if the string is already in the selection array
// returns the index within the selection array (-1 if the string is NULL)
int TupKeySelection::AddSelection(const TupString &selectionString,bool isDefault)
{
	if (!selectionString.IsEmpty())
	{
		m_selectionStrings.Add(selectionString);
		return m_selectionStrings.GetSize()-1;
	}
	return -1;
}

void TupKeySelection::Clear(void)
{
	m_defaultIndex = -1;
	m_selectionStrings.RemoveAll();
}

