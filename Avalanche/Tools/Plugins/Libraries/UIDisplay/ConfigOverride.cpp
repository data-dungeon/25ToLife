//-----------------------------------------------------------------------------
//	File:		DefaultOverride.cpp
//	Created:	Apr. 25, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The DefaultOverride class holds an array of key/value pairs used for 
//			overriding the defaults when building a config tree
//
//-----------------------------------------------------------------------------

/*
 * $History: ConfigOverride.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/UIDisplay
 * ui display changes to use TupString and TupArrays.  Some const stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 4:38p
 * Updated in $/Avalanche/tools/Plugins/Max/UIDisplay
 * Default overrides working - at least on this side
 */

#include "UIDisplayPCH.h"
#include "ConfigOverride.h"

// add a new entry to the end of the array
void ConfigOverrides::AddEntry(const char *key,const char *value,ConfigOverrides::OverrideType type)
{
	m_entries.Add(OverrideEntry(key,value,type));
}

// find a last match in the array that matches the key
int ConfigOverrides::KeyExistsAt(const char *key,ConfigOverrides::OverrideType type) const
{
	// Cycle through all the keynames in reverse order
	int nNumKeynames = m_entries.GetSize();
	for (int nIndex = nNumKeynames-1; nIndex >=0; nIndex--)
	{
		// if we find a match then stop and return
		if (m_entries[nIndex].m_key == key && type == m_entries[nIndex].m_type)
		{
			return nIndex;
		}
	}
	return -1;
}

int ConfigOverrides::GetSize(void) const
{
	return m_entries.GetSize();
}

void ConfigOverrides::SetSize(int size)
{
	m_entries.SetSize(size);
}

const char *ConfigOverrides::GetKeyAsString(int nIndex) const
{
	return m_entries[nIndex].m_key;
}

ConfigOverrides::OverrideType ConfigOverrides::GetType(int nIndex) const
{
	return (ConfigOverrides::OverrideType)m_entries[nIndex].m_type;
}

const char *ConfigOverrides::GetValueAsString(int nIndex) const
{
	return m_entries[nIndex].m_value;
}

int ConfigOverrides::GetValueAsInt(int nIndex) const
{
	return atoi(m_entries[nIndex].m_value);
}

float ConfigOverrides::GetValueAsFloat(int nIndex) const
{
	return (float) atof(m_entries[nIndex].m_value);
}

AttribColor ConfigOverrides::GetValueAsColor(int nIndex) const
{
	// Get the value for red:
	TupString strValue = m_entries[nIndex].m_key;
	int nFirstComma = strValue.Find(',');
	TupString strRed = strValue.Left(nFirstComma);
	int nRed = atoi(strRed);

	// Get the value for green:
	strValue = strValue.Mid(nFirstComma + 1);
	nFirstComma = strValue.Find(',');
	TupString strGreen = strValue.Left(nFirstComma);
	int nGreen = atoi(strGreen);

	// Get the value for blue:
	strValue = strValue.Mid(nFirstComma + 1);
	nFirstComma = strValue.Find(',');
	TupString strBlue = strValue.Left(nFirstComma);
	int nBlue = atoi(strBlue);

	// Get the name:
	TupString strName = strValue.Mid(nFirstComma + 1);

	// Create the color and return it:
	AttribColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	returnColor.m_strName = strName;
	return returnColor;
}

void ConfigOverrides::AddKeyPrefix(const char *value)
{
	m_entries.Add(OverrideEntry("dummy",value,OVERRIDE_KEY_PREFIX));
}

void ConfigOverrides::PopKeyPrefix(void)
{
	int numEntries = m_entries.GetSize();
	for (int index = numEntries-1;index>=0;index--)
	{
		if (m_entries[index].m_type == OVERRIDE_KEY_PREFIX)
		{
			// pull from array
			m_entries.RemoveAt(index);
			break; // done
		}
	}
}

void ConfigOverrides::AddKeyPostfix(const char *value)
{
	m_entries.Add(OverrideEntry("dummy",value,OVERRIDE_KEY_POSTFIX));
}

void ConfigOverrides::PopKeyPostfix(void)
{
	int numEntries = m_entries.GetSize();
	for (int index = numEntries-1;index>=0;index--)
	{
		if (m_entries[index].m_type == OVERRIDE_KEY_POSTFIX)
		{
			// pull from array
			m_entries.RemoveAt(index);
			break; // done
		}
	}
}

bool ConfigOverrides::HasPrefixOrPostfix(void) const
{
	int numEntries = m_entries.GetSize();

	for (int index = 0;index<numEntries;index++)
	{
		if (m_entries[index].m_type == OVERRIDE_KEY_POSTFIX)
		{
			return true;
		}
		if (m_entries[index].m_type == OVERRIDE_KEY_PREFIX)
		{
			return true;
		}
	}
	return false;
}

void ConfigOverrides::AddPrefixAndPostfix(TupString &string) const
{
	int numEntries = m_entries.GetSize();
	TupString newString(string);

	// apply from most recently applied to first applied
	for (int index = numEntries-1;index>=0;index--)
	{
		if (m_entries[index].m_type == OVERRIDE_KEY_POSTFIX)
		{
			newString+=m_entries[index].m_value;
		}
		if (m_entries[index].m_type == OVERRIDE_KEY_PREFIX)
		{
			newString.Insert(0,m_entries[index].m_value);
		}
	}
	string = newString;
}
