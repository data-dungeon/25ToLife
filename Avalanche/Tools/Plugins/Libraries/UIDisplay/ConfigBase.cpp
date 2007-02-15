//-----------------------------------------------------------------------------
//	File:		ConfigBase.cpp
//	Created:	Nov. 15, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigBase class is the root class for the UI data tree.  The
//			UI data tree is the tree that basically mimicks the configuration
//			file.  It is used to store all the possible ui for the custom
//			attributes (with parent-child relationships).
//
//	History:	11/15/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigBase.h"



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ConfigBase::ConfigBase
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigBase::ConfigBase() : 
	m_bNonUI(false),
	m_bDisabled(false),
	m_bExclusive(true),
	m_nAddedIndent(0),
	m_bCRCComputed(false),
	m_CRCValue(-1)
{
}


//-----------------------------------------------------------------------------
// ConfigBase::~ConfigBase
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigBase::~ConfigBase()
{
}


//-----------------------------------------------------------------------------
// ConfigBase::ReturnKeyNameSubstr
// This method will look at the string passed in and return the keyname
// substring part of the string (the part before the delimiter).  If there is
// no delimiter, then the input string is just returned.
//
// Input:
//  TupString strToParse -- String to get the keyname substr out of.
//	const char* pChDelimiter -- Character to use for the delimiter.
//
// Output: TupString representing the keyname.
//-----------------------------------------------------------------------------
TupString ConfigBase::ReturnKeyNameSubstr(const TupString &strToParse,const char* pChDelimiter)
{
	int nDividerIndex = strToParse.Find(pChDelimiter);
	if (nDividerIndex < 0)
		return strToParse;
	else
		return strToParse.Left(nDividerIndex);
}


//-----------------------------------------------------------------------------
// ConfigBase::ReturnLabelNameSubstr
// This method will look at the string passed in and return the label
// substring part of the string (the part after the delimiter).  If there is no
// delimiter, then the input string is just returned.
//
// Input:
//  TupString strToParse -- String to get the label substr out of.
//	char* pChDelimiter -- Character to use for the delimiter.
//
// Output: TupString representing the label.
//-----------------------------------------------------------------------------
TupString ConfigBase::ReturnLabelNameSubstr(const TupString &strToParse, const char* pChDelimiter)
{
	int nDividerIndex = strToParse.Find(pChDelimiter);
	if (nDividerIndex < 0)
		return strToParse;
	else
		return strToParse.Mid(nDividerIndex + 1);
}

//-----------------------------------------------------------------------------
// ConfigBase::StrToColor
// This method will take the incoming string and convert it into an AttribColor.
//
// Input:
//  TupString strColor -- The string to convert.
//
// Output: AttribColor converted from the string.
//-----------------------------------------------------------------------------
AttribColor ConfigBase::StringToColor(TupString strColor)
{
	// Get the value for red:
	int nFirstComma = strColor.Find(',');
	TupString strRed = strColor.Left(nFirstComma);
	int nRed = atoi((const char *)strRed);

	// Get the value for green:
	strColor = strColor.Right(nFirstComma + 1);
	nFirstComma = strColor.Find(',');
	TupString strGreen = strColor.Left(nFirstComma);
	int nGreen = atoi((const char *)strGreen);

	// Get the value for blue:
	strColor = strColor.Right(nFirstComma + 1);
	nFirstComma = strColor.Find(',');
	TupString strBlue = strColor.Left(nFirstComma);
	int nBlue = atoi((const char *)strBlue);

	// Get the name:
	TupString strName = strColor.Right(nFirstComma + 1);

	// Create the color and return it:
	AttribColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	returnColor.m_strName = strName;
	return returnColor;
}

//-----------------------------------------------------------------------------
// ConfigBase::StrToColor
// This method will take the incoming string and convert it into an AttribColor.
//
// Input:
//  TupString strColor -- The string to convert.
//
// Output: AttribColor converted from the string.
//-----------------------------------------------------------------------------
TupString ConfigBase::ColorToString(AttribColor color)
{
	char buffer[80];
	sprintf(buffer,"%d,%d,%d,%s",color.m_nRed,color.m_nGreen,color.m_nBlue,(const char *)color.m_strName);
	return TupString(buffer);
}

