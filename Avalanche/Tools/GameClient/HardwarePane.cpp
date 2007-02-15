// HardwarePane.cpp : implementation file
//

#include "stdafx.h"
#include "gameclient.h"
#include "HardwarePane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHardwarePane dialog


CHardwarePane::CHardwarePane(CWnd* pParent /*=NULL*/)
	: CGameClientPane(CHardwarePane::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHardwarePane)
	m_DeviceIPAddressStr = theApp.GetProfileString("Hardware", "DeviceIPAddress", "10.0.0.0");
	m_FileServerRoot = theApp.GetProfileString("Hardware", "FileServerRoot", "");
	//}}AFX_DATA_INIT
}


void CHardwarePane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHardwarePane)
	DDX_Control(pDX, IDC_CONNECT, m_Connect);
	DDX_Control(pDX, IDC_HARDWARETYPE, m_HardwareType);
	DDX_Text(pDX, IDC_DEVICEIPADDRESS, m_DeviceIPAddressStr);
	DDX_Text(pDX, IDC_FILESERVERROOT, m_FileServerRoot);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHardwarePane, CDialog)
	//{{AFX_MSG_MAP(CHardwarePane)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_FILESERVERROOT_BROWSE, OnFileserverrootBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHardwarePane message handlers

HardwareType CHardwarePane::GetHardwareType()
{
	return (HardwareType)m_HardwareType.GetCurSel();
}

BOOL CHardwarePane::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_HardwareType.SetCurSel(theApp.GetProfileInt("Hardware", "HardwareType", 0));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHardwarePane::OnConnect() 
{
	int check = m_Connect.GetCheck();

	if(check == 1)
	{
		UpdateData(TRUE);
	
		if(m_FileServerRoot.GetAt(m_FileServerRoot.GetLength()-1) != '\\')
		{
			m_FileServerRoot += '\\';
			UpdateData(FALSE);
		}


		if(!theApp.Connect(GetHardwareType(), m_DeviceIPAddressStr, m_FileServerRoot))
		{
			m_Connect.SetCheck(0);
		}
		else
		{
			theApp.WriteProfileInt("Hardware", "HardwareType", m_HardwareType.GetCurSel());
			theApp.WriteProfileString("Hardware", "DeviceIPAddress", m_DeviceIPAddressStr);
			theApp.WriteProfileString("Hardware", "FileServerRoot", m_FileServerRoot);
			EnableForConnected(TRUE);
		}
	}
	else if(check == 0)
	{
		if(!theApp.Disconnect())
			m_Connect.SetCheck(1);
		else
			EnableForConnected(FALSE);
	}
}

CString CHardwarePane::GetDeviceIPAddress()
{
	return m_DeviceIPAddressStr;
}

CString CHardwarePane::GetFileServerRoot()
{
	return m_FileServerRoot;
}

void CHardwarePane::EnableForConnected(BOOL connected)
{
	int i;

	for(i = 3000; i < 4000; i++)
	{
		CWnd *wnd = GetDlgItem(i);
		if(!wnd) break;
		wnd->EnableWindow(!connected);
	}
	for(i = 4000; i < 5000; i++)
	{
		CWnd *wnd = GetDlgItem(i);
		if(!wnd) break;
		wnd->EnableWindow(connected);
	}
}


void CHardwarePane::OnFileserverrootBrowse() 
{
	UpdateData(TRUE);

	CFileDialog dialog(
		TRUE, 
		NULL, 
		NULL,
		OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		"All Files (*.*)|*.*||", 
		this);

	dialog.m_ofn.lpstrTitle = "Pick any file in the server root folder";

	if(dialog.DoModal() == IDOK)
	{
		m_FileServerRoot = dialog.GetPathName();
		int pos = m_FileServerRoot.ReverseFind('\\');
		if(pos == -1)
		{
			UpdateData(TRUE);
			AfxMessageBox("That file didnt seem to have a path!!");
			return;
		}
		m_FileServerRoot = m_FileServerRoot.Left(pos);
		UpdateData(FALSE);
	}
}
