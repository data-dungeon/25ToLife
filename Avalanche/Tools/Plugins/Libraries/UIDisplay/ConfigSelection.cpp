//-----------------------------------------------------------------------------
//	File:		ConfigSelection.cpp
//	Created:	Nov. 16, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigSelection class represents the SELECTION configuration
//			data.
//
//	History:	11/16/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigSelection.h"
#include "ConfigComboBox.h"
#include "ConfigRadioGroup.h"
#include "ConfigCheckBox.h"
#include "ConfigEditBox.h"
#include "ConfigNumeric.h"
#include "ConfigFreeColorBox.h"
#include "ConfigSetColorBox.h"
#include "ConfigCheckList.h"
#include "ConfigLabel.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ConfigSelection::ConfigSelection
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigSelection::ConfigSelection()
{
}


//-----------------------------------------------------------------------------
// ConfigSelection::~ConfigSelection
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigSelection::~ConfigSelection()
{
	int numChildren = m_vectSelectionChildren.GetSize();
	for (int index = 0; index < numChildren; index++)
	{
		ConfigBase* pCurrChild = m_vectSelectionChildren[index];
		if (pCurrChild != NULL)
		{
			delete pCurrChild;
			pCurrChild = NULL;
		}
	}
}


//-----------------------------------------------------------------------------
// ConfigSelection::HasUIChildren
// This method will return true if the object has any ui children 
//
// Input: None.
//
// Output: TupString representing the name.
//-----------------------------------------------------------------------------
bool ConfigSelection::HasUIChildren(void) const
{
	return m_bHasUIChildren;
}

#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigSelection::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.  It will also tell any of its children to print their
// contents.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigSelection::PrintDebug(int nIndentLevel) const
{
	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs += "\t";
	}

	ATDebugPrint(strTabs);
	ATDebugPrint("SELECTION ");
	ATDebugPrint(" Key %s Label %s",(const char *)m_strKeyName,(const char *)m_strLabelName);
	ATDebugPrint(" = {\n");
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		((ConfigBase*) m_vectSelectionChildren[nIndex])->PrintDebug(nIndentLevel + 1);
	}
	ATDebugPrint("%s}\n", strTabs);
}
#endif


