//{{AFX_INCLUDES()
#include "strip.h"
//}}AFX_INCLUDES
#if !defined(AFX_STRIPDLG_H__E9A4340F_1AF8_49EF_BD72_ACDDEA801969__INCLUDED_)
#define AFX_STRIPDLG_H__E9A4340F_1AF8_49EF_BD72_ACDDEA801969__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StripDialog.h : header file
//

#include "ModelessDlg.h"
#include "WatchInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CStripDialog dialog

class CStripDlg : public CModelessDlg
{
// Construction
public:
	CStripDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStripDlg)
	enum { IDD = IDD__STRIPCHART_DIALOG };
	CListCtrl	m_varList;
	CStrip	m_strip;
	CImageList	m_imageList;
	//}}AFX_DATA

	void AddVarWatch(VAR_ADD_WATCH& vaw);
	void UpdateVarWatch(VAR_UPDATE_WATCH& vuw);
	void AddXY(VAR_ADD_XY& vaxy);
	void Clear() {m_strip.ClearAll();}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStripDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStripDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnStripchartProperties();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRIPDLG_H__E9A4340F_1AF8_49EF_BD72_ACDDEA801969__INCLUDED_)
