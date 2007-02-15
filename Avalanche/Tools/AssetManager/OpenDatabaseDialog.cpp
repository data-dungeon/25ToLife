////////////////////////////////////////////////////////////////////////////
//
// OpenDatabaseDialog
//
// Dialog for managing and picking the folders
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: OpenDatabaseDialog.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/08/03    Time: 12:57p
 * Updated in $/Avalanche/tools/AssetManager
 * size of folder set gui
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

#include "stdafx.h"
#include "assetmanager.h"
#include "OpenDatabaseDialog.h"
#include "FolderSet.h"
#include "FolderSets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_ROOT		"c:\\"
#define MAX_MRU_IN_COMBO	10

/////////////////////////////////////////////////////////////////////////////
// OpenDatabaseDialog dialog


OpenDatabaseDialog::OpenDatabaseDialog(CWnd* pParent /*=NULL*/)
	: CDialog(OpenDatabaseDialog::IDD, pParent),
	m_pWorkingFolderSets(NULL),
	m_bClosing(false),
	m_hIconBrowse(NULL)
{
	m_pWorkingFolderSets =  new FolderSets;
	m_pWorkingFolderSet =  new FolderSet;
		
	//{{AFX_DATA_INIT(OpenDatabaseDialog)
	//}}AFX_DATA_INIT
}

OpenDatabaseDialog::~OpenDatabaseDialog()
{
	delete m_pWorkingFolderSet;
	delete m_pWorkingFolderSets;
}


void OpenDatabaseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OpenDatabaseDialog)
	DDX_Control(pDX, ID_OK_BUTTON, m_okButton);
	DDX_Control(pDX, ID_NEW_DATABASE, m_newDatabaseButton);
	DDX_Control(pDX, ID_EDIT_DATABASE, m_editDatabaseButton);
	DDX_Control(pDX, ID_DELETE_DATABASE, m_deleteDatabaseButton);
	DDX_Control(pDX, ID_CANCEL_BUTTON, m_cancelButton);
	DDX_Control(pDX, ID_ACCEPT_DATABASE, m_acceptDatabaseButton);
	DDX_Control(pDX, IDC_BUTTON_LEFT_BROWSE, m_leftBrowseButton);
	DDX_Control(pDX, IDC_BUTTON_RIGHT_BROWSE, m_rightBrowseButton);
	DDX_Control(pDX, IDC_COMBO_LEFT, m_leftFolderCombo);
	DDX_Control(pDX, IDC_COMBO_RIGHT, m_rightFolderCombo);
	DDX_Control(pDX, IDC_DATABASE_DESCRIPTION, m_databaseDescription);
	DDX_Control(pDX, IDC_DATABASE_NAME, m_databaseName);
	DDX_Control(pDX, IDC_FOLDER_SETS, m_folderSetsListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OpenDatabaseDialog, CDialog)
	//{{AFX_MSG_MAP(OpenDatabaseDialog)
	ON_BN_CLICKED(ID_OK_BUTTON, OnOkButton)
	ON_BN_CLICKED(ID_NEW_DATABASE, OnNewDatabase)
	ON_BN_CLICKED(ID_EDIT_DATABASE, OnEditDatabase)
	ON_BN_CLICKED(ID_DELETE_DATABASE, OnDeleteDatabase)
	ON_BN_CLICKED(ID_ACCEPT_DATABASE, OnAcceptDatabase)
	ON_BN_CLICKED(ID_CANCEL_BUTTON, OnCancelButton)
	ON_LBN_SELCHANGE(IDC_FOLDER_SETS, OnSelchangeFolderSets)
	ON_BN_CLICKED(IDC_BUTTON_LEFT_BROWSE, OnButtonLeftBrowse)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_BROWSE, OnButtonRightBrowse)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OpenDatabaseDialog message handlers

