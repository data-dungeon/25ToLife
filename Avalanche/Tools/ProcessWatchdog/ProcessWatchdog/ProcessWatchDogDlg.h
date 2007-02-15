// ProcessWatchDogDlg.h : header file
//

#pragma once
#include "afxcmn.h"

#include "XListCtrl.h"
#include "afxwin.h"
#include "resource.h"

class WatchdogManager;
class ProcessManager;
class Tray;


// CProcessWatchDogDlg dialog
class CProcessWatchDogDlg : public CDialog
{
// Construction
public:
	CProcessWatchDogDlg(CWnd* pParent = NULL);	// standard constructor
	~CProcessWatchDogDlg();

// Dialog Data
	enum { IDD = IDD_PROCESS_WATCHDOG_DIALOG };

	enum MenuFrom
	{
		MENU_FROM_TOP,
		MENU_FROM_WATCHDOGS,
		MENU_FROM_PROCESSES,
	};

	class LogFlags
	{
	public:
		enum
		{
			FONT_STYLE_BOLD					= 1<<3,
			FONT_STYLE_ITALIC				= 1<<4,
			FONT_STYLE_STRIKEOUT			= 1<<5,
			FONT_STYLE_UNDERLINE			= 1<<6,

			FONT_COLOR_SHIFT				= 8,
			FONT_COLOR_MASK					= 0xff,
			FONT_COLOR_BLACK				= 0<<8,
			FONT_COLOR_DK_GREY				= 1<<8,
			FONT_COLOR_LT_GREY				= 2<<8,
			FONT_COLOR_WHITE				= 3<<8,
			FONT_COLOR_RED					= 4<<8,
			FONT_COLOR_LT_RED				= 5<<8,
			FONT_COLOR_DK_RED				= 6<<8,
			FONT_COLOR_ORANGE				= 7<<8,
			FONT_COLOR_LT_ORANGE			= 8<<8,
			FONT_COLOR_DK_ORANGE			= 9<<8,
			FONT_COLOR_YELLOW				= 10<<8,
			FONT_COLOR_LT_YELLOW			= 11<<8,
			FONT_COLOR_DK_YELLOW			= 12<<8,
			FONT_COLOR_GREEN				= 13<<8,
			FONT_COLOR_LT_GREEN				= 14<<8,
			FONT_COLOR_DK_GREEN				= 15<<8,
			FONT_COLOR_CYAN					= 16<<8,
			FONT_COLOR_LT_CYAN				= 17<<8,
			FONT_COLOR_DK_CYAN				= 18<<8,
			FONT_COLOR_BLUE					= 19<<8,
			FONT_COLOR_LT_BLUE				= 20<<8,
			FONT_COLOR_DK_BLUE				= 21<<8,
			FONT_COLOR_MAGENTA				= 22<<8,
			FONT_COLOR_LT_MAGENTA			= 23<<8,
			FONT_COLOR_DK_MAGENTA			= 24<<8,
		};
	};


	MenuFrom m_menuFrom;
	int m_listSelectionIndex;
	CString m_watchdogFilePath;

	Tray *m_pTray;
	BOOL  m_bHidden;
	bool m_bLoadDefaultFile;

	class LogMessage
	{
	public:
		int m_flags;
		TupString m_string;
	};
	TupArray<LogMessage> m_cachedLogMessages;
	CCriticalSection m_logCriticalSection;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void InitProcessListCtrl(void);
	void InitWatchdogListCtrl(void);
	void FillProcessList(void);
	void FillWatchdogList(void);
	BOOL PreTranslateMessage(MSG* pMsg);

	void Refresh(void);

	void RefreshLog(void);
	void ClearLog(void);
	void WriteLog(int flags,const char *string);

	const char *GetClientDataFilePath(void);
	const char *GetDefaultDataFilePath(void);
	bool DirectoryExists(const char *pPath);
	bool CreateDirs (const char *pPath, bool bIsFilePath /*= false*/);

	void SaveSettings(void);

	static bool BrowseFile(CWnd *pParent,const char *pTitle,const char *pFilter,const char *pFile,BOOL openFile,CString &fileString);

// Implementation
protected:
	HICON m_hIcon;

	//{{AFX_MSG(CProcessWatchDogDlg)
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void PostNcDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNMRclickProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnWatchdogNew();
	afx_msg void OnUpdateWatchdogNew(CCmdUI *pCmdUI);
	afx_msg void OnWatchdogEdit();
	afx_msg void OnUpdateWatchdogEdit(CCmdUI *pCmdUI);
	afx_msg void OnWatchdogDelete();
	afx_msg void OnUpdateWatchdogDelete(CCmdUI *pCmdUI);
	afx_msg void OnNMRclickWatchdogList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFileLoad();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnFileExit();
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LONG OnTrayNotify ( WPARAM wParam, LPARAM lParam ) ;
	afx_msg void OnHide();
	afx_msg void OnUnHide();
	afx_msg void OnRestore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	WatchdogManager *m_pWatchdogManager;
	ProcessManager *m_pProcessManager;

	CXListCtrl m_processList;
	CXListCtrl m_watchdogList;

	CStatic m_watchdogTitle;
	CStatic m_processTitle;
	CRichEditCtrl m_logWindow;
	CStatic m_logWindowTitle;

	// can be called from any thread (caches messages until RefreshLog is called
	void AddToLog(int flags,const char *pFormatString, ...);

	void PostRefresh(void);
	afx_msg void OnFileClose();
};
