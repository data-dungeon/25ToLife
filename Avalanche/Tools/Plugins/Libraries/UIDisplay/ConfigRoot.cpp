//-----------------------------------------------------------------------------
//	File:		ConfigRoot.cpp
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigRoot class represents the outermost level in the
//			configuration file data.  The configuration file itself can be
//			thought of as the outermost level, in a way.  That is because the
//			file can contain more than one atom, an of which may be an
//			aggragate, and so on, and so on....
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigRoot.h"
#include "ConfigComboBox.h"
#include "ConfigRadioGroup.h"
#include "ConfigCheckBox.h"
#include "ConfigEditBox.h"
#include "ConfigNumeric.h"
#include "ConfigFreeColorBox.h"
#include "ConfigSetColorBox.h"
#include "ConfigCheckList.h"
#include "ConfigLabel.h"
#include "UIController.h"
#include "ExclusiveController.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ConfigRoot::ConfigRoot
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigRoot::ConfigRoot()
{
}


//-----------------------------------------------------------------------------
// ConfigRoot::~ConfigRoot
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigRoot::~ConfigRoot()
{
	int numChildren = m_vectRootChildren.GetSize();
	for (int index = 0; index < numChildren; index++)
	{
		ConfigBase* pCurrChild = m_vectRootChildren[index];
		if (pCurrChild != NULL)
		{
			delete pCurrChild;
			pCurrChild = NULL;
		}
	}
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigRoot::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.  It will also tell any of its children to print their
// contents.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigRoot::PrintDebug(int nIndentLevel) const
{

	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs +="\t";
	}

	ATDebugPrint(strTabs);
	ATDebugPrint("ROOT\n");
	int nNumChildren = m_vectRootChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		((ConfigBase*) m_vectRootChildren[nIndex])->PrintDebug(nIndentLevel + 1);
	}
	ATDebugPrint("%s}\n", strTabs);
}
#endif


