////////////////////////////////////////////////////////////////////////////
//
// KeyValue
//
// Class to deal with the key value pairs
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: KeyValue.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/16/02    Time: 3:42p
 * Updated in $/Avalanche/tools/TupperAttrib
 * trim whitespace on value
 * don't check for case on checkbox value
 * "1" will work as a valid value
 * 
 * *****************  Version 8  *****************
 * User: Todd Blackburn Date: 7/10/02    Time: 2:57p
 * Updated in $/Avalanche/GameLib/TupperAttrib
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/23/02    Time: 4:16p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Merging and building code
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERATTRIB_KEY_VALUE_H
#define TUPPERATTRIB_KEY_VALUE_H

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "TupperAttrib/ConfigBase.h"
#endif

class TupKeyValue
{

public:
	struct KeyValuePair
	{
		TupString m_strKey;
		TupString m_strValue;
	};

private:
	TupArray<KeyValuePair> m_keyValuePairArray;	// Holds the actual data.

public:
	TupKeyValue(void);
	TupKeyValue(const char *pBuffer,const char *pBlockStart=NULL,const char *pBlockEnd=NULL);

	int KeyExistsAt(const char *pKey) const;
	int KeyExistsAt(const TupString &key) const;
	const char *GetKeyAsString(int nIndex) const;
	const char *GetValueAsString(int nIndex) const;
	bool GetValueAsBool(int nIndex) const;
	int GetValueAsInt(int nIndex) const;
	float GetValueAsFloat(int nIndex) const;
	TupAttribColor GetValueAsColor(int nIndex) const;
	int GetNumAttribs() const;

	int AddKeyValue(const char *pKey,const char *pValue,bool bReplace=true);
	int AddKeyValue(const TupString &key,const TupString &value,bool bReplace=true);
	bool MergeKeyValues(const TupKeyValue &keysToAdd,bool bReplace=true);
	TupString BuildString(const char *pBuffer1,const char *pBuffer2,const char *pBlockStart=NULL,const char *pBlockEnd=NULL);

private:
	static bool StripAllButRawData(TupString& strToFix,const char *pBlockStart,const char *pBlockEnd);
	static bool RemoveCharsFromStr(TupString& strToFix, char chToRemove);
	static bool GetBeforeBlockStart(TupString& strToFix,const char *pBlockStart);
	static bool GetAfterBlockEnd(TupString& strToFix,const char *pBlockEnd);
	static void StripStringWhiteSpace(TupString &strToFix);
};


#endif
