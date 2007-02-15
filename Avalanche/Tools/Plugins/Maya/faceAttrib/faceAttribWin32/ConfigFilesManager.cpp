
// precomp'ed header files
#include "StdAfx.h"
#include "FaceAttributes.h"
#include "ConfigFilesManager.h"

extern HINSTANCE hInstance; // global HINSTANCE

ConfigFilesManager::ConfigFilesManager(void) :
		m_currentFile(-1),
		m_bValidFile(false)
{
}

ConfigFilesManager::~ConfigFilesManager()
{
}

void ConfigFilesManager::LoadFromRegistry(void)
{
	m_configFiles.RemoveAll();
	m_currentFile = -1;

	Registry registry(CONFIG_FILES_KEY);

	int numConfigFiles = 0;
	char numString[10];
	DWORD tempInt;
#define STRING_SIZE 512
	char tempString[STRING_SIZE];

	if (registry.Get(TOTAL_CONFIG_FILES,&tempInt))
		numConfigFiles = tempInt;

	for (int configIndex=0;configIndex<numConfigFiles;configIndex++)
	{
		itoa(configIndex,numString,10);

		ConfigFile newConfigFile;

		// get the name
		TupString nameField(CONFIG_FILE_NAME_FIELD);
		nameField+= numString;
		if (registry.Get(nameField,&tempString,STRING_SIZE))
		{
			newConfigFile.m_name = tempString;
		}

		// get the path
		TupString pathField(CONFIG_FILE_PATH_FIELD);
		pathField+= numString;
		if (registry.Get(pathField,&tempString,STRING_SIZE))
		{
			newConfigFile.m_file = tempString;
		}

		m_configFiles.Add(newConfigFile);
	}

	// if there is no config files then use the default one
	if (!m_configFiles.GetSize())
	{
		char programPathBufferTemp[_MAX_PATH];
		char programPathBuffer[_MAX_PATH];
		GetModuleFileName(NULL,programPathBufferTemp,_MAX_PATH);
		FilePath::GetPath(programPathBufferTemp ,programPathBuffer);
		if (programPathBuffer[strlen(programPathBuffer)-1]!='\\')
			strcat(programPathBuffer,"\\");

		ConfigFile newConfigFile;
		newConfigFile.m_name = CFG_DEFAULT_NAME;

		newConfigFile.m_file = programPathBuffer;
		newConfigFile.m_file += CFG_DEFAULT_FILENAME;
		m_configFiles.Add(newConfigFile);
	}

	if (registry.Get(ACTIVE_CONFIG_FILE,&tempInt))
	{
		m_currentFile = tempInt;
	}
	else
	{
		m_currentFile = 0;
	}
}

void ConfigFilesManager::SaveToRegistry(void)
{
	Registry registry(CONFIG_FILES_KEY);

	int numConfigFiles = m_configFiles.GetSize();

	registry.Set(TOTAL_CONFIG_FILES,(DWORD)numConfigFiles);
	registry.Set(ACTIVE_CONFIG_FILE,(DWORD)m_currentFile);

	char numString[10];

	for (int configIndex=0;configIndex<numConfigFiles;configIndex++)
	{
		itoa(configIndex,numString,10);

		const ConfigFile &configFile = m_configFiles[configIndex];

		// set the name
		TupString nameField(CONFIG_FILE_NAME_FIELD);
		nameField+= numString;
		registry.Set(nameField,configFile.m_name);

		// set the path
		TupString pathField(CONFIG_FILE_PATH_FIELD);
		pathField+= numString;
		registry.Set(pathField,configFile.m_file);
	}
}


int ConfigFilesManager::GetNumConfigFiles(void) const
{
	return m_configFiles.GetSize();
}

const char *ConfigFilesManager::GetName(int index) const
{
	return m_configFiles[index].m_name;
}

int ConfigFilesManager::GetCurrentConfigIndex(void) const
{
	return m_currentFile;
}

void ConfigFilesManager::SetCurrentConfigIndex(int index)
{
	if (index!=m_currentFile)
	{
		m_currentFile = index;
		m_bValidFile = false;
		SaveToRegistry();
	}
}

