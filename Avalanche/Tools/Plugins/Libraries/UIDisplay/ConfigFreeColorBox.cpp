//-----------------------------------------------------------------------------
//	File:		ConfigFreeColorBox.cpp
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigFreeColorBox class represents the FREE_COLOR_BOX
//			configuration data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigFreeColorBox.h"
#include "UIDisplayFreeColor.h"
#include "UIController.h"
#include "CurrAttribBase.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

static int gCurrFreeColorBoxId = RESOURCE_START_FREECOLORBOX;

//-----------------------------------------------------------------------------
// Class Definition:

//-----------------------------------------------------------------------------
// ConfigFreeColorBox::ConfigFreeColorBox
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigFreeColorBox::ConfigFreeColorBox()
{
	m_defaultColor.m_nRed = 0;
	m_defaultColor.m_nGreen = 0;
	m_defaultColor.m_nBlue = 0;
	m_defaultColor.m_strName = "";
}


//-----------------------------------------------------------------------------
// ConfigFreeColorBox::~ConfigFreeColorBox
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigFreeColorBox::~ConfigFreeColorBox()
{
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigFreeColorBox::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.  It will also tell any of its children to print their
// contents.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigFreeColorBox::PrintDebug(int nIndentLevel) const
{
	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs+="\t";
	}

	// Print key and name:
	ATDebugPrint(strTabs);
	ATDebugPrint("FREE_COLOR_BOX ");
	ATDebugPrint(" Key %s Label %s",(const char *)m_strKeyName,(const char *)m_strLabelName);
	ATDebugPrint(" = {\n");

	// Print default color:
	ATDebugPrint(strTabs);
	ATDebugPrint("\tDEFAULT_COLOR %s = [%d,%d,%d]\n",m_defaultColor.m_strName,
												 m_defaultColor.m_nRed,
												 m_defaultColor.m_nGreen,
												 m_defaultColor.m_nBlue);
	ATDebugPrint("%s}\n", strTabs);
}
#endif


//-----------------------------------------------------------------------------
// ConfigFreeColorBox::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this freecolorbox.
//
// Input:
//  TupperwareAggregate *pFreeColorTupAgg -- A pointer to the freecolorbox
//											 tupperware aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigFreeColorBox::BuildConfigTree(TupperwareAggregate *pFreeColorTupAgg,ConfigOverrides &overrides)
{
	bool bSuccess = true;

	if (pFreeColorTupAgg == NULL || pFreeColorTupAgg->GetLastChildAtom() == NULL)
	{
		// TODO - A free color box has to be an aggregate and it has to have
		// children, so throw up an error!
	}
	else
	{
		// Set the name:
		TupString strName = pFreeColorTupAgg->GetName();

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

		// Cycle through all the children of the freecolorbox, building tree as we go:
		for( TupperwareIterator tupIterator = pFreeColorTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
		{
			// Check to see what the current atom's key is:
			TupperwareAtom* pTupAtom = tupIterator.GetAsAtom();
			const char* pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "DEFAULT_COLOR") == 0)
			{
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
				}
				// TODO - If there is more than one DEFAULT_COLOR, notify the
				// user there is an error.
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
				// TODO - Notify the user there is an error at freecolorbox level of
				// config file.
			}
		}
	}

	return bSuccess;
}


