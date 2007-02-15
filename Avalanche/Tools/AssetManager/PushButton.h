//
// PushButton.h
//
// Button object to handle the pushbuttons next to the sync button

#ifndef __PUSH_BUTTON__
#define __PUSH_BUTTON__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPushButton window

class CPushButton : public CButton
{
// Construction
public:
	CPushButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPushButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPushButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPushButton)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
