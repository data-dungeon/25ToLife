//------------------------------------------------------------------------------
// PatchExtractor.h
//
// Source for game patching library.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

// Enable/disable our debug print statements
//#define PATCH_PRINT_ENABLED

#include "PatchExtractor.h"
#include "libsn.h"

//------------------------------------------------------------------------------
// Load the named DLL into the given buffer and relocate code. If the module
// has been patched, the patched version is loaded from the patch buffer.
// Otherwise, it is loaded directly from DVD.
bool PatchExtractor::LoadModule(const char* name, char* buffer, int bufferSize)
{
	bool loadStatus = PatchItem(name, buffer, bufferSize);

	if (!loadStatus)
	{
		PATCH_PRINT("Patcher::LoadModule -- No entry found, just loading original file\n");
		// No patch available, just read the module into the buffer
		int rstat = m_TOC.ReadFile(name, buffer, bufferSize);
		if (rstat <= 0)
			return false;
	}

	// OK, the module is in the buffer. Relocate the code to prepare for execution
	FlushCache(0);
	return (!snDllLoaded(buffer, 0));
}

//------------------------------------------------------------------------------
// Load the last found DLL into the given buffer and relocate code.
// Only to be called after a positive return from PatchExists; this function
// does not load the module if it has not been patched.
bool PatchExtractor::LoadModule(char* buffer, int bufferSize)
{
	bool loadStatus = PatchItem(buffer, bufferSize);

	// OK, the module is in the buffer. Relocate the code to prepare for execution
	FlushCache(0);
	return (loadStatus && !snDllLoaded(buffer, 0));
}


