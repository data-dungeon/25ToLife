#if !defined(AFX_MODELESSDLG_H__FD4CFEB9_7EE5_496F_AB51_D6A7C0D5FA3C__INCLUDED_)
#define AFX_MODELESSDLG_H__FD4CFEB9_7EE5_496F_AB51_D6A7C0D5FA3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelessDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog

class CModelessDlg : public CDialog
{
// Construction
public:
	CModelessDlg(CWnd* pParent, UINT);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelessDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelessDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	void OnCancel();
	void OnOK();

// Implementation
protected:
	CWnd* pView;
	UINT idd;
	bool inited;

	// Generated message map functions
	//{{AFX_MSG(CModelessDlg)
	afx_msg void OnMove(int x, int y);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELESSDLG_H__FD4CFEB9_7EE5_496F_AB51_D6A7C0D5FA3C__INCLUDED_)
