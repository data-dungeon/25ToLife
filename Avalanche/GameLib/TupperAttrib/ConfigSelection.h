//-----------------------------------------------------------------------------
//	File:		ConfigSelection.h
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
 * $History: ConfigSelection.h $
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 9:29a
 * Updated in $/Avalanche/tools/TupperAttrib
 * changed name of array and string files
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

#ifndef TUPPERATTRIB_CONFIG_SELECTION_H
#define TUPPERATTRIB_CONFIG_SELECTION_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "ConfigBase.h"
#endif

#ifndef TUPPERWARE_STRING_H
#include "Tupperware/tupstring.h"
#endif

#ifndef TUPPERWARE_ARRAY_H
#include "Tupperware/tuparray.h"
#endif

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;

//-----------------------------------------------------------------------------
// Class Declaration:

class TupConfigSelection : public TupConfigBase
{
	TupString m_strName;
	TupArray<TupConfigBase*> m_vectSelectionChildren;

public:
	TupConfigSelection();
	~TupConfigSelection();

	bool BuildConfigTree(TupperwareAggregate* pSelectionTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides,bool selection=true);
	TupConfigBase::ControlType GetControlType(void) const { return TupConfigBase::SELECTION; }
	bool GetKeySelections(const TupKeyValue &path,const char *pKeyName,TupKeySelection &selections) const;
	void GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const;
	// Get Methods
	TupString GetName(void) const { return m_strName; }
	int GetNumChildren(void) const { return m_vectSelectionChildren.GetSize(); }
	const TupConfigBase *GetChild(int index) const { return m_vectSelectionChildren[index]; }
};

//-----------------------------------------------------------------------------


#endif
