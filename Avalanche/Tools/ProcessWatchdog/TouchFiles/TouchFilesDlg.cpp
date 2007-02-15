// TouchFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchFiles.h"
#include "TouchFilesDlg.h"
#include ".\touchfilesdlg.h"
#include "Registry.h"
#include "RegistryKeys.h"
#include "EditButtonsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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


// CTouchFilesDlg dialog



CTouchFilesDlg::CTouchFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTouchFilesDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTouchFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTouchFilesDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_COMMAND(ID_EDIT_EDITBUTTONS, OnEditEditbuttons)
END_MESSAGE_MAP()


// CTouchFilesDlg message handlers

BOOL CTouchFilesDlg::OnInitDialog()
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

	m_bClosing = false;
	LoadFromRegistry();
	UpdateButtons();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTouchFilesDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTouchFilesDlg::OnPaint() 
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
HCURSOR CTouchFilesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTouchFilesDlg::OnBnClickedButton1()
{
	if (m_touchButtons[0].m_path.GetLength())
	{
		if (!TouchFile(m_touchButtons[0].m_path))
		{
			CString strErrorString;
			strErrorString.Format("Error touching file %s",m_touchButtons[0].m_path);
			::AfxMessageBox(strErrorString, MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void CTouchFilesDlg::OnBnClickedButton2()
{
	if (m_touchButtons[1].m_path.GetLength())
	{
		if (!TouchFile(m_touchButtons[1].m_path))
		{
			CString strErrorString;
			strErrorString.Format("Error touching file %s",m_touchButtons[1].m_path);
			::AfxMessageBox(strErrorString, MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void CTouchFilesDlg::OnBnClickedButton3()
{
	if (m_touchButtons[2].m_path.GetLength())
	{
		if (!TouchFile(m_touchButtons[2].m_path))
		{
			CString strErrorString;
			strErrorString.Format("Error touching file %s",m_touchButtons[2].m_path);
			::AfxMessageBox(strErrorString, MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void CTouchFilesDlg::OnBnClickedButton4()
{
	if (m_touchButtons[3].m_path.GetLength())
	{
		if (!TouchFile(m_touchButtons[3].m_path))
		{
			CString strErrorString;
			strErrorString.Format("Error touching file %s",m_touchButtons[3].m_path);
			::AfxMessageBox(strErrorString, MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void CTouchFilesDlg::LoadFromRegistry(void)
{
	char numString[10];

#define STRING_SIZE 512
	char tempString[STRING_SIZE];

	Registry registry(TOUCH_FILES_KEY);

	for (int index=0;index<4;index++)
	{
		itoa(index,numString,10);

		CString buttonNameField(BUTTON_NAME_FIELD);
		buttonNameField+= numString; // append the set number

		if (registry.Get(buttonNameField,&tempString,STRING_SIZE))
		{
			m_touchButtons[index].m_name = tempString;
		}
		else
		{
			m_touchButtons[index].m_name = "";
		}

		CString buttonPathField(BUTTON_PATH_FIELD);
		buttonPathField+= numString; // append the set number

		if (registry.Get(buttonPathField,&tempString,STRING_SIZE))
		{
			m_touchButtons[index].m_path = tempString;
		}
		else
		{
			m_touchButtons[index].m_path = "";
		}
	}
}

void CTouchFilesDlg::SaveToRegistry(void)
{
	char numString[10];

	Registry registry(TOUCH_FILES_KEY);

	for (int index=0;index<4;index++)
	{
		itoa(index,numString,10);

		CString buttonNameField(BUTTON_NAME_FIELD);
		buttonNameField+= numString; // append the set number

		registry.Set(buttonNameField,m_touchButtons[index].m_name);

		CString buttonPathField(BUTTON_PATH_FIELD);
		buttonPathField+= numString; // append the set number

		registry.Set(buttonPathField,m_touchButtons[index].m_path);
	}
}

bool CTouchFilesDlg::TouchFile(CString pathToFile)
{
	FILETIME currentFileTime;
	SYSTEMTIME currentSystemTime;

	::GetSystemTime(&currentSystemTime);		// gets current time
	::SystemTimeToFileTime(&currentSystemTime, &currentFileTime);	// converts to file time format

	HANDLE hFile = ::CreateFile(pathToFile,  // open path
		GENERIC_WRITE,						// write access
		FILE_SHARE_READ,					// share for reading 
		NULL,									// no security 
		OPEN_EXISTING,						// existing file only 
		FILE_ATTRIBUTE_NORMAL,			// normal file 
		NULL);								// no attr. template 

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return false; // if we cannot open the file we could not set the time
	}

	bool bResult = false;
	if (::SetFileTime(hFile,(LPFILETIME) NULL, (LPFILETIME) NULL, &currentFileTime))
	{
		bResult = true;
	}
	CloseHandle(hFile);
	return bResult;
}

void CTouchFilesDlg::OnEditEditbuttons()
{
	EditButtonsDlg editButtonsDlg;
	editButtonsDlg.m_name1 = m_touchButtons[0].m_name;
	editButtonsDlg.m_path1 = m_touchButtons[0].m_path;
	editButtonsDlg.m_name2 = m_touchButtons[1].m_name;
	editButtonsDlg.m_path2 = m_touchButtons[1].m_path;
	editButtonsDlg.m_name3 = m_touchButtons[2].m_name;
	editButtonsDlg.m_path3 = m_touchButtons[2].m_path;
	editButtonsDlg.m_name4 = m_touchButtons[3].m_name;
	editButtonsDlg.m_path4 = m_touchButtons[3].m_path;

	if (editButtonsDlg.DoModal() == IDOK)
	{
		m_touchButtons[0].m_name = editButtonsDlg.m_name1;
		m_touchButtons[0].m_path = editButtonsDlg.m_path1;
		m_touchButtons[1].m_name = editButtonsDlg.m_name2;
		m_touchButtons[1].m_path = editButtonsDlg.m_path2;
		m_touchButtons[2].m_name = editButtonsDlg.m_name3;
		m_touchButtons[2].m_path = editButtonsDlg.m_path3;
		m_touchButtons[3].m_name = editButtonsDlg.m_name4;
		m_touchButtons[3].m_path = editButtonsDlg.m_path4;

		UpdateButtons();
	}
}

void CTouchFilesDlg::UpdateButtons(void)
{
	int buttonRes[] = { IDC_BUTTON1,IDC_BUTTON2,IDC_BUTTON3,IDC_BUTTON4 };

	CWnd *pCWnd;
	CString name;

	for (int index=0;index<4;index++)
	{
		pCWnd = GetDlgItem(buttonRes[index]);
		if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
		{
			if (m_touchButtons[index].m_name.GetLength())
			{
				pCWnd->SetWindowText(m_touchButtons[index].m_name);
				pCWnd->EnableWindow(TRUE);
			}
			else
			{
				pCWnd->SetWindowText("Empty");
				pCWnd->EnableWindow(FALSE);
			}
		}
	}
}

void CTouchFilesDlg::OnOK()
{
	// We just want to absorb this message, thus keeping the app from quitting
	// when the user hits the enter key:
	SetFocus();
}

void CTouchFilesDlg::OnCancel()
{
	if (m_bClosing)
	{
		CDialog::OnCancel();
	}
}

void CTouchFilesDlg::OnClose()
{
	m_bClosing = true;
	CDialog::OnClose();

	SaveToRegistry();
}

