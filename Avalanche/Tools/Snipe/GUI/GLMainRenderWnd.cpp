// GLMainRenderWnd.cpp : implementation of the CGLMainRenderWnd class
//

#include "stdafx.h"
#include "GLMainRenderWnd.h"
#include "Objects/Project.h"
#include "Snipe.h"
#include "BaseDoc.h"
#include "GLBaseFrame.h"
#include <afxpriv.h>
#include "resource.h"

// CGLMainRenderWnd
BEGIN_MESSAGE_MAP(CGLMainRenderWnd, CGLRenderWnd)
   ON_WM_CREATE()
   ON_WM_DESTROY()
   ON_WM_SIZE()
   ON_WM_MOUSEMOVE()
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()
   ON_WM_SYSKEYDOWN()
   ON_WM_SYSKEYUP()
   ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

CGLMainRenderWnd::CGLMainRenderWnd()
{
}

CGLMainRenderWnd::~CGLMainRenderWnd()
{
}

int CGLMainRenderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CGLRenderWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_glBackground.Init( NULL, this );
   m_glBackground.LoadBitmap( IDB_BACKGROUND );
//   m_glMenuBar.Init( &m_glBackground );
   m_glToolBar.Init( &m_glBackground );
   m_glToolBar.CreateFromResource( ID_VIEW_STANDARDBAR, IDS_TITLE_STANDARDBAR);
   m_glComponentCombo.Init( &m_glBackground );
   m_glManipulatorCombo.Init( &m_glBackground, NULL, CPoint(0,0), CSize(0,0), IDR_MANIPULATOR_COMBO );
   m_glToolBarContainer.Init( &m_glBackground );
   m_glExplorerSplitter.Init( &m_glBackground );
   m_glExplorerSplitter.m_bFixedSize = TRUE;
   m_glExplorerSplitter.SetSize( 2, 1 );
   m_glExplorerSplitter.SetHandleSize( 9 );
   m_glExplorerSplitter.m_colwidtharray[0] = GetApp().GetProfileInt("ToolBars", "Explorer Width", 360);
   m_glExplorerSplitter.m_colwidtharray[1] = 0.0f;
    m_glPanelBar.Init( &m_glExplorerSplitter );
    m_glTimeLineSplitter.Init( &m_glExplorerSplitter );
    m_glTimeLineSplitter.m_bFixedSize = TRUE;
    m_glTimeLineSplitter.SetSize( 1, 2 );
    m_glTimeLineSplitter.SetHandleSize( 9 );
    m_glTimeLineSplitter.m_rowheightarray[0] = 0.0f;
    m_glTimeLineSplitter.m_rowheightarray[1] = GetApp().GetProfileInt("ToolBars", "Timeline Height", 200);
     m_glDocumentBar.Init( &m_glTimeLineSplitter );
     m_glTimeLineBar.Init( &m_glTimeLineSplitter );
   m_glStatusBarControl.Init( &m_glBackground );
    m_glStatusPaneControl.Init( &m_glStatusBarControl );

    m_glModStatusPaneControl.Init( &m_glStatusBarControl );
    m_glModStatusPaneControl.SetVisible( FALSE );
    m_glModStatusPaneControl.SetIcon( IDB_ALLMODIFIERS, 16, 0 );

    m_glMouseStatusPaneControl[MOUSESTATUSPANE_LEFT].Init( &m_glStatusBarControl );
    m_glMouseStatusPaneControl[MOUSESTATUSPANE_LEFT].SetVisible( FALSE );
    m_glMouseStatusPaneControl[MOUSESTATUSPANE_LEFT].SetIcon( IDB_MOUSE, 16, 1 );

    m_glMouseStatusPaneControl[MOUSESTATUSPANE_MIDDLE].Init( &m_glStatusBarControl );
    m_glMouseStatusPaneControl[MOUSESTATUSPANE_MIDDLE].SetVisible( FALSE );
    m_glMouseStatusPaneControl[MOUSESTATUSPANE_MIDDLE].SetIcon( IDB_MOUSE, 16, 2 );

    m_glMouseStatusPaneControl[MOUSESTATUSPANE_RIGHT].Init( &m_glStatusBarControl );
    m_glMouseStatusPaneControl[MOUSESTATUSPANE_RIGHT].SetVisible( FALSE );
    m_glMouseStatusPaneControl[MOUSESTATUSPANE_RIGHT].SetIcon( IDB_MOUSE, 16, 3 );

    m_glMouseStatusPaneControl[MOUSESTATUSPANE_WHEEL].Init( &m_glStatusBarControl );
    m_glMouseStatusPaneControl[MOUSESTATUSPANE_WHEEL].SetVisible( FALSE );
    m_glMouseStatusPaneControl[MOUSESTATUSPANE_WHEEL].SetIcon( IDB_MOUSE, 16, 5 );

   return 0;
}

