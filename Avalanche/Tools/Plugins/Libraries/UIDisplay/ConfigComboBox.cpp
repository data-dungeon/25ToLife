//-----------------------------------------------------------------------------
//	File:		ConfigComboBox.cpp
//	Created:	Nov. 16, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigComboBox class represents the combo box configuration
//			data.
//
//	History:	11/16/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigComboBox.h"
#include "ConfigSelection.h"
#include "UIController.h"
#include "CurrAttribBase.h"
#include "UIDisplayCombo.h"
#include "ExclusiveController.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

//-----------------------------------------------------------------------------
// Defines, enums, etc.:

static int gCurrComboBoxId = RESOURCE_START_COMBOBOX;


//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ConfigComboBox::ConfigComboBox
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigComboBox::ConfigComboBox()
{
}


//-----------------------------------------------------------------------------
// ConfigComboBox::~ConfigComboBox
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigComboBox::~ConfigComboBox()
{
	int numChildren = m_vectComboChildren.GetSize();
	for (int index = 0; index < numChildren; index++)
	{
		ConfigSelection* pCurrChild = m_vectComboChildren[index];
		if (pCurrChild != NULL)
		{
			delete pCurrChild;
			pCurrChild = NULL;
		}
	}
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigComboBox::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.  It will also tell any of its children to print their
// contents.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigComboBox::PrintDebug(int nIndentLevel) const
{
	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs+="\t";
	}

	ATDebugPrint(strTabs);
	ATDebugPrint("COMBO_BOX ");
	ATDebugPrint("DefaultIndex = %d ", m_nDefaultIndex);
	ATDebugPrint(" Key %s Label %s",(const char *)m_strKeyName,(const char *)m_strLabelName);
	ATDebugPrint(" = {\n");
	int nNumChildren = m_vectComboChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		((ConfigSelection*) m_vectComboChildren[nIndex])->PrintDebug(nIndentLevel + 1);
	}
	ATDebugPrint("%s}\n", strTabs);
}
#endif


