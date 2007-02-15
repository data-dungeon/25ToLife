// WinUpdater.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// Just to make the names reasonable...
#define IDB_RELAUNCH_GAME IDOK
#define IDB_CLOSE IDCANCEL

// CWinUpdaterApp:
// See WinUpdater.cpp for the implementation of this class
//

class CWinUpdaterApp : public CWinApp
{
public:
	CWinUpdaterApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWinUpdaterApp theApp;