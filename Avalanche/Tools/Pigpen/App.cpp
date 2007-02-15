/***********************************/
/*                                 */
/* App.cpp                         */
/* Pigpen particle tool  12/07/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* main application class          */
/*                                 */
/***********************************/

/* this defines the main application for the particle tool */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/

#include <Layers/Memory.h>	// for MEM_USE_WIN32_MANAGER #define



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************* defines *****************************/

/******************* structures **************************/

/******************* local variables *********************/

/******************* externs *****************************/

/******************* class functions *********************/

/////////////////////////////////////////////////////////////////////////////
// CPigpenApp

BEGIN_MESSAGE_MAP(CPigpenApp, CWinApp)
	//{{AFX_MSG_MAP(CPigpenApp)
	ON_COMMAND(ID_VIEW_SFX, OnViewSFX)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)					// override OnFileOpen
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPigpenApp construction

CPigpenApp::CPigpenApp()
{
	/* no opened doc, no window restore info */

	m_pOpenedDoc = NULL;
	m_bRestoreWindow = FALSE;

	/* command line stuff */

	m_pCommandLineDoc = NULL;
	m_bCommandLineMode = FALSE;
	m_pExportFileName = NULL;
	m_nExportMode = EXPORT_PS2;	// default

	/* return code */

	m_nReturnCode = RETURN_CODE_SUCCESS;

	// Place all significant initialization in InitInstance
}

CPigpenApp::~CPigpenApp()
{
	/* shut down the game engine if necessary */

	if (GameEngine__IsRunning())
		GameEngine__Shutdown();

	/* clean up sound processing */

	CleanUpSound();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPigpenApp object

CPigpenApp PigpenApp;

/////////////////////////////////////////////////////////////////////////////
// CPigpenApp initialization

BOOL CPigpenApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Avalanche Software"));

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	pDocTemplate = new CMultiDocTemplate(
		IDR_VIEW,
		RUNTIME_CLASS(CPigpenDoc),
		RUNTIME_CLASS(CChildFrame),    // custom MDI child frame, so i have control over window title
		RUNTIME_CLASS(CDocView));

	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	/* don't open a new file unless specified on command line */

	if (strlen(cmdInfo.m_strFileName) == 0)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	/* determine if we are in command line mode-- just load up a doc and export it. do it before call to */
	/* ProcessShellCommand so that m_pCommandLineDoc gets set during doc opening */

	if (__argc >= 4)	// must have at least source file, dest file, export mode (plus executable name)
	{
		m_bCommandLineMode = TRUE;
		m_pExportFileName = __targv[2];	// argument 1 is source file name, handled by ParseCommandLine and ProcessShellCommand

		/* don't show window in command line mode */

		m_nCmdShow = 0;
	
		/* process rest of arguments */

		for (int i = 3; i < __argc; i++)
		{
			/* currently just export mode, more can be added */

			if (stricmp(__targv[i], "-PS2") == 0)
				m_nExportMode = EXPORT_PS2;
			if (stricmp(__targv[i], "-WIN32") == 0)
				m_nExportMode = EXPORT_PC;
			if (stricmp(__targv[i], "-GCN") == 0)
				m_nExportMode = EXPORT_GAMECUBE;
			if (stricmp(__targv[i], "-XBOX") == 0)
				m_nExportMode = EXPORT_XBOX;
		}
	}

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();

	/* set default preview stage file name */

	char CurrentDir[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, CurrentDir);
	DefaultStageFile = CurrentDir;
	DefaultStageFile += "\\Stage.dbl";

	/* we insist that win32 memory manager is used */

#ifndef MEM_USE_WIN32_MANAGER
	/* if this hits, you need to uncomment the #define MEM_USE_WIN32_MANAGER line in Layers\Memory.h. */
	/* pigpen needs to use the win32 memory manager because when the game engine is initialized it */
	/* typically creates its own heap, and then frees it when the game engine is shut down. if pigpen- */
	/* related memory is allocated out of that heap during the preview process, it gets orphaned when */
	/* the layers heap is freed */

	_ASSERT(false);
	return(false);
#endif //MEM_USE_WIN32_MANAGER

	/* create status bar control */

//yuch-- i wasn't able to get this to work. an mfc sample would be nice...
//	CStatusBar StatusBar;
//	StatusBar.Create(m_pMainWnd);
//	StatusBar.GetStatusBarCtrl();
//	StatusBar.SetParent(
//	StatusBar.SetPaneText(0, "Hello");

	/* load sound list */

	LoadSounds(__targv[0]);

	/* maybe export and close */

	if ((m_bCommandLineMode) && (m_pCommandLineDoc))
	{
		m_pCommandLineDoc->ExportDBLFile(m_nExportMode, m_pExportFileName);
		return(false);	// to close application
	}

	/* set assert handler */

	Assert::SetHandler(Assert::PigpenHandler);

	/* success */

	return(true);
}

/* override this so we can return an error code */

