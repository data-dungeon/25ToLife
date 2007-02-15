////////////////////////////////////////////////////////////////////////////
//
// TupperChainGuiAppDlg
//
// Class handle the dialog box
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupperChainGuiAppDlg.cpp $
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 4/11/03    Time: 12:44p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 8/19/02    Time: 3:20p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * added build date/time
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * File Dependency checks
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:48p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * better path support - the work directory can be different from the exe
 * file
 * also a log file is now saved out all the time
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/15/02    Time: 4:15p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Clear now resets the font color and style
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Worker thread for processing
 * Stop Processing Button
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:16p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Progress Bar & Text
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:59p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Put process button code in separate file
 * Changed plugin class (no export function)
 */

#include "stdafx.h"
#include "TupperChainGuiApp.h"
#include "TupperChainGuiAppDlg.h"
#include "registry.h"
#include <direct.h>
#include "log.h"
#include <assert.h>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

/////////////////////////////////////////////////////////////////////////////
// CTupperChainGuiAppDlg dialog

CTupperChainGuiAppDlg::CTupperChainGuiAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTupperChainGuiAppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTupperChainGuiAppDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hFolderIcon = AfxGetApp()->LoadIcon(IDI_FOLDER);
	m_hDeleteEntryIcon = AfxGetApp()->LoadIcon(IDI_DELETE_ENTRY);
}

void CTupperChainGuiAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTupperChainGuiAppDlg)
	DDX_Control(pDX, IDC_DATE_TIME, m_dataTimeText);
	DDX_Control(pDX, IDC_STOP_PROCESSING_BUTTON, m_stopProcessingButton);
	DDX_Control(pDX, IDC_OUTPUT_TUPPERWARE_FILE, m_outputTupperwareFile);
	DDX_Control(pDX, IDC_PROCESS_BAR_TEXT, m_progressBarText);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_progressBar);
	DDX_Control(pDX, IDC_RICHEDIT1, m_richEdit1);
	DDX_Control(pDX, IDC_CONFIGURATION_DELETE_BUTTON, m_configurationDeleteButton);
	DDX_Control(pDX, IDC_OUTPUT_DELETE_BUTTON, m_outputDeleteButton);
	DDX_Control(pDX, IDC_SOURCE_DELETE_BUTTON, m_sourceDeleteButton);
	DDX_Control(pDX, IDC_LOG_WINDOW_STATIC_TEXT, m_logWindowText);
	DDX_Control(pDX, IDC_VERBOSE_LEVEL_COMBO, m_verboseLevelCombo);
	DDX_Control(pDX, IDC_CONFIGURATION_FILE_COMBO, m_configurationFileCombo);
	DDX_Control(pDX, IDC_OUTPUT_FILE_COMBO, m_outputFileCombo);
	DDX_Control(pDX, IDC_SOURCE_FILE_COMBO, m_sourceFileCombo);
	DDX_Control(pDX, IDC_SOURCE_FILE_BUTTON, m_sourceFileButton);
	DDX_Control(pDX, IDC_PROCESS_BUTTON, m_processButton);
	DDX_Control(pDX, IDC_OUTPUT_FILE_BUTTON, m_outputFileButton);
	DDX_Control(pDX, IDC_CONFIGURATION_FILE_BUTTON, m_configurationFileButton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTupperChainGuiAppDlg, CDialog)
	//{{AFX_MSG_MAP(CTupperChainGuiAppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SOURCE_FILE_BUTTON, OnSourceFileButton)
	ON_BN_CLICKED(IDC_OUTPUT_FILE_BUTTON, OnOutputFileButton)
	ON_BN_CLICKED(IDC_CONFIGURATION_FILE_BUTTON, OnConfigurationFileButton)
	ON_BN_CLICKED(IDC_PROCESS_BUTTON, OnProcessButton)
	ON_CBN_SELCHANGE(IDC_OUTPUT_FILE_COMBO, OnSelchangeOutputFileCombo)
	ON_CBN_SELCHANGE(IDC_SOURCE_FILE_COMBO, OnSelchangeSourceFileCombo)
	ON_CBN_SELCHANGE(IDC_CONFIGURATION_FILE_COMBO, OnSelchangeConfigurationFileCombo)
	ON_CBN_SELCHANGE(IDC_VERBOSE_LEVEL_COMBO, OnSelchangeVerboseLevelCombo)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_CBN_KILLFOCUS(IDC_CONFIGURATION_FILE_COMBO, OnKillfocusConfigurationFileCombo)
	ON_CBN_KILLFOCUS(IDC_OUTPUT_FILE_COMBO, OnKillfocusOutputFileCombo)
	ON_CBN_KILLFOCUS(IDC_SOURCE_FILE_COMBO, OnKillfocusSourceFileCombo)
	ON_BN_CLICKED(IDC_SOURCE_DELETE_BUTTON, OnSourceDeleteButton)
	ON_BN_CLICKED(IDC_OUTPUT_DELETE_BUTTON, OnOutputDeleteButton)
	ON_BN_CLICKED(IDC_CONFIGURATION_DELETE_BUTTON, OnConfigurationDeleteButton)
	ON_BN_CLICKED(IDC_OUTPUT_TUPPERWARE_FILE, OnOutputTupperwareFile)
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
	ON_BN_CLICKED(IDC_STOP_PROCESSING_BUTTON, OnStopProcessingButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTupperChainGuiAppDlg message handlers

BOOL CTupperChainGuiAppDlg::OnInitDialog()
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
	char tempText[80];
	sprintf(tempText,"Build %s %s",g_pFilterChainDate,g_pFilterChainTime);
	m_dataTimeText.SetWindowText(tempText);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_pProcessThread = NULL;
	m_stopProcessingButton.EnableWindow(FALSE);

	m_verboseLevel = LogFlags::VERBOSE_LEVEL_NONE;
	m_outputTupperwareFile.SetCheck(FALSE);

	m_sourceFileButton.SetIcon(m_hFolderIcon);
	m_configurationFileButton.SetIcon(m_hFolderIcon);
	m_outputFileButton.SetIcon(m_hFolderIcon);

	m_sourceDeleteButton.SetIcon(m_hDeleteEntryIcon);
	m_configurationDeleteButton.SetIcon(m_hDeleteEntryIcon);
	m_outputDeleteButton.SetIcon(m_hDeleteEntryIcon);

	ReadSettings();

	// set default font
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_FACE;
//	strcpy(cf.szFaceName,"Courier");
	strcpy(cf.szFaceName,"Microsoft Sans Serif");
	m_richEdit1.SetDefaultCharFormat(cf);


	// move window to last saved position
	CRect desktopRect;
	desktopRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	desktopRect.bottom = desktopRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
	desktopRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	desktopRect.right = desktopRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);

	if (m_dialogRect.IsRectEmpty())
	{
		GetWindowRect(&m_dialogRect);
	}
	// make sure the window is visible on the screen
	if (m_dialogRect.top<desktopRect.top)
	{
		m_dialogRect.bottom = desktopRect.top+m_dialogRect.Height();
		m_dialogRect.top = desktopRect.top;
	}
	if (m_dialogRect.right<desktopRect.left+300)
	{
		m_dialogRect.right = desktopRect.left+m_dialogRect.Width();
		m_dialogRect.left = desktopRect.left;
	}
	if (m_dialogRect.top>desktopRect.bottom-300)
	{
		m_dialogRect.top = desktopRect.bottom-m_dialogRect.Height();
		m_dialogRect.bottom = desktopRect.bottom;
	}
	if (m_dialogRect.left>desktopRect.right-300)
	{
		m_dialogRect.left = desktopRect.right-m_dialogRect.Width();
		m_dialogRect.right = desktopRect.right;
	}
	MoveWindow(&m_dialogRect);

	int i;
	for (i=0;i<m_sourceFileStringArray.GetSize();i++)
	{
		m_sourceFileCombo.InsertString(i,m_sourceFileStringArray[i]);
	}
	for (i=0;i<m_outputFileStringArray.GetSize();i++)
	{
		m_outputFileCombo.InsertString(i,m_outputFileStringArray[i]);
	}
	for (i=0;i<m_configurationFileStringArray.GetSize();i++)
	{
		m_configurationFileCombo.InsertString(i,m_configurationFileStringArray[i]);
	}

	m_verboseLevelCombo.SetCurSel(m_verboseLevel);
	m_sourceFileCombo.SetCurSel(m_sourceFileCurSel);
	m_outputFileCombo.SetCurSel(m_outputFileCurSel);
	m_configurationFileCombo.SetCurSel(m_configurationFileCurSel);

	// set work path
	char workPathBuffer[_MAX_PATH];
	_getcwd( workPathBuffer, _MAX_PATH );
	if (workPathBuffer[strlen(workPathBuffer)-1]!='\\')
		strcat(workPathBuffer,"\\");
	m_workPath=workPathBuffer;

	m_progressBar.SetRange(0,1000);
	m_progressBar.SetPos(0);
	m_progressBarText.SetWindowText("Ready");

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTupperChainGuiAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTupperChainGuiAppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTupperChainGuiAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

