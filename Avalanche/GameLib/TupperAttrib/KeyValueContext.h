////////////////////////////////////////////////////////////////////////////
//
// KeyValueContext
//
// Class to deal with the key value pairs along with the context within the config tree
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: KeyValueContext.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 4:06p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added valid check
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:26p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Bugs...
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERATTRIB_KEY_VALUE_CONTEXT_H
#define TUPPERATTRIB_KEY_VALUE_CONTEXT_H

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "ConfigBase.h"
#endif

class TupKeySelection;

class TupKeyValueContext
{
public:
	struct KeyValuePairContext
	{
		TupString m_strKey; // the key used within the tree and the keyvalue class
		TupString m_strValue; // the string representing the data at the key
		const TupConfigBase *m_pConfigContext; // base class pointer to the control
		int m_selectionIndex; // which child selection our value was (only in checkboxes, comboboxes, and radio groups)

		KeyValuePairContext(void) : m_pConfigContext(NULL), m_selectionIndex(-1) {}
	};

private:
	TupArray<KeyValuePairContext> m_keyValuePairContextArray;	// Holds the actual data.

public:
	TupKeyValueContext(void);

	int GetNumAttribs(void) const;
	int KeyExistsAt(const char *pKey) const;
	const TupConfigBase *GetControlPtr(int nIndex) const;

	const char *GetKeyAsString(int nIndex) const;
	const char *GetValueAsString(int nIndex) const;
	bool GetValueAsBool(int nIndex) const;
	int GetValueAsInt(int nIndex) const;
	float GetValueAsFloat(int nIndex) const;
	TupAttribColor GetValueAsColor(int nIndex) const;
	int GetSelectionIndex(int nIndex) const;
	bool ValueValidAt(int nIndex) const;

	int AddKeyValue(const char *pKey,const char *pValue,int selectionIndex,const TupConfigBase *pConfigContext);
	int AddKeyValue(const TupString &key,const TupString &value,int selectionIndex,const TupConfigBase *pConfigContext);
	int AddKey(const char *pKey);
	int AddKey(const TupString &key);

};


#endif
