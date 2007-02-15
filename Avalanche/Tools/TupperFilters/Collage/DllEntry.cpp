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
 * User: Adam Clayton Date: 2/13/02    Time: 4:25p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Added Collage Image Reader
 * Uses Paintlib (16 extensions)
 * Can read file and add width and height to tup
 * More to come...
 */

////////////////////////////////////////////////////////////////////////////

#include "CollagePch.h"
#include "PluginDesc.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

CollagePluginDesc collagePluginDesc;

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
	return ("Collage Image Reader - Reads many image file types");
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) ImageReaderPluginDesc* LibPluginDesc(void)
{
	return &collagePluginDesc;
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_FILTER_CHAIN;
}

