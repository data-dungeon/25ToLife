////////////////////////////////////////////////////////////////////////////
//
// KeyValueContext
//
// Class to deal with the key value pairs along with the context within the config tree
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef __KEYVALUECONTEXT__
#define __KEYVALUECONTEXT__

#ifndef __CONFIG_BASE__
#include "ConfigBase.h"
#endif

class KeySelection;

class KeyValueContext
{
public:
	struct KeyValuePairContext
	{
		TupString m_strKey; // the key used within the tree and the keyvalue class
		TupString m_strValue; // the string representing the data at the key
		const ConfigBase *m_pConfigContext; // base class pointer to the control
		int m_selectionIndex; // which child selection our value was (only in checkboxes, comboboxes, and radio groups)
		bool m_bIsDefaultValue; // if the value is the default for this key
		TupString m_strDefaultValue; // what the default value is
		int m_keyValueIndex; // index into keyValue array 

		KeyValuePairContext(void) : m_pConfigContext(NULL), m_selectionIndex(-1), m_keyValueIndex(-1) {}
	};

private:
	Keeper<TupString> m_keyKeeper;
	TupArray<KeyValuePairContext> m_keyValuePairContextArray;	// Holds the actual data.

public:
	KeyValueContext(void);

	int GetNumAttribs(void) const;
	int KeyExistsAt(const char *pKey) const;
	int KeyExistsAt(const TupString &key) const;
	const ConfigBase *GetControlPtr(int nIndex) const;
	bool IsDefaultValue(int nIndex) const;
	int GetKeyValueIndex(int nIndex) const;
	const char *GetDefaultValueAsString(int nIndex) const;
	const char *GetKeyAsString(int nIndex) const;
	const char *GetValueAsString(int nIndex) const;
	bool GetValueAsBool(int nIndex) const;
	int GetValueAsInt(int nIndex) const;
	float GetValueAsFloat(int nIndex) const;
	AttribColor GetValueAsColor(int nIndex) const;
	int GetSelectionIndex(int nIndex) const;
	bool ValueValidAt(int nIndex) const;

	int AddKeyValue(const char *pKey,const char *pValue,int selectionIndex,const ConfigBase *pConfigContext,bool bDefaultValue,const char *pDefaultValue,int keyValueIndex);
	int AddKeyValue(const TupString &key,const TupString &value,int selectionIndex,const ConfigBase *pConfigContext,bool bDefaultValue,const char *pDefaultValue,int keyValueIndex);
	int AddKey(const char *pKey);
	int AddKey(const TupString &key);

};


#endif
