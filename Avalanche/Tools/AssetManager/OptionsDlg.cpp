//-----------------------------------------------------------------------------
//	File:		OptionsDlg.cpp
//	Created:	Sep 30, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The COptionsDlg class defines behavior for the options dialog.
//
//	History:	9/30/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#ifndef __ASSETMANAGER__
#include "AssetManager.h"
#endif
#ifndef __OPTIONSDLG__
#include "OptionsDlg.h"
#endif



//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

/////////////////////////////////////////////////////////////////////////////
// CDefaultUpdateDuplicatesPage property page

IMPLEMENT_DYNCREATE(CDefaultUpdateDuplicatesPage, CPropertyPage)

CDefaultUpdateDuplicatesPage::CDefaultUpdateDuplicatesPage() : CPropertyPage(CDefaultUpdateDuplicatesPage::IDD)
{
	//{{AFX_DATA_INIT(CDefaultUpdateDuplicatesPage)
	m_includeFrozenDuplicates = FALSE;
	m_includeParentDirectory = FALSE;
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CDefaultUpdateDuplicatesPage::~CDefaultUpdateDuplicatesPage()
{
}

void CDefaultUpdateDuplicatesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefaultUpdateDuplicatesPage)
	DDX_Check(pDX, IDC_INCLUDE_FROZEN_DUPLICATES, m_includeFrozenDuplicates);
	DDX_Check(pDX, IDC_INCLUDE_PARENT_DIRECTORY, m_includeParentDirectory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefaultUpdateDuplicatesPage, CPropertyPage)
//{{AFX_MSG_MAP(CDefaultUpdateDuplicatesPage)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDefaultAutoFreezePage property page

IMPLEMENT_DYNCREATE(CDefaultAutoFreezePage, CPropertyPage)

CDefaultAutoFreezePage::CDefaultAutoFreezePage() : CPropertyPage(CDefaultAutoFreezePage::IDD)
{
	//{{AFX_DATA_INIT(CDefaultAutoFreezePage)
	m_fileExtensions = _T("");
	m_filePrefixes = _T("");
	m_folderPrefixes = _T("");
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CDefaultAutoFreezePage::~CDefaultAutoFreezePage()
{
}

void CDefaultAutoFreezePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefaultAutoFreezePage)
	DDX_Text(pDX, IDC_FILE_EXTENSIONS, m_fileExtensions);
	DDX_Text(pDX, IDC_FILE_PREFIXES, m_filePrefixes);
	DDX_Text(pDX, IDC_FOLDER_PREFIXES, m_folderPrefixes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefaultAutoFreezePage, CPropertyPage)