//-----------------------------------------------------------------------------
// ConfigComboBox::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this combo box.  It builds the necessary data at this combobox level and
// then tells each of the children to build their part of the config tree.
//
// Input:
//  TupperwareAggregate *pComboBoxTupAgg -- A pointer to the combobox tupperware
//											aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigComboBox::BuildConfigTree(TupperwareAggregate *pComboBoxTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides)
{
	bool bSuccess = true;
	
	if (pComboBoxTupAgg == NULL || pComboBoxTupAgg->GetLastChildAtom() == NULL)
	{
		// TODO - A combo box has to be an aggregate and it has to have
		// children, so throw up an error!
	}
	else
	{
		// Set the name:
		TupString strName = pComboBoxTupAgg->GetName();

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
		// override the nonui flag
		bool bNonUIOverride = false;
		keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_NONUI);
		if (keyIndex!=-1)
		{
			if (overrides.GetValueAsInt(keyIndex)==1)
				m_bNonUI = true;
			bNonUIOverride = true;
		}
		// override the disabled flag
		bool bDisabledOverride = false;
		keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_DISABLED);
		if (keyIndex!=-1)
		{
			if (overrides.GetValueAsInt(keyIndex)==1)
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
		// override the keyname
		keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_KEY);
		if (keyIndex!=-1)
		{
			m_strKeyName = overrides.GetValueAsString(keyIndex);
		}
		// check for prefix and postfix
		if (overrides.HasPrefixOrPostfix())
		{
			overrides.AddPrefixAndPostfix(m_strKeyName);
		}

		// Cycle through all the children of the combobox, building tree as we go:
		for( TupperwareIterator tupIterator = pComboBoxTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
		{
			// Check to see what the current atom's key is:
			TupperwareAtom *pTupAtom = tupIterator.GetAsAtom();
			const char *pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "DEFAULT_SELECTION") == 0)
			{
				// We found the default selection, so set index and add a selection:
				m_nDefaultIndex = m_vectComboChildren.GetSize();
				ConfigSelection* pConfigSelection = new ConfigSelection();
				pConfigSelection->BuildConfigTree(tupIterator.GetAsAggregate(),setArray,overrides);
				m_vectComboChildren.Add(pConfigSelection);
				// TODO - If there is more than one DEFAULT_SELECTION, notify the
				// user there is an error.
			}
			else if (strcmp(pCharKey, "SELECTION") == 0)
			{
				// check if this name is the default instead
				if (bDefaultOverride)
				{
					TupString strChildName(pTupAtom->GetName());
					TupString strChildKey(ReturnKeyNameSubstr(strChildName));
					if (!strcmp(strDefaultOverrideValue,strChildKey))
						nOverrideDefaultIndex = m_vectComboChildren.GetSize();
				}

				// We found a selection, so create one and add it to the children:
				ConfigSelection* pConfigSelection = new ConfigSelection();
				pConfigSelection->BuildConfigTree(tupIterator.GetAsAggregate(),setArray,overrides);
				m_vectComboChildren.Add(pConfigSelection);
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
			else if (strcmp(pCharKey, "DISABLED") == 0)
			{
				if (!bDisabledOverride)
				{
					// set the non ui flag if set to 1
					if (tupIterator.GetAsScalar()->GetAsInt()==1)
						m_bDisabled = true;
				}
			}
			else if (strcmp(pCharKey, "EXCLUSIVE") == 0)
			{
				// clear the exclusive flag if set to 0
				if (tupIterator.GetAsScalar()->GetAsInt()==0)
					m_bExclusive = false;
			}
			else if (strcmp(pCharKey, "ADDED_INDENT") == 0)
			{
				// We found the ADDED_INDENT flag, so set it:
				SetAddedIndent(tupIterator.GetAsScalar()->GetAsInt());
			}
			else if (strcmp(pCharKey, "COMMENT") == 0)
			{
				if (!bCommentOverride)
				{
					// We found the COMMENT flag, so set it:
					SetComment(tupIterator.GetAsScalar()->GetAsString());
				}
			}
			else if (strcmp(pCharKey, "PURGE_KEY") == 0)
			{
				// We found the PURGE flag, so set it up:
				if (pTupAtom->GetAtomType() == TupperwareAtom::ATOM_LIST)
				{
					// The keys to purge are specified in a list:
					TupperwareList* pTupList = (TupperwareList*) pTupAtom;
					if (pTupList->GetDataType() == TupperwareList::DATA_STRING)
					{
						char** pStringListData = pTupList->GetAsString();
						int nLength = pTupList->GetLength();
						for (int stringIndex = 0; stringIndex < nLength; stringIndex++)
						{
							TupString newPurgeKey(pStringListData[stringIndex]);
							m_vectKeysToPurge.Add(newPurgeKey);
						}
					}
				}
				else if (pTupAtom->GetAtomType() == TupperwareAtom::ATOM_SCALAR)
				{
					// The key to purge is a string scaler
					TupperwareScalar* pTupScalar = (TupperwareScalar*) pTupAtom;
					if (pTupScalar->GetDataType() == TupperwareScalar::DATA_STRING)
					{
						TupString newPurgeKey(pTupScalar->GetAsString());
						m_vectKeysToPurge.Add(newPurgeKey);
					}
				}
			}
			else
			{
				// TODO - Notify the user there is an error at combobox level of
				// config file.
			}
			// TODO - Notify the user if there are no children of the combobox.
		}
		// override the default if we found one
		if (nOverrideDefaultIndex!=-1)
			m_nDefaultIndex = nOverrideDefaultIndex;
	}

	return bSuccess;
}


