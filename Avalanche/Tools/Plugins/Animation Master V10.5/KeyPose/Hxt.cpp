// KB  11/1/01  \Ken90\Plugin\HXT\Duplicator\Hxt.cpp
//

#include "stdafx.h"
#include "Hxt.h"
#include "Localize.h"
#include "sdk/HPropert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHxtApp

BEGIN_MESSAGE_MAP(CHxtApp, CWinApp)
	//{{AFX_MSG_MAP(CWizardApp)
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
// The one and only CWizardApp object

CHxtApp theApp;

BOOL CHxtApp::InitInstance() 
{
#ifndef _MAC
   SetRegistryKey( "Hash, Inc." );
 #ifndef _DEBUG
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup("AM Plugins");
 #endif
#endif   
   SetAppLocalizationDLL(m_hInstance);

   AllocPropertyInfos();

	return CWinApp::InitInstance();
}

int CHxtApp::ExitInstance() 
{
   FreePropertyInfos();

   return CWinApp::ExitInstance();
}

void CHxtApp::AllocPropertyInfos()
{
   CString string;
   
   string.LoadString(IDS_AVALANCHE);
   m_avalanchepropertyinfo = HPropertyInfo::New(string, "Avalanche");

      string.LoadString(IDS_KEYPOSEDATA);
      m_datapropertyinfo = HDataPropertyInfo::New(string, "KeyPoseData");
      m_datapropertyinfo->SetSaveProperty(FALSE);
      m_datapropertyinfo->SetConstant(TRUE);
}

void CHxtApp::FreePropertyInfos()
{
   //Model Properties
   delete m_avalanchepropertyinfo;
      delete m_datapropertyinfo;
}
