//-----------------------------------------------------------------------------
//	File:		CurrAttribData.cpp
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



//-----------------------------------------------------------------------------
// Includes:

#include "pfdModPCH.h"
#include "CurrAttribData.h"

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Class Definition:

bool operator==(const KeyValuePair& s1, const KeyValuePair& s2)
{
	return (s1.m_strKey==s2.m_strKey && s1.m_strValue==s2.m_strValue);
}


//-----------------------------------------------------------------------------
// CurrAttribData::CurrAttribData
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
CurrAttribData::CurrAttribData()
{
}


//-----------------------------------------------------------------------------
// CurrAttribData::~CurrAttribData
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
CurrAttribData::~CurrAttribData()
{
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// CurrAttribData::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void CurrAttribData::PrintDebug(void) const
{
	ATDebugPrint("CURRENTATTRIBTDATA:\n");
	int nVectSize = m_keyValuePairArray.GetSize();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		const KeyValuePair &currentAttrib = m_keyValuePairArray[nIndex];
		ATDebugPrint((const char *)currentAttrib.m_strKey);
		ATDebugPrint("=");
		ATDebugPrint((const char *)currentAttrib.m_strValue);
		ATDebugPrint("\n");
	}
}
#endif


//-----------------------------------------------------------------------------
// CurrAttribData::KeynameExistsAt
// This method will look in the current attribute data and return the index of
// where the keyname exists at.  If the keyname does not exist, a value of -1
// is returned.
//
// Input:
//  TupString strKeyname -- The keyname to look for.
//
// Output: Index of where keyname exists at.  -1 if it doesn't exist.
//-----------------------------------------------------------------------------
int CurrAttribData::KeynameExistsAt(const char *strKey) const 
{
	int nIndexAt = -1;

	// Cycle through all the keynames:
	int nNumKeynames = m_keyValuePairArray.GetSize();
	for (int nIndex = 0; nIndex < nNumKeynames; nIndex++)
	{
		if (!strcmp(m_keyValuePairArray[nIndex].m_strKey,strKey))
			nIndexAt = nIndex;
	}
	return nIndexAt;
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetKeyAsString
// This method will return the keyname (specified by the incoming index) as a
// string.
//
// Input:
//  int nIndex -- The index to get the key for.
//
// Output: A TupString of the key at the index specified.
//-----------------------------------------------------------------------------
TupString CurrAttribData::GetKeyAsString(int nIndex) const
{
	return m_keyValuePairArray[nIndex].m_strKey;
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetValueAsString
// This method will return the value (specified by the incoming index) as a
// string
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A TupString of the value at index specified.
//-----------------------------------------------------------------------------
TupString CurrAttribData::GetValueAsString(int nIndex) const
{
	return m_keyValuePairArray[nIndex].m_strValue;
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetValueAsBool
// This method will return the value (specified by the incoming index) as a
// bool.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A bool of the value at index specified.
//-----------------------------------------------------------------------------
bool CurrAttribData::GetValueAsBool(int nIndex) const
{
	int nIntValue = atoi(m_keyValuePairArray[nIndex].m_strValue);

	if (nIntValue == 0)
		return false;
	else
		return true;
	// TODO - Do some error checking to check for other values?
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetValueAsInt
// This method will return the value (specified by the incoming index) as an
// int.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: An int of the value at index specified.
//-----------------------------------------------------------------------------
int CurrAttribData::GetValueAsInt(int nIndex) const
{
	return atoi(m_keyValuePairArray[nIndex].m_strValue);
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetValueAsFloat
// This method will return the value (specified by the incoming index) as a
// float.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A float of the value at index specified.
//-----------------------------------------------------------------------------
float CurrAttribData::GetValueAsFloat(int nIndex) const
{
	return (float)atof(m_keyValuePairArray[nIndex].m_strValue);
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetValueAsColor
// This method will return the value (specified by the incoming index) as a
// color.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A color of the value at index specified.
//-----------------------------------------------------------------------------
AttribColor CurrAttribData::GetValueAsColor(int nIndex) const
{
	// Get the value for red:
	TupString strValue = m_keyValuePairArray[nIndex].m_strValue;
	int nFirstComma = strValue.Find(',');
	TupString strRed = strValue.Left(nFirstComma);
	int nRed = atoi((const char *)strRed);

	// Get the value for green:
	strValue = strValue.Right(nFirstComma + 1);
	nFirstComma = strValue.Find(',');
	TupString strGreen = strValue.Left(nFirstComma);
	int nGreen = atoi((const char *)strGreen);

	// Get the value for blue:
	strValue = strValue.Right(nFirstComma + 1);
	nFirstComma = strValue.Find(',');
	TupString strBlue = strValue.Left(nFirstComma);
	int nBlue = atoi((const char *)strBlue);

	// Get the name:
	TupString strName = strValue.Left(nFirstComma + 1);

	// Create the color and return it:
	AttribColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	returnColor.m_strName = strName;
	return returnColor;
}


//-----------------------------------------------------------------------------
// CurrAttribData::CurrObjDataToStr
// This method will form a string based on all of the keyname-value sets in
// the data vector and pass back the string.
//
// Input: None.
//
// Output: TupString representing all the keyname-value sets in the vector.
//		   The string passed back will be in the format:
//				keyname=value\r
//				keyname=value\r
//				keyname=value\r
//-----------------------------------------------------------------------------
TupString CurrAttribData::CurrObjDataToStr(void) const
{
	TupString strToReturn = "";

	// Cycle through the data vector, forming the string as we go:
	int nNumKeyValuePairs = m_keyValuePairArray.GetSize();
	for (int nIndex = 0; nIndex < nNumKeyValuePairs; nIndex++)
	{
		strToReturn+=m_keyValuePairArray[nIndex].m_strKey;
		strToReturn+="=";
		strToReturn+=m_keyValuePairArray[nIndex].m_strValue;
		strToReturn+="\r";
	}
	return strToReturn;
}

//-----------------------------------------------------------------------------
// CurrAttribData::GetNumAttribs
// This method will return the number of attribs in this object.
//
// Input: None.
//
// Output: Int representing the number of attribs in this object.
//-----------------------------------------------------------------------------
int CurrAttribData::GetNumAttribs(void) const
{
	return m_keyValuePairArray.GetSize();
}

//-----------------------------------------------------------------------------
// CurrAttribData::GetNumAttribs
// This method will return the number of attribs in this object.
//
// Input: None.
//
// Output: const TupArray<KeyValuePair> &representing the key value pairs used
//-----------------------------------------------------------------------------
const TupArray<KeyValuePair> &CurrAttribData::GetKeyValuePairs(void) const
{
	return m_keyValuePairArray;
}

//-----------------------------------------------------------------------------
// CurrAttribData::RemoveKeyname
// This method will remove the keyname at the index specified.
//
// Input:
//  int nIndex -- The index of the keyname to remove.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::RemoveKeyname(int nIndex)
{
	m_keyValuePairArray.RemoveAt(nIndex);
}


//-----------------------------------------------------------------------------
// CurrAttribData::RemoveKeyname
// This method will remove all instances of the key specified.
//
// Input:
//  TupString strKeyname -- The keyname to remove.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::RemoveKeyname(const char *strKey)
{
	int nNumKeyVals = m_keyValuePairArray.GetSize();
	for (int nIndex = nNumKeyVals - 1; nIndex >= 0; nIndex--)
	{
		if (!strcmp(m_keyValuePairArray[nIndex].m_strKey,strKey))
		{
			m_keyValuePairArray.RemoveAt(nIndex);
		}
	}
}


//-----------------------------------------------------------------------------
// CurrAttribData::RemoveAllKeynames
// This method will empty out the vector containing all of the current attrib
// data.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::RemoveAllKeynames(void)
{
	m_keyValuePairArray.RemoveAll();
}

//-----------------------------------------------------------------------------
// CurrAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the string passed in.
//
// Input:
//	const KeyValuePair &keyValuePair -- The key and value to add
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::AddKeyValue(const KeyValuePair &keyValuePair)
{
	RemoveKeyname(keyValuePair.m_strKey);
	m_keyValuePairArray.Add(keyValuePair);
}


//-----------------------------------------------------------------------------
// CurrAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the string passed in.
//
// Input:
//	const char *pKey -- The keyname to use for this attribute.
//  const char *pValue -- The value to use for this attribute.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::AddKeyValue(const char *pKey,const char *pValue)
{
	RemoveKeyname(pKey);
	KeyValuePair newEntry(pKey,pValue);
	m_keyValuePairArray.Add(newEntry);
}


//-----------------------------------------------------------------------------
// CurrAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the boolean passed in.
//
// Input:
//  const char *pKey -- The keyname to use for this attribute.
//  bool bValue -- The boolean value to use for this attribute (converted to a
//				   string.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::AddKeyValue(const char *pKey, bool bValue)
{
	RemoveKeyname(pKey);
	KeyValuePair newEntry(pKey);
	if (bValue == true)
		newEntry.m_strValue = "1";
	else
		newEntry.m_strValue = "0";
	m_keyValuePairArray.Add(newEntry);	
}


//-----------------------------------------------------------------------------
// CurrAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the integer passed in.
//
// Input:
//  const char *pKey -- The keyname to use for this attribute.
//  int nValue -- The int value to use for this attribute (converted to a
//				   string.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::AddKeyValue(const char *pKey, int nValue)
{
	RemoveKeyname(pKey);
	KeyValuePair newEntry(pKey);
	char pChValue[50];
	newEntry.m_strValue = itoa(nValue, pChValue, 10);
	m_keyValuePairArray.Add(newEntry);
}

static char pChValue[50];

//-----------------------------------------------------------------------------
// CurrAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the float passed in.
//
// Input:
//  const char *pKey -- The keyname to use for this attribute.
//  float fValue -- The float value to use for this attribute (converted to a
//					string.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::AddKeyValue(const char *pKey, float fValue)
{
	RemoveKeyname(pKey);
	KeyValuePair newEntry(pKey);
	newEntry.m_strValue = gcvt(fValue, 10, pChValue);
	m_keyValuePairArray.Add(newEntry);
}


//-----------------------------------------------------------------------------
// CurrAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the color passed in.
//
// Input:
//  const char *pKey -- The keyname to use for this attribute.
//  const AttribColor &colorValue -- The color value to use for this attribute
//							  (converted to a string).
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::AddKeyValue(const char *pKey,const AttribColor &colorValue)
{
	RemoveKeyname(pKey);
	KeyValuePair newEntry(pKey);

	// Set the value:
	TupString strColor = "";
	strColor+=itoa(colorValue.m_nRed, pChValue, 10);
	strColor+=",";
	strColor+=itoa(colorValue.m_nGreen, pChValue, 10);
	strColor+=",";
	strColor+=itoa(colorValue.m_nBlue, pChValue, 10);
	strColor+=",";
	strColor+=colorValue.m_strName;
	newEntry.m_strValue = strColor;

	// Add it to the vector:
	m_keyValuePairArray.Add(newEntry);
}

//-----------------------------------------------------------------------------
// CurrAttribData::StrToCurrAttribData
// This method will remove everything in the attribute data vector and reset it
// with the data passed in.
// NOTE:  The string passed in should be in the format:
//			keyname=value\r
//			keyname=value\r
//			keyname=value\r		This last newline is optional.
//
// Input:
//  const TupString &strData -- Represents the raw data to store in the attrib data
//						   vector.  One word of warning: the input string
//						   should have all cariage returns removed from it!
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::StrToCurrAttribData(const TupString &strData)
{
	m_keyValuePairArray.RemoveAll();

	// Parse through the attributes:
	int pos = 0;
	int strDataLength = strData.GetLength();
	while (pos<strDataLength)
	{
		// Parse the current line:
		int equalsIndex = strData.Find('=',pos);
		if (equalsIndex != -1)
		{
			TupString strKeyname;
			TupString strValue;
			
			// Get the keyname:
			strKeyname = strData.Mid(pos,equalsIndex - pos);
			
			// Get the value:
			int newLineIndex = strData.Find('\r',pos);
			if (newLineIndex != -1)
			{
				strValue = strData.Mid(equalsIndex + 1, newLineIndex - (equalsIndex + 1));
				pos = newLineIndex+1;
			}
			else
			{
				strValue = strData.Mid(equalsIndex + 1, strDataLength - (equalsIndex + 1));
				pos = strDataLength;
			}
			strKeyname.TrimLeft();
			strKeyname.TrimRight();
			strValue.TrimLeft();
			strValue.TrimRight();
			
			// Add the attribute to the data vector:
			KeyValuePair newEntry;
			newEntry.m_strKey = strKeyname;
			newEntry.m_strValue = strValue;
			m_keyValuePairArray.Add(newEntry);
		}
		else
		{
			break; // bail if no equals sign
		}
	}
}


//-----------------------------------------------------------------------------
// CurrAttribData::Find
// This method will attempt to find the keyValuePair and return the index with in 
// the m_keyValuePairArray.  If the keyValuePair cannot be found it will return -1
// Input:
//  const KeyValuePair &keyValuePair -- Represents the key and value to search for
//
// Output: int -- the index within the m_keyValuePairArray or -1 if not found
//-----------------------------------------------------------------------------
int CurrAttribData::Find(const KeyValuePair &keyValuePair) const
{
	int numEntries = m_keyValuePairArray.GetSize();
	for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
	{
		if (m_keyValuePairArray[entryIndex]==keyValuePair)
			return entryIndex;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// CurrAttribData::Compare
// This method will compare the given object with another and return true if they are equal
//
// Input:
//  const CurrAttribData &other -- Represents the object object to compare to
//
// Output: bool -- true if the objects are equal - false otherwise
//-----------------------------------------------------------------------------
bool CurrAttribData::Compare(const CurrAttribData &other) const
{
	int numKeyValuePairs = m_keyValuePairArray.GetSize();
	// they need the same number of entries
	if (numKeyValuePairs!=other.GetNumAttribs())
		return false;
	// see if all my keyValue pairs are found in the other
	for (int index=0;index<numKeyValuePairs;index++)
	{
		// if not found we are different
		if (other.Find(m_keyValuePairArray[index])==-1)
			return false;
	}
	return true;
}


