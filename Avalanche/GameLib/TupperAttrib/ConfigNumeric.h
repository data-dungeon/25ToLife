//-----------------------------------------------------------------------------
//	File:		ConfigNumeric.h
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

/*
 * $History: ConfigNumeric.h $
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

#ifndef TUPPERATTRIB_CONFIG_NUMERIC_H
#define TUPPERATTRIB_CONFIG_NUMERIC_H



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

class TupConfigNumeric : public TupConfigBase
{
public:
	enum NumericType
	{
		INT,
		FLOAT,
	};
private:
	TupString	m_strName;
	NumericType m_type;
	union
	{
		int m_nMin;
		float m_fMin;
	};
	union
	{
		int m_nMax;
		float m_fMax;
	};
	union
	{
		int m_nDefault;
		float m_fDefault;
	};
	int m_nTickStep;
	TupString m_strFormat;
	bool m_bHasEdit;
	bool m_bHasSpinner;
	bool m_bHasSlider;

public:
	TupConfigNumeric();
	~TupConfigNumeric();

	bool BuildConfigTree(TupperwareAggregate* pNumericTupAgg,ConfigOverrides &overrides);
	TupConfigBase::ControlType GetControlType(void) const { return TupConfigBase::NUMERIC; }
	void GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const;
	// Get Methods
	TupString GetName(void) const { return m_strName; }
	NumericType GetType(void) const { return m_type; }
	int GetDefaultAsInt(void) const { return m_nDefault; }
	float GetDefaultAsFloat(void) const { return m_fDefault; }
	int GetMinAsInt(void) const { return m_nMin; }
	float GetMinAsFloat(void) const { return m_fMin; }
	int GetMaxAsInt(void) const { return m_nMax; }
	float GetMaxAsFloat(void) const { return m_fMax; }
	int GetTickStep(void) const { return m_nTickStep; }
	TupString GetFormat(void) const { return m_strFormat; }
	bool HasEdit(void) const { return m_bHasEdit; }
	bool HasSpinner(void) const { return m_bHasSpinner; }
	bool HasSlider(void) const { return m_bHasSlider; }
};

//-----------------------------------------------------------------------------


#endif
