//-----------------------------------------------------------------------------
//	File:		ConfigCheckBox.h
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigCheckBox class represents the CHECK_BOX configuration
//			data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __CONFIGCHECKBOX__
#define __CONFIGCHECKBOX__


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


//-----------------------------------------------------------------------------
// Class Declaration:

class ConfigCheckBox : public ConfigBase
{
public:
	ConfigCheckBox();
	~ConfigCheckBox();

	bool BuildConfigTree(TupperwareAggregate* pCheckBoxTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides);
	ConfigBase::ControlType GetControlType(void) const {	return ConfigBase::CHECK_BOX; }
	bool GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections);
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys);
	void ClearCachedData(void);
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController, int nIndentLevel);
	void BuildExclusiveController(ExclusiveController* pExclusiveController);
	void FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria);

	// Get Methods
	TupString GetKeyName(void) const { return m_strKeyName; }
	TupString GetLabelName(void) const { return m_strLabelName; }
	int GetNumSelectionChildren(void) const { return m_vectCheckChildren.GetSize(); }
	const ConfigSelection *GetSelectionChild(int index) const { return m_vectCheckChildren[index]; }
	int GetDefaultChild(void) const { return m_nDefaultIndex; }

#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

protected:
	unsigned long ComputeCRC(void);

	TupString m_strKeyName;
	TupString m_strLabelName;
	TupArray<ConfigSelection*> m_vectCheckChildren;
	TupArray<TupString> m_vectKeysToPurge;
	int	m_nDefaultIndex;

private:
};

//-----------------------------------------------------------------------------


#endif
