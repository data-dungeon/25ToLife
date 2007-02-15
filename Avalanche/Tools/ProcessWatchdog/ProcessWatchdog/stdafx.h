// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// these need to be added to the objective toolkit stdafx.h file in order to work
#define WINVER		 0x0501
#define _WIN32_WINNT 0x0501
#define	_WIN32_IE	 0x0560

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxmt.h>

#define WM_TRAY_NOTIFY WM_APP+1000

#include <shlwapi.h>
#include <shlobj.h>


// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/tuparray.h>
#include <tupperware/tupstring.h>
#include <tupperware/pool.h>
#include <tupperware/keeper.h>