int CPigpenApp::ExitInstance()
{
	SaveStdProfileSettings();	// save mru status
	return(m_nReturnCode);
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

/* view the sfx.h sound effects list file */

void CPigpenApp::OnViewSFX() 
{
	if (SoundFileFound())
		WinExec("NOTEPAD.EXE SFX.h", SW_SHOW);
	else
		WarnNoSoundFile();
}

// App command to run the dialog
void CPigpenApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPigpenApp message handlers

void CPigpenApp::OnFileOpen()
{
	/* instead of letting base class fire, copy some code so i can use registry to save file name */

#if 0	// do not enable this section! it is for reference only so i can replicate it's functionality but add my own hooks
	/******************** MFC code *********************/

	/* this code is from CWinApp::OnFileOpen, mfc\src\appdlg.cpp, line 37 */

	ASSERT(m_pDocManager != NULL);
	m_pDocManager->OnFileOpen();

	/* this code is from CDocManager::OnFileOpen, mfc\src\docmgr.cpp, line 836 */

	// prompt the user (with all document templates)
	CString newName;
	if (!DoPromptFileName(newName, AFX_IDS_OPENFILE,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, NULL))
		return; // open cancelled
	AfxGetApp()->OpenDocumentFile(newName);

	/******************** end MFC code *****************/
#endif //0

	/* get last file name from registry */

	char InitialPath[_MAX_PATH];
	char Filename[_MAX_PATH];
	LoadPersistentString("Pigpen Load", InitialPath, sizeof(InitialPath));
	DissectPath(InitialPath, InitialPath, Filename);

	/* file dialog */

	CFileDialog FileDialog(
		TRUE,						// BOOL bOpenFileDialog
		"pig",					// LPCTSTR lpszDefExt = NULL
		Filename,				// LPCTSTR lpszFileName = NULL
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,	// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT-- HIDEREADONLY hides a dialog check box, not read only files
		"Pigpen Files (*.pig)|*.pig||",		// LPCTSTR lpszFilter = NULL, apparently can't have spaces around | symbols
		NULL);					// CWnd* pParentWnd = NULL
	FileDialog.m_ofn.lpstrInitialDir = InitialPath;	// set initial directory
	if (FileDialog.DoModal() != IDOK)
		return;

	/* get file name, save in registry */

	CString newName = FileDialog.GetPathName();
	MakePersistentString("Pigpen Load", newName.GetBuffer(10));

	/* open doc */

	OpenDocumentFile(newName);
}

/* override opening a document so that i can open correct windows (their status is loaded during serialization) */

CDocument* CPigpenApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	/* maybe not there? */

	FILE *pFile = fopen(lpszFileName, "rb");
	if (pFile == NULL)
	{
		char Message[200];
		sprintf(Message, "File \"%s\" not found.", lpszFileName);
		AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
		SetReturnCode(RETURN_CODE_PIG_FILE_NOT_FOUND);
		return(NULL);
	}
	fclose(pFile);

	/* automatic backup */

	if (strstr(lpszFileName, "PigpenBackup") == NULL)	// don't back up backup files
	{
		char TempPath[_MAX_PATH];
		if (GetEnvironmentVariable("TEMP", TempPath, _MAX_PATH) != 0)	// only backup if they have TEMP environment variable defined
		{
#define MAX_BACKUP_FILES	5
			char BackupFile[_MAX_PATH];
			char BackupFile2[_MAX_PATH];

			/* delete oldest backup file-- may not be one yet, of course */

			sprintf(BackupFile, "%s\\PigpenBackup%d.pig", TempPath, MAX_BACKUP_FILES);
			remove(BackupFile);

			/* rename other backup files to move them down one in history chain */

			for (int i = MAX_BACKUP_FILES - 1; i >= 1; i--)
			{
				sprintf(BackupFile, "%s\\PigpenBackup%d.pig", TempPath, i);
				sprintf(BackupFile2, "%s\\PigpenBackup%d.pig", TempPath, i + 1);
				rename(BackupFile, BackupFile2);
			}

			/* and copy pig file being loaded to backup file #1 */

			FILE *pSourceFile = fopen(lpszFileName, "rb");
			ASSERT(pSourceFile);
			sprintf(BackupFile, "%s\\PigpenBackup1.pig", TempPath);
			FILE *pBackupFile = fopen(BackupFile, "wb");
			ASSERT(pBackupFile);
			if ((pSourceFile) && (pBackupFile))
			{
				fseek(pSourceFile, 0, SEEK_END);	// to end of file
				int nSize = ftell(pSourceFile);
				fseek(pSourceFile, 0, SEEK_SET);	// back to start of file
				void *pBuffer = malloc(nSize);
				ASSERT(pBuffer);
				if (pBuffer)
				{
					fread(pBuffer, nSize, 1, pSourceFile);
					fwrite(pBuffer, nSize, 1, pBackupFile);
					free(pBuffer);
				}
			}
			if (pSourceFile)
				fclose(pSourceFile);
			if (pBackupFile)
				fclose(pBackupFile);
		}
	}

	/* let base class fire */

	m_pOpenedDoc = NULL;
	CDocument *pDoc = CWinApp::OpenDocumentFile(lpszFileName);

	/* record doc opened in command line mode */

	if (m_bCommandLineMode)
		m_pCommandLineDoc = m_pOpenedDoc;

	/* open up additional windows, if any */

	if ((pDoc) && (m_pOpenedDoc) && (m_bCommandLineMode == FALSE))	// may have canceled open
	{
		m_pOpenedDoc->OpenLoadedWindows();
		m_pOpenedDoc = NULL;	// done opening
	}
	return(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CPigpenApp routines

