// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef __STDAFX__
#define __STDAFX__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// these need to be added to the objective toolkit stdafx.h file in order to work
#define WINVER		 0x0501
#define _WIN32_WINNT 0x0501
#define	_WIN32_IE	 0x0560

#define ISOLATION_AWARE_ENABLED	1

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

extern const char *g_pAssetManagerDate;
extern const char *g_pAssetManagerTime;
extern const char *g_pAssetManagerVersion;

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxpriv.h>

// winsock stuff
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock")
#include <WinSock2.h>
#include <MSWSock.h>

// Stingray Objective Toolkit
// use Objective Toolkit as a DLL
#define _SECDLL
#include <toolkit\ot_layoutmgr.h>	// Objective Toolkit Pro Layout Manager
#include <toolkit\ot_treectrl.h>	// Objective Toolkit tree control
#include <toolkit\ot_buttons.h>		// Objective Toolkit button controls
#include <toolkit\browedit.h>		// Objective Toolkit edit browser stuff
// alternative to above: if you want to include ALL OT headers, uncomment this line
// to use the classic OT inclusion method (will increase build time)
//#include "toolkit\secall.h"

#include <commctrl.h>
#include <shlwapi.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/tuparray.h>
#include <tupperware/tupstring.h>
#include <tupperware/keeper.h>
#include <tupperware/tupsort.h>

// SyncUtils
#include <SyncUtils/SyncDBFactory.h>
#include <SyncUtils/SyncDBBase.h>
#include <SyncUtils/SyncTreeManager.h>
#include <SyncUtils/SyncTreeFactory.h>
#include <SyncUtils/SyncTreeNodeFile.h>
#include <SyncUtils/SyncTreeNodeFolder.h>
#include <SyncUtils/SyncTreeNodeFlags.h>
#include <SyncUtils/TreeFactory.h>
#include <SyncUtils/TreeNodeBase.h>
#include <SyncUtils/TreeNodeFolder.h>
#include <SyncUtils/TreeNodeFile.h>
#include <SyncUtils/TreeIterator.h>
#include <SyncUtils/LoadDBFeedback.h>
#include <SyncUtils/SyncFeedback.h>
#include <SyncUtils/DumpTree.h>
#include <SyncUtils/AutoExclude.h>
#include <SyncUtils/ClientDataManager.h>
#include <SyncUtils/UpdateDuplicates.h>
#include <SyncUtils/LogFile.h>

// Paintlib
#include <PaintLib/PaintLib/Common/planybmp.h>
#include <PaintLib/PaintLib/Common/planydec.h>
#include <PaintLib/PaintLib/Common/plbitmap.h>
#include <PaintLib/PaintLib/Common/config.h>
#include <PaintLib/PaintLib/Common/pldatasrc.h>
#include <PaintLib/PaintLib/Common/plexcept.h>
#include <PaintLib/PaintLib/Common/plpicdec.h>
#include <PaintLib/PaintLib/win/paintlib/plwinbmp.h>
#include <Paintlib/Paintlib/common/filter/PLFilterQuantize.h>

// Perforce API
#include <PerforceApi/include/clientapi.h>

#ifndef _CONSOLE
#include "AssetManager.h"
#include "resource.h"
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