//-----------------------------------------------------------------------------
// ConfigComboBox::BuildUIDisplay
// This method will build the UIDisplay data for this combo box.  The data will
// be stored in the UIController passed in and uses the current attrib data
// passed in, and the exclusive controller passed in.
//
// Input:
//  UIController* pUIController -- The UIController to store the data in.
//  CurrAttribBase* pAttribBase -- The current attrib data to inspect.
//	int nIndentLevel -- Level to indent the UIDisplay control.
//	ExclusiveController* pExclusiveController -- The exclusive controller to
//												 inspect.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigComboBox::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
									ExclusiveController* pExclusiveController, int nIndentLevel)
{
	bool bResult = true;

	// Check the non ui flag to see if this control (and it's children)
	// should even be shown:
	if (GetNonUI() == true)
		return bResult;

	UIDisplayCombo* pDisplayCombo = new UIDisplayCombo();

	// Set the label and keyname:
	pDisplayCombo->SetLabel(m_strLabelName);
	pDisplayCombo->SetKeyname(m_strKeyName);

	// Set the indent level:
	pDisplayCombo->SetIndentLevel(nIndentLevel);
	pDisplayCombo->SetAddedIndent(GetAddedIndent());

	// Set the exclusive flag:
	pDisplayCombo->SetExclusive(m_bExclusive);

	// Set the disabled flag:
	pDisplayCombo->SetDisabled(m_bDisabled);

	// Set the toolTip
	pDisplayCombo->SetToolTip(m_strLabelName,m_strKeyName,m_strComment);

	// Set the label id:
	if (m_strComment.GetLength()) // get id if there is a comment
	{
		pDisplayCombo->SetLabelID(gCurrComboBoxId);
		gCurrComboBoxId++;
		if (gCurrComboBoxId == RESOURCE_END_COMBOBOX)
			gCurrComboBoxId = RESOURCE_START_COMBOBOX;
	}
	else
	{
		pDisplayCombo->SetLabelID(IDC_ALL_LABELS);
	}

	// Set the control id:
	pDisplayCombo->SetControlID(gCurrComboBoxId);
	gCurrComboBoxId++;
	if (gCurrComboBoxId == RESOURCE_END_COMBOBOX)
		gCurrComboBoxId = RESOURCE_START_COMBOBOX;

	// Check to see what the display type should be:
	DisplayType dispType;
	TupString strDefaultVal = (m_vectComboChildren[m_nDefaultIndex])->GetKeyName();
	TupString strNonExclusiveValue = "";
	UniqueValues uniqueValues;
	Keeper<TupString> validValues;
	TupArray<int> validValueIndices;
	// build a list of valid values
	int nIndex;
	int nNumChildren = m_vectComboChildren.GetSize();
	for (nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		TupString strValue((m_vectComboChildren[nIndex])->GetKeyName());
		int oldSize = validValues.GetSize();
		int newIndex = validValues.Add(strValue);
		// if unique then add to indices table
		if (oldSize == newIndex)
		{
			validValueIndices.Add(nIndex);
		}
	}

	if (m_bExclusive)
	{
		// Check the current attrib data to see what the display type should be:
		dispType = FindDisplayType(m_strKeyName, strDefaultVal, pAttribBase, uniqueValues, &validValues);

		pDisplayCombo->SetUniqueValues(uniqueValues);
		pDisplayCombo->SetUniqueValuesID(gCurrComboBoxId);
		gCurrComboBoxId++;
		if (gCurrComboBoxId == RESOURCE_END_COMBOBOX)
			gCurrComboBoxId = RESOURCE_START_COMBOBOX;
	}
	else
	{
		// Check the exclusive controller to see what the display type should be:
		if (pExclusiveController->GetKeyVal(m_strKeyName, strNonExclusiveValue))
		{
			dispType = UNIQUE;
		}
		else
		{
			dispType = DEFAULT;
		}
	}

	// Create the list of keys to purge:
	int nNumPurgeKeys = m_vectKeysToPurge.GetSize();
	for (int nLoop = 0; nLoop < nNumPurgeKeys; nLoop++)
	{
		pDisplayCombo->AddPurgeKey(m_vectKeysToPurge[nLoop]);
	}

	// Create the list of text selections:
	int nFoundIndex = -1;
	for (nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		TupString strChildLabelName((m_vectComboChildren[nIndex])->GetLabelName());
		TupString strChildKeyName((m_vectComboChildren[nIndex])->GetKeyName());
		pDisplayCombo->AddComboKey(strChildKeyName);
		pDisplayCombo->AddComboToolTip(strChildLabelName,strChildKeyName,(m_vectComboChildren[nIndex])->GetComment());
		// add * in front if the selection has children
		if ((m_vectComboChildren[nIndex])->HasUIChildren())
			strChildLabelName.Insert(0,"*");
		pDisplayCombo->AddComboLabel(strChildLabelName);
		if (m_bExclusive)
		{
			if (dispType == UNIQUE)
			{
				if (strChildKeyName == uniqueValues.m_uniqueValues[uniqueValues.m_uniqueIndex].m_strValue)
				{
					nFoundIndex = nIndex;
				}
			}
		}
		else
		{
			if (strChildKeyName==strNonExclusiveValue)
			{
				nFoundIndex = nIndex;
			}
		}
	}
	// if we didn't find the index (usually only on non exclusives)
	if (dispType == UNIQUE && nFoundIndex==-1)
	{
		dispType = DEFAULT;
	}

	if (dispType == UNIQUE)
	{
		pDisplayCombo->SetShowIndeterminate(false);
		pDisplayCombo->SetSelectionIndex(nFoundIndex);
	}
	else if (dispType == INDETERMINATE)
	{
		pDisplayCombo->SetShowIndeterminate(true);
		pDisplayCombo->SetSelectionIndex(-1);
	}
	else if (dispType == DEFAULT)
	{
		pDisplayCombo->SetShowIndeterminate(false);
		pDisplayCombo->SetSelectionIndex(m_nDefaultIndex);
	}

	// Add the UIDisplayCombo to the UIController:
	pUIController->AddDisplayObject(pDisplayCombo);

	// Tell the selected child of this combo box to build its UIDisplay data:
	if (dispType == UNIQUE)
	{
		(m_vectComboChildren[nFoundIndex])->BuildUIDisplay(pUIController, pAttribBase,
			pExclusiveController, nIndentLevel + 1);
	}
	else if (dispType == DEFAULT)
	{
		(m_vectComboChildren[m_nDefaultIndex])->BuildUIDisplay(pUIController, pAttribBase,
			pExclusiveController, nIndentLevel + 1);
	}
	// It's indeterminate, build the ui display for common children of each unique value
	else  if (dispType == INDETERMINATE)
	{
		// build a list of requested selections
		TupArray<int> requestedChildren;
      for (int uniqueValueIndex = 0; uniqueValueIndex < uniqueValues.m_uniqueValues.GetSize(); uniqueValueIndex++)
		{
			// actually used
			if (uniqueValues.m_uniqueValues[uniqueValueIndex].m_count)
			{
				int validValueIndex = validValues.GetIndex(uniqueValues.m_uniqueValues[uniqueValueIndex].m_strValue);
				if (validValueIndex != -1)
				{
					int childIndex = validValueIndices[validValueIndex];
					requestedChildren.Add(childIndex);
				}
			}
		}
		// make sure some children to check through
		if (requestedChildren.GetSize()>=2)
		{
			ConfigSelection *pFirstChild = m_vectComboChildren[requestedChildren[0]];
			int numFirstGrandChildren = pFirstChild->GetNumChildren();
			TupArray<bool> validGrandChildren;
			validGrandChildren.SetSize(numFirstGrandChildren);
			// loop through the children of the first selected child
			// try to find grandchild matches with the other selected children
			for (int firstGrandChildIndex = 0; firstGrandChildIndex < numFirstGrandChildren; firstGrandChildIndex++)
			{
				bool bFirstGrandChildValid = true;

				ConfigBase *pFirstGrandChild = pFirstChild->GetChild(firstGrandChildIndex);
				unsigned long firstGrandChildCRC = pFirstGrandChild->GetCRC();
				for (int otherChildIndex = 1; otherChildIndex < requestedChildren.GetSize(); otherChildIndex++)
				{
					ConfigSelection *pOtherChild = m_vectComboChildren[requestedChildren[otherChildIndex]];
					int numOtherGrandChildren = pOtherChild->GetNumChildren();
					bool bFound = false;
					for (int otherGrandChildIndex = 0; otherGrandChildIndex < numOtherGrandChildren; otherGrandChildIndex++)
					{
						ConfigBase *pOtherGrandChild = pOtherChild->GetChild(otherGrandChildIndex);
						unsigned long otherGrandChildCRC = pOtherGrandChild->GetCRC();
						if (firstGrandChildCRC == otherGrandChildCRC)
						{
							bFound = true; // found a match from this selected child
							break;
						}
					}
					// if the other child does not have a child that matches
					if (!bFound)
					{
						bFirstGrandChildValid = false; // since one does not have a match we can't show it
						break;
					}
				}
				validGrandChildren[firstGrandChildIndex] = bFirstGrandChildValid;
			}
			// call the first child and do the grandchildren that all selections have in common
			pFirstChild->BuildUIDisplay(pUIController, pAttribBase,
				pExclusiveController, nIndentLevel + 1,validGrandChildren);
		}
	}
	return bResult;
}


