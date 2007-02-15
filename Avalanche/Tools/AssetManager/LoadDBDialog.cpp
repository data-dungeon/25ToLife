// LoadDBDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AssetManager.h"
#include "assetmanager.h"
#include "LoadDBDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LoadDBDialog dialog


LoadDBDialog::LoadDBDialog(CWnd* pParent /*=NULL*/)
	: CDialog(LoadDBDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(LoadDBDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCancel = false;
	m_bClose = false;
	m_pLoadingThread = NULL;
	m_bInitDialogCalled = false;
	m_bThreadRunning = false;
}


void LoadDBDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LoadDBDialog)
	DDX_Control(pDX, IDC_LOADDB_TEXT3, m_text3);
	DDX_Control(pDX, IDC_LOADDB_TEXT2, m_text2);
	DDX_Control(pDX, IDC_LOADDB_TEXT1, m_text1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LoadDBDialog, CDialog)
	//{{AFX_MSG_MAP(LoadDBDialog)
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
	ON_BN_CLICKED(IDB_CANCEL, OnCancelButton)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL LoadDBDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bInitDialogCalled = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// LoadDBDialog message handlers

// this function is used to completely get out of the dialog
// sent by the onkickidle function when the thread is complete
void LoadDBDialog::OnCancel() 
{
	if (m_bClose)
		EndDialog(0);
	// don't let the ESC key kick us out
}

void LoadDBDialog::OnOK() 
{
	// don't let the enter button kick us out
}

void LoadDBDialog::OnCancelButton() 
{
	m_text1.SetWindowText("Cancelling...");
	m_text2.SetWindowText("");
	m_text3.SetWindowText("");
	m_bCancel = true;
}

LRESULT LoadDBDialog::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
	if (m_pLoadingThread && !m_bThreadRunning)
	{
		// thread is done - close down the dialog
		m_pLoadingThread = NULL;
		m_bClose = true;
		// we can't call EndDialog here since we are in the idle processing...
		// post a cancel message instead
		PostMessage(WM_COMMAND, IDCANCEL, 0);
	}
	return FALSE;
}

bool LoadDBDialog::GetCancelState(void) const
{
	return m_bCancel;
}

void LoadDBDialog::SetTextLine1(const char *pString)
{
	if (!m_bCancel)
		m_text1.SetWindowText(pString);
}

void LoadDBDialog::SetTextLine2(const char *pString)
{
	if (!m_bCancel)
		m_text2.SetWindowText(pString);
}

void LoadDBDialog::SetTextLine3(const char *pString)
{
	if (!m_bCancel)
		m_text3.SetWindowText(pString);
}

void LoadDBDialog::SetThread(CWinThread *pLoadingThread)
{
	m_pLoadingThread = pLoadingThread;
	m_bThreadRunning = true;
}

void LoadDBDialog::SetThreadRunning(bool bThreadRunning)
{
	m_bThreadRunning = bThreadRunning;
}


bool LoadDBDialog::GetInitDialogCalled(void) const
{
	return m_bInitDialogCalled;
}
