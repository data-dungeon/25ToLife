////////////////////////////////////////////////////////////////////////////
//
// KeyValueContext
//
// Class to deal with the key value pairs along with the context within the config tree
//
////////////////////////////////////////////////////////////////////////////

#include "UIDisplayPch.h"
#include "KeyValueContext.h"

KeyValueContext::KeyValueContext(void)
{
}

int KeyValueContext::GetNumAttribs(void) const
{
	return m_keyValuePairContextArray.GetSize();
}

// returns the index of the key or -1 if not found
int KeyValueContext::KeyExistsAt(const char *pKey) const
{
	TupString key(pKey);
	return m_keyKeeper.GetIndex(key);
}

// returns the index of the key or -1 if not found
int KeyValueContext::KeyExistsAt(const TupString &key) const
{
	return m_keyKeeper.GetIndex(key);
}

const ConfigBase *KeyValueContext::GetControlPtr(int nIndex) const
{
	return m_keyValuePairContextArray[nIndex].m_pConfigContext;
}

bool KeyValueContext::IsDefaultValue(int nIndex) const
{
	return m_keyValuePairContextArray[nIndex].m_bIsDefaultValue;
}

int KeyValueContext::GetKeyValueIndex(int nIndex) const
{
	return m_keyValuePairContextArray[nIndex].m_keyValueIndex;
}

const char *KeyValueContext::GetDefaultValueAsString(int nIndex) const
{
	return m_keyValuePairContextArray[nIndex].m_strDefaultValue;
}

const char *KeyValueContext::GetKeyAsString(int nIndex) const
{
	return (const char *)m_keyValuePairContextArray[nIndex].m_strKey;
}

const char *KeyValueContext::GetValueAsString(int nIndex) const
{
	return (const char *)m_keyValuePairContextArray[nIndex].m_strValue;
}

bool KeyValueContext::GetValueAsBool(int nIndex) const
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

int KeyValueContext::GetValueAsInt(int nIndex) const
{
	// if the value is "True" then return 1 without call atoi
	if (!stricmp(m_keyValuePairContextArray[nIndex].m_strValue,"True"))
		return 1;

	return atoi((const char *)m_keyValuePairContextArray[nIndex].m_strValue);
}

float KeyValueContext::GetValueAsFloat(int nIndex) const
{
	return (float)atof((const char *)m_keyValuePairContextArray[nIndex].m_strValue);
}

AttribColor KeyValueContext::GetValueAsColor(int nIndex) const
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
	AttribColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	returnColor.m_strName = strName;
	return returnColor;
}

int KeyValueContext::GetSelectionIndex(int nIndex) const
{
	return m_keyValuePairContextArray[nIndex].m_selectionIndex;
}

bool KeyValueContext::ValueValidAt(int nIndex) const
{
	// if this pointer is set then it is valid
	return m_keyValuePairContextArray[nIndex].m_pConfigContext!=0;
}


int KeyValueContext::AddKeyValue(const char *pKey,const char *pValue,int selectionIndex,const ConfigBase *pConfigContext,bool bIsDefaultValue,const char *pDefaultValue,int keyValueIndex)
{
	int findIndex = KeyExistsAt(pKey);
	if (findIndex==-1)
	{
		KeyValuePairContext newKeyValuePair;
		newKeyValuePair.m_strKey = pKey;
		newKeyValuePair.m_strValue = pValue;
		newKeyValuePair.m_pConfigContext = pConfigContext;
		newKeyValuePair.m_selectionIndex = selectionIndex;
		newKeyValuePair.m_bIsDefaultValue = bIsDefaultValue;
		newKeyValuePair.m_strDefaultValue = pDefaultValue;
		newKeyValuePair.m_keyValueIndex = keyValueIndex;
		m_keyKeeper.Add(newKeyValuePair.m_strKey);
		m_keyValuePairContextArray.Add(newKeyValuePair);
		return m_keyValuePairContextArray.GetSize()-1;
	}
	else // if there is already a match then modify the value and return the location
	{
		m_keyValuePairContextArray[findIndex].m_strValue = pValue;
		m_keyValuePairContextArray[findIndex].m_pConfigContext = pConfigContext;
		m_keyValuePairContextArray[findIndex].m_selectionIndex = selectionIndex;
		m_keyValuePairContextArray[findIndex].m_bIsDefaultValue = bIsDefaultValue;
		m_keyValuePairContextArray[findIndex].m_strDefaultValue = pDefaultValue;
		m_keyValuePairContextArray[findIndex].m_keyValueIndex = keyValueIndex;
		return findIndex;
	}
}

int KeyValueContext::AddKeyValue(const TupString &key,const TupString &value,int selectionIndex,const ConfigBase *pConfigContext, bool bIsDefaultValue,const char *pDefaultValue,int keyValueIndex)
{
	int findIndex = KeyExistsAt(key);
	if (findIndex==-1)
	{
		KeyValuePairContext newKeyValuePair;
		newKeyValuePair.m_strKey = key;
		newKeyValuePair.m_strValue = value;
		newKeyValuePair.m_pConfigContext = pConfigContext;
		newKeyValuePair.m_selectionIndex = selectionIndex;
		newKeyValuePair.m_bIsDefaultValue = bIsDefaultValue;
		newKeyValuePair.m_strDefaultValue = pDefaultValue;
		newKeyValuePair.m_keyValueIndex = keyValueIndex;
		m_keyKeeper.Add(key);
		m_keyValuePairContextArray.Add(newKeyValuePair);
		return m_keyValuePairContextArray.GetSize()-1;
	}
	else // if there is already a match then modify the value and return the location
	{
		m_keyValuePairContextArray[findIndex].m_strValue = value;
		m_keyValuePairContextArray[findIndex].m_pConfigContext = pConfigContext;
		m_keyValuePairContextArray[findIndex].m_selectionIndex = selectionIndex;
		m_keyValuePairContextArray[findIndex].m_bIsDefaultValue = bIsDefaultValue;
		m_keyValuePairContextArray[findIndex].m_strDefaultValue = pDefaultValue;
		m_keyValuePairContextArray[findIndex].m_keyValueIndex = keyValueIndex;
		return findIndex;
	}
}

int KeyValueContext::AddKey(const char *pKey)
{
	int findIndex = KeyExistsAt(pKey);
	if (findIndex==-1)
	{
		KeyValuePairContext newKeyValuePair;
		newKeyValuePair.m_strKey = pKey;
		m_keyKeeper.Add(newKeyValuePair.m_strKey);
		m_keyValuePairContextArray.Add(newKeyValuePair);
		return m_keyValuePairContextArray.GetSize()-1;
	}
	else
	{
		return findIndex;
	}
}

int KeyValueContext::AddKey(const TupString &key)
{
	int findIndex = KeyExistsAt(key);
	if (findIndex==-1)
	{
		KeyValuePairContext newKeyValuePair;
		newKeyValuePair.m_strKey = key;
		m_keyKeeper.Add(key);
		m_keyValuePairContextArray.Add(newKeyValuePair);
		return m_keyValuePairContextArray.GetSize()-1;
	}
	else
	{
		return findIndex;
	}
}
