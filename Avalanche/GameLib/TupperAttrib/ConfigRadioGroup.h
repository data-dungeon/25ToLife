//-----------------------------------------------------------------------------
//	File:		TupConfigRadioGroup.h
//	Created:	Nov. 16, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TupConfigRadioGroup class represents the RADIO_GROUP configuration
//			data.
//
//	History:	11/16/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: ConfigRadioGroup.h $
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 9:29a
 * Updated in $/Avalanche/tools/TupperAttrib
 * changed name of array and string files
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

#ifndef TUPPERATTRIB_CONFIG_RADIO_GROUP_H
#define TUPPERATTRIB_CONFIG_RADIO_GROUP_H



//-----------------------------------------------------------------------------
// Includes:

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "ConfigBase.h"
#endif

#ifndef TUPPERWARE_ARRAY_H
#include "Tupperware/tuparray.h"
#endif

//-----------------------------------------------------------------------------
// Defines, globals, etc.:

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;
class TupConfigSelection;


//-----------------------------------------------------------------------------
// Class Declaration:

class TupConfigRadioGroup : public TupConfigBase
{
	TupString	m_strName;
	int	m_nDefaultIndex;
	TupArray<TupConfigSelection*> m_vectRadioChildren;

public:
	TupConfigRadioGroup();
	~TupConfigRadioGroup();

	bool BuildConfigTree(TupperwareAggregate *pRadioTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides);
	TupConfigBase::ControlType GetControlType(void) const { return TupConfigBase::RADIO_GROUP; }
	bool GetKeySelections(const TupKeyValue &path,const char *pKeyName,TupKeySelection &selections) const;
	void GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const;
	// Get Methods
	TupString GetName(void) const { return m_strName; }
	int GetNumSelectionChildren(void) const { return m_vectRadioChildren.GetSize(); }
	const TupConfigSelection *GetSelectionChild(int index) const { return m_vectRadioChildren[index]; }
	int GetDefaultChild(void) const { return m_nDefaultIndex; }
};

//-----------------------------------------------------------------------------


#endif