//{{AFX_MSG_MAP(CDefaultAutoFreezePage)
	ON_BN_CLICKED(IDC_BUTTON1, OnResetButton)
	ON_BN_CLICKED(IDC_BUTTON2, OnLoadButton)
	ON_BN_CLICKED(IDC_BUTTON3, OnSaveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDefaultAutoFreezePage::OnResetButton() 
{
	if (::AfxMessageBox("Reset Extensions and Prefixes to Default Values?",MB_OKCANCEL) == IDOK)
	{
		m_fileExtensions = ".tmp;.bak";
		m_filePrefixes = "_";
		m_folderPrefixes = "_";
		UpdateData(FALSE);
	}
}

void CDefaultAutoFreezePage::OnLoadButton() 
{
	UpdateData();

	static char szFilter[] = "AutoFreeze Extension Files|*.afe|"  \
										"All Files (*.*)|*.*||";

	static char szFile[] = "AutoFreeze.afe";

	CString fileString;
	if (BrowseFile(this,"Load Autofreeze Extension File",szFilter,szFile,TRUE,fileString))
	{
		TupperwareAggregate *pRootAggregate;
		if (TupperwareLoadSaveManager::Read(fileString,&pRootAggregate))
		{
			TupperwareScalar *pFileExtensionsScalar = pRootAggregate->FindScalarByKey("AutoFreezeFileExtensions");
			if (pFileExtensionsScalar && pFileExtensionsScalar->GetDataType()==TupperwareScalar::DATA_STRING)
			{
				m_fileExtensions = pFileExtensionsScalar->GetAsString();
			}
			TupperwareScalar *pFilePrefixesScalar = pRootAggregate->FindScalarByKey("AutoFreezeFilePrefixes");
			if (pFilePrefixesScalar && pFilePrefixesScalar->GetDataType()==TupperwareScalar::DATA_STRING)
			{
				m_filePrefixes = pFilePrefixesScalar->GetAsString();
			}
			TupperwareScalar *pFolderPrefixesScalar = pRootAggregate->FindScalarByKey("AutoFreezeFolderPrefixes");
			if (pFolderPrefixesScalar && pFolderPrefixesScalar->GetDataType()==TupperwareScalar::DATA_STRING)
			{
				m_folderPrefixes = pFolderPrefixesScalar->GetAsString();
			}
			delete pRootAggregate;
			UpdateData(FALSE);
		}
	}
}

void CDefaultAutoFreezePage::OnSaveButton() 
{
	UpdateData();

	static char szFilter[] = "AutoFreeze Extension Files|*.afe|"  \
		"All Files (*.*)|*.*||";
	
	static char szFile[] = "AutoFreeze.afe";
	
	CString fileString;
	if (BrowseFile(this,"Save Autofreeze Extension File",szFilter,szFile,FALSE,fileString))
	{
		TupperwareAggregate *pRootAggregate = new TupperwareAggregate;

		pRootAggregate->AddScalarString("AutoFreezeFileExtensions",NULL,m_fileExtensions);
		pRootAggregate->AddScalarString("AutoFreezeFilePrefixes",NULL,m_filePrefixes);
		pRootAggregate->AddScalarString("AutoFreezeFolderPrefixes",NULL,m_folderPrefixes);

		TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,fileString,pRootAggregate);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSetAutoFreezePage property page

IMPLEMENT_DYNCREATE(CFolderSetAutoFreezePage, CPropertyPage)

CFolderSetAutoFreezePage::CFolderSetAutoFreezePage() : CPropertyPage(CFolderSetAutoFreezePage::IDD)
{
	//{{AFX_DATA_INIT(CFolderSetAutoFreezePage)
	m_fileExtensions = _T("");
	m_filePrefixes = _T("");
	m_folderPrefixes = _T("");
	m_bUseDefaultAutoExclude = FALSE;
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CFolderSetAutoFreezePage::~CFolderSetAutoFreezePage()
{
}

void CFolderSetAutoFreezePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFolderSetAutoFreezePage)
	DDX_Text(pDX, IDC_FILE_EXTENSIONS, m_fileExtensions);
	DDX_Text(pDX, IDC_FILE_PREFIXES, m_filePrefixes);
	DDX_Text(pDX, IDC_FOLDER_PREFIXES, m_folderPrefixes);
	DDX_Check(pDX, IDC_CHECK1, m_bUseDefaultAutoExclude);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFolderSetAutoFreezePage, CPropertyPage)
//{{AFX_MSG_MAP(CFolderSetAutoFreezePage)
	ON_BN_CLICKED(IDC_BUTTON1, OnResetButton)
	ON_BN_CLICKED(IDC_BUTTON2, OnLoadButton)
	ON_BN_CLICKED(IDC_BUTTON3, OnSaveButton)
	ON_BN_CLICKED(IDC_CHECK1, OnUseAutoFreezeClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFolderSetAutoFreezePage::OnResetButton() 
{
	if (::AfxMessageBox("Reset Extensions and Prefixes to Default Values?",MB_OKCANCEL) == IDOK)
	{
		m_fileExtensions = ".tmp;.bak";
		m_filePrefixes = "_";
		m_folderPrefixes = "_";
		UpdateData(FALSE);
	}
}

void CFolderSetAutoFreezePage::OnLoadButton() 
{
	UpdateData();

	static char szFilter[] = "AutoFreeze Extension Files|*.afe|"  \
										"All Files (*.*)|*.*||";

	static char szFile[] = "AutoFreeze.afe";

	CString fileString;
	if (BrowseFile(this,"Load Autofreeze Extension File",szFilter,szFile,TRUE,fileString))
	{
		TupperwareAggregate *pRootAggregate;
		if (TupperwareLoadSaveManager::Read(fileString,&pRootAggregate))
		{
			TupperwareScalar *pFileExtensionsScalar = pRootAggregate->FindScalarByKey("AutoFreezeFileExtensions");
			if (pFileExtensionsScalar && pFileExtensionsScalar->GetDataType()==TupperwareScalar::DATA_STRING)
			{
				m_fileExtensions = pFileExtensionsScalar->GetAsString();
			}
			TupperwareScalar *pFilePrefixesScalar = pRootAggregate->FindScalarByKey("AutoFreezeFilePrefixes");
			if (pFilePrefixesScalar && pFilePrefixesScalar->GetDataType()==TupperwareScalar::DATA_STRING)
			{
				m_filePrefixes = pFilePrefixesScalar->GetAsString();
			}
			TupperwareScalar *pFolderPrefixesScalar = pRootAggregate->FindScalarByKey("AutoFreezeFolderPrefixes");
			if (pFolderPrefixesScalar && pFolderPrefixesScalar->GetDataType()==TupperwareScalar::DATA_STRING)
			{
				m_folderPrefixes = pFolderPrefixesScalar->GetAsString();
			}
			delete pRootAggregate;
			UpdateData(FALSE);
		}
	}
}

void CFolderSetAutoFreezePage::OnSaveButton() 
{
	UpdateData();

	static char szFilter[] = "AutoFreeze Extension Files|*.afe|"  \
		"All Files (*.*)|*.*||";
	
	static char szFile[] = "AutoFreeze.afe";
	
	CString fileString;
	if (BrowseFile(this,"Save Autofreeze Extension File",szFilter,szFile,FALSE,fileString))
	{
		TupperwareAggregate *pRootAggregate = new TupperwareAggregate;

		pRootAggregate->AddScalarString("AutoFreezeFileExtensions",NULL,m_fileExtensions);
		pRootAggregate->AddScalarString("AutoFreezeFilePrefixes",NULL,m_filePrefixes);
		pRootAggregate->AddScalarString("AutoFreezeFolderPrefixes",NULL,m_folderPrefixes);

		TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,fileString,pRootAggregate);
		delete pRootAggregate;
	}
}

BOOL CFolderSetAutoFreezePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	EnableControls(m_bUseDefaultAutoExclude ? FALSE : TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFolderSetAutoFreezePage::OnUseAutoFreezeClicked() 
{
	UpdateData(TRUE);
	EnableControls(m_bUseDefaultAutoExclude ? FALSE : TRUE);
}

void CFolderSetAutoFreezePage::EnableControls(BOOL bEnabled) 
{
	CWnd *pCWnd;
	pCWnd = GetDlgItem(IDC_FILE_EXTENSIONS);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
	pCWnd = GetDlgItem(IDC_FILE_PREFIXES);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
	pCWnd = GetDlgItem(IDC_FOLDER_PREFIXES);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
	pCWnd = GetDlgItem(IDC_TEXT1);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
	pCWnd = GetDlgItem(IDC_TEXT2);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
	pCWnd = GetDlgItem(IDC_TEXT3);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
	pCWnd = GetDlgItem(IDC_BUTTON1);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
	pCWnd = GetDlgItem(IDC_BUTTON2);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
	pCWnd = GetDlgItem(IDC_BUTTON3);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(bEnabled);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAutoFreezeListPage property page

IMPLEMENT_DYNCREATE(CAutoFreezeListPage, CPropertyPage)

CAutoFreezeListPage::CAutoFreezeListPage() : CPropertyPage(CAutoFreezeListPage::IDD)
{
	//{{AFX_DATA_INIT(CAutoFreezeListPage)
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
	m_changed = false;
}

CAutoFreezeListPage::~CAutoFreezeListPage()
{
}

void CAutoFreezeListPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoFreezeListPage)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoFreezeListPage, CPropertyPage)
//{{AFX_MSG_MAP(CAutoFreezeListPage)
	ON_BN_CLICKED(IDC_BUTTON1, OnClearAll)
	ON_BN_CLICKED(IDC_BUTTON2, OnClearUnused)
	ON_BN_CLICKED(IDC_BUTTON3, OnLoad)
	ON_BN_CLICKED(IDC_BUTTON4, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAutoFreezeListPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_list.SetHorizontalExtent(900);
	int numEntries = m_entries.GetSize();

	for (int index=0;index<numEntries;index++)
	{
		int newIndex = m_list.AddString(m_entries[index]);
		m_list.SetItemData(newIndex,index);
	}
	CheckClearUnused();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAutoFreezeListPage::OnClearAll() 
{
	if (m_list.GetCount())
	{
		m_list.ResetContent();
		m_changed = true;
	}
}

void CAutoFreezeListPage::OnClearUnused() 
{
	int numEntries = m_list.GetCount();
	for (int index=numEntries-1;index>=0;index--)
	{
		int arrayIndex = m_list.GetItemData(index);
		if (!m_entryUsed[arrayIndex])
		{
			m_list.DeleteString(index);
			m_changed = true;
		}
	}
	CWnd *pCWnd = GetDlgItem(IDC_BUTTON2);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		pCWnd->EnableWindow(FALSE);
	}
}

void CAutoFreezeListPage::CheckClearUnused(void)
{
	bool bAllUsed = true;
	for (int index=0;index<m_entryUsed.GetSize();index++)
	{
		if (!m_entryUsed[index])
		{
			bAllUsed = false;
			break;
		}
	}
	if (bAllUsed)
	{
		CWnd *pCWnd = GetDlgItem(IDC_BUTTON2);
		if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
		{
			pCWnd->EnableWindow(FALSE);
		}
	}
}

void CAutoFreezeListPage::OnLoad() 
{
	static char szFilter[] = "AutoFreeze Paths Files|*.afp|"  \
		"All Files (*.*)|*.*||";
	
	static char szFile[] = "AutoFreeze.afp";
	
	CString fileString;
	if (BrowseFile(this,"Load Autofreeze Paths File",szFilter,szFile,TRUE,fileString))
	{
		TupperwareAggregate *pRootAggregate;
		if (TupperwareLoadSaveManager::Read(fileString,&pRootAggregate))
		{
			TupperwareList *pFileListList = pRootAggregate->FindListByKey("AutoFreezePaths");
			if (pFileListList && pFileListList->GetDataType()==TupperwareList::DATA_STRING)
			{
				pFileListList->GetAsString(m_entries);
				int numEntries = m_entries.GetSize();

				m_entryUsed.SetSize(m_entries.GetSize());

				m_list.ResetContent();
				for (int index=0;index<numEntries;index++)
				{
					m_entryUsed[index] = true;
					int newIndex = m_list.AddString(m_entries[index]);
					m_list.SetItemData(newIndex,index);
				}
				// disable the clear unused button
				CWnd *pCWnd = GetDlgItem(IDC_BUTTON2);
				if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
				{
					pCWnd->EnableWindow(FALSE);
				}
			}
			delete pRootAggregate;
			m_changed = true;
		}
	}
}

void CAutoFreezeListPage::OnSave() 
{
	static char szFilter[] = "AutoFreeze List Files|*.afp|"  \
		"All Files (*.*)|*.*||";
	
	static char szFile[] = "AutoFreeze.afp";
	
	CString fileString;
	if (BrowseFile(this,"Save Autofreeze Paths File",szFilter,szFile,FALSE,fileString))
	{
		TupperwareAggregate *pRootAggregate = new TupperwareAggregate;

		TupArray<TupString> newEntries;
		int numEntriesInCtrl = m_list.GetCount();
		for (int index=0;index<numEntriesInCtrl;index++)
		{
			CString cstring;
			m_list.GetText(index,cstring);
			TupString tupString(cstring);
			newEntries.Add(tupString);
		}
		
		pRootAggregate->AddListString("AutoFreezePaths",NULL,newEntries);
		TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,fileString,pRootAggregate);
		delete pRootAggregate;
	}
}

