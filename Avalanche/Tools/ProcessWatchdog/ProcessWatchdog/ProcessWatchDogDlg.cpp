// ProcessWatchDogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessWatchDog.h"
#include "ProcessWatchDogDlg.h"
#include "ProcessList.h"
#include "WatchdogDlg.h"
#include "Watchdog.h"
#include "ProcessList.h"
#include "Tray.h"
#include "Registry.h"
#include "RegistryKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define APP_DATA_FOLDER		(TEXT("Avalanche Software\\ProcessWatchdog"))

static int watchdogListHeaderWidths[] = { 24, 24, 12 };	// sixty-fourths

static TCHAR *	watchdogListHeaders[] = { 
	_T("Name"),
	_T("Status"),
	_T("Num Processes"),
	NULL };

static int watchdogListAlignment[] = { 
	LVCFMT_LEFT,
	LVCFMT_CENTER,
	LVCFMT_CENTER};

static int processListHeaderWidths[] = { 18, 6, 8, 20 };	// sixty-fourths

static TCHAR *	processListHeaders[] = { 
	_T("Image Name"),
		_T("PID"),
		_T("Priority"),
		_T("Watchdog Status"),
		NULL };

static int processListAlignment[] = { 
	LVCFMT_LEFT,
	LVCFMT_CENTER,
	LVCFMT_CENTER,
	LVCFMT_CENTER};

// CAboutDlg dialog used for App About

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
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CProcessWatchDogDlg dialog



CProcessWatchDogDlg::CProcessWatchDogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessWatchDogDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pProcessManager = new ProcessManager;
	m_pWatchdogManager = new WatchdogManager(*this,*m_pProcessManager);

	Create (IDD, pParent); // Create modeless dialog. 
}

CProcessWatchDogDlg::~CProcessWatchDogDlg()
{
	delete m_pWatchdogManager;
	delete m_pProcessManager;
}

void CProcessWatchDogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESS_LIST, m_processList);
	DDX_Control(pDX, IDC_WATCHDOG_LIST, m_watchdogList);
	DDX_Control(pDX, IDC_WATCHDOG_TITLE, m_watchdogTitle);
	DDX_Control(pDX, IDC_PROCESS_TITLE, m_processTitle);
	DDX_Control(pDX, IDC_LOG_WINDOW, m_logWindow);
	DDX_Control(pDX, IDC_LOG_WINDOW_TITLE, m_logWindowTitle);
}

BEGIN_MESSAGE_MAP(CProcessWatchDogDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_TRAY_NOTIFY, OnTrayNotify)
	ON_NOTIFY(NM_RCLICK, IDC_PROCESS_LIST, OnNMRclickProcessList)
	ON_COMMAND(ID_WATCHDOG_NEW, OnWatchdogNew)
	ON_UPDATE_COMMAND_UI(ID_WATCHDOG_NEW, OnUpdateWatchdogNew)
	ON_COMMAND(ID_WATCHDOG_EDIT, OnWatchdogEdit)
	ON_UPDATE_COMMAND_UI(ID_WATCHDOG_EDIT, OnUpdateWatchdogEdit)
	ON_COMMAND(ID_WATCHDOG_DELETE, OnWatchdogDelete)
	ON_UPDATE_COMMAND_UI(ID_WATCHDOG_DELETE, OnUpdateWatchdogDelete)
	ON_NOTIFY(NM_RCLICK, IDC_WATCHDOG_LIST, OnNMRclickWatchdogList)
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_WM_CLOSE()
	ON_COMMAND(ID_RESTORE, OnRestore)
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
END_MESSAGE_MAP()


// CProcessWatchDogDlg message handlers

BOOL CProcessWatchDogDlg::OnInitDialog()
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

	m_bLoadDefaultFile = true;

	Registry registry(PROCESS_WATCHDOG_KEY);
	DWORD regInt;

	if (registry.Get(LOAD_DEFAULT_FILE,&regInt))
	{
		if (regInt)
			m_bLoadDefaultFile = true;
		else
			m_bLoadDefaultFile = false;
	}

	const char *pWatchdogFile;
	if (m_bLoadDefaultFile)
		pWatchdogFile = GetDefaultDataFilePath();
	else
		pWatchdogFile = GetClientDataFilePath();

	m_pWatchdogManager->DisableAddThreads();
	m_pWatchdogManager->PurgeWatchdogProcessThreads(true);
	m_pWatchdogManager->Load(pWatchdogFile);
	m_pWatchdogManager->Unlock();
	m_pWatchdogManager->EnableAddThreads();

	SetTimer(100,1000,NULL); // for refresh

	InitProcessListCtrl();
	InitWatchdogListCtrl();

	FillProcessList();
	FillWatchdogList();


	CRect dialogRect;
	dialogRect.SetRectEmpty();
	if (registry.Get(DIALOG_RECT_TOP_FIELD,&regInt))
		dialogRect.top = (LONG)regInt;
	if (registry.Get(DIALOG_RECT_BOTTOM_FIELD,&regInt))
		dialogRect.bottom = (LONG)regInt;
	if (registry.Get(DIALOG_RECT_LEFT_FIELD,&regInt))
		dialogRect.left = (LONG)regInt;
	if (registry.Get(DIALOG_RECT_RIGHT_FIELD,&regInt))
		dialogRect.right = (LONG)regInt;

	// move window to last saved position
	CRect desktopRect;
	desktopRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	desktopRect.bottom = desktopRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
	desktopRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	desktopRect.right = desktopRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);

	if (dialogRect.IsRectEmpty())
	{
		GetWindowRect(&dialogRect);
	}
	// make sure the window is visible on the screen
	if (dialogRect.top<desktopRect.top)
	{
		dialogRect.bottom = desktopRect.top+dialogRect.Height();
		dialogRect.top = desktopRect.top;
	}
	if (dialogRect.right<desktopRect.left+554)
	{
		dialogRect.right = desktopRect.left+dialogRect.Width();
		dialogRect.left = desktopRect.left;
	}
	if (dialogRect.top>desktopRect.bottom-419)
	{
		dialogRect.top = desktopRect.bottom-dialogRect.Height();
		dialogRect.bottom = desktopRect.bottom;
	}
	if (dialogRect.left>desktopRect.right-554)
	{
		dialogRect.left = desktopRect.right-dialogRect.Width();
		dialogRect.right = desktopRect.right;
	}
	MoveWindow(&dialogRect);


	m_pTray = NULL ;
	m_bHidden = FALSE;
	OnHide();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessWatchDogDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

