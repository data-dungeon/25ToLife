#if !defined(AFX_EXPORTMODELSDIALOG_H__58C09837_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
#define AFX_EXPORTMODELSDIALOG_H__58C09837_D8D2_11D3_83A5_00E0811049D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportModelsDialog.h : header file
//
#include <AfxTempl.h>
#include "StrClass.h"

/////////////////////////////////////////////////////////////////////////////
// CExportModelsDialog dialog

class ModelCacheHandle;
class CExportModelsDialog : public CDialog
{
// Construction
public:
   CExportModelsDialog(){}   // standard constructor
	CExportModelsDialog(ObjectCacheContainerHandle *occh);

   void FillCombo(int id);

// Dialog Data
   IEModelParms *m_iemp;
   ObjectCacheContainerHandle *m_occh;
   String m_dir;
	CCheckListBox	m_checklistbox;
   CList<ModelCacheHandle *, ModelCacheHandle *> m_selected;

   //{{AFX_DATA(CExportModelsDialog)
	enum { IDD = IDD_EXPORTMODELSDIALOG };
	BOOL	m_generatemodelmaps;
	int	m_maxsize;
	int	m_minsize;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportModelsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportModelsDialog)
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

#endif // !defined(AFX_EXPORTMODELSDIALOG_H__58C09837_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