//-----------------------------------------------------------------------------
// ConfigSelection::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this selection.  It builds the necessary data at this selection level and
// then tells each of the children to build their part of the config tree.
//
// Input:
//  TupperwareAggregate* pSelectionTupAgg -- A pointer to the selection
//											 tupperware aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigSelection::BuildConfigTree(TupperwareAggregate* pSelectionTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides,bool selection)
{
	bool bSuccess = true;
	int nDefaultOverridesSize;

	if (pSelectionTupAgg == NULL)
	{
		// TODO - A selection has to be an aggregate, even though it may be
		// an empty aggregate, so throw up error.
	}
	else
	{
		// Set the name if we are a selection
		if (selection)
		{
			// Set the name:
			TupString strName = pSelectionTupAgg->GetName();

			// check if there is a default override
			m_strKeyName = ReturnKeyNameSubstr(strName);
			m_strLabelName = ReturnLabelNameSubstr(strName);

			// remember how big the defaults overrides was before any of my kids did anything
			nDefaultOverridesSize = overrides.GetSize();
		}

		// Cycle through all the children of the selection, building tree as we go:
		for( TupperwareIterator tupIterator = pSelectionTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
		{
			// Check to see what the current atom's key is:
			TupperwareAtom *pTupAtom = tupIterator.GetAsAtom();
			const char *pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "COMBO_BOX") == 0)
			{
				// We found a combo-box, so create a combobox object and add it to
				// the selection's children:
				ConfigComboBox* pConfigComboBox = new ConfigComboBox();
				pConfigComboBox->BuildConfigTree(tupIterator.GetAsAggregate(), setArray, overrides);
				m_vectSelectionChildren.Add(pConfigComboBox);
			}
			else if (strcmp(pCharKey, "RADIO_GROUP") == 0)
			{
				// We found a radio group, so create a radiogroup object and add it to
				// the selection's children:
				ConfigRadioGroup* pConfigRadioGroup = new ConfigRadioGroup();
				pConfigRadioGroup->BuildConfigTree(tupIterator.GetAsAggregate(), setArray, overrides);
				m_vectSelectionChildren.Add(pConfigRadioGroup);
			}
			else if (strcmp(pCharKey, "CHECK_BOX") == 0)
			{
				// We found a checkbox, so create a checkbox object and add it to
				// the selection's children:
				ConfigCheckBox* pConfigCheckBox = new ConfigCheckBox();
				pConfigCheckBox->BuildConfigTree(tupIterator.GetAsAggregate(), setArray, overrides);
				m_vectSelectionChildren.Add(pConfigCheckBox);
			}
			else if (strcmp(pCharKey, "EDIT_BOX") == 0)
			{
				// We found an edit box, so create an editbox object and add it to
				// the selection's children:
				ConfigEditBox* pConfigEditBox = new ConfigEditBox();
				pConfigEditBox->BuildConfigTree(tupIterator.GetAsAggregate(), overrides);
				m_vectSelectionChildren.Add(pConfigEditBox);
			}
			else if (strcmp(pCharKey, "NUMERIC") == 0)
			{
				// We found a numeric, so create a numeric object and add it to
				// the selection's children:
				ConfigNumeric* pConfigNumeric = new ConfigNumeric();
				pConfigNumeric->BuildConfigTree(tupIterator.GetAsAggregate(), overrides);
				m_vectSelectionChildren.Add(pConfigNumeric);
			}
			else if (strcmp(pCharKey, "FREE_COLOR_BOX") == 0)
			{
				// We found a free color box, so create a freecolorbox object and
				// add it to the selection's children:
				ConfigFreeColorBox* pConfigFreeColorBox = new ConfigFreeColorBox();
				pConfigFreeColorBox->BuildConfigTree(tupIterator.GetAsAggregate(), overrides);
				m_vectSelectionChildren.Add(pConfigFreeColorBox);
			}
			else if (strcmp(pCharKey, "SET_COLOR_BOX") == 0)
			{
				// We found a set color box, so create a setcolorbox object and
				// add it to the selection's children:
				ConfigSetColorBox* pConfigSetColorBox = new ConfigSetColorBox();
				pConfigSetColorBox->BuildConfigTree(tupIterator.GetAsAggregate(), overrides);
				m_vectSelectionChildren.Add(pConfigSetColorBox);
			}
			else if (strcmp(pCharKey, "CHECK_LIST") == 0)
			{
				// We found a check list, so create a checklist object and
				// add it to the selection's children:
				ConfigCheckList* pConfigCheckList = new ConfigCheckList();
				pConfigCheckList->BuildConfigTree(tupIterator.GetAsAggregate(), overrides);
				m_vectSelectionChildren.Add(pConfigCheckList);
			}
			else if (strcmp(pCharKey, "LABEL") == 0)
			{
				// We found a check list, so create a checklist object and
				// add it to the root's children:
				ConfigLabel* pConfigLabel = new ConfigLabel();
				pConfigLabel->BuildConfigTree(tupIterator.GetAsAggregate(), overrides);
				m_vectSelectionChildren.Add(pConfigLabel);
			}
			else if (strcmp(pCharKey, "SET_REFERENCE") == 0)
			{
				TupperwareScalar *pReferenceScalar = tupIterator.GetAsScalar();
				assert(pReferenceScalar);
				char *pString = pReferenceScalar->GetAsString();
				assert(pString);
				// scan through the set array to find a match
				int totalSetEntries = setArray.GetSize();
				for (int setIndex=0;setIndex<totalSetEntries;setIndex++)
				{
					TupperwareAggregate *pSetEntryAgg = setArray[setIndex];
					// does the name match?
					if (strcmp(pString,pSetEntryAgg->GetName()) == 0)
					{
						//call again but use the set entry aggregate
						BuildConfigTree(pSetEntryAgg,setArray,overrides,false);
					}
				}
			}
			// add to override array
			else if (strcmp(pCharKey, "OVERRIDE_DEFAULT") == 0)
			{
				TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
				assert(pOverrideScalar);
				const char *pValue = pOverrideScalar->GetAsString();
				const char *pKey = pOverrideScalar->GetName();
				assert(pKey);
				assert(pValue);
				overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_DEFAULT);
			}
			else if (strcmp(pCharKey, "OVERRIDE_NONUI") == 0)
			{
				TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
				assert(pOverrideScalar);
				const char *pValue = pOverrideScalar->GetAsString();
				const char *pKey = pOverrideScalar->GetName();
				assert(pKey);
				assert(pValue);
				overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_NONUI);
			}
			else if (strcmp(pCharKey, "OVERRIDE_DISABLED") == 0)
			{
				TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
				assert(pOverrideScalar);
				const char *pValue = pOverrideScalar->GetAsString();
				const char *pKey = pOverrideScalar->GetName();
				assert(pKey);
				assert(pValue);
				overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_DISABLED);
			}
			else if (strcmp(pCharKey, "OVERRIDE_LABEL") == 0)
			{
				TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
				assert(pOverrideScalar);
				const char *pValue = pOverrideScalar->GetAsString();
				const char *pKey = pOverrideScalar->GetName();
				assert(pKey);
				assert(pValue);
				overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_LABEL);
			}
			else if (strcmp(pCharKey, "OVERRIDE_COMMENT") == 0)
			{
				TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
				assert(pOverrideScalar);
				const char *pValue = pOverrideScalar->GetAsString();
				const char *pKey = pOverrideScalar->GetName();
				assert(pKey);
				assert(pValue);
				overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_COMMENT);
			}
			else if (strcmp(pCharKey, "OVERRIDE_KEY") == 0)
			{
				TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
				assert(pOverrideScalar);
				const char *pValue = pOverrideScalar->GetAsString();
				const char *pKey = pOverrideScalar->GetName();
				assert(pKey);
				assert(pValue);
				overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_KEY);
			}
			else if (strcmp(pCharKey, "OVERRIDE_ADD_KEY_PREFIX") == 0)
			{
				TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
				assert(pOverrideScalar);
				const char *pValue = pOverrideScalar->GetAsString();
				assert(pValue);
				overrides.AddKeyPrefix(pValue);
			}
			else if (strcmp(pCharKey, "OVERRIDE_ADD_KEY_POSTFIX") == 0)
			{
				TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
				assert(pOverrideScalar);
				const char *pValue = pOverrideScalar->GetAsString();
				assert(pValue);
				overrides.AddKeyPostfix(pValue);
			}
			else if (strcmp(pCharKey, "OVERRIDE_POP_KEY_PREFIX") == 0)
			{
				overrides.PopKeyPrefix();
			}
			else if (strcmp(pCharKey, "OVERRIDE_POP_KEY_POSTFIX") == 0)
			{
				overrides.PopKeyPostfix();
			}
			else if (strcmp(pCharKey, "COMMENT") == 0)
			{
				// We found the COMMENT flag, so set it:
				SetComment(tupIterator.GetAsScalar()->GetAsString());
			}
			else
			{
				// TODO - Notify the user there is some unexpected child of this
				// selection in the config file.
			}
		}
		if (selection)
		{
			// restore the default overrides to the same as the start of call
			overrides.SetSize(nDefaultOverridesSize);
		}
	}
	// compute is we have any ui children
	m_bHasUIChildren = false;
	for (int childIndex = 0; childIndex < m_vectSelectionChildren.GetSize(); childIndex++)
	{
		if (!(m_vectSelectionChildren[childIndex])->GetNonUI())
		{
			m_bHasUIChildren = true;
			break;
		}
	}

	return bSuccess;
}


