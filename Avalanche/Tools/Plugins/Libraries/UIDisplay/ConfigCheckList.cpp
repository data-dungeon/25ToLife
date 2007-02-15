//-----------------------------------------------------------------------------
//	File:		ConfigCheckList.cpp
//	Created:	July 22, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigCheckList class represents the CHECK_LIST configuration
//			data.
//
//	History:	7/22/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigCheckList.h"
#include "UIDisplayCheckList.h"
#include "UIController.h"
#include "CurrAttribBase.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"


static int gCurrCheckListId = RESOURCE_START_CHECKLIST;

//-----------------------------------------------------------------------------
// Class Definition:

//-----------------------------------------------------------------------------
// ConfigCheckList::ConfigCheckList
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigCheckList::ConfigCheckList()
{
	m_nPixelHeight = 108;
}


//-----------------------------------------------------------------------------
// ConfigCheckList::~ConfigCheckList
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigCheckList::~ConfigCheckList()
{
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigCheckList::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigCheckList::PrintDebug(int nIndentLevel) const
{
#if 0
	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs.append("\t");
	}

	DebugPrint(strTabs.data());
	DebugPrint("EDIT_BOX ");
	DebugPrint(m_strName.data());
	DebugPrint(" = {\n");
	strTabs.append("\t");
	DebugPrint(strTabs.data());
	DebugPrint("DEFAULT_TEXT = %s\n", m_strDefaultText.data());
	DebugPrint(strTabs.data());
	switch (m_type)
	{
		case STRING:
			DebugPrint("TYPE = String\n");
			break;
		case FILENAME:
			DebugPrint("TYPE = Filename\n");
			break;
		case DIRECTORY:
			DebugPrint("TYPE = Directory\n");
			break;
		case OBJECTNAME:
			DebugPrint("TYPE = Objectname\n");
			break;
	}
	strTabs.erase(strTabs.size() - 1);
	DebugPrint("%s}\n", strTabs.data());
#endif
}
#endif


//-----------------------------------------------------------------------------
// ConfigCheckList::SetPixelHeight
// This method is an accessor for the m_nPixelHeight data member.
//
// Input:
//  int nNewPixelHeight -- The new pixel height to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigCheckList::SetPixelHeight(int nNewPixelHeight)
{
	m_nPixelHeight = nNewPixelHeight;
}


//-----------------------------------------------------------------------------
// ConfigCheckList::AddCriteriaToVect
// This method can be called to add a criteria to the vector of criteria
// structures.
//
// Input:
//  const KeyValPair &newKeyValPair -- The new keyvalpair struct to add.
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigCheckList::AddCriteriaToVect(const KeyValPair &newKeyValPair)
{
	m_vectCriteria.Add(newKeyValPair);
}