void CAutoFreezeListPage::OnOK() 
{
	int numPaths = m_list.GetCount();
	m_entries.RemoveAll();
	for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
	{
		CString cstring;
		m_list.GetText(pathIndex,cstring);
		TupString tupString(cstring);
		m_entries.Add(tupString);
	}
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CScanSettingsPage property page

IMPLEMENT_DYNCREATE(CScanSettingsPage, CPropertyPage)

CScanSettingsPage::CScanSettingsPage() : CPropertyPage(CScanSettingsPage::IDD)
, m_threadPriority(0)
{
	//{{AFX_DATA_INIT(CScanSettingsPage)
	m_bDoCRCCheck = FALSE;
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CScanSettingsPage::~CScanSettingsPage()
{
}

void CScanSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefaultUpdateDuplicatesPage)
	DDX_Check(pDX, IDC_CHECK1, m_bDoCRCCheck);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_COMBO1, m_threadPriority);
}


BEGIN_MESSAGE_MAP(CScanSettingsPage, CPropertyPage)
//{{AFX_MSG_MAP(CScanSettingsPage)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncSettingsPage property page

IMPLEMENT_DYNCREATE(CSyncSettingsPage, CPropertyPage)

CSyncSettingsPage::CSyncSettingsPage() : CPropertyPage(CSyncSettingsPage::IDD)
, m_threadPriority(0)
{
	//{{AFX_DATA_INIT(CSyncSettingsPage)
	m_bUseSyncDates = FALSE;
	m_bVerifyCopiedFiles = FALSE;
	m_bCloseWhenCompleted = FALSE;
	m_bExitWhenClosed = FALSE;
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CSyncSettingsPage::~CSyncSettingsPage()
{
}

void CSyncSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyncSettingsPage)
	DDX_Check(pDX, IDC_CHECK1, m_bUseSyncDates);
	DDX_Check(pDX, IDC_CHECK2, m_bVerifyCopiedFiles);
	DDX_Check(pDX, IDC_CHECK3, m_bCloseWhenCompleted);
	DDX_Check(pDX, IDC_CHECK4, m_bExitWhenClosed);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_COMBO1, m_threadPriority);
}


