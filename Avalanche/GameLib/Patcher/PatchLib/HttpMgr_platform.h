//------------------------------------------------------------------------------
// HttpMgr_platform.h
//
// Platform-specific includes for HttpMgr.
//
// Original Author:					David Goodenough, Crystal Dynamics
// Modified for Updater library:	Dan Stanfill, Pinniped Software
//									Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------
#ifndef __HTTPMGR_PLATFORM_H__
#define __HTTPMGR_PLATFORM_H__ 1

// Prototypes -- These functions need to be implemented for all platforms

// PollSocket -- Test the socket to see if it is ready to be read from. Returns
// -1 on error, zero if not ready, and 1 if ready.
int PollSocket(int s);

//------------------------------------------------------------------------------
#if defined(PATCH_COMPILER_SN_PS2)

#include <eekernel.h>
#include <string.h>
#include <libinsck.h>

// Get error code from last socket call
#define LastSocketError() sceInsockErrno
#define LastHostnameError() sceInsockHErrno

// For some reason Sony chose their own macro for this...
#define SD_BOTH SHUT_RDWR

// Call SocketSetup when creating a new socket to set platform-specific options
void SocketSetup(int sock);

// Call SocketCleanup when terminating a thread that was using the socket library.
void SocketCleanup(void);

//------------------------------------------------------------------------------
#elif defined(PATCH_COMPILER_MSVC_W32)

#include <stdio.h>
#include <winsock2.h>

// Get error code from last socket call
#define LastSocketError() WSAGetLastError()
#define LastHostnameError() WSAGetLastError()

// SocketSetup and SocketCleanup are a noop for windows
#define SocketSetup(s) __noop
#define SocketCleanup()

#endif


#endif // __HTTPMGR_PLATFORM_H__