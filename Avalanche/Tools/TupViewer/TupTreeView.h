// TupTreeView.h : header file
//

#ifndef TUP_TREE_VIEW_H
#define TUP_TREE_VIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef TUP_VIEWER_TREE_CTRL
#include "TupViewerTreeCtrl.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// TupTreeView dialog

class TupTreeView : public CExtResizableDialog
{
	// Construction
public:
	TupTreeView(CWnd* pParent = NULL);   // standard constructor

	void Refresh(void);

	// Dialog Data
	//{{AFX_DATA(TupTreeView)
	enum { IDD = IDD_TREEVIEW };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CExtWFF < TupViewerTreeCtrl > m_wndTree;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TupTreeView)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TupTreeView)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSeProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTreeviewTree(NMHDR *pNMHDR, LRESULT *pResult);

	void UpdateTreeItem(HTREEITEM hItem);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROFSTUDIOSOLUTIONEXPLORER_H__9018E2C0_46D5_4833_9E6B_BD58B521BB3A__INCLUDED_)
