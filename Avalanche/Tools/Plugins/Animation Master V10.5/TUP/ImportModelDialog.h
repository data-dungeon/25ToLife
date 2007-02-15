#if !defined(AFX_IMPORTMODELDIALOG_H__2965F0C3_D99B_11D3_83A5_00E0811049D9__INCLUDED_)
#define AFX_IMPORTMODELDIALOG_H__2965F0C3_D99B_11D3_83A5_00E0811049D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportModelDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportModelDialog dialog

class CImportModelDialog : public CDialog
{
// Construction
public:
	CImportModelDialog();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportModelDialog)
	enum { IDD = IDD_IMPORT_DIALOG };
	float	m_peaktolerance;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportModelDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportModelDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTMODELDIALOG_H__2965F0C3_D99B_11D3_83A5_00E0811049D9__INCLUDED_)
