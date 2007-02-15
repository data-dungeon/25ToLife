//-----------------------------------------------------------------------------
//	File:		ConfigSetColorBox.cpp
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigSetColorBox class represents the SET_COLOR_BOX
//			configuration data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigSetColorBox.h"
#include "UIDisplaySetColor.h"
#include "UIController.h"
#include "CurrAttribBase.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

static int gCurrSetColorBoxId = RESOURCE_START_SET_COLOR_BOX;

//-----------------------------------------------------------------------------
// Class Definition:

//-----------------------------------------------------------------------------
// ConfigSetColorBox::ConfigSetColorBox
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigSetColorBox::ConfigSetColorBox()
{
	m_defaultColor.m_nRed = 0;
	m_defaultColor.m_nGreen = 0;
	m_defaultColor.m_nBlue = 0;
	m_defaultColor.m_strName = "";
}


//-----------------------------------------------------------------------------
// ConfigSetColorBox::~ConfigSetColorBox
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigSetColorBox::~ConfigSetColorBox()
{
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigSetColorBox::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.  It will also tell any of its children to print their
// contents.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigSetColorBox::PrintDebug(int nIndentLevel) const
{
	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs += "\t";
	}

	// Print key and name:
	ATDebugPrint(strTabs);
	ATDebugPrint("SET_COLOR_BOX ");
	ATDebugPrint(" Key %s Label %s",(const char *)m_strKeyName,(const char *)m_strLabelName);
	ATDebugPrint(" = {\n");

	// Print default color:
	ATDebugPrint(strTabs);
	ATDebugPrint("\tDEFAULT_COLOR %s = [%d,%d,%d]\n",m_defaultColor.m_strName,
												 m_defaultColor.m_nRed,
												 m_defaultColor.m_nGreen,
												 m_defaultColor.m_nBlue);
	// Print the rest of the colors:
	int nNumChildren = m_vectColors.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		ATDebugPrint(strTabs);
		ATDebugPrint("\tCOLOR %s = [%d,%d,%d]\n", m_vectColors[nIndex].m_strName,
													 m_vectColors[nIndex].m_nRed,
													 m_vectColors[nIndex].m_nGreen,
													 m_vectColors[nIndex].m_nBlue);		
	}
	ATDebugPrint("%s}\n", strTabs);
}
#endif


