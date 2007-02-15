// testgameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testgame.h"
#include "testgameDlg.h"
#include ".\testgamedlg.h"

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


// CtestgameDlg dialog



CtestgameDlg::CtestgameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtestgameDlg::IDD, pParent)
	, mInstName(_T(""))
{
	memset( mCam, 0, sizeof( mCam ) );
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mSocket = NULL;
}

CtestgameDlg::~CtestgameDlg()
{
	CloseConnection();
}


void CtestgameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT, mConnect);
	DDX_Control(pDX, IDC_DISCONNECT, mDisconnect);
	DDX_Text(pDX, IDC_CAM00, mCam[0][0]);
	DDX_Text(pDX, IDC_CAM01, mCam[0][1]);
	DDX_Text(pDX, IDC_CAM02, mCam[0][2]);
	DDX_Text(pDX, IDC_CAM10, mCam[1][0]);
	DDX_Text(pDX, IDC_CAM11, mCam[1][1]);
	DDX_Text(pDX, IDC_CAM12, mCam[1][2]);
	DDX_Text(pDX, IDC_CAM20, mCam[2][0]);
	DDX_Text(pDX, IDC_CAM21, mCam[2][1]);
	DDX_Text(pDX, IDC_CAM22, mCam[2][2]);
	DDX_Text(pDX, IDC_INST00, mInst[0][0]);
	DDX_Text(pDX, IDC_INST01, mInst[0][1]);
	DDX_Text(pDX, IDC_INST02, mInst[0][2]);
	DDX_Text(pDX, IDC_INST03, mInst[0][3]);
	DDX_Text(pDX, IDC_INST10, mInst[1][0]);
	DDX_Text(pDX, IDC_INST11, mInst[1][1]);
	DDX_Text(pDX, IDC_INST12, mInst[1][2]);
	DDX_Text(pDX, IDC_INST13, mInst[1][3]);
	DDX_Text(pDX, IDC_INST20, mInst[2][0]);
	DDX_Text(pDX, IDC_INST21, mInst[2][1]);
	DDX_Text(pDX, IDC_INST22, mInst[2][2]);
	DDX_Text(pDX, IDC_INST23, mInst[2][3]);
	DDX_Text(pDX, IDC_INST30, mInst[3][0]);
	DDX_Text(pDX, IDC_INST31, mInst[3][1]);
	DDX_Text(pDX, IDC_INST32, mInst[3][2]);
	DDX_Text(pDX, IDC_INST33, mInst[3][3]);
	DDX_Text(pDX, IDC_INSTNAME, mInstName);
}

BEGIN_MESSAGE_MAP(CtestgameDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CONNECT, OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, OnBnClickedDisconnect)
END_MESSAGE_MAP()


// CtestgameDlg message handlers

BOOL CtestgameDlg::OnInitDialog()
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
	mTimerID = SetTimer(1, 50, NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtestgameDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CtestgameDlg::OnPaint() 
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
HCURSOR CtestgameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CtestgameDlg::OnBnClickedConnect()
{
	mConnect.EnableWindow( false );
	mDisconnect.EnableWindow( true );
	mSocket = new CAsyncSocket( );
	mSocket->Create( );
	mSocket->Connect( "localhost", 8953 );
}

void CtestgameDlg::CloseConnection()
{
	delete mSocket;
	mSocket = NULL;
}

void CtestgameDlg::OnBnClickedDisconnect()
{
	CloseConnection();
	mConnect.EnableWindow( true );
	mDisconnect.EnableWindow( false );
}

template <class T> void CtestgameDlg::Send(const T& var)
{
	Send( &var, sizeof( var ) );
}

void CtestgameDlg::Send(const void* buffer, int bufflen)
{
	if ( !mSocket )
		return;

	TRACE("GAME: Sending packet -- %d (%d : %d)", bufflen, GetLastError(), WSAGetLastError());
	char* dat = (char*)buffer;
	for ( int i = 0; i < bufflen; i++ )
	{
		if ( ( i % 8 ) == 0 )
			TRACE( "\n\t" );
		TRACE( "%02.2X ", *dat++ );
	}
	TRACE( "\n" );

	int bytes = mSocket->Send( buffer, bufflen );
	if ( bytes < bufflen )
	{
		TRACE("GAME: output of %d failed with %d (%d : %d)\n", bytes, bufflen, GetLastError(), WSAGetLastError());
		OnBnClickedDisconnect();
		return;
	}
}

template <class T> bool CtestgameDlg::Receive(T& var, bool waitforit)
{
	return Receive( &var, sizeof( var ), waitforit );
}

bool CtestgameDlg::Receive(void* buffer, int bufflen, bool waitforit)
{
	if ( !mSocket )
		return false;

	while ( bufflen > 0 )
	{
		int bytes = mSocket->Receive( buffer, bufflen, 0 );
		int err = WSAGetLastError();
		if ( bytes <= 0 && err != WSAEWOULDBLOCK )
		{
			OnBnClickedDisconnect();
			return false;
		}
		else if ( bytes <= 0 && err == WSAEWOULDBLOCK )
		{
			if ( !waitforit )
				return false;
		}
		if ( bytes > 0 )
		{
			bufflen -= bytes;
		}
		else
			Sleep(1);
	}
	return true;
}

void CtestgameDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( mSocket )
	{
		unsigned long signature;
		if ( !Receive(signature, false) )
			return;

		switch(signature & 0x000000FF)
		{
		case 0x00000000:
			// reserved for ping... just ack it.
			break;
		case 0x00000001:
			// update camera
			// read the matrix
			for ( int i = 0; i < 3; i++ ) for ( int j = 0; j < 3; j++ )
			{
				Receive(mCam[i][j]);
			}

			UpdateData(FALSE);
			break;
		case 0x00000002:
			TRACE("GAME: Recieved updateInstance command\n");
			// update camera
			// read the matrix
			char instname[64];
			Receive( &instname[0], 64 );
			instname[63] = '\0';
			mInstName = instname;
			for ( int i = 0; i < 4; i++ ) for ( int j = 0; j < 4; j++ )
			{
				Receive(mInst[i][j]);
			}
			TRACE("GAME: Recieved data\n");

			UpdateData(FALSE);
			break;
		}
		Send((unsigned char)0x00);
		Send(signature);
	}
}

