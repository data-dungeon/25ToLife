//-----------------------------------------------------------------------------
//	File:		DefaultOverride.h
//	Created:	Apr. 25, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The DefaultOverride class holds an array of key/value pairs used for 
//			overriding the defaults when building a config tree
//
//-----------------------------------------------------------------------------
/*
 * $History: ConfigOverride.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/UIDisplay
 * ui display changes to use TupString and TupArrays.  Some const stuff
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 3:18p
 * Updated in $/Avalanche/tools/Plugins/Max/UIDisplay
 * disable command and override default
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/27/02    Time: 3:27p
 * Updated in $/Avalanche/tools/Plugins/Max/UIDisplay
 * Override KeyName
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 4:38p
 * Updated in $/Avalanche/tools/Plugins/Max/UIDisplay
 * Default overrides working - at least on this side
 */


#ifndef __CONFIGOVERRIDE__
#define __CONFIGOVERRIDE__

#ifndef __CURRATTRIBBASE__
#include "CurrAttribBase.h"
#endif

//-----------------------------------------------------------------------------
// Defines, enums, helper structures, etc.:

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

//-----------------------------------------------------------------------------
// Class Declaration:

struct OverrideEntry
{
	int m_type;
	TupString m_key;
	TupString m_value;
	OverrideEntry(void) {}
	OverrideEntry(const char *key,const char *value,int type) : 
		m_key(key), m_value(value), m_type(type) {}
};

class ConfigOverrides
{
	TupArray<OverrideEntry> m_entries;

public:

	enum OverrideType
	{
		OVERRIDE_DEFAULT,
		OVERRIDE_NONUI,
		OVERRIDE_DISABLED,
		OVERRIDE_LABEL,
		OVERRIDE_KEY,
		OVERRIDE_COMMENT,
		OVERRIDE_KEY_PREFIX,
		OVERRIDE_KEY_POSTFIX,
	};

	void AddEntry(const char *key,const char *value,OverrideType type);

	int KeyExistsAt(const char *key,OverrideType type) const;

	int GetSize(void) const;
	void SetSize(int size);

	const char *GetKeyAsString(int nIndex) const;
	OverrideType GetType(int nIndex) const;

	const char *GetValueAsString(int nIndex) const;
	int GetValueAsInt(int nIndex) const;
	float GetValueAsFloat(int nIndex) const;
	AttribColor GetValueAsColor(int nIndex) const;

// key prefix and postfix
	void AddKeyPrefix(const char *value);
	void PopKeyPrefix(void);
	void AddKeyPostfix(const char *value);
	void PopKeyPostfix(void);

	bool HasPrefixOrPostfix(void) const;
	void AddPrefixAndPostfix(TupString &string) const;
};

#endif
