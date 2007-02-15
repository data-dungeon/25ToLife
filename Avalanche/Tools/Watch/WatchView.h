// WatchView.h : interface of the CWatchView class
//
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined(AFX_WATCHVIEW_H__91DC07DC_D788_4A32_8E9D_6D8A94AE9307__INCLUDED_)
#define AFX_WATCHVIEW_H__91DC07DC_D788_4A32_8E9D_6D8A94AE9307__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WatchSlider.h"
#include "PrintDlg.h"
#include "StripDlg.h"
#include "PlotDlg.h"
#include "Context.h"

typedef CArray<CWnd*, CWnd*> CWndArray;

class CControlView;

class CWatchView : public CFormView
{
protected: // create from serialization only
	CWatchView();
	DECLARE_DYNCREATE(CWatchView)

public:
	//{{AFX_DATA(CWatchView)
	enum { IDD = IDD_WATCH_FORM };
	CTabCtrl	m_tab;
	CWatchSlider	m_slider1;
	//}}AFX_DATA

// Attributes
public:
	CWatchDoc* GetDocument();

// Operations
public:
	int GetContext(char* context);
	void RegisterContext(CONTEXT_INFO&);
	void DebugPrint(char *, ...);
	void RegisterVariable(VAR_INFO&);
	void RegisterTableVariable(VAR_INFO&);
	void AddVarWatch(VAR_ADD_WATCH& vaw) {stripDlg->AddVarWatch(vaw);}
	void UpdateVarWatch(VAR_UPDATE_WATCH& vuw) {stripDlg->UpdateVarWatch(vuw);}
	void AddXY(VAR_ADD_XY& vaxy) {plotDlg->AddXY(vaxy);}
	void Clear() {plotDlg->Clear();}
	void RefreshVariable(VAR_INFO&);
	void RegisterLabel(LABEL_INFO&);
	void TableData(TABLE_DATA&);
	void OnListButton(int idx)
	{
		ASSERT(contexts.GetSize() > m_tab.GetCurSel());
		contexts[m_tab.GetCurSel()]->OnListButton(idx);
	}

	CPlotDlg* PlotDlg() {return plotDlg;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWatchView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int mouseButton;
	ContextArray contexts;
	CRect sliderRect;

	CStripDlg* stripDlg;
	CPlotDlg* plotDlg;
	CPrintDlg* printDlg;

//	CControlView* controlView;
	CWnd* controlView;

// Generated message map functions
protected:
	//{{AFX_MSG(CWatchView)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnConnectPc();
	afx_msg void OnConnectPlaystation();
	afx_msg void OnConnectXbox();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnViewDebugoutput(bool = true);
	afx_msg void OnUpdateViewDebugoutput(CCmdUI* pCmdUI);
	afx_msg void OnViewStripchart(bool = true);
	afx_msg void OnUpdateViewStripchart(CCmdUI* pCmdUI);
	afx_msg void OnViewPlotdialog(bool = true);
	afx_msg void OnUpdateViewPlotdialog(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveall();
	afx_msg void OnFileOpen();
	afx_msg void OnContextRefresh();
	afx_msg void OnSelchangeList();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CControlView;
};

#ifndef _DEBUG  // debug version in WatchView.cpp
inline CWatchDoc* CWatchView::GetDocument()
   { return (CWatchDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHVIEW_H__91DC07DC_D788_4A32_8E9D_6D8A94AE9307__INCLUDED_)
