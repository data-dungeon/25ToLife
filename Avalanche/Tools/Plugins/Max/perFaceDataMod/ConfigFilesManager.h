#pragma once
#include "afxwin.h"

#define __CONFIGFILESMANAGER_H__

class ConfigFilesDialog;

class ConfigFilesManager
{
	class ConfigFile
	{
	public:
		TupString m_name;
		TupString m_file;
	};

	TupArray<ConfigFile> m_configFiles;
	int m_currentFile;
	bool m_bValidFile;
	FILETIME m_lastLoadedTime; 

public:
	ConfigFilesManager(void);
	~ConfigFilesManager();

	void LoadFromRegistry(void);
	void SaveToRegistry(void);

	int GetNumConfigFiles(void) const;
	const char *GetName(int index) const;

	int GetCurrentConfigIndex(void) const;
	void SetCurrentConfigIndex(int index);

	void GetConfigData(TupperwareAggregate **pAgg);
	bool NeedToLoadData(void);

	bool EditConfigFileList(HWND hParentWnd);

private:
	static bool GetFileTime(const char *pFileName,FILETIME &fileTime);
	static int CompareTimes(FILETIME leftTime,FILETIME rightTime);

	friend class ConfigFilesDialog;
};

// ConfigFilesDialog dialog
class ConfigFilesDialog : public CDialog
{
	DECLARE_DYNAMIC(ConfigFilesDialog)

	ConfigFilesManager &m_configFilesManager;
	enum Mode
	{
		PICK_MODE,
		EDIT_MODE,
		NEW_MODE,
	};

	TupArray<ConfigFilesManager::ConfigFile> m_workingConfigFiles;
	ConfigFilesManager::ConfigFile m_workingConfigFile;
	int m_currentFile;

	bool m_bClosing; // If we are in the middle of closing the dialog
	Mode m_mode;
	CString m_strTempRootDir;
	HICON m_hIconBrowse;

	void SetupConfigFilesListBox(void);
	void SetupWorkingConfigFile(void);
	void UpdateFromWorkingConfigFile(int index);
	void SetupButtons(void);
	void SetupWorkingConfigFileControls(void);
	void UpdateWorkingConfigFile(void);
	bool VerifyWorkingConfigFile(void);


public:
	ConfigFilesDialog(ConfigFilesManager &configFilesManager,CWnd* pParent = NULL);   // standard constructor
	virtual ~ConfigFilesDialog();

	// Dialog Data
	enum { IDD = IDD_CONFIG_FILES };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_configFilesListBox;
	afx_msg void OnLbnSelchangeConfigList();
	CEdit m_configName;
	CEdit m_configFile;
	CButton m_browseButton;
	CButton m_okButton;
	CButton m_newButton;
	CButton m_editButton;
	CButton m_deleteButton;
	CButton m_acceptButton;
	CButton m_cancelButton;
	afx_msg void OnBnClickedBrowseFileButton();
	afx_msg void OnBnClickedOkButton();
	afx_msg void OnBnClickedNewButton();
	afx_msg void OnBnClickedEditButton();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedAcceptButton();
	afx_msg void OnBnClickedCancelButton();
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
	virtual void OnCancel();
};



