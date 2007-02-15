// PS2TestPane.cpp : implementation file
//

#include "stdafx.h"
#include "gameclient.h"
#include "PS2TestPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPS2TestPane dialog

CPS2TestPane::CPS2TestPane(CWnd* pParent /*=NULL*/)
	: CGameClientPane(CPS2TestPane::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPS2TestPane)
	m_LoadFile = theApp.GetProfileString("PS2Test", "LoadFile", "");
	//}}AFX_DATA_INIT

	IsCommunicating = FALSE;
}


void CPS2TestPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPS2TestPane)
	DDX_Text(pDX, IDC_LOADFILE, m_LoadFile);
	DDV_MaxChars(pDX, m_LoadFile, 300);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPS2TestPane, CDialog)
	//{{AFX_MSG_MAP(CPS2TestPane)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_HELLO, OnHello)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_LOADFILE_BROWSE, OnLoadfileBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPS2TestPane message handlers
#define	MCP_CODE_HELLO		0x0001
#define	MCP_CODE_HELLOR		0x0002
#define	MCP_CODE_EXEC		0x0003
#define	MCP_CODE_EXECR		0x0004
#define	MCP_CODE_RESET		0x0005
#define	MCP_CODE_RESETR		0x0006
#define MCP_CODE_KILL		0x0007
#define MCP_CODE_KILLR		0x0008

#define	MCP_TYPE_IOP		0x0001
#define	MCP_TYPE_EE			0x0002

#define MCP_EXEC_MAX_ARGC   32

#define MCP_ERR_OK                0
#define MCP_ERR_ALREADY_EXECUTING 1
#define MCP_ERR_CANNOT_LOAD_EE    2
#define MCP_ERR_CANNOT_LOAD_IOP   3
#define MCP_ERR_UNKNOWN_TARGET    4
#define MCP_ERR_NO_PROGRAM        5
#define MCP_ERR_BUSY              6

UINT CPS2TestPane::Hello(LPVOID TestPane)
{
	CPS2TestPane* testPane = (CPS2TestPane*)TestPane;

	PacketHdr hello;

	struct HelloResponse : public PacketHdr
	{
		unsigned int ident;
	} helloresponse;

	// just make sure the PS2 is there..
	CSocket socket;
	testPane->Status("Contacting PS2");
	socket.Create();
	if(!socket.Connect(testPane->GetDeviceIPAddress(), 9000))
	{
		int error = GetLastError();
		CString errmsg;
		errmsg.Format("Cant contact PS2.  Error=%d", error);
		testPane->Status(errmsg);
		testPane->SetCommunicating(FALSE);
		return FALSE;
	}

	testPane->SetCommunicating(FALSE);

	hello.code = MCP_CODE_HELLO;
	hello.id = 0;
	hello.length = 0;

	testPane->Status("Sending Hello");
	socket.Send(&hello, sizeof(hello));
	testPane->Status("Waiting for hello back");
	socket.Receive(&helloresponse, sizeof(helloresponse));
	testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " is operating correctly");

	return TRUE;
}

UINT CPS2TestPane::Reset(LPVOID TestPane)
{
	CPS2TestPane* testPane = (CPS2TestPane*)TestPane;

	PacketHdr reset;

	struct ResetResponse : public PacketHdr
	{
		unsigned long result;
		unsigned short reserved1;
	} resetresponse;

	// just make sure the PS2 is there..
	CSocket socket;
	testPane->Status("Contacting PS2");
	socket.Create();
	if(!socket.Connect(testPane->GetDeviceIPAddress(), 9000))
	{
		int error = GetLastError();
		CString errmsg;
		errmsg.Format("Cant contact PS2.  Error=%d", error);
		testPane->Status(errmsg);
		testPane->SetCommunicating(FALSE);
		return FALSE;
	}

	reset.code = MCP_CODE_RESET;
	reset.id = 0;
	reset.length = 0;

	testPane->Status("Sending Reset");
	socket.Send(&reset, sizeof(reset));
	testPane->Status("Waiting for reset reponse back");
	socket.Receive(&resetresponse, sizeof(resetresponse));

	testPane->SetCommunicating(FALSE);

	if(resetresponse.result == MCP_ERR_OK)
	{
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " has been reset");
		return TRUE;
	}

	CString errmsg;
	errmsg.Format("PS2 @ " + testPane->GetDeviceIPAddress() + " reports error %d when resetting", resetresponse.result);
	testPane->Status(errmsg);
	return FALSE;
}

