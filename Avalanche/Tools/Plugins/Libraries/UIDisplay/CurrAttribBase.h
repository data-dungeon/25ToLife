//-----------------------------------------------------------------------------
//	File:		CurrAttribBase.h
//	Created:	Feb. 18, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CurrAttribBase class acts as a base class to hold common
//			methods and data members used by any inherited CurrAttrib classes.
//
//	History:	2/18/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __CURRATTRIBBASE__
#define __CURRATTRIBBASE__



//-----------------------------------------------------------------------------
// Includes:



//-----------------------------------------------------------------------------
// Helper Structures:

class UniqueValue
{
public:
	TupString m_strValue;
	int m_count;
	bool operator==(const UniqueValue& other) const;
	bool operator<=(const UniqueValue& other) const;
	bool operator>=(const UniqueValue& other) const;
	bool operator<(const UniqueValue& other) const;
	bool operator>(const UniqueValue& other) const;

};

class UniqueValues
{
public:
	TupArray<UniqueValue> m_uniqueValues;
	int m_totalCount;
	int m_defaultIndex;
	int m_uniqueIndex;
	int m_totalHaveValue;
};

struct AttribColor
{
	int m_nRed;
	int m_nGreen;
	int m_nBlue;
	TupString m_strName;
};

struct KeyValPair
{
	TupString m_strKeyname;
	TupString m_strVal;
};


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class CurrAttribBase
{
public:
	// Constructors and destructors:
	CurrAttribBase();
	virtual ~CurrAttribBase();

	// Misc public methods:
	virtual void GetAllUniqueValues(const TupString &strKey, const TupString &strDefaultValue, UniqueValues& uniqueValues, Keeper<TupString> *pValidValues) const {};
	virtual bool FindEntityNames(const TupArray<KeyValPair> &vectCriteria, Keeper<TupString> &vectEntityNames) const;
	virtual bool FindCheckStates(const TupString &strKeyname,const Keeper<TupString> &vectEntityNames,
								 int nDefaultState, TupArray<int>& vectCheckedStates) const;
	static AttribColor StrToColor(const TupString &strColor);

protected:
	// Protected methods:

	// Protected data members:

private:
};

//-----------------------------------------------------------------------------


#endif
