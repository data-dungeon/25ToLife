//-----------------------------------------------------------------------------
//	File:		ConfigBase.h
//	Created:	Nov. 15, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ConfigBase class is the root class for the UI data tree.  The
//			UI data tree is the tree that basically mimicks the configuration
//			file.  It is used to store all the possible ui for the object
//			attributes (with parent-child relationships).
//
//	History:	11/15/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: ConfigBase.h $
 * 
 * *****************  Version 24  *****************
 * User: Brad Worthen Date: 8/29/02    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 23  *****************
 * User: Brad Worthen Date: 8/29/02    Time: 2:09p
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 22  *****************
 * User: Todd Blackburn Date: 6/06/02    Time: 1:14p
 * Updated in $/Avalanche/GameLib/TupperAttrib
 * atb fixed GCN compile errors
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 9:29a
 * Updated in $/Avalanche/tools/TupperAttrib
 * changed name of array and string files
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 5/15/02    Time: 4:08p
 * Updated in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 5/15/02    Time: 2:21p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Alternate memory manager hooked in
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 8:47a
 * Updated in $/Avalanche/tools/TupperAttrib
 * const functions and get methods
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */


#ifndef TUPPERATTRIB_CONFIG_BASE_H
#define TUPPERATTRIB_CONFIG_BASE_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef TUPPERWARE_STRING_H
#include "Tupperware/tupstring.h"
#endif

#ifndef TUPPERATTRIB_CONFIG_OVERRIDE_H
#include "TupperAttrib/ConfigOverride.h"
#endif

#ifndef TUPPERWARE_MEMORY_MANAGER_H
#include "Tupperware/memorymanager.h"
#endif

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class TupKeyValue;
class TupKeySelection;
class TupKeyValueContext;

//-----------------------------------------------------------------------------
// Helper Classes

struct TupAttribColor
{
	int m_nRed;
	int m_nGreen;
	int m_nBlue;
	TupString m_strName;
};


//-----------------------------------------------------------------------------
// Class Declaration:

class TupConfigBase
{
public:
	enum ControlType
	{
		BASE,
		ROOT,
		SELECTION,
		CHECK_BOX,
		COMBO_BOX,
		RADIO_GROUP,
		EDIT_BOX,
		NUMERIC,
		FREE_COLOR_BOX,
		SET_COLOR_BOX,
		CHECK_LIST,
	};

	TupConfigBase();
	virtual ~TupConfigBase();

	virtual ControlType GetControlType(void) const { return BASE; }
	virtual bool GetKeySelections(const TupKeyValue &path,const char *pKeyName,TupKeySelection &selections) const { return false; }
	virtual void GetKeyValueContexts(const TupKeyValue &path,TupKeyValueContext &keyValueContext,bool addKeys) const = 0;

	static TupString ReturnKeynameSubstr(TupString strToParse, const char* pChDelimiter = "|");
	static TupString ReturnLabelSubstr(TupString strToParse, const char* pChDelimiter = "|");

	static TupAttribColor StringToColor(TupString strColor);
	static TupString ColorToString(TupAttribColor color);

	bool GetNonUI(void) const;
	bool GetExclusive(void) const;

#ifdef USE_TUP_MEMORY_MANAGER
	void *operator new(size_t size);
	void operator delete(void *pBuffer);
	void *Allocate(size_t nSize);
	void FreeBuffer(void *pBuffer);
#endif

protected:

	void SetNonUI(bool nonUI);
	void SetExclusive(bool exclusive);

	bool m_bNonUI; // set if not displayed in the user interface
	bool m_bExclusive; // true if more than one child is active at a time

private:
};

//-----------------------------------------------------------------------------


#endif
