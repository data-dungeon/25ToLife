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
// Helper Structures:


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
	bool RemoveKeyname(const char *pKey);
	bool RemoveAllKeynames();
	void StrToCurrAttribData(const TupString &strData);
	bool AddKeyValue(const KeyValuePair &keyValuePair);
	bool AddKeyValue(const char *pKey,const char *pValue);
	bool AddKeyValue(const char *pKey, bool bValue);
	bool AddKeyValue(const char *pKey, int nValue);
	bool AddKeyValue(const char *pKey, float fValue);
	bool AddKeyValue(const char *pKey, const AttribColor &colorValue);
	
	// Misc:
	void FormCheckListVal(const TupString &strKeyname, const TupString &strValue, bool bChecked);

#ifdef _DEBUG
	void PrintDebug(void) const;
#endif

protected:
	// Protected data members:
	TupArray<KeyValuePair> m_keyValuePairArray;	// Holds the actual data.

private:
};

//-----------------------------------------------------------------------------


#endif
