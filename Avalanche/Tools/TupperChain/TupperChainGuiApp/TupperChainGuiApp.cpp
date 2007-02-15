// TupperChainGuiApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TupperChainGuiApp.h"
#include "TupperChainGuiAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTupperChainGuiAppApp

BEGIN_MESSAGE_MAP(CTupperChainGuiAppApp, CWinApp)
	//{{AFX_MSG_MAP(CTupperChainGuiAppApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTupperChainGuiAppApp construction

CTupperChainGuiAppApp::CTupperChainGuiAppApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTupperChainGuiAppApp object

CTupperChainGuiAppApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTupperChainGuiAppApp initialization

BOOL CTupperChainGuiAppApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	AfxInitRichEdit();

	// set executable path
	char programPathBuffer1[_MAX_PATH];
	char programPathBuffer2[_MAX_PATH];
	GetModuleFileName(NULL,programPathBuffer1,_MAX_PATH);
	FilePath::GetPath(programPathBuffer1 ,programPathBuffer2);
	if (programPathBuffer2[strlen(programPathBuffer2)-1]!='\\')
		strcat(programPathBuffer2,"\\");

	CTupperChainGuiAppDlg dlg;
	dlg.SetPluginPath(programPathBuffer2);

	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

