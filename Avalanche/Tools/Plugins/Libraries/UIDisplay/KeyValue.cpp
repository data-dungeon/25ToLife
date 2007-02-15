////////////////////////////////////////////////////////////////////////////
//
// KeyValue
//
// Class to deal with the key value pairs
//
////////////////////////////////////////////////////////////////////////////

#include "UIDisplayPCH.h"
#include "KeyValue.h"

bool operator==(const KeyValuePair& s1, const KeyValuePair& s2)
{
	return (s1.m_strKey==s2.m_strKey && s1.m_strValue==s2.m_strValue);
}


//-----------------------------------------------------------------------------
// KeyValue::KeyValue
// This method allows for the object to be constructed with no initial keyvalue pairs
//-----------------------------------------------------------------------------
KeyValue::KeyValue(void)
{
}

//-----------------------------------------------------------------------------
// KeyValue::KeyValue
//
// NOTE:  The string passed in should be in the format:
//			keyname=value\n
//			keyname=value\n
//			keyname=value\n		This last newline is optional.
//
// Input:
//  const char *pBuffer -- Represents the raw data to store in the attrib data
//						   vector.  One word of warning: the input string
//						   should have all cariage returns removed from it!
//	const char *pBlockStart -- Represents the string that starts a block (NULL if not needed)
//	const char *pBlockStart -- Represents the string that ends a block (NULL if not needed)
//
// Output: None
//-----------------------------------------------------------------------------
KeyValue::KeyValue(const char *pBuffer,const char *pBlockStart,const char *pBlockEnd)
{
	// make sure we can handle a NULL pointer
	if (!pBuffer)
		pBuffer = "";

	TupString tempStr(pBuffer);

	// Remove carriage returns from the string:
	RemoveCharsFromStr(tempStr,'\r');

	// Get only data within the block strings (if requested)
	StripAllButRawData(tempStr,pBlockStart,pBlockEnd);

	// Parse through the attributes:
	while (tempStr.IsEmpty() != true)
	{
		// Parse the current line:
		int equalsIndex = tempStr.Find('=');
		if (equalsIndex != -1)
		{
			TupString strKeyname;
			TupString strValue;

			// Get the keyname:
			strKeyname = tempStr.Left(equalsIndex);

			// Get the value:
			int newLineIndex = tempStr.Find('\n');
			if (newLineIndex != -1)
			{
				strValue = tempStr.Mid(equalsIndex + 1, newLineIndex - (equalsIndex + 1));
				tempStr.Delete(0, newLineIndex + 1);
			}
			else
			{
				int nSize = tempStr.GetLength();
				strValue = tempStr.Mid(equalsIndex + 1, nSize - (equalsIndex + 1));
				// If there is an equals, but no newline, then assume we are on
				// the last attribute, so clear the string thus forcing a bailout:
				tempStr.Empty();
			}
			StripStringWhiteSpace(strKeyname);
			StripStringWhiteSpace(strValue);


			// Add the attribute to the data vector:
			KeyValuePair newEntry;
			newEntry.m_strKey = strKeyname;
			newEntry.m_strValue = strValue;
			int oldSize = m_keyKeeper.GetSize();
			int newIndex = m_keyKeeper.Add(strKeyname);
			// if key not already there
			if (oldSize == newIndex)
			{
				m_keyValuePairArray.Add(newEntry);
			}
		}
		else
		{
			// There is no equals left in the string, so empty out the string
			// and bail out:
			tempStr.Empty();
		}
	}
}

KeyValue::KeyValue(const TupArray<KeyValuePair> keyValuePairArray)
{
	for (int index = 0; index < keyValuePairArray.GetSize(); index++)
	{
		int oldSize = m_keyKeeper.GetSize();
		int newIndex = m_keyKeeper.Add(keyValuePairArray[index].m_strKey);
		if (oldSize == newIndex)
		{
			m_keyValuePairArray.Add(keyValuePairArray[index]);
		}
	}
}
 
bool KeyValue::StripAllButRawData(TupString& strToFix,const char *pBlockStart,const char *pBlockEnd)
{
	if (pBlockStart && pBlockEnd)
	{
		// Find and remove the begin block indicator:
		int beginIndex = strToFix.Find(pBlockStart);
		if (beginIndex == -1)
		{
			// Error - There is no begin block indicator, so just bail:
			return false;
		}
		strToFix.Delete(0, beginIndex);
		int newLineIndex = strToFix.Find('\n');
		strToFix.Delete(0, newLineIndex + 1);

		// Find and remove the end block indicator:
		int endIndex = strToFix.Find(pBlockEnd);
		if (endIndex == -1)
		{
			// Error - There is no end block indicator, so just bail:
			return false;
		}
		strToFix.Delete(endIndex,strToFix.GetLength()-endIndex);
	}
	return true;
}

void KeyValue::StripStringWhiteSpace(TupString &strToFix)
{
	strToFix.TrimLeft();
	strToFix.TrimRight();
}

