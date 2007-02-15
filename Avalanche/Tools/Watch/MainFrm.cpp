// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Watch.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	rashDoneMsg = RegisterWindowMessage("WM_RASHDONE");
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

  	if (!m_wndStatusBar.Create(this) ||
  		!m_wndStatusBar.SetIndicators(indicators,
  		  sizeof(indicators)/sizeof(UINT)))
  	{
  		TRACE0("Failed to create status bar\n");
  		return -1;      // fail to create
  	}

  	EnableDocking(CBRS_ALIGN_ANY);
  	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  	DockControlBar(&m_wndToolBar);
	
	LoadBarState("Toolbars");

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

  	cs.x = AfxGetApp()->GetProfileInt("Location", "x", 0);
  	cs.y = AfxGetApp()->GetProfileInt("Location", "y", 0);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::DestroyWindow() 
{
	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnClose() 
{
	SaveBarState("Toolbars");
	
	CFrameWnd::OnClose();
}

void CMainFrame::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);

	if (AfxGetApp()->m_pMainWnd)
	{
		// save the window placement if the window is not minimized or maximized
		WINDOWPLACEMENT wndpl;
		GetWindowPlacement(&wndpl);
		if (SW_SHOWNORMAL == wndpl.showCmd)
		{
			RECT rect;
			AfxGetApp()->m_pMainWnd->GetWindowRect(&rect);
			AfxGetApp()->WriteProfileInt("Location", "x", rect.left);
			AfxGetApp()->WriteProfileInt("Location", "y", rect.top);
		}
	}
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == rashDoneMsg)
	{
		OnClose();
		return 0;
	}
	
	return CFrameWnd::WindowProc(message, wParam, lParam);
}
