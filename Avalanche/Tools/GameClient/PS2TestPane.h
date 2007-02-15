#if !defined(AFX_PS2TESTPANE_H__CA7D2343_D5C9_455F_86E4_D531CBF93F0A__INCLUDED_)
#define AFX_PS2TESTPANE_H__CA7D2343_D5C9_455F_86E4_D531CBF93F0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PS2TestPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPS2TestPane dialog

class CPS2TestPane : public CGameClientPane
{
	struct PacketHdr
	{
		unsigned short code;
		unsigned short id;
		unsigned long length;
	};

// Construction
public:
	CString GetDeviceIPAddress();
	BOOL GetIsCommunicating();
	void SetCommunicating(BOOL isCommunicating);
	void Status(CString string);
	virtual BOOL Connect(CString ipAddress);
	CPS2TestPane(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPS2TestPane)
	enum { IDD = IDD_PS2_TEST_COMMANDS };
	CString	m_LoadFile;
	//}}AFX_DATA
	virtual int GetDialogIDD() { return IDD; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPS2TestPane)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_DeviceIPAddress;
	BOOL IsCommunicating;

	// thread functions
	static UINT Reset(LPVOID TestPane);
	static UINT Hello(LPVOID TestPane);
	static UINT Load(LPVOID TestPane);

	// Generated message map functions
	//{{AFX_MSG(CPS2TestPane)
	afx_msg void OnLoad();
	afx_msg void OnHello();
	afx_msg void OnReset();
	afx_msg void OnLoadfileBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PS2TESTPANE_H__CA7D2343_D5C9_455F_86E4_D531CBF93F0A__INCLUDED_)
