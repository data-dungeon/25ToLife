// DiffPatchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DiffPatch.h"
#include "DiffPatchDlg.h"
#include <ShlObj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static int _validateUInt(char* str, UINT& value);

using namespace std;

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDiffPatchDlg dialog



CDiffPatchDlg::CDiffPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiffPatchDlg::IDD, pParent)
	, m_statusBox(_T(""))
	, m_originalDir(_T(""))
	, m_modifiedDir(_T(""))
	, m_patchPath(_T(""))
	, patchCompiler(0)
	, m_finalPatchSize(_T(""))
	, m_versionString(_T("2"))
	, m_versionNumber(2)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDiffPatchDlg::~CDiffPatchDlg()
{
	if ( patchCompiler )
	{
		delete patchCompiler;
		patchCompiler = 0;
	}
}

void CDiffPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progressBar);
	DDX_Text(pDX, IDC_STATUSBOX, m_statusBox);
	DDX_Text(pDX, ID_ORIGINALFILE, m_originalDir);
	DDX_Text(pDX, ID_NEWVERSION, m_modifiedDir);
	DDX_Control(pDX, IDC_LISTBOX, m_listCtrl);
	DDX_Text(pDX, ID_PATCHPATH, m_patchPath);
	DDX_Text(pDX, IDC_PATCHSIZE, m_finalPatchSize);
	DDX_Text(pDX, IDC_VERSION_EDIT, m_versionString);
	if (!pDX->m_bSaveAndValidate)
	{
		int pos = m_statusBox.GetLength();
		((CEdit*)GetDlgItem(IDC_STATUSBOX))->SetSel(pos, pos, FALSE);
	}
}

BEGIN_MESSAGE_MAP(CDiffPatchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE1, OnBnClickedBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBnClickedBrowse2)
	ON_BN_CLICKED(IDC_GENERATEPATCH, OnBnClickedGeneratepatch)
	ON_BN_CLICKED(IDC_BROWSE3, OnBnClickedBrowse3)
	ON_BN_CLICKED(IDC_DIFFBUTTON, OnBnClickedDiffbutton)
	ON_BN_CLICKED(IDC_SELALL, OnBnClickedSelall)
	ON_BN_CLICKED(IDC_SELNONE, OnBnClickedSelnone)
	ON_EN_UPDATE(IDC_VERSION_EDIT, OnEnUpdateVersionEdit)
END_MESSAGE_MAP()


// CDiffPatchDlg message handlers

BOOL CDiffPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ReadDefaults();

	CRect rect;	
	m_listCtrl.GetClientRect(&rect);
	int nColInterval = rect.Width();
	m_listCtrl.InsertColumn(0, "File", LVCFMT_LEFT, nColInterval/2+50);
	m_listCtrl.InsertColumn(1, "Mod Time", LVCFMT_LEFT, nColInterval/2-50);

	DWORD newStyle = m_listCtrl.GetExtendedStyle();
	newStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES;
	m_listCtrl.SetExtendedStyle(newStyle);

	GetDlgItem(IDC_GENERATEPATCH)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDiffPatchDlg::ReadDefaults()
{
	char buf[255];
	FILE *defFile = 0;
	defFile = fopen("defaults.txt","r");
	if ( !defFile )
		return;

	// Quick and dirty read of the defults.txt file
	// This is really a hack so we don't have to browse
	// for the  files every time during testing.
	fgets(buf, 255, defFile);
	m_originalDir = buf;
	m_originalDir.TrimRight("\n\r");
	fgets(buf, 255, defFile);
	m_modifiedDir = buf;
	m_modifiedDir.TrimRight("\n\r");
	fgets(buf, 255, defFile);
	m_patchPath = buf;
	m_patchPath.TrimRight("\n\r");
	fclose(defFile);

	UpdateData(FALSE);
}

void CDiffPatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDiffPatchDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDiffPatchDlg::OnQueryDragIcon(){
	return static_cast<HCURSOR>(m_hIcon);
}

