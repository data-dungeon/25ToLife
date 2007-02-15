//-----------------------------------------------------------------------------
//	File:		Registry.h
//	Created:	Feb. 19, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The Registry class handles storing, getting, setting, etc. of
//			information within the registry.
//
//	History:	2/19/2002: Initial creation - Adam Clayton
//				2/28/2002: Commented things a bit and added this class to the
//						   ObjectAttributes project - Brad Worthen
//				4/15/2002: Moved this to the UIDisplay project - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __REGISTRY__
#define __REGISTRY__



//-----------------------------------------------------------------------------
// Class Declaration:

class Registry
{
public:
	// Constructors & Destructors:
	Registry(char *keyName, HKEY hRootKey = HKEY_CURRENT_USER);
	~Registry(void);

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
