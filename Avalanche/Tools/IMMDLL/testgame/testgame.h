// testgame.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CtestgameApp:
// See testgame.cpp for the implementation of this class
//

class CtestgameApp : public CWinApp
{
public:
	CtestgameApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CtestgameApp theApp;