void CGLMainRenderWnd::OnDestroy()
{
   GetApp().WriteProfileInt("ToolBars", "Explorer Width", m_glExplorerSplitter.m_colwidtharray[0]);
   GetApp().WriteProfileInt("ToolBars", "Timeline Height", m_glTimeLineSplitter.m_rowheightarray[1]);

   CGLRenderWnd::OnDestroy();
}

void CGLMainRenderWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

   CPoint pt(0,0);

   m_glBackground.Move( pt, CSize( cx, cy ));

//   m_glMenuBar.Move( pt, CSize(cx, 16));
//   pt.y += m_glMenuBar.m_szControl.cy;

   m_glToolBar.Move( CPoint(pt.x+4, pt.y+4), CSize(min(m_glToolBar.GetMinWidth(),cx-8), 34-8));
   m_glComponentCombo.Move( CPoint(m_glToolBar.m_ptControl.x+m_glToolBar.m_szControl.cx+8, pt.y+7), CSize(144, 22));
   m_glManipulatorCombo.Move( CPoint(m_glComponentCombo.m_ptControl.x+m_glComponentCombo.m_szControl.cx+8, pt.y+7), CSize(144, 22));
   pt.y += m_glToolBar.m_szControl.cy + 8;

   m_glToolBarContainer.Move( CPoint(pt.x+4, pt.y), CSize(cx-8, 51));
   pt.y += m_glToolBarContainer.m_szControl.cy + 4;

   m_glExplorerSplitter.Move( pt, CSize(cx, cy - pt.y - 23));
   pt.y += m_glExplorerSplitter.m_szControl.cy;

   m_glStatusBarControl.Move( pt, CSize(cx, 23));
}

void CGLMainRenderWnd::OnIdleUpdateCmdUI()
{
   m_glToolBar.OnUpdateCmdUI((CFrameWnd *)AfxGetMainWnd(), TRUE);
   m_glComponentCombo.OnUpdateCmdUI((CFrameWnd *)AfxGetMainWnd(), TRUE);
   m_glManipulatorCombo.OnUpdateCmdUI((CFrameWnd *)AfxGetMainWnd(), TRUE);
   m_glToolBarContainer.OnUpdateCmdUI((CFrameWnd *)AfxGetMainWnd(), TRUE);
   m_glTimeLineBar.m_glHistoryControl.OnUpdateCmdUI((CFrameWnd *)AfxGetMainWnd(), TRUE);
   m_glTimeLineBar.m_glOutputFrame.m_glOutputControl.OnUpdateCmdUI((CFrameWnd *)AfxGetMainWnd(), TRUE);
   m_glTimeLineBar.m_glOutputFrame.m_glCommandLineControl.OnUpdateCmdUI((CFrameWnd *)AfxGetMainWnd(), TRUE);
   m_glTimeLineBar.m_glTransportControl.OnUpdateCmdUI((CFrameWnd *)AfxGetMainWnd(), TRUE);
   DrawInvalidControls();
}

BOOL CGLMainRenderWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
   if (CGLRenderWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
      return TRUE;

   CGLBaseView *pCurrentView = GetMainInterface()->GetCurrentView();

   if ( pCurrentView && m_pActiveControl != pCurrentView )
   {
      if (pCurrentView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
         return TRUE;
      if (pCurrentView->GetDocument()->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ))
         return TRUE;
   }
   
   return FALSE;
}