void ConfigFilesManager::GetConfigData(TupperwareAggregate **pAgg)
{
	if (GetFileTime(m_configFiles[m_currentFile].m_file,m_lastLoadedTime))
	{
		int result = TupperwareLoadSaveManager::Read(m_configFiles[m_currentFile].m_file,pAgg);
		if (result)
		{
			m_bValidFile = true;
		}
		else
		{
			TupperwareAggregate *pErrorTree = new TupperwareAggregate;
			*pAgg = pErrorTree;
			m_bValidFile = false;
			TupString string;
			string.Format("Error - Loading Config File|File - %s",(const char *)m_configFiles[m_currentFile].m_file);
			TupperwareAggregate *pEditBoxAggregate = pErrorTree->AddAggregate("LABEL",string);
		}
	}
	else
	{
		TupperwareAggregate *pErrorTree = new TupperwareAggregate;
		*pAgg = pErrorTree;
		m_bValidFile = false;
		TupString string;
		string.Format("Error - Cannot Find Config File|File - %s",(const char *)m_configFiles[m_currentFile].m_file);
		TupperwareAggregate *pEditBoxAggregate = pErrorTree->AddAggregate("LABEL",string);
	}
}

bool ConfigFilesManager::NeedToLoadData(void)
{
	bool bResult = true;
	if (m_bValidFile)
	{
		FILETIME currentFileTime;
		if (GetFileTime(m_configFiles[m_currentFile].m_file,currentFileTime))
		{
			if (CompareTimes(m_lastLoadedTime,currentFileTime)==0)
			{
				bResult = false;
			}
			else
			{
				AFX_MANAGE_STATE(AfxGetStaticModuleState( ))

				TupString messageString;
				messageString.Format("Reloading updated %s",(const char *)m_configFiles[m_currentFile].m_file);

				AfxMessageBox((const char *)messageString);

				m_bValidFile = false; // from now on we are old
			}
		}
		else
		{
			m_bValidFile = false;
		}
	}
	return bResult;
}

bool ConfigFilesManager::EditConfigFileList(HWND hParentWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ))

	CWnd parentWnd;
	parentWnd.Attach(hParentWnd);

	ConfigFilesDialog configFilesDialog(*this,&parentWnd);

	int result = configFilesDialog.DoModal();

	parentWnd.Detach();

	return (result==TRUE ? true : false);
}


bool ConfigFilesManager::GetFileTime(const char *pFileName,FILETIME &fileTime)
{
	bool bResult = false;
	HANDLE hFile = ::CreateFile(pFileName,  // open path
										0,										// access only
										FILE_SHARE_READ,					// share for reading 
										NULL,									// no security 
										OPEN_EXISTING,						// existing file only 
										FILE_ATTRIBUTE_NORMAL,			// normal file 
										NULL);								// no attr. template 

	if (hFile != INVALID_HANDLE_VALUE) 
	{
		::GetFileTime(hFile,(LPFILETIME) NULL,(LPFILETIME) NULL,&fileTime);
		::CloseHandle(hFile); 
		bResult = true;
	}
	return bResult;
}

int ConfigFilesManager::CompareTimes(FILETIME leftTime,FILETIME rightTime)
{
	__int64 leftValue = *((__int64*)&leftTime);
	__int64 rightValue = *((__int64*)&rightTime);
	__int64 diff = leftValue-rightValue;
	__int64 threshold = 2*10000000; // 2 seconds * 10 million (100 nanoseconds periods)
	if (diff<-threshold)
	{
		return -1; // left is older than right
	}
	else if (diff>threshold)
	{
		return 1; // right is older then left
	}
	else
	{
		return 0; // left and right are within the threshold
	}
}


