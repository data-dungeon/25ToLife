//-----------------------------------------------------------------------------
//	File:		ConfigEditBox.cpp
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigEditBox class represents the EDIT_BOX configuration
//			data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigEditBox.h"
#include "UIController.h"
#include "CurrAttribBase.h"
#include "UIDisplayEdit.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

static int gCurrEditBoxId = RESOURCE_START_EDITBOX;

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ConfigEditBox::ConfigEditBox
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigEditBox::ConfigEditBox()
{
	m_strDefaultText = "";
	m_type = STRING;
}


//-----------------------------------------------------------------------------
// ConfigEditBox::~ConfigEditBox
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigEditBox::~ConfigEditBox()
{
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigEditBox::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.  It will also tell any of its children to print their
// contents.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigEditBox::PrintDebug(int nIndentLevel) const
{
	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs+="\t";
	}

	ATDebugPrint(strTabs);
	ATDebugPrint("EDIT_BOX ");
	ATDebugPrint(" Key %s Label %s",(const char *)m_strKeyName,(const char *)m_strLabelName);
	ATDebugPrint(" = {\n");
	ATDebugPrint(strTabs);
	ATDebugPrint("\t");
	ATDebugPrint("DEFAULT_TEXT = %s\n", m_strDefaultText);
	ATDebugPrint(strTabs);
	ATDebugPrint("\t");
	switch (m_type)
	{
		case STRING:
			ATDebugPrint("TYPE = String\n");
			break;
		case FILENAME:
			ATDebugPrint("TYPE = Filename\n");
			break;
		case DIRECTORY:
			ATDebugPrint("TYPE = Directory\n");
			break;
		case OBJECTNAME:
			ATDebugPrint("TYPE = Objectname\n");
			break;
	}
	ATDebugPrint("%s}\n", strTabs);
}
#endif


//-----------------------------------------------------------------------------
// ConfigEditBox::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this edit box.
//
// Input:
//  TupperwareAggregate *pEditBoxTupAgg -- A pointer to the editbox tupperware
//										   aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigEditBox::BuildConfigTree(TupperwareAggregate *pEditBoxTupAgg,ConfigOverrides &overrides)
{
	bool bSuccess = true;

	if (pEditBoxTupAgg == NULL || pEditBoxTupAgg->GetLastChildAtom() == NULL)
	{
		// TODO - An edit box has to be an aggregate and it has to have
		// children, so throw up an error!
	}
	else
	{
		// Set the name:
		TupString strName = pEditBoxTupAgg->GetName();

		m_strKeyName = this->ReturnKeyNameSubstr(strName);
		m_strLabelName = this->ReturnLabelNameSubstr(strName);
		// check if there is a default override
		bool bDefaultOverride = false;
		int keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_DEFAULT);
		if (keyIndex!=-1)
		{
			m_strDefaultText = overrides.GetValueAsString(keyIndex);
			bDefaultOverride = true;
		}
		// change the label if overridden
		keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_LABEL);
		if (keyIndex!=-1)
		{
			m_strLabelName += overrides.GetValueAsString(keyIndex);
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

		// Cycle through all the children of the editbox, building tree as we go:
		for( TupperwareIterator tupIterator = pEditBoxTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
		{
			// Check to see what the current atom's key is:
			TupperwareAtom* pTupAtom = tupIterator.GetAsAtom();
			const char* pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "DEFAULT_TEXT") == 0)
			{
				// We found the default text, so set it:
				if (!bDefaultOverride) // make sure there is not an override already
				{
					TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
					m_strDefaultText = pTupScalar->GetAsString();

					// TODO - If there is more than one DEFAULT_TEXT, notify the
					// user there is an error.
				}
			}
			else if (strcmp(pCharKey, "TYPE") == 0)
			{
				// We found the type, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				const char* pCharType = pTupScalar->GetAsString();

				if (strcmp(pCharType, "string") == 0)
					m_type = STRING;
				else if (strcmp(pCharType, "filename") == 0)
					m_type = FILENAME;
				else if (strcmp(pCharType, "directory") == 0)
					m_type = DIRECTORY;
				else if (strcmp(pCharType, "objectname") == 0)
					m_type = OBJECTNAME;
				else
					;
					// Type not recognized, so throw up an error!
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
			else if (strcmp(pCharKey, "ADDED_INDENT") == 0)
			{
				// We found the ADDED_INDENT flag, so set it:
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
			else
			{
				// TODO - Notify the user there is an error at combobox level of
				// config file.
			}
			// TODO - Notify the user if there are no children of the combobox.
		}
	}

	return bSuccess;
}


