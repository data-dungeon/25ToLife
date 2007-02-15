#if !defined(AFX_GAMECLIENTPANE_H__E2BED491_6187_44F6_96AE_BF5D11D4B3FA__INCLUDED_)
#define AFX_GAMECLIENTPANE_H__E2BED491_6187_44F6_96AE_BF5D11D4B3FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameClientPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGameClientPane dialog

// to make a new pane, first create a new CDialog derived pane, 
// then change its parent to be this class..

class CGameClientPane : public CDialog
{
	DECLARE_DYNAMIC(CGameClientPane);

	
// Construction
public:
	virtual BOOL Disconnect();
	virtual BOOL Connect(CString ipAddress);
	virtual int GetDialogIDD() = 0L;
	CGameClientPane(int idd, CWnd* pParent = NULL);   // standard constructor

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMECLIENTPANE_H__E2BED491_6187_44F6_96AE_BF5D11D4B3FA__INCLUDED_)