BEGIN_MESSAGE_MAP(CSyncSettingsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSyncSettingsPage)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// COptionSheet

IMPLEMENT_DYNAMIC(COptionSheet, CPropertySheet)

COptionSheet::COptionSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags &= ~(PSH_HASHELP);
}

COptionSheet::COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags &= ~(PSH_HASHELP);
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

COptionSheet::~COptionSheet()
{
}


BEGIN_MESSAGE_MAP(COptionSheet, CPropertySheet)
//{{AFX_MSG_MAP(COptionSheet)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionSheet message handlers





/////////////////////////////////////////////////////////////////////////////
// CMyListBox

CMyListBox::CMyListBox()
{
}

CMyListBox::~CMyListBox()
{
}


BEGIN_MESSAGE_MAP(CMyListBox, CListBox)
	//{{AFX_MSG_MAP(CMyListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListBox message handlers

BOOL CMyListBox::PreTranslateMessage(MSG* pMsg) 
{
	// process any VK_DELETE key presses
	if (pMsg->message == WM_KEYDOWN && (int)pMsg->wParam == VK_DELETE)
	{
		int selectionIndex = GetCurSel();
		DeleteString(selectionIndex);
		if (selectionIndex>=GetCount())
			selectionIndex--;
		SetCurSel(selectionIndex);

		return TRUE;
	}
	return CListBox::PreTranslateMessage(pMsg);
}

