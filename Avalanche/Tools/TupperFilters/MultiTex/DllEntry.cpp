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
 * User: Adam Clayton Date: 8/20/02    Time: 3:11p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Duplicate Faces & Multitex Start
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 8/19/02    Time: 4:08p
 * Created in $/Avalanche/tools/TupperFilters/MultiTex
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Start
 */

////////////////////////////////////////////////////////////////////////////

#include "MultiTexPch.h"
#include "PluginDesc.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

MultiTexPluginDesc MultiTexPluginDesc;

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
	return ("Break Large Meshes Filter - Breaks up meshes that are too big into smaller meshes");
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) FilterPluginDesc* LibPluginDesc(void)
{
	return &MultiTexPluginDesc;
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_FILTER_CHAIN;
}

