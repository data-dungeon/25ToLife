#ifndef __SYNCDIALOG__
#define __SYNCDIALOG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyncDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SyncDialog dialog

class SyncDialog : public CDialog
{
// Construction
public:
	SyncDialog(CWnd* pParent = NULL);   // standard constructor

	BOOL m_bCloseCheck;
	BOOL m_bExitCheck;
	bool m_bUpdateDuplicates;

// Dialog Data
	//{{AFX_DATA(SyncDialog)
	enum { IDD = IDD_SYNC_DIALOG };
	CButton	m_doneButton;
	CStatic	m_syncText5;
	CStatic	m_syncText4;
	CStatic	m_syncText3;
	CStatic	m_syncText2b;
	CStatic	m_syncText2a;
	CStatic	m_syncText1b;
	CStatic	m_syncText1a;
	CProgressCtrl	m_progress2;
	CProgressCtrl	m_progress1;
	CRichEditCtrl	m_logWindow;
	//}}AFX_DATA

	void WriteLog(int flags,const char *string);
	void ClearLog(void);
	bool GetCancelState(void) const;
	void SetThread(CWinThread *pSynchingThread);
	void SetThreadRunning(bool bThreadRunning);
	void SetSourceFileTitle(const char *string);
	void SetSourceFile(const char *string);
	void SetDestFileTitle(const char *string);
	void SetDestFile(const char *string);
	void SetCurrentFileProgress(int percent);
	void SetTotalFileProgress(int percent);
	void SetTransferRate(const char *string);
	void SetEstimatedTime(const char *string);
	void SetErrorText(const char *string);
	bool GetInitDialogCalled(void) const;

	void SetDialogTitle(const char *string);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SyncDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWinThread *m_pSynchingThread;
	bool m_bThreadRunning;
	bool m_bCancel;
	bool m_bClose;
	bool m_bInitDialogCalled;
	CString m_titleText;

	LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(SyncDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDoneButton();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCloseCheck();
	afx_msg void OnBnClickedExitCheck();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
