// Dan  4/11/00  \Dan85\AMViewer\AMViewer.cpp
//

#include "stdafx.h"
#include "AMViewerDoc.h"
#include "AMViewer.h"
#include "MainFrm.h"
#include "AMViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *g_mainframe;

BEGIN_MESSAGE_MAP(CAMViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CAMViewerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()

CAMViewerApp::CAMViewerApp()
{
   m_doc = NULL;
}

CAMViewerApp theApp;

BOOL CAMViewerApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

   SetRegistryKey(_T("Hash, Inc."));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAMViewerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAMViewerView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

BOOL CAMViewerApp::OnIdle(LONG lCount) 
{
   BOOL moreidle;

   moreidle = CWinApp::OnIdle(lCount);

   if (g_mainframe && g_mainframe->m_modelview)
      if (g_mainframe->m_modelview->TimerProc())
         moreidle = TRUE;

	return moreidle;
}

BOOL CAMViewerApp::PreTranslateMessage(MSG* pMsg) 
{
//   if (g_mainframe && g_mainframe->m_modelview)
//      g_mainframe->m_modelview->TimerProc();
	return CWinApp::PreTranslateMessage(pMsg);
}

void CAMViewerApp::OnFileOpen()
{
#ifdef _MAC
   static char BASED_CODE szFilter[] = "Script Files (*.spt)|****||";
#else
   static char BASED_CODE szFilter[] = "Script Files (*.spt)|*.spt||";
#endif

   CString filename;
   
   if (m_doc)
      filename = m_doc->GetPathName();
   if (filename.IsEmpty())
      filename = AfxGetApp()->GetProfileString( "Files", "Last Script File" );
   
   CFileDialog fd(TRUE, "spt", filename, OFN_HIDEREADONLY, szFilter);

   if (fd.DoModal()!=IDOK)
      return;

   AfxGetApp()->WriteProfileString( "Files", "Last Script File", fd.GetPathName() );

   OpenDocumentFile( fd.GetPathName() );
}

//******************* About *************************

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

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
   CString m_version, m_vendor, m_renderer;
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
   m_version = glGetString( GL_VERSION );
   m_vendor = glGetString( GL_VENDOR );
   m_renderer = glGetString( GL_RENDERER );
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
   DDX_Text(pDX, IDC_VERSION, m_version);
   DDX_Text(pDX, IDC_VENDOR, m_vendor);
   DDX_Text(pDX, IDC_RENDERER, m_renderer);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAMViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


