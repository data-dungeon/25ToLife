// WinUpdaterDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class UpdaterCommandLine;

enum DloadState {
	DLOADSTATE_INIT,				// Haven't started yet
	DLOADSTATE_ERROR,				// An error occured; we are dead in the water
	DLOADSTATE_READY,				// Initialized, but haven't checked yet
	DLOADSTATE_TESTING_VERSION,		// Checking for the presence of an update
	DLOADSTATE_UPDATE_NEEDED,		// We need an update, ready to download
	DLOADSTATE_UPDATING,			// In process of downloading update
	DLOADSTATE_PREPARING,			// The update is being prepared for use
	DLOADSTATE_PATCHABLE,			// The update is fully prepared and can be used to patch
	DLOADSTATE_EXITING,				// Finished with everything, need to clean up
	DLOADSTATE_DONE					// Finished with everything and PatcherDLL dll uninstalled
};

// CWinUpdaterDlg dialog
class CWinUpdaterDlg : public CDialog
{
// Construction
public:
	CWinUpdaterDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CWinUpdaterDlg() {Cleanup();}

	void SetCommandLineInfo(const UpdaterCommandLine& cmdLine);

// Dialog Data
	enum { IDD = IDD_WINUPDATER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON		m_hIcon;
	UINT_PTR	m_timer;
	std::string m_versionHost;
	std::string m_versionPath;
	std::string m_dirToPatch;
	DloadState	m_state;
	char*		m_buf;
	int			m_bufsize;
	HANDLE		m_parentHandle;

	// My functions
	void ChangeState(DloadState newState);
	void PrintLine(bool endline, char* fmt, ...);
	void VersionCheckWork(void);
	void UpdatingWork(void);
	void PreparingWork(void);
	void ApplyPatches(void);
	void Cleanup(void);
	void PrintErrorDetails(void);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CProgressCtrl m_progressBarCtrl;
	CString m_statusString;
	CString m_currentAction;
	CString m_progressText;
	CEdit m_statusCtrl;
	CStatic m_titleTextCtrl;	// Title banner at top of dialog
	CStatic m_restartTextCtrl;
	CStatic m_statusTitle;
	CStatic m_currentActionCtrl;
	CStatic m_progressTextCtrl;
	CButton m_restartButton;
public:
	afx_msg void OnTimer(UINT nIDEvent);
	CButton m_cancelButton;
};
