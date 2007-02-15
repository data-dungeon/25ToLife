#if !defined(AFX_TCLAMPDIALOG_H__92BBE998_909B_4B9D_B08C_21CEF4F4CF7C__INCLUDED_)
#define AFX_TCLAMPDIALOG_H__92BBE998_909B_4B9D_B08C_21CEF4F4CF7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TClampDialog.h : header file
//
#include "Resource.h"

#ifndef __STRCLASS_H
#include "strclass.h"
#endif
/////////////////////////////////////////////////////////////////////////////
// TClampDialog dialog

class TClampDialog : public CDialog
{
// Construction
public:

	TClampDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TClampDialog)
	enum { IDD = IDD_TCLAMP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TClampDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TClampDialog)
	afx_msg void OnClampsingle();
	afx_msg void OnClmpmultnostretch();
	afx_msg void OnClmpmultstretch();
	afx_msg void OnBlendcheckbox();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowsealphaimage();
	afx_msg void OnBrowseprimimage();
	afx_msg void OnBrowsesecimage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void			StoreDialogDefaults();
	CString			BrowseDialog();
	
public:
	bool SingleClamp;
	bool MultipleClampStretch;
	bool MultipleClampNoStretch;

	int BlendMapping;
	String PrimaryMapPath,SecondaryMapPath,AlphaMapPath;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCLAMPDIALOG_H__92BBE998_909B_4B9D_B08C_21CEF4F4CF7C__INCLUDED_)
