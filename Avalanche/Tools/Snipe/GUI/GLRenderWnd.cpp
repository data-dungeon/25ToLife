// GLRenderWnd.cpp : implementation of the CGLRenderWnd class
//

#include "stdafx.h"
#include "Snipe.h"
#include "GLRenderWnd.h"
#include "GLControls.h"
#include "Objects/Scene.h"

PFNGLADDSWAPHINTRECTWINPROC glAddSwapHintRectWIN;

static BOOL IsExtSupported( const char *extension )
{
   GLubyte *extensions = NULL;
   const GLubyte *start;
   GLubyte *where, *terminator;

   where = (GLubyte *) strchr(extension, ' ');
   if (where || *extension == '\0')
      return FALSE;

   extensions = (GLubyte *)glGetString(GL_EXTENSIONS);

   start = extensions;
   while (TRUE) 
   {
      where = (GLubyte *) strstr((const char *) start, extension);

      if (!where)
         break;

      terminator = where + strlen(extension);

      if (where == start || *(where - 1) == ' ') 
      {
         if (*terminator == ' ' || *terminator == '\0') 
            return TRUE;
      }
      start = terminator;
   }

   return FALSE;
}

static void *GetExtensionPointer( const char *szExtensionName )
{
   void *pfnExtension = (void *)wglGetProcAddress(szExtensionName);

   if (!pfnExtension)
      AfxMessageBox( "OpenGL extension not supported.  " + (CString)szExtensionName );

   return pfnExtension;
}

void InitGLExtensions()
{
   const GLubyte *version = glGetString(GL_VERSION);
   GLfloat glVersion = atof( (char *)version );

   glAddSwapHintRectWIN = (PFNGLADDSWAPHINTRECTWINPROC)GetExtensionPointer("glAddSwapHintRectWIN");
}

////////////////////////////////////////////////////
// GLContextPusher

GLContextPusher::GLContextPusher( CGLRenderWnd *glwnd )
{
	m_hOldGLRC = wglGetCurrentContext();
	m_hOldHDC = wglGetCurrentDC();

   glwnd->MakeRenderContextCurrent();
}

GLContextPusher::~GLContextPusher()
{
   wglMakeCurrent( m_hOldHDC, m_hOldGLRC );
   CGLRenderWnd::m_glrcCurrent = m_hOldGLRC;
}

////////////////////////////////////////////////////
// CGLRenderWnd

HGLRC CGLRenderWnd::m_glrcShared;
HGLRC CGLRenderWnd::m_glrcCurrent;

BEGIN_MESSAGE_MAP(CGLRenderWnd, CWnd)
	// Standard printing commands
//	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()
   ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
   ON_WM_SETFOCUS()
   ON_WM_KILLFOCUS()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDOWN()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_MBUTTONUP()
   ON_WM_RBUTTONDOWN()
   ON_WM_RBUTTONDBLCLK()
   ON_WM_RBUTTONUP()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSEWHEEL()
   ON_WM_SETCURSOR()
   ON_WM_MOVE()
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()
END_MESSAGE_MAP()

// CGLRenderWnd construction/destruction

CGLRenderWnd::CGLRenderWnd()
{
	m_glrc = NULL;
   m_pActiveControl = NULL;
   m_pMouseOverControl = NULL;
}

CGLRenderWnd::~CGLRenderWnd()
{
}

#ifdef _DEBUG
static void DumpTree( CDumpContext& dc, CGLControl *pControl, CString indent )
{
   if (pControl) {
      pControl->Dump( dc );

      for (int i=0; i<pControl->m_childcontrolarray.GetCount(); i++) {
         CGLControl *pChildControl = pControl->m_childcontrolarray[i];
         dc << indent;
         DumpTree( dc, pChildControl, indent + "  " );
      }
   }
   else
      dc << (void *)NULL << "\n";
}

void CGLRenderWnd::Dump(CDumpContext& dc) const
{
   CWnd::Dump(dc);

   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      DumpTree( dc, m_childcontrolarray[i], "  " );
}
#endif

int CGLRenderWnd::AddControl( CGLControl *pControl )
{
   return m_childcontrolarray.Add( pControl );
}

BOOL CGLRenderWnd::RemoveControl( CGLControl *pControl )
{
   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      if (m_childcontrolarray[i] == pControl)
         return RemoveControlAt( i );

   return FALSE;
}