UINT CPS2TestPane::Load(LPVOID TestPane)
{
	CPS2TestPane* testPane = (CPS2TestPane*)TestPane;

	// ok, now tell the thing to load...
	struct Exe : public PacketHdr
	{
		unsigned short type;
		unsigned short argc;
		unsigned long size;
		char filename[400];
	} exe;

	struct ExeResponse : public PacketHdr
	{
		unsigned long result;
		unsigned short reserved1;
	} exeresponse;

	CString filename;
	filename.Format("atfile:%s,%s%c\0", theApp.GetMyIP(), testPane->m_LoadFile, '\0');
	if(testPane->m_LoadFile.Find(".irx") != -1)
	{
		exe.type = MCP_TYPE_IOP;
	}
	else
	{
		exe.type = MCP_TYPE_EE;
	}

	exe.code = MCP_CODE_EXEC;
	exe.argc = 1;
	exe.length = 8;
	exe.size = filename.GetLength() + 1;
	exe.length += exe.size;

	// replace spaces with nulls...
	for(int i = 0; i < filename.GetLength(); i++)
	{
		if(filename[i] == ' ')
		{
			filename.SetAt(i, '\0');
			exe.argc++;
		}
	}

	memcpy(exe.filename, filename, filename.GetLength()+1);
	CSocket socket;
	testPane->Status("Contacting PS2");
	socket.Create();
	if(!socket.Connect(testPane->GetDeviceIPAddress(), 9000))
	{
		testPane->Status("Cant contact PS2");
		testPane->SetCommunicating(FALSE);
		return FALSE;
	}

	testPane->Status("Sending Execute command");
	socket.Send(&exe, exe.length + sizeof(PacketHdr));
	testPane->Status("Waiting for response to execute command");
	socket.Receive(&exeresponse, sizeof(exeresponse));

	testPane->SetCommunicating(FALSE);

	switch(exeresponse.result)
	{
	case MCP_ERR_OK:
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " should be executing " + filename);
		return TRUE;
	case MCP_ERR_ALREADY_EXECUTING:
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " reports already running a command");
		return FALSE;
	case MCP_ERR_CANNOT_LOAD_EE:
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " cant find EE executable " + filename);
		return FALSE;
	case MCP_ERR_CANNOT_LOAD_IOP:
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " cant find IOP executable " + filename);
		return FALSE;
	case MCP_ERR_UNKNOWN_TARGET:
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " reports unknown target");
		return FALSE;
	case MCP_ERR_NO_PROGRAM:
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " cant find executable " + filename);
		return FALSE;
	case MCP_ERR_BUSY:
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " is busy right now, please leave a message");
		return FALSE;
	default:
		testPane->Status("PS2 @ " + testPane->GetDeviceIPAddress() + " returned an unknown response");
		return FALSE;
	}
}

void CPS2TestPane::OnLoad() 
{
	ASSERT(!GetIsCommunicating());

	UpdateData(TRUE);
	theApp.WriteProfileString("PS2Test", "LoadFile", m_LoadFile);

	SetCommunicating(TRUE);
	AfxBeginThread(&CPS2TestPane::Load, this);
}

BOOL CPS2TestPane::Connect(CString ipAddress)
{
	ASSERT(!GetIsCommunicating());

	m_DeviceIPAddress = ipAddress;
	return Hello(this);
}

void CPS2TestPane::Status(CString string)
{
	CWnd* status = GetDlgItem(IDC_STATUS);
	if(IsWindow(*status))
		status->SetWindowText(string);
}

void CPS2TestPane::OnHello() 
{
	ASSERT(!GetIsCommunicating());

	SetCommunicating(TRUE);
	AfxBeginThread(&CPS2TestPane::Hello, this);
}

void CPS2TestPane::OnReset() 
{
	ASSERT(!GetIsCommunicating());

	SetCommunicating(TRUE);
	AfxBeginThread(&CPS2TestPane::Reset, this);
}

void CPS2TestPane::SetCommunicating(BOOL isCommunicating)
{
	IsCommunicating = isCommunicating;
	EnableWindow(!IsCommunicating);
}

BOOL CPS2TestPane::GetIsCommunicating()
{
	return IsCommunicating;
}

CString CPS2TestPane::GetDeviceIPAddress()
{
	return m_DeviceIPAddress;
}

void CPS2TestPane::OnLoadfileBrowse() 
{
	UpdateData(TRUE);

	CFileDialog dialog(
		TRUE, 
		NULL, 
		m_LoadFile, 
		OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		"Elf Files (*.elf)|*.elf|IRX Files (*.irx)|*.irx|All Files (*.*)|*.*||", 
		this);

	if(dialog.DoModal() == IDOK)
	{
		m_LoadFile = dialog.GetPathName();
		UpdateData(FALSE);
	}
}
