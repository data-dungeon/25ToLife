#if !defined(AFX_EXPORTACTIONSDIALOG_H__98884FB3_DA64_11D3_83A6_00E0811049D9__INCLUDED_)
#define AFX_EXPORTACTIONSDIALOG_H__98884FB3_DA64_11D3_83A6_00E0811049D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportActionsDialog.h : header file
//
#include <AfxTempl.h>
#include "StrClass.h"

/////////////////////////////////////////////////////////////////////////////
// CExportActionsDialog dialog

class CExportActionsDialog : public CDialog
{
// Construction
public:
   CExportActionsDialog(){}   // standard constructor
	CExportActionsDialog(HActionCacheContainer *hacc);   // standard constructor

   void FillCombo(int id);

   // Dialog Data
//   IEModelParms *m_iemp;
   HActionCacheContainer *m_hacc;
   String m_dir;
	CCheckListBox	m_checklistbox;
   CList<HActionCache *, HActionCache *> m_selected;

	//{{AFX_DATA(CExportActionsDialog)
	enum { IDD = IDD_EXPORTMODELSDIALOG };
	BOOL	m_generatemodelmaps;
	int	m_maxsize;
	int	m_minsize;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportActionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportActionsDialog)
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

#endif // !defined(AFX_EXPORTACTIONSDIALOG_H__98884FB3_DA64_11D3_83A6_00E0811049D9__INCLUDED_)
