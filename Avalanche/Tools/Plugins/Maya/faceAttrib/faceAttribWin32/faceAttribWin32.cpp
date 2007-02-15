/****************************************************************************

	$Workfile: faceAttribWin32.cpp $

	$Archive: /Avalanche/tools/Plugins/Maya/faceAttrib/faceAttribWin32/faceAttribWin32.cpp $

	$Author: Adam Clayton $

	$Date: 1/03/03 3:40p $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// objAttribWin32.cpp : Defines the initialization routines for the DLL.

// precomp'ed header files
#include "StdAfx.h"

// volitile local header files
#include "faceAttribWin32.h"
#include "faceAttribContainerWnd.h"
#include "SetupCustomControls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------
// the one and only FaceAttribWin32App object
FaceAttribWin32App theApp;

// global objects
//FaceAttribContainerWnd* g_faceAttribContainerWnd = NULL;

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
// FaceAttribWin32App
BEGIN_MESSAGE_MAP(FaceAttribWin32App, CWinApp)
	//{{AFX_MSG_MAP(FaceAttribWin32App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------
FaceAttribWin32App::FaceAttribWin32App()
{

}

//------------------------------------------------------------------------
BOOL FaceAttribWin32App::InitInstance() 
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

//	if(g_faceAttribContainerWnd != NULL)
//	{
//		// destroy the container window
//		FaceAttribContainerWnd::DestroySubClassedWindow(g_faceAttribContainerWnd);
//		g_faceAttribContainerWnd = NULL;
//	}
}

//------------------------------------------------------------------------
MStatus DllExport doIt(const MArgList& args, MStringArray& resultStrings)
// - This is called by the main Maya MLL to perform the "doIt" function
//
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theFaceAttributes.SetupAttributes();
	
/*

	MSyntax syntax;
	// parse the command line arguments
	syntax.addFlag("a", "attach");
	syntax.addFlag("d", "detach");
	MArgParser parser(syntax, args);

	// attach command
	//
	if(parser.isFlagSet("attach"))
	{
		HWND hWnd = NULL;
		EnumChildWindows(::GetDesktopWindow(), EnumChildProc, (long)&hWnd);
		if(hWnd == NULL)
		{
			// houston, we have a problem (i.e. Maya pane not found)
			MGlobal::displayError("can't find window 'dummy control'");
			return MS::kFailure;
		}

		// create a new editor window by sub-classing the HWND 
		g_faceAttribContainerWnd = FaceAttribContainerWnd::CreateSubClassedWindow(hWnd);
		
		// check for failure
		if(g_faceAttribContainerWnd == NULL)
		{
			// houston we have a problem (i.e. unable to dynamically subclass the HWND)
			MGlobal::displayError("unable to create FaceAttribContainerWnd");
			return MS::kFailure;
		}
		resultStrings.append("created and attached FaceAttribContainerWnd");
	}

	// detach command
	//
	else if(parser.isFlagSet("detach"))
	{
		// can only detach if we are attached
		if(g_faceAttribContainerWnd != NULL)
		{
			// destroy the specified window
			FaceAttribContainerWnd::DestroySubClassedWindow(g_faceAttribContainerWnd);
			g_faceAttribContainerWnd = NULL;
			resultStrings.append("detached and destroyed FaceAttribContainerWnd");
		}
	}
	
	// bad command
	//
	else
	{
		// houston we have a problem (i.e. user is a moron; no command specified!)
		MGlobal::displayError("no command specified");

		return MS::kFailure;
	}

*/
	return MS::kSuccess;
}



