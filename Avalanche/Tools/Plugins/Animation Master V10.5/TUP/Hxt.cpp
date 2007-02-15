// Dan  3/24/2003  \DanLP105\Plugin\HXT\Ply\Hxt.cpp
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
   m_iemp.m_isbuildmaparray = TRUE;
   m_iemp.m_isbuildattrarray = TRUE;
   m_iemp.m_isbuildbonelist = TRUE;
   m_hprogressbar = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHxtApp object

CHxtApp theApp;

BOOL CHxtApp::InitInstance() 
{
#ifndef _OS9
   SetRegistryKey( "Hash, Inc." );
#ifndef _DEBUG
	free((void*)m_pszProfileName);
#endif
	m_pszProfileName = _tcsdup("AM Plugins");
#endif   

   SetAppLocalizationDLL(m_hInstance); // looks for PLY.lng (resource only dll) and sets the resource to it.
	return CWinApp::InitInstance();
}