//-----------------------------------------------------------------------------
// ConfigBase::GetNonUI
// This method will return the m_bNonUI bool
//
// Output: bool -- current value of the m_bNonUI bool (true if nonUI)
//-----------------------------------------------------------------------------
bool ConfigBase::GetNonUI(void)	const
{
	return m_bNonUI;
}

//-----------------------------------------------------------------------------
// ConfigBase::SetNonUI
// This method will allow for the derived classed to set the m_bNonUI bool
//
// Input:
//  bool bNonUI -- new value of the m_bNonUI bool (true if nonUI)
//
//-----------------------------------------------------------------------------
void ConfigBase::SetNonUI(bool bNonUI)
{
	m_bNonUI = bNonUI;
}

//-----------------------------------------------------------------------------
// ConfigBase::GetDisabled
// This method will return the m_bDisabled bool
//
// Output: bool -- current value of the m_bDisabled bool (true if disabled)
//-----------------------------------------------------------------------------
bool ConfigBase::GetDisabled(void) const
{
	return m_bDisabled;
}

//-----------------------------------------------------------------------------
// ConfigBase::SetDisabled
// This method will allow for the derived classed to set the m_bDisabled bool
//
// Input:
//  bool bDisabled -- new value of the m_bDisabled bool (true if disabled)
//
//-----------------------------------------------------------------------------
void ConfigBase::SetDisabled(bool bDisabled)
{
	m_bDisabled = bDisabled;
}

//-----------------------------------------------------------------------------
// GetExclusive
// This method is an accessor for the m_bExclusive member variable.
//
// Input: None.
//
// Output: Bool representing the m_bExclusive value.
//-----------------------------------------------------------------------------
bool ConfigBase::GetExclusive(void)	const
{
	return m_bExclusive;
}


//-----------------------------------------------------------------------------
// ConfigBase::SetExclusive
// This method is a mutator for the m_bExclusive member variable.
//
// Input:
//  bool bExclusive -- The value to set the m_bExclusive to.
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigBase::SetExclusive(bool bExclusive)
{
	m_bExclusive = bExclusive;
}


//-----------------------------------------------------------------------------
// ConfigBase::GetAddedIndent
// This method is an accessor for the m_nAddedIndent member variable.
//
// Input: None.
//
// Output: int representing the m_nAddedIndent value.
//-----------------------------------------------------------------------------
int ConfigBase::GetAddedIndent(void) const
{
	return m_nAddedIndent;
}


//-----------------------------------------------------------------------------
// ConfigBase::SetAddedIndent
// This method is a mutator for the m_nAddedIndent member variable.
//
// Input:
//  int nAddedIndent -- The value to set the m_nAddedIndent to.
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigBase::SetAddedIndent(int nAddedIndent)
{
	m_nAddedIndent = nAddedIndent;
}


//-----------------------------------------------------------------------------
// ConfigBase::GetComment
// This method is a accessor for the m_strComment member variable.
//
// Input: None.
//
// Output: TupString representing the m_strComment value
//-----------------------------------------------------------------------------
TupString ConfigBase::GetComment(void) const
{
	return m_strComment;
}

//-----------------------------------------------------------------------------
// ConfigBase::SetComment
// This method is a mutator for the m_strComment member variable.
//
// Input:
//  TupString strComment -- The value to set the m_strComment to.
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigBase::SetComment(TupString strComment)
{
	m_strComment = strComment;
	m_strComment.Replace("|","\n"); // turn | into newlines
}

//-----------------------------------------------------------------------------
// ConfigBase::HasComment
// This method is a accessor to indicate if it has a valid comment
//
// Input: None.
//
// Output: bool -- if the comment is valid or not
//-----------------------------------------------------------------------------
bool ConfigBase::HasComment(void) const
{
	return (m_strComment.GetLength()!=0);
}

