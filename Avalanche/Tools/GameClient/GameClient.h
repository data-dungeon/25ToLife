// GameClient.h : main header file for the GAMECLIENT application
//

#if !defined(AFX_GAMECLIENT_H__DF352AA8_24C5_4511_910F_C7A7BB3BC657__INCLUDED_)
#define AFX_GAMECLIENT_H__DF352AA8_24C5_4511_910F_C7A7BB3BC657__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "GameClientDlg.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CGameClientApp:
// See GameClient.cpp for the implementation of this class
//

class CCommunicator;

enum HardwareType {
	HT_PCLOCAL,
	HT_PCREMOTE,
	HT_XBOX,
	HT_PS2,
	HT_GAMECUBE,
};

class CGameClientApp : public CWinApp
{
public:
	CString GetMyIP();
	BOOL IsConnected();
	void ResetPanes();
	int AddPane(CGameClientPane* dlg, CString tabText);
	CString GetFileServerRoot();
	CGameClientDlg* m_GameClientDlg;
	BOOL Connect(HardwareType hardware, CString ipAddress, CString fileServerRoot);
	BOOL Disconnect();
	CGameClientApp();
	CCommunicator* m_Communicator;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGameClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CString m_FileServerRoot;
};


extern CGameClientApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMECLIENT_H__DF352AA8_24C5_4511_910F_C7A7BB3BC657__INCLUDED_)