//-----------------------------------------------------------------------------
// ConfigSetColorBox::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this setcolorbox.
//
// Input:
//  TupperwareAggregate *pSetColorTupAgg -- A pointer to the setcolorbox
//											tupperware aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigSetColorBox::BuildConfigTree(TupperwareAggregate *pSetColorTupAgg,ConfigOverrides &overrides)
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
		TupString strName = pSetColorTupAgg->GetName();

		m_strKeyName = ReturnKeyNameSubstr(strName);
		m_strLabelName = ReturnLabelNameSubstr(strName);
		// check if there is a default override
		bool bDefaultOverride = false;
		int keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_DEFAULT);
		if (keyIndex!=-1)
		{
			m_defaultColor = overrides.GetValueAsColor(keyIndex);
			bDefaultOverride = true;
		}
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

		// Cycle through all the children of the setcolorbox, building tree as we go:
		for( TupperwareIterator tupIterator = pSetColorTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
		{
			// Check to see what the current atom's key is:
			TupperwareAtom* pTupAtom = tupIterator.GetAsAtom();
			const char* pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "DEFAULT_COLOR") == 0)
			{
				// make sure it wasn't overridden
				if (!bDefaultOverride)
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
					// TODO - If there is more than one DEFAULT_COLOR, notify the
					// user there is an error.
				}
			}
			else if (strcmp(pCharKey, "COLOR") == 0)
			{
				// We found a color, so set it:
				TupperwareList* pTupList = tupIterator.GetAsList();
				const int* intArray = pTupList->GetAsInt();
				// TODO - Do some error checking to make sure this is a list, and
				// that it is an integer list and that it has 3 elements.
				AttribColor newColor;
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
// ConfigSetColorBox::BuildUIDisplay
// This method will build the UIDisplay data for this setcolorbox.  The data
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
bool ConfigSetColorBox::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
									   ExclusiveController* pExclusiveController, int nIndentLevel)
{
	bool bResult = true;

	// Check the non ui flag to see if this control should even be shown:
	if (GetNonUI() == true)
		return bResult;

	UIDisplaySetColor* pDisplaySetColor = new UIDisplaySetColor();

	// Set the label and keyname:
	pDisplaySetColor->SetLabel(m_strLabelName);
	pDisplaySetColor->SetKeyname(m_strKeyName);

	// Set the indent level:
	pDisplaySetColor->SetIndentLevel(nIndentLevel);
	pDisplaySetColor->SetAddedIndent(GetAddedIndent());

	// Set the exlcusive flag:
	pDisplaySetColor->SetExclusive(m_bExclusive);

	// Set the disabled flag:
	pDisplaySetColor->SetDisabled(m_bDisabled);

	// Check the current attrib data to see what the display type should be:
	TupString strDefaultColor;
	strDefaultColor.Format("%d,%d,%d",m_defaultColor.m_nRed,m_defaultColor.m_nGreen,m_defaultColor.m_nBlue);

	UniqueValues uniqueValues;
	DisplayType dispType = FindDisplayType(m_strKeyName, strDefaultColor, pAttribBase, uniqueValues);

	// Create the list of colors:
	int nFoundIndex = -1;
	int nNumChildren = m_vectColors.GetSize();
	for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
	{
		TupString strColorName = m_vectColors[nIndex].m_strName;
		pDisplaySetColor->AddColor(m_vectColors[nIndex]);
		if (dispType == UNIQUE)
		{
			TupString colorName;
			if (uniqueValues.m_uniqueValues[uniqueValues.m_uniqueIndex].m_strValue == strColorName)
			{
				nFoundIndex = nIndex;
			}
		}
	}
	// Add the default color, since it isn't in the vector of colors - Fix this!
	pDisplaySetColor->AddColor(m_defaultColor);

	pDisplaySetColor->SetToolTip(m_strLabelName,m_strKeyName,m_strComment);

	// Set the label id:
	if (m_strComment.GetLength()) // get id if there is a comment
	{
		pDisplaySetColor->SetLabelID(gCurrSetColorBoxId);
		gCurrSetColorBoxId++;
		if (gCurrSetColorBoxId == RESOURCE_END_SET_COLOR_BOX)
			gCurrSetColorBoxId = RESOURCE_START_SET_COLOR_BOX;
	}
	else
	{
		pDisplaySetColor->SetLabelID(IDC_ALL_LABELS);
	}


	// Set the control ids:
	pDisplaySetColor->SetCurrColorID(gCurrSetColorBoxId);
	gCurrSetColorBoxId++;
	if (gCurrSetColorBoxId == RESOURCE_END_SET_COLOR_BOX)
		gCurrSetColorBoxId = RESOURCE_START_SET_COLOR_BOX;
	int nNumControlIDs = m_vectColors.GetSize() + 1;
	for (int nLoop = 0; nLoop < nNumControlIDs; nLoop++)
	{
		pDisplaySetColor->AddChoiceCtrlID(gCurrSetColorBoxId);
		gCurrSetColorBoxId++;
		if (gCurrSetColorBoxId == RESOURCE_END_SET_COLOR_BOX)
			gCurrSetColorBoxId = RESOURCE_START_SET_COLOR_BOX;
	}

	// Set the current selection index and if to show the indeterminate state:
	if (dispType == DEFAULT)
	{
		pDisplaySetColor->SetShowIndeterminate(false);
		pDisplaySetColor->SetSelectionIndex(nNumChildren);
	}
	else if (dispType == UNIQUE)
	{
		pDisplaySetColor->SetShowIndeterminate(false);
		if (nFoundIndex < 0)
		{
			pDisplaySetColor->SetSelectionIndex(nNumChildren);
		}
		else
		{
			pDisplaySetColor->SetSelectionIndex(nFoundIndex);
		}
	}
	else
	{
		// This is the indeterminate case:
		pDisplaySetColor->SetShowIndeterminate(true);
		pDisplaySetColor->SetSelectionIndex(-1);
	}

	// Add the UIDisplaySetColor to the UIController:
	pUIController->AddDisplayObject(pDisplaySetColor);

	return bResult;
}

//-----------------------------------------------------------------------------
// ConfigSetColorBox::GetKeyValueContexts
// This method will check to see if the name of this object matches the key and if so
// set up the selections object to have all the children selections within this object
// If this object or one of it's children finds a match it will return true
//
// Input:
//  const KeyValue &path -- a const reference to the keyvalue pairs that denote the path
//  KeyValueContext &keyValueContext -- holds the key value contexts to be set
//  bool addKeys -- if false we will only update existing keys and not add new ones within the context
//
//-----------------------------------------------------------------------------
void ConfigSetColorBox::GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys)
{
	TupString defaultValue = ColorToString(m_defaultColor); 
	TupString value = defaultValue; // start with default color

	int keyValueIndex = -1;
	bool bIsDefaultValue = true;

	if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
	{
		// if we find a match then replace the value with the one in the path
		keyValueIndex = path.KeyExistsAt(m_strKeyName);
		if (keyValueIndex!=-1)
		{
			TupString newValue = path.GetValueAsString(keyValueIndex);
			if (newValue != value)
			{
				bIsDefaultValue = false;
				value = newValue;
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
// ConfigSetColorBox::GetCRC
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
unsigned long ConfigSetColorBox::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	CRCGenerator.AnalyzeData(m_strKeyName);
	CRCGenerator.AnalyzeData(m_strLabelName);
	CRCGenerator.AnalyzeData(m_defaultColor.m_nRed);
	CRCGenerator.AnalyzeData(m_defaultColor.m_nGreen);
	CRCGenerator.AnalyzeData(m_defaultColor.m_nBlue);
	CRCGenerator.AnalyzeData(m_defaultColor.m_strName);
	for (int colorIndex = 0; colorIndex < m_vectColors.GetSize(); colorIndex++)
	{
		const AttribColor &color = m_vectColors[colorIndex];
		CRCGenerator.AnalyzeData(color.m_nRed);
		CRCGenerator.AnalyzeData(color.m_nGreen);
		CRCGenerator.AnalyzeData(color.m_nBlue);
		CRCGenerator.AnalyzeData(color.m_strName);
	}
	return CRCGenerator.GetResult();
}
