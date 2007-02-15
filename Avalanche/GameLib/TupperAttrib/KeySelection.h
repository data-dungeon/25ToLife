////////////////////////////////////////////////////////////////////////////
//
// KeySelection
//
// Class to deal with the selection children of a key
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: KeySelection.h $
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

#ifndef TUPPERATTRIB_KEY_SELECTION_H
#define TUPPERATTRIB_KEY_SELECTION_H

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "ConfigBase.h"
#endif

class TupKeySelection
{
public:
	TupKeySelection(void);

	int GetSelectionIndex(const char *pSelectionString) const;
	int GetDefaultIndex(void) const;
	int GetNumSelections(void) const;
	const char *GetSelection(int index) const;

	int AddSelection(const char *pSelectionString,bool isDefault);
	int AddSelection(const TupString &selectionString,bool isDefault);
	void Clear(void);

private:

	int m_defaultIndex;
	TupArray<TupString> m_selectionStrings;
};

#endif