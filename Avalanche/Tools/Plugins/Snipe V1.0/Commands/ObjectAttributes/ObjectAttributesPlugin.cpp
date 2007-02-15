#include "stdafx.h"
#include "ObjectAttributesPlugin.h"
#include "SetupCustomControls.h"

/////////////////////////////////////////////////////////////////////////////
// CSnipePluginApp

BEGIN_MESSAGE_MAP(ObjectAttributesPluginApp, CWinApp)
	//{{AFX_MSG_MAP(ObjectAttributesPluginApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnipePluginApp construction

ObjectAttributesPluginApp::ObjectAttributesPluginApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHxtApp object

ObjectAttributesPluginApp theApp;

HINSTANCE hInstance; // global HINSTANCE

BOOL ObjectAttributesPluginApp::InitInstance() 
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