#define NUM_SLOTS 15
void CTupperChainGuiAppDlg::ReadSettings(void)
{
	m_sourceFileStringArray.RemoveAll();
	m_outputFileStringArray.RemoveAll();
	m_configurationFileStringArray.RemoveAll();

	Registry registry(FILTER_CHAIN_KEY);

	// First check the installed flag
	char registryName[80];
	char registryData[_MAX_PATH];

	int i;
	for (i=0;i<NUM_SLOTS;i++)
	{
		sprintf(registryName,"SourceFile%02d",i);
		if (registry.Get(registryName,registryData, sizeof(registryData)-1))
		{
			CString registryDataString(registryData);
			m_sourceFileStringArray.Add(registryDataString);
		}
	}
	for (i=0;i<NUM_SLOTS;i++)
	{
		sprintf(registryName,"OutputFile%02d",i);
		if (registry.Get(registryName,registryData, sizeof(registryData)-1))
		{
			CString registryDataString(registryData);
			m_outputFileStringArray.Add(registryDataString);
		}
	}
	for (i=0;i<NUM_SLOTS;i++)
	{
		sprintf(registryName,"ConfigurationFile%02d",i);
		if (registry.Get(registryName,registryData, sizeof(registryData)-1))
		{
			CString registryDataString(registryData);
			m_configurationFileStringArray.Add(registryDataString);
		}
	}
	DWORD temp;
	m_verboseLevel = LogFlags::VERBOSE_LEVEL_NONE;
	m_outputTupperwareFile.SetCheck(FALSE);
	m_sourceFileCurSel = -1;
	m_outputFileCurSel = -1;
	m_configurationFileCurSel = -1;

	if (registry.Get("VerboseLevel",&temp))
		m_verboseLevel = (int)temp;
	if (registry.Get("OutputTupperwareFile",&temp))
		m_outputTupperwareFile.SetCheck((int)temp);
	if (registry.Get("SourceFileCurSel",&temp))
		m_sourceFileCurSel = (int)temp;
	if (registry.Get("OutputFileCurSel",&temp))
		m_outputFileCurSel = (int)temp;
	if (registry.Get("ConfigurationFileCurSel",&temp))
		m_configurationFileCurSel = (int)temp;
	m_dialogRect.SetRectEmpty();
	if (registry.Get("DialogRectTop",&temp))
		m_dialogRect.top = (LONG)temp;
	if (registry.Get("DialogRectBottom",&temp))
		m_dialogRect.bottom = (LONG)temp;
	if (registry.Get("DialogRectLeft",&temp))
		m_dialogRect.left = (LONG)temp;
	if (registry.Get("DialogRectRight",&temp))
		m_dialogRect.right = (LONG)temp;
}