static bool BrowseFile(CWnd *pParent,const char *pTitle,const char *pFilter,const char *pFile,BOOL openFile,CString &fileString)
{
	fileString = "";

	char pathBuffer[_MAX_PATH];

	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];

	char pFilterBuffer[512];
	strcpy(pFilterBuffer,pFilter);
	char *pch = pFilterBuffer;
	// change '|' to \0
	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

	::GetModuleFileName(NULL,pathBuffer,_MAX_PATH);

	_splitpath(pathBuffer,fdrive,fdir,NULL,NULL);
	_splitpath(pFile,NULL,NULL,fname,fext);
	_makepath(pathBuffer,fdrive,fdir,NULL,NULL);

	OPENFILENAME ofn;       // common dialog box structure
	char fResultFile[_MAX_PATH];
	if (pFile)
	{
		strcpy(fResultFile,pFile);
	}
	else
	{
		fResultFile[0] = '\0';
	}

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pParent->m_hWnd;
	ofn.lpstrFile = fResultFile;
	ofn.nMaxFile = sizeof(fResultFile);
	ofn.lpstrFilter = pFilterBuffer;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = pathBuffer;
	ofn.Flags = openFile ? (OFN_FILEMUSTEXIST | OFN_HIDEREADONLY) : (OFN_HIDEREADONLY);

	// Display the Open dialog box. 

	if (openFile)
	{
		if (!GetOpenFileName(&ofn)) 
		{
			return false;
		}
	}
	else
	{
		if (!GetSaveFileName(&ofn)) 
		{
			return false;
		}
	}
	// if saving the file make sure the extension is at the end
	if (!openFile)
	{
		_splitpath(ofn.lpstrFile,fdrive,fdir,fname,NULL);
		_makepath(ofn.lpstrFile,fdrive,fdir,fname,fext);
	}
	fileString = ofn.lpstrFile;
	return true;
}

// ConfigFilesDialog dialog

IMPLEMENT_DYNAMIC(ConfigFilesDialog, CDialog)
ConfigFilesDialog::ConfigFilesDialog(ConfigFilesManager &configFilesManager,CWnd* pParent /*=NULL*/) :
	m_configFilesManager(configFilesManager),
	CDialog(ConfigFilesDialog::IDD, pParent),
	m_bClosing(false),
	m_hIconBrowse(NULL)
{
}

ConfigFilesDialog::~ConfigFilesDialog()
{
	if (m_hIconBrowse!=NULL)
		::DestroyIcon(m_hIconBrowse);
}

void ConfigFilesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIG_LIST, m_configFilesListBox);
	DDX_Control(pDX, IDC_CONFIG_NAME, m_configName);
	DDX_Control(pDX, IDC_CONFIG_FILE, m_configFile);
	DDX_Control(pDX, IDC_BROWSE_FILE_BUTTON, m_browseButton);
	DDX_Control(pDX, ID_OK_BUTTON, m_okButton);
	DDX_Control(pDX, ID_NEW_BUTTON, m_newButton);
	DDX_Control(pDX, ID_EDIT_BUTTON, m_editButton);
	DDX_Control(pDX, ID_DELETE, m_deleteButton);
	DDX_Control(pDX, ID_ACCEPT_BUTTON, m_acceptButton);
	DDX_Control(pDX, ID_CANCEL_BUTTON, m_cancelButton);
}