//-----------------------------------------------------------------------------
// ConfigCheckList::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this check list.
//
// Input:
//  TupperwareAggregate* pCheckListTupAgg -- A pointer to the checklist
//											 tupperware aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigCheckList::BuildConfigTree(TupperwareAggregate* pCheckListTupAgg, ConfigOverrides& overrides)
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
		TupString strName = pCheckListTupAgg->GetName();

		// check if there is a default override
		m_strKeyName = ReturnKeyNameSubstr(strName);
		m_strLabelName = ReturnLabelNameSubstr(strName);
		TupString strDefaultOverrideValue;
		bool bDefaultOverride = false;
		int keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_DEFAULT);
		if (keyIndex!=-1)
		{
			strDefaultOverrideValue = overrides.GetValueAsString(keyIndex);
			bDefaultOverride = true;
		}
		int nOverrideDefaultIndex = -1;
		// change the label if overridden
		keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_LABEL);
		if (keyIndex!=-1)
		{
			m_strLabelName = overrides.GetValueAsString(keyIndex);
		}
		// Override the nonui flag, if necessary:
		bool bNonUIOverride = false;
		keyIndex = overrides.KeyExistsAt(m_strKeyName, ConfigOverrides::OVERRIDE_NONUI);
		if (keyIndex != -1)
		{
			if (overrides.GetValueAsInt(keyIndex) == 1)
				m_bNonUI = true;
			bNonUIOverride = true;
		}
		// Override the disabled flag, if necessary:
		bool bDisabledOverride = false;
		keyIndex = overrides.KeyExistsAt(m_strKeyName, ConfigOverrides::OVERRIDE_DISABLED);
		if (keyIndex != -1)
		{
			if (overrides.GetValueAsInt(keyIndex) == 1)
				m_bDisabled = true;
			bDisabledOverride = true;
		}
		// override the comment
		bool bCommentOverride = false;
		keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_COMMENT);
		if (keyIndex!=-1)
		{
			SetComment(overrides.GetValueAsString(keyIndex));
			bCommentOverride = true;
		}
		// make sure this is the last override
		// Override the keyname, if necessary:
		keyIndex = overrides.KeyExistsAt(m_strKeyName, ConfigOverrides::OVERRIDE_KEY);
		if (keyIndex != -1)
		{
			m_strKeyName = overrides.GetValueAsString(keyIndex);
		}
		// check for prefix and postfix
		if (overrides.HasPrefixOrPostfix())
		{
			overrides.AddPrefixAndPostfix(m_strKeyName);
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
					newKeyValPair.m_strKeyname = ReturnKeyNameSubstr(newString, "=");
					newKeyValPair.m_strVal = ReturnLabelNameSubstr(newString, "=");
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
			else if (strcmp(pCharKey, "ADDED_INDENT") == 0)
			{
				// We found the added_indent flag, so set it:
				this->SetAddedIndent(tupIterator.GetAsScalar()->GetAsInt());
			}
			else if (strcmp(pCharKey, "COMMENT") == 0)
			{
				if (!bCommentOverride)
				{
					// We found the COMMENT flag, so set it:
					SetComment(tupIterator.GetAsScalar()->GetAsString());
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
			else if (strcmp(pCharKey, "DISABLED") == 0)
			{
				// We found the disabled flag, so set it if necessary:
				if (bDisabledOverride == false)
				{
					// Set the disabled flag if set to 1
					if (tupIterator.GetAsScalar()->GetAsInt() == 1)
						m_bDisabled = true;
				}
			}
			else if (strcmp(pCharKey, "PIXEL_HEIGHT") == 0)
			{
				// We found the pixel_height flag, so set it:
				this->SetPixelHeight(tupIterator.GetAsScalar()->GetAsInt());
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
// ConfigCheckList::BuildUIDisplay
// This method will build the UIDisplay data for this checklist.  The data will
// be stored in the UIController passed in and uses the current attrib data
// passed in.
//
// Input:
//  UIController* pUIController -- The UIController to store the data in.
//  CurrAttribBase* pAttribBase -- The current attrib data to inspect.
//	int nIndentLevel -- Level to indent the UIDisplay control.
//	ExclusiveController* pExclusiveController -- The exclusive controller to
//												 inspect, if needed.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigCheckList::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
									 ExclusiveController* pExclusiveController, int nIndentLevel)
{
	bool bResult = true;

	// Check the non ui flag to see if this control should even be shown:
	if (this->GetNonUI() == true)
		return bResult;

	UIDisplayCheckList* pDisplayCheckList = new UIDisplayCheckList();

	// Set the label and keyname:
	pDisplayCheckList->SetLabel(m_strLabelName);
	pDisplayCheckList->SetKeyname(m_strKeyName);

	// Set the indent level:
	pDisplayCheckList->SetIndentLevel(nIndentLevel);
	pDisplayCheckList->SetAddedIndent(GetAddedIndent());

	// Set the exclusive flag:
	pDisplayCheckList->SetExclusive(m_bExclusive);

	// Set the disabled flag:
	pDisplayCheckList->SetDisabled(m_bDisabled);

	pDisplayCheckList->SetToolTip(m_strLabelName,m_strKeyName,m_strComment);

	// Set the label id:
	if (m_strComment.GetLength()) // get id if there is a comment
	{
		pDisplayCheckList->SetLabelID(gCurrCheckListId);
		gCurrCheckListId++;
		if (gCurrCheckListId == RESOURCE_END_CHECKLIST)
			gCurrCheckListId = RESOURCE_START_CHECKLIST;
	}
	else
	{
		pDisplayCheckList->SetLabelID(IDC_ALL_LABELS);
	}

	// Set the control id:
	pDisplayCheckList->SetControlID(gCurrCheckListId);
	gCurrCheckListId++;
	if (gCurrCheckListId == RESOURCE_END_CHECKLIST)
		gCurrCheckListId = RESOURCE_START_CHECKLIST;

	// Set the pixel height:
	pDisplayCheckList->SetPixelHeight(m_nPixelHeight);

	// Set the default state:
	int nDefaultState = BST_UNCHECKED;
	pDisplayCheckList->SetDefaultState(nDefaultState);

	// Find the entity names and their checked state:
	TupArray<int> vectCheckedStates;
	pAttribBase->FindEntityNames(m_vectCriteria, m_vectEntityNames);
	pAttribBase->FindCheckStates(m_strKeyName, m_vectEntityNames, nDefaultState, vectCheckedStates);

	// Set the entity names and checked states:
	pDisplayCheckList->SetEntityNames(m_vectEntityNames);
	pDisplayCheckList->SetCheckedStates(vectCheckedStates);

	// Add the UIDisplayEdit to the UIController:
	pUIController->AddDisplayObject(pDisplayCheckList);


	return bResult;
}

void ConfigCheckList::FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria)
{
	if (criteria == "ConfigCheckList")
	{
		keys.Add(m_strKeyName);
	}
}

//-----------------------------------------------------------------------------
// ConfigCheckList::GetKeySelections
// This method will check to see if the name of this object matches the key and if so
// set up the selections object to have all the children selections within this object
// If this object or one of it's children finds a match it will return true
//
// Input:
//  const KeyValue &path -- a const reference to the keyvalue pairs that denote the path
//  const char *pKeyName -- a const char * that holds the keyname that is used for matching
//  KeySelection &selections -- the selections to be build
//
// Output: bool - True if found on object or child, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigCheckList::GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections)
{
	// if the key we are looking for is me
	if (!m_strKeyName.Compare(pKeyName))
	{
		selections.Clear();
		if (!m_bEntityNamesValid)
		{
			// try to only do this once (that is why we have the valid value)
			pAttribBase->FindEntityNames(m_vectCriteria, m_vectEntityNames);
			m_bEntityNamesValid = true;
		}
		int numNames = m_vectEntityNames.GetSize();
		for (int nIndex = 0; nIndex < numNames; nIndex++)
		{
			// add selection children
			selections.AddSelection(m_vectEntityNames[nIndex],false);
		}
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// ConfigCheckList::GetKeyValueContexts
// This method will check to see if the name of this object matches the key and if so
// set up the selections object to have all the children selections within this object
// If this object or one of it's children finds a match it will return true
//
// Input:
//  CurrAttribBase* pAttribBase -- access to other object's attributes
//  const KeyValue &path -- a const reference to the keyvalue pairs that denote the path
//  KeyValueContext &keyValueContext -- holds the key value contexts to be set
//  bool addKeys -- if false we will only update existing keys and not add new ones within the context
//
//-----------------------------------------------------------------------------
void ConfigCheckList::GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue& path, KeyValueContext& keyValueContext,bool addKeys)
{
	TupString value = ""; // start with empty value
	bool bIsDefaultValue = true;
	int keyValueIndex = -1;

	// add if addkeys or key already exists within context
	if (addKeys || keyValueContext.KeyExistsAt(m_strKeyName)!=-1)
	{
		if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
		{
			// if we find a match then replace the value with the one in the path
			keyValueIndex = path.KeyExistsAt(m_strKeyName);
			if (keyValueIndex!=-1)
			{
				// get a list of valid names
				if (!m_bEntityNamesValid)
				{
					// try to only do this once (that is why we have the valid value)
					pAttribBase->FindEntityNames(m_vectCriteria, m_vectEntityNames);
					m_bEntityNamesValid = true;
				}
				TupArray<TupString> checkedNames;
				ParseStrForCheckedNames(checkedNames, path.GetValueAsString(keyValueIndex));
				TupString newValue;
				// only add names that are valid
				for (int checkedNamesIndex = 0; checkedNamesIndex < checkedNames.GetSize(); checkedNamesIndex++)
				{
					if (m_vectEntityNames.GetIndex(checkedNames[checkedNamesIndex])!=-1)
					{
						newValue += checkedNames[checkedNamesIndex];
						newValue += ",";
					}
				}
				if (newValue != value)
				{
					value = newValue;
					bIsDefaultValue = false;
				}
			}
		}
		keyValueContext.AddKeyValue(m_strKeyName,value,-1,this,bIsDefaultValue,"",keyValueIndex);
	}
}


//-----------------------------------------------------------------------------
// ConfigCheckList::ParseStrForCheckedNames
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
void ConfigCheckList::ParseStrForCheckedNames(TupArray<TupString>& arrayCheckedNames, const TupString &strToParse)
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

//-----------------------------------------------------------------------------
// ConfigCheckList::ClearCachedData
// This method will call the entire tree an attempt to clear any cached data
//
// Input: none
//
// Output: none
//
//-----------------------------------------------------------------------------
void ConfigCheckList::ClearCachedData(void)
{
	m_bEntityNamesValid = false; // so later we will scan for new names (but only once)
}

//-----------------------------------------------------------------------------
// ConfigCheckList::GetCRC
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
unsigned long ConfigCheckList::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	CRCGenerator.AnalyzeData(m_strKeyName);
	CRCGenerator.AnalyzeData(m_strLabelName);
	CRCGenerator.AnalyzeData(m_strDefault);
	CRCGenerator.AnalyzeData(m_nPixelHeight);
	for (int criteriaIndex = 0; criteriaIndex < m_vectCriteria.GetSize(); criteriaIndex++)
	{
		const KeyValPair &criteria = m_vectCriteria[criteriaIndex];
		CRCGenerator.AnalyzeData(criteria.m_strKeyname);
		CRCGenerator.AnalyzeData(criteria.m_strVal);
	}
	return CRCGenerator.GetResult();
}