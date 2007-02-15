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


#ifndef __CONFIGBASE__
#define __CONFIGBASE__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __CURRATTRIBBASE__
#include "CurrAttribBase.h"
#endif

#ifndef __CONFIGOVERRIDE__
#include "ConfigOverride.h"
#endif

#ifndef __CRC__
#include "CRC.h"
#endif

//-----------------------------------------------------------------------------
// Defines, enums, helper structures, etc.:

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class UIController;
class ExclusiveController;
class KeyValue;
class KeySelection;
class KeyValueContext;

#define RESOURCE_START_CHECKBOX 1500
#define RESOURCE_START_COMBOBOX 2001
#define RESOURCE_START_EDITBOX 2501
#define RESOURCE_START_FREECOLORBOX 3001
#define RESOURCE_START_NUMERIC 3501
#define RESOURCE_START_RADIO_GROUP 4001
#define RESOURCE_START_SET_COLOR_BOX 4501
#define RESOURCE_START_CHECKLIST 5001
#define RESOURCE_START_LABEL 5501
#define RESOURCE_START_LAST 6001

#define RESOURCE_END_CHECKBOX (RESOURCE_START_COMBOBOX-1)
#define RESOURCE_END_COMBOBOX (RESOURCE_START_EDITBOX-1)
#define RESOURCE_END_EDITBOX (RESOURCE_START_FREECOLORBOX-1)
#define RESOURCE_END_FREECOLORBOX (RESOURCE_START_NUMERIC-1)
#define RESOURCE_END_NUMERIC (RESOURCE_START_RADIO_GROUP-1)
#define RESOURCE_END_RADIO_GROUP (RESOURCE_START_SET_COLOR_BOX-1)
#define RESOURCE_END_SET_COLOR_BOX (RESOURCE_START_CHECKLIST-1)
#define RESOURCE_END_CHECKLIST (RESOURCE_START_LABEL-1)
#define RESOURCE_END_LABEL (RESOURCE_START_LAST-1)

//-----------------------------------------------------------------------------
// Class Declaration:

class ConfigBase
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
		LABEL,
	};

	enum DisplayType
	{
		DEFAULT,
		UNIQUE,
		INDETERMINATE
	};

	ConfigBase();
	virtual ~ConfigBase();

	virtual ControlType GetControlType(void) const { return BASE; }
	virtual bool GetKeySelections(CurrAttribBase* pAttribBase,const KeyValue &path,const char *pKeyName,KeySelection &selections) { return false; }
	virtual void GetKeyValueContexts(CurrAttribBase* pAttribBase,const KeyValue &path,KeyValueContext &keyValueContext,bool addKeys) = 0;
	virtual void ClearCachedData(void) {};
	virtual bool BuildUIDisplay(UIController* pUIController, CurrAttribBase* pAttribBase, ExclusiveController* pExclusiveController,int nIndentLevel){return false;}
	virtual void BuildExclusiveController(ExclusiveController* pExclusiveController){};
	virtual void FindConfigKeys(Keeper<TupString> &keys,const TupString &criteria) {};
	unsigned long GetCRC(void);

	static TupString ReturnKeyNameSubstr(const TupString &strToParse,const char* charDelimiter = "|");
	static TupString ReturnLabelNameSubstr(const TupString &strToParse,const char* charDelimiter = "|");

	static AttribColor StringToColor(TupString strColor);
	static TupString ColorToString(AttribColor color);

	bool HasComment(void) const;
	TupString GetComment(void) const;
	void SetComment(TupString tupString);

#ifdef _DEBUG
	virtual void PrintDebug(int nIndentLevel) const = 0;
#endif

	bool GetNonUI(void) const;
	bool GetDisabled(void) const;
	bool GetExclusive(void) const;
	int GetAddedIndent(void) const;

protected:
	DisplayType FindDisplayType(const TupString &strKeyname,const TupString &strDefaultVal,
								CurrAttribBase* pAttribBase, UniqueValues &uniqueValues, Keeper<TupString> *pValidValues = NULL) const;

	void SetNonUI(bool bNonUI);
	void SetDisabled(bool bDisabled);
	void SetExclusive(bool bExclusive);
	void SetAddedIndent(int nAddedIndent);

	bool m_bNonUI;		// Set if not displayed in the user interface
	bool m_bDisabled;	// Set if control is disabled in UI
	bool m_bExclusive;	// True if more than one child is active at a time
	int m_nAddedIndent;
	TupString m_strComment; // comment to use in tooltip

	virtual unsigned long ComputeCRC(void);
	// crc value for entry (includes children)
	unsigned long m_CRCValue;
	bool m_bCRCComputed;

private:
};

//-----------------------------------------------------------------------------


#endif