BEGIN_MESSAGE_MAP(ConfigFilesDialog, CDialog)
	ON_LBN_SELCHANGE(IDC_CONFIG_LIST, OnLbnSelchangeConfigList)
	ON_BN_CLICKED(IDC_BROWSE_FILE_BUTTON, OnBnClickedBrowseFileButton)
	ON_BN_CLICKED(ID_OK_BUTTON, OnBnClickedOkButton)
	ON_BN_CLICKED(ID_NEW_BUTTON, OnBnClickedNewButton)
	ON_BN_CLICKED(ID_EDIT_BUTTON, OnBnClickedEditButton)
	ON_BN_CLICKED(ID_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(ID_ACCEPT_BUTTON, OnBnClickedAcceptButton)
	ON_BN_CLICKED(ID_CANCEL_BUTTON, OnBnClickedCancelButton)
END_MESSAGE_MAP()



void ConfigFilesDialog::OnLbnSelchangeConfigList()
{
	int selectionIndex = m_configFilesListBox.GetCurSel();
	if (selectionIndex!=-1)
	{
		m_currentFile = m_configFilesListBox.GetItemData(selectionIndex);
		SetupWorkingConfigFile();
		SetupButtons();
		SetupWorkingConfigFileControls();
	}
}

void ConfigFilesDialog::OnBnClickedBrowseFileButton()
{
	static char szFilter[] = "Attribute Configuration Files|*.cfg|"  \
		"All Files (*.*)|*.*||";

	CString fileText;
	m_configFile.GetWindowText(fileText);
	if (fileText.GetLength()==0)
	{
		fileText = "c:\\";
	}

	CString fileString;
	if (BrowseFile(this,"Browse for Configuration File",szFilter,fileText,TRUE,fileString))
	{
		if (fileString.GetLength())
		{
			m_configFile.SetWindowText(fileString);
		}
	}
}

void ConfigFilesDialog::OnBnClickedOkButton()
{
	if(VerifyWorkingConfigFile())
	{
		m_configFilesManager.m_configFiles = m_workingConfigFiles;
		m_configFilesManager.m_currentFile = m_currentFile;
		m_configFilesManager.m_bValidFile = false;
		EndDialog(TRUE);
	}
}

void ConfigFilesDialog::OnBnClickedNewButton()
{
	m_mode = NEW_MODE;
	SetupButtons();
	SetupWorkingConfigFileControls();
}

void ConfigFilesDialog::OnBnClickedEditButton()
{
	m_mode = EDIT_MODE;
	SetupButtons();
	SetupWorkingConfigFileControls();
}

void ConfigFilesDialog::OnBnClickedDelete()
{
	m_workingConfigFiles.RemoveAt(m_currentFile);
	if (m_currentFile==m_workingConfigFiles.GetSize())
		m_currentFile--;
	SetupConfigFilesListBox();
	SetupWorkingConfigFile();
	SetupButtons();
	SetupWorkingConfigFileControls();
}

void ConfigFilesDialog::OnBnClickedAcceptButton()
{
	if(VerifyWorkingConfigFile())
	{
		switch(m_mode) 
		{
		case EDIT_MODE:
			{
				UpdateFromWorkingConfigFile(m_currentFile);
			}
			break;
		case NEW_MODE:
			{
				m_workingConfigFiles.Add(m_workingConfigFile);
				m_currentFile = m_workingConfigFiles.GetSize()-1;
			}
			break;
		}
		m_mode = PICK_MODE;
		SetupConfigFilesListBox();
		SetupWorkingConfigFile();
		SetupButtons();
		SetupWorkingConfigFileControls();
	}
}

void ConfigFilesDialog::OnBnClickedCancelButton()
{
	if (m_mode==PICK_MODE)
	{
		EndDialog(FALSE); // we are done and not going to use whatever we did
	}
	else
	{
		m_mode = PICK_MODE;
		SetupConfigFilesListBox();
		SetupWorkingConfigFile();
		SetupButtons();
		SetupWorkingConfigFileControls();
	}
}

BOOL ConfigFilesDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_workingConfigFiles = m_configFilesManager.m_configFiles;
	m_currentFile = m_configFilesManager.m_currentFile;
	m_workingConfigFile = m_configFilesManager.m_configFiles[m_currentFile];

	if (m_hIconBrowse!=NULL)
	{
		::DestroyIcon(m_hIconBrowse);
		m_hIconBrowse = NULL;
	}

	m_hIconBrowse = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 0, 0, 0);
	m_browseButton.SetIcon(m_hIconBrowse);

	SetupConfigFilesListBox();
	SetupWorkingConfigFile();
	m_mode = PICK_MODE;
	SetupButtons();
	SetupWorkingConfigFileControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ConfigFilesDialog::SetupConfigFilesListBox(void)
{
	m_configFilesListBox.ResetContent();
	int numConfigFiles = m_workingConfigFiles.GetSize();
	for (int configFileIndex = 0;configFileIndex<numConfigFiles;configFileIndex++)
	{
		TupString listString = m_workingConfigFiles[configFileIndex].m_name;

		int listBoxIndex = m_configFilesListBox.AddString(listString);
		m_configFilesListBox.SetItemData(listBoxIndex,configFileIndex);
		if (m_currentFile == configFileIndex)
			m_configFilesListBox.SetCurSel(listBoxIndex);
	}
}

void ConfigFilesDialog::SetupWorkingConfigFile(void)
{
	m_workingConfigFile.m_name = m_workingConfigFiles[m_currentFile].m_name;
	m_workingConfigFile.m_file = m_workingConfigFiles[m_currentFile].m_file;
}