//-----------------------------------------------------------------------------
// ConfigRoot::BuildConfigTree
// This method will start to build the configuration tree.  It builds the
// necessary data at the root level and then tells each of the children to
// build their part of the config tree.
//
// Input:
//  TupperwareAggregate *rootTupAgg -- A pointer to the root tupperware
//									   aggregate.
//	bool root -- indicates if this is that root call - this routine is 
//				 recursive due to the addition of set entries
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigRoot::BuildConfigTree(TupperwareAggregate *rootTupAgg,bool root)
{
	bool bSuccess = true;
	// if this is the root call set up the set entries
	if (root)
	{
		m_setArray.RemoveAll(); // start fresh
		m_overrides.SetSize(0); // empty the default overrides to 0
		// scan for set entries and add the aggregate pointers to the array
		for( TupperwareIterator tupSetIterator = rootTupAgg->FindByKey("SET_ENTRY"); !tupSetIterator.Finished(); tupSetIterator.Next() )
		{
			m_setArray.Add(tupSetIterator.GetAsAggregate());
		}
	}

	// Cycle through all the children of the root, building tree as we go:
	for( TupperwareIterator tupIterator = rootTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
	{
		// Check to see what the current atom's key is:
		TupperwareAtom *pTupAtom = tupIterator.GetAsAtom();
		const char *pCharKey = pTupAtom->GetKey();

		if (strcmp(pCharKey, "VERSION_NUMBER") == 0)
		{
			// TODO - If needed, use the version number of the config file.
		}
		else if (strcmp(pCharKey, "COMBO_BOX") == 0)
		{
			// We found a combo-box, so create a combobox object and add it to
			// the root's children:
			ConfigComboBox* pConfigComboBox = new ConfigComboBox();
			pConfigComboBox->BuildConfigTree(tupIterator.GetAsAggregate(), m_setArray, m_overrides);
			m_vectRootChildren.Add(pConfigComboBox);
		}
		else if (strcmp(pCharKey, "RADIO_GROUP") == 0)
		{
			// We found a radio group, so create a radiogroup object and add it to
			// the root's children:
			ConfigRadioGroup* pConfigRadioGroup = new ConfigRadioGroup();
			pConfigRadioGroup->BuildConfigTree(tupIterator.GetAsAggregate(), m_setArray, m_overrides);
			m_vectRootChildren.Add(pConfigRadioGroup);
		}
		else if (strcmp(pCharKey, "CHECK_BOX") == 0)
		{
			// We found a checkbox, so create a checkbox object and add it to
			// the root's children:
			ConfigCheckBox* pConfigCheckBox = new ConfigCheckBox();
			pConfigCheckBox->BuildConfigTree(tupIterator.GetAsAggregate(), m_setArray, m_overrides);
			m_vectRootChildren.Add(pConfigCheckBox);
		}
		else if (strcmp(pCharKey, "EDIT_BOX") == 0)
		{
			// We found an edit box, so create an editbox object and add it to
			// the root's children:
			ConfigEditBox* pConfigEditBox = new ConfigEditBox();
			pConfigEditBox->BuildConfigTree(tupIterator.GetAsAggregate(), m_overrides);
			m_vectRootChildren.Add(pConfigEditBox);
		}
		else if (strcmp(pCharKey, "NUMERIC") == 0)
		{
			// We found a numeric, so create a numeric object and add it to
			// the root's children:
			ConfigNumeric* pConfigNumeric = new ConfigNumeric();
			pConfigNumeric->BuildConfigTree(tupIterator.GetAsAggregate(), m_overrides);
			m_vectRootChildren.Add(pConfigNumeric);
		}
		else if (strcmp(pCharKey, "FREE_COLOR_BOX") == 0)
		{
			// We found a free color box, so create a freecolorbox object and
			// add it to the root's children:
			ConfigFreeColorBox* pConfigFreeColorBox = new ConfigFreeColorBox();
			pConfigFreeColorBox->BuildConfigTree(tupIterator.GetAsAggregate(), m_overrides);
			m_vectRootChildren.Add(pConfigFreeColorBox);
		}
		else if (strcmp(pCharKey, "SET_COLOR_BOX") == 0)
		{
			// We found a set color box, so create a setcolorbox object and
			// add it to the root's children:
			ConfigSetColorBox* pConfigSetColorBox = new ConfigSetColorBox();
			pConfigSetColorBox->BuildConfigTree(tupIterator.GetAsAggregate(), m_overrides);
			m_vectRootChildren.Add(pConfigSetColorBox);
		}
		else if (strcmp(pCharKey, "CHECK_LIST") == 0)
		{
			// We found a check list, so create a checklist object and
			// add it to the root's children:
			ConfigCheckList* pConfigCheckList = new ConfigCheckList();
			pConfigCheckList->BuildConfigTree(tupIterator.GetAsAggregate(), m_overrides);
			m_vectRootChildren.Add(pConfigCheckList);
		}
		else if (strcmp(pCharKey, "LABEL") == 0)
		{
			// We found a check list, so create a checklist object and
			// add it to the root's children:
			ConfigLabel* pConfigLabel = new ConfigLabel();
			pConfigLabel->BuildConfigTree(tupIterator.GetAsAggregate(), m_overrides);
			m_vectRootChildren.Add(pConfigLabel);
		}
		else if (strcmp(pCharKey, "SET_ENTRY") == 0)
		{
			// ignore here.
			// this aggregate may be referenced anywhere through the use of 
			// SET_REFERENCE
		}
		else if (strcmp(pCharKey, "SET_REFERENCE") == 0)
		{
			TupperwareScalar *pReferenceScalar = tupIterator.GetAsScalar();
			assert(pReferenceScalar);
			char *pString = pReferenceScalar->GetAsString();
			assert(pString);
			// scan through the set array to find a match
			int totalSetEntries = m_setArray.GetSize();
			for (int setIndex=0;setIndex<totalSetEntries;setIndex++)
			{
				TupperwareAggregate *pSetEntryAgg = m_setArray[setIndex];
				// does the name match?
				if (strcmp(pString,pSetEntryAgg->GetName()) == 0)
				{
					//call again but use the set entry aggregate
					BuildConfigTree(pSetEntryAgg,false);
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
			m_overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_DEFAULT);
		}
		else if (strcmp(pCharKey, "OVERRIDE_NONUI") == 0)
		{
			TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
			assert(pOverrideScalar);
			const char *pValue = pOverrideScalar->GetAsString();
			const char *pKey = pOverrideScalar->GetName();
			assert(pKey);
			assert(pValue);
			m_overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_NONUI);
		}
		else if (strcmp(pCharKey, "OVERRIDE_DISABLED") == 0)
		{
			TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
			assert(pOverrideScalar);
			const char *pValue = pOverrideScalar->GetAsString();
			const char *pKey = pOverrideScalar->GetName();
			assert(pKey);
			assert(pValue);
			m_overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_DISABLED);
		}
		else if (strcmp(pCharKey, "OVERRIDE_LABEL") == 0)
		{
			TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
			assert(pOverrideScalar);
			const char *pValue = pOverrideScalar->GetAsString();
			const char *pKey = pOverrideScalar->GetName();
			assert(pKey);
			assert(pValue);
			m_overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_LABEL);
		}
		else if (strcmp(pCharKey, "OVERRIDE_COMMENT") == 0)
		{
			TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
			assert(pOverrideScalar);
			const char *pValue = pOverrideScalar->GetAsString();
			const char *pKey = pOverrideScalar->GetName();
			assert(pKey);
			assert(pValue);
			m_overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_COMMENT);
		}
		else if (strcmp(pCharKey, "OVERRIDE_KEY") == 0)
		{
			TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
			assert(pOverrideScalar);
			const char *pValue = pOverrideScalar->GetAsString();
			const char *pKey = pOverrideScalar->GetName();
			assert(pKey);
			assert(pValue);
			m_overrides.AddEntry(pKey,pValue,ConfigOverrides::OVERRIDE_KEY);
		}
		else if (strcmp(pCharKey, "OVERRIDE_ADD_KEY_PREFIX") == 0)
		{
			TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
			assert(pOverrideScalar);
			const char *pValue = pOverrideScalar->GetAsString();
			assert(pValue);
			m_overrides.AddKeyPrefix(pValue);
		}
		else if (strcmp(pCharKey, "OVERRIDE_ADD_KEY_POSTFIX") == 0)
		{
			TupperwareScalar *pOverrideScalar = tupIterator.GetAsScalar();
			assert(pOverrideScalar);
			const char *pValue = pOverrideScalar->GetAsString();
			assert(pValue);
			m_overrides.AddKeyPostfix(pValue);
		}
		else if (strcmp(pCharKey, "OVERRIDE_POP_KEY_PREFIX") == 0)
		{
			m_overrides.PopKeyPrefix();
		}
		else if (strcmp(pCharKey, "OVERRIDE_POP_KEY_POSTFIX") == 0)
		{
			m_overrides.PopKeyPostfix();
		}
		else
		{
			// TODO - Notify the user there is an error at root level of
			// config file.
		}

	}
	// get rid of stuff we don't need if we are the root
	if (root)
	{
		m_setArray.RemoveAll(); // start fresh
		m_overrides.SetSize(0); // empty the default overrides to 0
	}
	return bSuccess;
}