void CTupperChainGuiAppDlg::WriteSettings(void)
{
	Registry registry(FILTER_CHAIN_KEY);

	// First check the installed flag
	char registryName[80];

	int i;
	for (i=0;i<NUM_SLOTS;i++)
	{
		sprintf(registryName,"SourceFile%02d",i);
		registry.Delete(registryName);
	}
	for (i=0;i<NUM_SLOTS;i++)
	{
		sprintf(registryName,"OutputFile%02d",i);
		registry.Delete(registryName);
	}
	for (i=0;i<NUM_SLOTS;i++)
	{
		sprintf(registryName,"ConfigurationFile%02d",i);
		registry.Delete(registryName);
	}
	int size = m_sourceFileStringArray.GetSize();
	if (size>NUM_SLOTS)
		size = NUM_SLOTS;
	for (i=0;i<size;i++)
	{
		sprintf(registryName,"SourceFile%02d",i);
		registry.Set(registryName,LPCTSTR(m_sourceFileStringArray[i]));
	}
	size = m_outputFileStringArray.GetSize();
	if (size>NUM_SLOTS)
		size = NUM_SLOTS;
	for (i=0;i<size;i++)
	{
		sprintf(registryName,"OutputFile%02d",i);
		registry.Set(registryName,LPCTSTR(m_outputFileStringArray[i]));
	}
	size = m_configurationFileStringArray.GetSize();
	if (size>NUM_SLOTS)
		size = NUM_SLOTS;
	for (i=0;i<size;i++)
	{
		sprintf(registryName,"ConfigurationFile%02d",i);
		registry.Set(registryName,LPCTSTR(m_configurationFileStringArray[i]));
	}
	registry.Set("VerboseLevel",(DWORD)m_verboseLevel);
	registry.Set("OutputTupperwareFile",(DWORD)m_outputTupperwareFile.GetCheck());
	registry.Set("SourceFileCurSel",(DWORD)m_sourceFileCurSel);
	registry.Set("OutputFileCurSel",(DWORD)m_outputFileCurSel);
	registry.Set("ConfigurationFileCurSel",(DWORD)m_configurationFileCurSel);

	GetWindowRect(&m_dialogRect);
	registry.Set("DialogRectTop",(DWORD)m_dialogRect.top);
	registry.Set("DialogRectBottom",(DWORD)m_dialogRect.bottom);
	registry.Set("DialogRectLeft",(DWORD)m_dialogRect.left);
	registry.Set("DialogRectRight",(DWORD)m_dialogRect.right);
}


void CTupperChainGuiAppDlg::BrowseFile(int &curSelValue,CComboBox &comboBox,CStringArray &array,const char *pTitle,const char *pFilter,BOOL openFile)
{
	CString path;
	comboBox.GetWindowText(path);
	if (path.IsEmpty())
	{
		path=m_workPath;
	}
	char drive[_MAX_DRIVE];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char dir[_MAX_DIR];
	_splitpath((char *)LPCTSTR(path),drive,dir,fname,ext);
	path = drive;
	path += dir;

	unsigned long flags = openFile ? (OFN_FILEMUSTEXIST | OFN_HIDEREADONLY) : (OFN_HIDEREADONLY);

	CFileDialog dialog(openFile, // TRUE Open - FALSE Save
		ext, // default extension
		fname, // initial filename
		flags, // flags
		pFilter, // filter (using | instead of null characters)
		this); // parent window

	dialog.m_ofn.lpstrTitle = pTitle;
	dialog.m_ofn.lpstrInitialDir = (char *)LPCTSTR(path);
    if (dialog.DoModal() != IDOK)
	{
        return;
	}
	path = dialog.GetPathName(); 
	UpdateComboBox(curSelValue,comboBox,array,path);
}

void CTupperChainGuiAppDlg::UpdateComboBox(int &curSelValue,CComboBox &comboBox,CStringArray &array,CString &path) 
{
	if (path.GetLength()==0)
		return;
	// find the text and make the cursel point to the entry if found
	for (int i=0;i<array.GetSize();i++)
	{
		if (array[i]==path)
		{
			curSelValue = i;
			comboBox.SetCurSel(i);
			return;
		}
	}
	// add to start of array
	array.InsertAt(0,path);

	// remove all entrys in the array that are past NUM_SLOTS
	while (array.GetSize()>NUM_SLOTS)
	{
		array.RemoveAt(NUM_SLOTS);
	}

	// delete entries from the combo box list
	while (comboBox.GetCount())
		comboBox.DeleteString(0);

	// add the array to the combo box list
	for (i=0;i<array.GetSize();i++)
	{
		comboBox.InsertString(i,array[i]);
	}
	comboBox.SetCurSel(0);
	curSelValue = 0;
}

