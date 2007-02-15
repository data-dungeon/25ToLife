////////////////////////////////////////////////////////////////////////////
//
// Attributes
//
// Class deal the attributes of nodes and faces
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Attributes.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Attributes class
 */

#include "Tupper3dPch.h"
#include "Attributes.h"

//-----------------------------------------------------------------------------
// TupAttributes::TupAttributes
// This method is the default constructor for this class.
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
TupAttributes::TupAttributes(const char *pBuffer,const char *pBlockStart,const char *pBlockEnd)
{
	std::string tempStr(pBuffer);

	// Remove carriage returns from the string:
	RemoveCharsFromStr(tempStr,'\r');

	// Get only data within the block strings (if requested)
	StripAllButRawData(tempStr,pBlockStart,pBlockEnd);

	// Parse through the attributes:
	while (tempStr.empty() != true)
	{
		// Parse the current line:
		std::string::size_type equalsIndex = tempStr.find('=');
		if (equalsIndex != std::string::npos)
		{
			std::string strKeyname;
			std::string strValue;

			// Get the keyname:
			strKeyname = tempStr.substr(0, equalsIndex);

			// Get the value:
			std::string::size_type newLineIndex = tempStr.find('\n');
			if (newLineIndex != std::string::npos)
			{
				strValue = tempStr.substr(equalsIndex + 1, newLineIndex - (equalsIndex + 1));
				tempStr.erase(0, newLineIndex + 1);
			}
			else
			{
				std::string::size_type nSize = tempStr.size();
				strValue = tempStr.substr(equalsIndex + 1, nSize - (equalsIndex + 1));
				// If there is an equals, but no newline, then assume we are on
				// the last attribute, so clear the string thus forcing a bailout:
				tempStr.erase();
			}

			// Add the attribute to the data vector:
			KeyValuePair newEntry;
			newEntry.m_strKey = strKeyname;
			newEntry.m_strValue = strValue;
			m_keyValuePairArray.push_back(newEntry);
		}
		else
		{
			// There is no equals left in the string, so empty out the string
			// and bail out:
			tempStr.erase();
		}
	}

}

bool TupAttributes::StripAllButRawData(std::string& strToFix,const char *pBlockStart,const char *pBlockEnd)
{
	if (pBlockStart && pBlockEnd)
	{
		// Find and remove the begin block indicator:
		std::string::size_type beginIndex = strToFix.find(pBlockStart);
		if (beginIndex == std::string::npos)
		{
			// Error - There is no begin block indicator, so just bail:
			return false;
		}
		strToFix.erase(0, beginIndex);
		std::string::size_type newLineIndex = strToFix.find('\n');
		strToFix.erase(0, newLineIndex + 1);

		// Find and remove the end block indicator:
		std::string::size_type endIndex = strToFix.find(pBlockEnd);
		if (endIndex == std::string::npos)
		{
			// Error - There is no end block indicator, so just bail:
			return false;
		}
		strToFix.erase(endIndex);
	}
	return true;
}

// ObjAttrController::RemoveCharsFromStr
// This method will look at the string passed in and remove all instances of 
// the specified character.
//
// Input:
//  std::string strToFix -- The string to remove characters from.
//  char chToRemove -- The character to remove.
//
// Output: True if characters were removed, false otherwise.
//-----------------------------------------------------------------------------
bool TupAttributes::RemoveCharsFromStr(std::string& strToFix, char chToRemove)
{
	bool bCharsRemoved = false;

	std::string::size_type charIndex = strToFix.find(chToRemove);
	while (charIndex != std::string::npos)
	{
		strToFix.erase(charIndex, 1);
		charIndex = strToFix.find(chToRemove);
		bCharsRemoved = true;
	}

	return bCharsRemoved;
}

