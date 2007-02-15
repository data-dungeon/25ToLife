//-----------------------------------------------------------------------------
//	File:		ConfigNumeric.cpp
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigNumeric class represents the NUMERIC configuration
//			data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ConfigNumeric.h"
#include "UIDisplayNumeric.h"
#include "UIController.h"
#include "CurrAttribBase.h"
#include "KeySelection.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

static int gCurrNumericId = RESOURCE_START_NUMERIC;

//-----------------------------------------------------------------------------
// Class Definition:

//-----------------------------------------------------------------------------
// ConfigNumeric::ConfigNumeric
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigNumeric::ConfigNumeric()
{
	m_nTickStep = 10;
	m_fSpinnerStep = 0.1f;
	m_nMin = 0;
	m_fMin = 0.0;
	m_nMax = 0;
	m_fMax = 0.0;
	m_nDefault = 0;
	m_fDefault = 0.0;
	m_strFormat = ".3f";
	m_bHasEdit = true;
	m_bHasSpinner = true;
	m_bHasSlider = false;
	m_type = INT;
}


//-----------------------------------------------------------------------------
// ConfigNumeric::~ConfigNumeric
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ConfigNumeric::~ConfigNumeric()
{
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// ConfigNumeric::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.  It will also tell any of its children to print their
// contents.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void ConfigNumeric::PrintDebug(int nIndentLevel) const
{
	TupString strTabs = "";
	for (int nCurrLevel = 0; nCurrLevel < nIndentLevel; nCurrLevel++)
	{
		strTabs+="\t";
	}

	// Print key and name:
	ATDebugPrint(strTabs);
	ATDebugPrint("NUMERIC ");
	ATDebugPrint(" Key %s Label %s",(const char *)m_strKeyName,(const char *)m_strLabelName);
	ATDebugPrint(" = {\n");

	// Print type:
	ATDebugPrint(strTabs);
	switch (m_type)
	{
		case INT:
			ATDebugPrint("\tTYPE = int\n");
			break;
		case FLOAT:
			ATDebugPrint("\tTYPE = float\n");
			break;
	}

	// Print min:
	ATDebugPrint(strTabs);
	if (m_type == INT)
		ATDebugPrint("\tMIN = %d\n", m_nMin);
	else
		ATDebugPrint("\tMIN = %8.5f\n", m_fMin);

	// Print max:
	ATDebugPrint(strTabs);
	if (m_type == INT)
		ATDebugPrint("\tMAX = %d\n", m_nMax);
	else
		ATDebugPrint("\tMAX = %8.5f\n", m_fMax);

	// Print Default:
	ATDebugPrint(strTabs);
	if (m_type == INT)
		ATDebugPrint("\tDEFAULT = %d\n", m_nDefault);
	else
		ATDebugPrint("\tDEFAULT = %8.5f\n", m_fDefault);

	// Print format:
	ATDebugPrint(strTabs);
	ATDebugPrint("\tFORMAT = %s\n", m_strFormat);

	// Print tick_step:
	ATDebugPrint(strTabs);
	ATDebugPrint("\tTICK_STEP = %d\n", m_nTickStep);

	// Print has_edit:
	ATDebugPrint(strTabs);
	if (m_bHasEdit == true)
		ATDebugPrint("\tHAS_EDIT = 1\n");
	else
		ATDebugPrint("\tHAS_EDIT = 0\n");

	// Print has_spinner:
	ATDebugPrint(strTabs);
	if (m_bHasSpinner == true)
		ATDebugPrint("\tHAS_SPINNER = 1\n");
	else
		ATDebugPrint("\tHAS_SPINNER = 0\n");

	// Print has_slider:
	ATDebugPrint(strTabs);
	if (m_bHasSlider == true)
		ATDebugPrint("\tHAS_SLIDER = 1\n");
	else
		ATDebugPrint("\tHAS_SLIDER = 0\n");
	ATDebugPrint("%s}\n", strTabs);
}
#endif


//-----------------------------------------------------------------------------
// ConfigNumeric::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this numeric.
//
// Input:
//  TupperwareAggregate* pNumericTupAgg -- A pointer to the numeric tupperware
//										   aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ConfigNumeric::BuildConfigTree(TupperwareAggregate* pNumericTupAgg,ConfigOverrides &overrides)
{
	bool bSuccess = true;

	if (pNumericTupAgg == NULL || pNumericTupAgg->GetLastChildAtom() == NULL)
	{
		// TODO - A numeric has to be an aggregate and it has to have
		// children, so throw up an error!
	}
	else
	{
		// Set the name:
		TupString strName = pNumericTupAgg->GetName();

		m_strKeyName = this->ReturnKeyNameSubstr(strName);
		m_strLabelName = this->ReturnLabelNameSubstr(strName);
		// check if there is a default override
		bool bDefaultOverride = false;
		int nOverrideDefault;
		float fOverrideDefault;
		int keyIndex = overrides.KeyExistsAt(m_strKeyName,ConfigOverrides::OVERRIDE_DEFAULT);
		if (keyIndex!=-1)
		{
			nOverrideDefault = overrides.GetValueAsInt(keyIndex);
			fOverrideDefault = overrides.GetValueAsFloat(keyIndex);
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
		for( TupperwareIterator tupIterator = pNumericTupAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
		{
			// Check to see what the current atom's key is:
			TupperwareAtom* pTupAtom = tupIterator.GetAsAtom();
			const char* pCharKey = pTupAtom->GetKey();

			if (strcmp(pCharKey, "TYPE") == 0)
			{
				// We found the type, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				const char* pCharType = pTupScalar->GetAsString();

				if (stricmp(pCharType, "int") == 0)
					m_type = INT;
				else if (stricmp(pCharType, "float") == 0)
					m_type = FLOAT;
				else
					;
					// Type not recognized, so throw up an error!
			}
			else if (strcmp(pCharKey, "MIN") == 0)
			{
				// We found the minimum, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				if (pTupScalar->GetDataType() == TupperwareAtom::DATA_INT)
					m_nMin = pTupScalar->GetAsInt();
				else if (pTupScalar->GetDataType() == TupperwareAtom::DATA_FLOAT)
					m_fMin = pTupScalar->GetAsFloat();
			}
			else if (strcmp(pCharKey, "MAX") == 0)
			{
				// We found the maximum, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				if (pTupScalar->GetDataType() == TupperwareAtom::DATA_INT)
					m_nMax = pTupScalar->GetAsInt();
				else if (pTupScalar->GetDataType() == TupperwareAtom::DATA_FLOAT)
					m_fMax = pTupScalar->GetAsFloat();
			}
			else if (strcmp(pCharKey, "DEFAULT") == 0)
			{
				// We found the default, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				if (pTupScalar->GetDataType() == TupperwareAtom::DATA_INT)
					m_nDefault = pTupScalar->GetAsInt();
				else if (pTupScalar->GetDataType() == TupperwareAtom::DATA_FLOAT)
					m_fDefault = pTupScalar->GetAsFloat();
			}
			else if (strcmp(pCharKey, "FORMAT") == 0)
			{
				// We found the format, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				m_strFormat = pTupScalar->GetAsString();
			}
			else if (strcmp(pCharKey, "TICK_STEP") == 0)
			{
				// We found the tickstep, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				if (pTupScalar->GetDataType() == TupperwareAtom::DATA_INT)
					m_nTickStep = pTupScalar->GetAsInt();
				else if (pTupScalar->GetDataType() == TupperwareAtom::DATA_FLOAT)
					m_nTickStep = (int)pTupScalar->GetAsFloat();
			}
			else if (strcmp(pCharKey, "SPINNER_STEP") == 0)
			{
				// We found the tickstep, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				if (pTupScalar->GetDataType() == TupperwareAtom::DATA_INT)
					m_fSpinnerStep = (float)pTupScalar->GetAsInt();
				else if (pTupScalar->GetDataType() == TupperwareAtom::DATA_FLOAT)
					m_fSpinnerStep = pTupScalar->GetAsFloat();
			}
			else if (strcmp(pCharKey, "HAS_EDIT") == 0)
			{
				// We found the hasedit, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				if (pTupScalar->GetAsInt() == 1)
				{
					m_bHasEdit = true;
				}
				else
				{
					m_bHasEdit = false;
				}
			}
			else if (strcmp(pCharKey, "HAS_SPINNER") == 0)
			{
				// We found the hasspinner, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				if (pTupScalar->GetAsInt() == 1)
				{
					m_bHasSpinner = true;
				}
				else
				{
					m_bHasSpinner = false;
				}
			}
			else if (strcmp(pCharKey, "HAS_SLIDER") == 0)
			{
				// We found the hasslider, so set it:
				TupperwareScalar* pTupScalar = tupIterator.GetAsScalar();
				if (pTupScalar->GetAsInt() == 1)
				{
					m_bHasSlider = true;
				}
				else
				{
					m_bHasSlider = false;
				}
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
				// TODO - Notify the user there is an error at numeric level of
				// config file.
			}
			// TODO - Do checking at each level to see if there is more than one of
			// something, or if something is missing (such as two MINs or no MAX).
		}
		// override the default if we found one
		if (bDefaultOverride)
		{
			if (m_type == INT)
			{
				m_nDefault = nOverrideDefault;
			}
			else
			{
				m_fDefault = fOverrideDefault;
			}
		}
	}

	return bSuccess;
}


//-----------------------------------------------------------------------------
// ConfigNumeric::BuildUIDisplay
// This method will build the UIDisplay data for this numeric.  The data will
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
bool ConfigNumeric::BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
								   ExclusiveController* pExclusiveController, int nIndentLevel)
{
	bool bResult = true;

	// Check the non ui flag to see if this control should even be shown:
	if (GetNonUI() == true)
		return bResult;

	UIDisplayNumeric* pDisplayNumeric = new UIDisplayNumeric();

	// Set the label and keyname:
	pDisplayNumeric->SetLabel(m_strLabelName);
	pDisplayNumeric->SetKeyname(m_strKeyName);

	// Set the indent level:
	pDisplayNumeric->SetIndentLevel(nIndentLevel);
	pDisplayNumeric->SetAddedIndent(GetAddedIndent());

	// Set the exclusive flag:
	pDisplayNumeric->SetExclusive(m_bExclusive);

	// Set the disabled flag:
	pDisplayNumeric->SetDisabled(m_bDisabled);

	pDisplayNumeric->SetToolTip(m_strLabelName,m_strKeyName,m_strComment);

	// Set the label id:
	if (m_strComment.GetLength()) // get id if there is a comment
	{
		pDisplayNumeric->SetLabelID(gCurrNumericId);
		gCurrNumericId++;
		if (gCurrNumericId == RESOURCE_END_NUMERIC)
			gCurrNumericId = RESOURCE_START_NUMERIC;
	}
	else
	{
		pDisplayNumeric->SetLabelID(IDC_ALL_LABELS);
	}

	// Set the control ids:
	if (m_bHasEdit == true)
	{
		pDisplayNumeric->SetEditID(gCurrNumericId);
		gCurrNumericId++;
		if (gCurrNumericId == RESOURCE_END_NUMERIC)
			gCurrNumericId = RESOURCE_START_NUMERIC;
	}
	else
	{
		pDisplayNumeric->SetEditID(-1);
	}
	if (m_bHasSpinner == true)
	{
		pDisplayNumeric->SetSpinnerID(gCurrNumericId);
		gCurrNumericId++;
		if (gCurrNumericId == RESOURCE_END_NUMERIC)
			gCurrNumericId = RESOURCE_START_NUMERIC;
	}
	else
	{
		pDisplayNumeric->SetSpinnerID(-1);
	}
	if (m_bHasSlider == true)
	{
		pDisplayNumeric->SetSliderID(gCurrNumericId);
		gCurrNumericId++;
		if (gCurrNumericId == RESOURCE_END_NUMERIC)
			gCurrNumericId = RESOURCE_START_NUMERIC;
	}
	else
	{
		pDisplayNumeric->SetSliderID(-1);
	}

	// Check the current attrib data to see what the display type should be:
	TupString strDefaultVal = "";
	if (m_type == INT)
	{
		char pChValue[50];
		strDefaultVal = itoa(m_nDefault, pChValue, 10);
	}
	else
	{
		char pChValue[50];
		strDefaultVal = gcvt(m_fDefault, 10, pChValue);
	}

	UniqueValues uniqueValues;
	DisplayType dispType = FindDisplayType(m_strKeyName, strDefaultVal, pAttribBase, uniqueValues);

	// Set the id for the popup:
	pDisplayNumeric->SetUniqueValues(uniqueValues);
	pDisplayNumeric->SetUniqueValuesID(gCurrNumericId);
	gCurrNumericId++;
	if (gCurrNumericId == RESOURCE_END_NUMERIC)
		gCurrNumericId = RESOURCE_START_NUMERIC;

	// Set the current value:
	if (dispType == INDETERMINATE)
	{
		pDisplayNumeric->SetShowIndeterminate(true);
		pDisplayNumeric->SetCurrValue(m_nDefault);
		pDisplayNumeric->SetCurrValue(m_fDefault);
	}
	else if (dispType == UNIQUE)
	{
		pDisplayNumeric->SetShowIndeterminate(false);
		if (m_type == INT)
		{
			int nVal = atoi(uniqueValues.m_uniqueValues[uniqueValues.m_uniqueIndex].m_strValue);
			pDisplayNumeric->SetCurrValue(nVal);
		}
		else
		{
			float fVal = (float)atof(uniqueValues.m_uniqueValues[uniqueValues.m_uniqueIndex].m_strValue);
			pDisplayNumeric->SetCurrValue(fVal);
		}
	}
	else
	{
		// This is the default case:
		pDisplayNumeric->SetShowIndeterminate(false);
		if (m_type == INT)
		{
			pDisplayNumeric->SetCurrValue(m_nDefault);
		}
		else
		{
			pDisplayNumeric->SetCurrValue(m_fDefault);
		}
	}

	// Set the type:
	pDisplayNumeric->SetType(m_type);

	// Set the format:
	pDisplayNumeric->SetFormat(m_strFormat);

	// Set the tickstep:
	pDisplayNumeric->SetTickStep(m_nTickStep);

	// Set the spinnerStep:
	pDisplayNumeric->SetSpinnerStep(m_fSpinnerStep);

	// Set the min:
	if (m_type == INT)
		pDisplayNumeric->SetMin(m_nMin);
	else
		pDisplayNumeric->SetMin(m_fMin);

	// Set the max:
	if (m_type == INT)
		pDisplayNumeric->SetMax(m_nMax);
	else
		pDisplayNumeric->SetMax(m_fMax);

	// Set the default:
	if (m_type == INT)
	{
		pDisplayNumeric->SetDefault(m_nDefault);
	}
	else
	{
		pDisplayNumeric->SetDefault(m_fDefault);
	}

	// Set whether or not there is a spinner:
	pDisplayNumeric->SetHasSpinner(m_bHasSpinner);

	// Set whether or not there is a slider:
	pDisplayNumeric->SetHasSlider(m_bHasSlider);

	// Set whether or not there is an edit:
	pDisplayNumeric->SetHasEdit(m_bHasEdit);

	// Add the UIDisplayNumeric to the UIController:
	pUIController->AddDisplayObject(pDisplayNumeric);

	return bResult;
}

//-----------------------------------------------------------------------------
// ConfigNumeric::GetKeyValueContexts
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
void ConfigNumeric::GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys)
{
	TupString defaultValue;

	int keyValueIndex = -1;
	bool bIsDefaultValue = true;
	if (m_type == INT)
	{
		char pChValue[50];
		itoa(m_nDefault, pChValue, 10);
		defaultValue = pChValue;
	}
	else
	{
		char pChValue[50];
		defaultValue = gcvt(m_fDefault, 10, pChValue);
	}
	TupString value = defaultValue;

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
// ConfigNumeric::GetCRC
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
unsigned long ConfigNumeric::ComputeCRC(void)
{
	CRC CRCGenerator;
	CRCGenerator.AnalyzeData(ConfigBase::ComputeCRC());
	CRCGenerator.AnalyzeData(m_strKeyName);
	CRCGenerator.AnalyzeData(m_strLabelName);
	CRCGenerator.AnalyzeData((int)m_type);
	CRCGenerator.AnalyzeData(m_nMin);
	CRCGenerator.AnalyzeData(m_fMin);
	CRCGenerator.AnalyzeData(m_nMax);
	CRCGenerator.AnalyzeData(m_fMax);
	CRCGenerator.AnalyzeData(m_nDefault);
	CRCGenerator.AnalyzeData(m_fDefault);
	CRCGenerator.AnalyzeData(m_nTickStep);
	CRCGenerator.AnalyzeData(m_fSpinnerStep);
	CRCGenerator.AnalyzeData(m_strFormat);
	CRCGenerator.AnalyzeData(m_bHasEdit);
	CRCGenerator.AnalyzeData(m_bHasSpinner);
	CRCGenerator.AnalyzeData(m_bHasSlider);
	return CRCGenerator.GetResult();
}