BOOL OpenDatabaseDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT(m_pWorkingFolderSets);

	m_hIconBrowse = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 0, 0, 0);
	m_leftBrowseButton.SetIcon(m_hIconBrowse);
	m_rightBrowseButton.SetIcon(m_hIconBrowse);

	SetupFolderSetsListBox();
	SetupWorkingSet();
		int numEnabledSets = GetNumEnabledSets();
	if (numEnabledSets)
	{
		m_mode = PICK_MODE;
	}
	else
	{
		m_mode = NEW_MODE;
	}
	SetupButtons();
	SetupWorkingSetControls();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void OpenDatabaseDialog::SetFolderSets(const FolderSets &folderSets)
{
	(*m_pWorkingFolderSets) = folderSets;
}

const FolderSets &OpenDatabaseDialog::GetFolderSets(void) const
{
	return (*m_pWorkingFolderSets);
}

void OpenDatabaseDialog::OnOkButton() 
{
	if(VerifyWorkingSet())
	{
		EndDialog(TRUE);
	}
}

void OpenDatabaseDialog::OnNewDatabase() 
{
	m_mode = NEW_MODE;
	SetupButtons();
	SetupWorkingSetControls();
}

void OpenDatabaseDialog::OnEditDatabase() 
{
	m_mode = EDIT_MODE;
	SetupButtons();
	SetupWorkingSetControls();
}

void OpenDatabaseDialog::OnDeleteDatabase() 
{
	int activeSet = m_pWorkingFolderSets->GetActiveSet();
	m_pWorkingFolderSets->SetEnabled(activeSet,false);
	SetupFolderSetsListBox();
	SetupWorkingSet();
	SetupButtons();
	SetupWorkingSetControls();
}

void OpenDatabaseDialog::OnAcceptDatabase() 
{
	if(VerifyWorkingSet())
	{
		switch(m_mode) 
		{
			case EDIT_MODE:
				{
					int activeSet = m_pWorkingFolderSets->GetActiveSet();
					UpdateFromWorkingSet(activeSet);
				}
				break;
			case NEW_MODE:
				{
					int newSet = m_pWorkingFolderSets->CreateNew();
					UpdateFromWorkingSet(newSet);
					m_pWorkingFolderSets->SetActiveSet(newSet);
				}
				break;
		}
		m_mode = PICK_MODE;
		SetupFolderSetsListBox();
		SetupWorkingSet();
		SetupButtons();
		SetupWorkingSetControls();
	}
}

void OpenDatabaseDialog::OnCancelButton() 
{
	if (m_mode==PICK_MODE)
	{
		EndDialog(FALSE); // we are done and not going to use whatever we did
	}
	else
	{
		int numEnabledSets = GetNumEnabledSets();
		if (numEnabledSets)
		{
			m_mode = PICK_MODE;
			SetupFolderSetsListBox();
			SetupWorkingSet();
			SetupButtons();
			SetupWorkingSetControls();
		}
		else
		{
			EndDialog(FALSE); // we are done and not going to use whatever we did
		}
	}
}

void OpenDatabaseDialog::OnSelchangeFolderSets() 
{
	int selectionIndex = m_folderSetsListBox.GetCurSel();
	if (selectionIndex!=-1)
	{
		int setIndex = m_folderSetsListBox.GetItemData(selectionIndex);
		m_pWorkingFolderSets->SetActiveSet(setIndex);
		SetupWorkingSet();
		SetupButtons();
		SetupWorkingSetControls();
	}
}

void OpenDatabaseDialog::OnButtonLeftBrowse() 
{
	CString comboText;
	m_leftFolderCombo.GetWindowText(comboText);
	if (comboText.GetLength()==0)
	{
		comboText = DEFAULT_ROOT;
	}
	// Ask the user for the folder to browse for:
	CString strFolderPicked = ShowFolderPickDlg(IDS_SPECIFY_LEFT_FOLDER, comboText);
	// set only if result has something
	if (!strFolderPicked.IsEmpty())
		m_leftFolderCombo.SetWindowText(strFolderPicked);
}

