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

/*
 * $History: ConfigSelection.cpp $
 * 
 * *****************  Version 20  *****************
 * User: Brad Worthen Date: 8/29/02    Time: 2:09p
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 6/27/02    Time: 2:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Override Keyname additions
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */


//-----------------------------------------------------------------------------
// Includes:

#include "TupperAttribPch.h"
#include "ConfigSelection.h"
#include "ConfigComboBox.h"
#include "ConfigRadioGroup.h"
#include "ConfigCheckBox.h"
#include "ConfigEditBox.h"
#include "ConfigNumeric.h"
#include "ConfigFreeColorBox.h"
#include "ConfigSetColorBox.h"
#include "ConfigCheckList.h"

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
TupConfigSelection::TupConfigSelection()
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
TupConfigSelection::~TupConfigSelection()
{
	int numChildren = m_vectSelectionChildren.GetSize();
	for (int index = 0; index < numChildren; index++)
	{
		TupConfigBase* pCurrChild = m_vectSelectionChildren[index];
		if (pCurrChild != NULL)
		{
			delete pCurrChild;
			pCurrChild = NULL;
		}
	}
}

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
bool TupConfigSelection::BuildConfigTree(TupperwareAggregate* pSelectionTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides,bool selection)
{
	bool bSuccess = true;
	int nDefaultOverridesSize = 0;

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
			m_strName = pSelectionTupAgg->GetName();
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
				TupConfigComboBox* pConfigComboBox = new TupConfigComboBox();
				pConfigComboBox->BuildConfigTree(tupIterator.GetAsAggregate(),setArray,overrides);
				m_vectSelectionChildren.Add(pConfigComboBox);
			}
			else if (strcmp(pCharKey, "RADIO_GROUP") == 0)
			{
				// We found a radio group, so create a radiogroup object and add it to
				// the selection's children:
				TupConfigRadioGroup* pConfigRadioGroup = new TupConfigRadioGroup();
				pConfigRadioGroup->BuildConfigTree(tupIterator.GetAsAggregate(),setArray,overrides);
				m_vectSelectionChildren.Add(pConfigRadioGroup);
			}
			else if (strcmp(pCharKey, "CHECK_BOX") == 0)
			{
				// We found a checkbox, so create a checkbox object and add it to
				// the selection's children:
				TupConfigCheckBox* pConfigCheckBox = new TupConfigCheckBox();
				pConfigCheckBox->BuildConfigTree(tupIterator.GetAsAggregate(),setArray,overrides);
				m_vectSelectionChildren.Add(pConfigCheckBox);
			}
			else if (strcmp(pCharKey, "EDIT_BOX") == 0)
			{
				// We found an edit box, so create an editbox object and add it to
				// the selection's children:
				TupConfigEditBox* pConfigEditBox = new TupConfigEditBox();
				pConfigEditBox->BuildConfigTree(tupIterator.GetAsAggregate(),overrides);
				m_vectSelectionChildren.Add(pConfigEditBox);
			}
			else if (strcmp(pCharKey, "NUMERIC") == 0)
			{
				// We found a numeric, so create a numeric object and add it to
				// the selection's children:
				TupConfigNumeric* pConfigNumeric = new TupConfigNumeric();
				pConfigNumeric->BuildConfigTree(tupIterator.GetAsAggregate(),overrides);
				m_vectSelectionChildren.Add(pConfigNumeric);
			}
			else if (strcmp(pCharKey, "FREE_COLOR_BOX") == 0)
			{
				// We found a free color box, so create a freecolorbox object and
				// add it to the selection's children:
				TupConfigFreeColorBox* pConfigFreeColorBox = new TupConfigFreeColorBox();
				pConfigFreeColorBox->BuildConfigTree(tupIterator.GetAsAggregate(),overrides);
				m_vectSelectionChildren.Add(pConfigFreeColorBox);
			}
			else if (strcmp(pCharKey, "SET_COLOR_BOX") == 0)
			{
				// We found a set color box, so create a setcolorbox object and
				// add it to the selection's children:
				TupConfigSetColorBox* pConfigSetColorBox = new TupConfigSetColorBox();
				pConfigSetColorBox->BuildConfigTree(tupIterator.GetAsAggregate(),overrides);
				m_vectSelectionChildren.Add(pConfigSetColorBox);
			}
			else if (strcmp(pCharKey, "CHECK_LIST") == 0)
			{
				// We found a checklist, so create a checklist object and
				// add it to the selection's children:
				TupConfigCheckList* pConfigCheckList = new TupConfigCheckList();
				pConfigCheckList->BuildConfigTree(tupIterator.GetAsAggregate(), overrides);
				m_vectSelectionChildren.Add(pConfigCheckList);
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

	return bSuccess;
}

//-----------------------------------------------------------------------------
// TupConfigSelection::GetKeySelections
// This method will call the children of this object and stop when the result is true
//
// Input:
//  const TupKeyValue &path -- a const reference to the keyvalue pairs that denote the path
//  const char *pKeyName -- a const char * that holds the keyname that is used for matching
//  TupKeySelection &selections -- the selections to be build
//
// Output: bool - True if found on object or child, false otherwise.
//-----------------------------------------------------------------------------
bool TupConfigSelection::GetKeySelections(const TupKeyValue &path,const char *pKeyName,TupKeySelection &selections) const
{
	// Have each child of the root build its part of the UIDisplay data:
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		// if this child had the key then return
		if (m_vectSelectionChildren[nIndex]->GetKeySelections(path,pKeyName,selections))
			return true;
	}
	return false;
}


//-----------------------------------------------------------------------------
// TupConfigSelection::GetKeyValueContexts
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
void TupConfigSelection::GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const
{
	int nNumChildren = m_vectSelectionChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		m_vectSelectionChildren[nIndex]->GetKeyValueContexts(path,keyValueContext,addKeys);
	}
}