//-----------------------------------------------------------------------------
// ConfigComboBox::BuildExclusiveController
// This method will set up its part of the exclusivecontroller passed in.  It
// then tells all of its children to set up their part.
//
// Input:
//  ExclusiveController* pExclusiveController -- The exclusivecontroller to set
//												 up.
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigComboBox::BuildExclusiveController(ExclusiveController* pExclusiveController)
{
	// If necessary, set the exclusivecontroller based on this combobox:
	if (m_bExclusive == false)
	{
		pExclusiveController->SetKeeperFlag(m_strKeyName);
	}

	// Now tell all the children to set up their part of the exclusivecontroller:
	int nNumChildren = m_vectComboChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		(m_vectComboChildren[nIndex])->BuildExclusiveController(pExclusiveController);
	}
}

//-----------------------------------------------------------------------------
// ConfigComboBox::FindConfigKeys
// This method is used for searching for specific config keys
//
// Input:
//   Keeper<TupString> &keys -- keep track the requested keys
//	  const TupString &criteria -- type of keys to look for
//
// Output: None.
//-----------------------------------------------------------------------------
void ConfigComboBox::FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria)
{
	// Now tell all the children to set up their part of the exclusivecontroller:
	int nNumChildren = m_vectComboChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		(m_vectComboChildren[nIndex])->FindConfigKeys(keys,criteria);
	}
}

