// FileServerPane.cpp : implementation file
//

#include "stdafx.h"
#include "gameclient.h"
#include "FileServerPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileServerPane dialog

IMPLEMENT_DYNAMIC(CFileServerPane, CGameClientPane);


CFileServerPane::CFileServerPane(CWnd* pParent /*=NULL*/)
	: CGameClientPane(CFileServerPane::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileServerPane)
	m_HideErrors = TRUE;
	//}}AFX_DATA_INIT

	m_NextHistory = 0;
}


void CFileServerPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileServerPane)
	DDX_Control(pDX, IDC_FILELOG, m_FileLog);
	DDX_Check(pDX, IDC_HIDEERRORS, m_HideErrors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileServerPane, CDialog)
	//{{AFX_MSG_MAP(CFileServerPane)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CURRENTLYOPENFILES, OnCurrentlyopenfiles)
	ON_BN_CLICKED(IDC_FILEACCESSHISTORY, OnFileaccesshistory)
	ON_BN_CLICKED(IDC_CLEARHISTORY, OnClearhistory)
	ON_BN_CLICKED(IDC_HIDEERRORS, OnHideerrors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileServerPane message handlers

void CFileServerPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(!::IsWindow(m_FileLog))
		return;

	CRect client;
	CRect button;
	GetClientRect(&client);
	GetDlgItem(IDC_CLEARHISTORY)->GetClientRect(&button);

	CRect rect = client;
	rect.DeflateRect(0,0,0,button.Height() + 2);
	m_FileLog.MoveWindow(&rect);

#define BOTTOMCTLS 4
	int ctlsize = (client.right - client.left) / BOTTOMCTLS - (BOTTOMCTLS-1)*2;

	rect.top = client.bottom - button.Height();
	rect.bottom = client.bottom;
	rect.left = client.left;
	rect.right = rect.left + ctlsize;
	GetDlgItem(IDC_CLEARHISTORY)->MoveWindow(&rect);

	rect.left = rect.right + 2;
	rect.right = rect.left + ctlsize;
	GetDlgItem(IDC_FILEACCESSHISTORY)->MoveWindow(&rect);

	rect.left = rect.right + 2;
	rect.right = rect.left + ctlsize;
	GetDlgItem(IDC_CURRENTLYOPENFILES)->MoveWindow(&rect);

	rect.left = rect.right + 2;
	rect.right = rect.left + ctlsize;
	GetDlgItem(IDC_HIDEERRORS)->MoveWindow(&rect);
}

BOOL CFileServerPane::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_FileLog.InsertColumn(0, "File Name", LVCFMT_LEFT, 190);
	m_FileLog.InsertColumn(1, "Mode", LVCFMT_LEFT, 40);
	m_FileLog.InsertColumn(2, "Bytes Read", LVCFMT_LEFT, 90);
	m_FileLog.InsertColumn(3, "Written", LVCFMT_LEFT, 70);

	CheckRadioButton(IDC_CURRENTLYOPENFILES, IDC_FILEACCESSHISTORY, IDC_FILEACCESSHISTORY);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileServerPane::AddOpenConnection(int id, CString filename, int mode)
{
	for(int i = 0; i < MAX_OPENFILES; i++)
	{
		if(m_Open[i].fileName.IsEmpty())
		{
			m_Open[i].fileName = filename;
			m_Open[i].bytesRead = 0;
			m_Open[i].bytesWritten = 0;
			m_Open[i].id = id;
			m_Open[i].mode = mode;

			if(!IsViewingHistory())
				InsertItem(m_Open[i]);

			return;
		}
	}
	AfxMessageBox("Too many open files to track, open file list will be wrong", MB_OK|MB_ICONERROR);
}

void CFileServerPane::RemoveOpenConnection(int id)
{
	int handle;

	for(handle = 0; handle < MAX_OPENFILES; handle++)
	{
		if(m_Open[handle].id == id)
			break;
	}
	if(handle == MAX_OPENFILES) return;

	m_History[m_NextHistory++] = m_Open[handle];

	if(m_NextHistory == MAX_HISTORIES)
	{
		m_NextHistory = 0;
	}

	if(IsViewingHistory())
		InsertItem(m_Open[handle]);
	else
		DeleteItem(m_Open[handle]);

	m_Open[handle].fileName.Empty();
}


void CFileServerPane::UpdateOpenConnection(int id, int read, int write)
{
	int handle;

	for(handle = 0; handle < MAX_OPENFILES; handle++)
	{
		if(m_Open[handle].id == id)
			break;
	}

	m_Open[handle].bytesRead += read;
	m_Open[handle].bytesWritten += write;

	if(!IsViewingHistory())
		UpdateItem(m_Open[handle]);
}


void CFileServerPane::OnCurrentlyopenfiles() 
{
	m_FileLog.DeleteAllItems();
	for(int i = 0; i < MAX_OPENFILES; i++)
	{
		if(!m_Open[i].fileName.IsEmpty())
			InsertItem(m_Open[i]);
	}
	GetDlgItem(IDC_HIDEERRORS)->EnableWindow(FALSE);
}

