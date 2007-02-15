//-----------------------------------------------------------------------------
//	File:		TupConfigSetColorBox.cpp
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TupConfigSetColorBox class represents the SET_COLOR_BOX
//			configuration data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: ConfigSetColorBox.cpp $
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 6/27/02    Time: 2:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Override Keyname additions
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

//-----------------------------------------------------------------------------
// Includes:

#include "TupperAttribPch.h"
#include "ConfigSetColorBox.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// TupConfigSetColorBox::TupConfigSetColorBox
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigSetColorBox::TupConfigSetColorBox()
{
	m_defaultColor.m_nRed = 0;
	m_defaultColor.m_nGreen = 0;
	m_defaultColor.m_nBlue = 0;
	m_defaultColor.m_strName = "";
}


//-----------------------------------------------------------------------------
// TupConfigSetColorBox::~TupConfigSetColorBox
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigSetColorBox::~TupConfigSetColorBox()
{
}

//-----------------------------------------------------------------------------
// TupConfigSetColorBox::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this setcolorbox.
//
// Input:
//  TupperwareAggregate *pSetColorTupAgg -- A pointer to the setcolorbox
//											tupperware aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool TupConfigSetColorBox::BuildConfigTree(TupperwareAggregate *pSetColorTupAgg,ConfigOverrides &overrides)
{
	bool bSuccess = true;

	if (pSetColorTupAgg == NULL || pSetColorTupAgg->GetLastChildAtom() == NULL)
	{
		// TODO - A set color box has to be an aggregate and it has to have
		// children, so throw up an error!
	}
	else
	{
		// Set the name:
		m_strName = pSetColorTupAgg->GetName();

		// check if there is a default override
		TupString strKeyname = this->ReturnKeynameSubstr(m_strName);
		bool bDefaultOverride = false;
		int keyIndex = overrides.KeyExistsAt(strKeyname,ConfigOverrides::OVERRIDE_DEFAULT);
		if (keyIndex!=-1)
		{
			m_defaultColor = overrides.GetValueAsColor(keyIndex);
			bDefaultOverride = true;
		}
		// change the label if overridden
		keyIndex = overrides.KeyExistsAt(strKeyname,ConfigOverrides::OVERRIDE_LABEL);
		if (keyIndex!=-1)
		{
			m_strName = strKeyname;
			m_strName += "|";
			m_strName += overrides.GetValueAsString(keyIndex);
		}
		// override the nonui flag
		bool bNonUIOverride = false;
		keyIndex = overrides.KeyExistsAt(strKeyname,ConfigOverrides::OVERRIDE_NONUI);
		if (keyIndex!=-1)
		{
			if (overrides.GetValueAsInt(keyIndex)==1)
				m_bNonUI = true;
			bNonUIOverride = true;
		}
		// make sure this is the last override
		// override the keyname
		keyIndex = overrides.KeyExistsAt(strKeyname,ConfigOverrides::OVERRIDE_KEY);
		if (keyIndex!=-1)
		{
			TupString strLabelname = this->ReturnLabelSubstr(m_strName);
			m_strName = strKeyname = overrides.GetValueAsString(keyIndex);
			m_strName += "|";
			m_strName += strLabelname;
		}
		// check for prefix and postfix
		if (overrides.HasPrefixOrPostfix())
		{
			TupString strLabelname = ReturnLabelSubstr(m_strName);
			overrides.AddPrefixAndPostfix(strKeyname);
			m_strName = strKeyname;
			m_strName += "|";
			m_strName += strLabelname;
		}

		// Cycle through all the children of the setcolorbox, building tree as we go:
		for( TupperwareIterator tupIterator = pSetColorTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
		{
			// Check to see what the current atom's key is:
			TupperwareAtom* pTupAtom = tupIterator.GetAsAtom();
			const char* pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "DEFAULT_COLOR") == 0)
			{
				// We found the default text, so set it:
				if (!bDefaultOverride) // make sure there is not an override already
				{
					// We found the default color, so set it:
					TupperwareList* pTupList = tupIterator.GetAsList();
					const int* intArray = pTupList->GetAsInt();
					// TODO - Do some error checking to make sure this is a list, and
					// that it is an integer list and that it has 3 elements.
					m_defaultColor.m_nRed = intArray[0];
					m_defaultColor.m_nGreen = intArray[1];
					m_defaultColor.m_nBlue = intArray[2];
					m_defaultColor.m_strName = (tupIterator.GetAsAtom())->GetName();
				}
				// TODO - If there is more than one DEFAULT_COLOR, notify the
				// user there is an error.
			}
			else if (strcmp(pCharKey, "COLOR") == 0)
			{
				// We found a color, so set it:
				TupperwareList* pTupList = tupIterator.GetAsList();
				const int* intArray = pTupList->GetAsInt();
				// TODO - Do some error checking to make sure this is a list, and
				// that it is an integer list and that it has 3 elements.
				TupAttribColor newColor;
				newColor.m_nRed = intArray[0];
				newColor.m_nGreen = intArray[1];
				newColor.m_nBlue = intArray[2];
				newColor.m_strName = (tupIterator.GetAsAtom())->GetName();
				m_vectColors.Add(newColor);
			}
			else if (strcmp(pCharKey, "NONUI") == 0)
			{
				if (!bNonUIOverride)
				{
					// set the non ui flag if set to 1
					if (tupIterator.GetAsScalar()->GetAsInt()==1)
						m_bNonUI = true;
				}
			}
			else
			{
				// TODO - Notify the user there is an error at setcolorbox level of
				// config file.
			}
		}
	}

	return bSuccess;
}

//-----------------------------------------------------------------------------
// TupConfigSetColorBox::GetKeyValueContexts
// This method will check to see if the name of this object matches the key and if so
// set up the selections object to have all the children selections within this object
// If this object or one of it's children finds a match it will return true
//
// Input:
//  const TupKeyValue &path -- a const reference to the keyvalue pairs that denote the path
//  TupKeyValueContext &keyValueContext -- holds the key value contexts to be set
//  bool addKeys -- if false we will only update existing keys and not add new ones within the context
//
//-----------------------------------------------------------------------------
void TupConfigSetColorBox::GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const
{
	// Peel off the keyname and the label:
	TupString strKeyname = ReturnKeynameSubstr(m_strName);
	TupString value = ColorToString(m_defaultColor); // start with default color

	if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
	{
		// if we find a match then replace the value with the one in the path
		int keyIndex = path.KeyExistsAt(strKeyname);
		if (keyIndex!=-1)
		{
			value = path.GetValueAsString(keyIndex);
		}
	}
	// add if addkeys or key already exists within context
	if (addKeys || keyValueContext.KeyExistsAt(strKeyname)!=-1)
	{
		keyValueContext.AddKeyValue(strKeyname,value,-1,this);
	}
}

