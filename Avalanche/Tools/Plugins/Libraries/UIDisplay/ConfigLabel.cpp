//-----------------------------------------------------------------------------
//	File:		ConfigLabel.cpp
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigLabel class represents the EDIT_BOX configuration
//			data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigLabel.h"
#include "UIController.h"
#include "CurrAttribBase.h"
#include "UIDisplayLabel.h"

static int gCurrLabelId = RESOURCE_START_LABEL;

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ConfigLabel::ConfigLabel
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigLabel::ConfigLabel()
{
}


//-----------------------------------------------------------------------------
// ConfigLabel::~ConfigLabel
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigLabel::~ConfigLabel()
{
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigLabel::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.  It will also tell any of its children to print their
// contents.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigLabel::PrintDebug(int nIndentLevel) const
{
	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs+="\t";
	}

	ATDebugPrint(strTabs);
	ATDebugPrint("LABEL ");
	ATDebugPrint(" Label %s",(const char *)m_strLabelName);
	ATDebugPrint(" = {\n");
	ATDebugPrint("%s}\n", strTabs);
}
#endif


//-----------------------------------------------------------------------------
// ConfigLabel::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this edit box.
//
// Input:
//  TupperwareAggregate *pLabelTupAgg -- A pointer to the editbox tupperware
//										   aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigLabel::BuildConfigTree(TupperwareAggregate *pLabelTupAgg,ConfigOverrides &overrides)
{
	bool bSuccess = true;

	if (pLabelTupAgg == NULL)
	{
		// TODO - A label has to be an aggregate
	}
	else
	{
		// Set the name:
		m_strLabelName = pLabelTupAgg->GetName();

		m_strLabelName.Replace("|","\n"); // turn | into newlines

		// Cycle through all the children of the editbox, building tree as we go:
		for( TupperwareIterator tupIterator = pLabelTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
		{
			// Check to see what the current atom's key is:
			TupperwareAtom* pTupAtom = tupIterator.GetAsAtom();
			const char* pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "NONUI") == 0)
			{
				// set the non ui flag if set to 1
				if (tupIterator.GetAsScalar()->GetAsInt()==1)
					m_bNonUI = true;
			}
			else if (strcmp(pCharKey, "DISABLED") == 0)
			{
				// set the non ui flag if set to 1
				if (tupIterator.GetAsScalar()->GetAsInt()==1)
					m_bDisabled = true;
			}
			else if (strcmp(pCharKey, "ADDED_INDENT") == 0)
			{
				// We found the ADDED_INDENT flag, so set it:
				this->SetAddedIndent(tupIterator.GetAsScalar()->GetAsInt());
			}
			else if (strcmp(pCharKey, "COMMENT") == 0)
			{
				// We found the COMMENT flag, so set it:
				SetComment(tupIterator.GetAsScalar()->GetAsString());
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
// ConfigLabel::BuildUIDisplay
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
bool ConfigLabel::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
											  ExclusiveController* pExclusiveController, int nIndentLevel)
{
	bool bResult = true;

	// Check the non ui flag to see if this control should even be shown:
	if (this->GetNonUI() == true)
		return bResult;

	UIDisplayLabel* pDisplayLabel = new UIDisplayLabel();

	// Set the label and keyname:
	pDisplayLabel->SetLabel(m_strLabelName);
	pDisplayLabel->SetKeyname(m_strLabelName);

	// Set the indent level:
	pDisplayLabel->SetIndentLevel(nIndentLevel);
	pDisplayLabel->SetAddedIndent(GetAddedIndent());

	// Set the exclusive flag:
	pDisplayLabel->SetExclusive(m_bExclusive);

	// Set the disabled flag:
	pDisplayLabel->SetDisabled(m_bDisabled);

	// set the toolTip
	pDisplayLabel->SetLabelToolTip(m_strComment);

	int numLines = 1;
	int nStart = 0;
	while (1)
	{
		int nIndex = m_strLabelName.Find('\n',nStart);
		if (nIndex!=-1)
		{
			pDisplayLabel->AddLabelString(m_strLabelName.Mid(nStart,nIndex-nStart));
			if (m_strComment.GetLength()) // get id if there is a comment
			{
				pDisplayLabel->AddLabelID(gCurrLabelId);
				gCurrLabelId++;
				if (gCurrLabelId == RESOURCE_END_LABEL)
					gCurrLabelId = RESOURCE_END_LABEL;
			}
			else
			{
				pDisplayLabel->AddLabelID(IDC_ALL_LABELS);
			}

			nStart = nIndex+1;
		}
		else
		{
			pDisplayLabel->AddLabelString(m_strLabelName.Mid(nStart));
			if (m_strComment.GetLength()) // get id if there is a comment
			{
				pDisplayLabel->AddLabelID(gCurrLabelId);
				gCurrLabelId++;
				if (gCurrLabelId == RESOURCE_END_LABEL)
					gCurrLabelId = RESOURCE_END_LABEL;
			}
			else
			{
				pDisplayLabel->AddLabelID(IDC_ALL_LABELS);
			}
			break; 
		}
	}

	// Add the UIDisplayLabel to the UIController:
	pUIController->AddDisplayObject(pDisplayLabel);

	return bResult;
}

//-----------------------------------------------------------------------------
// ConfigLabel::GetCRC
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
unsigned long ConfigLabel::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	CRCGenerator.AnalyzeData(m_strLabelName);
	return CRCGenerator.GetResult();
}
