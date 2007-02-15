//-----------------------------------------------------------------------------
//	File:		TupConfigNumeric.cpp
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TupConfigNumeric class represents the NUMERIC configuration
//			data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: ConfigNumeric.cpp $
 * 
 * *****************  Version 27  *****************
 * User: Todd Blackburn Date: 4/19/03    Time: 11:58a
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 26  *****************
 * User: Benson       Date: 4/11/03    Time: 11:31a
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 6/27/02    Time: 2:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Override Keyname additions
 * 
 * *****************  Version 24  *****************
 * User: Todd Blackburn Date: 6/24/02    Time: 3:50p
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 5/09/02    Time: 3:37p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Working with CodeWarrior Compiler
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

//-----------------------------------------------------------------------------
// Includes:

#include "TupperAttribPch.h"
#include "ConfigNumeric.h"
#include "KeyValue.h"
#include "KeyValueContext.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// TupConfigNumeric::TupConfigNumeric
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigNumeric::TupConfigNumeric()
{
	m_nTickStep = 0;
	m_nMin = 0;
	m_fMin = 0.0;
	m_nMax = 0;
	m_fMax = 0.0;
	m_nDefault = 0;
	m_fDefault = 0.0;
	m_bHasEdit = true;
	m_bHasSpinner = true;
	m_bHasSlider = false;
	m_type = INT;
}


//-----------------------------------------------------------------------------
// TupConfigNumeric::~TupConfigNumeric
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
TupConfigNumeric::~TupConfigNumeric()
{
}



//-----------------------------------------------------------------------------
// TupConfigNumeric::BuildConfigTree
// This method will build the portion of the configuration tree dealing with
// this numeric.
//
// Input:
//  TupperwareAggregate* pNumericTupAgg -- A pointer to the numeric tupperware
//										   aggregate.
//
// Output: bool - True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool TupConfigNumeric::BuildConfigTree(TupperwareAggregate* pNumericTupAgg,ConfigOverrides &overrides)
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
		m_strName = pNumericTupAgg->GetName();

		// check if there is a default override
		TupString strKeyname = this->ReturnKeynameSubstr(m_strName);
		int nOverrideDefault = 0;
		float fOverrideDefault = 0.0f;
		bool bDefaultOverride = false;
		int keyIndex = overrides.KeyExistsAt(strKeyname,ConfigOverrides::OVERRIDE_DEFAULT);
		if (keyIndex!=-1)
		{
			nOverrideDefault = overrides.GetValueAsInt(keyIndex);
			fOverrideDefault = overrides.GetValueAsFloat(keyIndex);
			bDefaultOverride = true;
		}
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
				m_nTickStep = pTupScalar->GetAsInt();
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
// TupConfigNumeric::GetKeyValueContexts
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
void TupConfigNumeric::GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const
{
	// Peel off the keyname and the label:
	TupString strKeyname = ReturnKeynameSubstr(m_strName);
	TupString value;
	if (m_type == INT)
	{
		char pChValue[50];
#ifdef PS2
		sprintf(pChValue, "%d", m_nDefault);
#elif defined(GCN)
		sprintf(pChValue, "%d", m_nDefault);
#else
		itoa(m_nDefault, pChValue, 10);
#endif
		value = pChValue;
	}
	else
	{
		char pChValue[50];
#ifdef GCN
		sprintf( pChValue, "%f", m_fDefault);
		value = pChValue;
#else
		value = gcvt(m_fDefault, 10, pChValue);
#endif
	}

	if (!GetNonUI()) // make sure we are a ui control (if a nonUI we use the default and ignore the path)
	{
		// if we find a match then replace the value with the one in the path
		int keyIndex = path.KeyExistsAt(strKeyname);
		if (keyIndex!=-1)
		{
			value = path.GetValueAsString(keyIndex);
		}
	}
	// add if addkeys or key already exists within context
	if (addKeys || keyValueContext.KeyExistsAt(strKeyname)!=-1)
	{
		keyValueContext.AddKeyValue(strKeyname,value,-1,this);
	}
}