BOOL CProcessWatchDogDlg::PreTranslateMessage(MSG* pMsg) 
{
	BOOL ret = CDialog::PreTranslateMessage(pMsg);
	//
	// Addition to make OnUpdate........(CCmdUI* pCmdUI) work in dialogs
	//
	CMenu* pMainMenu = GetMenu();
	CCmdUI cmdUI; 
	for (UINT n = 0; n < pMainMenu->GetMenuItemCount(); ++n) 
	{
		CMenu* pSubMenu = pMainMenu->GetSubMenu(n);
		cmdUI.m_nIndexMax = pSubMenu->GetMenuItemCount();
		for (UINT i = 0; i < cmdUI.m_nIndexMax;++i)  
		{   
			cmdUI.m_nIndex = i;
			cmdUI.m_nID = pSubMenu->GetMenuItemID(i);   
			cmdUI.m_pMenu = pSubMenu;
			cmdUI.DoUpdate(this, FALSE);  
		}
	}  return ret;
}


void CProcessWatchDogDlg::OnDestroy()
{
	SaveSettings();

	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();

	// Remove Icon from Tray
	if (m_pTray)
	{
		delete m_pTray ;
		m_pTray = NULL ;
	}	
}

void CProcessWatchDogDlg::SaveSettings(void)
{
	const char *pWatchdogFile = GetClientDataFilePath();
	m_pWatchdogManager->Lock();
	m_pWatchdogManager->Save(pWatchdogFile);
	m_pWatchdogManager->Unlock();

	Registry registry(PROCESS_WATCHDOG_KEY);
	CRect dialogRect;
	GetWindowRect(&dialogRect);
	registry.Set(DIALOG_RECT_TOP_FIELD,(DWORD)dialogRect.top);
	registry.Set(DIALOG_RECT_BOTTOM_FIELD,(DWORD)dialogRect.bottom);
	registry.Set(DIALOG_RECT_LEFT_FIELD,(DWORD)dialogRect.left);
	registry.Set(DIALOG_RECT_RIGHT_FIELD,(DWORD)dialogRect.right);

	registry.Set(LOAD_DEFAULT_FILE,(DWORD) m_bLoadDefaultFile ? 1 : 0);

}

