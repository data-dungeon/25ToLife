// KB  4/25/00  \Ken85\Plugin\Shaders\Anime\Shader.cpp
//

#include "stdafx.h"
#include "Shader.h"
#include "F_Protos.h"
#include "Localize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShaderApp

BEGIN_MESSAGE_MAP(CShaderApp, CWinApp)
	//{{AFX_MSG_MAP(CShaderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardApp construction

CShaderApp::CShaderApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CShaderApp object

CShaderApp theApp;

BOOL CShaderApp::InitInstance() 
{
/*
#ifndef _MAC
   SetRegistryKey( "Hash, Inc." );
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup("AM Plugins");
#endif   

   SetAppLocalizationDLL(m_hInstance); // looks for Shader.lng (resource only dll) and sets the resource to it.
*/
   return CWinApp::InitInstance();
}
