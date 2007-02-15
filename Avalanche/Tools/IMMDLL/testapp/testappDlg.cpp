// testappDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testapp.h"
#include "testappDlg.h"
#include ".\testappdlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CtestappDlg dialog

CtestappDlg* gDialog;
IMM::NewConnectionAction NewConnectionCallback(IMM::IConnection* connection)
{
	return gDialog->NewConnection(connection);
}


CtestappDlg::CtestappDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtestappDlg::IDD, pParent)
{
	mConnection = NULL;
	mTheKitchenSink = NULL;
	gDialog = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestappDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISCONNECT, mDisconnect);
}

BEGIN_MESSAGE_MAP(CtestappDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DISCONNECT, OnBnClickedDisconnect)
END_MESSAGE_MAP()


// CtestappDlg message handlers

BOOL CtestappDlg::OnInitDialog()
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

	// TODO: Add extra initialization here
	if(!IMM::AddNewConnectionCallback(NewConnectionCallback))
		return FALSE;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtestappDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CtestappDlg::OnPaint() 
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

	if ( mConnection )
	{
		static float x = 0;

		IMM::Vector3 eye;
		IMM::Vector3 focus;
		IMM::Vector3 up;
		eye.v[0] = 0.0f;
		eye.v[1] = 0.0f;
		eye.v[2] = 0.0f;
		focus.v[0] = sinf(x);
		focus.v[1] = 0.0f;
		focus.v[2] = cosf(x);
		up.v[0] = 0.0f;
		up.v[1] = 1.0f;
		up.v[2] = 0.0f;
		x += 3.1415f / 180.0f;

		mConnection->UpdateCamera( eye, focus, up );
		IMM::Matrix matrix;
		for(int i = 0; i < 4; i++) for(int j = 0; j < 4; j++)
         matrix.m[i][j] = i + j*4 + x;
		mConnection->MoveInstanceByName( "My Instance", matrix );
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtestappDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

IMM::NewConnectionAction CtestappDlg::NewConnection(IMM::IConnection* connection)
{
	if ( mConnection != NULL )
		return IMM::REFUSE_CONNECTION;

	mConnection = connection;
	mTheKitchenSink = new IMMEventSink( mConnection, this );
	mDisconnect.EnableWindow( true );
	
	return IMM::ACCEPT_CONNECTION;
}

CtestappDlg::~CtestappDlg( )
{
	IMM::RemoveNewConnectionCallback( NewConnectionCallback );

	if ( mTheKitchenSink )
		delete mTheKitchenSink;

	if ( mConnection )
		delete mConnection;
}

void CtestappDlg::OnBnClickedDisconnect( )
{
	ASSERT( mConnection != NULL );

	delete mTheKitchenSink;
	mTheKitchenSink = NULL;

	delete mConnection;
	mConnection = NULL;

	mDisconnect.EnableWindow( false );
}
