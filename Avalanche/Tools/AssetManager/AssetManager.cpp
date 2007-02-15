//-----------------------------------------------------------------------------
//	File:		AssetManager.cpp
//	Created:	Sep 13, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CAssetManagerApp class defines behavior for the application as
//			a whole.
//
//	History:	9/13/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#ifndef __ASSETMANAGER__
#include "AssetManager.h"
#endif
#ifndef __ASSETMANAGERDLG__
#include "AssetManagerDlg.h"
#include ".\assetmanager.h"
#endif



//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//-----------------------------------------------------------------------------
// CAssetManagerApp class definition:


//-----------------------------------------------------------------------------
// Message map:

BEGIN_MESSAGE_MAP(CAssetManagerApp, CWinApp)
	//{{AFX_MSG_MAP(CAssetManagerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


//-----------------------------------------------------------------------------
// CAssetManagerApp::CAssetManagerApp
// This method is the default constructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
CAssetManagerApp::CAssetManagerApp()
{
	m_bAccelEnable = false;
	m_pAccelTarget = NULL;
	m_mainAccelTable = 0;
	m_updateDuplicatesAccelTable = 0;
	m_accelType = ACCEL_MAIN;
	m_hSyncClientDatabasesMutex = NULL;
}

//-----------------------------------------------------------------------------
// The one and only CAssetManagerApp object:
CAssetManagerApp theApp;

//-----------------------------------------------------------------------------
// CAssetManagerApp::InitInstance
// This method will handle initialization of any instances of the app.
//
// Input: None.
//
// Output: TRUE if initialization was successful, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL CAssetManagerApp::InitInstance()
{
#define WSA_VERSION  MAKEWORD(2,0)
	WSADATA		WSAData = { 0 };
	if ( 0 != WSAStartup( WSA_VERSION, &WSAData ) )
	{
		// Tell the user that we could not find a usable
		// WinSock DLL.
		if ( LOBYTE( WSAData.wVersion ) != LOBYTE(WSA_VERSION) ||
			HIBYTE( WSAData.wVersion ) != HIBYTE(WSA_VERSION) )
			::MessageBox(NULL, _T("Incorrect version of WS2_32.dll found"), _T("Error"), MB_OK);

		WSACleanup( );
		return FALSE;
	}

   CheckCommandLine();

	AfxEnableControlContainer();
	m_hSyncClientDatabasesMutex = CreateMutex(NULL,FALSE,"AssetManagerClientDatabaseMutex");

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#if 0
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif
	AfxInitRichEdit();			// So we can use righ edit controls

	m_mainAccelTable = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAIN));
	m_updateDuplicatesAccelTable = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_UPDATE_DUPLICATES));

	CAssetManagerDlg dlg;
	if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_FOLDER_SET)
	{
		dlg.SetFolderSetToLoad(m_commandLineOptions.m_folderSetName);
	}

	bool bSetCopyToLeft = false;
	bool bSetCopyToRight = false;
	bool bSetDeleteFromLeft = false;
	bool bSetDeleteFromRight = false;
	bool setSync = false;
	if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_COPY_TO_LEFT)
	{
		bSetCopyToLeft = true;
	}
	if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_COPY_TO_RIGHT)
	{
		bSetCopyToRight = true;
	}
	if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_DELETE_FROM_LEFT)
	{
		bSetDeleteFromLeft = true;
	}
	if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_DELETE_FROM_RIGHT)
	{
		bSetDeleteFromRight = true;
	}
	dlg.SetAutoPressButtons(bSetCopyToRight,bSetCopyToLeft,bSetDeleteFromRight,bSetDeleteFromLeft,false);

	if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_SYNC_STYLE)
	{
		dlg.SetSyncSyleToUse(m_commandLineOptions.m_syncStyle);
	}

	m_pAccelTarget = &dlg;
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

BOOL CAssetManagerApp::ProcessMessageFilter(int code, LPMSG lpMsg) 
{
	if (code < 0)
		CWinApp::ProcessMessageFilter(code, lpMsg);
	
	if (m_bAccelEnable && ::IsWindow(m_pAccelTarget->m_hWnd))
	{
		switch (m_accelType)
		{
			case ACCEL_MAIN:
				if (m_mainAccelTable)
				{
					if (::TranslateAccelerator(m_pAccelTarget->m_hWnd, m_mainAccelTable, lpMsg))
						return(TRUE);
				}
				break;
			case ACCEL_UPDATE_DUPLICATES:
				if (m_updateDuplicatesAccelTable)
				{
					if (::TranslateAccelerator(m_pAccelTarget->m_hWnd, m_updateDuplicatesAccelTable, lpMsg))
						return(TRUE);
				}
				break;
		};
	}
	return CWinApp::ProcessMessageFilter(code, lpMsg);
}

void CAssetManagerApp::CheckCommandLine(void)
{
	m_commandLineOptions.m_flags = 0;
	int argc = __argc;
	char ** argv = __argv;

	if (__argc>1)
	{
		m_commandLineOptions.m_folderSetName = __argv[1];
		m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_FOLDER_SET;
	}
	for (int i = 2; i < __argc; i++)
	{
		TupString optionString(__argv[i]);
		if (!stricmp(optionString,"/S"))
		{
			i++;
			if (i<__argc)
			{
				if (!stricmp(__argv[i],"Normal Sync"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_2_WAY;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Augment Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_AUGMENT_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Augment Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_AUGMENT_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Refresh Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_REFRESH_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Refresh Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_REFRESH_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Update Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_UPDATE_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Update Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_UPDATE_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Update and Prune Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_UPDATE_AND_PRUNE_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Update and Prune Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Force Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_FORCE_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Force Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_FORCE_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
			}
		}
		else if (!stricmp(optionString,"/CR"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_COPY_TO_RIGHT;
		}
		else if (!stricmp(optionString,"/CL"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_COPY_TO_LEFT;
		}
		else if (!stricmp(optionString,"/DR"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_DELETE_FROM_RIGHT;
		}
		else if (!stricmp(optionString,"/DL"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_DELETE_FROM_LEFT;
		}
	}
}


int CAssetManagerApp::ExitInstance()
{
	// Terminate use of the WS2_32.DLL
	WSACleanup();

	CloseHandle(m_hSyncClientDatabasesMutex);

	return CWinApp::ExitInstance();
}
