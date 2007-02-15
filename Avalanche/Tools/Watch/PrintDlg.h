#if !defined(AFX_PRINTDLG_H__0A52131C_FBD2_4631_B40E_49CF117E8033__INCLUDED_)
#define AFX_PRINTDLG_H__0A52131C_FBD2_4631_B40E_49CF117E8033__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintDlg.h : header file
//

#include "ModelessDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg dialog

class CPrintDlg : public CModelessDlg
{
// Construction
public:
	CPrintDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrintDlg)
	enum { IDD = IDD_TEXTOUT_DIALOG };
	CEdit	m_ctlDebugOut;
	//}}AFX_DATA

	void DebugPrint(char *format, ...);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrintDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTDLG_H__0A52131C_FBD2_4631_B40E_49CF117E8033__INCLUDED_)
