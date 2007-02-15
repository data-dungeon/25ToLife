////////////////////////////////////////////////////////////////////////////
//
// ProcessList
//
// Class to query the current processes in the system
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: WatchdogDlg.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/02/03   Time: 2:59p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * First test release
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/02/03   Time: 12:37p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * looks like we are here!
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/29/03    Time: 2:44p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * starting applications - killing processes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/24/03    Time: 3:23p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * updating of watchdogs
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 9/22/03    Time: 3:37p
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * watchdog property page
*/

#include "stdafx.h"
#include "ProcessWatchDog.h"
#include "WatchdogDlg.h"
#include ".\watchdogdlg.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage property page

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
	ofn.lpstrDefExt = fext;
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

//-----------------------------------------------------------------------------
// FolderPickCallbackProc
// This function is a callback proc for the shell browse folder dialog.  It is
// called whenever an event occurs in the shell dialog.
//
// Input:
//  HWND hwnd -- Window handle to the browse dialog box.
//  UINT uMsg -- Value identifying the event.
//  LPARAM lParam -- Value dependant on the message contained in uMsg.
//  LPARAM lpData -- Set by application.
//
// Output: Returns 0.
//-----------------------------------------------------------------------------
int CALLBACK FolderPickCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		{
			// Get the root directory to use:
			CString strRootDir;
			CString *pDir = ((CString *) lpData);
			if (pDir != NULL)
				strRootDir = *pDir;
			else
				strRootDir = "c:\\";
			::SendMessage(hwnd, BFFM_SETSELECTION, 1, (LPARAM) ((LPCTSTR) strRootDir));
			break;
		}
	case BFFM_SELCHANGED:
		{
			TCHAR szText[MAX_PATH] = {0};
			SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), szText);
			::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, reinterpret_cast<LPARAM>(szText));
			break;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// ShowFolderPickDlg
// This method can be called to show a folder picker dialog.  It will return
// the path to the folder picked.
//
// Input:
//	CString strTitle -- The string to show in the
//							 caption of the dialog.
//	CString strRootDir -- The root directory to specify for the dialog.
//
// Output: CString representing the folder chosen.  This will be empty if the
//		   user canceled out of the dialog, or for some reason did not specify
//		   a folder.
//-----------------------------------------------------------------------------
static CString ShowFolderPickDlg(CDialog &dialog,CString strTitle, CString strRootDir)
{
	CString strFolder = "";

	// Have to call CoInitialize to use the BIF_USENEWUI flag in the browseinfo:
	CoInitialize(NULL);

	// Set up some data to use:
	LPMALLOC pMalloc = NULL;
	LPITEMIDLIST pidl = NULL;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	// Set the browseinfo's default values:
	bi.hwndOwner = dialog.GetSafeHwnd();
	bi.lpszTitle = strTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_USENEWUI;
	bi.lpfn = FolderPickCallbackProc;
	bi.lParam = (LPARAM) &strRootDir;

	// Show the dialog:
	pidl = SHBrowseForFolder(&bi);

	// Get the result back:
	TCHAR szDir[MAX_PATH];
	if(pidl != NULL)
	{
		SHGetPathFromIDList(pidl, szDir);
		strFolder = szDir;
		// Free up memory:
		if(SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);  
			pMalloc->Release(); 
		}
	}

	return strFolder;
}

IMPLEMENT_DYNCREATE(CGeneralPage, CPropertyPage)

CGeneralPage::CGeneralPage() : CPropertyPage(CGeneralPage::IDD)
, m_name(_T(""))
, m_checkInterval(2.0f)
, m_waitForResponse(5.0f)
, m_waitAfterCreation(30.0f)
, m_waitForNoProcesses(5.0f)
{
	//{{AFX_DATA_INIT(CGeneralPage)
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralPage)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_NAME_EDIT, m_name);
	DDX_Text(pDX, IDC_CHECK_INTERVAL_EDIT, m_checkInterval);
	DDX_Text(pDX, IDC_WAIT_FOR_RESPONSE_EDIT, m_waitForResponse);
	DDX_Text(pDX, IDC_WAIT_AFTER_CREATION_EDIT, m_waitAfterCreation);
	DDX_Text(pDX, IDC_WAIT_FOR_NO_PROCESSES, m_waitForNoProcesses);
	DDV_MinMaxFloat(pDX, m_checkInterval, 0.001f, 5000.0f);
	DDV_MinMaxFloat(pDX, m_waitForResponse, 0.000f, 5000.0f);
	DDV_MinMaxFloat(pDX, m_waitAfterCreation, 0.000f, 5000.0f);
	DDV_MinMaxFloat(pDX, m_waitForNoProcesses, 0.000f, 5000.0f);
}


