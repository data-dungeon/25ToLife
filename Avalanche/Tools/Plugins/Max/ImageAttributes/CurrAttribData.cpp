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

#include "ImageAttributesPCH.h"
#include "CurrAttribData.h"



//-----------------------------------------------------------------------------
// Class Definition:


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
void CurrAttribData::PrintDebug()
{
	DebugPrint("CURRENTATTRIBTDATA:\n");
	int nVectSize = m_vectAttribData.size();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		AttribKeynameValue currentAttrib = m_vectAttribData.at(nIndex);
		DebugPrint(currentAttrib.m_strKeyname.data());
		DebugPrint("=");
		DebugPrint(currentAttrib.m_strValue.data());
		DebugPrint("\n");
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
//  std::string strKeyname -- The keyname to look for.
//
// Output: Index of where keyname exists at.  -1 if it doesn't exist.
//-----------------------------------------------------------------------------
int CurrAttribData::KeynameExistsAt(std::string strKeyname)
{
	int nIndexAt = -1;

	// Cycle through all the keynames:
	int nNumKeynames = m_vectAttribData.size();
	for (int nIndex = 0; nIndex < nNumKeynames; nIndex++)
	{
		if (m_vectAttribData.at(nIndex).m_strKeyname == strKeyname)
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
// Output: A std::string of the key at the index specified.
//-----------------------------------------------------------------------------
std::string CurrAttribData::GetKeyAsString(int nIndex)
{
	return m_vectAttribData.at(nIndex).m_strKeyname;
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetValueAsString
// This method will return the value (specified by the incoming index) as a
// string
//
// Input:
//  int nIndex -- The index to get the value for.
//
// Output: A std::string of the value at index specified.
//-----------------------------------------------------------------------------
std::string CurrAttribData::GetValueAsString(int nIndex)
{
	return m_vectAttribData.at(nIndex).m_strValue;
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
bool CurrAttribData::GetValueAsBool(int nIndex)
{
	int nIntValue = atoi(m_vectAttribData.at(nIndex).m_strValue.data());

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
int CurrAttribData::GetValueAsInt(int nIndex)
{
	return atoi(m_vectAttribData.at(nIndex).m_strValue.data());
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
float CurrAttribData::GetValueAsFloat(int nIndex)
{
	return atof(m_vectAttribData.at(nIndex).m_strValue.data());
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
AttribColor CurrAttribData::GetValueAsColor(int nIndex)
{
	// Get the value for red:
	std::string strValue = m_vectAttribData.at(nIndex).m_strValue;
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
// Output: std::string representing all the keyname-value sets in the vector.
//		   The string passed back will be in the format:
//				keyname=value\n
//				keyname=value\n
//				keyname=value\n
//-----------------------------------------------------------------------------
std::string CurrAttribData::CurrObjDataToStr()
{
	std::string strToReturn = "";

	// Cycle through the data vector, forming the string as we go:
	int nNumDataSets = m_vectAttribData.size();
	for (int nIndex = 0; nIndex < nNumDataSets; nIndex++)
	{
		strToReturn.append(m_vectAttribData.at(nIndex).m_strKeyname);
		strToReturn.append("=");
		strToReturn.append(m_vectAttribData.at(nIndex).m_strValue);
		strToReturn.append("\n");
	}

	return strToReturn;
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetEntityString
// This method will return the m_pEntityString member.  This member points to the
// object or component that this attribute data pertains to.
//
// Input: None
//
// Output: void* representing the object or component.
//-----------------------------------------------------------------------------
const char *CurrAttribData::GetEntityString()
{
	return m_pEntityString.c_str();
}


//-----------------------------------------------------------------------------
// CurrAttribData::GetNumAttribs
// This method will return the number of attribs in this object.
//
// Input: None.
//
// Output: Int representing the number of attribs in this object.
//-----------------------------------------------------------------------------
int CurrAttribData::GetNumAttribs()
{
	return m_vectAttribData.size();
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
	if (nIndex >= 0 && nIndex < m_vectAttribData.size())
	{
		std::vector<AttribKeynameValue>::iterator pos = m_vectAttribData.begin();
		pos += nIndex;
		m_vectAttribData.erase(pos);
	}
}


//-----------------------------------------------------------------------------
// CurrAttribData::RemoveKeyname
// This method will remove all instances of the key specified.
//
// Input:
//  std::string strKeyname -- The keyname to remove.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::RemoveKeyname(std::string strKeyname)
{
	int nNumKeyVals = m_vectAttribData.size();
	for (int nIndex = nNumKeyVals - 1; nIndex >= 0; nIndex--)
	{
		if (m_vectAttribData.at(nIndex).m_strKeyname == strKeyname)
		{
			std::vector<AttribKeynameValue>::iterator pos = m_vectAttribData.begin();
			pos += nIndex;
			m_vectAttribData.erase(pos);			
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
void CurrAttribData::RemoveAllKeynames()
{
	m_vectAttribData.clear();
}


//-----------------------------------------------------------------------------
// CurrAttribData::SetStringAsString
// This method will add an entry to the current attrib data vector.  It will
// use the string passed in.
//
// Input:
//	std::string strKeyname -- The keyname to use for this attribute.
//  std::string strValue -- The value to use for this attribute.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::SetStringAsString(std::string strKeyname, std::string strValue)
{
	AttribKeynameValue newEntry;
	newEntry.m_strKeyname = strKeyname;
	newEntry.m_strValue = strValue;
	m_vectAttribData.push_back(newEntry);
}


//-----------------------------------------------------------------------------
// CurrAttribData::SetBoolAsString
// This method will add an entry to the current attrib data vector.  It will
// use the boolean passed in.
//
// Input:
//  std::string strKeyname -- The keyname to use for this attribute.
//  bool bValue -- The boolean value to use for this attribute (converted to a
//				   string.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::SetBoolAsString(std::string strKeyname, bool bValue)
{
	AttribKeynameValue newEntry;
	newEntry.m_strKeyname = strKeyname;
	if (bValue == true)
		newEntry.m_strValue = "1";
	else
		newEntry.m_strValue = "0";
	m_vectAttribData.push_back(newEntry);	
}


//-----------------------------------------------------------------------------
// CurrAttribData::SetIntAsString
// This method will add an entry to the current attrib data vector.  It will
// use the integer passed in.
//
// Input:
//  std::string strKeyname -- The keyname to use for this attribute.
//  int nValue -- The int value to use for this attribute (converted to a
//				   string.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::SetIntAsString(std::string strKeyname, int nValue)
{
	AttribKeynameValue newEntry;
	newEntry.m_strKeyname = strKeyname;
	char pChValue[50];
	newEntry.m_strValue = itoa(nValue, pChValue, 10);
	m_vectAttribData.push_back(newEntry);
}


//-----------------------------------------------------------------------------
// CurrAttribData::SetFloatAsString
// This method will add an entry to the current attrib data vector.  It will
// use the float passed in.
//
// Input:
//  std::string strKeyname -- The keyname to use for this attribute.
//  float fValue -- The float value to use for this attribute (converted to a
//					string.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::SetFloatAsString(std::string strKeyname, float fValue)
{
	AttribKeynameValue newEntry;
	newEntry.m_strKeyname = strKeyname;
	char pChValue[50];
	newEntry.m_strValue = gcvt(fValue, 10, pChValue);
	m_vectAttribData.push_back(newEntry);
}


//-----------------------------------------------------------------------------
// CurrAttribData::SetColorAsString
// This method will add an entry to the current attrib data vector.  It will
// use the color passed in.
//
// Input:
//  std::string strKeyname -- The keyname to use for this attribute.
//  AttribColor colorValue -- The color value to use for this attribute
//							  (converted to a string).
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::SetColorAsString(std::string strKeyname, AttribColor colorValue)
{
	AttribKeynameValue newEntry;

	// Set the keyname:
	newEntry.m_strKeyname = strKeyname;

	// Set the value:
	std::string strColor = "";
	char chBuff[50];
	strColor.append(itoa(colorValue.m_nRed, chBuff, 10));
	strColor.append(",");
	strColor.append(itoa(colorValue.m_nGreen, chBuff, 10));
	strColor.append(",");
	strColor.append(itoa(colorValue.m_nBlue, chBuff, 10));
	strColor.append(",");
	strColor.append(colorValue.m_strName);
	newEntry.m_strValue = strColor;

	// Add it to the vector:
	m_vectAttribData.push_back(newEntry);
}


//-----------------------------------------------------------------------------
// CurrAttribData::SetEntityPtr
// This method will set the m_pEntity data member to point to whatever object
// or component that is passed in.  This pointer should point to the object
// or component that the attrib data pertains to.
//
// Input:
//  void* pNewEntity -- The new object or component to point to.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::SetEntityString(const char *pNewEntityString)
{
	m_pEntityString = pNewEntityString;
}


//-----------------------------------------------------------------------------
// CurrAttribData::StrToCurrAttribData
// This method will remove everything in the attribute data vector and reset it
// with the data passed in.
// NOTE:  The string passed in should be in the format:
//			keyname=value\n
//			keyname=value\n
//			keyname=value\n		This last newline is optional.
//
// Input:
//  std::string strData -- Represents the raw data to store in the attrib data
//						   vector.  One word of warning: the input string
//						   should have all cariage returns removed from it!
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribData::StrToCurrAttribData(std::string strData)
{
	// Free up the data vector:
	m_vectAttribData.clear();

	// Parse through the attributes:
	while (strData.empty() != true)
	{
		// Parse the current line:
		std::string::size_type equalsIndex = strData.find('=');
		if (equalsIndex != std::string::npos)
		{
			std::string strKeyname;
			std::string strValue;

			// Get the keyname:
			strKeyname = strData.substr(0, equalsIndex);

			// Get the value:
			std::string::size_type newLineIndex = strData.find('\n');
			if (newLineIndex != std::string::npos)
			{
				strValue = strData.substr(equalsIndex + 1, newLineIndex - (equalsIndex + 1));
				strData.erase(0, newLineIndex + 1);
			}
			else
			{
				std::string::size_type nSize = strData.size();
				strValue = strData.substr(equalsIndex + 1, nSize - (equalsIndex + 1));
				// If there is an equals, but no newline, then assume we are on
				// the last attribute, so clear the string thus forcing a bailout:
				strData.erase();
			}

			// Add the attribute to the data vector:
			this->SetStringAsString(strKeyname, strValue);
		}
		else
		{
			// There is no equals left in the string, so empty out the string
			// and bail out:
			strData.erase();
		}
	}

}
