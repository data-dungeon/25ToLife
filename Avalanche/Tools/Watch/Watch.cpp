// Watch.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Watch.h"

#include "MainFrm.h"
#include "WatchDoc.h"
#include "WatchView.h"
#include "WatchCmdLine.h"
#include "WatchComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchApp

BEGIN_MESSAGE_MAP(CWatchApp, CWinApp)
	//{{AFX_MSG_MAP(CWatchApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

char* remoteMachine;

/////////////////////////////////////////////////////////////////////////////
// CWatchApp construction

CWatchApp::CWatchApp()
{
	remoteMachine = new char[256];
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CWatchApp::~CWatchApp()
{
	delete remoteMachine;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWatchApp object

CWatchApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWatchApp initialization

BOOL CWatchApp::InitInstance()
{
	// Initialize OLE
	if (!AfxOleInit())
	{
		AfxMessageBox("OLE Initialization failed.");
		return false;
	}
	AfxEnableControlContainer();

	// Why?  I don't know.
	if (RunEmbedded() || RunAutomated())
		return true;

	COleObjectFactory::UpdateRegistryAll();

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Avalanche"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWatchDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWatchView));
	AddDocTemplate(pDocTemplate);

#if 0 // Don't force them into a command line
	if (strlen(m_lpCmdLine) <= 0)
	{
		AfxMessageBox("Usage:  Watch <remote address>");
		return FALSE;
	}
#endif

	// Parse command line for standard shell commands, DDE, file open
	CWatchCmdLine cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CWatchApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWatchApp message handlers


BOOL CWatchApp::OnIdle(LONG lCount)
{
//	PSX2Comm::Instance()->Idle();

	return CWinApp::OnIdle(lCount);
}
