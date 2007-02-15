#if !defined(AFX_CONNECTINGMESSAGE_H__B00497C5_22F3_4E86_9ABB_25C675D28E80__INCLUDED_)
#define AFX_CONNECTINGMESSAGE_H__B00497C5_22F3_4E86_9ABB_25C675D28E80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectingMessage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectingMessage dialog

class CConnectingMessage : public CDialog
{
// Construction
public:
	CConnectingMessage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectingMessage)
	enum { IDD = IDD_CONNECTING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectingMessage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectingMessage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTINGMESSAGE_H__B00497C5_22F3_4E86_9ABB_25C675D28E80__INCLUDED_)
