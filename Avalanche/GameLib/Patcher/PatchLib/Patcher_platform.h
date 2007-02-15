//------------------------------------------------------------------------------
// Patcher_platform.h
//
// Platform-specific includes for Patcher.
//
// Copyright (c) 2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------
#ifndef __PATCHER_PLATFORM_H__
#define __PATCHER_PLATFORM_H__ 1

//------------------------------------------------------------------------------
#if defined(PATCH_COMPILER_MSVC_W32)

// For Windows, define a function to launch the patcher application, WinUpdater.
PATCHER_EXPORT bool LaunchPatcherApp(const char* host, const char* hostPath, const char* dirToPatch, const char* updateApp = NULL);

#endif	// PATCH_COMPILER_...
//------------------------------------------------------------------------------

#endif // __PATCHER_PLATFORM_H__