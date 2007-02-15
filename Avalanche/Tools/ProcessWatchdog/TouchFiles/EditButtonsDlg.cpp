// EditButtonsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchFiles.h"
#include "EditButtonsDlg.h"
#include ".\editbuttonsdlg.h"

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

// EditButtonsDlg dialog

IMPLEMENT_DYNAMIC(EditButtonsDlg, CDialog)
EditButtonsDlg::EditButtonsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EditButtonsDlg::IDD, pParent)
	, m_name1(_T(""))
	, m_name2(_T(""))
	, m_name3(_T(""))
	, m_name4(_T(""))
	, m_path1(_T(""))
	, m_path2(_T(""))
	, m_path3(_T(""))
	, m_path4(_T(""))
{
}

EditButtonsDlg::~EditButtonsDlg()
{
}

void EditButtonsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME1, m_name1);
	DDX_Text(pDX, IDC_EDIT_NAME2, m_name2);
	DDX_Text(pDX, IDC_EDIT_NAME3, m_name3);
	DDX_Text(pDX, IDC_EDIT_NAME4, m_name4);
	DDX_Text(pDX, IDC_EDIT_PATH1, m_path1);
	DDX_Text(pDX, IDC_EDIT_PATH2, m_path2);
	DDX_Text(pDX, IDC_EDIT_PATH3, m_path3);
	DDX_Text(pDX, IDC_EDIT_PATH4, m_path4);
}


BEGIN_MESSAGE_MAP(EditButtonsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnBnClickedButtonBrowse1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnBnClickedButtonBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnBnClickedButtonBrowse3)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE4, OnBnClickedButtonBrowse4)
END_MESSAGE_MAP()


// EditButtonsDlg message handlers

void EditButtonsDlg::OnBnClickedButtonBrowse1()
{
	static char szFilter[] = "All Files (*.*)|*.*||";

	CString fileString;
	if (BrowseFile(this,"Specify Target File",szFilter,m_path1,TRUE,fileString))
	{
		m_path1 = fileString;
		UpdateData(FALSE);
	}
}

void EditButtonsDlg::OnBnClickedButtonBrowse2()
{
	static char szFilter[] = "All Files (*.*)|*.*||";

	CString fileString;
	if (BrowseFile(this,"Specify Target File",szFilter,m_path2,TRUE,fileString))
	{
		m_path2 = fileString;
		UpdateData(FALSE);
	}
}

void EditButtonsDlg::OnBnClickedButtonBrowse3()
{
	static char szFilter[] = "All Files (*.*)|*.*||";

	CString fileString;
	if (BrowseFile(this,"Specify Target File",szFilter,m_path3,TRUE,fileString))
	{
		m_path3 = fileString;
		UpdateData(FALSE);
	}
}

void EditButtonsDlg::OnBnClickedButtonBrowse4()
{
	static char szFilter[] = "All Files (*.*)|*.*||";

	CString fileString;
	if (BrowseFile(this,"Specify Target File",szFilter,m_path4,TRUE,fileString))
	{
		m_path4 = fileString;
		UpdateData(FALSE);
	}
}