BOOL CGLRenderWnd::RemoveControlAt( int nIndex )
{
   if (nIndex < 0 || nIndex >= m_childcontrolarray.GetCount())
      return FALSE;

   m_childcontrolarray.RemoveAt( nIndex );

   return TRUE;
}

void CGLRenderWnd::AddInvalidControl( CGLControl *pControl )
{
   if (pControl->m_bInvalid)
      return;

   m_invalidcontrollist.AddTail( pControl );
   pControl->m_bInvalid = TRUE;
}

void CGLRenderWnd::RemoveInvalidControl( CGLControl *pControl )
{
   if (!pControl->m_bInvalid)
      return;

   if (m_bDrawingInvalidControls)
      return;

   POSITION pos = m_invalidcontrollist.Find( pControl );
   if (pos)
      m_invalidcontrollist.RemoveAt( pos );
   pControl->m_bInvalid = FALSE;
}

void CGLRenderWnd::DrawInvalidControls()
{
   if (m_invalidcontrollist.IsEmpty())
      return;

#ifdef DRAWDIAGNOSTICS
   g_outputWindow << GetRuntimeClass()->m_lpszClassName << " has " << m_invalidcontrollist.GetCount() << " invalid controls.\n";
#endif

   int nOldCount = m_invalidcontrollist.GetCount();

   m_bDrawingInvalidControls = TRUE;

   for (POSITION pos=m_invalidcontrollist.GetHeadPosition(); pos; ) {
      CGLControl *pControl = m_invalidcontrollist.GetAt( pos );
      if (pControl->IsThisVisible())
         pControl->Draw();
      pControl->m_bInvalid = FALSE;
      m_invalidcontrollist.GetNext( pos );
   }

   if (m_childcontrolarray.GetCount() > 1) // we have popups, they have to be drawn regardless
   {
      for (int i=1; i<m_childcontrolarray.GetCount(); i++)
      {
         CGLControl *pControl = m_childcontrolarray[i];
         if (!m_invalidcontrollist.Find(pControl) && pControl->IsThisVisible())
            pControl->Draw();
      }
   }

   m_bDrawingInvalidControls = FALSE;

   m_invalidcontrollist.RemoveAll();

   Update();
}

CGLControl *CGLRenderWnd::SetActiveControl( CGLControl *pControl )
{
   CGLControl *pPrevActive = m_pActiveControl;
   if (pControl && GetFocus() != this)
      SetFocus();

   if (pPrevActive != pControl) {
      m_pActiveControl = pControl;

      if (pPrevActive)
         pPrevActive->OnActivate( CGLControl::ACTIVATION_INACTIVE, pControl );

      if (pControl)
         pControl->OnActivate( CGLControl::ACTIVATION_ACTIVE, pPrevActive );
   }
   return pPrevActive;
}

void CGLRenderWnd::Update()
{
   MakeRenderContextCurrent();

   CDC *pDC = GetDC();
   SwapBuffers( *pDC );
   ReleaseDC( pDC );
}

BOOL CGLRenderWnd::Create( CWnd *pParent )
{
   return CWnd::Create( NULL, NULL, WS_CHILD|WS_VISIBLE|WS_BORDER, CRect(0,0,1,1), pParent, AFX_IDW_PANE_FIRST );
}

BOOL CGLRenderWnd::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW)); 
	return CWnd::PreCreateWindow(cs);
}

int CGLRenderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

   EnableToolTips();

	CDC *pDC = GetDC();
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd 
		1,                     // version number 
		PFD_DRAW_TO_WINDOW |   // support window 
		PFD_SUPPORT_OPENGL |   // support OpenGL 
		PFD_DOUBLEBUFFER |     // double buffered 
      PFD_SWAP_COPY,         // copy on swap
		PFD_TYPE_RGBA,         // RGBA type 
		32,                    // 24-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		0,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // 32-bit z-buffer     
		0,                     // no stencil buffer 
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
		}; 

	int iPixelFormat = ChoosePixelFormat(*pDC, &pfd);
	if (!iPixelFormat)
		return -1;

	if (!SetPixelFormat( *pDC, iPixelFormat, &pfd ))
		return -1;

	m_glrc = wglCreateContext( *pDC );
	if (!m_glrc)
		return -1;

   if (m_glrcShared)
      if (!wglShareLists( m_glrcShared, m_glrc ))
         return -1;

   MakeRenderContextCurrent();
   InitGLExtensions();

	glShadeModel(GL_SMOOTH);						// Enables Smooth Shading

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					// Black Background

	glClearDepth( 100.0f );							// Depth Buffer Setup
	glEnable( GL_DEPTH_TEST );						// Enables Depth Testing
	glDepthFunc( GL_LEQUAL );						// The Type Of Depth Test To Do

   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   glEnable( GL_LINE_SMOOTH );

   glEnable( GL_SCISSOR_TEST );

   glEnable( GL_POLYGON_OFFSET_FILL );
   glPolygonOffset( 1.0f, 1.0f );

