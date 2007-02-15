//-----------------------------------------------------------------------------
//	File:		ConfigCheckList.cpp
//	Created:	Aug. 29, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TupConfigCheckList class represents the CHECK_LIST
//			configuration data.
//
//	History:	8/29/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: ConfigCheckList.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Brad Worthen Date: 8/29/02    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 1  *****************
 * User: Brad Worthen Date: 8/29/02    Time: 2:09p
 * Created in $/Avalanche/tools/TupperAttrib
 */ 



//-----------------------------------------------------------------------------
// Includes:

#include "TupperAttribPch.h"

#ifndef TUPPERATTRIB_CONFIG_CHECK_LIST_H
#include "ConfigCheckList.h"
#endif
#ifndef TUPPERATTRIB_KEY_VALUE_H
#include "KeyValue.h"
#endif
#ifndef TUPPERATTRIB_KEY_VALUE_CONTEXT_H
#include "KeyValueContext.h"
#endif



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// TupConfigCheckList::TupConfigCheckList
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigCheckList::TupConfigCheckList()
{
	m_strName = "";
	m_strDefault = "";
}


//-----------------------------------------------------------------------------
// TupConfigCheckList::~TupConfigCheckList
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigCheckList::~TupConfigCheckList()
{
}


//-----------------------------------------------------------------------------
// TupConfigCheckList::AddCriteriaToVect
// This method can be called to add a criteria to the vector of criteria
// structures.
//
// Input:
//  KeyValPair newKeyValPair -- The new keyvalpair struct to add.
//
// Output: None.
//-----------------------------------------------------------------------------
void TupConfigCheckList::AddCriteriaToVect(KeyValPair newKeyValPair)
{
	m_vectCriteria.Add(newKeyValPair);
}


//-----------------------------------------------------------------------------
// TupConfigCheckList::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this check list.
//
// Input:
//  TupperwareAggregate* pCheckListTupAgg -- A pointer to the checklist
//											 tupperware aggregate.
//	ConfigOverrides& overrides -- List of possible overrides.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool TupConfigCheckList::BuildConfigTree(TupperwareAggregate* pCheckListTupAgg, ConfigOverrides& overrides)
{
	bool bSuccess = true;

	if (pCheckListTupAgg == NULL || pCheckListTupAgg->GetLastChildAtom() == NULL)
	{
		// TODO - A checklist has to be an aggregate and it has to have
		// children, so throw up an error!
	}
	else
	{
		// Set the name:
		m_strName = pCheckListTupAgg->GetName();

		// Override the default, if necessary:
		TupString strKeyname = this->ReturnKeynameSubstr(m_strName);
		bool bDefaultOverride = false;
		int nKeyIndex = overrides.KeyExistsAt(strKeyname, ConfigOverrides::OVERRIDE_DEFAULT);
		if (nKeyIndex != -1)
		{
			m_strDefault = overrides.GetValueAsString(nKeyIndex);
			bDefaultOverride = true;
		}
		// Override the label, if necessary:
		nKeyIndex = overrides.KeyExistsAt(strKeyname, ConfigOverrides::OVERRIDE_LABEL);
		if (nKeyIndex != -1)
		{
			m_strName = strKeyname;
			m_strName += "|";
			m_strName += overrides.GetValueAsString(nKeyIndex);
		}
		// Override the nonui flag, if necessary:
		bool bNonUIOverride = false;
		nKeyIndex = overrides.KeyExistsAt(strKeyname, ConfigOverrides::OVERRIDE_NONUI);
		if (nKeyIndex != -1)
		{
			if (overrides.GetValueAsInt(nKeyIndex) == 1)
				m_bNonUI = true;
			bNonUIOverride = true;
		}
		// Override the keyname, if necessary:
		nKeyIndex = overrides.KeyExistsAt(strKeyname, ConfigOverrides::OVERRIDE_KEY);
		if (nKeyIndex != -1)
		{
			TupString strLabelname = this->ReturnLabelSubstr(m_strName);
			m_strName = strKeyname = overrides.GetValueAsString(nKeyIndex);
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

		// Cycle through all the children of the checklist, building tree as we go:
		for(TupperwareIterator tupIterator = pCheckListTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next())
		{
			// Check to see what the current atom's key is:
			TupperwareAtom* pTupAtom = tupIterator.GetAsAtom();
			const char* pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "CRITERIA") == 0)
			{
				// We found the criteria flag, so set it:
				const TupperwareList* pTupList = tupIterator.GetAsList();
				const char** strArray = pTupList->GetAsString();
				int nNumStrings = pTupList->GetLength();
				for (int nStrIndex = 0; nStrIndex < nNumStrings; nStrIndex++)
				{
					TupString newString(strArray[nStrIndex]);
					KeyValPair newKeyValPair;
					newKeyValPair.m_strKeyname = this->ReturnKeynameSubstr(newString, "=");
					newKeyValPair.m_strVal = this->ReturnLabelSubstr(newString, "=");
					this->AddCriteriaToVect(newKeyValPair);
				}
			}
			else if (strcmp(pCharKey, "DEFAULT") == 0)
			{
				// We found the default flag, so set it if necessary:
				if (bDefaultOverride == false)
				{
					TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
					m_strDefault = pTupScalar->GetAsString();

					// TODO - If there is more than one DEFAULT, notify the
					// user there is an error.
				}
			}
			else if (strcmp(pCharKey, "NONUI") == 0)
			{
				// We found the nonui flag, so set it if necessary:
				if (bNonUIOverride == false)
				{
					// Set the nonui flag if set to 1:
					if (tupIterator.GetAsScalar()->GetAsInt() == 1)
						m_bNonUI = true;
				}
			}
			else
			{
				// TODO - Notify the user there is an error at checklist level of
				// config file.
			}
			// TODO - Notify the user if there are no children of the checklist.
		}
	}

	return bSuccess;
}