bool CDiffPatchDlg::BrowseForFolder(CString& outputStr)
{
	// Set up and call SHBrowseForFolder().
	// return TRUE if user clicks OK, return FALSE otherwise
	bool bReturnValue = FALSE;

	BROWSEINFO browseInfo;

	// set up the browseinfo structure used by SHBrowseForFolder()
	::ZeroMemory(&browseInfo, sizeof(BROWSEINFO));
	browseInfo.hwndOwner = GetSafeHwnd();
	browseInfo.lParam = (LPARAM)this;
	browseInfo.lpfn = NULL;
	browseInfo.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;

	CString fdpath = "";
	LPOLESTR ofdpath = fdpath.AllocSysString();
    LPMALLOC pMalloc = NULL;
	::SHGetMalloc(&pMalloc);
	LPITEMIDLIST pidFolder = NULL;
	IShellFolder *sfRoot   = NULL;
	::SHGetDesktopFolder(&sfRoot);
	sfRoot->ParseDisplayName(NULL, NULL, ofdpath, NULL, &pidFolder, NULL);
	browseInfo.pidlRoot = pidFolder;

	CoInitialize(NULL);
	ITEMIDLIST *idl = SHBrowseForFolder(&browseInfo);
	if (idl)
	{
		TCHAR lpstrBuffer[_MAX_PATH] = _T("");
		if (SHGetPathFromIDList(idl, lpstrBuffer))	// get path string from ITEMIDLIST
		{
			CString tmp = lpstrBuffer;
			outputStr = tmp;
			bReturnValue = TRUE;
		}
		pMalloc->Free(idl);
	}

	SetFocus();									// ensure focus returns to this control
	pMalloc->Free(pidFolder);
	pMalloc->Release();
	return bReturnValue;
}

void CDiffPatchDlg::GenericFileOpen(CString &path)
{
	static char szFilter[] = "All Files (*.*)|*.*|All Files (*.*)|*.*||";
	CFileDialog *dlg = new CFileDialog(TRUE, "", NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szFilter, NULL);
	if ( dlg->DoModal() == IDOK ) 
	{
		path = dlg->GetPathName();	
	}

	// Clean up the file dialog
	delete dlg;
	dlg = NULL;

	UpdateData(FALSE);
}

void CDiffPatchDlg::OnBnClickedBrowse1()
{
	UpdateData(TRUE);
	BrowseForFolder(m_originalDir);
	UpdateData(FALSE);
}

void CDiffPatchDlg::OnBnClickedBrowse2()
{
	UpdateData(TRUE);
	BrowseForFolder(m_modifiedDir);
	UpdateData(FALSE);
}

void CDiffPatchDlg::OnBnClickedBrowse3()
{
	UpdateData(TRUE);

	TCHAR szFilters[] =
		_T ("Patch files (*.patch)|*.patch|All files (*.*)|*.*||");

	//CFileDialog dlg (TRUE, _T ("txt"), _T ("*.txt"),
	//	OFN_FILEMUSTEXIST ¦ OFN_HIDEREADONLY, szFilters);
	CFileDialog dlg (FALSE, _T ("patch"), NULL, 0, szFilters);

	if (dlg.DoModal () == IDOK) {
		m_patchPath = dlg.GetPathName ();
	}
	UpdateData(FALSE);
}

void CDiffPatchDlg::OnBnClickedGeneratepatch()
{
	CWaitCursor wait;

	// Make sure we have the latest values.
	UpdateData(TRUE);

	if ( !patchCompiler )
	{
		GetDlgItem(IDC_GENERATEPATCH)->EnableWindow(FALSE);
		return;
	}

	list<string> diffList;
	CString file;
	int i, count = m_listCtrl.GetItemCount();
	for ( i = 0; i<count; i++ )
	{
		if ( m_listCtrl.GetCheck(i) )
		{
			string file = (LPCSTR)m_listCtrl.GetItemText(i, 0);
			diffList.push_back(file);
		}
	}

	// Construct a complete patch for the output
	CString patchName = m_patchPath;
	
	PutStatus("\r\nCompiling Patch...\r\n");
	if (!patchCompiler->CompilePatch(diffList, (LPCSTR)patchName, m_versionNumber))
	{
		PutStatus("ERROR! Check output filename and sufficient disk space!\r\n");
	}
	else
	{
		PutStatus("Done creating patch file!\r\n");
		m_finalPatchSize.Format("%d bytes", patchCompiler->GetFileSize((LPCSTR)patchName));
	}

	UpdateData(FALSE);
}

