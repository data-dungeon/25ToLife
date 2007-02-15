/-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Includes:

#include "ApplyColorsPCH.h"
#include "ApplyColorsRegistry.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ApplyColorsRegistry::ApplyColorsRegistry
// This method is the constructor for the class.
//
// Input:
//  char *keyName -- The keyname to use.
//  HKEY hRootKey -- The root key.
//
// Output: None.
//-----------------------------------------------------------------------------
ApplyColorsRegistry::ApplyColorsRegistry(const char *keyName,HKEY hRootKey)
{
	// open/create registry key
	m_hKey = NULL;
	m_dwDisposition = (DWORD)-1;
	RegCreateKeyEx(hRootKey, keyName, 0, NULL,
               REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
               NULL, &m_hKey, &m_dwDisposition);
	assert(m_hKey != NULL);
}


//-----------------------------------------------------------------------------
// ApplyColorsRegistry::~ApplyColorsRegistry
// This is the destructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
ApplyColorsRegistry::~ApplyColorsRegistry(void)
{
	if (m_hKey)
	{
		RegFlushKey(m_hKey);
		RegCloseKey(m_hKey);
	}
}


//-----------------------------------------------------------------------------
// ApplyColorsRegistry::QuerySize
// This method will query the size of an item.
//
// Input:
//  const char *field -- The field to use.
//  int *size -- The size of the item.
//
// Output: TRUE if the method was successfull, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL ApplyColorsRegistry::QuerySize(const char *field,int *size)
{
	// Do we have an open entry?
	if (m_hKey == NULL)
		return(FALSE);

	// Get the key
	DWORD realSize = 0;
	LONG ret = RegQueryValueEx(m_hKey, field, NULL, NULL, NULL, &realSize);
	*size = realSize;

	// Did it work?
	return(ret == ERROR_SUCCESS);
}


//-----------------------------------------------------------------------------
// ApplyColorsRegistry::Set
// This method will set data within the registry.
//
// Input:
//  const char *field -- The field to use.
//  DWORD type -- The type of the data.
//  const void *data -- The data itself.
//  int size -- The size to use.
//
// Output: TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL ApplyColorsRegistry::Set(const char *field,DWORD type,const void *data,int size)
{
	// Do we have an open entry?
	if (m_hKey == NULL)
		return(FALSE);

	// Set it
	return(RegSetValueEx(m_hKey, field, 0, type, (CONST BYTE *)data, size) == ERROR_SUCCESS);
}


//-----------------------------------------------------------------------------
// ApplyColorsRegistry::Get
// This method will get a value from the registry.
//
// Input:
//  const char *field -- The field to use.
//  DWORD type -- The type of the data to get.
//  void *data -- The data itself
//  int size -- The size to use.
//
// Output: TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL ApplyColorsRegistry::Get(const char *field,DWORD type,void *data,int size)
{
	// Do we have an open entry?
	if (m_hKey == NULL)
		return(FALSE);

	// Get the key
	DWORD realSize = size;
	DWORD tempType = type;
	LONG ret = RegQueryValueEx(m_hKey, field, NULL, &tempType, (BYTE *)data, &realSize);
	if (ret == ERROR_MORE_DATA)
	{
		BYTE *temp = (BYTE *)malloc(realSize);
		ret = RegQueryValueEx(m_hKey, field, NULL, &tempType, temp, &realSize);
		if (ret == ERROR_SUCCESS)
			memcpy(data, temp, size);
		free(temp);
	}

	// Did it work?
	return(ret == ERROR_SUCCESS);
}


//-----------------------------------------------------------------------------
// ApplyColorsRegistry::Delete
// This method will delete an item from the registry.
//
// Input:
//  const char *field -- The field to delete.
//
// Output: TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL ApplyColorsRegistry::Delete(const char *field)
{
	int tempSize; 
	if (QuerySize(field,&tempSize))
	{
		LONG ret = RegDeleteValue(m_hKey,field);
		return(ret == ERROR_SUCCESS);
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// ApplyColorsRegistry::EnumValue
// This method will copy one indexed value name and data block for the
// specified key each time it is called.
//
// Input:
//  DWORD dwIndex -- The index of the value to get.
//  LPTSTR lpValName -- On output, this will hold the name of the value.
//  LPDWORD lpdValLength -- On input, this should specify the length of the
//							incoming lpValName.  On output, this will hold the
//							length of the name in the lpValName buffer.
//	LPBYTE lpbData -- On output, this will hold the data for the value.
//	LPDWORD lpdDataLength -- On input, this should specify the length of the
//							 incoming lpbData.  On output, this will hold the
//							 length of the data in the lpbData buffer.
//
// Output: ERROR_SUCCESS if this call was successfull, otherwise nonzero.
//-----------------------------------------------------------------------------
DWORD ApplyColorsRegistry::EnumValue(DWORD dwIndex, LPTSTR lpValName, LPDWORD lpdValLength,
						  LPBYTE lpbData, LPDWORD lpdDataLength)
{
	DWORD dwResult;

	dwResult = RegEnumValue(m_hKey, dwIndex, lpValName, lpdValLength,
							NULL, NULL, lpbData, lpdDataLength);

	return dwResult;

}


//-----------------------------------------------------------------------------
// ApplyColorsRegistry::GetNumValues
// This method will return the number of values for this key.
//
// Input: None.
//
// Output: DWORD representing the number of values.  If this function fails,
//         then the return value will be -1.
//-----------------------------------------------------------------------------
DWORD ApplyColorsRegistry::GetNumValues()
{
	DWORD dwNumValues;

	if (RegQueryInfoKey(m_hKey, NULL, NULL, NULL, NULL, NULL, NULL,
					&dwNumValues, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		dwNumValues = -1;
	}

	return dwNumValues;
}