//-----------------------------------------------------------------------------
// TupConfigCheckList::GetKeyValueContexts
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
void TupConfigCheckList::GetKeyValueContexts(const TupKeyValue& path, TupKeyValueContext& keyValueContext,bool addKeys) const
{
	// Peel off the keyname and the label:
	TupString strKeyname = ReturnKeynameSubstr(m_strName);
	TupString value = ""; // start with empty value

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


//-----------------------------------------------------------------------------
// TupConfigCheckList::ParseStrForCheckedNames
// This method will parse the incoming string for names that are checked, and
// then store those names in the incoming string array.  An example of an
// incoming string would be "sphere01,sphere02,;sphere03,".  With this example,
// the incoming string array would contain two strings: sphere01 and sphere02.
// The semicolon is used to delimit those names that are "checked" from those
// that aren't.  The ones on the left are considered "checked".
//
// Input:
//  TupArray<TupString>& arrayCheckedNames -- Upon output, this vector will
//											  contain all the strings of the
//											  checked names.
//  const TupString strToParse -- The string to parse.
//
// Output: None.
//-----------------------------------------------------------------------------
void TupConfigCheckList::ParseStrForCheckedNames(TupArray<TupString>& arrayCheckedNames, const TupString strToParse)
{
	// Clear out the array:
	arrayCheckedNames.RemoveAll();

	// Get rid of the semicolon and anything to the right of it:
	TupString strTemp = strToParse;
	int nSemiColonIndex = strTemp.Find(";", 0);
	if (nSemiColonIndex > -1)
		strTemp = strTemp.Left(nSemiColonIndex);

	// Now start parsing out the names, using the commas as delimiters:
	int nCommaIndex = strTemp.Find(",", 0);
	while (nCommaIndex > -1)
	{
		// Add the string:
		TupString strToAdd = strTemp.Left(nCommaIndex);
		arrayCheckedNames.Add(strToAdd);

		// Adjust the string:
		int nStrLength = strTemp.GetLength();
		strTemp = strTemp.Right(nStrLength - (nCommaIndex + 1));
		nCommaIndex = strTemp.Find(",", 0);
	}
}
