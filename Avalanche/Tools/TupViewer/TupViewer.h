// TupViewer.h : main header file for the TupViewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//
// Prof-UIS command manager profile name
//
#define __PROF_UIS_PROJECT_CMD_PROFILE_NAME _T("CTupViewerApp-command-manager-profile")

#define __PROF_UIS_PROJECT_DLG_PERSIST_REG_KEY _T("CTupViewerApp-resizable-dialog-positions")


// CTupViewerApp:
// See TupViewer.cpp for the implementation of this class
//

class CTupViewerApp : public CWinApp
{
public:
	CTupViewerApp();
	~CTupViewerApp();


// Overrides
public:
	TupperwareAggregate *m_pTupRoot;
	TupArray<TupperwareReader::FileBuffer> *m_pTupBuffers;
	TupperwareAtom *m_pSelectedAtom;

	void RefreshAll(void);
	void RefreshSelected(int windowFlags,void *pData);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTupViewerApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//
	// Prof-UIS advanced options
   //
	void SetupUiAdvancedOptions();

	//{{AFX_MSG(CTupViewerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	HMENU m_hMenu;
	HACCEL m_hAccel;

};

extern CTupViewerApp theApp;