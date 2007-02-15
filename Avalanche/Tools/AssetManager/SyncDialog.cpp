// SyncDialog.cpp : implementation file
//

#include "stdafx.h"
#include "assetmanager.h"
#include "SyncDialog.h"
#include ".\syncdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SyncDialog dialog


SyncDialog::SyncDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SyncDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(SyncDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCancel = false;
	m_bClose = false;
	m_pSynchingThread = NULL;
	m_bThreadRunning = false;
	m_bInitDialogCalled = false;
	m_titleText = "Syncronizing...";
	m_bCloseCheck = false;
	m_bExitCheck = false;
	m_bUpdateDuplicates = false;

}


void SyncDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SyncDialog)
	DDX_Control(pDX, IDB_DONE, m_doneButton);
	DDX_Check(pDX, IDC_CLOSE_CHECK, m_bCloseCheck);
	DDX_Check(pDX, IDC_EXIT_CHECK, m_bExitCheck);
	DDX_Control(pDX, IDC_SYNC_TEXT5, m_syncText5);
	DDX_Control(pDX, IDC_SYNC_TEXT4, m_syncText4);
	DDX_Control(pDX, IDC_SYNC_TEXT3, m_syncText3);
	DDX_Control(pDX, IDC_SYNC_TEXT2B, m_syncText2b);
	DDX_Control(pDX, IDC_SYNC_TEXT2A, m_syncText2a);
	DDX_Control(pDX, IDC_SYNC_TEXT1B, m_syncText1b);
	DDX_Control(pDX, IDC_SYNC_TEXT1A, m_syncText1a);
	DDX_Control(pDX, IDC_PROGRESS2, m_progress2);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress1);
	DDX_Control(pDX, IDC_LOGWINDOW, m_logWindow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SyncDialog, CDialog)
	//{{AFX_MSG_MAP(SyncDialog)
	ON_BN_CLICKED(IDB_DONE, OnDoneButton)
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CLOSE_CHECK, OnBnClickedCloseCheck)
	ON_BN_CLICKED(IDC_EXIT_CHECK, OnBnClickedExitCheck)
END_MESSAGE_MAP()

BOOL SyncDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bInitDialogCalled = true;
	SetWindowText(m_titleText);

	// hide checkboxes
	if (m_bUpdateDuplicates)
	{
		GetDlgItem(IDC_EXIT_CHECK)->ShowWindow(FALSE);
		GetDlgItem(IDC_CLOSE_CHECK)->ShowWindow(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


/////////////////////////////////////////////////////////////////////////////
// SyncDialog message handlers

// this function is used to completely get out of the dialog
// sent by the onkickidle function when the thread is complete
void SyncDialog::OnCancel() 
{
	if (m_bClose)
		EndDialog(0);
	// don't let the ESC key kick us out
}

void SyncDialog::OnOK() 
{
	// don't let the enter button kick us out
}

void SyncDialog::OnDoneButton() 
{
	m_bCancel = true;
	if (!m_pSynchingThread)
	{
		EndDialog(0);
	}
}

LRESULT SyncDialog::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
	if (m_pSynchingThread && !m_bThreadRunning)
	{
		// thread is done - close down the dialog
		m_pSynchingThread = NULL;
		if (!m_bCloseCheck)
		{
			m_doneButton.SetWindowText("OK");
		}
		else
		{
			m_bClose = true;
			// we can't call EndDialog here since we are in the idle processing...
			// post a cancel message instead
			PostMessage(WM_COMMAND, IDCANCEL, 0);
		}
	}
	return FALSE;
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

void SyncDialog::WriteLog(int flags,const char *string)
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

void SyncDialog::ClearLog(void)
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

void SyncDialog::SetThread(CWinThread *pSynchingThread)
{
	m_pSynchingThread = pSynchingThread;
}

void SyncDialog::SetThreadRunning(bool bThreadRunning)
{
	m_bThreadRunning = bThreadRunning;
}

bool SyncDialog::GetCancelState(void) const
{
	return m_bCancel;
}

void SyncDialog::SetSourceFileTitle(const char *string)
{
	m_syncText1a.SetWindowText(string);
}

void SyncDialog::SetSourceFile(const char *string)
{
	m_syncText1b.SetWindowText(string);
}

void SyncDialog::SetDestFileTitle(const char *string)
{
	m_syncText2a.SetWindowText(string);
}

void SyncDialog::SetDestFile(const char *string)
{
	m_syncText2b.SetWindowText(string);
}

void SyncDialog::SetCurrentFileProgress(int percent)
{
	m_progress1.SetRange(0,100);
	m_progress1.SetPos(percent);
}

void SyncDialog::SetTotalFileProgress(int percent)
{
	m_progress2.SetRange(0,100);
	m_progress2.SetPos(percent);
}

void SyncDialog::SetTransferRate(const char *string)
{
	m_syncText4.SetWindowText(string);
}

void SyncDialog::SetEstimatedTime(const char *string)
{
	m_syncText3.SetWindowText(string);
}

void SyncDialog::SetErrorText(const char *string)
{
	m_syncText5.SetWindowText(string);
}

bool SyncDialog::GetInitDialogCalled(void) const
{
	return m_bInitDialogCalled;
}

void SyncDialog::SetDialogTitle(const char *string)
{
	m_titleText = string;
	if (::IsWindow(m_hWnd))
	{
		SetWindowText(m_titleText);
	}
}


void SyncDialog::OnBnClickedCloseCheck()
{
	UpdateData(TRUE);
}

void SyncDialog::OnBnClickedExitCheck()
{
	UpdateData(TRUE);
}
