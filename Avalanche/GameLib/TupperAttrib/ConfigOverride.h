//-----------------------------------------------------------------------------
//	File:		ConfigOverride.h
//	Created:	Apr. 25, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The Override class holds an array of key/value pairs used for 
//			overriding the defaults when building a config tree
//
//-----------------------------------------------------------------------------
/*
 * $History: ConfigOverride.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/27/02    Time: 2:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Override Keyname additions
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Overrides added
 */


#ifndef TUPPERATTRIB_CONFIG_OVERRIDE_H
#define TUPPERATTRIB_CONFIG_OVERRIDE_H

struct TupAttribColor;

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
		OVERRIDE_LABEL,
		OVERRIDE_KEY,
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
	TupAttribColor GetValueAsColor(int nIndex) const;

// key prefix and postfix
	void AddKeyPrefix(const char *value);
	void PopKeyPrefix(void);
	void AddKeyPostfix(const char *value);
	void PopKeyPostfix(void);

	bool HasPrefixOrPostfix(void) const;
	void AddPrefixAndPostfix(TupString &string) const;
};

#endif