void CProcessWatchDogDlg::PostNcDestroy() 
{
	delete this;
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessWatchDogDlg::OnPaint() 
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessWatchDogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProcessWatchDogDlg::InitWatchdogListCtrl(void)
{
	// set column width according to window rect
	CRect rect;
	m_watchdogList.GetWindowRect(&rect);

	int w = rect.Width() - 19;

	int i;
	int total_cx = 0;
	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	// add columns
	for (i = 0; ; i++)
	{
		if (watchdogListHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = watchdogListAlignment[i];
		lvcolumn.pszText = watchdogListHeaders[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = (watchdogListHeaders[i+1] == NULL) ? w - total_cx - 2 : (w * watchdogListHeaderWidths[i]) / 64;
		total_cx += lvcolumn.cx;
		m_watchdogList.InsertColumn(i, &lvcolumn);
	}

	memset(&lvcolumn, 0, sizeof(lvcolumn));

	// set the format again - must do this twice or first column does not get set
	for (i = 0; ; i++)
	{
		if (watchdogListHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM;
		lvcolumn.fmt = watchdogListAlignment[i];
		lvcolumn.iSubItem = i;
		m_watchdogList.SetColumn(i, &lvcolumn);
	}
}

void CProcessWatchDogDlg::InitProcessListCtrl(void)
{
	// set column width according to window rect
	CRect rect;
	m_processList.GetWindowRect(&rect);

	int w = rect.Width() - 19;

	int i;
	int total_cx = 0;
	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	// add columns
	for (i = 0; ; i++)
	{
		if (processListHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = processListAlignment[i];
		lvcolumn.pszText = processListHeaders[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = (processListHeaders[i+1] == NULL) ? w - total_cx - 2 : (w * processListHeaderWidths[i]) / 64;
		total_cx += lvcolumn.cx;
		m_processList.InsertColumn(i, &lvcolumn);
	}

	memset(&lvcolumn, 0, sizeof(lvcolumn));

	// set the format again - must do this twice or first column does not get set
	for (i = 0; ; i++)
	{
		if (processListHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM;
		lvcolumn.fmt = processListAlignment[i];
		lvcolumn.iSubItem = i;
		m_processList.SetColumn(i, &lvcolumn);
	}
}


void CProcessWatchDogDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);


	int watchDogProcessDividerY = (int)((float)rect.Height()*.2429);
	int processLogDividerY = (int)((float)rect.Height()*.7495);

	const int watchdogListRightGap = 10;
	const int watchdogListBottomGap = 0;

	const int processTitleTopGap = 5;

	const int processListRightGap = 10;
	const int processListTopGap = 21;
	const int processListBottomGap = 0;

	const int logWindowTitleTopGap = 4;

	const int logWindowTopGap = 18;
	const int logWindowRightGap = 10;
	const int logWindowBottomGap = 12;

	// watchdog list
	if (::IsWindow(m_watchdogList.m_hWnd))
	{
		CRect myRect;
		m_watchdogList.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_watchdogList.MoveWindow(myRect.left, myRect.top,rect.right - (myRect.left + watchdogListRightGap), (watchDogProcessDividerY-watchdogListBottomGap)-myRect.top);
	}

	// process title
	if (::IsWindow(m_processTitle.m_hWnd))
	{
		CRect myRect;
		m_processTitle.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_processTitle.MoveWindow(myRect.left, watchDogProcessDividerY + processTitleTopGap,myRect.Width(), myRect.Height());

		int w = ((rect.right - processListRightGap)-myRect.left) - 19;

		int i;
		int total_cx = 0;
		LV_COLUMN lvcolumn;
		memset(&lvcolumn, 0, sizeof(lvcolumn));

		for (i = 0; ; i++)
		{
			if (watchdogListHeaders[i] == NULL)
				break;

			lvcolumn.mask = LVCF_WIDTH;
			lvcolumn.iSubItem = i;
			lvcolumn.cx = (watchdogListHeaders[i+1] == NULL) ? w - total_cx - 2 : (w * watchdogListHeaderWidths[i]) / 64;
			total_cx += lvcolumn.cx;
			m_watchdogList.SetColumn(i, &lvcolumn);
		}
	}

	// process list
	if (::IsWindow(m_processList.m_hWnd))
	{
		CRect myRect;
		m_processList.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_processList.SetRedraw(FALSE);

		m_processList.MoveWindow(myRect.left, watchDogProcessDividerY + processListTopGap,(rect.right - processListRightGap)-myRect.left, (processLogDividerY-processListBottomGap)-(watchDogProcessDividerY + processListTopGap));

		int w = ((rect.right - processListRightGap)-myRect.left) - 19;

		int i;
		int total_cx = 0;
		LV_COLUMN lvcolumn;
		memset(&lvcolumn, 0, sizeof(lvcolumn));

		for (i = 0; ; i++)
		{
			if (processListHeaders[i] == NULL)
				break;

			lvcolumn.mask = LVCF_WIDTH;
			lvcolumn.iSubItem = i;
			lvcolumn.cx = (processListHeaders[i+1] == NULL) ? w - total_cx - 2 : (w * processListHeaderWidths[i]) / 64;
			total_cx += lvcolumn.cx;
			m_processList.SetColumn(i, &lvcolumn);
		}
		m_processList.SetRedraw(TRUE);
	}

	// log window title
	if (::IsWindow(m_logWindowTitle.m_hWnd))
	{
		CRect myRect;
		m_logWindowTitle.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_logWindowTitle.MoveWindow(myRect.left, processLogDividerY + logWindowTitleTopGap,myRect.right, myRect.Height());
	}

	// log window
	if (::IsWindow(m_logWindow.m_hWnd))
	{
		CRect myRect;
		m_logWindow.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_logWindow.MoveWindow(myRect.left, processLogDividerY + logWindowTopGap,(rect.right - logWindowRightGap)-myRect.left, (rect.bottom - logWindowBottomGap)-(processLogDividerY + logWindowTopGap));
	}

}

void CProcessWatchDogDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 400;
}

void CProcessWatchDogDlg::Refresh(void)
{
	FillProcessList();
	FillWatchdogList();
	RefreshLog();
}

void CProcessWatchDogDlg::PostRefresh(void)
{
	if (::IsWindow(m_hWnd))
		PostMessage(WM_TIMER,100);
}

void CProcessWatchDogDlg::FillProcessList(void)
{
	m_pWatchdogManager->Lock();
	m_pProcessManager->Lock();

	if (!m_pProcessManager->CompareToOld())
	{
		m_pProcessManager->UpdateOld();

		m_processList.SetRedraw(FALSE);

		int processSel = m_processList.GetCurSel();
		int processScroll = m_processList.GetTopIndex();

		m_processList.DeleteAllItems();

		int numProcesses = m_pProcessManager->GetNumProcesses();
		for (int processIndex=0;processIndex<numProcesses;processIndex++)
		{
			const ProcessManager::ProcessData &process = m_pProcessManager->GetProcess(processIndex);
			TupString processName = "  ";
			processName += process.m_processName;
			int listIndex = m_processList.InsertItem(processIndex,processName);
			m_processList.SetItemData(listIndex,processIndex);

			char IDStr[10];
			itoa(process.m_processID,IDStr,10);
			TupString idName = IDStr;
			idName+="  ";
			m_processList.SetItemText(listIndex,1,idName);

			TupString priority;
			switch(process.m_priorityClassBase)
			{
				case REALTIME_PRIORITY_CLASS :
					priority = "Realtime";
					break;
				case HIGH_PRIORITY_CLASS :
					priority = "High";
					break;
				case ABOVE_NORMAL_PRIORITY_CLASS:
					priority = "AboveNormal";
					break;
				case NORMAL_PRIORITY_CLASS:
					priority = "Normal";
					break;
				case BELOW_NORMAL_PRIORITY_CLASS:
					priority = "BelowNormal";
					break;
				case IDLE_PRIORITY_CLASS:
					priority = "Low";
					break;
				default:
					priority = "Normal";
					break;
			}
			m_processList.SetItemText(listIndex,2,priority);

			TupString statusString = "-";

			switch (process.m_watchdogProcessStatus)
			{
				case Watchdog::Process::PROCESS_STATUS_ACTIVE:
					statusString = "Active";
					break;
				case Watchdog::Process::PROCESS_STATUS_CONNECTING:
					statusString = "Connecting";
					break;
				case Watchdog::Process::PROCESS_STATUS_NOT_RESPONDING:
					statusString = "Not Responding";
					break;
				case Watchdog::Process::PROCESS_STATUS_WAITING_AFTER_CREATION:
					statusString = "Waiting after Creation";
					break;
			}
			m_processList.SetItemText(listIndex,3,statusString);
		}

		m_processList.SetCurSel(processSel);
		m_processList.EnsureVisible(processScroll,FALSE);
		m_processList.EnsureVisible(processScroll+m_processList.GetCountPerPage()-1,FALSE);
		m_processList.Invalidate(FALSE);
		m_processList.SetRedraw(TRUE);

	}
	m_pProcessManager->Unlock();
	m_pWatchdogManager->Unlock();

}

void CProcessWatchDogDlg::FillWatchdogList(void)
{
	m_watchdogList.SetRedraw(FALSE);

	int watchdogSel = m_watchdogList.GetCurSel();
	int watchdogScroll = m_watchdogList.GetTopIndex();

	m_pWatchdogManager->Lock();

	m_watchdogList.DeleteAllItems();

	int numWatchdogs = m_pWatchdogManager->GetNumWatchdogs();
	for (int watchdogIndex=0;watchdogIndex<numWatchdogs;watchdogIndex++)
	{
		const Watchdog &watchdog = m_pWatchdogManager->GetWatchdog(watchdogIndex);
		TupString name = "  ";
		name += watchdog.m_name;
		m_watchdogList.InsertItem(watchdogIndex,name);

		int numProcesses = watchdog.m_processes.GetSize();
		int numNotResponding = 0;
		for (int processIndex=0;processIndex<numProcesses;processIndex++)
		{
			if (watchdog.m_processes[processIndex].m_status==Watchdog::Process::PROCESS_STATUS_NOT_RESPONDING)
				numNotResponding++;
		}
		CString statusString = "Ok";
		if (numNotResponding)
		{
			statusString.Format("%d Not Responding",numNotResponding);
		}
		m_watchdogList.SetItemText(watchdogIndex,1,statusString);

		CString numProcessesString = "None";
		if (numProcesses)
		{
			numProcessesString.Format("%d",numProcesses);
		}
		m_watchdogList.SetItemText(watchdogIndex,2,numProcessesString);

	}

	m_pWatchdogManager->Unlock();

	m_watchdogList.SetCurSel(watchdogSel);
	m_watchdogList.EnsureVisible(watchdogScroll,FALSE);
	m_watchdogList.EnsureVisible(watchdogScroll+m_watchdogList.GetCountPerPage()-1,FALSE);
	m_watchdogList.Invalidate(FALSE);
	m_watchdogList.SetRedraw(TRUE);

}

void CProcessWatchDogDlg::OnNMRclickProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	int processSel = m_processList.GetCurSel();
	if (processSel>=0)
	{
		CString name = m_processList.GetItemText(processSel,0);
		name.TrimLeft();
		int watchdogIndex = m_pWatchdogManager->FindWatchdogByName(name);
		if (watchdogIndex!=-1)
		{
			m_menuFrom = MENU_FROM_WATCHDOGS;
			m_listSelectionIndex = watchdogIndex;
		}
		else
		{
			m_menuFrom = MENU_FROM_PROCESSES;
			m_listSelectionIndex = m_processList.GetItemData(processSel);

		}
	}
	else
	{
		m_menuFrom = MENU_FROM_TOP;
	}

	DWORD	dwPos;
	CPoint ptClick;

	dwPos = ::GetMessagePos();

	ptClick.x = LOWORD (dwPos);
	ptClick.y = HIWORD (dwPos);

	UINT menuFlags;

	CMenu menu;
	menu.CreatePopupMenu();

	menuFlags = (m_menuFrom!=MENU_FROM_WATCHDOGS) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_WATCHDOG_NEW, "New Watchdog");
	menuFlags = (m_menuFrom==MENU_FROM_WATCHDOGS) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_WATCHDOG_EDIT, "Edit Watchdog");
	menuFlags = (m_menuFrom==MENU_FROM_WATCHDOGS) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_WATCHDOG_DELETE, "Delete Watchdog");

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		ptClick.x, ptClick.y, this);

	*pResult = 0;
}

