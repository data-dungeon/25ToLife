// KB  11/1/01  \Ken90\Plugin\HXT\Duplicator\Hxt.cpp
//

#include "stdafx.h"
#include "Hxt.h"
#include "MirrorBonesDialog.h"
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

BOOL CHxtApp::OnMirrorBones(HTreeObject *htreeobject)
{ 
	MirrorBonesDialog dlg;

	int command = dlg.DoModal();

	if (command==IDCANCEL)
		return TRUE;

	if (dlg.m_adjustright)
		m_adjustright = true;
	else
		m_adjustright = false;
	
	if (dlg.m_assignvertices)
		m_assignvertices = true;
	else
		m_assignvertices = false;

	if (dlg.m_createbones)
		m_createbones = true;
	else
		m_createbones = false;

	if (dlg.m_generatelogfile)
		m_generatelogfile = true;
	else
		m_generatelogfile = false;
   
	m_tolerance = dlg.m_tolerance;

	if (!Evaluate(htreeobject))
		return FALSE;

	return TRUE;
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
