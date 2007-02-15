// Dan  12/28/00  \Dan90\Plugin\HXT\KFM\Hxt.cpp
//

#include "stdafx.h"
#include "Hxt.h"
#include "Localize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHxtApp

BEGIN_MESSAGE_MAP(CHxtApp, CWinApp)
	//{{AFX_MSG_MAP(CHxtApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHxtApp construction

CHxtApp::CHxtApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHxtApp object

CHxtApp theApp;

BOOL CHxtApp::InitInstance() 
{
/*
#ifndef _MAC
   SetRegistryKey( "Hash, Inc." );
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup("AM Plugins");
#endif
*/
//   SetAppLocalizationDLL(m_hInstance); // looks for KFM.lng (resource only dll) and sets the resource to it.
	return CWinApp::InitInstance();
}

