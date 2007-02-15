// TupperChainGuiAppDlg.h : header file
//

#if !defined(AFX_TUPPERCHAINGUIAPPDLG_H__AC5F3836_4AA6_491E_90FB_E91FA7D7D2F1__INCLUDED_)
#define AFX_TUPPERCHAINGUIAPPDLG_H__AC5F3836_4AA6_491E_90FB_E91FA7D7D2F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTupperChainGuiAppDlg dialog

class TupperChainLog;

class CTupperChainGuiAppDlg : public CDialog
{
// Construction
public:
	CTupperChainGuiAppDlg(CWnd* pParent = NULL);	// standard constructor

	void WriteLog(int flags,const char *string);
	void ClearLog(void);
	void SetProgressBar(int min,int max,int value);
	void SetProgressText(const char *string);
	bool StopButtonPressed(void);
	void SetPluginPath(const char *pluginPath);
	void CheckFileDependencies(TupperChainLog &log,FilePathArray &filePathArray);

// Dialog Data
	//{{AFX_DATA(CTupperChainGuiAppDlg)
	enum { IDD = IDD_TUPPERCHAINGUIAPP_DIALOG };
	CStatic	m_dataTimeText;
	CButton	m_stopProcessingButton;
	CButton	m_outputTupperwareFile;
	CStatic	m_progressBarText;
	CProgressCtrl	m_progressBar;
	CRichEditCtrl	m_richEdit1;
	CButton	m_configurationDeleteButton;
	CButton	m_outputDeleteButton;
	CButton	m_sourceDeleteButton;
	CStatic	m_logWindowText;
	CComboBox	m_verboseLevelCombo;
	CComboBox	m_configurationFileCombo;
	CComboBox	m_outputFileCombo;
	CComboBox	m_sourceFileCombo;
	CButton	m_sourceFileButton;
	CButton	m_processButton;
	CButton	m_outputFileButton;
	CButton	m_configurationFileButton;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTupperChainGuiAppDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hFolderIcon;
	HICON m_hDeleteEntryIcon;

	CStringArray m_sourceFileStringArray;
	CStringArray m_outputFileStringArray;
	CStringArray m_configurationFileStringArray;
	int m_verboseLevel;
	int m_sourceFileCurSel;
	int m_outputFileCurSel;
	int m_configurationFileCurSel;
	CRect m_dialogRect;

	CString m_workPath;
	CString m_pluginPath;
	CString m_configPath;
	CWinThread *m_pProcessThread;
	bool m_stopButtonPressed;
	bool m_threadRunning;

	void ReadSettings(void);
	void WriteSettings(void);
	void BrowseFile(int &curSelValue,CComboBox &comboBox,CStringArray &array,const char *pTitle,const char *pFilter,BOOL openFile);
	void UpdateComboBox(int &curSelValue,CComboBox &comboBox,CStringArray &array,CString &path); 
	void DeleteComboBoxEntry(int &curSelValue,CComboBox &comboBox,CStringArray &array,CString &path);

	void UpdateSourceFileComboList(void);
	void UpdateOutputFileComboList(void);
	void UpdateConfigurationFileComboList(void);

	static UINT ProcessProc(LPVOID pParam);
	UINT ProcessThread(void);
	LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
	void EnableControls(BOOL enable);

	// Generated message map functions
	//{{AFX_MSG(CTupperChainGuiAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSourceFileButton();
	afx_msg void OnOutputFileButton();
	afx_msg void OnConfigurationFileButton();
	afx_msg void OnProcessButton();
	afx_msg void OnSelchangeOutputFileCombo();
	afx_msg void OnSelchangeSourceFileCombo();
	afx_msg void OnSelchangeConfigurationFileCombo();
	afx_msg void OnSelchangeVerboseLevelCombo();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO *lpMMI);
	afx_msg void OnKillfocusConfigurationFileCombo();
	afx_msg void OnKillfocusOutputFileCombo();
	afx_msg void OnKillfocusSourceFileCombo();
	afx_msg void OnSourceDeleteButton();
	afx_msg void OnOutputDeleteButton();
	afx_msg void OnConfigurationDeleteButton();
	afx_msg void OnOutputTupperwareFile();
	afx_msg void OnStopProcessingButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUPPERCHAINGUIAPPDLG_H__AC5F3836_4AA6_491E_90FB_E91FA7D7D2F1__INCLUDED_)