BEGIN_MESSAGE_MAP(CGeneralPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGeneralPage)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CActiveProcessPage property page

IMPLEMENT_DYNCREATE(CActiveProcessPage, CPropertyPage)

CActiveProcessPage::CActiveProcessPage() : CPropertyPage(CActiveProcessPage::IDD)
, m_setting(0)
, m_priority(3)
{
	//{{AFX_DATA_INIT(CActiveProcessPage)
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CActiveProcessPage::~CActiveProcessPage()
{
}

void CActiveProcessPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActiveProcessPage)
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_PRIORITY_COMBO, m_priority);
}


BEGIN_MESSAGE_MAP(CActiveProcessPage, CPropertyPage)
	//{{AFX_MSG_MAP(CActiveProcessPage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_NO_ACTION, OnBnClickedNoAction)
	ON_BN_CLICKED(IDC_KILL_PROCESS, OnBnClickedKillProcess)
	ON_BN_CLICKED(IDC_SET_PRIORITY, OnBnClickedSetPriority)
END_MESSAGE_MAP()

BOOL CActiveProcessPage::OnInitDialog(void) 
{
	CPropertyPage::OnInitDialog();

	CheckRadioButton(IDC_NO_ACTION,IDC_SET_PRIORITY,IDC_NO_ACTION+m_setting);
	EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CActiveProcessPage::OnBnClickedNoAction()
{
	m_setting = 0;
	EnableControls();
}

void CActiveProcessPage::OnBnClickedKillProcess()
{
	m_setting = 1;
	EnableControls();
}

void CActiveProcessPage::OnBnClickedSetPriority()
{
	m_setting = 2;
	EnableControls();
}

void CActiveProcessPage::EnableControls(void)
{
	CWnd *pCWnd;

	if (m_setting == 2)
	{
		pCWnd = GetDlgItem(IDC_PRIORITY_COMBO);
		if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
		{
			pCWnd->EnableWindow(TRUE);
		}
		pCWnd = GetDlgItem(IDC_PRIORITY_TITLE);
		if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
		{
			pCWnd->EnableWindow(TRUE);
		}
	}
	else
	{
		pCWnd = GetDlgItem(IDC_PRIORITY_COMBO);
		if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
		{
			pCWnd->EnableWindow(FALSE);
		}
		pCWnd = GetDlgItem(IDC_PRIORITY_TITLE);
		if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
		{
			pCWnd->EnableWindow(FALSE);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CNoProcessesPage property page

IMPLEMENT_DYNCREATE(CNoProcessesPage, CPropertyPage)

CNoProcessesPage::CNoProcessesPage() : CPropertyPage(CNoProcessesPage::IDD)
, m_targetPath(_T(""))
, m_startInPath(_T(""))
, m_parameters(_T(""))
, m_runType(0)
{
	//{{AFX_DATA_INIT(CNoProcessesPage)
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CNoProcessesPage::~CNoProcessesPage()
{
}

void CNoProcessesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNoProcessesPage)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_TARGET_PATH, m_targetPath);
	DDX_Text(pDX, IDC_START_IN_PATH, m_startInPath);
	DDX_Text(pDX, IDC_PARAMETERS, m_parameters);
	DDX_CBIndex(pDX, IDC_RUN_COMBO, m_runType);
}


BEGIN_MESSAGE_MAP(CNoProcessesPage, CPropertyPage)
	//{{AFX_MSG_MAP(CNoProcessesPage)
	ON_BN_CLICKED(IDC_NO_ACTION, OnBnClickedNoAction)
	ON_BN_CLICKED(IDC_KILL_PROCESS, OnBnClickedOpen)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_TARGET_PATH_BROWSE, OnBnClickedTargetBrowse)
	ON_BN_CLICKED(IDC_START_IN_BROWSE, OnBnClickedStartInBrowse)
END_MESSAGE_MAP()

BOOL CNoProcessesPage::OnInitDialog(void) 
{
	CPropertyPage::OnInitDialog();

	CheckRadioButton(IDC_NO_ACTION,IDC_OPEN,IDC_NO_ACTION+m_setting);
	EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CNoProcessesPage::OnBnClickedNoAction()
{
	m_setting = 0;
	EnableControls();
}

void CNoProcessesPage::OnBnClickedOpen()
{
	m_setting = 1;
	EnableControls();
}

void CNoProcessesPage::OnBnClickedTargetBrowse()
{
	static char szFilter[] = "Executable files|*.exe|"  \
		"All Files (*.*)|*.*||";

	CString fileString;
	if (BrowseFile(this,"Specify Target File",szFilter,m_targetPath,TRUE,fileString))
	{
		m_targetPath = fileString;
		UpdateData(FALSE);
	}
}

void CNoProcessesPage::OnBnClickedStartInBrowse()
{
	CString path = ShowFolderPickDlg(*this,"Specify Start In Folder", m_startInPath);
	if (path!="")
	{
		m_startInPath = path;
		UpdateData(FALSE);
	}
}

void CNoProcessesPage::EnableControls(void)
{
	CWnd *pCWnd;

	int enable = m_setting==1 ? TRUE : FALSE;

	pCWnd = GetDlgItem(IDC_TARGET_PATH);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_TARGET_PATH_TITLE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_TARGET_PATH_BROWSE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_PARAMETERS);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_PARAMETERS_TITLE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_START_IN_PATH);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_START_IN_TITLE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_START_IN_BROWSE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_RUN_COMBO);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_RUN_TITLE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
}



