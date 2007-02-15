#if !defined(AFX_PROPERTIESPANE_H__9A60EAC5_EB5B_4C96_BE70_C9A972B10CDC__INCLUDED_)
#define AFX_PROPERTIESPANE_H__9A60EAC5_EB5B_4C96_BE70_C9A972B10CDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesPane window

class CPropertiesPane : public CWnd
{
// Construction
public:
	CPropertiesPane();

// Attributes
public:

	CXTPPropertyGrid m_wndPropertyGrid;
	CXTPToolBar m_wndToolBar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesPane)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertiesPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertiesPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnPaneproperiesCategorized();
	afx_msg void OnUpdatePaneproperiesCategorized(CCmdUI* pCmdUI);
	afx_msg void OnPaneproperiesAlphabetic();
	afx_msg void OnUpdatePaneproperiesAlphabetic(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESPANE_H__9A60EAC5_EB5B_4C96_BE70_C9A972B10CDC__INCLUDED_)
