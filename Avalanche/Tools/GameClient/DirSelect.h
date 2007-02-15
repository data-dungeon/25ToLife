#if !defined(AFX_DIRSELECT_H__A3A643FF_C2C7_46EC_9DFB_3F6FD8A7F48E__INCLUDED_)
#define AFX_DIRSELECT_H__A3A643FF_C2C7_46EC_9DFB_3F6FD8A7F48E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirSelect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirSelect dialog

class CDirSelect : public CDialog
{
// Construction
public:
	CDirSelect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDirSelect)
	enum { IDD = IDD_DIRSELECT };
	CTreeCtrl	m_PathTree;
	CComboBox	m_Drive;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDirSelect)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_DIRSELECT_H__A3A643FF_C2C7_46EC_9DFB_3F6FD8A7F48E__INCLUDED_)