//-----------------------------------------------------------------------------
// ConfigBase::FindDisplayType
// This method will return the display type (DEFAULT, UNIQUE, or INDETERMINATE)
// for the control based on the current attribute data passed in.
//
// Input:
//  TupString strKeyname -- The keyname to get the values for.
//  TupString strDefaultVal -- The default value for the key.
//  CurrAttribBase* pAttribBase -- The current attribute data.
//  TupString& strUniqueVal -- On return, if the display type is found to be
//								 UNIQUE, this string will hold the unique val.
//
// Output: Either DEFAULT, UNIQUE, or INDETERMINATE.
//-----------------------------------------------------------------------------
ConfigBase::DisplayType ConfigBase::FindDisplayType(const TupString &strKeyname, const TupString &strDefaultVal,
													CurrAttribBase* pAttribBase, UniqueValues &uniqueValues, Keeper<TupString> *pValidValues) const
{
	DisplayType dispType;

	if (pAttribBase != NULL)
	{
		pAttribBase->GetAllUniqueValues(strKeyname, strDefaultVal, uniqueValues, pValidValues);

		// See whether to show the default, a unique value, or the indeterminate state:
		// if the selection is not using the same value
		if (uniqueValues.m_uniqueIndex==-1)
		{
			// Show the indeterminate state:
			dispType = INDETERMINATE;
		}
		else 
		{
			// if the unique index is the default index
			if (uniqueValues.m_uniqueIndex==uniqueValues.m_defaultIndex)
			{
				dispType = DEFAULT;
			}
			else
			{
				dispType = UNIQUE;
			}
		}
	}
	else
	{
		// this should not happen but we will handle this case
		uniqueValues.m_uniqueValues.RemoveAll();
		uniqueValues.m_defaultIndex = 0;
		uniqueValues.m_totalCount = 0;
		uniqueValues.m_totalHaveValue = 0;
		uniqueValues.m_uniqueIndex = 0;
		UniqueValue newUniqueValue;
		newUniqueValue.m_count = 0;
		newUniqueValue.m_strValue = strDefaultVal;
		uniqueValues.m_uniqueValues.Add(newUniqueValue);

		// Show the default value:
		dispType = DEFAULT;
	}

	return dispType;
}

//-----------------------------------------------------------------------------
// ConfigBase::GetCRC
// This method returns a CRC value for the entry in the config tree.  If this entry
// has children, these CRC values of these entries are also used.
// The primary purpose of the CRC is to allow common children to be shown when a parent
// has multiple selections
// 
// In order the CRC to be computed correctly for derived classes, each derived class
// must implement the ComputeCRC function
//
// Input: none
//
// Output: unsigned long -- CRC value for class instance
//
//-----------------------------------------------------------------------------
unsigned long ConfigBase::GetCRC(void)
{
	if (!m_bCRCComputed) // only do the compute step once
	{
		m_CRCValue = ComputeCRC();
		m_bCRCComputed = true;
	}
	return m_CRCValue;
}

//-----------------------------------------------------------------------------
// ConfigBase::GetCRC
// This method returns a CRC value for the entry in the config tree.  If this entry
// has children, these CRC values of these entries are also used.
// The primary purpose of the CRC is to allow common children to be shown when a parent
// has multiple selections
// 
// In order the CRC to be computed correctly for derived classes, each derived class
// must implement this function
//
// Input: none
//
// Output: unsigned long -- CRC value for class instance
//
//-----------------------------------------------------------------------------
unsigned long ConfigBase::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData((int)GetControlType());
	CRCGenerator.AnalyzeData(m_bNonUI);
	CRCGenerator.AnalyzeData(m_bDisabled);
	CRCGenerator.AnalyzeData(m_bExclusive);
	CRCGenerator.AnalyzeData(m_nAddedIndent);
	CRCGenerator.AnalyzeData(m_strComment);
	return CRCGenerator.GetResult();
}