void OpenDatabaseDialog::OnButtonRightBrowse() 
{
	CString comboText;
	m_rightFolderCombo.GetWindowText(comboText);
	if (comboText.GetLength()==0)
	{
		comboText = DEFAULT_ROOT;
	}
	// Ask the user for the folder to browse for:
	CString strFolderPicked = ShowFolderPickDlg(IDS_SPECIFY_RIGHT_FOLDER, comboText);
	// set only if result has something
	if (!strFolderPicked.IsEmpty())
		m_rightFolderCombo.SetWindowText(strFolderPicked);
}

//-----------------------------------------------------------------------------
// OpenDatabaseDialog::FolderPickCallbackProc
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
int CALLBACK OpenDatabaseDialog::FolderPickCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch(uMsg)
	{
		case BFFM_INITIALIZED:
		{
			// Get the root directory to use:
			CString strRootDir;
			OpenDatabaseDialog* pODDlg = ((OpenDatabaseDialog*) lpData);
			if (pODDlg != NULL)
				strRootDir = pODDlg->GetTempRootDir();
			else
				strRootDir = DEFAULT_ROOT;
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
// OpenDatabaseDialog::ShowFolderPickDlg
// This method can be called to show a folder picker dialog.  It will return
// the path to the folder picked.
//
// Input:
//	UINT nCaptionStringID -- The id of the string resource to show in the
//							 caption of the dialog.
//	CString strRootDir -- The root directory to specify for the dialog.
//
// Output: CString representing the folder chosen.  This will be empty if the
//		   user canceled out of the dialog, or for some reason did not specify
//		   a folder.
//-----------------------------------------------------------------------------
CString OpenDatabaseDialog::ShowFolderPickDlg(UINT nCaptionStringID, CString strRootDir)
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
	CHAR charBuff[200];
	LoadString(AfxGetResourceHandle(), nCaptionStringID, charBuff, sizeof(charBuff));
	bi.hwndOwner = GetSafeHwnd();
	bi.lpszTitle = charBuff;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_USENEWUI;
	bi.lpfn = OpenDatabaseDialog::FolderPickCallbackProc;
	bi.lParam = (LPARAM) this;

	// Set the temp root directory:
	SetTempRootDir(strRootDir);

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


#if 0
//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnButtonLeftBrowse
// This handler will be called whenever the user clicks on the left folder
// browse button.  It will bring up a browse window for the user to choose
// the left folder, and will add the entry to the combo box accordingly.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnButtonLeftBrowse() 
{
	// Get the current folder from the left combobox:
	CString strRootDir = DEFAULT_ROOT;
	int nCurSel = m_comboLeft.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		m_comboLeft.GetLBText(nCurSel, strRootDir);
	}

	// Ask the user for the folder to browse for:
	CString strFolderPicked = ShowFolderPickDlg(IDS_SPECIFY_LEFT_FOLDER, strRootDir);

	// If the string is not null, then set the entry in the combo box:
	if (strFolderPicked != "")
	{
		// Check to see if the picked folder is the same as what is currently
		// chosen:
		if (strFolderPicked.CompareNoCase(strRootDir) != 0)
		{
			// Remove any duplicates:
			RemoveStrFromCombo(m_comboLeft, strFolderPicked);

			// Now set the combobox accordingly:
			int nNumFolders = m_comboLeft.GetCount();
			m_comboLeft.InsertString(0, strFolderPicked);
			m_comboLeft.SetCurSel(0);
			if (nNumFolders == MAX_MRU_IN_COMBO)
			{
				m_comboLeft.DeleteString(nNumFolders);
			}

			// Now set up the left tree control:
			delete m_pLeftDataBase;
			m_pLeftDataBase = NULL;
			LoadDatabases();
		}
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnButtonRightBrowse
// This handler will be called whenever the user clicks on the right folder
// browse button.  It will bring up a browse window for the user to choose
// the right folder, and will add the entry to the combo box accordingly.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnButtonRightBrowse() 
{
	// Get the current folder from the right combobox:
	CString strRootDir = DEFAULT_ROOT;
	int nCurSel = m_comboRight.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		m_comboRight.GetLBText(nCurSel, strRootDir);
	}

	// Ask the user for the folder to browse for:
	CString strFolderPicked = ShowFolderPickDlg(IDS_SPECIFY_RIGHT_FOLDER, strRootDir);

	// If the string is not null, then set the entry in the combo box:
	if (strFolderPicked != "")
	{
		// Check to see if the picked folder is the same as what is currently
		// chosen:
		if (strFolderPicked.CompareNoCase(strRootDir) != 0)
		{
			// Remove any duplicates:
			RemoveStrFromCombo(m_comboRight, strFolderPicked);

			// Now set the combobox accordingly:
			int nNumFolders = m_comboRight.GetCount();
			m_comboRight.InsertString(0, strFolderPicked);
			m_comboRight.SetCurSel(0);
			if (nNumFolders == MAX_MRU_IN_COMBO)
			{
				m_comboRight.DeleteString(nNumFolders);
			}

			// Now set up the right tree control:
			delete m_pRightDataBase;
			m_pRightDataBase = NULL;
			LoadDatabases();
		}
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnSelChangeLeftCombo
// This message handler will be called when the user has changed the selection
// in the left combobox.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnSelChangeLeftCombo()
{
	// Get the new selected item:
	int nCurrSel = m_comboLeft.GetCurSel();

	// If the user has selected item zero, it is already loaded in the pane, so
	// don't do anything:
	if (nCurrSel != 0)
	{
		// Get the text:
		CString strSelected;
		m_comboLeft.GetLBText(nCurrSel, strSelected);

		// Make sure the path is valid:
		if (::PathFileExists((LPCTSTR) strSelected) == TRUE)
		{
			// Move this item to item zero:
			m_comboLeft.DeleteString(nCurrSel);
			m_comboLeft.InsertString(0, strSelected);
			m_comboLeft.SetCurSel(0);

			// Now set up the left tree control:
			delete m_pLeftDataBase;
			m_pLeftDataBase = NULL;
			LoadDatabases();
		}
		else
		{
			// Path is not valid, so tell the user that:
			CString strNoFolder;
			strNoFolder.LoadString(IDS_FOLDER_DOESNT_EXIST);
			CString strPickAnother;
			strPickAnother.LoadString(IDS_PICK_ANOTHER);
			CString strInvalidPath;
			strInvalidPath = strNoFolder + "\n\n" + strSelected + "\n\n" + strPickAnother;
			::AfxMessageBox((LPCTSTR) strInvalidPath, MB_OK | MB_ICONEXCLAMATION);

			// Get rid of the entry and set selection back to entry 0:
			m_comboLeft.DeleteString(nCurrSel);
			m_comboLeft.SetCurSel(0);
		}
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnSelChangeRightCombo
// This message handler will be called when the user has changed the selection
// in the right combobox.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnSelChangeRightCombo()
{
	// Get the new selected item:
	int nCurrSel = m_comboRight.GetCurSel();

	// If the user has selected item zero, it is already loaded in the pane, so
	// don't do anything:
	if (nCurrSel != 0)
	{
		// Get the text:
		CString strSelected;
		m_comboRight.GetLBText(nCurrSel, strSelected);

		// Make sure the path is valid:
		if (::PathFileExists((LPCTSTR) strSelected) == TRUE)
		{
			// Move this item to item zero:
			m_comboRight.DeleteString(nCurrSel);
			m_comboRight.InsertString(0, strSelected);
			m_comboRight.SetCurSel(0);

			// Now set up the left tree control:
			delete m_pRightDataBase;
			m_pRightDataBase = NULL;
			LoadDatabases();
		}
		else
		{
			// Path is not valid, so tell the user that:
			CString strNoFolder;
			strNoFolder.LoadString(IDS_FOLDER_DOESNT_EXIST);
			CString strPickAnother;
			strPickAnother.LoadString(IDS_PICK_ANOTHER);
			CString strInvalidPath;
			strInvalidPath = strNoFolder + "\n\n" + strSelected + "\n\n" + strPickAnother;
			::AfxMessageBox((LPCTSTR) strInvalidPath, MB_OK | MB_ICONEXCLAMATION);

			// Get rid of the entry and set selection back to entry 0:
			m_comboRight.DeleteString(nCurrSel);
			m_comboRight.SetCurSel(0);
		}
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnKillFocusLeftCombo
// This message handler is called when the focus has left the left combo box.
// It will check to see if the user has typed a valid folder path in the edit
// control of the combo box.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnKillFocusLeftCombo()
{
	if (m_bClosing)	// if we are in the middle of exiting don't worry about it
		return;

	if (m_bIgnoreFocus)
		return;

	// Get the text from the edit control and see if it matches item 0:
	CString strEditText;
	m_comboLeft.GetWindowText(strEditText);
	int nNumItems = m_comboLeft.GetCount();
	if (nNumItems)
	{
		CString strItem0;
		m_comboLeft.GetLBText(0, strItem0);
		if (strEditText.CompareNoCase(strItem0) ==0)
		{
			return; // if it is the same then we are done
		}
	}
	// If the string is not null, then set the entry in the combo box:
	if (strEditText != "")
	{
		if (::PathFileExists((LPCTSTR) strEditText) == TRUE)
		{
			// delete any matches from list
			int nNumFolders;
			nNumFolders = m_comboLeft.GetCount();
			int index;
			for (index=nNumFolders-1;index>=0;index--)
			{
				CString strItem;
				m_comboLeft.GetLBText(index,strItem);
				if (strEditText.CompareNoCase(strItem)==0)
				{
					m_comboLeft.DeleteString(index);
				}
			}
			// put this item at the top
			m_comboLeft.InsertString(0, strEditText);
			// select the first entry
			m_comboLeft.SetCurSel(0);
			// delete any off the bottom
			while (m_comboLeft.GetCount() > MAX_MRU_IN_COMBO)
			{
				m_comboLeft.DeleteString(m_comboLeft.GetCount()-1);
			}
			// Now set up the left tree control:
			delete m_pLeftDataBase;
			m_pLeftDataBase = NULL;
			LoadDatabases();
		}
		else
		{
			// Path is not valid, so tell the user that:
			CString strNoFolder;
			strNoFolder.LoadString(IDS_FOLDER_DOESNT_EXIST);
			CString strPickAnother;
			strPickAnother.LoadString(IDS_PICK_ANOTHER);
			CString strInvalidPath;
			strInvalidPath = strNoFolder + "\n\n" + strEditText + "\n\n" + strPickAnother;
			::AfxMessageBox((LPCTSTR) strInvalidPath, MB_OK | MB_ICONEXCLAMATION);
			m_bIgnoreFocus = true;
			m_comboLeft.SetFocus();
			m_bIgnoreFocus = false;
		}
	}
	else
	{
		delete m_pLeftDataBase;
		m_pLeftDataBase = NULL;
		LoadDatabases();
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnKillFocusRightCombo
// This message handler is called when the focus has left the right combo box.
// It will check to see if the user has typed a valid folder path in the edit
// control of the combo box.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnKillFocusRightCombo()
{
	if (m_bClosing)
		return;

	if (m_bIgnoreFocus)
		return;

	// Get the text from the edit control and see if it matches item 0:
	CString strEditText;
	m_comboRight.GetWindowText(strEditText);
	int nNumItems = m_comboRight.GetCount();
	if (nNumItems)
	{
		CString strItem0;
		m_comboRight.GetLBText(0, strItem0);
		if (strEditText.CompareNoCase(strItem0) ==0)
		{
			return; // if it is the same then we are done
		}
	}
	// If the string is not null, then set the entry in the combo box:
	if (strEditText != "")
	{
		if (::PathFileExists((LPCTSTR) strEditText) == TRUE)
		{
			// delete any matches from list
			int nNumFolders;
			nNumFolders = m_comboRight.GetCount();
			int index;
			for (index=nNumFolders-1;index>=0;index--)
			{
				CString strItem;
				m_comboRight.GetLBText(index,strItem);
				if (strEditText.CompareNoCase(strItem)==0)
				{
					m_comboLeft.DeleteString(index);
				}
			}
			// put this item at the top
			m_comboRight.InsertString(0, strEditText);
			// select the first entry
			m_comboRight.SetCurSel(0);
			// delete any off the bottom
			while (m_comboRight.GetCount() > MAX_MRU_IN_COMBO)
			{
				m_comboRight.DeleteString(m_comboLeft.GetCount()-1);
			}

			// Now set up the right tree control:
			delete m_pRightDataBase;
			m_pRightDataBase = NULL;
			LoadDatabases();
		}
		else
		{
			// Path is not valid, so tell the user that:
			CString strNoFolder;
			strNoFolder.LoadString(IDS_FOLDER_DOESNT_EXIST);
			CString strPickAnother;
			strPickAnother.LoadString(IDS_PICK_ANOTHER);
			CString strInvalidPath;
			strInvalidPath = strNoFolder + "\n\n" + strEditText + "\n\n" + strPickAnother;
			::AfxMessageBox((LPCTSTR) strInvalidPath, MB_OK | MB_ICONEXCLAMATION);
			m_bIgnoreFocus = true;
			m_comboRight.SetFocus();
			m_bIgnoreFocus = false;
		}
	}
	else
	{
		delete m_pRightDataBase;
		m_pRightDataBase = NULL;
		LoadDatabases();
	}
}

#endif

void OpenDatabaseDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

void OpenDatabaseDialog::OnOK()
{
	// We just want to absorb this message, thus keeping the app from quitting
	// when the user hits the enter key:
	SetFocus();
}

void OpenDatabaseDialog::OnCancel()
{
	if (m_bClosing)
	{
		CDialog::OnCancel();
	}
}

int OpenDatabaseDialog::GetNumEnabledSets(void) const
{
	int numEnabledSets = 0;
	int numFolderSets = m_pWorkingFolderSets->GetNumFolderSets();
	for (int folderSetIndex = 0;folderSetIndex<numFolderSets;folderSetIndex++)
	{
		if (m_pWorkingFolderSets->IsEnabled(folderSetIndex))
			numEnabledSets++;
	}
	return numEnabledSets;
}

void OpenDatabaseDialog::SetupFolderSetsListBox(void)
{
	m_folderSetsListBox.ResetContent();
//	int tabStops[1];
//	tabStops[0] = 200;
//	m_folderSetsListBox.SetTabStops(1,tabStops);
	int activeSet = m_pWorkingFolderSets->GetActiveSet();
	int numFolderSets = m_pWorkingFolderSets->GetNumFolderSets();
	for (int folderSetIndex = 0;folderSetIndex<numFolderSets;folderSetIndex++)
	{
		if (m_pWorkingFolderSets->IsEnabled(folderSetIndex))
		{
			TupString listString = m_pWorkingFolderSets->GetName(folderSetIndex);
			
			int listBoxIndex = m_folderSetsListBox.AddString(listString);
			m_folderSetsListBox.SetItemData(listBoxIndex,folderSetIndex);
			if (folderSetIndex==activeSet)
				m_folderSetsListBox.SetCurSel(listBoxIndex);
		}
	}
}

void OpenDatabaseDialog::SetupWorkingSet(void)
{
	m_pWorkingFolderSet->Init();
	int activeSet = m_pWorkingFolderSets->GetActiveSet();
	if (activeSet!=-1)
	{
		m_pWorkingFolderSet->SetName(m_pWorkingFolderSets->GetName(activeSet));
		m_pWorkingFolderSet->SetDescription(m_pWorkingFolderSets->GetDescription(activeSet));
		m_pWorkingFolderSet->SetLeftFolderPath(m_pWorkingFolderSets->GetLeftFolderPath(activeSet));
		m_pWorkingFolderSet->SetRightFolderPath(m_pWorkingFolderSets->GetRightFolderPath(activeSet));
		m_pWorkingFolderSet->SetEnabled(true);
	}
}

void OpenDatabaseDialog::UpdateFromWorkingSet(int index)
{
	m_pWorkingFolderSets->SetName(index,m_pWorkingFolderSet->GetName());
	m_pWorkingFolderSets->SetDescription(index,m_pWorkingFolderSet->GetDescription());
	m_pWorkingFolderSets->SetLeftFolderPath(index,m_pWorkingFolderSet->GetLeftFolderPath());
	m_pWorkingFolderSets->SetRightFolderPath(index,m_pWorkingFolderSet->GetRightFolderPath());
	m_pWorkingFolderSets->SetEnabled(index,true);
	// update previous paths
	m_pWorkingFolderSets->AddLeftPreviousPath(m_pWorkingFolderSet->GetLeftFolderPath(),MAX_MRU_IN_COMBO);
	m_pWorkingFolderSets->AddRightPreviousPath(m_pWorkingFolderSet->GetRightFolderPath(),MAX_MRU_IN_COMBO);
}

void OpenDatabaseDialog::SetupButtons(void)
{
	int numEnabledSets = GetNumEnabledSets();

	switch (m_mode)
	{
		case PICK_MODE:
			m_okButton.EnableWindow(TRUE);
			m_newDatabaseButton.EnableWindow(TRUE);
			m_editDatabaseButton.EnableWindow(TRUE);
			m_deleteDatabaseButton.EnableWindow(numEnabledSets>1 ? TRUE : FALSE);
			m_cancelButton.EnableWindow(TRUE);
			m_acceptDatabaseButton.EnableWindow(FALSE);
			break;
		case EDIT_MODE:
		case NEW_MODE:
			m_okButton.EnableWindow(FALSE);
			m_newDatabaseButton.EnableWindow(FALSE);
			m_editDatabaseButton.EnableWindow(FALSE);
			m_deleteDatabaseButton.EnableWindow(FALSE);
			m_cancelButton.EnableWindow(TRUE); //m_mode==EDIT_MODE || numEnabledSets>0 ) ? TRUE : FALSE);
			m_acceptDatabaseButton.EnableWindow(TRUE);
			break;
	}
}

void OpenDatabaseDialog::SetupWorkingSetControls(void)
{
	m_databaseName.SetWindowText(m_pWorkingFolderSet->GetName());
	m_databaseDescription.SetWindowText(m_pWorkingFolderSet->GetDescription());
	// set up left combo
	m_leftFolderCombo.ResetContent();
	int numLeftFolders = m_pWorkingFolderSets->GetNumLeftPreviousPaths();
	for (int leftFolderIndex=0;leftFolderIndex<numLeftFolders;leftFolderIndex++)
	{
		m_leftFolderCombo.AddString(m_pWorkingFolderSets->GetLeftPreviousPath(leftFolderIndex));
	}
	m_leftFolderCombo.SetWindowText(m_pWorkingFolderSet->GetLeftFolderPath());
	// set up right combo
	m_rightFolderCombo.ResetContent();
	int numRightFolders = m_pWorkingFolderSets->GetNumRightPreviousPaths();
	for (int rightFolderIndex=0;rightFolderIndex<numRightFolders;rightFolderIndex++)
	{
		m_rightFolderCombo.AddString(m_pWorkingFolderSets->GetRightPreviousPath(rightFolderIndex));
	}
	m_rightFolderCombo.SetWindowText(m_pWorkingFolderSet->GetRightFolderPath());
	
	switch (m_mode)
	{
		case PICK_MODE:
			m_leftBrowseButton.EnableWindow(FALSE);
			m_rightBrowseButton.EnableWindow(FALSE);
			m_leftFolderCombo.EnableWindow(FALSE);
			m_rightFolderCombo.EnableWindow(FALSE);
			m_databaseDescription.SetReadOnly(TRUE);
			m_databaseName.SetReadOnly(TRUE);
			break;
		case EDIT_MODE:
		case NEW_MODE:
			m_leftBrowseButton.EnableWindow(TRUE);
			m_rightBrowseButton.EnableWindow(TRUE);
			m_leftFolderCombo.EnableWindow(TRUE);
			m_rightFolderCombo.EnableWindow(TRUE);
			m_databaseDescription.SetReadOnly(FALSE);
			m_databaseName.SetReadOnly(FALSE);
			break;
	}
}

void OpenDatabaseDialog::SetTempRootDir(CString strNewRoot)
{
	m_strTempRootDir = strNewRoot;
}


CString OpenDatabaseDialog::GetTempRootDir(void)
{
	return m_strTempRootDir;
}

void OpenDatabaseDialog::UpdateWorkingSet(void)
{
	CString tempString;
	m_databaseName.GetWindowText(tempString);
	m_pWorkingFolderSet->SetName(tempString);

	m_databaseDescription.GetWindowText(tempString);
	m_pWorkingFolderSet->SetDescription(tempString);

	m_leftFolderCombo.GetWindowText(tempString);
	m_pWorkingFolderSet->SetLeftFolderPath(tempString);

	m_rightFolderCombo.GetWindowText(tempString);
	m_pWorkingFolderSet->SetRightFolderPath(tempString);
}

bool OpenDatabaseDialog::VerifyWorkingSet(void)
{
	UpdateWorkingSet();

	const char *pName = m_pWorkingFolderSet->GetName();
	bool bProblem = false;
	CString strProblem;
	CString strTemp;
	if (strlen(pName)==0)
	{
		bProblem = true;
		strTemp.LoadString(IDS_NAME_CANNOT_BE_EMPTY);
		strProblem = strProblem + strTemp + "\n\n";
	}
	if (m_mode == NEW_MODE)
	{
		int matchSet = m_pWorkingFolderSets->FindSetByName(pName);
		// if one of the sets already has that name
		if (matchSet!=-1)
		{
			bProblem = true;
			strTemp.LoadString(IDS_NAME_NEEDS_TO_BE_UNIQUE);
			strProblem = strProblem + "\n" + strTemp + "\n\n";
		}
	}
	CString leftFolderPath = m_pWorkingFolderSet->GetLeftFolderPath();
	if (::PathFileExists((LPCTSTR) leftFolderPath) == FALSE)
	{
		bProblem = true;
		strTemp.LoadString(IDS_LEFT_FOLDER_DOESNT_EXIST);
		strProblem = strProblem + strTemp + "\n" + leftFolderPath + "\n\n";
	}
	CString rightFolderPath = m_pWorkingFolderSet->GetRightFolderPath();
	if (::PathFileExists((LPCTSTR) rightFolderPath) == FALSE)
	{
		bProblem = true;
		strTemp.LoadString(IDS_RIGHT_FOLDER_DOESNT_EXIST);
		strProblem = strProblem + strTemp + "\n" + rightFolderPath + "\n\n";
	}
	if (bProblem)
	{
		::AfxMessageBox((LPCTSTR) strProblem, MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}
