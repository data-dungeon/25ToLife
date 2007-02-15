// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__255F2020_5A67_47AE_A829_AC73AA1374AB__INCLUDED_)
#define AFX_STDAFX_H__255F2020_5A67_47AE_A829_AC73AA1374AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

extern const char *g_pFilterChainDate;
extern const char *g_pFilterChainTime;

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxpriv.h>

#include <io.h>

#include "tupperware/aggregate.h"
#include "tupperware/loadsave.h"
#include "tupperware/tuparray.h"
#include "tupperware/tupstring.h"
#include "tupperware/keeper.h"

#include "TupperFilters/Libraries/Plugins/FilterPlugin.h"
#include "TupperFilters/Libraries/Plugins/Options.h"
#include "TupperFilters/Libraries/Plugins/DataManager.h"
#include "TupperFilters/Libraries/Plugins/imageManager.h"
#include "TupperFilters/Libraries/Plugins/PluginError.h"
#include "TupperFilters/Libraries/Plugins/PluginLog.h"
#include "TupperFilters/Libraries/Plugins/filepath.h"
#include "TupperFilters/Libraries/Plugins/filepatharray.h"
#include "TupperFilters/Libraries/Plugins/ImageReaderOptions.h"
#include "TupperFilters/Libraries/Plugins/TokenizeString.h"

#include "TupperChain/ChainManager.h"
#include "TupperChain/FilterPluginManager.h"
#include "TupperChain/ConfigManager.h"
#include "TupperChain/OptionOverrides.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__255F2020_5A67_47AE_A829_AC73AA1374AB__INCLUDED_)