//-----------------------------------------------------------------------------
// TupAttributes::KeynameExistsAt
// This method will look in the current attribute data and return the index of
// where the keyname exists at.  If the keyname does not exist, a value of -1
// is returned.
//
// Input:
//  const char *pKey -- The keyname to look for.
//
// Output: Index of where keyname exists at.  -1 if it doesn't exist.
//-----------------------------------------------------------------------------
int TupAttributes::KeyExistsAt(const char *pKey) const
{
	int nIndexAt = -1;

	// Cycle through all the keys
	int nNumKeys = m_keyValuePairArray.size();
	for (int nIndex = 0; nIndex < nNumKeys; nIndex++)
	{
		if (m_keyValuePairArray.at(nIndex).m_strKey == pKey)
			nIndexAt = nIndex;
	}
	return nIndexAt;
}


//-----------------------------------------------------------------------------
// TupAttributes::GetKeyAsString
// This method will return the keyname (specified by the incoming index) as a
// string.
//
// Input:
//  int nIndex -- The index to get the key for.
//
// Output: A const char * of the key at the index specified.
//-----------------------------------------------------------------------------
const char *TupAttributes::GetKeyAsString(int nIndex) const
{
	return m_keyValuePairArray.at(nIndex).m_strKey.c_str();
}


//-----------------------------------------------------------------------------
// TupAttributes::GetValueAsString
// This method will return the value (specified by the incoming index) as a
// string
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A const char *  of the value at index specified.
//-----------------------------------------------------------------------------
const char *TupAttributes::GetValueAsString(int nIndex) const
{
	return m_keyValuePairArray.at(nIndex).m_strValue.c_str();
}


//-----------------------------------------------------------------------------
// TupAttributes::GetValueAsBool
// This method will return the value (specified by the incoming index) as a
// bool.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A bool of the value at index specified.
//-----------------------------------------------------------------------------
bool TupAttributes::GetValueAsBool(int nIndex) const
{
	int nIntValue = atoi(m_keyValuePairArray.at(nIndex).m_strValue.c_str());

	if (nIntValue == 0)
		return false;
	else
		return true;
	// TODO - Do some error checking to check for other values?
}


//-----------------------------------------------------------------------------
// TupAttributes::GetValueAsInt
// This method will return the value (specified by the incoming index) as an
// int.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: An int of the value at index specified.
//-----------------------------------------------------------------------------
int TupAttributes::GetValueAsInt(int nIndex) const
{
	return atoi(m_keyValuePairArray.at(nIndex).m_strValue.c_str());
}


//-----------------------------------------------------------------------------
// TupAttributes::GetValueAsFloat
// This method will return the value (specified by the incoming index) as a
// float.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A float of the value at index specified.
//-----------------------------------------------------------------------------
float TupAttributes::GetValueAsFloat(int nIndex) const
{
	return atof(m_keyValuePairArray.at(nIndex).m_strValue.c_str());
}


//-----------------------------------------------------------------------------
// TupAttributes::GetValueAsColor
// This method will return the value (specified by the incoming index) as a
// color.
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A color of the value at index specified.
//-----------------------------------------------------------------------------
TupAttributes::AttribColor TupAttributes::GetValueAsColor(int nIndex) const
{
	// Get the value for red:
	std::string strValue = m_keyValuePairArray.at(nIndex).m_strValue;
	int nFirstComma = strValue.find(',');
	std::string strRed = strValue.substr(0, nFirstComma);
	int nRed = atoi(strRed.data());

	// Get the value for green:
	strValue = strValue.substr(nFirstComma + 1);
	nFirstComma = strValue.find(',');
	std::string strGreen = strValue.substr(0, nFirstComma);
	int nGreen = atoi(strGreen.data());

	// Get the value for blue:
	strValue = strValue.substr(nFirstComma + 1);
	nFirstComma = strValue.find(',');
	std::string strBlue = strValue.substr(0, nFirstComma);
	int nBlue = atoi(strBlue.data());

	// Get the name:
	std::string strName = strValue.substr(nFirstComma + 1);

	// Create the color and return it:
	AttribColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	return returnColor;
}

//-----------------------------------------------------------------------------
// TupAttributes::GetNumAttribs
// This method will return the number of attribs in this object.
//
// Input: None.
//
// Output: Int representing the number of attribs in this object.
//-----------------------------------------------------------------------------
int TupAttributes::GetNumAttribs() const
{
	return m_keyValuePairArray.size();
}
