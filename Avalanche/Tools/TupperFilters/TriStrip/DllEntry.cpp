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
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/TriStrip
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/17/02    Time: 2:08p
 * Updated in $/Avalanche/tools/TupperFilters/TriStrip
 * Start of tristrip project
 */

////////////////////////////////////////////////////////////////////////////

#include "TriStripPch.h"
#include "PluginDesc.h"

HINSTANCE hInstance;

TriStripPluginDesc triStripPluginDesc;

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
	return ("TriStrip Filter - Builds Triangle Strips from TriMeshes");
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) FilterPluginDesc* LibPluginDesc(void)
{
	return &triStripPluginDesc;
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_FILTER_CHAIN;
}