//-----------------------------------------------------------------------------
// ConfigSelection::BuildUIDisplay
// This method will tell each of the children of this selection to build its
// part of the UIDisplay data.  The data will be stored in the UIController
// passed in and uses the current attrib data passed in.
//
// Input:
//  UIController* pUIController -- The UIController to store the data in.
//  CurrAttribBase* pAttribBase -- The current attrib data to inspect.
//	int nIndentLevel -- Level to indent the UIDisplay control.
//	ExclusiveController* pExclusiveController -- The exclusive controller.
//
// Output: True if successful, false otherwise. 
//-----------------------------------------------------------------------------
bool ConfigSelection::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
									 ExclusiveController* pExclusiveController, int nIndentLevel)
{
	bool bResult = true;

	// A selection doesn't itself add anything to the UIDisplay data, so just
	// cycle through its children telling each one of them to add their data:
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		(m_vectSelectionChildren[nIndex])->BuildUIDisplay(pUIController, pAttribBase,
														   pExclusiveController, nIndentLevel);
	}

	return bResult;
}

//-----------------------------------------------------------------------------
// ConfigSelection::BuildUIDisplay
// This method will tell each of the children of this selection to build its
// part of the UIDisplay data.  The data will be stored in the UIController
// passed in and uses the current attrib data passed in.
//
// This function will allow for only certain children to be built.  It is used
// in parents that desire to show common children.
//
// Input:
//  UIController* pUIController -- The UIController to store the data in.
//  CurrAttribBase* pAttribBase -- The current attrib data to inspect.
//	int nIndentLevel -- Level to indent the UIDisplay control.
//	ExclusiveController* pExclusiveController -- The exclusive controller.
//	const TupArray<bool> &validChildren -- do only ones that are true
//
// Output: True if successful, false otherwise. 
//-----------------------------------------------------------------------------
bool ConfigSelection::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
												 ExclusiveController* pExclusiveController, int nIndentLevel,
												 const TupArray<bool> &validChildren)
{
	bool bResult = true;

	// A selection doesn't itself add anything to the UIDisplay data, so just
	// cycle through its children telling each one of them to add their data:
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		if (validChildren[nIndex])
		{
			(m_vectSelectionChildren[nIndex])->BuildUIDisplay(pUIController, pAttribBase,
				pExclusiveController, nIndentLevel);
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------
// ConfigSelection::BuildExclusiveController
// This method will set up its part of the exclusivecontroller passed in.  It
// then tells all of its children to set up their part.
//
// Input:
//  ExclusiveController* pExclusiveController -- The exclusivecontroller to
//												 set up.
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigSelection::BuildExclusiveController(ExclusiveController* pExclusiveController)
{
	// Have each child of the selection set up its part of the exclusivecontroller:
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		(m_vectSelectionChildren[nIndex])->BuildExclusiveController(pExclusiveController);
	}
}

//-----------------------------------------------------------------------------
// ConfigSelection::FindConfigKeys
// This method is used for searching for specific config keys
//
// Input:
//   Keeper<TupString> &keys -- keep track the requested keys
//	  const TupString &criteria -- type of keys to look for
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigSelection::FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria)
{
	// Now tell all the children to set up their part of the exclusivecontroller:
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		(m_vectSelectionChildren[nIndex])->FindConfigKeys(keys,criteria);
	}
}

