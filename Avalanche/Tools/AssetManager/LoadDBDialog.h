#ifndef __LOADDBDIALOG__
#define __LOADDBDIALOG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadDBDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LoadDBDialog dialog

class LoadDBDialog : public CDialog
{
// Construction
public:
	LoadDBDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(LoadDBDialog)
	enum { IDD = IDD_LOADDB_DIALOG };
	CStatic	m_text3;
	CStatic	m_text2;
	CStatic	m_text1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LoadDBDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	bool GetCancelState(void) const;
	void SetTextLine1(const char *pString);
	void SetTextLine2(const char *pString);
	void SetTextLine3(const char *pString);
	void SetThread(CWinThread *pLoadingThread);
	void SetThreadRunning(bool bThreadRunning);
	bool GetInitDialogCalled(void) const;

// Implementation
protected:
	CWinThread *m_pLoadingThread;
	bool m_bThreadRunning;
	bool m_bCancel;
	bool m_bClose;
	bool m_bInitDialogCalled;

	LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
	// Generated message map functions
	//{{AFX_MSG(LoadDBDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelButton();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
