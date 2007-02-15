#if !defined(AFX_MirrorBonesDialog_H__92BBE998_909B_4B9D_B08C_21CEF4F4CF7C__INCLUDED_)
#define AFX_MirrorBonesDialog_H__92BBE998_909B_4B9D_B08C_21CEF4F4CF7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MirrorBonesDialog.h : header file
//
#include "Resource.h"

#ifndef __STRCLASS_H
#include "strclass.h"
#endif
/////////////////////////////////////////////////////////////////////////////
// MirrorBonesDialog dialog

class MirrorBonesDialog : public CDialog
{
// Construction
public:

	MirrorBonesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MirrorBonesDialog)
	enum { IDD = IDD_MIRRORBONES_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MirrorBonesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MirrorBonesDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int	m_adjustright;
	int	m_assignvertices;
	int	m_createbones;
	float	m_tolerance;
   int   m_generatelogfile;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MirrorBonesDialog_H__92BBE998_909B_4B9D_B08C_21CEF4F4CF7C__INCLUDED_)
