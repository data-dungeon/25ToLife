//-----------------------------------------------------------------------------
//	File:		ConfigSetColorBox.h
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

/*
 * $History: ConfigSetColorBox.h $
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

#ifndef TUPPERATTRIB_CONFIG_SET_COLOR_BOX_H
#define TUPPERATTRIB_CONFIG_SET_COLOR_BOX_H


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

class TupConfigSetColorBox : public TupConfigBase
{
	TupString	m_strName;
	TupAttribColor	m_defaultColor;
	TupArray<TupAttribColor> m_vectColors;

public:
	TupConfigSetColorBox();
	~TupConfigSetColorBox();

	bool BuildConfigTree(TupperwareAggregate *pSetColorTupAgg,ConfigOverrides &overrides);
	TupConfigBase::ControlType GetControlType(void) const { return TupConfigBase::SET_COLOR_BOX; }
	void GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const;
	// Get Methods
	TupString GetName(void) const { return m_strName; }
	TupAttribColor GetDefaultColor(void) const { return m_defaultColor; }
	int GetNumColors(void) const { return m_vectColors.GetSize(); }
	TupAttribColor GetColor(int index) const { return m_vectColors[index]; }
};

//-----------------------------------------------------------------------------


#endif
