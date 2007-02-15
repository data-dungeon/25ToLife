#if !defined(AFX_OUTPUTPANE_H__C81C6B0E_433E_46A5_9598_EFBD9CD6A93E__INCLUDED_)
#define AFX_OUTPUTPANE_H__C81C6B0E_433E_46A5_9598_EFBD9CD6A93E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputPane.h : header file
//
#include "DockPaneWnd.h"
/////////////////////////////////////////////////////////////////////////////
// COutputPane window

class COutputPane : public CWnd
{
// Construction
public:
	COutputPane();

// Attributes
public:
	CDockPaneWnd<CEdit> m_wndOutput;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputPane)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTPANE_H__C81C6B0E_433E_46A5_9598_EFBD9CD6A93E__INCLUDED_)
