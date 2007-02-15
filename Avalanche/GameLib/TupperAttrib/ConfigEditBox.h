//-----------------------------------------------------------------------------
//	File:		ConfigEditBox.h
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

/*
 * $History: ConfigEditBox.h $
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

#ifndef TUPPERATTRIB_CONFIG_EDIT_BOX_H
#define TUPPERATTRIB_CONFIG_EDIT_BOX_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "ConfigBase.h"
#endif

//-----------------------------------------------------------------------------
// Defines, globals, etc.:

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;


//-----------------------------------------------------------------------------
// Class Declaration:

class TupConfigEditBox : public TupConfigBase
{
public:
	enum EditBoxType
	{
		STRING,
		FILENAME,
		DIRECTORY,
		OBJECTNAME
	};
private:
	TupString	m_strName;
	TupString	m_strDefaultText;
	EditBoxType m_type;

public:
	TupConfigEditBox();
	~TupConfigEditBox();

	bool BuildConfigTree(TupperwareAggregate *pEditBoxTupAgg,ConfigOverrides &overrides);
	TupConfigBase::ControlType GetControlType(void) const { return TupConfigBase::EDIT_BOX; }
	void GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const;
	// Get Methods
	TupString GetName(void) const { return m_strName; }
	EditBoxType GetType(void) const { return m_type; }
	TupString GetDefaultText(void) const { return m_strDefaultText; }
};

//-----------------------------------------------------------------------------


#endif
