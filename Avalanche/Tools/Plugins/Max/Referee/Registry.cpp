//***************************************************************************
// A simple class wrapper for accessing the registry
//***************************************************************************

//#include "stdafx.h"
#include <windows.h>
#include "registry.h"

// Open up the key
Registry::Registry(char *keyName, HKEY hRootKey)
{
	// open/create registry key
	hKey = NULL;
	disposition = (DWORD)-1;
	RegCreateKeyEx(hRootKey, keyName, 0, NULL,
               REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
               NULL, &hKey, &disposition);
}

// Close out this entry
Registry::~Registry(void)
{
	if (hKey)
	{
		RegFlushKey(hKey);
		RegCloseKey(hKey);
	}
}

// Query the size of an item
BOOL Registry::QuerySize(const char *field,int *size)
{
	// Do we have an open entry?
	if (hKey == NULL)
		return(FALSE);

	// Get the key
	DWORD realSize = 0;
	LONG ret = RegQueryValueEx(hKey, field, NULL, NULL, NULL, &realSize);
	*size = realSize;

	// Did it work?
	return(ret == ERROR_SUCCESS);
}

// Set a value
BOOL Registry::Set(const char *field,DWORD type,const void *data,int size)
{
	// Do we have an open entry?
	if (hKey == NULL)
		return(FALSE);

	// Set it
	return(RegSetValueEx(hKey, field, 0, type, (CONST BYTE *)data, size) == ERROR_SUCCESS);
}

// Get a value
BOOL Registry::Get(const char *field,DWORD type,void *data,int size)
{
	// Do we have an open entry?
	if (hKey == NULL)
		return(FALSE);

	// Get the key
	DWORD realSize = size;
	DWORD tempType = type;
	LONG ret = RegQueryValueEx(hKey, field, NULL, &tempType, (BYTE *)data, &realSize);
	if (ret == ERROR_MORE_DATA)
	{
		BYTE *temp = (BYTE *)malloc(realSize);
		ret = RegQueryValueEx(hKey, field, NULL, &tempType, temp, &realSize);
		if (ret == ERROR_SUCCESS)
			memcpy(data, temp, size);
		free(temp);
	}

	// Did it work?
	return(ret == ERROR_SUCCESS);
}

// delete an entry
BOOL Registry::Delete(const char *field)
{
	int tempSize; 
	if (QuerySize(field,&tempSize))
	{
		LONG ret = RegDeleteValue(hKey,field);
		return(ret == ERROR_SUCCESS);
	}
	return FALSE;
}
