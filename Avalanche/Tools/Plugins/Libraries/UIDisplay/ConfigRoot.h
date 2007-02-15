//-----------------------------------------------------------------------------
//	File:		ConfigRoot.h
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigRoot class represents the outermost level in the
//			configuration file data.  The configuration file itself can be
//			thought of as the outermost level, in a way.  That is because the
//			file can contain more than one atom, an of which may be an
//			aggragate, and so on, and so on....
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __CONFIGROOT__
#define __CONFIGROOT__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __CONFIGBASE__
#include "ConfigBase.h"
#endif

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;
class UIController;
class CurrAttribBase;
class ExclusiveController;



//-----------------------------------------------------------------------------
// Class Declaration:

class ConfigRoot : public ConfigBase
{
public:
	ConfigRoot();
	~ConfigRoot();

	bool BuildConfigTree(TupperwareAggregate* rootTupAgg, bool root = true);
	ConfigBase::ControlType GetControlType(void) const { return ConfigBase::ROOT; }
	bool GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections);
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys);
	void ClearCachedData(void);
	const ConfigBase *GetChild(int index) const { return m_vectRootChildren[index]; }

	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController);
	void BuildExclusiveController(ExclusiveController* pExclusiveController);
	void FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria);

	// Get Methods
	int GetNumChildren(void) const { return m_vectRootChildren.GetSize(); }

#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

protected:
	unsigned long ComputeCRC(void);

	TupArray<ConfigBase*> m_vectRootChildren;
	TupArray<TupperwareAggregate *> m_setArray;
	ConfigOverrides m_overrides;

private:
};

//-----------------------------------------------------------------------------


#endif