bool KeyValue::GetBeforeBlockStart(TupString& strToFix,const char *pBlockStart)
{
	assert(pBlockStart);
	// Find and remove the begin block indicator:
	int beginIndex = strToFix.Find(pBlockStart);
	if (beginIndex == -1)
	{
		strToFix.Empty();
		// Error - There is no begin block indicator, so just bail:
		return false;
	}
	strToFix.Delete(beginIndex,strToFix.GetLength());
	strToFix.TrimLeft();
	strToFix.TrimRight();
	return (strToFix.GetLength()!=0);
}

bool KeyValue::GetAfterBlockEnd(TupString& strToFix,const char *pBlockEnd)
{
	assert(pBlockEnd);
	// Find and remove the begin block indicator:
	int endIndex = strToFix.Find(pBlockEnd);
	if (endIndex == -1)
	{
		strToFix.Empty();
		// Error - There is no begin block indicator, so just bail:
		return false;
	}
	strToFix.Delete(0,endIndex+strlen(pBlockEnd));
	strToFix.TrimLeft();
	strToFix.TrimRight();
	return (strToFix.GetLength()!=0);
}

// ObjAttrController::RemoveCharsFromStr
// This method will look at the string passed in and remove all instances of 
// the specified character.
//
// Input:
//  TupString strToFix -- The string to remove characters from.
//  char chToRemove -- The character to remove.
//
// Output: True if characters were removed, false otherwise.
//-----------------------------------------------------------------------------
bool KeyValue::RemoveCharsFromStr(TupString& strToFix, char chToRemove)
{
	bool bCharsRemoved = false;

	int charIndex = strToFix.Find(chToRemove);
	while (charIndex != -1)
	{
		strToFix.Delete(charIndex, 1);
		charIndex = strToFix.Find(chToRemove);
		bCharsRemoved = true;
	}

	return bCharsRemoved;
}

//-----------------------------------------------------------------------------
// KeyValue::KeynameExistsAt
// This method will look in the current attribute data and return the index of
// where the keyname exists at.  If the keyname does not exist, a value of -1
// is returned.
//
// Input:
//  const char *pKey -- The keyname to look for.
//
// Output: Index of where keyname exists at.  -1 if it doesn't exist.
//-----------------------------------------------------------------------------
int KeyValue::KeyExistsAt(const char *pKey) const
{
	TupString key(pKey);
	return m_keyKeeper.GetIndex(key);
}

//-----------------------------------------------------------------------------
// KeyValue::KeynameExistsAt
// This method will look in the current attribute data and return the index of
// where the keyname exists at.  If the keyname does not exist, a value of -1
// is returned.
//
// Input:
//  const TupString &key -- The keyname to look for.
//
// Output: Index of where keyname exists at.  -1 if it doesn't exist.
//-----------------------------------------------------------------------------
int KeyValue::KeyExistsAt(const TupString &key) const
{
	return m_keyKeeper.GetIndex(key);
}

//-----------------------------------------------------------------------------
// KeyValue::GetKeyAsString
// This method will return the keyname (specified by the incoming index) as a
// string.
//
// Input:
//  int nIndex -- The index to get the key for.
//
// Output: A const char * of the key at the index specified.
//-----------------------------------------------------------------------------
const char *KeyValue::GetKeyAsString(int nIndex) const
{
	return (const char *)m_keyValuePairArray[nIndex].m_strKey;
}


//-----------------------------------------------------------------------------
// KeyValue::GetValueAsString
// This method will return the value (specified by the incoming index) as a
// string
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A const char *  of the value at index specified.
//-----------------------------------------------------------------------------
const char *KeyValue::GetValueAsString(int nIndex) const
{
	return (const char *)m_keyValuePairArray[nIndex].m_strValue;
}


//-----------------------------------------------------------------------------
// KeyValue::GetValueAsBool
// This method will return the value (specified by the incoming index) as a
// bool.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A bool of the value at index specified.
//-----------------------------------------------------------------------------
bool KeyValue::GetValueAsBool(int nIndex) const
{
	if (stricmp((const char *)m_keyValuePairArray[nIndex].m_strValue, "true") == 0)
		return(true);
	return(false);
}


//-----------------------------------------------------------------------------
// KeyValue::GetValueAsInt
// This method will return the value (specified by the incoming index) as an
// int.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: An int of the value at index specified.
//-----------------------------------------------------------------------------
int KeyValue::GetValueAsInt(int nIndex) const
{
	if (stricmp((const char *)m_keyValuePairArray[nIndex].m_strValue, "true") == 0)
		return(1);
	return atoi((const char *)m_keyValuePairArray[nIndex].m_strValue);
}


//-----------------------------------------------------------------------------
// KeyValue::GetValueAsFloat
// This method will return the value (specified by the incoming index) as a
// float.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A float of the value at index specified.
//-----------------------------------------------------------------------------
float KeyValue::GetValueAsFloat(int nIndex) const
{
	return (float)atof((const char *)m_keyValuePairArray[nIndex].m_strValue);
}


