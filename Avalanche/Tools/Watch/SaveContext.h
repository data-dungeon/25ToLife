#if !defined(AFX_SAVECONTEXT_H__9EA7693D_A85C_474A_9785_7927281EDBE1__INCLUDED_)
#define AFX_SAVECONTEXT_H__9EA7693D_A85C_474A_9785_7927281EDBE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveContext.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSaveContext dialog

class CSaveContext : public CDialog
{
// Construction
public:
	CSaveContext(int id, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveContext)
//	enum { IDD = IDD_CONTEXT_SAVE };
	CString	m_filename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveContext)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveContext)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVECONTEXT_H__9EA7693D_A85C_474A_9785_7927281EDBE1__INCLUDED_)
