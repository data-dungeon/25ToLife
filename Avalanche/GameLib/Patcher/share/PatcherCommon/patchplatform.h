//------------------------------------------------------------------------------
// patchplatform.h
//
// This gem :-b provides a way of incorporating the patching library into 
// multiple game engines and platforms. It is an attempt to keep all the muss
// out of the code and hidden in here.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#ifndef __PATCHPLATFORM_H__
#define __PATCHPLATFORM_H__ 1

// Define the various portable platforms we want to cover here.
#define PATCH_PLATFORM_GOE			1	// The Crystal Dynamics GOE engine
#define PATCH_PLATFORM_AVALANCHE	2	// Avalanche Software
#define PATCH_PLATFORM_NONE			3	// No platform defined

///////////////////////////////////////////////
// Hardwired to Avalanche
//#define PATCH_PLATFORM		PATCH_PLATFORM_AVALANCHE
///////////////////////////////////////////////

// Make one of the platforms our current platform. You can define it before 
// including this (like on the command line), or hardwire it here.
#ifndef PATCH_PLATFORM
	#define PATCH_PLATFORM		PATCH_PLATFORM_NONE
#endif

// Define a compiler. Do it here so as to make sure we cover all hw/compiler combos
// needed.
#if (defined(_MSC_VER))	// _WIN32 may be defined for non MSVC platforms; _MSC_VER is safer
	#define PATCH_COMPILER_MSVC_W32 1
#elif (defined(PS2))	// Does SN have a built-in macro we could use?
	#define PATCH_COMPILER_SN_PS2 1
#endif

// Define a print statement based on our compiler. To enable debug printing
// in a given module, define PATCH_PRINT_ENABLED above the include for this file
#if (defined(PATCH_COMPILER_SN_PS2))
	#include <eekernel.h>
	#ifdef PATCH_PRINT_ENABLED
		#define PATCH_PRINT scePrintf
	#else
		#define PATCH_PRINT(...)
	#endif
#elif (defined(PATCH_COMPILER_MSVC_W32))
	#include <stdio.h>
	#ifdef PATCH_PRINT_ENABLED
		#define PATCH_PRINT printf
	#else
		#define PATCH_PRINT __noop
	#endif
#endif

// Handle exports for dlls
#ifdef PATCH_COMPILER_MSVC_W32
	#ifdef PATCHER_EXPORTS
		#define PATCHER_EXPORT __declspec( dllexport )
	#else
		#define PATCHER_EXPORT __declspec( dllimport )
	#endif
#else
	#define PATCHER_EXPORT
#endif

// PLATFORM_PREAMBLE -- This macro goes at the top of the public section of 
// class declarations. For platforms that require something declared for all
// its classes in this spot, define it here. If there is more than one thing,
// use continuation lines to put it all under PLATFORM_PREAMBLE.
#if (PATCH_PLATFORM == PATCH_PLATFORM_GOE)
	#define PLATFORM_PREAMBLE GOE_NEW_N_DELETE(MEM_GAME)
#else
	#define PLATFORM_PREAMBLE
#endif

// Memory allocation and asserts
#if (PATCH_PLATFORM == PATCH_PLATFORM_GOE)
	#define PATCH_ASSERT(x)		GOE_ASSERT(x)
#elif (PATCH_PLATFORM == PATCH_PLATFORM_AVALANCHE)
	// Note: Correct platform defines, like SN and PS2, must be defined before this point!
	#include "platform/BaseType.h"
	#include "Layers/Assert/Assert.h"
	#define PATCH_ASSERT(x)		ASSERT(x)
#else
	#define PATCH_ASSERT(x)
#endif

#endif // __PATCHPLATFORM_H__
