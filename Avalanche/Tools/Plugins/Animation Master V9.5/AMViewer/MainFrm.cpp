// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AMViewer.h"
#include "MainFrm.h"
#include "ScriptView.h"
#include "AMViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _MAC
   #define PANEWIDTH 224
#else
   #define PANEWIDTH 181
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

CMainFrame *g_mainframe;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
   ID_RENDEREDFPS,
   ID_ACTIONFRAME,
   ID_SCRIPTFRAME,
   ID_SPLITS,
	ID_PATCHCOUNT,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
   g_mainframe = this;
   m_modelview = NULL;
   m_scriptview = NULL;
}

CMainFrame::~CMainFrame()
{
   g_mainframe = NULL;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
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

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFrameWnd::PreCreateWindow(cs);
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

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CScriptView), CSize(PANEWIDTH, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CAMViewerView), CSize(100, 100), pContext))	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

   m_wndSplitter.SetColumnInfo( 0, AfxGetApp()->GetProfileInt( "General", "Script Pane Width", PANEWIDTH ), PANEWIDTH );

   m_scriptview = (CScriptView *)m_wndSplitter.GetPane(0,0);
   m_modelview = (CAMViewerView *)m_wndSplitter.GetPane(0,1);

   return TRUE;
}

void CMainFrame::OnClose() 
{
   int width, min;
   m_wndSplitter.GetColumnInfo( 0, width, min );
   AfxGetApp()->WriteProfileInt( "General", "Script Pane Width", width );
	
	CFrameWnd::OnClose();
}

void CMainFrame::SetStatusMessage(const CString &text, BOOL bupdate)
{ 
   m_wndStatusBar.SetPaneText(0, text); 
   if (bupdate)
      m_wndStatusBar.UpdateWindow();
}

void CMainFrame::SetRenderedFPS(const CString &text, BOOL bupdate)
{ 
   m_wndStatusBar.SetPaneText(SI_RENDEREDFPS, text); 
   if (bupdate)
      m_wndStatusBar.UpdateWindow();
}

void CMainFrame::SetActionFrame(const CString &text, BOOL bupdate)
{ 
   m_wndStatusBar.SetPaneText(SI_ACTIONFRAME, text); 
   if (bupdate)
      m_wndStatusBar.UpdateWindow();
}

void CMainFrame::SetScriptFrame(const CString &text, BOOL bupdate)
{ 
   m_wndStatusBar.SetPaneText(SI_SCRIPTFRAME, text); 
   if (bupdate)
      m_wndStatusBar.UpdateWindow();
}

void CMainFrame::SetPatchPolyCount(const CString &text, BOOL bupdate)
{ 
   m_wndStatusBar.SetPaneText(SI_PATCHCOUNT, text); 
   if (bupdate)
      m_wndStatusBar.UpdateWindow();
}

void CMainFrame::SetSubLevel(const CString &text, BOOL bupdate)
{ 
   m_wndStatusBar.SetPaneText(SI_SUBLEVEL, text); 
   if (bupdate)
      m_wndStatusBar.UpdateWindow();
}

void CMainFrame::UpdateStatusBar()
{
   m_wndStatusBar.UpdateWindow();
}
