#pragma warning(disable:4786)

// these need to be added to the objective toolkit stdafx.h file in order to work
#define WINVER		 0x0501
#define _WIN32_WINNT 0x0501
#define	_WIN32_IE	 0x0560

//#define _AFXEXT

//#define CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

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

#include "Max.h"
#include <Plugins/Libraries/KeyVal/resource.h>
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "utilapi.h"
#include <meshdlib.h>
#include <iparamb2.h>
#include <modstack.h>

#include "MAX_MemDirect.h"


// Tupperware
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
#include <Plugins/Libraries/UIDisplay/UIController.h>
#include <Plugins/Libraries/UIDisplay/ExclusiveController.h>
#include <Plugins/Libraries/UIDisplay/FuncPtrController.h>
#include <Plugins/Libraries/UIDisplay/Registry.h>
#include <Plugins/Libraries/UIDisplay/CurrAttribBase.h>
#include <Plugins/Libraries/UIDisplay/UIDisplayBase.h>

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

#include "resource.h"
#include "RegistryKeys.h"
#include "FilePath.h"

#include "define.h"

//#include "ModData.h"
//#include "FaceBase.h"
//#include "PerFaceDataMod.h"
//#include "restore.h"
//#include "PaintMouseProc.h"
//#include "PaintMode.h"
//#include "veroverrides.h"

#ifndef STRICT
#define STRICT
#endif
