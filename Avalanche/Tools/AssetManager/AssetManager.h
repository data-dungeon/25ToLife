//-----------------------------------------------------------------------------
//	File:		AssetManager.h
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

#ifndef __ASSETMANAGER__
#define __ASSETMANAGER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//-----------------------------------------------------------------------------
// Includes:

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols



//-----------------------------------------------------------------------------
// CAssetManagerApp class declaration:

class CAssetManagerApp : public CWinApp
{
	class CommandLineOptions
	{
	public:
		enum Flags
		{
			FLAGS_SET_FOLDER_SET					= 0x0001,
			FLAGS_SET_SYNC_STYLE					= 0x0002,
			FLAGS_SET_COPY_TO_RIGHT				= 0x0004,
			FLAGS_SET_COPY_TO_LEFT				= 0x0008,
			FLAGS_SET_DELETE_FROM_LEFT			= 0x0010,
			FLAGS_SET_DELETE_FROM_RIGHT		= 0x0020,
		};
		int m_flags;
		TupString m_folderSetName;
		int m_syncStyle;
	};
	CommandLineOptions m_commandLineOptions;
public:
	// Constructors and destructors:
	CAssetManagerApp();

	bool m_bAccelEnable;
	CWnd	  *m_pAccelTarget;	 // window for accelerator
	HACCEL  m_mainAccelTable;   // Handle to main app accelerator table.
	HACCEL  m_updateDuplicatesAccelTable;   // Handle to update duplicates accelerator table.

	enum AccelType
	{
		ACCEL_MAIN,
		ACCEL_UPDATE_DUPLICATES,
	};

	AccelType m_accelType;

	HANDLE m_hSyncClientDatabasesMutex;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssetManagerApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
	//}}AFX_VIRTUAL

	// Implementation

	//{{AFX_MSG(CAssetManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void RunAsGUI(void);
	void CheckCommandLine(void);
public:
	virtual int ExitInstance();
};

extern CAssetManagerApp theApp;

//-----------------------------------------------------------------------------

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif
