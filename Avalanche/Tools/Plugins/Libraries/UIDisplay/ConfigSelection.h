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


#ifndef __CONFIGSELECTION__
#define __CONFIGSELECTION__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __CONFIGBASE__
#include "ConfigBase.h"
#endif

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;



//-----------------------------------------------------------------------------
// Class Declaration:

class ConfigSelection : public ConfigBase
{
public:
	ConfigSelection();
	~ConfigSelection();

	bool BuildConfigTree(TupperwareAggregate* pSelectionTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides,bool selection=true);
	ConfigBase::ControlType GetControlType(void) const { return ConfigBase::SELECTION; }
	bool GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections);
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys);
	void ClearCachedData(void);
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController, int nIndentLevel);
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController, int nIndentLevel, 
						const TupArray<bool> &validChildren);
	void BuildExclusiveController(ExclusiveController* pExclusiveController);
	void FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria);
	TupString GetKeyName(void) const { return m_strKeyName; }
	TupString GetLabelName(void) const { return m_strLabelName; }
	bool HasUIChildren(void) const;
	int GetNumChildren(void) const { return m_vectSelectionChildren.GetSize(); }
	const ConfigBase *GetChild(int index) const { return m_vectSelectionChildren[index]; }
	ConfigBase *GetChild(int index) { return m_vectSelectionChildren[index]; }

#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

protected:
	unsigned long ComputeCRC(void);

	TupString m_strKeyName;
	TupString m_strLabelName;
	TupArray<ConfigBase*> m_vectSelectionChildren;
	bool m_bHasUIChildren;

private:
};

//-----------------------------------------------------------------------------


#endif