//   glEnable( GL_COLOR_MATERIAL );
//	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

// Lighting components
   glDisable( GL_LIGHTING );
   glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, TRUE );
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE );
   glEnable( GL_NORMALIZE );

   glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );			// Really Nice Perspective Calculations

   ReleaseDC( pDC );

   if (AfxGetApp())
      GetApp().m_glRenderWndList.AddTail( this );

   return 0;
}

void CGLRenderWnd::OnDestroy()
{
   POSITION pos = GetApp().m_glRenderWndList.Find( this );
   if (pos)
      GetApp().m_glRenderWndList.RemoveAt( pos );

   wglDeleteContext( m_glrc );
	m_glrc = NULL;

   CWnd::OnDestroy();
}

// CGLRenderWnd drawing

BOOL CGLRenderWnd::OnEraseBkgnd(CDC* pDC)
{
	// Fool Windows into thinking that we cleared to remove flicker

	return TRUE;
}
void CGLRenderWnd::MakeRenderContextCurrent()
{
   if (m_glrcCurrent == m_glrc)
      return;

	CDC *pDC = GetDC();
	wglMakeCurrent( *pDC, m_glrc );
	ReleaseDC( pDC );
   
   m_glrcCurrent = m_glrc;
}

void CGLRenderWnd::OnPaint()
{
   CPaintDC dc(this);

	MakeRenderContextCurrent();

   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
   {
      CGLControl *pChild = m_childcontrolarray[i];
      if (pChild && pChild->IsThisVisible())
         pChild->Draw( CGLControl::DRAW_CHILDREN );
   }

	CDC *pDC = GetDC();
   SwapBuffers( *pDC );
	ReleaseDC( pDC );
}

// CGLRenderWnd message handlers

void CGLRenderWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

   if (m_childcontrolarray.GetCount())
      m_childcontrolarray[0]->Move( CPoint(0,0), CSize(cx,cy));
}

CGLControl *CGLRenderWnd::HitTestControls( const CPoint &point ) const
{
   for (int i=m_childcontrolarray.GetCount()-1; i>=0; i--)
   {
      CGLControl *pChild = m_childcontrolarray[i];
      if (pChild && pChild->IsThisVisible()/* && pChild->m_bEnabled*/)
      {
         CGLControl *pHit = pChild->HitTestAll( point );
         if (pHit)
            return pHit;
      }
   }

   return NULL;
}

void CGLRenderWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   SetFocus();

   if (!CGLControl::m_pCaptureControl)
   {
      CGLControl::m_pCaptureControl = HitTestControls( point );
      if (!CGLControl::m_pCaptureControl->IsEnabled())
         CGLControl::m_pCaptureControl = NULL;
   }

   if (CGLControl::m_pCaptureControl)
   {
      SetCapture();
      CGLControl::m_pCaptureControl->ClientToControl( point );
      CGLControl::m_pCaptureControl->OnLButtonDown( nFlags, point );
      if (CGLControl::m_pCaptureControl && !CGLControl::m_pCaptureControl->OnSetCursor( WM_LBUTTONDOWN, point ))
         ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   }
}

void CGLRenderWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   SetFocus();

   if (!CGLControl::m_pCaptureControl)
   {
      CGLControl::m_pCaptureControl = HitTestControls( point );
      if (!CGLControl::m_pCaptureControl->IsEnabled())
         CGLControl::m_pCaptureControl = NULL;
   }

   if (CGLControl::m_pCaptureControl)
   {
      SetCapture();
      CGLControl::m_pCaptureControl->ClientToControl( point );
      CGLControl::m_pCaptureControl->OnLButtonDblClk( nFlags, point );
   }
}

void CGLRenderWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   CGLControl *pControl = CGLControl::m_pCaptureControl;
   if (pControl)
   {
      if (!(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
         CGLControl::m_pCaptureControl = NULL;
      pControl->ClientToControl( point );
      pControl->OnLButtonUp( nFlags, point );
      if ((!CGLControl::m_pCaptureControl || pControl==CGLControl::m_pCaptureControl) && !(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
         ::ReleaseCapture();
   }
}

void CGLRenderWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   SetFocus();

   if (!CGLControl::m_pCaptureControl)
   {
      CGLControl::m_pCaptureControl = HitTestControls( point );
      if (!CGLControl::m_pCaptureControl->IsEnabled())
         CGLControl::m_pCaptureControl = NULL;
   }

   if (CGLControl::m_pCaptureControl)
   {
      SetCapture();
      CGLControl::m_pCaptureControl->ClientToControl( point );
      CGLControl::m_pCaptureControl->OnMButtonDown( nFlags, point );
      if (CGLControl::m_pCaptureControl && !CGLControl::m_pCaptureControl->OnSetCursor( WM_MBUTTONDOWN, point ))
         ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   }
}

void CGLRenderWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   SetFocus();

   if (!CGLControl::m_pCaptureControl)
   {
      CGLControl::m_pCaptureControl = HitTestControls( point );
      if (!CGLControl::m_pCaptureControl->IsEnabled())
         CGLControl::m_pCaptureControl = NULL;
   }

   if (CGLControl::m_pCaptureControl)
   {
      SetCapture();
      CGLControl::m_pCaptureControl->ClientToControl( point );
      CGLControl::m_pCaptureControl->OnMButtonDblClk( nFlags, point );
   }
}

void CGLRenderWnd::OnMButtonUp(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   CGLControl *pControl = CGLControl::m_pCaptureControl;
   if (pControl)
   {
      if (!(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
         CGLControl::m_pCaptureControl = NULL;
      pControl->ClientToControl( point );
      pControl->OnMButtonUp( nFlags, point );
      if (!CGLControl::m_pCaptureControl && !(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
         ::ReleaseCapture();
   }
}

void CGLRenderWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
   CWnd::OnRButtonDown( nFlags, point );

   if (IsDownAlt())
      nFlags |= MK_ALT;

   SetFocus();

   if (!CGLControl::m_pCaptureControl)
   {
      CGLControl::m_pCaptureControl = HitTestControls( point );
      if (!CGLControl::m_pCaptureControl->IsEnabled())
         CGLControl::m_pCaptureControl = NULL;
   }

   if (CGLControl::m_pCaptureControl)
   {
      SetCapture();
      CGLControl::m_pCaptureControl->ClientToControl( point );
      CGLControl::m_pCaptureControl->OnRButtonDown( nFlags, point );
      if (CGLControl::m_pCaptureControl && !CGLControl::m_pCaptureControl->OnSetCursor( WM_RBUTTONDOWN, point ))
         ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   }
}

void CGLRenderWnd::OnRButtonDblClk(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   SetFocus();

   if (!CGLControl::m_pCaptureControl)
   {
      CGLControl::m_pCaptureControl = HitTestControls( point );
      if (!CGLControl::m_pCaptureControl->IsEnabled())
         CGLControl::m_pCaptureControl = NULL;
   }

   if (CGLControl::m_pCaptureControl)
   {
      SetCapture();
      CGLControl::m_pCaptureControl->ClientToControl( point );
      CGLControl::m_pCaptureControl->OnRButtonDblClk( nFlags, point );
   }
}

void CGLRenderWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   BOOL rValue = FALSE;
   CGLControl *pControl = CGLControl::m_pCaptureControl;
   if (pControl)
   {
      if (!(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
         CGLControl::m_pCaptureControl = NULL;

      CPoint ptControl = point;
      pControl->ClientToControl( ptControl );
      if (!pControl->OnRButtonUp( nFlags, ptControl ))
      {
         // Context Menu
         ClientToScreen( &point );
         pControl->OnContextMenu( point );
      }
      if ((!CGLControl::m_pCaptureControl || pControl==CGLControl::m_pCaptureControl) && !(nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
         ::ReleaseCapture();
   }
}

void CGLRenderWnd::OnMouseMove(UINT nFlags, CPoint point)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   if (CGLControl::m_pCaptureControl)
   {
      CGLControl::m_pCaptureControl->ClientToControl( point );
      CGLControl::m_pCaptureControl->OnMouseMove( nFlags, point );
      if (!CGLControl::m_pCaptureControl->OnSetCursor( WM_MOUSEMOVE, point ))
         ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   }
   else
   {
      CGLControl *pControl = HitTestControls( point );
      if (!pControl->IsEnabled())
         pControl = NULL;

      if (pControl != m_pMouseOverControl)
      {
         if (m_pMouseOverControl)
            m_pMouseOverControl->OnMouseLeave();

         m_pMouseOverControl = pControl;

         if (m_pMouseOverControl)
            m_pMouseOverControl->OnMouseEnter();
      }

      if (pControl)
      {
         pControl->ClientToControl( point );
         pControl->OnMouseMove( nFlags, point );
      }
   }
}

BOOL CGLRenderWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
   if (IsDownAlt())
      nFlags |= MK_ALT;

   ScreenToClient( &pt );

   if (CGLControl::m_pCaptureControl)
      return CGLControl::m_pCaptureControl->OnMouseWheel( nFlags, zDelta, pt );
   else {
      CGLControl *pControl = HitTestControls( pt );
      if (!pControl->IsEnabled())
         pControl = NULL;
      if (pControl)
         return pControl->OnMouseWheel( nFlags, zDelta, pt );
      else if (m_pActiveControl)
         return m_pActiveControl->OnMouseWheel( nFlags, zDelta, pt );
   }
   return FALSE;
}

void CGLRenderWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (CGLControl::m_pCaptureControl && CGLControl::m_pCaptureControl->OnKeyDown( nChar, nRepCnt, nFlags ))
      return;
   else if (m_pActiveControl && m_pActiveControl->OnKeyDown( nChar, nRepCnt, nFlags ))
      return;
   else
      CWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}

void CGLRenderWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (CGLControl::m_pCaptureControl && CGLControl::m_pCaptureControl->OnKeyUp( nChar, nRepCnt, nFlags ))
      return;
   else if (m_pActiveControl && m_pActiveControl->OnKeyUp( nChar, nRepCnt, nFlags ))
      return;
   else
      CWnd::OnKeyUp( nChar, nRepCnt, nFlags );
}

void CGLRenderWnd::OnSetFocus(CWnd* pOldWnd)
{
   CWnd::OnSetFocus(pOldWnd);

//   Invalidate( FALSE ); // overkill
}

void CGLRenderWnd::OnKillFocus(CWnd* pNewWnd)
{
   CWnd::OnKillFocus(pNewWnd);

//   Invalidate( FALSE ); // overkill
}

BOOL CGLRenderWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
   if (CGLControl::m_pContextMenuControl)
   {
      if (CGLControl::m_pContextMenuControl->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ))
         return TRUE;
   }
   if (m_pActiveControl && m_pActiveControl->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ))
      return TRUE;

   return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CGLRenderWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   CPoint point;
   GetCursorPos( &point );
   ScreenToClient( &point );

   if (CGLControl::m_pCaptureControl)
   {
      CPoint ptControl = point;
      CGLControl::m_pCaptureControl->ClientToControl( ptControl );

      if (CGLControl::m_pCaptureControl->OnSetCursor( message, ptControl ))
         return TRUE;
      else 
      {
         ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
         return TRUE;
      }
   }

   CGLControl *pControl = HitTestControls( point );
   if (!pControl->IsEnabled())
      pControl = NULL;
   if (pControl)
   {
      pControl->ClientToControl( point );
      if (pControl->OnSetCursor( message, point ))
         return TRUE;
   }

   ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   return TRUE;
}

void CGLRenderWnd::OnEditPaste()
{
#ifdef _DEBUG
   Dump( afxDump );
#endif
}

INT_PTR CGLRenderWnd::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
   static UINT uPrevID;
   static String strPrevText;
   String strText;

   CGLControl *pControl = HitTestControls( point );
   if (!pControl)
      return CWnd::OnToolHitTest(point, pTI);

   CPoint ptControl = point;
   pControl->ClientToControl( ptControl );
   UINT uID = pControl->GetToolTipText( ptControl, strText );

   if (uID==uPrevID && strText==strPrevText)
      return uID;
   uPrevID = uID;
   strPrevText = strText;

   if (uID==-1 || strText.IsEmpty())
      return -1;

   pTI->lpszText = _tcsdup(strText.Get());

   pTI->hwnd = m_hWnd;
   pTI->uId = uID;
   pTI->uFlags = 0;

   CRect rcControl = pControl->GetViewportRect();
   pTI->rect = rcControl;

   AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
   CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;

   pToolTip->SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
   pToolTip->SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
   pToolTip->SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 600);
   pToolTip->SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 600);

   return uID;
}

void CGLRenderWnd::OnMove(int x, int y)
{
   CWnd::OnMove(x, y);

//   Invalidate( FALSE );
}
