#if !defined(AFX_SOLUTIONEXPLORERPANE_H__5FFD8D7B_C89F_48B8_A003_5425148766C2__INCLUDED_)
#define AFX_SOLUTIONEXPLORERPANE_H__5FFD8D7B_C89F_48B8_A003_5425148766C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SolutionExplorerPane.h : header file
//

#include "DockPaneWnd.h"
/////////////////////////////////////////////////////////////////////////////
// CSolutionExplorerPane window

class CObjectExplorerPane : public CWnd
{
// Construction
public:
	CObjectExplorerPane();

// Attributes
public:
	CDockPaneWnd<CTreeCtrl> m_wndObjectExplorerView;
	CImageList m_ilSolutionView;
	CXTPToolBar m_wndToolBar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSolutionExplorerPane)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CObjectExplorerPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSolutionExplorerPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void OnEmptyCommand(UINT);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOLUTIONEXPLORERPANE_H__5FFD8D7B_C89F_48B8_A003_5425148766C2__INCLUDED_)
