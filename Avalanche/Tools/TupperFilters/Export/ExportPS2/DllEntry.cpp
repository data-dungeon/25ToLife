////////////////////////////////////////////////////////////////////////////
//
// DllEntry.cpp
//
// Dll Exported Functions for plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DllEntry.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 2:05p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Start of PS2 Filter
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Start of Tupperchain filter for the PC 
 * Currently got as far to output the SOM data (should not work yet)
 */

////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "PluginDesc.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

ExportPS2PluginDesc exportPCPluginDesc;

// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.
	return (TRUE);
}

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const char* LibDescription()
{
	return ("ExportPC Filter - Exports data to a PC DBL file");
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) FilterPluginDesc* LibPluginDesc(void)
{
	return &exportPCPluginDesc;
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_FILTER_CHAIN;
}

