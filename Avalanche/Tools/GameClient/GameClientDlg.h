// GameClientDlg.h : header file
//

#if !defined(AFX_GAMECLIENTDLG_H__8BFD2C51_FB0C_466C_A494_E0EB1FAE0BA4__INCLUDED_)
#define AFX_GAMECLIENTDLG_H__8BFD2C51_FB0C_466C_A494_E0EB1FAE0BA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGameClientDlgAutoProxy;
class CGameClientPane;

/////////////////////////////////////////////////////////////////////////////
// CGameClientDlg dialog

#include "OutputLogPane.h"
#include "FileServerPane.h"

class CGameClientDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameClientDlg);
	friend class CGameClientDlgAutoProxy;

// Construction
public:
	void ResetPanes();
	void ShowPane(int pane);
	CGameClientPane* GetPane(int paneNumber);
	void ShowPane(CDialog* pane);
	void GetReducedClientRect(LPRECT rect);
	int AddPane(CGameClientPane* dlg, CString name);
	CGameClientDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CGameClientDlg();

	virtual void OnOK();
	virtual void OnCancel();

// Dialog Data
	//{{AFX_DATA(CGameClientDlg)
	enum { IDD = IDD_GAMECLIENT_DIALOG };
	CTabCtrl	m_MasterTab;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void KillAllPanes(int startWithPane = 0);
	CTypedPtrArray<CObArray, CDialog*> m_Panes;
	CGameClientDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	//{{AFX_MSG(CGameClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeMastertab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingMastertab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMECLIENTDLG_H__8BFD2C51_FB0C_466C_A494_E0EB1FAE0BA4__INCLUDED_)