//-----------------------------------------------------------------------------
// ConfigEditBox::BuildUIDisplay
// This method will build the UIDisplay data for this editbox.  The data will
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
bool ConfigEditBox::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
								   ExclusiveController* pExclusiveController, int nIndentLevel)
{
	bool bResult = true;

	// Check the non ui flag to see if this control should even be shown:
	if (this->GetNonUI() == true)
		return bResult;

	UIDisplayEdit* pDisplayEdit = new UIDisplayEdit();

	// Set the label and keyname:
	pDisplayEdit->SetLabel(m_strLabelName);
	pDisplayEdit->SetKeyname(m_strKeyName);

	// Set the indent level:
	pDisplayEdit->SetIndentLevel(nIndentLevel);
	pDisplayEdit->SetAddedIndent(GetAddedIndent());

	// Set the exclusive flag:
	pDisplayEdit->SetExclusive(m_bExclusive);

	// Set the disabled flag:
	pDisplayEdit->SetDisabled(m_bDisabled);

	// set the toolTip
	pDisplayEdit->SetToolTip(m_strLabelName,m_strKeyName,m_strComment);

	// Set the label id:
	if (m_strComment.GetLength()) // get id if there is a comment
	{
		pDisplayEdit->SetLabelID(gCurrEditBoxId);
		gCurrEditBoxId++;
		if (gCurrEditBoxId == RESOURCE_END_EDITBOX)
			gCurrEditBoxId = RESOURCE_START_EDITBOX;
	}
	else
	{
		pDisplayEdit->SetLabelID(IDC_ALL_LABELS);
	}

	// Set the control id:
	pDisplayEdit->SetControlID(gCurrEditBoxId);
	gCurrEditBoxId++;
	if (gCurrEditBoxId == RESOURCE_END_EDITBOX)
		gCurrEditBoxId = RESOURCE_START_EDITBOX;

	// Check the current attrib data to see what the display type should be:
	UniqueValues uniqueValues;
	DisplayType dispType = FindDisplayType(m_strKeyName, m_strDefaultText, pAttribBase, uniqueValues);

	// Set the id for the popup:
	pDisplayEdit->SetUniqueValues(uniqueValues);
	pDisplayEdit->SetUniqueValuesID(gCurrEditBoxId);
	gCurrEditBoxId++;
	if (gCurrEditBoxId == RESOURCE_END_EDITBOX)
		gCurrEditBoxId = RESOURCE_START_EDITBOX;

	// Set the current text:
	if (dispType == INDETERMINATE)
	{
		TupString strCurrText = "";
		pDisplayEdit->SetCurrText(strCurrText);
	}
	else if (dispType == UNIQUE)
	{
		pDisplayEdit->SetCurrText(uniqueValues.m_uniqueValues[uniqueValues.m_uniqueIndex].m_strValue);
	}
	else
	{
		// This is the default case:
		pDisplayEdit->SetCurrText(m_strDefaultText);
	}

	// Set the type:
	pDisplayEdit->SetType(m_type);

	// Add the UIDisplayEdit to the UIController:
	pUIController->AddDisplayObject(pDisplayEdit);

	return bResult;
}

//-----------------------------------------------------------------------------
// ConfigEditBox::GetKeyValueContexts
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
void ConfigEditBox::GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys)
{
	TupString value = m_strDefaultText; // start with default text
	bool bIsDefaultValue = true;
	int keyValueIndex = -1;

	if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
	{
		// if we find a match then replace the value with the one in the path
		keyValueIndex = path.KeyExistsAt(m_strKeyName);
		if (keyValueIndex!=-1)
		{
			TupString newValue = path.GetValueAsString(keyValueIndex);
			if (newValue != value)
			{
				value = newValue;
				bIsDefaultValue = false;
			}
		}
	}
	// add if addkeys or key already exists within context
	if (addKeys || keyValueContext.KeyExistsAt(m_strKeyName)!=-1)
	{
		keyValueContext.AddKeyValue(m_strKeyName,value,-1,this,bIsDefaultValue,m_strDefaultText,keyValueIndex);
	}
}

//-----------------------------------------------------------------------------
// ConfigEditBox::GetCRC
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
unsigned long ConfigEditBox::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	CRCGenerator.AnalyzeData(m_strKeyName);
	CRCGenerator.AnalyzeData(m_strLabelName);
	CRCGenerator.AnalyzeData(m_strDefaultText);
	CRCGenerator.AnalyzeData((int)m_type);
	return CRCGenerator.GetResult();
}

