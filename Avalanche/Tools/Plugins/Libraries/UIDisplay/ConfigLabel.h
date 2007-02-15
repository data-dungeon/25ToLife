//-----------------------------------------------------------------------------
//	File:		ConfigLabel.h
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


#ifndef __CONFIGLABEL__
#define __CONFIGLABEL__



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

class ConfigLabel : public ConfigBase
{
public:
	ConfigLabel();
	~ConfigLabel();

	bool BuildConfigTree(TupperwareAggregate *pLabelTupAgg,ConfigOverrides &overrides);
	ConfigBase::ControlType GetControlType(void) const { return ConfigBase::LABEL; }
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys) {}
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
		ExclusiveController* pExclusiveController, int nIndentLevel);

#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

	TupString GetLabelName(void) const { return m_strLabelName; }

protected:
	unsigned long ComputeCRC(void);

	TupString m_strLabelName;

private:
};

//-----------------------------------------------------------------------------


#endif