void CTupperChainGuiAppDlg::DeleteComboBoxEntry(int &curSelValue,CComboBox &comboBox,CStringArray &array,CString &path) 
{
	// find the text and make the delete from the array
	for (int i=0;i<array.GetSize();i++)
	{
		if (array[i]==path)
		{
			array.RemoveAt(i);
			break;
		}
	}
	// delete entries from the combo box list
	while (comboBox.GetCount())
		comboBox.DeleteString(0);

	// add the array to the combo box list
	for (i=0;i<array.GetSize();i++)
	{
		comboBox.InsertString(i,array[i]);
	}
	if (array.GetSize())
	{
		comboBox.SetCurSel(0);
		curSelValue = 0;
	}
	else
	{
		comboBox.SetCurSel(-1);
		curSelValue = -1;
	}
}

void CTupperChainGuiAppDlg::OnSourceFileButton() 
{
    static char szTitle[] = "Browse Source File";
    static char szFilter[] = "Tupperware Files|*.tup|"  \
                              "All Files (*.*)|*.*||";

	BrowseFile(m_sourceFileCurSel,m_sourceFileCombo,m_sourceFileStringArray,szTitle,szFilter,TRUE);
}

void CTupperChainGuiAppDlg::OnOutputFileButton() 
{
    static char szTitle[] = "Browse Output File";
    static char szFilter[] = "Dibble Files|*.dbl|"  \
                              "All Files (*.*)|*.*||";

	BrowseFile(m_outputFileCurSel,m_outputFileCombo,m_outputFileStringArray,szTitle,szFilter,FALSE);
}

void CTupperChainGuiAppDlg::OnConfigurationFileButton() 
{
    static char szTitle[] = "Browse Configuration File";
    static char szFilter[] = "Filter Chain Configuration Files|*.twc|"  \
                              "All Files (*.*)|*.*||";

	BrowseFile(m_configurationFileCurSel,m_configurationFileCombo,m_configurationFileStringArray,szTitle,szFilter,TRUE);
}


BOOL CTupperChainGuiAppDlg::DestroyWindow() 
{
	WriteSettings();
	m_stopButtonPressed = true;
	if (m_pProcessThread)
	{
		TerminateThread(m_pProcessThread->m_hThread,0);
	}
	return CDialog::DestroyWindow();
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


void CTupperChainGuiAppDlg::WriteLog(int flags,const char *string)
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
	m_richEdit1.SetSel(-2,-2);
	long startSelection1,endSelection1;
	m_richEdit1.GetSel(startSelection1,endSelection1);
	m_richEdit1.ReplaceSel(string);
	m_richEdit1.SetSel(-2,-2);
	m_richEdit1.ReplaceSel("\r\n");
	m_richEdit1.SetSel(-2,-2);
	long startSelection2,endSelection2;
	m_richEdit1.GetSel(startSelection2,endSelection2);
	if (cf.dwMask)
	{
		m_richEdit1.SetSel(startSelection1,endSelection2);
		m_richEdit1.SetSelectionCharFormat(cf);
	}
	m_richEdit1.LineScroll(1);
	m_richEdit1.UpdateWindow();
}

void CTupperChainGuiAppDlg::ClearLog(void)
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD|CFM_ITALIC|CFM_STRIKEOUT|CFM_UNDERLINE|CFM_COLOR;
	cf.dwEffects = 0;
	cf.crTextColor = FontColors[LogFlags::FONT_COLOR_BLACK];
	m_richEdit1.SetSel(0,-2);
	m_richEdit1.ReplaceSel("");
	m_richEdit1.SetSel(0,-2);
	m_richEdit1.SetSelectionCharFormat(cf);
	m_richEdit1.UpdateWindow();
}

