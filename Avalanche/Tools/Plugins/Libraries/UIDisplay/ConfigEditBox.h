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


#ifndef __CONFIGEDITBOX__
#define __CONFIGEDITBOX__



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

class ConfigEditBox : public ConfigBase
{
public:
	enum EditBoxType
	{
		STRING,
		FILENAME,
		DIRECTORY,
		OBJECTNAME
	};

	ConfigEditBox();
	~ConfigEditBox();

	bool BuildConfigTree(TupperwareAggregate *pEditBoxTupAgg,ConfigOverrides &overrides);
	ConfigBase::ControlType GetControlType(void) const { return ConfigBase::EDIT_BOX; }
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys);
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController, int nIndentLevel);

#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

	// Get Methods
	TupString GetKeyName(void) const { return m_strKeyName; }
	TupString GetLabelName(void) const { return m_strLabelName; }
	EditBoxType GetType(void) const { return m_type; }
	TupString GetDefaultText(void) const { return m_strDefaultText; }

protected:
	unsigned long ComputeCRC(void);

	TupString m_strKeyName;
	TupString m_strLabelName;
	TupString m_strDefaultText;
	EditBoxType m_type;

private:
};

//-----------------------------------------------------------------------------


#endif