//-----------------------------------------------------------------------------
// ConfigRoot::BuildUIDisplay
// This method will build the display data using this configuration tree and
// the current attrib data passed in.
//
// Input:
//  UIController* pUIController -- Pointer to the class holding the display
//								   data.
//  CurrAttribBase* pAttribBase -- Pointer to the current attrib data controller.
//	ExclusiveController* pExclusiveController -- Pointer to the exclusive
//												 controller.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigRoot::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
								ExclusiveController* pExclusiveController)
{
	bool bResult = true;

	// Have each child of the root build its part of the UIDisplay data:
	int nNumChildren = m_vectRootChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		(m_vectRootChildren[nIndex])->BuildUIDisplay(pUIController, pAttribBase, pExclusiveController, 0);
	}

	return bResult;
}


//-----------------------------------------------------------------------------
// ConfigRoot::BuildExclusiveController
// This method will set up the exclusivecontroller passed in based on the
// exclusive flags set in the config tree.
//
// Input:
//  ExclusiveController* pExclusiveController -- The exclusivecontroller to
//												 set up.
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigRoot::BuildExclusiveController(ExclusiveController* pExclusiveController)
{
	// Have each child of the root set up its part of the exclusivecontroller:
	int nNumChildren = m_vectRootChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		(m_vectRootChildren[nIndex])->BuildExclusiveController(pExclusiveController);
	}
}

//-----------------------------------------------------------------------------
// ConfigRoot::FindConfigKeys
// This method is used for searching for specific config keys
//
// Input:
//   Keeper<TupString> &keys -- keep track the requested keys
//	  const TupString &criteria -- type of keys to look for
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigRoot::FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria)
{
	// Now tell all the children to set up their part of the exclusivecontroller:
	int nNumChildren = m_vectRootChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		(m_vectRootChildren[nIndex])->FindConfigKeys(keys,criteria);
	}
}

//-----------------------------------------------------------------------------
// ConfigRoot::GetKeySelections
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
bool ConfigRoot::GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections)
{
	int nNumChildren = m_vectRootChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		// if this child had the key then return
		if (m_vectRootChildren[nIndex]->GetKeySelections(pAttribBase,path,pKeyName,selections))
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// ConfigRoot::GetKeyValueContexts
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
void ConfigRoot::GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys)
{
	int nNumChildren = m_vectRootChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		m_vectRootChildren[nIndex]->GetKeyValueContexts(pAttribBase,path,keyValueContext,addKeys);
	}
}

//-----------------------------------------------------------------------------
// ConfigRoot::ClearCachedData
// This method will call the entire tree an attempt to clear any cached data
//
// Input: none
//
// Output: none
//
//-----------------------------------------------------------------------------
void ConfigRoot::ClearCachedData(void)
{
	int nNumChildren = m_vectRootChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		// do all paths
		m_vectRootChildren[nIndex]->ClearCachedData();
	}
}

//-----------------------------------------------------------------------------
// ConfigRoot::GetCRC
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
unsigned long ConfigRoot::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	for (int childIndex = 0; childIndex < m_vectRootChildren.GetSize(); childIndex++)
	{
		ConfigBase *pChild = m_vectRootChildren[childIndex];
		CRCGenerator.AnalyzeData(pChild->GetCRC());
	}
	return CRCGenerator.GetResult();
}