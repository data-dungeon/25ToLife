////////////////////////////////////////////////////////////////////////////
//
// KeyValueContext
//
// Class to deal with the key value pairs along with the context within the config tree
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: KeyValueContext.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/16/02    Time: 3:42p
 * Updated in $/Avalanche/tools/TupperAttrib
 * trim whitespace on value
 * don't check for case on checkbox value
 * "1" will work as a valid value
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 4:06p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added valid check
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 11:26a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Fix GetValueAsBool so that True will return true
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:20a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added check in GetValueAsInt to check for "True"
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

#include "TupperAttribPch.h"
#include "KeyValueContext.h"

TupKeyValueContext::TupKeyValueContext(void)
{
}

int TupKeyValueContext::GetNumAttribs(void) const
{
	return m_keyValuePairContextArray.GetSize();
}

// returns the index of the key or -1 if not found
int TupKeyValueContext::KeyExistsAt(const char *pKey) const
{
	int nIndexAt = -1;

	// Cycle through all the keys
	int nNumKeys = m_keyValuePairContextArray.GetSize();
	for (int nIndex = 0; nIndex < nNumKeys; nIndex++)
	{
		if (m_keyValuePairContextArray[nIndex].m_strKey == pKey)
			nIndexAt = nIndex;
	}
	return nIndexAt;
}

const TupConfigBase *TupKeyValueContext::GetControlPtr(int nIndex) const
{
	return m_keyValuePairContextArray[nIndex].m_pConfigContext;
}

const char *TupKeyValueContext::GetKeyAsString(int nIndex) const
{
	return (const char *)m_keyValuePairContextArray[nIndex].m_strKey;
}

const char *TupKeyValueContext::GetValueAsString(int nIndex) const
{
	return (const char *)m_keyValuePairContextArray[nIndex].m_strValue;
}

bool TupKeyValueContext::GetValueAsBool(int nIndex) const
{
	// if the value is "True" then return 1 without call atoi
	if (!stricmp(m_keyValuePairContextArray[nIndex].m_strValue,"true"))
		return true;

	int nIntValue = atoi((const char *)m_keyValuePairContextArray[nIndex].m_strValue);

	if (nIntValue == 0)
		return false;
	else
		return true;
	// TODO - Do some error checking to check for other values?
}

int TupKeyValueContext::GetValueAsInt(int nIndex) const
{
	// if the value is "True" then return 1 without call atoi
	if (!stricmp(m_keyValuePairContextArray[nIndex].m_strValue,"True"))
		return 1;

	return atoi((const char *)m_keyValuePairContextArray[nIndex].m_strValue);
}

float TupKeyValueContext::GetValueAsFloat(int nIndex) const
{
	return (float)atof((const char *)m_keyValuePairContextArray[nIndex].m_strValue);
}

TupAttribColor TupKeyValueContext::GetValueAsColor(int nIndex) const
{
	// Get the value for red:
	TupString strValue = m_keyValuePairContextArray[nIndex].m_strValue;
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
	TupString strName = strValue.Right(nFirstComma + 1);

	// Create the color and return it:
	TupAttribColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	returnColor.m_strName = strName;
	return returnColor;
}

int TupKeyValueContext::GetSelectionIndex(int nIndex) const
{
	return m_keyValuePairContextArray[nIndex].m_selectionIndex;
}

bool TupKeyValueContext::ValueValidAt(int nIndex) const
{
	// if this pointer is set then it is valid
	return m_keyValuePairContextArray[nIndex].m_pConfigContext!=0;
}


int TupKeyValueContext::AddKeyValue(const char *pKey,const char *pValue,int selectionIndex,const TupConfigBase *pConfigContext)
{
	int findIndex = KeyExistsAt(pKey);
	if (findIndex==-1)
	{
		KeyValuePairContext newKeyValuePair;
		newKeyValuePair.m_strKey = pKey;
		newKeyValuePair.m_strValue = pValue;
		newKeyValuePair.m_pConfigContext = pConfigContext;
		newKeyValuePair.m_selectionIndex = selectionIndex;
		m_keyValuePairContextArray.Add(newKeyValuePair);
		return m_keyValuePairContextArray.GetSize()-1;
	}
	else // if there is already a match then modify the value and return the location
	{
		m_keyValuePairContextArray[findIndex].m_strValue = pValue;
		m_keyValuePairContextArray[findIndex].m_pConfigContext = pConfigContext;
		m_keyValuePairContextArray[findIndex].m_selectionIndex = selectionIndex;
		return findIndex;
	}
}

int TupKeyValueContext::AddKeyValue(const TupString &key,const TupString &value,int selectionIndex,const TupConfigBase *pConfigContext)
{
	int findIndex = KeyExistsAt(key);
	if (findIndex==-1)
	{
		KeyValuePairContext newKeyValuePair;
		newKeyValuePair.m_strKey = key;
		newKeyValuePair.m_strValue = value;
		newKeyValuePair.m_pConfigContext = pConfigContext;
		newKeyValuePair.m_selectionIndex = selectionIndex;
		m_keyValuePairContextArray.Add(newKeyValuePair);
		return m_keyValuePairContextArray.GetSize()-1;
	}
	else // if there is already a match then modify the value and return the location
	{
		m_keyValuePairContextArray[findIndex].m_strValue = value;
		m_keyValuePairContextArray[findIndex].m_pConfigContext = pConfigContext;
		m_keyValuePairContextArray[findIndex].m_selectionIndex = selectionIndex;
		return findIndex;
	}
}

int TupKeyValueContext::AddKey(const char *pKey)
{
	int findIndex = KeyExistsAt(pKey);
	if (findIndex==-1)
	{
		KeyValuePairContext newKeyValuePair;
		newKeyValuePair.m_strKey = pKey;
		m_keyValuePairContextArray.Add(newKeyValuePair);
		return m_keyValuePairContextArray.GetSize()-1;
	}
	else
	{
		return findIndex;
	}
}

int TupKeyValueContext::AddKey(const TupString &key)
{
	int findIndex = KeyExistsAt(key);
	if (findIndex==-1)
	{
		KeyValuePairContext newKeyValuePair;
		newKeyValuePair.m_strKey = key;
		m_keyValuePairContextArray.Add(newKeyValuePair);
		return m_keyValuePairContextArray.GetSize()-1;
	}
	else
	{
		return findIndex;
	}
}
