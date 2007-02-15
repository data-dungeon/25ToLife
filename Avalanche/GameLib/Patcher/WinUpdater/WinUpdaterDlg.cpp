// WinUpdaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WinUpdater.h"
#include "WinUpdaterDlg.h"
#include "UpdaterCommandLine.h"
#include "PatchLib/Patcher.h"
#include "PatchExt/FilePatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Extra debugging output
#if 1
#define DBGPRINT PrintLine
#else
#define DBGPRINT __noop
#endif

// PAUSE is used to slow down the process to make sure the right things are
// displayed at the right time.
#if 0
	#define PAUSE PrintLine(false, "> "); Sleep(2000)
#else
	#define PAUSE
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CAboutDlg dialog used for App About
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

//------------------------------------------------------------------------------
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CWinUpdaterDlg dialog
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
CWinUpdaterDlg::CWinUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinUpdaterDlg::IDD, pParent)
	, m_statusString(_T(""))
	, m_currentAction(_T(""))
	, m_progressText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_state = DLOADSTATE_INIT;
	m_buf = NULL;
	m_bufsize = 0;
	m_parentHandle = NULL;
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::SetCommandLineInfo(const UpdaterCommandLine& cmdLine)
{
	// Before we start our dialog, get the process handle of the game that
	// launched us. Don't wait for it to exit until later, though, because it
	// is waiting for input idle on this process before exiting.
	// Note: we can ignore a NULL return here, because we just won't wait if
	// we don't have a valid parent process id.
	m_parentHandle = OpenProcess(SYNCHRONIZE, FALSE, cmdLine.m_parentProcessID);

	m_versionHost = cmdLine.m_versionHost;
	m_versionPath = cmdLine.m_versionPath;
	m_dirToPatch  = cmdLine.m_dirToPatch;
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressBarCtrl);
	DDX_Text(pDX, IDC_STATUS_INFO, m_statusString);
	DDX_Text(pDX, IDC_CURRENT_ACTION, m_currentAction);
	DDX_Text(pDX, IDC_PROGRESS_TEXT, m_progressText);
	DDX_Control(pDX, IDC_STATUS_INFO, m_statusCtrl);
	DDX_Control(pDX, IDC_TITLE_TEXT, m_titleTextCtrl);
	DDX_Control(pDX, IDC_RESTART_INSTRUCTIONS, m_restartTextCtrl);
	DDX_Control(pDX, IDC_STATUS_TITLE, m_statusTitle);
	DDX_Control(pDX, IDC_CURRENT_ACTION, m_currentActionCtrl);
	DDX_Control(pDX, IDC_PROGRESS_TEXT, m_progressTextCtrl);
	DDX_Control(pDX, IDOK, m_restartButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);

	if (!pDX->m_bSaveAndValidate)
	{
		int pos = m_statusString.GetLength();
		m_statusCtrl.SetSel(pos, pos, FALSE);
		m_statusCtrl.HideCaret();
	}
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::ChangeState(DloadState newState)
{
	m_state = newState;
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::PrintLine(bool endline, char* fmt, ...)
{
	va_list args;
	char buffer[512];

	va_start( args, fmt );

	// Format the output into our buffer
	vsprintf( buffer, fmt, args );

	// Put the string onto our current output
	m_statusString += buffer;
	if (endline)
		m_statusString += "\r\n";

	// Keep the string within the size limits of a simple edit control
	#define MAXOUTPUTSTRLEN 32767
	while (m_statusString.GetLength() > MAXOUTPUTSTRLEN)
	{
		int trimPos = m_statusString.Find("\r\n");
		if (trimPos < 0)
			trimPos = m_statusString.GetLength() - MAXOUTPUTSTRLEN;
		trimPos += 2;

		int count = m_statusString.GetLength() - trimPos;
		m_statusString = m_statusString.Right(count);
	}

	UpdateData(FALSE);
	m_statusCtrl.UpdateWindow();
}


BEGIN_MESSAGE_MAP(CWinUpdaterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CWinUpdaterDlg message handlers
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
BOOL CWinUpdaterDlg::OnInitDialog()
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

	// DFS -- Extra init

	// Make our title font bold (wish there was a flag to do then when
	// defining the dialog...)
	LOGFONT lfont;
	CFont* titleFont = m_titleTextCtrl.GetFont();
	titleFont->GetLogFont(&lfont);
	lfont.lfWeight = 700;

	CFont newFont;
	newFont.CreateFontIndirect(&lfont);
	m_titleTextCtrl.SetFont(&newFont);

	// Create an updater
	Patcher::Create(1);

	// Set our initial state
	ChangeState(DLOADSTATE_READY);

	// Start our timer
	m_timer = SetTimer(1, 60, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWinUpdaterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWinUpdaterDlg::OnPaint() 
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

//------------------------------------------------------------------------------
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWinUpdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::OnTimer(UINT nIDEvent)
{
	Patcher::Status stat;
	Patcher* pPatcher = Patcher::GetPatcher();

	pPatcher->Update(0.06f);

	switch (m_state)
	{
		case DLOADSTATE_READY:
			m_currentAction = "Downloading:";
			PrintLine(false, "Downloading version information... ");
			stat = pPatcher->BeginVersionCheck(m_versionHost.c_str(), m_versionPath.c_str(), 0);
			if (stat == Patcher::PATCH_STATUS_OK) {
				ChangeState(DLOADSTATE_TESTING_VERSION);
			}
			else {
				PrintLine(true, "BeginVersionCheck failed! Status = %d", (int)stat);
				ChangeState(DLOADSTATE_ERROR);
			}
			break;

		case DLOADSTATE_TESTING_VERSION:
			VersionCheckWork();
			break;

		case DLOADSTATE_UPDATE_NEEDED:
			PrintLine(false, "Downloading patch, %d bytes... ", m_bufsize);
			stat = pPatcher->DownloadPatch(m_buf, m_bufsize);
			if (stat == Patcher::PATCH_STATUS_OK) {
				m_progressBarCtrl.SetRange32(0, m_bufsize);
				m_progressBarCtrl.SetPos(0);
				ChangeState(DLOADSTATE_UPDATING);
			}
			else {
				PrintLine(true, "Failed! Status = %d", (int)stat);
				ChangeState(DLOADSTATE_ERROR);
			}
			break;

		case DLOADSTATE_UPDATING:
			UpdatingWork();
			break;

		case DLOADSTATE_PREPARING:
			PreparingWork();
			break;

		case DLOADSTATE_PATCHABLE:
			m_currentAction = "Installing:";
			ApplyPatches();
			break;

		case DLOADSTATE_ERROR:
			// Change the message at the top of the dialog
			m_titleTextCtrl.SetWindowText("An error occurred while trying to update your game");
			PrintErrorDetails();
			ChangeState(DLOADSTATE_DONE);
			break;

		case DLOADSTATE_EXITING:
			// Change the message at the top of the dialog
			m_titleTextCtrl.SetWindowText("You have successfully updated your game.");

			// Reveal the restart message and the restart button.
			m_restartTextCtrl.ShowWindow(SW_SHOW);
			m_restartButton.ShowWindow(SW_SHOW);

			// Change the Cancel button to a Close button
			m_cancelButton.SetWindowText("Close");

			// Make the Restart button the default button
			PostMessage(WM_NEXTDLGCTL, (WPARAM)m_restartButton.GetSafeHwnd(), (LPARAM)TRUE);

			// Hide all the other controls
			m_statusCtrl.ShowWindow(SW_HIDE);
			m_statusTitle.ShowWindow(SW_HIDE);
			m_currentActionCtrl.ShowWindow(SW_HIDE);
			m_progressTextCtrl.ShowWindow(SW_HIDE);
			m_progressBarCtrl.ShowWindow(SW_HIDE);

			// Cleanup memory etc.
			Cleanup();
			Patcher::Destroy();
			KillTimer(m_timer);
			ChangeState(DLOADSTATE_DONE);
			break;
	}

	CDialog::OnTimer(nIDEvent);
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::VersionCheckWork(void)
{
	Patcher::Status stat;
	Patcher::CompletionInfo info;
	Patcher* pPatcher = Patcher::GetPatcher();

	PAUSE;
	stat = pPatcher->TestCompletion(info);
	switch (stat) {
		case Patcher::PATCH_STATUS_OK:
			PrintLine(true, "done!");
			//PrintLine(true, "Version check complete, required bufsize=%d, storesize=%d, clearsize=%d", info.reqBufSize, info.storeSize, info.clearSize);
			if (info.reqBufSize && info.storeSize) {
				if (m_buf && (info.reqBufSize > m_bufsize)) {
					delete [] m_buf;
					m_buf = NULL;
				}
				if (!m_buf) {
					m_buf = new char[info.reqBufSize];
					m_bufsize = m_buf ? info.reqBufSize : 0;
				}
				if (m_buf) {
					ChangeState(DLOADSTATE_UPDATE_NEEDED);
				}
				else {
					PrintLine(true, "Out of memory!");
					PrintLine(true, "Please free some hard drive space and try again.");
					ChangeState(DLOADSTATE_ERROR);
				}
			}
			else {
				// No update available.
				PrintLine(true, "No update available!");
				// For now, just bail whether an error occurred or there is no update
				ChangeState(DLOADSTATE_ERROR);
			}
			break;

		case Patcher::PATCH_STATUS_INIT_FAILED:
		case Patcher::PATCH_STATUS_NET_ERROR:
			PrintLine(true, "Failed!");
			PrintLine(true, "Check of version information failed, aborting...");
			ChangeState(DLOADSTATE_ERROR);
			break;

		case Patcher::PATCH_STATUS_BUSY:
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::UpdatingWork(void)
{
	Patcher::Status stat;
	Patcher::CompletionInfo info;
	Patcher* pPatcher = Patcher::GetPatcher();

	// Get the current progress information
	int downloaded = pPatcher->GetBytesDownloaded();
	m_progressBarCtrl.SetPos(downloaded);

	float fDownloaded = (float)downloaded / 1024.0f;
	float fTotal = (float)m_bufsize / 1024.0f;
	m_progressText.Format("%0.2f KB of %0.2f KB", fDownloaded, fTotal);
	m_progressTextCtrl.SetWindowText(m_progressText);

	PAUSE;
	stat = pPatcher->TestCompletion(info);
	switch (stat) {
		case Patcher::PATCH_STATUS_OK:
			PrintLine(true, "done!");
			PrintLine(false, "Preparing patch... ");
			pPatcher->PreparePatch();
			ChangeState(DLOADSTATE_PREPARING);
			break;
		case Patcher::PATCH_STATUS_INIT_FAILED:
		case Patcher::PATCH_STATUS_NET_ERROR:
			PrintLine(true, "Failed with network error!");
			ChangeState(DLOADSTATE_ERROR);
			break;
		case Patcher::PATCH_STATUS_BUSY:
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::PreparingWork(void)
{
	Patcher::CompletionInfo info;

	PAUSE;
	Patcher::Status stat = Patcher::GetPatcher()->TestCompletion(info);
	switch (stat) {
		case Patcher::PATCH_STATUS_OK:
			PrintLine(true, "done!");
			ChangeState(DLOADSTATE_PATCHABLE);
			break;
		case Patcher::PATCH_STATUS_BUSY:
			break;
		default:
			PrintLine(true, "Failed!");
			PrintLine(true, "An error occurred during preparation, aborting.");
			ChangeState(DLOADSTATE_ERROR);
			break;
	}
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::ApplyPatches(void)
{
	// Before we start patching, we need to make sure that the process that created
	// this process has exited.
	if (m_parentHandle) {
		static bool waited = false;

		// Wait for up to 8 milliseconds. If the process has not exited,
		// let the user know why we are waiting. Note: we can't just wait here
		// because this is the UI thread. Normally it is bad to spin, but this
		// will never happen in a properly written game (it will have already
		// exited by the time we get here).
		DWORD done = WaitForSingleObject(m_parentHandle, 8);
		if (done == WAIT_TIMEOUT) {
			// Timed out. Print a message, and return to try again later
			if (!waited) {
				PrintLine(true, "Waiting for game to exit...");
				waited = true;
				}
			return;
		}
		else {
			CloseHandle(m_parentHandle);
			m_parentHandle = NULL;
		}
	}

	CWaitCursor wait;

	FilePatcher patcher(m_dirToPatch, m_buf, m_bufsize);
	if (!patcher.IsGood()) {
		PrintLine(true, "Error initializing patch extractor!");
		PrintLine(true, "The patch data has a problem, perhaps an error occurred during download.");
		ChangeState(DLOADSTATE_ERROR);
		return;
	}

	// Set up the status bar range
	int totalCount = patcher.GetFileCount();
	m_progressBarCtrl.SetRange32(0, totalCount);
	m_progressBarCtrl.SetPos(0);

	// Go ahead and patch everything
	int count = 1;
	bool failed = false;
	int pstat;
	std::string fname;
	for (count = 1, failed = false; (count <= totalCount) && !failed; count++) {
		m_progressText.Format("File %d of %d", count, totalCount);

		pstat = patcher.FindNextFile(fname);
		if (pstat < 0) {
			failed = true;
			PrintLine(true, "Error occurred trying to find next file!!!");
		}
		else if (pstat > 0) {
			PrintLine(false, "Installing %s (update %d of %d)... ", fname.c_str(), count, totalCount);
			pstat = patcher.PatchCurrentFile();
			PAUSE;
			if (pstat > 0) {
				PrintLine(true, "done!");
			}
			else if (pstat < 0) {
				PrintLine(true, "FAILED!!!");
				failed = true;
			}
		}
		m_progressBarCtrl.SetPos(count);
	}

	if (!failed) {
		PrintLine(true, "All files patched successfully!");
		ChangeState(DLOADSTATE_EXITING);
	}
	else {
		PrintLine(true, "Patching Failed!!!!!!!!!");
		ChangeState(DLOADSTATE_ERROR);
	}
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::Cleanup(void)
{
	if (m_buf) {
		delete [] m_buf;
		m_buf = NULL;
		m_bufsize = 0;
	}
}

//------------------------------------------------------------------------------
void CWinUpdaterDlg::PrintErrorDetails(void)
{
	PatchError eCode = Patcher::GetPatcher()->GetPatchError();

	char* pMsg = NULL;

	switch (eCode)
	{
		// Internal errors -- These spot development problems that should never happen in the finished game.
		case PATCH_ERROR_SOCKET:			// Unable to create socket (out of memory?)
			pMsg = "Unable to open socket on network; out of memory or network configuration error.";
			break;
		case PATCH_ERROR_BAD_RESOURCE:		// The string length of the requested resource is too long. Shorten it or make the temp buffer bigger.
			pMsg = "Internal error: bad patch information was provided to the update application.";
			break;
		case PATCH_ERROR_BUF_TOO_SMALL:		// The buffer passed for reading is too small
			pMsg = "Internal error: buffer is too small";
			break;
		case PATCH_ERROR_PARSE_ERROR:		// Error parsing http response; perhaps we are not compatible with server
			pMsg = "Error parsing HTTP response; possible server issue.";
			break;

		// Encrypt/decrypt errors (not currently used for Windows)
		case PATCH_ERROR_CRYPT_INIT:		// Error initializing decrypt/encrypt system
		case PATCH_ERROR_ENCRYPT_SIZE:		// Error obtaining size needed to encrypt the data for storage
		case PATCH_ERROR_ENCRYPT:			// Error encrypting the data (reason unknown)
		case PATCH_ERROR_DECRYPT_SIZE:		// Error obtaining size needed to decrypt data
		case PATCH_ERROR_DECRYPT:			// Error decrypting the patch
			pMsg = "Internal error: encrypt/decrypt error";
			break;

		// Server configuration problems -- These should not happen, but could if someone does something bad
		// on the server
		case PATCH_ERROR_REDIRECT:				// Server tried to redirect us to another URI
			pMsg = "Server configuration error: attempt to redirect not allowed.";
			break;
		case PATCH_ERROR_HTTP_CLIENT:			// An http 4xx error occurred other than file not found (404)
			pMsg = "Internal error: Server did not like our request.";
			break;
		case PATCH_ERROR_HTTP_SERVER:			// The http server had an internal error
			pMsg = "Server reported an internal error; perhaps try again.";
			break;
		case PATCH_ERROR_UNRECOGNIZED_RESPONSE:	// An http 1xx or 2xx response that is not 200 (OK)
			pMsg = "Server configuration error: unrecognized response";
			break;

			// Network errors -- These can and will occur in the finished game and should be handled.
		case PATCH_ERROR_HOSTNOTFOUND:		// Unable to find requested host (name server problem or bad host name)
			pMsg = "Error looking up address for patch server; host name may not be registered.";
			break;
		case PATCH_ERROR_NAMESERVER_ERROR:	// Unable to resolve host name because of a problem with the name server
			pMsg = "Error looking up address for patch server; name server may be down or unreachable.";
			break;
		case PATCH_ERROR_NOCONNECT:			// connect failed; host may not be currently available
		case PATCH_ERROR_TIMEOUT:			// Network timeout
			pMsg = "Patch server is not responding; try again later.";
			break;
		case PATCH_ERROR_SENDFAIL:			// send failed; host may be having problems
			pMsg = "Problem sending information to patch server; try again later.";
			break;
		case PATCH_ERROR_POLLERROR:			// Error occurred while polling socket (should save more info)
		case PATCH_ERROR_RECVERROR:			// Error occurred while reading on the socket (should save more info)
			pMsg = "Error receiving response from server; cause unknown.";
			break;

		// OK errors -- errors that occur in normal processing, even when everything works
		case PATCH_ERROR_FILE_NOT_FOUND:	// The requested resource was not found on the server
		case PATCH_ERROR_NONE:				// No error has occurred
		default:
			break;
	}
	if (pMsg)
		PrintLine(true, pMsg);
}
