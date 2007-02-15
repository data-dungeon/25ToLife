////////////////////////////////////////////////////////////////////////////
//
// DllEntry.cpp
//
// Dll Exported Functions for plugin
//
////////////////////////////////////////////////////////////////////////////
#include "Pch.h"
#include "PluginDesc.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

DrOctopusPluginDesc DrOctopusPluginDesc;

// This function is called by Windows when the DLL is loaded.  This
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(
HINSTANCE hinstDLL,
ULONG fdwReason,
LPVOID lpvReserved)
{
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.
	return (TRUE);
}

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const char* LibDescription()
{
	return ("DrOctopus Filter - Exports mesh collision data");
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) FilterPluginDesc* LibPluginDesc(void)
{
	return &DrOctopusPluginDesc;
}

// This function returns a pre-defined constant indicating the version of
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_FILTER_CHAIN;
}