//-----------------------------------------------------------------------------
// ConfigComboBox::GetKeySelections
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
bool ConfigComboBox::GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections)
{
	// if the key we are looking for is me
	if (!m_strKeyName.Compare(pKeyName))
	{
		selections.Clear();
		int nNumChildren = m_vectComboChildren.GetSize();
		for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
		{
			// add selection children
			selections.AddSelection((m_vectComboChildren[nIndex])->GetKeyName(),nIndex==m_nDefaultIndex);
		}
		return true;
	}
	if (GetExclusive()) // only one active path
	{
		int childIndex = m_nDefaultIndex; // start with default
		if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
		{
			// if we are not a match then continue down the path
			int keyIndex = path.KeyExistsAt(m_strKeyName);
			if (keyIndex!=-1)
			{
				int nNumChildren = m_vectComboChildren.GetSize();
				for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
				{
					TupString strChildName = (m_vectComboChildren[nIndex])->GetKeyName();
					if (!strChildName.Compare(path.GetValueAsString(keyIndex)))
					{
						childIndex = nIndex; // update child to this one
						break;
					}
				}
			}
		}
		// call to correct child in path
		return m_vectComboChildren[childIndex]->GetKeySelections(pAttribBase,path,pKeyName,selections);
	}
	else // if all children are active paths
	{
		int nNumChildren = m_vectComboChildren.GetSize();
		for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
		{
			// do all paths (until we hit one that worked)
			if (m_vectComboChildren[nIndex]->GetKeySelections(pAttribBase,path,pKeyName,selections))
				return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// ConfigComboBox::GetKeyValueContexts
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
void ConfigComboBox::GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys)
{
	if (GetExclusive()) // only one active path
	{
		int keyValueIndex = -1;
		bool bIsDefaultValue = true;
		int childIndex = m_nDefaultIndex; // start with default
		TupString defaultValue = (m_vectComboChildren[m_nDefaultIndex])->GetKeyName();
		if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
		{
			// if we are not a match then continue down the path
			int keyIndex = path.KeyExistsAt(m_strKeyName);
			if (keyIndex!=-1)
			{
				int nNumChildren = m_vectComboChildren.GetSize();
				for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
				{
					TupString strChildName = (m_vectComboChildren[nIndex])->GetKeyName();
					if (!strChildName.Compare(path.GetValueAsString(keyIndex)))
					{
						keyValueIndex = keyIndex;
						if (nIndex != childIndex)
						{
							bIsDefaultValue = false;
							childIndex = nIndex; // update child to this one
						}
						break;
					}
				}
			}
		}
		// add if addkeys or key already exists within context
		if (addKeys || keyValueContext.KeyExistsAt(m_strKeyName)!=-1)
		{
			keyValueContext.AddKeyValue(m_strKeyName,(m_vectComboChildren[childIndex])->GetKeyName(),childIndex,this,bIsDefaultValue,defaultValue,keyValueIndex);
		}

		// call to correct child in path
		m_vectComboChildren[childIndex]->GetKeyValueContexts(pAttribBase,path,keyValueContext,addKeys);
	}
	else // all children are active paths
	{
		int nNumChildren = m_vectComboChildren.GetSize();
		for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
		{
			// do all paths 
			m_vectComboChildren[nIndex]->GetKeyValueContexts(pAttribBase,path,keyValueContext,addKeys);
		}
	}
}

//-----------------------------------------------------------------------------
// ConfigComboBox::ClearCachedData
// This method will call the entire tree an attempt to clear any cached data
//
// Input: none
//
// Output: none
//
//-----------------------------------------------------------------------------
void ConfigComboBox::ClearCachedData(void)
{
	int nNumChildren = m_vectComboChildren.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		// do all paths
		m_vectComboChildren[nIndex]->ClearCachedData();
	}
}

//-----------------------------------------------------------------------------
// ConfigComboBox::GetCRC
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
unsigned long ConfigComboBox::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	CRCGenerator.AnalyzeData(m_strKeyName);
	CRCGenerator.AnalyzeData(m_strLabelName);
	for (int childIndex = 0; childIndex < m_vectComboChildren.GetSize(); childIndex++)
	{
		ConfigSelection *pChild = m_vectComboChildren[childIndex];
		CRCGenerator.AnalyzeData(pChild->GetCRC());
	}
	for (int purgeIndex = 0; purgeIndex < m_vectKeysToPurge.GetSize(); purgeIndex++)
	{
		CRCGenerator.AnalyzeData(m_vectKeysToPurge[purgeIndex]);
	}
	CRCGenerator.AnalyzeData(m_nDefaultIndex);
	return CRCGenerator.GetResult();
}