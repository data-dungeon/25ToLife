// DiffPatch.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CDiffPatchApp:
// See DiffPatch.cpp for the implementation of this class
//

class CDiffPatchApp : public CWinApp
{
public:
	CDiffPatchApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDiffPatchApp theApp;