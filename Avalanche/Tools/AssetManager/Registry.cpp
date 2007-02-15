//-----------------------------------------------------------------------------
//	File:		Registry.cpp
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



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#ifndef __REGISTRY__
#include "Registry.h"
#endif



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// Registry::Registry
// This method is the constructor for the class.
//
// Input:
//  char *keyName -- The keyname to use.
//  HKEY hRootKey -- The root key.
//
// Output: None.
//-----------------------------------------------------------------------------
Registry::Registry(char *keyName,HKEY hRootKey)
{
	// open/create registry key
	m_hKey = NULL;
	m_dwDisposition = (DWORD)-1;
	RegCreateKeyEx(hRootKey, keyName, 0, NULL,
               REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
               NULL, &m_hKey, &m_dwDisposition);
	ASSERT(m_hKey != NULL);
}


//-----------------------------------------------------------------------------
// Registry::~Registry
// This is the destructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
Registry::~Registry(void)
{
	if (m_hKey)
	{
		RegFlushKey(m_hKey);
		RegCloseKey(m_hKey);
	}
}


//-----------------------------------------------------------------------------
// Registry::QuerySize
// This method will query the size of an item.
//
// Input:
//  const char *field -- The field to use.
//  int *size -- The size of the item.
//
// Output: TRUE if the method was successfull, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL Registry::QuerySize(const char *field,int *size)
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
// Registry::Set
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
BOOL Registry::Set(const char *field,DWORD type,const void *data,int size)
{
	// Do we have an open entry?
	if (m_hKey == NULL)
		return(FALSE);

	// Set it
	return(RegSetValueEx(m_hKey, field, 0, type, (CONST BYTE *)data, size) == ERROR_SUCCESS);
}


//-----------------------------------------------------------------------------
// Registry::Get
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
BOOL Registry::Get(const char *field,DWORD type,void *data,int size)
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
// Registry::Delete
// This method will delete an item from the registry.
//
// Input:
//  const char *field -- The field to delete.
//
// Output: TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL Registry::Delete(const char *field)
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
// Registry::EnumValue
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
DWORD Registry::EnumValue(DWORD dwIndex, LPTSTR lpValName, LPDWORD lpdValLength,
						  LPBYTE lpbData, LPDWORD lpdDataLength)
{
	DWORD dwResult;

	dwResult = RegEnumValue(m_hKey, dwIndex, lpValName, lpdValLength,
							NULL, NULL, lpbData, lpdDataLength);

	return dwResult;

}


//-----------------------------------------------------------------------------
// Registry::GetNumValues
// This method will return the number of values for this key.
//
// Input: None.
//
// Output: DWORD representing the number of values.  If this function fails,
//         then the return value will be -1.
//-----------------------------------------------------------------------------
DWORD Registry::GetNumValues()
{
	DWORD dwNumValues;

	if (RegQueryInfoKey(m_hKey, NULL, NULL, NULL, NULL, NULL, NULL,
					&dwNumValues, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		dwNumValues = -1;
	}

	return dwNumValues;
}
