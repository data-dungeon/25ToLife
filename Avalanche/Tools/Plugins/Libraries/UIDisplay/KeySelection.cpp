////////////////////////////////////////////////////////////////////////////
//
// KeySelection
//
// Class to deal with the selection children of a key
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "UIDisplayPch.h"
#include "KeySelection.h"

KeySelection::KeySelection(void) : m_defaultIndex(-1)
{
}

// will search through the selection and return the index of the first match
// will return the default index if is not found or the string is null
int KeySelection::GetSelectionIndex(const char *pSelectionString) const
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

int KeySelection::GetDefaultIndex(void) const
{
	return m_defaultIndex;
}

int KeySelection::GetNumSelections(void) const
{
	return m_selectionStrings.GetSize();
}

const char *KeySelection::GetSelection(int index) const
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
int KeySelection::AddSelection(const char *pSelectionString,bool isDefault)
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
int KeySelection::AddSelection(const TupString &selectionString,bool isDefault)
{
	if (!selectionString.IsEmpty())
	{
		m_selectionStrings.Add(selectionString);
		return m_selectionStrings.GetSize()-1;
	}
	return -1;
}

void KeySelection::Clear(void)
{
	m_defaultIndex = -1;
	m_selectionStrings.RemoveAll();
}

