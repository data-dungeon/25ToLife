// KB  3/8/00  \Ken85\Plugin\IE\AV3\IE.cpp
//

#include "stdafx.h"
#include "IE.h"
#include "Localize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIEApp

BEGIN_MESSAGE_MAP(CIEApp, CWinApp)
	//{{AFX_MSG_MAP(CIEApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIEApp construction

CIEApp::CIEApp()
{
   m_iemp.m_isbuildmaparray = TRUE;
   m_iemp.m_isbuildattrarray = TRUE;
   m_iemp.m_isbuildmaterialarray = TRUE;
   m_iemp.m_isbuildshaderarray = TRUE;
   m_iemp.m_isbuildbonelist = TRUE;
   m_progressbarhandle = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIEApp object

CIEApp theApp;

BOOL CIEApp::InitInstance() 
{
#ifndef _MAC
   SetRegistryKey( "Hash, Inc." );
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup("AM Plugins");
#endif   

   SetAppLocalizationDLL(m_hInstance); // looks for AV3.lng (resource only dll) and sets the resource to it.
	return CWinApp::InitInstance();
}



