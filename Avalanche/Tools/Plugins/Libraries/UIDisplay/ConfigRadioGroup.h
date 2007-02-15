//-----------------------------------------------------------------------------
//	File:		ConfigRadioGroup.h
//	Created:	Nov. 16, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigRadioGroup class represents the RADIO_GROUP configuration
//			data.
//
//	History:	11/16/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __CONFIGRADIOGROUP__
#define __CONFIGRADIOGROUP__



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
class ConfigSelection;
class ExclusiveController;



//-----------------------------------------------------------------------------
// Class Declaration:

class ConfigRadioGroup : public ConfigBase
{
public:
	ConfigRadioGroup();
	~ConfigRadioGroup();

	bool BuildConfigTree(TupperwareAggregate *pRadioTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides);
	ConfigBase::ControlType GetControlType(void) const { return ConfigBase::RADIO_GROUP; }
	bool GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections);
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys);
	void ClearCachedData(void);
	// Get Methods
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController, int nIndentLevel);
	void BuildExclusiveController(ExclusiveController* pExclusiveController);
	void FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria);

#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

	TupString GetKeyName(void) const { return m_strKeyName; }
	TupString GetLabelName(void) const { return m_strLabelName; }
	int GetNumSelectionChildren(void) const { return m_vectRadioChildren.GetSize(); }
	const ConfigSelection *GetSelectionChild(int index) const { return m_vectRadioChildren[index]; }
	int GetDefaultChild(void) const { return m_nDefaultIndex; }

protected:
	unsigned long ComputeCRC(void);

	TupString m_strKeyName;
	TupString m_strLabelName;
	int	m_nDefaultIndex;
	TupArray<ConfigSelection*> m_vectRadioChildren;
	TupArray<TupString> m_vectKeysToPurge;

private:
};

//-----------------------------------------------------------------------------


#endif