//-----------------------------------------------------------------------------
// ConfigFreeColorBox::BuildUIDisplay
// This method will build the UIDisplay data for this freecolorbox.  The data
// will be stored in the UIController passed in and uses the current attrib
// data passed in.
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
bool ConfigFreeColorBox::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
										ExclusiveController* pExclusiveController, int nIndentLevel)
{
	bool bResult = true;

	// Check the non ui flag to see if this control should even be shown:
	if (this->GetNonUI() == true)
		return bResult;

	UIDisplayFreeColor* pDisplayFreeColor = new UIDisplayFreeColor();

	// Set the label and keyname:
	pDisplayFreeColor->SetLabel(m_strLabelName);
	pDisplayFreeColor->SetKeyname(m_strKeyName);

	// Set the indent level:
	pDisplayFreeColor->SetIndentLevel(nIndentLevel);
	pDisplayFreeColor->SetAddedIndent(GetAddedIndent());

	// Set the exclusive flag:
	pDisplayFreeColor->SetExclusive(m_bExclusive);

	// Set the disabled flag:
	pDisplayFreeColor->SetDisabled(m_bDisabled);

	pDisplayFreeColor->SetToolTip(m_strLabelName,m_strKeyName,m_strComment);

	// Set the label id:
	if (m_strComment.GetLength()) // get id if there is a comment
	{
		pDisplayFreeColor->SetLabelID(gCurrFreeColorBoxId);
		gCurrFreeColorBoxId++;
		if (gCurrFreeColorBoxId == RESOURCE_END_FREECOLORBOX)
			gCurrFreeColorBoxId = RESOURCE_START_FREECOLORBOX;
	}
	else
	{
		pDisplayFreeColor->SetLabelID(IDC_ALL_LABELS);
	}

	// Set the control id:
	pDisplayFreeColor->SetControlID(gCurrFreeColorBoxId);
	gCurrFreeColorBoxId++;
	if (gCurrFreeColorBoxId == RESOURCE_END_FREECOLORBOX)
		gCurrFreeColorBoxId = RESOURCE_START_FREECOLORBOX;

	// Check the current attrib data to see what the display type should be:

	TupString strDefaultColor;
	strDefaultColor.Format("%d,%d,%d",m_defaultColor.m_nRed,m_defaultColor.m_nGreen,m_defaultColor.m_nBlue);

	UniqueValues uniqueValues;
	DisplayType dispType = FindDisplayType(m_strKeyName, strDefaultColor, pAttribBase, uniqueValues);

	// Set the current color:
	if (dispType == INDETERMINATE)
	{
		AttribColor indeterColor;
		indeterColor.m_nRed = 0;
		indeterColor.m_nGreen = 0;
		indeterColor.m_nBlue = 0;
		indeterColor.m_strName = "";
		pDisplayFreeColor->SetCurrentColor(indeterColor);
	}
	else if (dispType == UNIQUE)
	{
		AttribColor uniqueColor = CurrAttribBase::StrToColor(uniqueValues.m_uniqueValues[uniqueValues.m_uniqueIndex].m_strValue);
		pDisplayFreeColor->SetCurrentColor(uniqueColor);
	}
	else
	{
		// This is the default case:
		pDisplayFreeColor->SetCurrentColor(m_defaultColor);
	}

	// Add the UIDisplayFreeColor to the UIController:
	pUIController->AddDisplayObject(pDisplayFreeColor);

	return bResult;
}

//-----------------------------------------------------------------------------
// ConfigFreeColorBox::GetKeyValueContexts
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
void ConfigFreeColorBox::GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys)
{
	TupString defaultValue = ColorToString(m_defaultColor);
	TupString value = defaultValue; // start with default color
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
		keyValueContext.AddKeyValue(m_strKeyName,value,-1,this,bIsDefaultValue,defaultValue,keyValueIndex);
	}
}

//-----------------------------------------------------------------------------
// ConfigFreeColorBox::GetCRC
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
unsigned long ConfigFreeColorBox::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	CRCGenerator.AnalyzeData(m_strKeyName);
	CRCGenerator.AnalyzeData(m_strLabelName);
	CRCGenerator.AnalyzeData(m_defaultColor.m_nRed);
	CRCGenerator.AnalyzeData(m_defaultColor.m_nGreen);
	CRCGenerator.AnalyzeData(m_defaultColor.m_nBlue);
	CRCGenerator.AnalyzeData(m_defaultColor.m_strName);
	return CRCGenerator.GetResult();
}
