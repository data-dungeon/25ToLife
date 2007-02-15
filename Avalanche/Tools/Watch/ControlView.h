#if !defined(AFX_CONTROLVIEW_H__D3E8B8F6_7875_4EC5_BE7C_70527DE509AF__INCLUDED_)
#define AFX_CONTROLVIEW_H__D3E8B8F6_7875_4EC5_BE7C_70527DE509AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CWatchView;

class CControlView : public CFormView
{
protected:
	CControlView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CControlView)

// Form Data
public:
	//{{AFX_DATA(CControlView)
	enum { IDD = IDD_CONTROL_PAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CControlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CControlView)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnConnectPc();
	afx_msg void OnConnectPlaystation();
	afx_msg void OnConnectXbox();
	afx_msg void OnViewDebugoutput();
	afx_msg void OnViewPlotdialog();
	afx_msg void OnViewStripchart();
	afx_msg void OnUpdateViewDebugoutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPlotdialog(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewStripchart(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveall();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnContextRefresh();
	afx_msg void OnSelchangeList();
	afx_msg void OnRowNewBtn();
	afx_msg void OnRowFillBtn();
	afx_msg void OnColumnNewBtn();
	afx_msg void OnColumnFillBtn();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CWatchView;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLVIEW_H__D3E8B8F6_7875_4EC5_BE7C_70527DE509AF__INCLUDED_)
