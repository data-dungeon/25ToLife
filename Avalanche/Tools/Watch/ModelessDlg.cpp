// ModelessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "watch.h"
#include "ModelessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog


CModelessDlg::CModelessDlg(CWnd* pParent, UINT id)
	: CDialog(id, pParent)
{
	//{{AFX_DATA_INIT(CModelessDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	idd = id;
	pView = pParent;
	inited = false;
}


void CModelessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelessDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelessDlg, CDialog)
	//{{AFX_MSG_MAP(CModelessDlg)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg message handlers

void CModelessDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
	pView->SendMessage(WM_USER + idd);
//	DestroyWindow();
}

void CModelessDlg::OnOK()
{
	OnCancel();
}

void CModelessDlg::PostNcDestroy() 
{
//	delete this;
}

void CModelessDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	if (inited)
	{
		CWinApp* pApp = AfxGetApp();
		CWnd* pWnd = pApp->m_pMainWnd;

		// save the window placement if the window is not minimized or maximized
		WINDOWPLACEMENT wndpl;
		pWnd->GetWindowPlacement(&wndpl);
		if (SW_SHOWNORMAL == wndpl.showCmd)
		{
			RECT rect;
			GetWindowRect(&rect);
			CString str;
			str.Format("Dialogs\\%d", idd);
			pApp->WriteProfileInt(str, "x", rect.left);
			pApp->WriteProfileInt(str, "y", rect.top);
//			TRACE("x: %d   y: %d\n", x, y);
		}
	}
}

LRESULT CModelessDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (WM_CANCEL_DIALOG == message)
	{
		OnCancel();
		return 0;
	}
	else
		return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CModelessDlg::OnInitDialog() 
{
	inited = true;

	CString str;
	str.Format("Dialogs\\%d", idd);

	int x, y;
   x = AfxGetApp()->GetProfileInt(str, "x", 0);
   y = AfxGetApp()->GetProfileInt(str, "y", 0);

	TRACE("OnInitDialog Window x: %d   y: %d\n", x, y);
	SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
