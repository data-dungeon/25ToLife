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


#ifndef __CONFIGNUMERIC__
#define __CONFIGNUMERIC__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __CONFIGBASE__
#include "ConfigBase.h"
#endif

//-----------------------------------------------------------------------------
// Defines, globals, etc.:

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;


//-----------------------------------------------------------------------------
// Class Declaration:

class ConfigNumeric : public ConfigBase
{
public:
	enum NumericType
	{
		INT,
		FLOAT,
	};

	ConfigNumeric();
	~ConfigNumeric();

	bool BuildConfigTree(TupperwareAggregate* pNumericTupAgg,ConfigOverrides &overrides);
	ConfigBase::ControlType GetControlType(void) const { return ConfigBase::NUMERIC; }
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys);
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController, int nIndentLevel);

#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

	// Get Methods
	TupString GetKeyName(void) const { return m_strKeyName; }
	TupString GetLabelName(void) const { return m_strLabelName; }
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

protected:
	unsigned long ComputeCRC(void);

	TupString m_strKeyName;
	TupString m_strLabelName;
	NumericType m_type;
	int m_nMin;
	float m_fMin;
	int m_nMax;
	float m_fMax;
	int m_nDefault;
	float m_fDefault;
	int m_nTickStep;
	float m_fSpinnerStep;
	TupString m_strFormat;
	bool m_bHasEdit;
	bool m_bHasSpinner;
	bool m_bHasSlider;

private:
};

//-----------------------------------------------------------------------------


#endif