void CTupperChainGuiAppDlg::OnSelchangeOutputFileCombo() 
{
	m_outputFileCurSel = m_outputFileCombo.GetCurSel();
}

void CTupperChainGuiAppDlg::OnSelchangeSourceFileCombo() 
{
	m_sourceFileCurSel = m_sourceFileCombo.GetCurSel();
}

void CTupperChainGuiAppDlg::OnSelchangeConfigurationFileCombo() 
{
	m_configurationFileCurSel = m_configurationFileCombo.GetCurSel();
}

void CTupperChainGuiAppDlg::OnSelchangeVerboseLevelCombo() 
{
	m_verboseLevel = m_verboseLevelCombo.GetCurSel();
}

void CTupperChainGuiAppDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);

	if (::IsWindow(m_processButton.m_hWnd))
	{
		CRect myRect;
		m_processButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_processButton.MoveWindow(rect.Width()-88, myRect.TopLeft().y, 75, myRect.Height());
	}
	if (::IsWindow(m_stopProcessingButton.m_hWnd))
	{
		CRect myRect;
		m_stopProcessingButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_stopProcessingButton.MoveWindow(rect.Width()-180, myRect.TopLeft().y, 86, myRect.Height());
	}
	if (::IsWindow(m_sourceFileCombo.m_hWnd))
	{
		CRect myRect;
		m_sourceFileCombo.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_sourceFileCombo.MoveWindow(myRect.TopLeft().x, myRect.TopLeft().y, rect.Width()-(myRect.TopLeft().x+60), myRect.Height());
	}
	if (::IsWindow(m_outputFileCombo.m_hWnd))
	{
		CRect myRect;
		m_outputFileCombo.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_outputFileCombo.MoveWindow(myRect.TopLeft().x, myRect.TopLeft().y, rect.Width()-(myRect.TopLeft().x+60), rect.Height());
	}
	if (::IsWindow(m_configurationFileCombo.m_hWnd))
	{
		CRect myRect;
		m_configurationFileCombo.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_configurationFileCombo.MoveWindow(myRect.TopLeft().x, myRect.TopLeft().y, rect.Width()-(myRect.TopLeft().x+60), rect.Height());
	}
	if (::IsWindow(m_sourceFileButton.m_hWnd))
	{
		CRect myRect;
		m_sourceFileButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_sourceFileButton.MoveWindow(rect.Width()-56, myRect.TopLeft().y, 24, myRect.Height());
	}
	if (::IsWindow(m_outputFileButton.m_hWnd))
	{
		CRect myRect;
		m_outputFileButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_outputFileButton.MoveWindow(rect.Width()-56, myRect.TopLeft().y, 24, myRect.Height());
	}
	if (::IsWindow(m_configurationFileButton.m_hWnd))
	{
		CRect myRect;
		m_configurationFileButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_configurationFileButton.MoveWindow(rect.Width()-56, myRect.TopLeft().y, 24, myRect.Height());
	}
	if (::IsWindow(m_sourceDeleteButton.m_hWnd))
	{
		CRect myRect;
		m_sourceDeleteButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_sourceDeleteButton.MoveWindow(rect.Width()-28, myRect.TopLeft().y, 24, myRect.Height());
	}
	if (::IsWindow(m_outputDeleteButton.m_hWnd))
	{
		CRect myRect;
		m_outputDeleteButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_outputDeleteButton.MoveWindow(rect.Width()-28, myRect.TopLeft().y, 24, myRect.Height());
	}
	if (::IsWindow(m_configurationDeleteButton.m_hWnd))
	{
		CRect myRect;
		m_configurationDeleteButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_configurationDeleteButton.MoveWindow(rect.Width()-28, myRect.TopLeft().y, 24, myRect.Height());
	}
	if (::IsWindow(m_richEdit1.m_hWnd))
	{
		CRect myRect;
		m_richEdit1.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_richEdit1.MoveWindow(myRect.TopLeft().x, myRect.TopLeft().y, rect.Width()-(myRect.TopLeft().x+10), rect.Height()-(myRect.TopLeft().y+24));
	}
	if (::IsWindow(m_progressBar.m_hWnd))
	{
		CRect myRect;
		m_progressBar.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		const float desiredRatio = (500.0f-312.0f)/510.0f;
		int width = (int)(rect.Width()*desiredRatio);
		m_progressBar.MoveWindow(rect.Width()-(width+10), rect.Height()-21, width, 18);
	}
	if (::IsWindow(m_progressBarText.m_hWnd))
	{
		CRect myRect;
		m_progressBarText.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		const float desiredRatio = (310.0f-11.0f)/510.0f;
		int width = (int)(rect.Width()*desiredRatio);
		m_progressBarText.MoveWindow(myRect.TopLeft().x, rect.Height()-18, width, 13);
	}
	Invalidate();
}

