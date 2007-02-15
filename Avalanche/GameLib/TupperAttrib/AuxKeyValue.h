////////////////////////////////////////////////////////////////////////////
//
// AuxKeyValue
//
// Class to deal with the auxiliary key value pairs
//
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERATTRIB_AUX_KEY_VALUE_H
#define TUPPERATTRIB_AUX_KEY_VALUE_H

#ifndef TUPPERATTRIB_KEY_VALUE_H
#include "KeyValue.h"
#endif

class TupKeyValueContext;

class TupAuxKeyValue
{

public:
	struct Entry
	{
		TupString m_entryName;
		TupKeyValue m_keyValue;

		Entry(void);
		Entry(const char *pEntryName,const char *pEntryValues);
		// copy constructor
		Entry(const Entry& src);
		// assignment operator
		const Entry& operator=(const Entry& src);
	};

private:
	int m_maxEntryNameLength; // 0 = no limit to length
	bool m_bCaseSensitiveEntryNames;
	Keeper<TupString> m_uniqueEntryNames;
	TupArray<Entry> m_entries;

public:
	TupAuxKeyValue(void);

	void LoadEntries(TupperwareAggregate *pAgg);
	void SaveEntries(TupperwareAggregate *pAgg);
	int GetMaxEntryNameLength(void) const;
	void SetMaxEntryNameLength(int value);
	bool IsEntryNamesCaseSensitive(void) const;
	void EntryNamesCaseSensitive(bool bValue);
	bool MergeToKeyValues(TupKeyValue &keyValue,const char *pEntryName,bool bReplace);
	void AddFromContext(const TupKeyValue &keyValue,const TupKeyValueContext &keyValueContext,const char *pEntryName);
	bool GetKeyValues(const char *pEntryName,TupKeyValue &keyValue);
private:
	TupString GetEntryKeeperName(const TupString &name) const;
};


#endif
