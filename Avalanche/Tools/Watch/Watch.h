// Watch.h : main header file for the WATCH application
//

#if !defined(AFX_WATCH_H__53F505CB_93D7_4FE7_9004_49993F43490E__INCLUDED_)
#define AFX_WATCH_H__53F505CB_93D7_4FE7_9004_49993F43490E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

extern char* remoteMachine;


#define WM_CANCEL_DIALOG					 (WM_USER + 1)
#define WM_STRIPDLG_DONE					 (WM_USER + IDD__STRIPCHART_DIALOG)
#define WM_PRINTDLG_DONE					 (WM_USER + IDD_TEXTOUT_DIALOG)
#define WM_PLOTDLG_DONE						 (WM_USER + IDD_PLOT_DIALOG)


/////////////////////////////////////////////////////////////////////////////
// CWatchApp:
// See Watch.cpp for the implementation of this class
//

class CWatchApp : public CWinApp
{
public:
	CWatchApp();
	~CWatchApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWatchApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCH_H__53F505CB_93D7_4FE7_9004_49993F43490E__INCLUDED_)
