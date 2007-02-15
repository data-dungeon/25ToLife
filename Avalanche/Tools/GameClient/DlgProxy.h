// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__0702D4AC_8E4B_4B13_AA52_798AF223B939__INCLUDED_)
#define AFX_DLGPROXY_H__0702D4AC_8E4B_4B13_AA52_798AF223B939__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGameClientDlg;

/////////////////////////////////////////////////////////////////////////////
// CGameClientDlgAutoProxy command target

class CGameClientDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CGameClientDlgAutoProxy)

	CGameClientDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CGameClientDlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameClientDlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGameClientDlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CGameClientDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CGameClientDlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CGameClientDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__0702D4AC_8E4B_4B13_AA52_798AF223B939__INCLUDED_)
