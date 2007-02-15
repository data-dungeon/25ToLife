#if !defined(AFX_OUTPUTLOGPANE_H__2DAA4602_2180_4FC5_BF13_AD4C22E439CD__INCLUDED_)
#define AFX_OUTPUTLOGPANE_H__2DAA4602_2180_4FC5_BF13_AD4C22E439CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputLogPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputLogPane dialog

#include "GameClientPane.h"

class COutputLogPane : public CGameClientPane
{
	DECLARE_DYNAMIC(COutputLogPane);

// Construction
public:
	void AppendText(CString text);
	COutputLogPane(CWnd* pParent = NULL);   // standard constructor
	virtual int GetDialogIDD() { return IDD; }

// Dialog Data
	//{{AFX_DATA(COutputLogPane)
	enum { IDD = IDD_OUTPUTLOG };
	CRichEditCtrl	m_Log;
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputLogPane)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COutputLogPane)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClearoutputlog();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTLOGPANE_H__2DAA4602_2180_4FC5_BF13_AD4C22E439CD__INCLUDED_)