void CProcessWatchDogDlg::OnNMRclickWatchdogList(NMHDR *pNMHDR, LRESULT *pResult)
{
	int watchdogSel = m_watchdogList.GetCurSel();
	if (watchdogSel>=0)
	{
		m_menuFrom = MENU_FROM_WATCHDOGS;
		m_listSelectionIndex = watchdogSel;
	}
	else
	{
		m_menuFrom = MENU_FROM_TOP;
	}

	DWORD	dwPos;
	CPoint ptClick;

	dwPos = ::GetMessagePos();

	ptClick.x = LOWORD (dwPos);
	ptClick.y = HIWORD (dwPos);

	UINT menuFlags;

	CMenu menu;
	menu.CreatePopupMenu();

	menuFlags = (m_menuFrom!=MENU_FROM_WATCHDOGS) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_WATCHDOG_NEW, "New Watchdog");
	menuFlags = (m_menuFrom==MENU_FROM_WATCHDOGS) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_WATCHDOG_EDIT, "Edit Watchdog");
	menuFlags = (m_menuFrom==MENU_FROM_WATCHDOGS) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_WATCHDOG_DELETE, "Delete Watchdog");

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		ptClick.x, ptClick.y, this);

	*pResult = 0;
}

void CProcessWatchDogDlg::OnWatchdogNew()
{
	CWatchdogSheet sheet(_T("New Watchdog"), this);

	CGeneralPage generalPage;
	CActiveProcessPage activeProcessPage;
	CNoProcessesPage noProcessesPage;
	CNotRepondingPage notRespondingPage;
	CWatchFilePage watchFilePage;

	if (m_menuFrom==MENU_FROM_PROCESSES)
	{
		m_pProcessManager->Lock();
		const ProcessManager::ProcessData &process = m_pProcessManager->GetProcess(m_listSelectionIndex);
		generalPage.m_name = process.m_processName;
		m_pProcessManager->Unlock();
	}
	else
	{
		generalPage.m_name = "";
	}

	generalPage.m_checkInterval = 2.0f;
	generalPage.m_waitForResponse = 5.0f;
	generalPage.m_waitAfterCreation = 30.0f;
	generalPage.m_waitForNoProcesses = 5.0f;

	activeProcessPage.m_setting = 0;
	activeProcessPage.m_priority = 3;

	noProcessesPage.m_setting = 0;
	noProcessesPage.m_targetPath = "";
	noProcessesPage.m_startInPath = "";
	noProcessesPage.m_parameters = "";
	noProcessesPage.m_runType = 0;

	notRespondingPage.m_setting = 0;

	watchFilePage.m_setting = 0;
	watchFilePage.m_watchFilePath = "";

	sheet.AddPage(&generalPage);
	sheet.AddPage(&activeProcessPage);
	sheet.AddPage(&noProcessesPage);
	sheet.AddPage(&notRespondingPage);
	sheet.AddPage(&watchFilePage);

	bool bAddWatchdog = false;
	while (1)
	{
		if (sheet.DoModal() == IDOK)
		{
			m_pWatchdogManager->Lock();
			int foundWatchdogIndex = m_pWatchdogManager->FindWatchdogByName(generalPage.m_name);
			m_pWatchdogManager->Unlock();

			// if one already exists with that name
			if (foundWatchdogIndex!=-1)
			{
				AfxMessageBox("Watchdog with that name already exists...\nTry another");
			}
			else if (generalPage.m_name.IsEmpty())
			{
				AfxMessageBox("Watchdog must have a name");
			}
			else
			{
				bAddWatchdog = true;
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (bAddWatchdog)
	{
		Watchdog newWatchdog;
		newWatchdog.m_name = generalPage.m_name;
		newWatchdog.m_checkInterval = generalPage.m_checkInterval;
		newWatchdog.m_waitForResponse = generalPage.m_waitForResponse;
		newWatchdog.m_waitAfterCreation = generalPage.m_waitAfterCreation;
		newWatchdog.m_waitForNoProcesses = generalPage.m_waitForNoProcesses;
		SYSTEMTIME currentSystemTime;
		::GetSystemTime(&currentSystemTime);		// gets current time
		::SystemTimeToFileTime(&currentSystemTime,&newWatchdog.m_noProcessTime);	// converts to file time format

		newWatchdog.m_flags = 0;

		switch (activeProcessPage.m_setting)
		{
			case 0:
				newWatchdog.m_flags |= Watchdog::ACTIVE_STATE_IGNORE;
				break;
			case 1:
				newWatchdog.m_flags |= Watchdog::ACTIVE_STATE_KILL;
				break;
			case 2:
				newWatchdog.m_flags |= Watchdog::ACTIVE_STATE_SET_PRIORITY;
				break;
		}
		newWatchdog.m_priority = activeProcessPage.m_priority;

		switch (noProcessesPage.m_setting)
		{
			case 0:
				newWatchdog.m_flags |= Watchdog::NO_PROCESSES_STATE_IGNORE;
				break;
			case 1:
				newWatchdog.m_flags |= Watchdog::NO_PROCESSES_STATE_OPEN;
				break;
		}

		newWatchdog.m_targetPath = noProcessesPage.m_targetPath;
		newWatchdog.m_startInPath = noProcessesPage.m_startInPath;
		newWatchdog.m_parameters = noProcessesPage.m_parameters;
		newWatchdog.m_runType = noProcessesPage.m_runType;

		switch (notRespondingPage.m_setting)
		{
			case 0:
				newWatchdog.m_flags |= Watchdog::NOT_RESPONDING_STATE_IGNORE;
				break;
			case 1:
				newWatchdog.m_flags |= Watchdog::NOT_RESPONDING_STATE_KILL;
				break;
		}

		switch (watchFilePage.m_setting)
		{
			case 0:
				newWatchdog.m_flags |= Watchdog::WATCH_FILE_IGNORE;
				break;
			case 1:
				newWatchdog.m_flags |= Watchdog::WATCH_FILE_KILL;
				break;
		}

		newWatchdog.m_watchFilePath = watchFilePage.m_watchFilePath;

		m_pWatchdogManager->Lock();
		m_pWatchdogManager->AddWatchdog(newWatchdog);
		m_pWatchdogManager->Unlock();
		Refresh();
	}
}

void CProcessWatchDogDlg::OnUpdateWatchdogNew(CCmdUI *pCmdUI)
{
	int watchdogSel = m_watchdogList.GetCurSel();
	if (watchdogSel>=0)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int processSel = m_processList.GetCurSel();
	if (processSel>=0)
	{
		pCmdUI->Enable(TRUE);
		m_menuFrom = MENU_FROM_PROCESSES;
		m_listSelectionIndex = m_processList.GetItemData(processSel);
		return;
	}

	pCmdUI->Enable(TRUE);
	m_menuFrom = MENU_FROM_TOP;
}

void CProcessWatchDogDlg::OnWatchdogEdit()
{
	CWatchdogSheet sheet(_T("Edit Watchdog"), this);

	CGeneralPage generalPage;
	CActiveProcessPage activeProcessPage;
	CNoProcessesPage noProcessesPage;
	CNotRepondingPage notRespondingPage;
	CWatchFilePage watchFilePage;

	m_pWatchdogManager->Lock();
	const Watchdog &watchdog = m_pWatchdogManager->GetWatchdog(m_listSelectionIndex);

	generalPage.m_name = watchdog.m_name;
	generalPage.m_checkInterval = watchdog.m_checkInterval;
	generalPage.m_waitForResponse = watchdog.m_waitForResponse;
	generalPage.m_waitAfterCreation = watchdog.m_waitAfterCreation;
	generalPage.m_waitForNoProcesses = watchdog.m_waitForNoProcesses;
	int flags = watchdog.m_flags;

	switch (flags & Watchdog::ACTIVE_STATE_MASK)
	{
		case Watchdog::ACTIVE_STATE_IGNORE:
			activeProcessPage.m_setting = 0;
			break;
		case Watchdog::ACTIVE_STATE_KILL:
			activeProcessPage.m_setting = 1;
			break;
		case Watchdog::ACTIVE_STATE_SET_PRIORITY:
			activeProcessPage.m_setting = 2;
			break;
	}
	activeProcessPage.m_priority = watchdog.m_priority;

	switch (flags & Watchdog::NO_PROCESSES_STATE_MASK)
	{
		case Watchdog::NO_PROCESSES_STATE_IGNORE:
			noProcessesPage.m_setting = 0;
			break;
		case Watchdog::NO_PROCESSES_STATE_OPEN:
			noProcessesPage.m_setting = 1;
			break;
	}

	noProcessesPage.m_targetPath = watchdog.m_targetPath;
	noProcessesPage.m_startInPath = watchdog.m_startInPath;
	noProcessesPage.m_parameters = watchdog.m_parameters;
	noProcessesPage.m_runType = watchdog.m_runType;

	switch (flags & Watchdog::NOT_RESPONDING_STATE_MASK)
	{
		case Watchdog::NOT_RESPONDING_STATE_IGNORE:
			notRespondingPage.m_setting = 0;
			break;
		case Watchdog::NOT_RESPONDING_STATE_KILL:
			notRespondingPage.m_setting = 1;
			break;
	}

	switch (flags & Watchdog::WATCH_FILE_MASK)
	{
		case Watchdog::WATCH_FILE_IGNORE:
			watchFilePage.m_setting = 0;
			break;
		case Watchdog::WATCH_FILE_KILL:
			watchFilePage.m_setting = 1;
			break;
	}

	watchFilePage.m_watchFilePath = watchdog.m_watchFilePath;

	sheet.AddPage(&generalPage);
	sheet.AddPage(&activeProcessPage);
	sheet.AddPage(&noProcessesPage);
	sheet.AddPage(&notRespondingPage);
	sheet.AddPage(&watchFilePage);

	m_pWatchdogManager->Unlock();

	bool bEditWatchdog = false;
	while (1)
	{
		if (sheet.DoModal() == IDOK)
		{
			bool bWatchdogNameConflict = false;
			m_pWatchdogManager->Lock();
			Watchdog &watchdog = m_pWatchdogManager->GetWatchdog(m_listSelectionIndex);
			// if the name is changing see if new name is on another watchdog
			if (stricmp(watchdog.m_name,generalPage.m_name)!=0)
			{
				TupString oldName = watchdog.m_name;
				watchdog.m_name = "";
				if (m_pWatchdogManager->FindWatchdogByName(generalPage.m_name)!=-1)
					bWatchdogNameConflict = true;
				watchdog.m_name = oldName; // put back
			}
			m_pWatchdogManager->Unlock();

			if (bWatchdogNameConflict)
			{
				AfxMessageBox("Watchdog with that name already exists...\nTry another");
			}
			else
			{
				bEditWatchdog = true;
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (bEditWatchdog)
	{
		m_pWatchdogManager->Lock();
		Watchdog &watchdog = m_pWatchdogManager->GetWatchdog(m_listSelectionIndex);
		watchdog.m_name = generalPage.m_name;
		watchdog.m_checkInterval = generalPage.m_checkInterval;
		watchdog.m_waitForResponse = generalPage.m_waitForResponse;
		watchdog.m_waitAfterCreation = generalPage.m_waitAfterCreation;
		watchdog.m_waitForNoProcesses = generalPage.m_waitForNoProcesses;
		SYSTEMTIME currentSystemTime;
		::GetSystemTime(&currentSystemTime);		// gets current time
		::SystemTimeToFileTime(&currentSystemTime,&watchdog.m_noProcessTime);	// converts to file time format
		watchdog.m_flags = 0;

		switch (activeProcessPage.m_setting)
		{
			case 0:
				watchdog.m_flags |= Watchdog::ACTIVE_STATE_IGNORE;
				break;
			case 1:
				watchdog.m_flags |= Watchdog::ACTIVE_STATE_KILL;
				break;
			case 2:
				watchdog.m_flags |= Watchdog::ACTIVE_STATE_SET_PRIORITY;
				break;
		}
		watchdog.m_priority = activeProcessPage.m_priority;

		switch (noProcessesPage.m_setting)
		{
			case 0:
				watchdog.m_flags |= Watchdog::NO_PROCESSES_STATE_IGNORE;
				break;
			case 1:
				watchdog.m_flags |= Watchdog::NO_PROCESSES_STATE_OPEN;
				break;
		}

		watchdog.m_targetPath = noProcessesPage.m_targetPath;
		watchdog.m_startInPath = noProcessesPage.m_startInPath;
		watchdog.m_parameters = noProcessesPage.m_parameters;
		watchdog.m_runType = noProcessesPage.m_runType;

		switch (notRespondingPage.m_setting)
		{
			case 0:
				watchdog.m_flags |= Watchdog::NOT_RESPONDING_STATE_IGNORE;
				break;
			case 1:
				watchdog.m_flags |= Watchdog::NOT_RESPONDING_STATE_KILL;
				break;
		}

		switch (watchFilePage.m_setting)
		{
			case 0:
				watchdog.m_flags |= Watchdog::WATCH_FILE_IGNORE;
				break;
			case 1:
				watchdog.m_flags |= Watchdog::WATCH_FILE_KILL;
				break;
		}

		watchdog.m_watchFilePath = watchFilePage.m_watchFilePath;
		m_pWatchdogManager->Unlock();
		Refresh();
	}
}

void CProcessWatchDogDlg::OnUpdateWatchdogEdit(CCmdUI *pCmdUI)
{
	int watchdogSel = m_watchdogList.GetCurSel();
	if (watchdogSel>=0)
	{
		pCmdUI->Enable(TRUE);
		m_menuFrom = MENU_FROM_WATCHDOGS;
		m_listSelectionIndex = watchdogSel;
		return;
	}

	int processSel = m_processList.GetCurSel();
	if (processSel>=0)
	{
		CString processName = m_processList.GetItemText(processSel,0);
		processName.TrimLeft();
		watchdogSel = m_pWatchdogManager->FindWatchdogByName(processName);
		if (watchdogSel!=-1)
		{
			pCmdUI->Enable(TRUE);
			m_menuFrom = MENU_FROM_WATCHDOGS;
			m_listSelectionIndex = watchdogSel;
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CProcessWatchDogDlg::OnWatchdogDelete()
{
	m_pWatchdogManager->DisableAddThreads();
	while(1)
	{
		bool bProcessesPurged = true;
		m_pWatchdogManager->Lock();
		Watchdog &watchdog = m_pWatchdogManager->GetWatchdog(m_listSelectionIndex);
		int numProcesses = watchdog.m_processes.GetSize();
		for (int processIndex=0;processIndex<numProcesses;processIndex++)
		{
			watchdog.m_processes[processIndex].m_bEndThread = true;
			bProcessesPurged = false;
		}
		if (bProcessesPurged)
		{
			m_pWatchdogManager->DeleteWatchdog(m_listSelectionIndex);
			m_pWatchdogManager->Unlock();
			break;
		}
		else
		{
			m_pWatchdogManager->Unlock();
			Sleep(1000);
		}
	}
	m_pWatchdogManager->EnableAddThreads();
	Refresh();
}

void CProcessWatchDogDlg::OnUpdateWatchdogDelete(CCmdUI *pCmdUI)
{
	int watchdogSel = m_watchdogList.GetCurSel();
	if (watchdogSel>=0)
	{
		pCmdUI->Enable(TRUE);
		m_menuFrom = MENU_FROM_WATCHDOGS;
		m_listSelectionIndex = watchdogSel;
		return;
	}

	int processSel = m_processList.GetCurSel();
	if (processSel>=0)
	{
		CString processName = m_processList.GetItemText(processSel,0);
		processName.TrimLeft();
		watchdogSel = m_pWatchdogManager->FindWatchdogByName(processName);
		if (watchdogSel!=-1)
		{
			pCmdUI->Enable(TRUE);
			m_menuFrom = MENU_FROM_WATCHDOGS;
			m_listSelectionIndex = watchdogSel;
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}


void CProcessWatchDogDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==100)
		Refresh();
	CDialog::OnTimer(nIDEvent);
}

const char *CProcessWatchDogDlg::GetClientDataFilePath(void)
{
	static TupString pathString = "";

	TCHAR path[MAX_PATH];
	HRESULT hResult = SHGetFolderPath(NULL,                //hWnd	
		CSIDL_APPDATA | CSIDL_FLAG_CREATE,       //csidl
		NULL,                //hToken
		SHGFP_TYPE_CURRENT,  //dwFlags
		path);             //pszPath
	if (SUCCEEDED(hResult))
	{
		PathAppend(path, APP_DATA_FOLDER);
	}
	else
	{
		strcpy(path,"c:\\"); // just use c:
		PathAppend(path, APP_DATA_FOLDER);
	}

	PathAppend(path, TEXT("watchdogs.cfg"));
	pathString = path;
	CreateDirs(pathString,TRUE);

	return pathString;
}

const char *CProcessWatchDogDlg::GetDefaultDataFilePath(void)
{
	static TupString pathString = "";

	char pathBuffer[_MAX_PATH];

	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];

	::GetModuleFileName(NULL,pathBuffer,_MAX_PATH);

	_splitpath(pathBuffer,fdrive,fdir,NULL,NULL);
	_makepath(pathBuffer,fdrive,fdir,"default","pwd");
	pathString = pathBuffer;
	return pathString;
}

bool CProcessWatchDogDlg::DirectoryExists(const char *pPath)
{
	DWORD fileAttributes = ::GetFileAttributes(pPath);
	return (fileAttributes != INVALID_FILE_ATTRIBUTES);
}

//
//  CreateDirs()
//  DESC: Creates each directory in a given path. 
//  ARGS: sPath - The fully qualified path whose directories we will create.
//           As each portion of the the path is parsed out, if it doesn't
//           exists, it will be created by CreateDirectory().  
//        bIsFilePath - If TRUE, CreateDirs() assumes the last token in the
//           path is a filename, and skips it.
bool CProcessWatchDogDlg::CreateDirs (const char *pPath, bool bIsFilePath /*= false*/)
{
	char outpath[MAX_PATH] = "";      // we build this during the parsing of the source path
	char inpath[MAX_PATH];           // create local copy of source path (we destructively parse it)  
	char *p = strcpy (inpath, pPath); 
	char *slash = NULL;

	// store the drive portion of path and skip past it
	if (!((slash = strchr (p, '\\')) || (slash = strchr (p, '/'))))
		return false;
	*slash = 0;           // terminate p at first slash so we can...
	strcpy (outpath, p);  // copy drive letter and : to outpath
	p = slash + 1;        // advance p to next token in path

	ASSERT(outpath[1] == ':'); // so far, so good

	// parse out slash delimited tokens, 
	// creating each part of the directory path as we go
	do 
	{
		if ((slash = strchr (p, '\\')) || (slash = strchr (p, '/')))     
			*slash = 0;                         // terminate so we can call strcat() with p
		else if (bIsFilePath)                   // skip last token if the path is to a file
			return true;
		strcat (outpath, "\\");                 // add token to the output path path...
		strcat (outpath, p);                    // ...
		if (!DirectoryExists (outpath) &&       // if path doesn't already exists
			!CreateDirectory (outpath, NULL))   // try to create it
			return false;                       // something went wrong
		p = slash + 1;                          // advance p to next token in path
	}
	while (slash);

	return true;
}

void CProcessWatchDogDlg::OnFileLoad()
{
	static char szFilter[] = "Process Watchdog Files|*.pwd|"  \
		"All Files (*.*)|*.*||";

	if (!m_watchdogFilePath.GetLength())
		m_watchdogFilePath = "default.pwd";

	CString fileString;
	if (BrowseFile(this,"Load Process Watchdog File",szFilter,m_watchdogFilePath,TRUE,fileString))
	{
		m_watchdogFilePath = fileString;
		m_pWatchdogManager->DisableAddThreads();
		m_pWatchdogManager->PurgeWatchdogProcessThreads(true);
		m_pWatchdogManager->Load(fileString);
		m_pWatchdogManager->Unlock();
		m_pWatchdogManager->EnableAddThreads();
	}
	Refresh();
}

void CProcessWatchDogDlg::OnFileSave()
{
	if (m_watchdogFilePath.GetLength())
	{
		m_pWatchdogManager->Lock();
		m_pWatchdogManager->Save(m_watchdogFilePath);
		m_pWatchdogManager->Unlock();
	}
	else
	{
		OnFileSaveas();
	}
}

void CProcessWatchDogDlg::OnFileSaveas()
{
	static char szFilter[] = "Process Watchdog Files|*.pwd|"  \
		"All Files (*.*)|*.*||";

	if (!m_watchdogFilePath.GetLength())
		m_watchdogFilePath = "default.pwd";

	CString fileString;
	if (BrowseFile(this,"Save Process Watchdog File",szFilter,m_watchdogFilePath,FALSE,fileString))
	{
		m_watchdogFilePath = fileString;
		m_pWatchdogManager->Lock();
		m_pWatchdogManager->Save(m_watchdogFilePath);
		m_pWatchdogManager->Unlock();
	}
}

bool CProcessWatchDogDlg::BrowseFile(CWnd *pParent,const char *pTitle,const char *pFilter,const char *pFile,BOOL openFile,CString &fileString)
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

void CProcessWatchDogDlg::OnClose()
{
	SaveSettings();
	OnHide();
}

void CProcessWatchDogDlg::AddToLog(int flags,const char *pFormatString, ...)
{
	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	m_logCriticalSection.Lock();
	LogMessage newMessage;
	newMessage.m_flags = flags;
	newMessage.m_string = acMessage;
	m_cachedLogMessages.Add(newMessage);
	m_logCriticalSection.Unlock();
}

void CProcessWatchDogDlg::RefreshLog(void)
{
	m_logCriticalSection.Lock();
	int numMessages = m_cachedLogMessages.GetSize();
	for (int messageIndex=0;messageIndex<numMessages;messageIndex++)
	{
		LogMessage &message = m_cachedLogMessages[messageIndex];
		WriteLog(message.m_flags,message.m_string);
	}
	m_cachedLogMessages.RemoveAll();
	m_logCriticalSection.Unlock();
}


static COLORREF FontColors[] = 
{
	RGB(0,0,0), // FONT_COLOR_BLACK
	RGB(128,128,128), // FONT_COLOR_DK_GREY
	RGB(160,160,160), // FONT_COLOR_LT_GREY
	RGB(255,255,255), // FONT_COLOR_WHITE
	RGB(192,0,0), // FONT_COLOR_RED
	RGB(255,0,0), // FONT_COLOR_LT_RED
	RGB(128,0,0), // FONT_COLOR_DK_RED
	RGB(207,127,0), // FONT_COLOR_ORANGE
	RGB(253,155,0), // FONT_COLOR_LT_ORANGE
	RGB(169,104,0), // FONT_COLOR_DK_ORANGE
	RGB(192,192,0), // FONT_COLOR_YELLOW
	RGB(255,255,0), // FONT_COLOR_LT_YELLOW
	RGB(128,128,0), // FONT_COLOR_DK_YELLOW
	RGB(0,192,0), // FONT_COLOR_GREEN
	RGB(0,255,0), // FONT_COLOR_LT_GREEN
	RGB(0,128,0), // FONT_COLOR_DK_GREEN
	RGB(0,192,192), // FONT_COLOR_CYAN
	RGB(0,255,255), // FONT_COLOR_LT_CYAN
	RGB(0,128,128), // FONT_COLOR_DK_CYAN
	RGB(0,0,192), // FONT_COLOR_BLUE
	RGB(0,0,255), // FONT_COLOR_LT_BLUE
	RGB(0,0,128), // FONT_COLOR_DK_BLUE
	RGB(192,0,192), // FONT_COLOR_MAGENTA
	RGB(255,0,255), // FONT_COLOR_LT_MAGENTA
	RGB(128,0,128), // FONT_COLOR_DK_MAGENTA
};

void CProcessWatchDogDlg::WriteLog(int flags,const char *string)
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = 0;
	cf.dwEffects = 0;

	if (flags & LogFlags::FONT_STYLE_BOLD)
	{
		cf.dwMask |= CFM_BOLD;
		cf.dwEffects |= CFE_BOLD;
	}
	if (flags & LogFlags::FONT_STYLE_ITALIC)
	{
		cf.dwMask |= CFM_ITALIC;
		cf.dwEffects |= CFE_ITALIC;
	}
	if (flags & LogFlags::FONT_STYLE_STRIKEOUT)
	{
		cf.dwMask |= CFM_STRIKEOUT;
		cf.dwEffects |= CFE_STRIKEOUT;
	}
	if (flags & LogFlags::FONT_STYLE_UNDERLINE)
	{
		cf.dwMask |= CFM_UNDERLINE;
		cf.dwEffects |= CFE_UNDERLINE;
	}
	int color = (flags>>LogFlags::FONT_COLOR_SHIFT)&LogFlags::FONT_COLOR_MASK;
	if (color!=LogFlags::FONT_COLOR_BLACK)
	{
		cf.dwMask |= CFM_COLOR;
		cf.crTextColor = FontColors[color];
	}
	m_logWindow.SetSel(-2,-2);
	long startSelection1,endSelection1;
	m_logWindow.GetSel(startSelection1,endSelection1);
	m_logWindow.ReplaceSel(string);
	m_logWindow.SetSel(-2,-2);
	long startSelection2,endSelection2;
	m_logWindow.GetSel(startSelection2,endSelection2);
	if (cf.dwMask)
	{
		m_logWindow.SetSel(startSelection1,endSelection2);
		m_logWindow.SetSelectionCharFormat(cf);
	}
	m_logWindow.LineScroll(1);
	m_logWindow.UpdateWindow();
}

void CProcessWatchDogDlg::ClearLog(void)
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD|CFM_ITALIC|CFM_STRIKEOUT|CFM_UNDERLINE|CFM_COLOR;
	cf.dwEffects = 0;
	cf.crTextColor = FontColors[LogFlags::FONT_COLOR_BLACK];
	m_logWindow.SetSel(0,-2);
	m_logWindow.ReplaceSel("");
	m_logWindow.SetSel(0,-2);
	m_logWindow.SetSelectionCharFormat(cf);
	m_logWindow.UpdateWindow();
}

//-----------------------------------------------------------------------------
// CProcessWatchDogDlg::OnOK
// This message handler will be called by the framework when the user hits the
// enter key while the main app dialog is up.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CProcessWatchDogDlg::OnOK(void)
{
	// We just want to absorb this message, thus keeping the app from quitting
	// when the user hits the enter key:
	SetFocus();
}

//-----------------------------------------------------------------------------
// CProcessWatchDogDlg::OnCancel
// This message handler will be called by the framework when the user hits the
// esc key while the main app dialog is up.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CProcessWatchDogDlg::OnCancel(void)
{
	SaveSettings();
	OnHide();
}

void CProcessWatchDogDlg::OnHide(void) 
{
	if (!m_bHidden)
	{
		ShowWindow(SW_HIDE);
		ShowOwnedPopups(FALSE);

		// put the window at the bottom of zorder, so it isn't activated
		SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		// Load icon onto taskbar tray
		m_pTray = new Tray(this,WM_TRAY_NOTIFY,
			NULL,theApp.m_pIconList);
		m_pTray->SetState(0);
		m_bHidden = TRUE;
	}
}

void CProcessWatchDogDlg::OnUnHide(void)
{
	if (m_bHidden)
	{
		ShowWindow (SW_RESTORE) ;
		m_bHidden = FALSE ;
		/////////////////////////////////////
		// Remove icon from taskbar tray
		if (m_pTray)
		{
			delete m_pTray ;
			m_pTray = NULL ;
		}
	}
}


LONG CProcessWatchDogDlg::OnTrayNotify ( WPARAM wParam, LPARAM lParam )
{
	switch (lParam)
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		{
			CMenu menu ;
			/////////////////////////////////////
			// Load and Verify Menu
			VERIFY(menu.LoadMenu(IDR_TRAY));
			CMenu* pPopup = menu.GetSubMenu (0) ;
			ASSERT(pPopup != NULL);

			// Get the cursor position
			POINT pt ;
			GetCursorPos (&pt) ;

			// Fix Microsofts' BUG!!!!
			SetForegroundWindow();

			///////////////////////////////////
			// Display The Menu
			pPopup->TrackPopupMenu(TPM_RIGHTALIGN | TPM_LEFTBUTTON |TPM_RIGHTBUTTON,pt.x, pt.y, AfxGetMainWnd());
			break ;
		}
	case WM_LBUTTONDBLCLK:
		//////////////////////////////////
		// Unhide our Window
		OnUnHide() ;
		break ;
	}
	return (0) ;
}

void CProcessWatchDogDlg::OnFileExit()
{
	if (m_pTray)
	{
		delete m_pTray ;
		m_pTray = NULL;
	}
	DestroyWindow();
}

void CProcessWatchDogDlg::OnRestore()
{
	OnUnHide();
}


void CProcessWatchDogDlg::OnFileClose()
{
	SaveSettings();
	OnHide();
}
