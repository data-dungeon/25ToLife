// testappDlg.h : header file
//

#pragma once

#include "..\IMMDLL\IMMDLL.h"
#include "afxwin.h"

// CtestappDlg dialog
class CtestappDlg : public CDialog
{
// Construction
public:
	CtestappDlg(CWnd* pParent = NULL);	// standard constructor
	~CtestappDlg();

// Dialog Data
	enum { IDD = IDD_TESTAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	IMM::IConnection* mConnection;
	class IMMEventSink : public IMM::IEventSink
	{
		IMM::IConnection* mConnection;
		CtestappDlg* mDialog;
	public:
		IMMEventSink(
			IMM::IConnection* connection,
			CtestappDlg* dialog
		) :
			mConnection( connection ),
			mDialog( dialog )
		{
			mConnection->SetEventSink(this);
		}
		~IMMEventSink()
		{
			mConnection->SetEventSink(NULL);
		}
		
		virtual void OnCloseConnection()
		{
			TRACE("sink reported closed\n");
			// we must POST this message as otherwise, the result would delete
			// the sink, which is a bit illegal (to delete it from within it)
			// not to mention that the result also kills the connection, which
			// contains the thread that called this routine... shutting down
			// is nasty business
			mDialog->PostMessage( WM_COMMAND, IDC_DISCONNECT, NULL );
		}
	};

	IMMEventSink* mTheKitchenSink;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	IMM::NewConnectionAction NewConnection(IMM::IConnection* connection);
	afx_msg void OnBnClickedDisconnect();
	CButton mDisconnect;
};
