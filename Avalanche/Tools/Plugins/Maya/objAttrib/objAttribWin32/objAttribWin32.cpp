/****************************************************************************

	$Workfile: objAttribWin32.cpp $

	$Archive: /Avalanche/tools/Plugins/Maya/objAttrib/objAttribWin32/objAttribWin32.cpp $

	$Author: Adam Clayton $

	$Date: 12/16/02 9:33a $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// objAttribWin32.cpp : Defines the initialization routines for the DLL.

// precomp'ed header files
#include "StdAfx.h"

// volitile local header files
#include "objAttribWin32.h"
#include "objAttribContainerWnd.h"
#include "SetupCustomControls.h"
#include "UndoRedoState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------
// the one and only ObjAttribWin32App object
ObjAttribWin32App theApp;

// global objects
ObjAttribContainerWnd* g_objAttribContainerWnd = NULL;

HINSTANCE hInstance; // global HINSTANCE

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

//------------------------------------------------------------------------
// ObjAttribWin32App
BEGIN_MESSAGE_MAP(ObjAttribWin32App, CWinApp)
	//{{AFX_MSG_MAP(ObjAttribWin32App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------
ObjAttribWin32App::ObjAttribWin32App()
{

}

//------------------------------------------------------------------------
BOOL ObjAttribWin32App::InitInstance() 
{	
	hInstance = AfxGetInstanceHandle(); // set global hInstance

	SetupCustomControls::Init(hInstance); // Initialize my custom controls
	InitCommonControls();			// Initialize Win95 controls

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex); 

	
	return CWinApp::InitInstance();
}

//------------------------------------------------------------------------
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
// function for enumerating windows to search for the 
// the maya pane "dummy window"
//
//
{
	char str[1024];
	::GetWindowText(hwnd, str, 1024);
	if(strcmp(str, "dummy control") == 0)
	{
		*(HWND*)lParam = hwnd;
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------------
// --                     EXPORTED DLL FUNCTIONS                        --
//------------------------------------------------------------------------
void DllExport CleanUp(void)
// - This is called by the main Maya MLL to perform clean-up when the
//   Maya MLL is uninitialized
//
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(g_objAttribContainerWnd != NULL)
	{
		// destroy the container window
		ObjAttribContainerWnd::DestroySubClassedWindow(g_objAttribContainerWnd);
		g_objAttribContainerWnd = NULL;
	}
}

//------------------------------------------------------------------------
MStatus DllExport ExecuteCommand(const MArgDatabase& argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theObjectAttributes.ExecuteCommand(argDatabase,resultStrings,undoRedoState);
}

void DllExport RefreshWindow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	theObjectAttributes.RefreshWindow();
}