void CFileServerPane::OnFileaccesshistory() 
{
	UpdateData(TRUE);
	m_FileLog.DeleteAllItems();
	for(int i = m_NextHistory; ; i++)
	{
		if(i == MAX_HISTORIES) i = 0;

		if(!m_History[i].fileName.IsEmpty())
		{
			if(!m_HideErrors || m_History[i].id != -1 )
				InsertItem(m_History[i]);
		}

		if(i + 1 == m_NextHistory) break;
		if(m_NextHistory == 0 && i == MAX_HISTORIES-1) break;
	}
	GetDlgItem(IDC_HIDEERRORS)->EnableWindow(TRUE);
}

int CFileServerPane::FindItem(int id)
{
	LVFINDINFO find;
	find.flags = LVFI_PARAM;
	find.lParam = id;
	return m_FileLog.FindItem(&find);
}

void CFileServerPane::DeleteItem(const Entry &entry)
{
	int item = FindItem(entry.id);
	if(item == -1) return;
	m_FileLog.DeleteItem(item);
}

void CFileServerPane::InsertItem(const Entry &entry)
{
	int item = m_FileLog.InsertItem(m_FileLog.GetItemCount(), entry.fileName);
	m_FileLog.SetItem(item, 0, LVIF_PARAM, entry.fileName, 0, 0, 0, entry.id);
	UpdateItem(item, entry);

	CString mode;
	switch(entry.mode)
	{
	case 1:
		mode = "Read";
		break;
	case 2:
		mode = "Write";
		break;
	case 3:
		mode = "Unknown";
	}
	m_FileLog.SetItem(item, 1, LVIF_TEXT, mode, 0, 0, 0, entry.id);

	int count = m_FileLog.GetCountPerPage();
	if(count + m_FileLog.GetTopIndex() + 1 == item)
		m_FileLog.EnsureVisible(item, FALSE);
}

void CFileServerPane::UpdateItem(const Entry &entry)
{
	int item = FindItem(entry.id);
	if(item == -1) return;
	UpdateItem(item, entry);
}

void CFileServerPane::UpdateItem(int index, const Entry &entry)
{
	CString temp;

	if(entry.id == -1)
	{
		switch(entry.bytesRead)
		{
		case -1:
			temp = "Open Error";
			break;
		default:
			temp = "Unknown Error";
			break;
		}
		m_FileLog.SetItem(index, 2, LVIF_TEXT, temp, 0, 0, 0, entry.id);

		switch(entry.bytesWritten)
		{
		case ERROR_FILE_NOT_FOUND:
			temp = "Not Found";
			break;
		case ERROR_PATH_NOT_FOUND:
			temp = "Bad Path";
			break;
		case ERROR_ACCESS_DENIED:
			temp = "Access Denied";
			break;
		default:
			temp = "Unknown Error";
			break;
		}
		m_FileLog.SetItem(index, 3, LVIF_TEXT, temp, 0, 0, 0, entry.id);
	}
	else
	{
		temp.Format("%d", entry.bytesRead);
		m_FileLog.SetItem(index, 2, LVIF_TEXT, temp, 0, 0, 0, entry.id);

		temp.Format("%d", entry.bytesWritten);
		m_FileLog.SetItem(index, 3, LVIF_TEXT, temp, 0, 0, 0, entry.id);
	}
}

BOOL CFileServerPane::IsViewingHistory()
{
	return GetCheckedRadioButton(IDC_CURRENTLYOPENFILES, IDC_FILEACCESSHISTORY) == IDC_FILEACCESSHISTORY;
}

void CFileServerPane::OnClearhistory() 
{
	if(IsViewingHistory())
	{
		m_FileLog.DeleteAllItems();
	}
	for(int i = 0; i < MAX_HISTORIES; i++)
	{
		m_History[i].fileName.Empty();
	}
	m_NextHistory = 0;
}

void CFileServerPane::AddDefunctConnection(CString filename, int mode, int defuncCode, int errorCode)
{
	m_History[m_NextHistory].bytesRead = defuncCode;
	m_History[m_NextHistory].bytesWritten = errorCode;
	m_History[m_NextHistory].fileName = filename;
	m_History[m_NextHistory].id = -1;
	m_History[m_NextHistory].mode = mode;

	UpdateData(TRUE);
	if(IsViewingHistory() && !m_HideErrors)
		InsertItem(m_History[m_NextHistory]);

	m_NextHistory++;
	if(m_NextHistory == MAX_HISTORIES)
	{
		m_NextHistory = 0;
	}
}

void CFileServerPane::OnHideerrors() 
{
	UpdateData(TRUE);
	if(IsViewingHistory())
		OnFileaccesshistory();
	else
		OnCurrentlyopenfiles();
}