//-----------------------------------------------------------------------------
// KeyValue::GetValueAsColor
// This method will return the value (specified by the incoming index) as a
// color.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A color of the value at index specified.
//-----------------------------------------------------------------------------
AttribColor KeyValue::GetValueAsColor(int nIndex) const
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
// KeyValue::GetNumAttribs
// This method will return the number of attribs in this object.
//
// Input: None.
//
// Output: Int representing the number of attribs in this object.
//-----------------------------------------------------------------------------
int KeyValue::GetNumAttribs() const
{
	return m_keyValuePairArray.GetSize();
}

//-----------------------------------------------------------------------------
// KeyValue::AddKeyValue
// This method will return the number of attribs in this object.
//
// Input: 
//	const char *pKey - the key to add
//	const char *pValue - the value to add
// bool bReplace - if old value if key already there
//
// Output: Int representing the position within the array for the key-value
//-----------------------------------------------------------------------------
int KeyValue::AddKeyValue(const char *pKey,const char *pValue,bool bReplace)
{
	int findIndex = KeyExistsAt(pKey);
	if (findIndex==-1)
	{
		KeyValuePair newKeyValuePair;
		newKeyValuePair.m_strKey = pKey;
		newKeyValuePair.m_strValue = pValue;
		m_keyKeeper.Add(newKeyValuePair.m_strKey);
		m_keyValuePairArray.Add(newKeyValuePair);
		return m_keyValuePairArray.GetSize()-1;
	}
	else // if there is already a match then modify the value and return the location
	{
		if (bReplace)
		{
			m_keyValuePairArray[findIndex].m_strValue = pValue;
		}
		return findIndex;
	}
}

//-----------------------------------------------------------------------------
// KeyValue::AddKeyValue
// This method will return the number of attribs in this object.
//
// Input: 
//	const char *pKey - the key to add
//	const char *pValue - the value to add
// bool bReplace - if old value if key already there
//
// Output: Int representing the position within the array for the key-value
//-----------------------------------------------------------------------------
int KeyValue::AddKeyValue(const TupString &key,const TupString &value,bool bReplace)
{
	int findIndex = KeyExistsAt(key);
	if (findIndex==-1)
	{
		KeyValuePair newKeyValuePair;
		newKeyValuePair.m_strKey = key;
		newKeyValuePair.m_strValue = value;
		m_keyKeeper.Add(key);
		m_keyValuePairArray.Add(newKeyValuePair);
		return m_keyValuePairArray.GetSize()-1;
	}
	else // if there is already a match then modify the value and return the location
	{
		if (bReplace)
		{
			m_keyValuePairArray[findIndex].m_strValue = value;
		}
		return findIndex;
	}
}

// this function will update the the object with another
// new keys will be added or values will be replaces in the current object
// return true if we added or changed an entry
bool KeyValue::MergeKeyValues(const KeyValue &keysToAdd,bool bReplace)
{
	bool bChanged = false;
	int total = keysToAdd.GetNumAttribs();
	for (int attIndex = 0;attIndex<total;attIndex++)
	{
		int oldSize = m_keyValuePairArray.GetSize();
		int index = AddKeyValue(keysToAdd.GetKeyAsString(attIndex),keysToAdd.GetValueAsString(attIndex),bReplace);
		// if we were added or replaced
		if (index==oldSize || bReplace)
		{
			bChanged = true;
		}
	}
	return bChanged;
}

TupString KeyValue::BuildString(const char *pBuffer1,const char *pBuffer2,const char *pBlockStart,const char *pBlockEnd)
{
	TupString tempString;

	// make sure we can handle null pointers
	if (!pBuffer1) pBuffer1 = "";
	if (!pBuffer2) pBuffer2 = "";

	// create strings for holding before and after block headers
	TupString begin1(pBuffer1);
	RemoveCharsFromStr(begin1,'\r');
	TupString end1(begin1);
	TupString begin2(pBuffer2);
	RemoveCharsFromStr(begin2,'\r');
	TupString end2(begin2);

	// create a string that will hold the key/value pairs
	TupString keyValString;
	int total = GetNumAttribs();
	for (int attIndex = 0;attIndex<total;attIndex++)
	{
		keyValString+="\t\t\t\t";
		keyValString+= GetKeyAsString(attIndex);
		keyValString+= "=";
		keyValString+= GetValueAsString(attIndex);
		keyValString+= "\n";
	}

	// if there are block headers then extract the data around
	if (pBlockStart && pBlockEnd)
	{
		// save text around the headers
		if (GetBeforeBlockStart(begin1,pBlockStart))
		{
			tempString += begin1;
			tempString += "\n";
		}
		if (GetAfterBlockEnd(end1,pBlockEnd))
		{
			tempString += end1;
			tempString += "\n";
		}
		if (GetBeforeBlockStart(begin2,pBlockStart))
		{
			tempString += begin2;
			tempString += "\n";
		}
		if (GetAfterBlockEnd(end2,pBlockEnd))
		{
			tempString += end2;
			tempString += "\n";
		}
		// if the string is not empty then tab over from the newline
		if (tempString.GetLength())
		{
			tempString += "\t\t\t\t";
		}
		tempString += pBlockStart;
		tempString += "\n";
		tempString += keyValString;
		tempString += "\t\t\t\t";
		tempString += pBlockEnd;
	}
	else
	{
		tempString += keyValString;
	}
	return tempString;
}
