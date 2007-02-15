// GameClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "DlgProxy.h"

#include "HardwarePane.h"
#include "Communicator.h"

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
// CGameClientDlg dialog

IMPLEMENT_DYNAMIC(CGameClientDlg, CDialog);

CGameClientDlg::CGameClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGameClientDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;

}

CGameClientDlg::~CGameClientDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CGameClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameClientDlg)
	DDX_Control(pDX, IDC_MASTERTAB, m_MasterTab);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGameClientDlg, CDialog)
	//{{AFX_MSG_MAP(CGameClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_MASTERTAB, OnSelchangeMastertab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_MASTERTAB, OnSelchangingMastertab)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameClientDlg message handlers

BOOL CGameClientDlg::OnInitDialog()
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

	// create all the pane dialogs as children of this dialog...
	CGameClientPane *dlg;
	dlg = new CHardwarePane();
	AddPane(dlg, "Hardware");

	ShowPane(0);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGameClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGameClientDlg::OnPaint() 
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
HCURSOR CGameClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CGameClientDlg::OnClose() 
{
	if (CanExit())
	{
		if(theApp.IsConnected())
			theApp.Disconnect();

		KillAllPanes();

		CDialog::OnClose();
	}
}

BOOL CGameClientDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

// shut these down for esc & return to do nothing
void CGameClientDlg::OnOK()
{
}

void CGameClientDlg::OnCancel()
{
	if(m_Panes.GetSize() == 0)
		CDialog::OnCancel();
}

void CGameClientDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	RECT rect;
	GetReducedClientRect(&rect);

	// resize the tabs
	if(::IsWindow(m_MasterTab.m_hWnd))
	{
		m_MasterTab.MoveWindow(&rect);
		// resize all panes...
		m_MasterTab.GetClientRect(&rect);
		m_MasterTab.AdjustRect(FALSE, &rect);
		for(int i = 0; i <= m_Panes.GetUpperBound(); i++)
		{
			if(IsWindow(*m_Panes[i]))
				m_Panes[i]->MoveWindow(&rect);
		}
	}
}

int CGameClientDlg::AddPane(CGameClientPane *dlg, CString name)
{
	int numPanes = m_MasterTab.GetItemCount();

	VERIFY(dlg->Create(dlg->GetDialogIDD(), &m_MasterTab));

	if(theApp.m_Communicator)
		if(!dlg->Connect(theApp.m_Communicator->GetDeviceIPAddress()))
			return -1;

	if(numPanes == 0)
		ShowPane(dlg);
	else
		dlg->ShowWindow(SW_HIDE);

	m_MasterTab.InsertItem(TCIF_TEXT|TCIF_PARAM, numPanes, name, 0, (LPARAM)dlg);

	m_MasterTab.SetCurSel(0);

	m_Panes.Add(dlg);

	return numPanes;
}

void CGameClientDlg::GetReducedClientRect(LPRECT rect)
{
	CRect* r = (CRect*)rect;
	GetClientRect(r);
	r->DeflateRect(11,11,11,11);
}


void CGameClientDlg::OnSelchangeMastertab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ShowPane(m_MasterTab.GetCurSel());
	
	*pResult = 0;
}

void CGameClientDlg::OnSelchangingMastertab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CDialog* dlg = GetPane(m_MasterTab.GetCurSel());
	
	dlg->ShowWindow(SW_HIDE);
	
	*pResult = 0;
}

void CGameClientDlg::ShowPane(CDialog* pane)
{
	CRect rect;

	m_MasterTab.GetClientRect(&rect);
	m_MasterTab.AdjustRect(FALSE, &rect);

	pane->MoveWindow(&rect);
	pane->ShowWindow(SW_SHOW);
}

CGameClientPane* CGameClientDlg::GetPane(int paneNumber)
{
	TCITEM item;
	m_MasterTab.GetItem(paneNumber, &item);
	CDialog* dlg = (CDialog*)item.lParam;
	ASSERT(dlg->IsKindOf(RUNTIME_CLASS(CDialog)));
	return (CGameClientPane*)dlg;
}

void CGameClientDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	KillAllPanes();
}

void CGameClientDlg::ShowPane(int pane)
{
	CDialog* dlg = GetPane(m_MasterTab.GetCurSel());

	ShowPane(dlg);
}

void CGameClientDlg::ResetPanes()
{
	KillAllPanes(1);
	RedrawWindow();
}

void CGameClientDlg::KillAllPanes(int startWithPane)
{
	// run through and kill all the sub dialogs...
	while(m_MasterTab.GetItemCount() > startWithPane)
	{
		CGameClientPane* dlg = GetPane(startWithPane);
		if(theApp.m_Communicator)
			dlg->Disconnect();
		dlg->DestroyWindow();
		for(int i = m_Panes.GetUpperBound(); i >= 0; i--)
		{
			if(m_Panes[i] == dlg)
				m_Panes.RemoveAt(i);
		}
		delete dlg;
		m_MasterTab.DeleteItem(startWithPane);
	}
}
