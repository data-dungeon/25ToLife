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

/*
 * $History: ConfigRoot.h $
 * 
 * *****************  Version 19  *****************
 * User: Dross        Date: 6/12/02    Time: 5:14p
 * Updated in $/Avalanche/tools/TupperAttrib
 * changed array.h to tuparray.h
 * 
 * *****************  Version 18  *****************
 * User: Todd Blackburn Date: 6/06/02    Time: 1:14p
 * Updated in $/Avalanche/GameLib/TupperAttrib
 * atb fixed GCN compile errors
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

#ifndef TUPPERATTRIB_CONFIG_ROOT_H
#define TUPPERATTRIB_CONFIG_ROOT_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef TUPPERATTRIB_CONFIG_BASE_H
#include "TupperAttrib/ConfigBase.h"
#endif

#ifndef TUPPERWARE_ARRAY_H
#include "Tupperware/tuparray.h"
#endif

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupperwareAggregate;

//-----------------------------------------------------------------------------
// Class Declaration:

class TupConfigRoot : public TupConfigBase
{
	TupArray<TupConfigBase*> m_vectRootChildren;
	TupArray<TupperwareAggregate *> m_setArray;
	ConfigOverrides m_overrides;

public:
	TupConfigRoot();
	~TupConfigRoot();

	bool BuildConfigTree(TupperwareAggregate *rootTupAgg,bool root=true);
	TupConfigBase::ControlType GetControlType(void) const { return TupConfigBase::ROOT; }
	bool GetKeySelections(const TupKeyValue &path,const char *pKeyName,TupKeySelection &selections) const;
	void GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const;
	// Get Methods
	int GetNumChildren(void) const { return m_vectRootChildren.GetSize(); }
	const TupConfigBase *GetChild(int index) const { return m_vectRootChildren[index]; }
};

//-----------------------------------------------------------------------------


#endif
