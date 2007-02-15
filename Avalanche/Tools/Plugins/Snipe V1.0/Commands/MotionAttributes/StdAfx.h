
#define __STDAFX_H_
#pragma once

#pragma warning (disable : 4786)

#define WINVER		 0x0501
#define _WIN32_WINNT 0x0501
#define	_WIN32_IE	 0x0560

//#define _AFXEXT

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define ISOLATION_AWARE_ENABLED	1

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxpriv.h>

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

extern const char *g_pBuildDate;
extern const char *g_pBuildTime;
extern const char *g_pBuildVersion;

#ifndef STRICT
#define STRICT
#endif

// windows header files
#include <windows.h>
#include <comdef.h>
#include <direct.h>
//#include <tchar.h>

// stl header files
#include <string>
#include <vector>

// Snipe header files
#define catch_msg
#include "../Utility/RGBColor.h"
#include "SDKMacros.h"
#include "../Utility/AString.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HComponent.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HCommandPluginHelper.h"
#include "HHierObject.h"
#include "HStringProperty.h"



// Tupperware header files
#include <Tupperware/Atom.h>
#include <Tupperware/Aggregate.h>
#include <Tupperware/Scalar.h>
#include <Tupperware/List.h>
#include <Tupperware/loadsave.h>
#include <Tupperware/TupString.h>
#include <Tupperware/TupArray.h>
#include <Tupperware/Hash.h>
#include <Tupperware/Keeper.h>
#include <Tupperware/tupSort.h>

// UIDisplay header files
#include <Plugins/Libraries/UIDisplay/ConfigRoot.h>
#include <Plugins/Libraries/UIDisplay/ConfigCheckList.h>
#include <Plugins/Libraries/UIDisplay/UIController.h>
#include <Plugins/Libraries/UIDisplay/ExclusiveController.h>
#include <Plugins/Libraries/UIDisplay/FuncPtrController.h>
#include <Plugins/Libraries/UIDisplay/Registry.h>
#include <Plugins/Libraries/UIDisplay/CurrAttribBase.h>
#include <Plugins/Libraries/UIDisplay/UIDisplayBase.h>
#include <Plugins/Libraries/UIDisplay/KeyValue.h>
#include <Plugins/Libraries/UIDisplay/KeySelection.h>
#include <Plugins/Libraries/UIDisplay/KeyValueContext.h>

// AttrControls
#include <plugins/libraries/attrcontrols/CustomControls.h>
#include <Plugins/Libraries/AttrControls/WinUtil.h>
#include <plugins/libraries/attrcontrols/ICustomRollupWindow.h>
#include <plugins/libraries/attrcontrols/ICustomRollupPanel.h>
#include <Plugins/Libraries/AttrControls/DebugPrint.h>
#include <Plugins/Libraries/AttrControls/ICustomEditBox.h>
#include <Plugins/Libraries/AttrControls/ICustomSlider.h>
#include <Plugins/Libraries/AttrControls/ICustomSpinner.h>
#include <Plugins/Libraries/AttrControls/ICustomToolTip.h>

// KeyVal
#include <Plugins/Libraries/KeyVal/KeyValPanelController.h>
#include <Plugins/Libraries/KeyVal/KeyValCommunicatorBase.h>
#include <Plugins/Libraries/KeyVal/resource.h>

#include "MotionAttributesPlugin.h"
#include "resource.h"
#include "FilePath.h"
#include "RegistryKeys.h"

#include "UndoRedoState.h"
#include "CommandArgs.h"

#define BEGIN_BLOCK	"BEGIN MotionAttributes"
#define END_BLOCK	"END MotionAttributes"

#define CFG_DEFAULT_NAME _T("ObjAttr")	
#define CFG_DEFAULT_FILENAME _T("plug-ins\\ObjAttr.cfg")	

#ifdef _DEBUG
#define SNEW new (__FILE__, __LINE__)
#else
#define SNEW new
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

