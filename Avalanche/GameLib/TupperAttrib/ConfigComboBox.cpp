//-----------------------------------------------------------------------------
//	File:		TupConfigComboBox.cpp
//	Created:	Nov. 16, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TupConfigComboBox class represents the combo box configuration
//			data.
//
//	History:	11/16/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: ConfigComboBox.cpp $
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 7/14/03    Time: 1:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * override default fixes
 * 
 * *****************  Version 27  *****************
 * User: Adam Clayton Date: 6/27/02    Time: 2:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Override Keyname additions
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 1:01p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Opps on the compare function
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 2:33p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Exclusive command check in build config tree
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:26p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Bugs...
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

//-----------------------------------------------------------------------------
// Includes:

#include "TupperAttribPch.h"
#include "ConfigComboBox.h"
#include "ConfigSelection.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

//-----------------------------------------------------------------------------
// Defines, enums, etc.:

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// TupConfigComboBox::TupConfigComboBox
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigComboBox::TupConfigComboBox() :
	m_nDefaultIndex(0)
{
}


//-----------------------------------------------------------------------------
// TupConfigComboBox::~TupConfigComboBox
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigComboBox::~TupConfigComboBox()
{
	int numChildren = m_vectComboChildren.GetSize();
	for (int index = 0; index < numChildren; index++)
	{
		TupConfigSelection* pCurrChild = m_vectComboChildren[index];
		if (pCurrChild != NULL)
		{
			delete pCurrChild;
			pCurrChild = NULL;
		}
	}
}

//-----------------------------------------------------------------------------
// TupConfigComboBox::BuildConfigTree
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
bool TupConfigComboBox::BuildConfigTree(TupperwareAggregate *pComboBoxTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides)
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
		m_strName = pComboBoxTupAgg->GetName();

		// check if there is a default override
		TupString strKeyname = this->ReturnKeynameSubstr(m_strName);
		TupString strDefaultOverrideValue;
		bool bDefaultOverride = false;
		int keyIndex = overrides.KeyExistsAt(strKeyname,ConfigOverrides::OVERRIDE_DEFAULT);
		if (keyIndex!=-1)
		{
			strDefaultOverrideValue = overrides.GetValueAsString(keyIndex);
			bDefaultOverride = true;
		}
		int nOverrideDefaultIndex = -1;
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
				TupConfigSelection* pConfigSelection = new TupConfigSelection();
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
					TupString strChildKey(ReturnKeynameSubstr(strChildName));
					if (!strcmp(strDefaultOverrideValue,strChildKey))
						nOverrideDefaultIndex = m_vectComboChildren.GetSize();
				}

				// We found a selection, so create one and add it to the children:
				TupConfigSelection* pConfigSelection = new TupConfigSelection();
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
			else if (strcmp(pCharKey, "EXCLUSIVE") == 0)
			{
				// clear the exclusive flag if set to 0
				if (tupIterator.GetAsScalar()->GetAsInt()==0)
					m_bExclusive = false;
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
// TupConfigComboBox::GetKeySelections
// This method will check to see if the name of this object matches the key and if so
// set up the selections object to have all the children selections within this object
// If this object or one of it's children finds a match it will return true
//
// Input:
//  const TupKeyValue &path -- a const reference to the keyvalue pairs that denote the path
//  const char *pKeyName -- a const char * that holds the keyname that is used for matching
//  TupKeySelection &selections -- the selections to be build
//
// Output: bool - True if found on object or child, false otherwise.
//-----------------------------------------------------------------------------
bool TupConfigComboBox::GetKeySelections(const TupKeyValue &path,const char *pKeyName,TupKeySelection &selections) const
{
	// Peel off the keyname and the label:
	TupString strKeyname = ReturnKeynameSubstr(m_strName);
	// if the key we are looking for is me
	if (!strKeyname.Compare(pKeyName))
	{
		selections.Clear();
		int nNumChildren = m_vectComboChildren.GetSize();
		for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
		{
			// add selection children
			selections.AddSelection(ReturnKeynameSubstr(m_vectComboChildren[nIndex]->GetName()),nIndex==m_nDefaultIndex);
		}
		return true;
	}
	if (GetExclusive()) // only one active path
	{
		int childIndex = m_nDefaultIndex; // start with default
		if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
		{
			// if we are not a match then continue down the path
			int keyIndex = path.KeyExistsAt(strKeyname);
			if (keyIndex!=-1)
			{
				int nNumChildren = m_vectComboChildren.GetSize();
				for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
				{
					TupString strChildName = ReturnKeynameSubstr(m_vectComboChildren[nIndex]->GetName());
					if (!strChildName.Compare(path.GetValueAsString(keyIndex)))
					{
						childIndex = nIndex; // update child to this one
						break;
					}
				}
			}
		}
		// call to correct child in path
		return m_vectComboChildren[childIndex]->GetKeySelections(path,pKeyName,selections);
	}
	else // if all children are active paths
	{
		int nNumChildren = m_vectComboChildren.GetSize();
		for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
		{
			// do all paths (until we hit one that worked)
			if (m_vectComboChildren[nIndex]->GetKeySelections(path,pKeyName,selections))
				return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// TupConfigComboBox::GetKeyValueContexts
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
void TupConfigComboBox::GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const
{
	// Peel off the keyname and the label:
	TupString strKeyname = ReturnKeynameSubstr(m_strName);
	if (GetExclusive()) // only one active path
	{
		int childIndex = m_nDefaultIndex; // start with default
		if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
		{
			// if we are not a match then continue down the path
			int keyIndex = path.KeyExistsAt(strKeyname);
			if (keyIndex!=-1)
			{
				int nNumChildren = m_vectComboChildren.GetSize();
				for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
				{
					TupString strChildName = ReturnKeynameSubstr(m_vectComboChildren[nIndex]->GetName());
					if (!strChildName.Compare(path.GetValueAsString(keyIndex)))
					{
						childIndex = nIndex; // update child to this one
						break;
					}
				}
			}
		}
		// add if addkeys or key already exists within context
		if (addKeys || keyValueContext.KeyExistsAt(strKeyname)!=-1)
		{
			keyValueContext.AddKeyValue(strKeyname,ReturnKeynameSubstr(m_vectComboChildren[childIndex]->GetName()),childIndex,this);
		}

		// call to correct child in path
		m_vectComboChildren[childIndex]->GetKeyValueContexts(path,keyValueContext,addKeys);
	}
	else // all children are active paths
	{
		int nNumChildren = m_vectComboChildren.GetSize();
		for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
		{
			// do all paths 
			m_vectComboChildren[nIndex]->GetKeyValueContexts(path,keyValueContext,addKeys);
		}
	}
}
