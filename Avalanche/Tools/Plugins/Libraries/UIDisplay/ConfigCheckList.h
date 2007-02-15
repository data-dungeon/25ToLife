//-----------------------------------------------------------------------------
//	File:		ConfigCheckList.h
//	Created:	July 22, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigCheckList class represents the CHECK_LIST configuration
//			data.
//
//	History:	7/22/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __CONFIGCHECKLIST__
#define __CONFIGCHECKLIST__



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

class ConfigCheckList : public ConfigBase
{
public:
	// Constructors & Destructors:
	ConfigCheckList();
	~ConfigCheckList();

	// Public accessors & mutators:

	// Public methods:
	bool BuildConfigTree(TupperwareAggregate* pCheckListTupAgg, ConfigOverrides& overrides);
	ConfigBase::ControlType GetControlType(void) const {return ConfigBase::CHECK_LIST;}
	bool GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections);
	void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue& path, KeyValueContext& keyValueContext, bool addKeys);
	void ClearCachedData(void);
	bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase,
						ExclusiveController* pExclusiveController, int nIndentLevel);
	void FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria);
#ifdef _DEBUG
	void PrintDebug(int nIndentLevel) const;
#endif

	// Public accessors:
	TupString GetKeyName(void) const { return m_strKeyName; }
	TupString GetLabelName(void) const { return m_strLabelName; }
	TupString GetDefault(void) const {return m_strDefault;}
	int GetNumCriteria(void) const {return m_vectCriteria.GetSize();}
	int GetNumEntityNames(void) const {return m_vectEntityNames.GetSize();}
	const KeyValPair GetCriteria(int index) const {return m_vectCriteria[index];}
	const TupString GetEntityName(int index) const {return m_vectEntityNames[index];}
	static void ParseStrForCheckedNames(TupArray<TupString>& arrayCheckedNames, const TupString &strToParse);

protected:
	// Protected accessors & mutators:
	void SetPixelHeight(int nNewPixelHeight);
	void AddCriteriaToVect(const KeyValPair &newKeyValPair);
	unsigned long ComputeCRC(void);

	// Protected data members:
	TupString m_strKeyName;
	TupString m_strLabelName;
	TupString m_strDefault;
	int m_nPixelHeight;
	TupArray<KeyValPair> m_vectCriteria;
	bool m_bEntityNamesValid;
	Keeper<TupString> m_vectEntityNames;

private:
};

//-----------------------------------------------------------------------------


#endif
