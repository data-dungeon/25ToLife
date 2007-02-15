/**********************************************************************
 *<
	FILE: DllEntry.cpp

	DESCRIPTION: Contains the Dll Entry stuff

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/
#include "ObjectAttributesPCH.h"
#include "ObjectAttributes.h"
#include "SetupCustomControls.h"
#include "DllEntry.h"
using namespace AttrControls;


extern ClassDesc2* GetObjectAttributesDesc();

HINSTANCE hInstance;
int controlsInit = FALSE;

//------------------------------------------------------------------------
// the one and only ObjectAttributesApp object
ObjectAttributesApp theApp;

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
// ObjectAttributesApp
BEGIN_MESSAGE_MAP(ObjectAttributesApp, CWinApp)
	//{{AFX_MSG_MAP(ObjectAttributesApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------
ObjectAttributesApp::ObjectAttributesApp()
{

}

//------------------------------------------------------------------------
BOOL ObjectAttributesApp::InitInstance() 
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


// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.
#if 0
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.

	if (!controlsInit) 
	{
		controlsInit = TRUE;
//		InitCustomControls(hInstance);	// Initialize MAX's custom controls (soon to be gone)
		SetupCustomControls::Init(hInstance); // Initialize my custom controls
		INITCOMMONCONTROLSEX ctlex;
		ctlex.dwSize = sizeof(ctlex);
		ctlex.dwICC = ICC_BAR_CLASSES|ICC_LISTVIEW_CLASSES|ICC_TREEVIEW_CLASSES;
		InitCommonControlsEx(&ctlex);	// Initialize controls
	}
			
	return (TRUE);
}
#endif

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

// This function returns the number of plug-in classes this DLL
//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
	return 1;
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i) {
		case 0: return GetObjectAttributesDesc();
		default: return 0;
	}
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

