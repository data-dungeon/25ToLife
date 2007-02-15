// TouchFiles.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CTouchFilesApp:
// See TouchFiles.cpp for the implementation of this class
//

class CTouchFilesApp : public CWinApp
{
public:
	CTouchFilesApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTouchFilesApp theApp;