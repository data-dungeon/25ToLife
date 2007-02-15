// TupViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TupViewer.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "TupViewerDoc.h"
#include "TupViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTupViewerApp

BEGIN_MESSAGE_MAP(CTupViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CTupViewerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CTupViewerApp construction

CTupViewerApp::CTupViewerApp() :
	m_pSelectedAtom(NULL),
	m_pTupRoot(NULL),
	m_pTupBuffers(NULL)
{
}

CTupViewerApp::~CTupViewerApp() 
{
	m_pSelectedAtom = NULL;
	if (m_pTupBuffers)
	{
		int numBuffers = m_pTupBuffers->GetSize();
		for (int index=0;index<numBuffers;index++)
		{
			delete [] (*m_pTupBuffers)[index].m_pBuffer;
			(*m_pTupBuffers)[index].m_pBuffer = NULL;
		}
		delete m_pTupBuffers;
	}
	delete m_pTupRoot;
}


// The one and only CTupViewerApp object

CTupViewerApp theApp;

// CTupViewerApp initialization




/////////////////////////////////////////////////////////////////////////////
// Prof-UIS advanced options

void CTupViewerApp::SetupUiAdvancedOptions()
{

	//
	// Prof-UIS command manager profile
	//
	VERIFY(
		g_CmdManager->ProfileSetup(
			__PROF_UIS_PROJECT_CMD_PROFILE_NAME
			)
		);

	//
	// General UI look
	//
        // For change current UI look change  CExtPaintManagerXP on
	//  CExtPaintManager - FOR OFFICE 2000 UI STYLE
	//  CExtPaintManagerXP - FOR OFFICE XP UI STYLE
	//  CExtPaintManagerOffice2003 - FOR OFFICE 2003 UI STYLE
	g_PaintManager.InstallPaintManager(
		new CExtPaintManagerXP
       	);


	//
	// Popup menu option: Display menu shadows
	//
	CExtPopupMenuWnd::g_bMenuWithShadows = true;
	//
	// Popup menu option: Display menu cool tips
	//
	CExtPopupMenuWnd::g_bMenuShowCoolTips = true;
	//
	// Popup menu option: Initially hide rarely used items (RUI)
	//
	CExtPopupMenuWnd::g_bMenuExpanding = true;
	//
	// Popup menu option: Display RUI in different style
	//
	CExtPopupMenuWnd::g_bMenuHighlightRarely = true;
	//
	// Popup menu option: Animate when expanding RUI (like Office XP)
	//
	CExtPopupMenuWnd::g_bMenuExpandAnimation = true;
	//
	// Popup menu option: Align to desktop work area (false - to screen area)
	//
	CExtPopupMenuWnd::g_bUseDesktopWorkArea = true;
	//
	// Popup menu option: Popup menu animation effect (when displaying)
	//
	CExtPopupMenuWnd::g_DefAnimationType =
		CExtPopupMenuWnd::__AT_FADE;
		//$$__PROFUISAPPWIZ_KEY_MENU_ANIM_DISPMS$$

}



BOOL CTupViewerApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();


	//
	// Prof-UIS advanced options
        //
	SetupUiAdvancedOptions();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Avalanche Software"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views

	CMDIFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create main ProfStudio frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;


	// try to load shared ProfStudio menus and accelerator table
	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need. 

	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_TUP_VIEWER_TYPE));
	m_hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_TUP_VIEWER_TYPE));

	// window placement persistence
	pFrame->ActivateFrame( m_nCmdShow );

	return TRUE;
}

int CTupViewerApp::ExitInstance() 
{
	//TODO: handle additional resources you may have added
	if (m_hMenu != NULL)
		FreeResource(m_hMenu);
	if (m_hAccel != NULL)
		FreeResource(m_hAccel);

	return CWinApp::ExitInstance();
}


// CAboutDlg dialog used for App About
class CAboutDlg : public CExtResizableDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	CExtButton	m_BtnOK;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CExtResizableDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_BtnOK);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CExtResizableDialog)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
	VERIFY( CExtResizableDialog::OnInitDialog() );

	//
	// Show size gripper
	//
	CExtResizableDialog::ShowSizeGrip( FALSE );

	//
	// Enable dialog position saving/restoring in registry
	//
	CExtResizableDialog::EnableSaveRestore(
		__PROF_UIS_PROJECT_DLG_PERSIST_REG_KEY,
		_T("CAboutDlg-Saved-Position")
		);

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// App command to run the dialog
void CTupViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CTupViewerApp::OnFileNew()
{
	if (m_pTupRoot)
		delete m_pTupRoot;
	m_pTupRoot = new TupperwareAggregate;
	
	CMainFrame * pFrame =
		STATIC_DOWNCAST(
		CMainFrame,
		m_pMainWnd
		);
	pFrame->RefreshAll();
}

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


void CTupViewerApp::OnFileOpen()
{
	static char szFilter[] = "Tup Files|*.tup|"  \
		"All Files (*.*)|*.*||";

	static char szFile[] = "";

	CString fileString;
	if (BrowseFile(m_pMainWnd,"Load Tup File",szFilter,szFile,TRUE,fileString))
	{
		if (m_pTupRoot)
			delete m_pTupRoot;
		if (m_pTupBuffers)
		{
			int numBuffers = m_pTupBuffers->GetSize();
			for (int index=0;index<numBuffers;index++)
			{
				delete [] (*m_pTupBuffers)[index].m_pBuffer;
				(*m_pTupBuffers)[index].m_pBuffer = NULL;
			}
			delete m_pTupBuffers;
		}
		m_pTupBuffers = new TupArray<TupperwareReader::FileBuffer>;

		TupperwareLoadSaveManager::Read(fileString,&m_pTupRoot,m_pTupBuffers);
		CMainFrame * pFrame = STATIC_DOWNCAST(CMainFrame,m_pMainWnd);
		pFrame->RefreshAll();
	}
}

void CTupViewerApp::RefreshAll(void)
{
	CMainFrame * pFrame =
		STATIC_DOWNCAST(
		CMainFrame,
		m_pMainWnd
		);
	pFrame->RefreshAll();
}

void CTupViewerApp::RefreshSelected(int windowFlags,void *pData)
{
	CMainFrame * pFrame =
		STATIC_DOWNCAST(
		CMainFrame,
		m_pMainWnd
		);
	pFrame->RefreshSelected(windowFlags,pData);
}

