//-----------------------------------------------------------------------------
//	File:		TupConfigBase.cpp
//	Created:	Nov. 15, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TupConfigBase class is the root class for the UI data tree.  The
//			UI data tree is the tree that basically mimicks the configuration
//			file.  It is used to store all the possible ui for the custom
//			attributes (with parent-child relationships).
//
//	History:	11/15/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: ConfigBase.cpp $
 * 
 * *****************  Version 16  *****************
 * User: Brad Worthen Date: 8/29/02    Time: 2:09p
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 5/15/02    Time: 2:21p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Alternate memory manager hooked in
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */


//-----------------------------------------------------------------------------
// Includes:

#include "TupperAttribPch.h"
#include "ConfigBase.h"
//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// TupConfigBase::TupConfigBase
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigBase::TupConfigBase() : m_bNonUI(false) , m_bExclusive(true)
{
}


//-----------------------------------------------------------------------------
// TupConfigBase::~TupConfigBase
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigBase::~TupConfigBase()
{
}


//-----------------------------------------------------------------------------
// TupConfigBase::ReturnKeynameSubstr
// This method will look at the string passed in and return the keyname
// substring part of the string (the part before the |).  If there is no
// divider, then the input string is just returned.
//
// Input:
//  TupString strToParse -- String to get the keyname substr out of.
//
// Output: TupString representing the keyname.
//-----------------------------------------------------------------------------
TupString TupConfigBase::ReturnKeynameSubstr(TupString strToParse, const char* pChDelimiter)
{
	int nDividerIndex = strToParse.Find(pChDelimiter);
	if (nDividerIndex < 0)
		return strToParse;
	else
		return strToParse.Left(nDividerIndex);
}


//-----------------------------------------------------------------------------
// TupConfigBase::ReturnLabelSubstr
// This method will look at the string passed in and return the label
// substring part of the string (the part after the |).  If there is no
// divider, then the input string is just returned.
//
// Input:
//  TupString strToParse -- String to get the label substr out of.
//
// Output: TupString representing the label.
//-----------------------------------------------------------------------------
TupString TupConfigBase::ReturnLabelSubstr(TupString strToParse, const char* pChDelimiter)
{
	int nDividerIndex = strToParse.Find(pChDelimiter);
	if (nDividerIndex < 0)
		return strToParse;
	else
		return strToParse.Right(nDividerIndex + 1);
}

//-----------------------------------------------------------------------------
// TupConfigBase::GetNonUI
// This method will return the nonUI bool
//
// Output: bool -- current value of the nonUI bool (true if nonUI)
//-----------------------------------------------------------------------------
bool TupConfigBase::GetNonUI(void) const
{
	return m_bNonUI;
}

//-----------------------------------------------------------------------------
// TupConfigBase::SetNonUI
// This method will allow for the derived classed to set the non ui bool
//
// Input:
//  bool nonUI -- new value of the nonUI bool (true if nonUI)
//
//-----------------------------------------------------------------------------
void TupConfigBase::SetNonUI(bool nonUI)
{
	m_bNonUI = nonUI;
}

//-----------------------------------------------------------------------------
// TupConfigBase::GetExclusive
// This method will return the exclusive bool
//
// Output: bool -- current value of the exclusive bool (true if exclusive)
//-----------------------------------------------------------------------------
bool TupConfigBase::GetExclusive(void) const
{
	return m_bExclusive;
}

//-----------------------------------------------------------------------------
// TupConfigBase::SetNonExclusive
// This method will allow for the derived classed to set the exclusive bool
//
// Input:
//  bool exclusive -- new value of the exclusive bool (true if exclusive)
//
//-----------------------------------------------------------------------------
void TupConfigBase::SetExclusive(bool exclusive)
{
	m_bExclusive = exclusive;
}

//-----------------------------------------------------------------------------
// TupConfigBase::StrToColor
// This method will take the incoming string and convert it into an TupAttribColor.
//
// Input:
//  TupString strColor -- The string to convert.
//
// Output: TupAttribColor converted from the string.
//-----------------------------------------------------------------------------
TupAttribColor TupConfigBase::StringToColor(TupString strColor)
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
	TupAttribColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	returnColor.m_strName = strName;
	return returnColor;
}

//-----------------------------------------------------------------------------
// TupConfigBase::StrToColor
// This method will take the incoming string and convert it into an TupAttribColor.
//
// Input:
//  TupString strColor -- The string to convert.
//
// Output: TupAttribColor converted from the string.
//-----------------------------------------------------------------------------
TupString TupConfigBase::ColorToString(TupAttribColor color)
{
	char buffer[80];
	sprintf(buffer,"%d,%d,%d,%s",color.m_nRed,color.m_nGreen,color.m_nBlue,(const char *)color.m_strName);
	return TupString(buffer);
}

#ifdef USE_TUP_MEMORY_MANAGER
void *TupConfigBase::operator new(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}

void TupConfigBase::operator delete(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}

void *TupConfigBase::Allocate(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}

void TupConfigBase::FreeBuffer(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}
#endif