void CTupperChainGuiAppDlg::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 300;
}

void CTupperChainGuiAppDlg::OnKillfocusConfigurationFileCombo() 
{
	CString path;
	m_configurationFileCombo.GetWindowText(path);
	UpdateComboBox(m_configurationFileCurSel,m_configurationFileCombo,m_configurationFileStringArray,path);
}

void CTupperChainGuiAppDlg::OnKillfocusOutputFileCombo() 
{
	CString path;
	m_outputFileCombo.GetWindowText(path);
	UpdateComboBox(m_outputFileCurSel,m_outputFileCombo,m_outputFileStringArray,path);
}

void CTupperChainGuiAppDlg::OnKillfocusSourceFileCombo() 
{
	CString path;
	m_sourceFileCombo.GetWindowText(path);
	UpdateComboBox(m_sourceFileCurSel,m_sourceFileCombo,m_sourceFileStringArray,path);
}

void CTupperChainGuiAppDlg::OnSourceDeleteButton() 
{
	m_sourceFileCurSel = m_sourceFileCombo.GetCurSel();
	CString path;
	m_sourceFileCombo.GetWindowText(path);
	DeleteComboBoxEntry(m_sourceFileCurSel,m_sourceFileCombo,m_sourceFileStringArray,path);
}

void CTupperChainGuiAppDlg::OnOutputDeleteButton() 
{
	m_outputFileCurSel = m_outputFileCombo.GetCurSel();
	CString path;
	m_outputFileCombo.GetWindowText(path);
	DeleteComboBoxEntry(m_outputFileCurSel,m_outputFileCombo,m_outputFileStringArray,path);
}

void CTupperChainGuiAppDlg::OnConfigurationDeleteButton() 
{
	m_configurationFileCurSel = m_configurationFileCombo.GetCurSel();
	CString path;
	m_configurationFileCombo.GetWindowText(path);
	DeleteComboBoxEntry(m_configurationFileCurSel,m_configurationFileCombo,m_configurationFileStringArray,path);
}

void CTupperChainGuiAppDlg::OnOutputTupperwareFile() 
{
	UpdateData(TRUE);
}

void CTupperChainGuiAppDlg::SetProgressBar(int min,int max,int value)
{
	m_progressBar.SetRange(min,max);
	m_progressBar.SetPos(value);
}

void CTupperChainGuiAppDlg::SetProgressText(const char *string)
{
	m_progressBarText.SetWindowText(string);
}

void CTupperChainGuiAppDlg::EnableControls(BOOL enable)
{
	m_configurationDeleteButton.EnableWindow(enable);
	m_outputDeleteButton.EnableWindow(enable);
	m_sourceDeleteButton.EnableWindow(enable);
	m_logWindowText.EnableWindow(enable);
	m_verboseLevelCombo.EnableWindow(enable);
	m_configurationFileCombo.EnableWindow(enable);
	m_outputFileCombo.EnableWindow(enable);
	m_sourceFileCombo.EnableWindow(enable);
	m_sourceFileButton.EnableWindow(enable);
	m_processButton.EnableWindow(enable);
	m_outputFileButton.EnableWindow(enable);
	m_configurationFileButton.EnableWindow(enable);
	m_outputTupperwareFile.EnableWindow(enable);
}

void CTupperChainGuiAppDlg::SetPluginPath(const char *pluginPath)
{
	m_pluginPath = pluginPath;
}
