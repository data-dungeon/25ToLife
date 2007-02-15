// Dan  3/24/2003  \DanLP105\Plugin\HXT\Ply\Hxt.cpp
//

#include "stdafx.h"
#include "BasicNavigatorPlugin.h"

/////////////////////////////////////////////////////////////////////////////
// CSnipePluginApp

BEGIN_MESSAGE_MAP(CSnipePluginApp, CWinApp)
	//{{AFX_MSG_MAP(CSnipePluginApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnipePluginApp construction

CSnipePluginApp::CSnipePluginApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHxtApp object

CSnipePluginApp g_theApp;

BOOL CSnipePluginApp::InitInstance() 
{
   SetRegistryKey( "Avalanche Software" );

	return CWinApp::InitInstance();
}