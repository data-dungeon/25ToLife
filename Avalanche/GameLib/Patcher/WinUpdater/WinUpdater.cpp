// WinUpdater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WinUpdater.h"
#include "WinUpdaterDlg.h"
#include "UpdaterCommandLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinUpdaterApp

BEGIN_MESSAGE_MAP(CWinUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWinUpdaterApp construction

CWinUpdaterApp::CWinUpdaterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWinUpdaterApp object

CWinUpdaterApp theApp;


// CWinUpdaterApp initialization

BOOL CWinUpdaterApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	// Get the command line and parse it
	UpdaterCommandLine cmdLine;
	ParseCommandLine(cmdLine);

	// Prepare our dialog box
	CWinUpdaterDlg dlg;
	m_pMainWnd = &dlg;

	// Copy the command line info into the dialog
	dlg.SetCommandLineInfo(cmdLine);

	// Run the dialog box
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDB_RELAUNCH_GAME)
	{
		if (cmdLine.m_gameToLaunch.size())
		{
			// Relaunch game
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			memset(&si, 0, sizeof(si));
			memset(&pi, 0, sizeof(pi));
			si.cb = sizeof(si);

			std::string parms = cmdLine.m_gameToLaunch;
			if (cmdLine.m_launchParameters.size())
				parms += " " + cmdLine.m_launchParameters;
			if (::CreateProcess((LPTSTR)cmdLine.m_gameToLaunch.c_str(),
								(LPTSTR)parms.c_str(),
								NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				// Just exit, don't need to wait for anything.
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			else
			{
				// Should notify user here...
			}
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