//-----------------------------------------------------------------------------
// ConfigSelection::GetKeySelections
// This method will call the children of this object and stop when the result is true
//
// Input:
//  CurrAttribBase* pAttribBase -- access to other object's attributes
//  const KeyValue &path -- a const reference to the keyvalue pairs that denote the path
//  const char *pKeyName -- a const char * that holds the keyname that is used for matching
//  KeySelection &selections -- the selections to be build
//
// Output: bool - True if found on object or child, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigSelection::GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections)
{
	// Have each child of the root build its part of the UIDisplay data:
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		// if this child had the key then return
		if (m_vectSelectionChildren[nIndex]->GetKeySelections(pAttribBase,path,pKeyName,selections))
			return true;
	}
	return false;
}


//-----------------------------------------------------------------------------
// ConfigSelection::GetKeyValueContexts
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
void ConfigSelection::GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys)
{
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		m_vectSelectionChildren[nIndex]->GetKeyValueContexts(pAttribBase,path,keyValueContext,addKeys);
	}
}

//-----------------------------------------------------------------------------
// ConfigSelection::ClearCachedData
// This method will call the entire tree an attempt to clear any cached data
//
// Input: none
//
// Output: none
//
//-----------------------------------------------------------------------------
void ConfigSelection::ClearCachedData(void)
{
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		// do all paths
		m_vectSelectionChildren[nIndex]->ClearCachedData();
	}
}

//-----------------------------------------------------------------------------
// ConfigSelection::GetCRC
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
unsigned long ConfigSelection::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	CRCGenerator.AnalyzeData(m_strKeyName);
	CRCGenerator.AnalyzeData(m_strLabelName);
	for (int childIndex = 0; childIndex < m_vectSelectionChildren.GetSize(); childIndex++)
	{
		ConfigBase *pChild = m_vectSelectionChildren[childIndex];
		CRCGenerator.AnalyzeData(pChild->GetCRC());
	}
	return CRCGenerator.GetResult();
}