//-----------------------------------------------------------------------------
//	File:		TupConfigCheckBox.h
//	Created:	Nov. 20, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TupConfigCheckBox class represents the CHECK_BOX configuration
//			data.
//
//	History:	11/20/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

#ifndef TUPPERATTRIB_CONFIG_CHECK_BOX_H
#define TUPPERATTRIB_CONFIG_CHECK_BOX_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "ConfigBase.h"
#endif

#ifndef TUPPERWARE_ARRAY_H
#include "Tupperware/tuparray.h"
#endif


//-----------------------------------------------------------------------------
// Defines, globals, etc.:

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;
class TupConfigSelection;

//-----------------------------------------------------------------------------
// Class Declaration:

class TupConfigCheckBox : public TupConfigBase
{
	TupString m_strName;
	TupArray<TupConfigSelection*> m_vectCheckChildren;
	int	m_nDefaultIndex;

public:
	TupConfigCheckBox();
	~TupConfigCheckBox();

	bool BuildConfigTree(TupperwareAggregate* pCheckBoxTupAgg,TupArray<TupperwareAggregate *> &setArray,ConfigOverrides &overrides);
	TupConfigBase::ControlType GetControlType(void) const {	return TupConfigBase::CHECK_BOX; }
	bool GetKeySelections(const TupKeyValue &path,const char *pKeyName,TupKeySelection &selections) const;
	void GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const;

	// Get Methods
	TupString GetName(void) const { return m_strName; }
	int GetNumSelectionChildren(void) const { return m_vectCheckChildren.GetSize(); }
	const TupConfigSelection *GetSelectionChild(int index) const { return m_vectCheckChildren[index]; }
	int GetDefaultChild(void) const { return m_nDefaultIndex; }
};

//-----------------------------------------------------------------------------


#endif
