//-----------------------------------------------------------------------------
//	File:		CurrAttribBase.cpp
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



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "CurrAttribBase.h"



//-----------------------------------------------------------------------------
// Class Definition:

// UniqueValue methods
bool UniqueValue::operator==(const UniqueValue& other) const
{
	return (m_strValue.Compare(other.m_strValue)==0);
}

bool UniqueValue::operator<=(const UniqueValue& other) const
{
	return (m_strValue.Compare(other.m_strValue)<=0);
}

bool UniqueValue::operator>=(const UniqueValue& other) const
{
	return (m_strValue.Compare(other.m_strValue)>=0);
}

bool UniqueValue::operator<(const UniqueValue& other) const
{
	return (m_strValue.Compare(other.m_strValue)<0);
}

bool UniqueValue::operator>(const UniqueValue& other) const
{
	return (m_strValue.Compare(other.m_strValue)>0);
}


//-----------------------------------------------------------------------------
// CurrAttribBase::CurrAttribBase
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
CurrAttribBase::CurrAttribBase()
{
}


//-----------------------------------------------------------------------------
// CurrAttribBase::~CurrAttribBase
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
CurrAttribBase::~CurrAttribBase()
{
}


//-----------------------------------------------------------------------------
// CurrAttribBase::FindEntityNames
// This method can be overriden in CurrAttribBase derived classes to return the
// entity names for the objects, faces, etc. that those classes represent, and
// that match the incoming criteria.  The behavior in this base class does
// nothing.
//
// Input:
//  const TupArray<KeyValPair> &vectCriteria -- The vector of criteria to match
//											against the entities.
//  Keeper<TupString>& vectEntityNames -- Upon output, this vector will
//												 contain the entity names that
//												 match the criteria.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribBase::FindEntityNames(const TupArray<KeyValPair> &vectCriteria, Keeper<TupString>& vectEntityNames) const
{
	vectEntityNames.RemoveAll();
	return false;
}


//-----------------------------------------------------------------------------
// CurrAttribBase::FindCheckStates
// This method can be overriden in CurrAttribBase derives classes to return the
// checked states for the entity names passed in.  The currently selected
// entities (nodes, faces,etc.) should be inspected to see if they contain the
// keyname passed in.  If so, the vals can be inspected and compared to the
// incoming entity names to find the final checked states.  The behavior in
// this base class does nothing.
//
// Input:
//  const TupString &strKeyname -- The keyname to look for on the selected entities.
//  const Keeper<TupString>& vectEntityNames -- The names to look for in the
//												 vals for the selected entities.
//  int nDefaultState -- The default checked state.
//  TupArray<int>& vectCheckedStates -- Upon output, this vector will hold
//										   the checked state for each entity
//										   name.  The size of this vector will
//										   be the same as the incoming entity
//										   name vector.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribBase::FindCheckStates(const TupString &strKeyname, const Keeper<TupString> &vectEntityNames,
									 int nDefaultState, TupArray<int> &vectCheckedStates) const
{
	vectCheckedStates.RemoveAll();
	return false;
}


//-----------------------------------------------------------------------------
// CurrAttribBase::StrToColor
// This method will take the incoming string and convert it into an AttribColor.
//
// Input:
//  TupString strColor -- The string to convert.
//
// Output: AttribColor converted from the string.
//-----------------------------------------------------------------------------
AttribColor CurrAttribBase::StrToColor(const TupString &string)
{
	TupString strColor(string);
	// Get the value for red:
	int nFirstComma = strColor.Find(',');
	TupString strRed = strColor.Left(nFirstComma);
	int nRed = atoi(strRed);

	// Get the value for green:
	strColor = strColor.Mid(nFirstComma + 1);
	nFirstComma = strColor.Find(',');
	TupString strGreen = strColor.Left(nFirstComma);
	int nGreen = atoi(strGreen);

	// Get the value for blue:
	strColor = strColor.Mid(nFirstComma + 1);
	nFirstComma = strColor.Find(',');
	TupString strBlue = strColor.Left(nFirstComma);
	int nBlue = atoi(strBlue);

	// Get the name:
	TupString strName = strColor.Mid(nFirstComma + 1);

	// Create the color and return it:
	AttribColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	returnColor.m_strName = strName;
	return returnColor;
}
