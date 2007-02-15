#if !defined(AFX_EXPORTMODELDIALOG_H__58C09836_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
#define AFX_EXPORTMODELDIALOG_H__58C09836_D8D2_11D3_83A5_00E0811049D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StrClass.h"
// ExportModelDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportModelDialog dialog

class CExportModelDialog : public CDialog
{
// Construction
public:
   CExportModelDialog(){}   // standard constructor
	CExportModelDialog(const String &name);

   void FillCombo(int id);

// Dialog Data
//   IEModelParms *m_iemp;
   String m_filename;

	//{{AFX_DATA(CExportModelDialog)
	enum { IDD = IDD_EXPORTMODELDIALOG };
	BOOL	m_generatemodelmaps;
	int	m_maxsize;
	int	m_minsize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportModelDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportModelDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	afx_msg void OnMaxSize();
	afx_msg void OnMinSize();
	afx_msg void OnGeneratemodelmaps();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTMODELDIALOG_H__58C09836_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
