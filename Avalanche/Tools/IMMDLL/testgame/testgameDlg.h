// testgameDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CtestgameDlg dialog
class CtestgameDlg : public CDialog
{
// Construction
public:
	CtestgameDlg(CWnd* pParent = NULL);	// standard constructor
	~CtestgameDlg();
	void CloseConnection();

// Dialog Data
	enum { IDD = IDD_TESTGAME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CAsyncSocket* mSocket;
	UINT_PTR mTimerID;

	template <class T> bool Receive(T& var, bool waitforit = true);
	bool Receive(void* buffer, int bufflen, bool waitforit = true);
	template <class T> void Send(const T& var);
	void Send(const void* buffer, int bufflen);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	CButton mConnect;
	CButton mDisconnect;
	float mCam[3][3];
	float mInst[4][4];
	CString mInstName;
};
