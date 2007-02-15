// KB  11/1/01  \Ken90\Plugin\HXT\Duplicator\Hxt.cpp
//

#include "stdafx.h"
#include "Hxt.h"
#include "TClampDialog.h"
#include "Localize.h"

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

BOOL CHxtApp::OnTextureClamp(HTreeObject *htreeobject)
{
	TClampDialog dlg;

	int command = dlg.DoModal();

	SingleClamp = dlg.SingleClamp;
	MultipleClampStretch = dlg.MultipleClampStretch;
	MultipleClampNoStretch = dlg.MultipleClampNoStretch;

	BlendMapping = dlg.BlendMapping;
	PrimaryMapPath = dlg.PrimaryMapPath;
	SecondaryMapPath = dlg.SecondaryMapPath;
	AlphaMapPath = dlg.AlphaMapPath;

	if (command==IDCANCEL)
		return TRUE;

   return Evaluate(htreeobject);      
}

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
	return CWinApp::InitInstance();
}