/////////////////////////////////////////////////////////////////////////////
// CNotRepondingPage property page

IMPLEMENT_DYNCREATE(CNotRepondingPage, CPropertyPage)

CNotRepondingPage::CNotRepondingPage() : CPropertyPage(CNotRepondingPage::IDD)
{
	//{{AFX_DATA_INIT(CNotRepondingPage)
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CNotRepondingPage::~CNotRepondingPage()
{
}

void CNotRepondingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotRepondingPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNotRepondingPage, CPropertyPage)
	//{{AFX_MSG_MAP(CNotRepondingPage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_NO_ACTION, OnBnClickedNoAction)
	ON_BN_CLICKED(IDC_KILL_PROCESS, OnBnClickedKillProcess)
END_MESSAGE_MAP()

BOOL CNotRepondingPage::OnInitDialog(void) 
{
	CPropertyPage::OnInitDialog();

	CheckRadioButton(IDC_NO_ACTION,IDC_KILL_PROCESS,IDC_NO_ACTION+m_setting);
	EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CNotRepondingPage::OnBnClickedNoAction()
{
	m_setting = 0;
	EnableControls();
}

void CNotRepondingPage::OnBnClickedKillProcess()
{
	m_setting = 1;
	EnableControls();
}

void CNotRepondingPage::EnableControls(void)
{
}


/////////////////////////////////////////////////////////////////////////////
// CWatchFilePage property page

IMPLEMENT_DYNCREATE(CWatchFilePage, CPropertyPage)

CWatchFilePage::CWatchFilePage() : CPropertyPage(CWatchFilePage::IDD)
, m_watchFilePath(_T(""))
{
	//{{AFX_DATA_INIT(CWatchFilePage)
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CWatchFilePage::~CWatchFilePage()
{
}

void CWatchFilePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralPage)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_WATCH_FILE_PATH, m_watchFilePath);
}


BEGIN_MESSAGE_MAP(CWatchFilePage, CPropertyPage)
	//{{AFX_MSG_MAP(CWatchFilePage)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_NO_ACTION, OnBnClickedNoAction)
	ON_BN_CLICKED(IDC_KILL_PROCESS, OnBnClickedKillProcess)
	ON_BN_CLICKED(IDC_WATCH_FILE_BROWSE, OnBnClickedWatchFileBrowse)
END_MESSAGE_MAP()

BOOL CWatchFilePage::OnInitDialog(void) 
{
	CPropertyPage::OnInitDialog();

	CheckRadioButton(IDC_NO_ACTION,IDC_KILL_PROCESS,IDC_NO_ACTION+m_setting);
	EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CWatchFilePage::OnBnClickedNoAction()
{
	m_setting = 0;
	EnableControls();
}

void CWatchFilePage::OnBnClickedKillProcess()
{
	m_setting = 1;
	EnableControls();
}

void CWatchFilePage::EnableControls(void)
{
	CWnd *pCWnd;
	int enable = m_setting==1 ? TRUE : FALSE;

	pCWnd = GetDlgItem(IDC_WATCH_FILE_PATH);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_WATCH_FILE_TITLE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
	pCWnd = GetDlgItem(IDC_WATCH_FILE_BROWSE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(enable);
	}
}

void CWatchFilePage::OnBnClickedWatchFileBrowse()
{
	static char szFilter[] = "All Files (*.*)|*.*||";

	CString fileString;
	if (BrowseFile(this,"Specify Watch File",szFilter,m_watchFilePath,TRUE,fileString))
	{
		m_watchFilePath = fileString;
		UpdateData(FALSE);
	}
}


// CWatchdogSheet

IMPLEMENT_DYNAMIC(CWatchdogSheet, CPropertySheet)

CWatchdogSheet::CWatchdogSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags &= ~(PSH_HASHELP);
}

CWatchdogSheet::CWatchdogSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags &= ~(PSH_HASHELP);
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CWatchdogSheet::~CWatchdogSheet()
{
}


BEGIN_MESSAGE_MAP(CWatchdogSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CWatchdogSheet)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchdogSheet message handlers










