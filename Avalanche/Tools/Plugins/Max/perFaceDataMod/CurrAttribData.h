//-----------------------------------------------------------------------------
//	File:		CurrAttribData.h
//	Created:	Nov. 29, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CurrAttribData class handles storing, getting, setting, etc. of
//			the current attribute data for the selected entity(s).
//
//	History:	11/29/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __CURRATTRIBDATA__
#define __CURRATTRIBDATA__



//-----------------------------------------------------------------------------
// Includes:



//-----------------------------------------------------------------------------
// Helper classes

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

//-----------------------------------------------------------------------------
// Class Declaration:

class CurrAttribData
{
public:
	
	CurrAttribData();
	virtual ~CurrAttribData();

	// Accessors:
	int KeynameExistsAt(const char *strKeyname) const;
	TupString GetKeyAsString(int nIndex) const;
	TupString GetValueAsString(int nIndex) const;
	bool GetValueAsBool(int nIndex) const;
	int GetValueAsInt(int nIndex) const;
	float GetValueAsFloat(int nIndex) const;
	AttribColor GetValueAsColor(int nIndex) const;
	TupString CurrObjDataToStr(void) const;
	int GetNumAttribs(void) const;
	int Find(const KeyValuePair &keyValuePair) const;
	bool Compare(const CurrAttribData &other) const;
	const TupArray<KeyValuePair> &GetKeyValuePairs(void) const;

	// Mutators:
	void RemoveKeyname(int nIndex);
	void RemoveKeyname(const char *pKey);
	void RemoveAllKeynames(void);
	void StrToCurrAttribData(const TupString &strData);
	void AddKeyValue(const KeyValuePair &keyValuePair);
	void AddKeyValue(const char *pKey,const char *pValue);
	void AddKeyValue(const char *pKey, bool bValue);
	void AddKeyValue(const char *pKey, int nValue);
	void AddKeyValue(const char *pKey, float fValue);
	void AddKeyValue(const char *pKey, const AttribColor &colorValue);

#ifdef _DEBUG
	void PrintDebug() const;
#endif

protected:
	// Protected data members:
	TupArray<KeyValuePair> m_keyValuePairArray;	// Holds the actual data.

private:
};

//-----------------------------------------------------------------------------


#endif
