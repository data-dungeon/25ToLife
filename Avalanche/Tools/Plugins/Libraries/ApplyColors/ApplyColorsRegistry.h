//-----------------------------------------------------------------------------
//	File:		ApplyColorsRegistry.h
//	Created:	November 17th, 2004
//				Copyright (c) 2002-2004 Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The ApplyColorsRegistry class handles storing, getting, setting, etc. of
//			information within the registry.
//
//-----------------------------------------------------------------------------


#ifndef __APPLYCOLORSREGISTRY__
#define __APPLYCOLORSREGISTRY__


//-----------------------------------------------------------------------------
// Class Declaration:

class ApplyColorsRegistry
{
public:
	// Constructors & Destructors:
	ApplyColorsRegistry(const char *keyName, HKEY hRootKey = HKEY_CURRENT_USER);
	~ApplyColorsRegistry(void);

	// Get methods:
	BOOL Get(const char *field, char *data, int maxSize)
		{ return(Get(field, REG_SZ, data, maxSize)); }
	BOOL Get(const char *field, DWORD *data)
		{ return(Get(field, REG_DWORD, data, sizeof(DWORD))); }
	BOOL Get(const char *field, void *data, int maxSize)
		{ return(Get(field, REG_BINARY, data, maxSize)); }

	// Set methods:
	BOOL Set(const char *field, const char *data)
		{ return(Set(field, REG_SZ, data, strlen(data) + 1)); }
	BOOL Set(const char *field, const DWORD data)
		{ return(Set(field, REG_DWORD, &data, sizeof(data))); }
	BOOL Set(const char *field, const void *data, int size)
		{ return(Set(field, REG_BINARY, data, size)); }

	// Misc. methods:
	// Was the key there, or created?
	BOOL KeyWasNew(void)
		{ return(m_dwDisposition == REG_CREATED_NEW_KEY); }
	// Query the size of an item
	BOOL QuerySize(const char *field, int *size);
	// Delete an entry
	BOOL Delete(const char *field);
	// Enumerate a key:
	DWORD EnumValue(DWORD dwIndex, LPTSTR lpValName, LPDWORD lpdValLength,
					LPBYTE lpbData, LPDWORD lpdDataLength);
	// Get the number of values in this key:
	DWORD GetNumValues();

private:
	// Private methods:
	// Set a value
	BOOL Set(const char *field, DWORD type, const void *data, int size);
	// Get a value
	BOOL Get(const char *field, DWORD type, void *data, int size);

	// Private data members:
	HKEY	m_hKey;
	DWORD	m_dwDisposition;
};

#endif