void ConfigFilesDialog::UpdateFromWorkingConfigFile(int index)
{
	m_workingConfigFiles[index].m_name = m_workingConfigFile.m_name;
	m_workingConfigFiles[index].m_file = m_workingConfigFile.m_file;
}

void ConfigFilesDialog::SetupButtons(void)
{
	int numConfigFiles = m_workingConfigFiles.GetSize();

	switch (m_mode)
	{
		case PICK_MODE:
			m_okButton.EnableWindow(TRUE);
			m_newButton.EnableWindow(TRUE);
			m_editButton.EnableWindow(TRUE);
			m_deleteButton.EnableWindow(numConfigFiles>1 ? TRUE : FALSE);
			m_cancelButton.EnableWindow(TRUE);
			m_acceptButton.EnableWindow(FALSE);
			break;
		case EDIT_MODE:
		case NEW_MODE:
			m_okButton.EnableWindow(FALSE);
			m_newButton.EnableWindow(FALSE);
			m_editButton.EnableWindow(FALSE);
			m_deleteButton.EnableWindow(FALSE);
			m_cancelButton.EnableWindow(TRUE);
			m_acceptButton.EnableWindow(TRUE);
			break;
	}
}

void ConfigFilesDialog::SetupWorkingConfigFileControls(void)
{
	m_configName.SetWindowText(m_workingConfigFile.m_name);
	m_configFile.SetWindowText(m_workingConfigFile.m_file);

	switch (m_mode)
	{
		case PICK_MODE:
			m_configFilesListBox.EnableWindow(TRUE);
			m_browseButton.EnableWindow(FALSE);
			m_configName.SetReadOnly(TRUE);
			m_configFile.SetReadOnly(TRUE);
			break;
		case EDIT_MODE:
		case NEW_MODE:
			m_configFilesListBox.EnableWindow(FALSE);
			m_browseButton.EnableWindow(TRUE);
			m_configName.SetReadOnly(FALSE);
			m_configFile.SetReadOnly(FALSE);
			break;
	}
}

void ConfigFilesDialog::UpdateWorkingConfigFile(void)
{
	CString tempString;
	m_configName.GetWindowText(tempString);
	m_workingConfigFile.m_name = tempString;

	m_configFile.GetWindowText(tempString);
	m_workingConfigFile.m_file = tempString;

}

bool ConfigFilesDialog::VerifyWorkingConfigFile(void)
{
	UpdateWorkingConfigFile();

	bool bProblem = false;
	CString strProblem;
	if (m_workingConfigFile.m_name.GetLength()==0)
	{
		bProblem = true;
		strProblem = "Name Cannot Be Empty\n\n";
	}
	else if (m_mode == NEW_MODE)
	{
		// see if one with that name exists already
		int numConfigFiles = m_workingConfigFiles.GetSize();
		bool bFoundMatch = false;
		for (int configFileIndex=0;configFileIndex<numConfigFiles;configFileIndex++)
		{
			if (m_workingConfigFile.m_name==m_workingConfigFiles[configFileIndex].m_name)
			{
				bFoundMatch = true;
				break;
			}
		}
		if (bFoundMatch)
		{
			bProblem = true;
			strProblem = "Name needs to be unique\n" + m_workingConfigFile.m_name + "\n";
		}
	}

	if (!bProblem)
	{
		HANDLE hFile = ::CreateFile(m_workingConfigFile.m_file,  // open path
			0,										// access only
			FILE_SHARE_READ,					// share for reading 
			NULL,									// no security 
			OPEN_EXISTING,						// existing file only 
			FILE_ATTRIBUTE_NORMAL,			// normal file 
			NULL);								// no attr. template 

		if (hFile == INVALID_HANDLE_VALUE) 
		{
			bProblem = true;
			strProblem = "File path is not valid\n" + m_workingConfigFile.m_file + "\n";
		}
		else
		{
			::CloseHandle(hFile); 
		}
	}

	if (bProblem)
	{
		::AfxMessageBox((LPCTSTR) strProblem, MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}


void ConfigFilesDialog::OnOK()
{
	// We just want to absorb this message, thus keeping the app from quitting
	// when the user hits the enter key:
	SetFocus();
}

void ConfigFilesDialog::OnCancel()
{
	if (m_bClosing)
	{
		CDialog::OnCancel();
	}
}

