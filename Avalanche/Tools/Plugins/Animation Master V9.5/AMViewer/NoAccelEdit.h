#if !defined(AFX_NOACCELEDIT_H__A19E64A0_749F_11D2_BFEB_0080ADC91403__INCLUDED_)
#define AFX_NOACCELEDIT_H__A19E64A0_749F_11D2_BFEB_0080ADC91403__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NoAccelEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNoAccelEdit window

class CNoAccelEdit : public CEdit
{
public:
   CString m_originaltext;
	CNoAccelEdit();

   void StoreText();
   void ResetText();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoAccelEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNoAccelEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNoAccelEdit)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOACCELEDIT_H__A19E64A0_749F_11D2_BFEB_0080ADC91403__INCLUDED_)