/////////////////////////////////////////////////
// CGLSharedRenderWnd

BEGIN_MESSAGE_MAP(CGLSharedRenderWnd, CGLRenderWnd)
   ON_WM_CREATE()
   ON_WM_DESTROY()
END_MESSAGE_MAP()

CGLSharedRenderWnd::CGLSharedRenderWnd()
{
#ifdef USEFREETYPE
   memset( &m_font, 0, sizeof(freetype::font_data));
#endif
#ifdef USEFTGL
   m_pFont = NULL;
#endif
}

CGLSharedRenderWnd::~CGLSharedRenderWnd()
{
}

int CGLSharedRenderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CGLRenderWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   POSITION pos = GetApp().m_glRenderWndList.Find( this );
   if (pos)
      GetApp().m_glRenderWndList.RemoveAt( pos );

   m_glrcShared = m_glrc;

   CDC *pDC = GetDC();
   HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
   if (hFont == NULL)
      hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
   HFONT hOldFont = (HFONT)pDC->SelectObject(hFont);
   BOOL rvalue = wglUseFontBitmaps( pDC->GetSafeHdc(), 0, 255, ALPHADISPLAYLISTBASE );
   pDC->SelectObject(hOldFont);
   ReleaseDC( pDC );
#ifdef USEFREETYPE
   // build font
   try {
      m_font.init("Snipe.ttf", 9);					    //Build the freetype font
	}
   catch (std::exception &e) {
		AfxMessageBox( e.what(),/*"CAUGHT AN EXCEPTION",*/MB_OK | MB_ICONINFORMATION);
	}
#endif
#ifdef USEFTGL
//   m_pFont = new FTGLOutlineFont( "Snipe.ttf" );
//   m_pFont = new FTGLPolygonFont( "Snipe.ttf" );
//   m_pFont = new FTGLTextureFont( "Snipe.ttf" );
//   m_pFont = new FTGLBitmapFont( "Snipe.ttf" );
   m_pFont = SNEW FTGLPixmapFont( "Snipe.ttf" );
   if (!m_pFont->FaceSize( 12 ))
      return -1;
#endif

   return 0;
}

void CGLSharedRenderWnd::OnDestroy()
{
#ifdef USEFREETYPE
	m_font.clean();
#endif
#ifdef USEFTFL
   if (m_pFont)
   {
      delete m_pFont;
      m_pFont = NULL;
   }
#endif
   CGLRenderWnd::OnDestroy();
}

BOOL CGLSharedRenderWnd::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style &= ~WS_VISIBLE;

   return CGLRenderWnd::PreCreateWindow(cs);
}

void CGLMainRenderWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (nChar==VK_SHIFT || nChar==VK_CONTROL || nChar==VK_MENU)
   {
      OnSetCursor( this, 0, 0 );
      RefreshStatusBarInstructions();
   }

   CGLRenderWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGLMainRenderWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (nChar==VK_SHIFT || nChar==VK_CONTROL || nChar==VK_MENU)
   {
      OnSetCursor( this, 0, 0 );
      RefreshStatusBarInstructions();
   }

   CGLRenderWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CGLMainRenderWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (nChar==VK_SHIFT || nChar==VK_CONTROL || nChar==VK_MENU)
   {
      OnSetCursor( this, 0, 0 );
      RefreshStatusBarInstructions();
      return;
   }

   CGLRenderWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CGLMainRenderWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (nChar==VK_SHIFT || nChar==VK_CONTROL || nChar==VK_MENU)
   {
      OnSetCursor( this, 0, 0 );
      RefreshStatusBarInstructions();
      return;
   }

   CGLRenderWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CGLMainRenderWnd::RefreshStatusBarInstructions()
{
   CPoint point;
   GetCursorPos( &point );
   ScreenToClient( &point );

   CGLControl *pControl = HitTestControls( point );
   if (pControl && pControl->IsKindOf(RUNTIME_CLASS(CGLBaseView)))
   {
      pControl->ClientToControl( point );
      ((CGLBaseView *)pControl)->RefreshStatusBarInstructions(TRUE);
   }
}
