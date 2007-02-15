////////////////////////////////////////////////////////////////////////////
//
// KeyValue
//
// Class to deal with the key value pairs
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef __KEYVALUE__
#define __KEYVALUE__

#ifndef __CONFIGBASE__
#include "ConfigBase.h"
#endif

struct KeyValuePair
{
	TupString m_strKey;
	TupString m_strValue;

	KeyValuePair(void) {}
	KeyValuePair(const char *pKey) : m_strKey(pKey) {}
	KeyValuePair(const char *pKey,const char *pValue) : m_strKey(pKey), m_strValue(pValue) {}
	KeyValuePair(const TupString &strKey) : m_strKey(strKey) {}
	KeyValuePair(const TupString &strKey,const TupString &strValue) : m_strKey(strKey), m_strValue(strValue) {}
};

bool operator==(const KeyValuePair& s1, const KeyValuePair& s2);

class KeyValue
{

private:
	Keeper<TupString> m_keyKeeper; // for fast key lookup
	TupArray<KeyValuePair> m_keyValuePairArray;	// Holds the actual data.

public:
	KeyValue(void);
	KeyValue(const char *pBuffer,const char *pBlockStart=NULL,const char *pBlockEnd=NULL);
	KeyValue(const TupArray<KeyValuePair> keyValuePairArray);

	int KeyExistsAt(const char *pKey) const;
	int KeyExistsAt(const TupString &key) const;
	const char *GetKeyAsString(int nIndex) const;
	const char *GetValueAsString(int nIndex) const;
	bool GetValueAsBool(int nIndex) const;
	int GetValueAsInt(int nIndex) const;
	float GetValueAsFloat(int nIndex) const;
	AttribColor GetValueAsColor(int nIndex) const;
	int GetNumAttribs() const;

	int AddKeyValue(const char *pKey,const char *pValue,bool bReplace=true);
	int AddKeyValue(const TupString &key,const TupString &value,bool bReplace=true);
	bool MergeKeyValues(const KeyValue &keysToAdd,bool bReplace=true);
	TupString BuildString(const char *pBuffer1,const char *pBuffer2,const char *pBlockStart=NULL,const char *pBlockEnd=NULL);

private:
	static bool StripAllButRawData(TupString& strToFix,const char *pBlockStart,const char *pBlockEnd);
	static bool RemoveCharsFromStr(TupString& strToFix, char chToRemove);
	static bool GetBeforeBlockStart(TupString& strToFix,const char *pBlockStart);
	static bool GetAfterBlockEnd(TupString& strToFix,const char *pBlockEnd);
	static void StripStringWhiteSpace(TupString &strToFix);
};


#endif
