// ProcessWatchDog.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ProcessWatchDog.h"
#include "ProcessWatchDogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProcessWatchDogApp

BEGIN_MESSAGE_MAP(CProcessWatchDogApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CProcessWatchDogApp construction

CProcessWatchDogApp::CProcessWatchDogApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CProcessWatchDogApp object

CProcessWatchDogApp theApp;


// CProcessWatchDogApp initialization

BOOL CProcessWatchDogApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	AfxInitRichEdit();			// So we can use righ edit controls

	// Load Icons for Tray 
	m_pIconList[0] = LoadIcon (MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIconList[1] = LoadIcon (MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIconList[2] = LoadIcon (MAKEINTRESOURCE(IDR_MAINFRAME));

	CWinApp::InitInstance();

	m_pMainWnd = new CProcessWatchDogDlg;

	if (m_pMainWnd)
		return TRUE;
	else
		return FALSE;
}
