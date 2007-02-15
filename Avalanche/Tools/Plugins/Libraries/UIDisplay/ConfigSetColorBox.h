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


#ifndef __CONFIGSETCOLORBOX__
#define __CONFIGSETCOLORBOX__



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

class ConfigSetColorBox : public ConfigBase
{
public:
	ConfigSetColorBox();
	~ConfigSetColorBox();

	bool BuildConfigTree(TupperwareAggregate *pSetColorTupAgg,ConfigOverrides &overrides);
	ConfigBase::ControlType GetControlType(void) const { return ConfigBase::SET_COLOR_BOX; }
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys);
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController, int nIndentLevel);
	
#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

	// Get Methods
	TupString GetKeyName(void) const { return m_strKeyName; }
	TupString GetLabelName(void) const { return m_strLabelName; }
	AttribColor GetDefaultColor(void) const { return m_defaultColor; }
	int GetNumColors(void) const { return m_vectColors.GetSize(); }
	AttribColor GetColor(int index) const { return m_vectColors[index]; }

protected:
	unsigned long ComputeCRC(void);

	TupString m_strKeyName;
	TupString m_strLabelName;
	AttribColor	m_defaultColor;
	TupArray<AttribColor> m_vectColors;

private:
};

//-----------------------------------------------------------------------------


#endif
