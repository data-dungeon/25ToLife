////////////////////////////////////////////////////////////////////////////
//
// KeySelection
//
// Class to deal with the selection children of a key
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef __KEYSELECTION__
#define __KEYSELECTION__

#ifndef __CONFIGBASE__
#include "ConfigBase.h"
#endif

class KeySelection
{
public:
	KeySelection(void);

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