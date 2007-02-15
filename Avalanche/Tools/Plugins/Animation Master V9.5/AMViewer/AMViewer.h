// Dan  4/11/00  \Dan85\AMViewer\AMViewer.h
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#ifdef _MAC
#define NATIVESEPARATOR ':'
#else
#define NATIVESEPARATOR '\\' 
#endif

/////////////////////////////////////////////////////////////////////////////
// CAMViewerApp:
// See AMViewer.cpp for the implementation of this class
//

class CAMViewerApp : public CWinApp
{
public:
   CAMViewerApp();
   CDocument *m_doc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMViewerApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAMViewerApp)
	afx_msg void OnAppAbout();
   afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
