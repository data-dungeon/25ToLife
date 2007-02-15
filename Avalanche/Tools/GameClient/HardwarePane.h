#if !defined(AFX_HARDWAREPANE_H__A5EE4E91_29A0_4D7C_9870_051069CCDE5D__INCLUDED_)
#define AFX_HARDWAREPANE_H__A5EE4E91_29A0_4D7C_9870_051069CCDE5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HardwarePane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHardwarePane dialog

#include "GameClientPane.h"

class CHardwarePane : public CGameClientPane
{
// Construction
public:
	CString GetFileServerRoot();
	CString GetDeviceIPAddress();
	CHardwarePane(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHardwarePane)
	enum { IDD = IDD_HARDWARE };
	CButton	m_Connect;
	CComboBox	m_HardwareType;
	CString	m_DeviceIPAddressStr;
	CString	m_FileServerRoot;
	//}}AFX_DATA

	HardwareType GetHardwareType();
	virtual int GetDialogIDD() { return IDD; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHardwarePane)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableForConnected(BOOL connected);

	// Generated message map functions
	//{{AFX_MSG(CHardwarePane)
	virtual BOOL OnInitDialog();
	afx_msg void OnConnect();
	afx_msg void OnFileserverrootBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HARDWAREPANE_H__A5EE4E91_29A0_4D7C_9870_051069CCDE5D__INCLUDED_)
