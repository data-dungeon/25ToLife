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
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/24/03    Time: 9:41a
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 * Added Stamp Filter
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/24/03    Time: 9:29a
 * Created in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////

#include "StampPch.h"
#include "PluginDesc.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

StampPluginDesc StampPluginDesc;

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
	return ("Stamp Filter - Applies stamps to meshes");
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) FilterPluginDesc* LibPluginDesc(void)
{
	return &StampPluginDesc;
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_FILTER_CHAIN;
}