void CDiffPatchDlg::OnBnClickedDiffbutton()
{
	CWaitCursor wait;

	UpdateData( TRUE );

	if ( !m_originalDir.GetLength() || !m_modifiedDir.GetLength() )
		return;

	if ( patchCompiler )
		delete patchCompiler;

	list<string> fileList1;
	list<string> fileList2;
	list<string> diffList;
	patchCompiler = new RPatchCompiler((LPCSTR)m_originalDir, (LPCSTR)m_modifiedDir);

	PutStatus("Generating Original File List...\r\n");
	if (!patchCompiler->GenerateFileList(fileList1, (LPCSTR)m_originalDir))
	{
		PutStatus("ERROR! Perhaps the directory name is bad; aborting!\r\n");
		return;
	}

	PutStatus("Generating Modified File List...\r\n");
	if (!patchCompiler->GenerateFileList(fileList2, (LPCSTR)m_modifiedDir))
	{
		PutStatus("ERROR! Perhaps the directory name is bad; aborting!\r\n");
		return;
	}

	PutStatus("Diffing the Directories...\r\n");
	patchCompiler->GetDifferenceList(fileList1, fileList2, diffList);
	PutStatus("Done diffing directories!\r\n");

	int count = 0;
	m_listCtrl.DeleteAllItems();
	diffList.sort();
	list<string>::iterator iter;
	for ( iter = diffList.begin(); iter != diffList.end(); iter++ )
	{
		CString tmpname;
		string file = (*iter);
				// Insert the item
		LVITEM lvi;
		lvi.mask =  LVIF_TEXT;
		lvi.iItem = count;
		lvi.iSubItem = 0;
		tmpname = file.c_str();
		lvi.pszText = (LPTSTR)(LPCTSTR)tmpname;
		lvi.iImage = 0;
		m_listCtrl.InsertItem(&lvi);
		m_listCtrl.SetCheck(count);

		// Put the file modification date in the second column
		CFileStatus fileStatus;
		CFile::GetStatus(file.c_str(), fileStatus);
		CString modTime = fileStatus.m_mtime.Format("%m/%d/%y %I:%M %p");
		lvi.iSubItem = 1;
		lvi.pszText = (LPTSTR)(LPCTSTR)modTime;
		m_listCtrl.SetItem(&lvi);

		count++;
	}

	if ( m_listCtrl.GetItemCount() > 0 )
		GetDlgItem(IDC_GENERATEPATCH)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_GENERATEPATCH)->EnableWindow(FALSE);
}

void CDiffPatchDlg::OnBnClickedSelall()
{
	int count = m_listCtrl.GetItemCount();
	for ( int i = 0; i<count; i++ )
		m_listCtrl.SetCheck(i);
}

void CDiffPatchDlg::OnBnClickedSelnone()
{
	int count = m_listCtrl.GetItemCount();
	for ( int i = 0; i<count; i++ )
		m_listCtrl.SetCheck(i, false);
}

//------------------------------------------------------------------------------
// DFS -- Custom number filter, because the one MFC provides sucks...
void CDiffPatchDlg::OnEnUpdateVersionEdit()
{
	UpdateData(TRUE);

	char s[256];
	strncpy(s, (LPCTSTR)m_versionString, 255);
	s[255] = 0;

	int pos = _validateUInt(s, m_versionNumber);
	if (pos >= 0) {
		// Keep checking until we have a valid string
		while (_validateUInt(s, m_versionNumber) >= 0)
			;	// This line intentionally left blank

		// An illegal character was typed, give a beep to let the user know
		::MessageBeep(MB_ICONASTERISK);
		m_versionString = s;
	}

	// Put the data back into the dialog before changing the control
	UpdateData(FALSE);

	// Set the insertion caret to the right place
	((CEdit*)GetDlgItem(IDC_VERSION_EDIT))->SetSel(pos, pos);
}

//------------------------------------------------------------------------------
typedef long long U64;
typedef unsigned int U32;

int _validateUInt(char* text, UINT& value)
{
	int len = (int)strlen(text);
	U32 base = 10;
	U64 val = 0;
	U64 tmp = 0;
	U32 trunc;
	U32 digit;

	for (int pos = 0; pos < len; pos++) {
		U32 c = (int)text[pos];

		// Always allow a digit
		if (isdigit(c)) {
			digit = c - '0';
			tmp = (val * base) + digit;

			// Check for overflow
			trunc = (U32)tmp;
			if (trunc != tmp)
				break;

			val = tmp;
			continue;
		}

		// Keep things simpler
		c = toupper(c);

		// Allow an X if and only if it is the second character preceded by a zero
		if (c == 'X') {
			if ((pos == 1) && (text[0] == '0')) {
				base = 16;
				continue;
			}
		}

		// If we are hex, allow a-f
		if (base == 16) {
			if ((c >= 'A') && (c <= 'F')) {
				digit = c - 'A' + 10;
				tmp = (val * base) + digit;

				// Check for overflow
				trunc = (U32)tmp;
				if (trunc != tmp)
					break;

				val = tmp;
				continue;
			}
		}

		// If we got here, the character is illegal!
		break;
	}

	// Store the answer
	value = (UINT)val;

	// If a character was bad, zap it and return
	if (pos < len) {
		for (int i = pos; i < len; i++)
			text[i] = text[i+1];
		return pos;
	}

	// The whole string was good
	return -1;
}

void CDiffPatchDlg::PutStatus(const CString& str)
{
	m_statusBox += str;
	UpdateData(FALSE);
}
