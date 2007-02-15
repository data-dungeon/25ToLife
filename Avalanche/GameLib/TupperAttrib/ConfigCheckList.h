//-----------------------------------------------------------------------------
//	File:		ConfigCheckList.h
//	Created:	Aug. 29, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TupConfigCheckList class represents the CHECK_LIST
//			configuration data.
//
//	History:	8/29/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: ConfigCheckList.h $
 * 
 * *****************  Version 2  *****************
 * User: Brad Worthen Date: 8/29/02    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 1  *****************
 * User: Brad Worthen Date: 8/29/02    Time: 2:09p
 * Created in $/Avalanche/tools/TupperAttrib
 */


#ifndef TUPPERATTRIB_CONFIG_CHECK_LIST_H
#define TUPPERATTRIB_CONFIG_CHECK_LIST_H



//-----------------------------------------------------------------------------
// Includes:

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "ConfigBase.h"
#endif



//-----------------------------------------------------------------------------
// Defines, globals, etc.:

struct KeyValPair
{
	TupString m_strKeyname;
	TupString m_strVal;
};



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;



//-----------------------------------------------------------------------------
// Class Declaration:

class TupConfigCheckList : public TupConfigBase
{
public:
	// Constructors & Destructors:
	TupConfigCheckList();
	~TupConfigCheckList();

	// Public methods:
	bool BuildConfigTree(TupperwareAggregate* pCheckListTupAgg, ConfigOverrides& overrides);
	TupConfigBase::ControlType GetControlType(void) const {return TupConfigBase::CHECK_LIST;}

	// Public accessors:
	TupString GetName(void) const {return m_strName;}
	TupString GetDefault(void) const {return m_strDefault;}
	int GetNumCriteria(void) const {return m_vectCriteria.GetSize();}
	int GetNumEntityNames(void) const {return m_vectEntityNames.GetSize();}
	const KeyValPair GetCriteria(int index) const {return m_vectCriteria[index];}
	const TupString GetEntityName(int index) const {return m_vectEntityNames[index];}
	void GetKeyValueContexts(const TupKeyValue& path, TupKeyValueContext& keyValueContext, bool addKeys) const;
	static void ParseStrForCheckedNames(TupArray<TupString>& arrayCheckedNames, const TupString strToParse);

protected:
	// Protected data members:
	TupString m_strName;
	TupString m_strDefault;
	TupArray<KeyValPair> m_vectCriteria;
	TupArray<TupString> m_vectEntityNames;

	// Protected methods:
	void AddCriteriaToVect(KeyValPair newKeyValPair);

};

//-----------------------------------------------------------------------------


#endif
