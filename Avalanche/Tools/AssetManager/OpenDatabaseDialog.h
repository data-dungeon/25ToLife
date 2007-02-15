////////////////////////////////////////////////////////////////////////////
//
// OpenDatabaseDialog
//
// Dialog for managing and picking the folders
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: OpenDatabaseDialog.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/20/03    Time: 1:12p
 * Updated in $/Avalanche/tools/AssetManager
 * Phase 2 complete
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/16/03    Time: 4:29p
 * Updated in $/Avalanche/tools/AssetManager
 * More work on Phase 2 - Getting the folder sets dialog working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/15/03    Time: 4:12p
 * Updated in $/Avalanche/tools/AssetManager
 * part one of client database stuff
 */

#ifndef ASSET_MANAGER_OPEN_DATABASE_DIALOG_H
#define ASSET_MANAGER_OPEN_DATABASE_DIALOG_H

/////////////////////////////////////////////////////////////////////////////
// OpenDatabaseDialog dialog
class FolderSet;
class FolderSets;

class OpenDatabaseDialog : public CDialog
{
// Construction
public:
	OpenDatabaseDialog(CWnd* pParent = NULL);   // standard constructor
	~OpenDatabaseDialog();

	void SetFolderSets(const FolderSets &folderSets);
	const FolderSets &GetFolderSets(void) const;
	
// Dialog Data
	//{{AFX_DATA(OpenDatabaseDialog)
	enum { IDD = IDD_OPEN_DATABASE };
	CButton	m_okButton;
	CButton	m_newDatabaseButton;
	CButton	m_editDatabaseButton;
	CButton	m_deleteDatabaseButton;
	CButton	m_cancelButton;
	CButton	m_acceptDatabaseButton;
	CButton	m_leftBrowseButton;
	CButton	m_rightBrowseButton;
	CComboBox	m_leftFolderCombo;
	CComboBox	m_rightFolderCombo;
	CEdit	m_databaseDescription;
	CEdit	m_databaseName;
	CListBox	m_folderSetsListBox;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OpenDatabaseDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
		
protected:
	enum Mode
	{
		PICK_MODE,
		EDIT_MODE,
		NEW_MODE,
	};
		
	FolderSets *m_pWorkingFolderSets;
	FolderSet *m_pWorkingFolderSet;
	bool m_bClosing; // If we are in the middle of closing the dialog
	Mode m_mode;
	CString m_strTempRootDir;
	HICON m_hIconBrowse;
	
	// Generated message map functions
	//{{AFX_MSG(OpenDatabaseDialog)
	afx_msg void OnOkButton();
	afx_msg void OnNewDatabase();
	afx_msg void OnEditDatabase();
	afx_msg void OnDeleteDatabase();
	afx_msg void OnAcceptDatabase();
	afx_msg void OnCancelButton();
	afx_msg void OnSelchangeFolderSets();
	afx_msg void OnButtonLeftBrowse();
	afx_msg void OnButtonRightBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int GetNumEnabledSets(void) const;
	void SetupFolderSetsListBox(void);
	void SetupWorkingSet(void);
	void UpdateFromWorkingSet(int index);
	void SetupButtons(void);
	void SetupWorkingSetControls(void);
	static int CALLBACK FolderPickCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	CString ShowFolderPickDlg(UINT nCaptionStringID, CString strRootDir);
	void SetTempRootDir(CString strNewRoot);
	CString GetTempRootDir(void);
	void UpdateWorkingSet(void);
	bool VerifyWorkingSet(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
