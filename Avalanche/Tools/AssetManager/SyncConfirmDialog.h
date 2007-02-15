#ifndef __CSYNCCONFIRMDIALOG__
#define __CSYNCCONFIRMDIALOG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyncConfirmDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSyncConfirmDialog dialog

class CSyncConfirmDialog : public CDialog
{
// Construction
public:
	CSyncConfirmDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSyncConfirmDialog)
	enum { IDD = IDD_SYNC_CONFIRM };
	CString	m_confirmText1;
	CString	m_confirmText2;
	CString	m_confirmText3;
	CString	m_confirmText4;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncConfirmDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSyncConfirmDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
