// GameClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

#include "Server.h"
#include "HardwarePane.h"

#include "CommunicatorPC.h"
#include "CommunicatorPS2.h"
#include "CommunicatorXBOX.h"
#include "CommunicatorGameCube.h"

#include "ConnectingMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameClientApp

BEGIN_MESSAGE_MAP(CGameClientApp, CWinApp)
	//{{AFX_MSG_MAP(CGameClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameClientApp construction

CGameClientApp::CGameClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGameClientApp object

CGameClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGameClientApp initialization

BOOL CGameClientApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Parse the command line to see if launched as OLE server
	if (RunEmbedded() || RunAutomated())
	{
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();
	}
	else
	{
		// When a server application is launched stand-alone, it is a good idea
		//  to update the system registry in case it has been damaged.
		COleObjectFactory::UpdateRegistryAll();
	}

	// we use rich edit controls in dialogs, so we need this...
	if(!AfxInitRichEdit())
	{
		AfxMessageBox(IDS_RICHEDITINITFAIL);
		return FALSE;
	}

	m_Communicator = NULL;
	m_GameClientDlg = new CGameClientDlg();
	m_pMainWnd = m_GameClientDlg;
	int nResponse = m_GameClientDlg->DoModal();

	// delete the main dialog
	delete m_GameClientDlg;

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CGameClientApp::Disconnect()
{
	ASSERT(m_Communicator);
	if(m_Communicator->Disconnect())
	{
		delete m_Communicator;
		m_Communicator = NULL;
		return TRUE;
	}
	return FALSE;
}

BOOL CGameClientApp::Connect(HardwareType hardware, CString ipAddress, CString fileServerRoot)
{
	m_FileServerRoot = fileServerRoot;

	ASSERT(m_Communicator == NULL);

	switch(hardware)
	{
	case HT_PCLOCAL:
		m_Communicator = new CCommunicatorPC("127.0.0.1");
		break;
	case HT_PCREMOTE:
		m_Communicator = new CCommunicatorPC(ipAddress);
		break;
	case HT_XBOX:
		m_Communicator = new CCommunicatorXBOX(ipAddress);
		break;
	case HT_PS2:
		m_Communicator = new CCommunicatorPS2(ipAddress);
		break;
	case HT_GAMECUBE:
		m_Communicator = new CCommunicatorGameCube(ipAddress);
		break;
	default:
		AfxMessageBox("Unknown Hardware Type Requested", MB_OK|MB_ICONERROR);
		return FALSE;
	}

	m_GameClientDlg->EnableWindow(FALSE);
	CConnectingMessage message;
	message.Create(CConnectingMessage::IDD, m_GameClientDlg);
	
	BOOL result = TRUE;
	if(!m_Communicator->Connect())
	{
		ResetPanes();
		delete m_Communicator;
		m_Communicator = NULL;
		result = FALSE;
	}

	m_GameClientDlg->EnableWindow(TRUE);
	m_GameClientDlg->RedrawWindow();

	message.DestroyWindow();

	return result;
}

CString CGameClientApp::GetFileServerRoot()
{
	return m_FileServerRoot;
}

int CGameClientApp::AddPane(CGameClientPane *dlg, CString tabText)
{
	return m_GameClientDlg->AddPane(dlg, tabText);
}

void CGameClientApp::ResetPanes()
{
	m_GameClientDlg->ResetPanes();
}

BOOL CGameClientApp::IsConnected()
{
	return m_Communicator != NULL;
}

CString CGameClientApp::GetMyIP()
{
	UINT port;
	CString addr;
	CAsyncSocket socket;
	char buffer[100];
	gethostname(buffer, 100);
	socket.Create();
	socket.Connect(buffer, 7);
	socket.GetPeerName(addr, port);

	return addr;
}
