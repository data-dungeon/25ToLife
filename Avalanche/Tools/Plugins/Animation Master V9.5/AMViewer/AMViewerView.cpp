// Dan  4/11/00  \Dan85\AMViewer\AMViewerView.cpp
//

#include "stdafx.h"
#include "AMViewerDoc.h"
#include "AMViewer.h"
#include "AMViewerView.h"
#include "Model.h"
#include "MainFrm.h"
#include "Time.h"

#ifdef _MAC
#include "macname1.h"
#include "Events.h"
#include "macname2.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *g_mainframe;

class CAMViewerDoc;

IMPLEMENT_DYNCREATE(CAMViewerView, CView)

BEGIN_MESSAGE_MAP(CAMViewerView, CView)
	//{{AFX_MSG_MAP(CAMViewerView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
   ON_WM_QUERYNEWPALETTE()
   ON_WM_PALETTECHANGED()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
   ON_COMMAND(ID_MOVE, OnMove)
   ON_UPDATE_COMMAND_UI(ID_MOVE, OnUpdateMove)
   ON_COMMAND(ID_TURN, OnTurn)
   ON_UPDATE_COMMAND_UI(ID_TURN, OnUpdateTurn)
   ON_COMMAND(ID_ZOOM, OnZoom)
   ON_UPDATE_COMMAND_UI(ID_ZOOM, OnUpdateZoom)
	ON_WM_SETCURSOR()
   ON_COMMAND(ID_ZOOMFIT, OnZoomFit)
   ON_COMMAND(ID_FRONTVIEW, OnFrontView)
   ON_COMMAND(ID_BACKVIEW, OnBackView)
   ON_COMMAND(ID_LEFTVIEW, OnLeftView)
   ON_COMMAND(ID_RIGHTVIEW, OnRightView)
   ON_COMMAND(ID_TOPVIEW, OnTopView)
   ON_COMMAND(ID_BOTTOMVIEW, OnBottomView)
   ON_UPDATE_COMMAND_UI(ID_POLYMODE, OnUpdatePolyMode)
   ON_COMMAND(ID_POLYMODE, OnPolyMode)
   ON_UPDATE_COMMAND_UI(ID_SUBDIVIDEDMODE, OnUpdateSubdividedMode)
   ON_COMMAND(ID_SUBDIVIDEDMODE, OnSubdividedMode)
   ON_UPDATE_COMMAND_UI(ID_SHADED, OnUpdateShaded)
   ON_COMMAND(ID_SHADED, OnShaded)
   ON_UPDATE_COMMAND_UI(ID_WIREFRAME, OnUpdateWireframe)
   ON_COMMAND(ID_WIREFRAME, OnWireframe)
   ON_UPDATE_COMMAND_UI(ID_VERTICES, OnUpdateVertices)
   ON_COMMAND(ID_VERTICES, OnVertices)
   ON_UPDATE_COMMAND_UI(ID_TANGENTS, OnUpdateTangents)
   ON_COMMAND(ID_TANGENTS, OnTangents)
   ON_UPDATE_COMMAND_UI(ID_NORMALS, OnUpdateNormals)
   ON_COMMAND(ID_NORMALS, OnNormals)
   ON_UPDATE_COMMAND_UI(ID_MATERIALS, OnUpdateMaterials)
   ON_COMMAND(ID_MATERIALS, OnMaterials)
   ON_UPDATE_COMMAND_UI(ID_DECALS, OnUpdateDecals)
   ON_COMMAND(ID_DECALS, OnDecals)
   ON_UPDATE_COMMAND_UI(ID_PERSPECTIVE, OnUpdatePerspective)
   ON_COMMAND(ID_PERSPECTIVE, OnPerspective)
   ON_UPDATE_COMMAND_UI(ID_ORTHOGONAL, OnUpdateOrthogonal)
   ON_COMMAND(ID_ORTHOGONAL, OnOrthogonal)
   ON_COMMAND(ID_VIEW_OPTIONS, OnOptions)
   ON_UPDATE_COMMAND_UI(ID_PLAY, OnUpdatePlay)
   ON_COMMAND(ID_PLAY, OnPlay)
	ON_COMMAND(ID_NEXTFRAME, OnNextframe)
	ON_UPDATE_COMMAND_UI(ID_NEXTFRAME, OnUpdateNextframe)
	ON_COMMAND(ID_PREVFRAME, OnPrevframe)
	ON_UPDATE_COMMAND_UI(ID_PREVFRAME, OnUpdatePrevframe)
	ON_COMMAND(ID_LASTFRAME, OnLastframe)
	ON_COMMAND(ID_FIRSTFRAME, OnFirstframe)
	ON_COMMAND(ID_INCREASESUB, OnIncreaseSub)
	ON_UPDATE_COMMAND_UI(ID_INCREASESUB, OnUpdateIncreaseSub)
	ON_COMMAND(ID_DECREASESUB, OnDecreaseSub)
	ON_UPDATE_COMMAND_UI(ID_DECREASESUB, OnUpdateDecreaseSub)
	ON_COMMAND(ID_MAXSUB, OnMaxSub)
	ON_UPDATE_COMMAND_UI(ID_MAXSUB, OnUpdateMaxSub)
	ON_COMMAND(ID_MINSUB, OnMinSub)
	ON_UPDATE_COMMAND_UI(ID_MINSUB, OnUpdateMinSub)
   ON_COMMAND(ID_FIXSUB, OnFixSub)
   ON_UPDATE_COMMAND_UI(ID_FIXSUB, OnUpdateFixSub)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_LASTFRAME, OnUpdateNextframe)
	ON_UPDATE_COMMAND_UI(ID_FIRSTFRAME, OnUpdatePrevframe)
	ON_COMMAND(ID_SHOWBONES, OnShowBones)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMViewerView construction/destruction

void CAMViewerView::SetupForBlit()
{
#ifdef _MAC
   GWorldPtr oldGWorld;
   GDHandle oldGDevice;
   GrafPtr port;
   RGBColor white, black;
      
   GetPort(&port);
   GetGWorld( &oldGWorld,&oldGDevice );		// save the old port 
   white.red = white.green = white.blue = 65535;
   black.red = black.green = black.blue = 0;
   AGLDrawable drawable = aglGetDrawable(m_context);
   SetPort((GrafPtr)drawable);   
   SetGWorld( drawable, NULL );
   RGBForeColor( &black );
   RGBBackColor( &white );
   SetPort( port );
   SetGWorld( oldGWorld,oldGDevice );
#endif
}

CAMViewerView::CAMViewerView()
{
   m_hpalette = NULL;
   m_mode = TURN;
   m_cursorid = ID_TURN;
   m_rotate.Set(0.0f,0.0f,0.0f);
   m_zoom = 1.0f;
   m_offset.Set(0.0);
   m_bplay = FALSE;
   m_time.SetSeconds(0);
   m_bsubdivided = FALSE;

   m_bbilinear = AfxGetApp()->GetProfileInt("Options", "bilinear", 0);
   m_bskipframes = AfxGetApp()->GetProfileInt("Options", "skip frames", 1);

   m_bshaded = AfxGetApp()->GetProfileInt("Options", "show shaded", 1);
   m_bvertices = AfxGetApp()->GetProfileInt("Options", "show vertices", 0);
   m_btangents = AfxGetApp()->GetProfileInt("Options", "show tangents", 0);
   m_bnormals = AfxGetApp()->GetProfileInt("Options", "show normals", 0);
   m_bmaterials = AfxGetApp()->GetProfileInt("Options", "show materials", 1);
   m_bdecals = AfxGetApp()->GetProfileInt("Options", "show decals", 0);
   m_patchsplitter.m_isvariable = AfxGetApp()->GetProfileInt("Options", "variable sub", 1);

   CString temp = AfxGetApp()->GetProfileString("Options", "straight", "6");
   sscanf((LPCTSTR)temp, "%f", &m_patchsplitter.m_straight);

   temp = AfxGetApp()->GetProfileString("Options", "focallength", "35");
   sscanf((LPCTSTR)temp, "%f", &m_focallength);
   m_focaldistance = 200;

   m_patchsplitter.m_maxlevel = AfxGetApp()->GetProfileInt("Options", "maxlevel", 2);
   m_patchsplitter.m_view = this;

   m_bperspective = AfxGetApp()->GetProfileInt("Options", "perspective", 0);

   m_prevtime = 0;
}

CAMViewerView::~CAMViewerView()
{
   g_mainframe->m_modelview = NULL;

   AfxGetApp()->WriteProfileInt("Options", "show shaded", m_bshaded);
   AfxGetApp()->WriteProfileInt("Options", "show vertices", m_bvertices);
   AfxGetApp()->WriteProfileInt("Options", "show tangents", m_btangents);
   AfxGetApp()->WriteProfileInt("Options", "show normals", m_bnormals);
   AfxGetApp()->WriteProfileInt("Options", "show materials", m_bmaterials);
   AfxGetApp()->WriteProfileInt("Options", "show decals", m_bdecals);
   AfxGetApp()->WriteProfileInt("Options", "variable sub", m_patchsplitter.m_isvariable);
   
   CString temp;
   temp.Format("%g", m_patchsplitter.m_straight);
   AfxGetApp()->WriteProfileString("Options", "straight", temp);
   AfxGetApp()->WriteProfileInt("Options", "max level", m_patchsplitter.m_maxlevel);
   AfxGetApp()->WriteProfileInt("Options", "perspective", m_bperspective);

   temp.Format("%g", m_focallength);
   AfxGetApp()->WriteProfileString("Options", "focallength", temp);
}

BOOL CAMViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
   cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS,
    NULL); 

   return CView::PreCreateWindow(cs);
}

void CAMViewerView::OnDraw(CDC* pDC)
{
#ifdef _MAC
   DWORD starttime = TickCount();
   DWORD frequency = TICKSPERSEC;
#else
   LARGE_INTEGER starttime, endtime;
   QueryPerformanceCounter(&starttime);
#endif

   Render();
#ifdef _MAC
   SetupForBlit();
   aglSwapBuffers(m_context);
   endtime = TickCount();
   GetDocument()->m_elapse = endtime - starttime;
#else
   SwapBuffers(GetDC()->GetSafeHdc());// for some reason can't use m_dc here
   QueryPerformanceCounter(&endtime);
   GetDocument()->m_elapse.QuadPart = endtime.QuadPart - starttime.QuadPart;
#endif
   GetDocument()->SetStatusBarStats();
}

/////////////////////////////////////////////////////////////////////////////
// CAMViewerView diagnostics

#ifdef _DEBUG
void CAMViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CAMViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAMViewerDoc* CAMViewerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAMViewerDoc)));
	return (CAMViewerDoc*)m_pDocument;
}
#endif //_DEBUG

void CAMViewerView::SetOpenGLPalette()
{
	HPALETTE hRetPal = NULL;
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE *pPal;
	int nColors;
	int i;
	BYTE RedRange,GreenRange,BlueRange;
   CDC *dc = GetDC();
#ifdef _MAC
#else

	int nPixelFormat = GetPixelFormat(dc->GetSafeHdc());
	nPixelFormat = GetPixelFormat(dc->GetSafeHdc());
 	DescribePixelFormat(dc->GetSafeHdc(), nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
#endif

	if (!(pfd.dwFlags & PFD_NEED_PALETTE))
		return;

	nColors = 1 << pfd.cColorBits;	

	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +nColors*sizeof(PALETTEENTRY));
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	RedRange = (1 << pfd.cRedBits) -1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) -1;

	for(i = 0; i < nColors; i++)
		{
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
       (double) pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
		 (double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
		 (double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char) NULL;
	}
		
	hRetPal = CreatePalette(pPal);

	SelectPalette(dc->GetSafeHdc(), hRetPal,FALSE);
	RealizePalette(dc->GetSafeHdc());

	free(pPal);

	m_hpalette = hRetPal;
}

BOOL CAMViewerView::SetPlaying(BOOL play) { 
   BOOL wasplay = m_bplay;
   m_bplay = play;
   m_prevtime = 0;
   return wasplay;
}

/////////////////////////////////////////////////////////////////////////////
// CAMViewerView message handlers

int CAMViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;


#ifdef _MAC
   static long attrib [] = {
      AGL_RGBA,
      AGL_DOUBLEBUFFER,
      AGL_DEPTH_SIZE, 16,
      AGL_NONE
   };

   AGLPixelFormat pixelformat = aglChoosePixelFormat( NULL, 0, (long *)attrib );
   m_context = aglCreateContext( pixelformat, NULL );
   aglDestroyPixelFormat( pixelformat );

   aglSetCurrentContext( m_context );

   aglEnable( m_context, AGL_SWAP_RECT );
   aglEnable( m_context, AGL_BUFFER_RECT );

   OnSize(0,0,0);
#else
   static PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,                   // Version of structure
      PFD_DRAW_TO_WINDOW |
      PFD_SUPPORT_OPENGL |
      PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,       // RGBA Color Mode   
      24,                  // Want 24 bit
      0,0,0,0,0,0,
      0,0,
      0,0,0,0,0,
      16,//32                  // Size of Depth Buffer
      0,
      0,
      PFD_MAIN_PLANE,
      0,
      0,0,0 };

   HDC hdc = GetDC()->GetSafeHdc();

   int nPixelFormat = ChoosePixelFormat(hdc, &pfd);
   SetPixelFormat(hdc, nPixelFormat, &pfd);

   SetOpenGLPalette();
   
   m_hrc = wglCreateContext(hdc);
   wglMakeCurrent(hdc, m_hrc);
#endif   

	glEnable(GL_DEPTH_TEST);
   glDisable(GL_TEXTURE_1D);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
   glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
   glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
   glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1.0f, 1.0f);
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glShadeModel(GL_SMOOTH);
   glDisable(GL_CULL_FACE);
   glDisable(GL_LIGHTING);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f );

   return 0;
}

BOOL CAMViewerView::OnQueryNewPalette()
{
#ifndef _MAC
	if (m_hpalette) {
      HDC hdc = GetDC()->GetSafeHdc();
		SelectPalette(hdc, m_hpalette, FALSE);
		RealizePalette(hdc);
		InvalidateRect(NULL,FALSE);
		return TRUE;
	}
#endif
   return FALSE;
}

void CAMViewerView::OnPaletteChanged( CWnd* pFocusWnd )
{
#ifndef _MAC
	if ((m_hpalette != NULL) && (pFocusWnd != this)) {
      HDC hdc = GetDC()->GetSafeHdc();
		SelectPalette(hdc, m_hpalette, FALSE);
		RealizePalette(hdc);
		UpdateColors(hdc);
   }
#endif
}
 
void CAMViewerView::OnDestroy() 
{
	CView::OnDestroy();

#ifdef _MAC
   if (m_context) {
      aglSetCurrentContext( NULL );
      aglSetDrawable( m_context, NULL );
      aglDestroyContext( m_context );
      m_context = NULL;
   }
#else
   HDC hdc = GetDC()->GetSafeHdc();

   wglMakeCurrent(hdc, NULL);
   wglDeleteContext(m_hrc);
#endif

   if (m_hpalette != NULL)
      DeleteObject(m_hpalette);
}

void CAMViewerView::OnSize(UINT nType, int cx, int cy) 
{
   m_width = cx;
   m_height = cy;

   m_center.x = m_width/2;
   m_center.y = m_height/2;

#ifdef _MAC
   CDC *dc = GetDC();
   GWorldPtr oldGWorld;
   GDHandle oldGDevice;
   GrafPtr port;
   RGBColor white, black;
      
   AGLDrawable drawable = (AGLDrawable)CheckoutPort( dc->GetSafeHdc()/*m_hDC*/, CA_NONE );
   aglSetDrawable( m_context, drawable );
   GetPort(&port);
   GetGWorld( &oldGWorld,&oldGDevice );		// save the old port 
   white.red = white.green = white.blue = 65535;
   black.red = black.green = black.blue = 0;
   SetPort((GrafPtr)drawable);   
   SetGWorld( drawable, NULL );
   RGBForeColor( &black );
   RGBBackColor( &white );
   SetPort( port );
   SetGWorld( oldGWorld,oldGDevice );

   CheckinPort( dc->m_hDC, CA_NONE );
   ReleaseDC( dc );

   CRect rcWindow, rcFrame;
   GetWindowRect( &rcWindow );
   GetParentFrame()->GetWindowRect( &rcFrame );

   GLint param[4];
   param[0] = rcWindow.left - rcFrame.left - GetSystemMetrics(SM_CXFRAME) - 1;
   param[1] = rcFrame.bottom - rcWindow.bottom - GetSystemMetrics(SM_CYFRAME) - 2;
   param[2] = rcWindow.Width();
   param[3] = rcWindow.Height();
   aglSetInteger( m_context, AGL_BUFFER_RECT, param );
   param[0] = 0;
   param[1] = 0;
   aglSetInteger( m_context, AGL_SWAP_RECT, param);
#endif

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glViewport(0, 0, m_width, m_height);

   if (m_bperspective)
      m_zoom = m_width/35.0f;

  	CView::OnSize(nType, cx, cy);
}

void CAMViewerView::OnLButtonDown(UINT nFlags, CPoint point) 
{
   SetCapture();
   m_mouse = point;
   m_bmousedown = TRUE;

   switch (m_mode) {
   case MOVE:
      StartMove();
      break;
   case ZOOM:
      StartZoom();
      break;
   case TURN:
      StartTurn();
      break;
   }
	CView::OnLButtonDown(nFlags, point);
}

void CAMViewerView::OnRButtonDown(UINT nFlags, CPoint point) 
{
   m_bmousedown = TRUE;
   SetCapture();
   m_mouse = point;

   switch (m_mode) {
      case ZOOM:
         StartBoundZoom();
         break;
   }
	CView::OnRButtonDown(nFlags, point);
}

static void RemoveCachedMessages()
{
   MSG msg;

   while (PeekMessage( &msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE ));
   while (PeekMessage( &msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE ));
}

void CAMViewerView::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (!m_bmousedown)
      return;

   m_mouse = point;

   RemoveCachedMessages();
   if (nFlags & MK_LBUTTON) {
      switch (m_mode) {
         case MOVE:
            DoMove();
            break;
         case ZOOM:
            DoZoom();
            break;
         case TURN:
            DoTurn();
            break;
      }
   }
   else if (nFlags & MK_RBUTTON) {
      switch (m_mode) {
         case ZOOM:
            DoBoundZoom();
            break;
      }
   }
	CView::OnMouseMove(nFlags, point);
}

void CAMViewerView::OnLButtonUp(UINT nFlags, CPoint point) 
{
   ::ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
   m_bmousedown = FALSE;
}

void CAMViewerView::OnRButtonUp(UINT nFlags, CPoint point) 
{
   m_bmousedown = FALSE;
   switch (m_mode) {
      case ZOOM:
         EndBoundZoom();
         break;
   }
   ::ReleaseCapture();
	CView::OnRButtonUp(nFlags, point);
}

void CAMViewerView::OnZoom() 
{
   m_mode = ZOOM;
   m_cursorid = ID_ZOOM;
   ::SetCursor( AfxGetApp()->LoadCursor(ID_ZOOM));
}

void CAMViewerView::OnUpdateZoom(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(m_mode == ZOOM);
}

void CAMViewerView::OnMove() 
{
   m_mode = MOVE;
   m_cursorid = ID_MOVE;
   ::SetCursor( AfxGetApp()->LoadCursor(ID_MOVE));
}

void CAMViewerView::OnUpdateMove(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(m_mode == MOVE);
}

void CAMViewerView::OnTurn() 
{
   m_mode = TURN;
   m_cursorid = ID_TURN;
   ::SetCursor( AfxGetApp()->LoadCursor(ID_TURN));
}

void CAMViewerView::OnUpdateTurn(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(m_mode == TURN);
}

BOOL CAMViewerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   if (GetDocument()->m_model)
      ::SetCursor(AfxGetApp()->LoadCursor(m_cursorid));
   else
      ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CAMViewerView::InitView()
{
#ifdef _MAC
   CDC *dc = GetDC();
   AGLDrawable drawable = (AGLDrawable)CheckoutPort( dc->GetSafeHdc(), CA_NONE );
   aglSetDrawable( m_context, drawable );
   CheckinPort( dc->m_hDC, CA_NONE );
   ReleaseDC( dc );
#endif

   m_rotate.Set(0.0f,0.0f,0.0f);
   if (m_bperspective)
      m_zoom = m_width/35.0f;
   else
      m_zoom = 1.0f;
   m_offset.Set(0.0f);

  	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   m_matrix = YXZRotate( -m_rotate.y, -m_rotate.x, -m_rotate.z ).Transpose();
   m_time.SetSeconds(0);
   m_prevtime = 0;
   GetDocument()->m_lasttime.SetTagged(TRUE);
   m_bsubdivided = FALSE;
   m_patchsplitter.m_maxlevel = 0;
   ZoomFit();
   ComputeSpan();
}

void CAMViewerView::OnInitialUpdate() 
{
   CView::OnInitialUpdate();
   InitView();
}

void CAMViewerView::OnFrontView()
{
   m_rotate.Set(0.0f, 0.0f, 0.0f);
   m_matrix = YXZRotate( -m_rotate.y, -m_rotate.x, -m_rotate.z ).Transpose();
   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}

void CAMViewerView::OnBackView()
{
   m_rotate.Set(0.0f, 180.0f, 0.0f);
   m_matrix = YXZRotate( -m_rotate.y, -m_rotate.x, -m_rotate.z ).Transpose();
   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}

void CAMViewerView::OnLeftView()
{
   m_rotate.Set(0.0f, 90.0f, 0.0f);
   m_matrix = YXZRotate( -m_rotate.y, -m_rotate.x, -m_rotate.z ).Transpose();
   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}
void CAMViewerView::OnRightView()
{
   m_rotate.Set(0.0f, -90.0f, 0.0f);
   m_matrix = YXZRotate( -m_rotate.y, -m_rotate.x, -m_rotate.z ).Transpose();
   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}
void CAMViewerView::OnTopView()
{
   m_rotate.Set(90.0f, 0.0f, 0.0f);
   m_matrix = YXZRotate( -m_rotate.y, -m_rotate.x, -m_rotate.z ).Transpose();
   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}
void CAMViewerView::OnBottomView()
{
   m_rotate.Set(-90.0f,0.0f,0.0f);
   m_matrix = YXZRotate( -m_rotate.y, -m_rotate.x, -m_rotate.z ).Transpose();
   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}

void CAMViewerView::OnUpdatePolyMode(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(!m_bsubdivided);
}

void CAMViewerView::OnPolyMode()
{
   m_bsubdivided = FALSE;
   UpdateSubLevelStats();
   Model *model = GetDocument()->m_model;
   CString strcount;
   if (dynamic_cast<PatchModel *>(model))
      strcount.Format("%d Patches", model->GetPatchPolyCount());
   else
      strcount.Format("%d Polys", model->GetPatchPolyCount());
   g_mainframe->SetPatchPolyCount(strcount);
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateSubdividedMode(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_bsubdivided);
   pCmdUI->Enable(dynamic_cast<PatchModel *>(GetDocument()->m_model)!=NULL);
}

void CAMViewerView::OnSubdividedMode()
{
   m_bsubdivided = TRUE;
   UpdateSubLevelStats();
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateShaded(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(m_bshaded);
}

void CAMViewerView::OnShaded()
{
   m_bshaded = TRUE;
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateWireframe(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(!m_bshaded);
}

void CAMViewerView::OnWireframe()
{
   m_bshaded = FALSE;
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateVertices(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(m_bvertices);
}

void CAMViewerView::OnVertices()
{
   m_bvertices = !m_bvertices;
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateTangents(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_btangents);
   pCmdUI->Enable(dynamic_cast<PatchModel *>(GetDocument()->m_model)!=NULL);
}

void CAMViewerView::OnTangents()
{
   m_btangents = !m_btangents;
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateNormals(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(m_bnormals);
}

void CAMViewerView::OnNormals()
{
   m_bnormals = !m_bnormals;
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateMaterials(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model && GetDocument()->m_model->m_attrlist.GetSize());
   pCmdUI->SetCheck(m_bmaterials);
}

void CAMViewerView::OnMaterials()
{
   m_bmaterials = !m_bmaterials;
   GetDocument()->m_model->m_lastattr = -1;
   GetDocument()->m_model->m_lastattrdiffuse = -1;
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateDecals(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model && GetDocument()->m_model->m_maplist.GetSize());
   pCmdUI->SetCheck(m_bdecals);
}

void CAMViewerView::OnDecals()
{
   m_bdecals = !m_bdecals;
   GetDocument()->m_model->m_lasttexture = 0;
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdatePerspective(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(m_bperspective);
}

void CAMViewerView::OnPerspective()
{
   m_bperspective = TRUE;
   m_zoom = m_width/35.0f;
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateOrthogonal(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->m_model!=NULL);
   pCmdUI->SetCheck(!m_bperspective);
}

void CAMViewerView::OnOrthogonal()
{
   m_bperspective = FALSE;
   ZoomFit();
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdatePlay(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(!GetDocument()->m_actionlist.IsEmpty() && GetDocument()->m_model);
   pCmdUI->SetCheck(IsPlaying());
}

void CAMViewerView::OnPlay()
{
   SetPlaying(!IsPlaying());
}

BOOL CAMViewerView::TimerProc()
{
   if (!IsPlaying())
      return FALSE;

   if (m_time != GetCurrentPlayTime(FALSE)) {
      Invalidate(FALSE);
      UpdateWindow();
   }

   return TRUE;
}

void CAMViewerView::OnNextframe() 
{
    m_time.StepFrames(1, GetDocument()->m_fps);
    Invalidate(FALSE);
    UpdateWindow();
}

void CAMViewerView::OnUpdateNextframe(CCmdUI* pCmdUI) 
{
   Time nexttime = m_time;
   nexttime.StepFrames(1, GetDocument()->m_fps);
   pCmdUI->Enable(GetDocument()->m_scriptlength.GetSeconds() && !IsPlaying() && nexttime <= GetDocument()->m_scriptlength);
}

void CAMViewerView::OnPrevframe() 
{
    m_time.StepFrames(-1, GetDocument()->m_fps);
    Invalidate(FALSE);
    UpdateWindow();
}

void CAMViewerView::OnUpdatePrevframe(CCmdUI* pCmdUI) 
{
   Time nexttime = m_time;
   nexttime.StepFrames(-1, GetDocument()->m_fps);
   pCmdUI->Enable(GetDocument()->m_scriptlength.GetSeconds() && !IsPlaying() && nexttime >= 0);
}

void CAMViewerView::OnLastframe() 
{
    m_time = GetDocument()->m_scriptlength;
    Invalidate(FALSE);
    UpdateWindow();
}

void CAMViewerView::OnFirstframe() 
{
    m_time.SetSeconds(0);
    Invalidate(FALSE);
    UpdateWindow();
}

Time CAMViewerView::GetCurrentPlayTime( BOOL store /*TRUE*/ )
{
   if (GetDocument()->m_scriptlength.GetSeconds()==0)
      return 0.0f;

   if (!IsPlaying())
      return m_time;

   DWORD curtime = GetTime();

   if (m_prevtime == 0)
      m_prevtime = curtime;

   DWORD elapsed = curtime - m_prevtime;
   double secondselapsed = (double)elapsed / TICKSPERSEC;

   Time elapsedtime ( secondselapsed );
   Time newnonfloortime = m_lastnonfloortime + elapsedtime;
   Time newtime = newnonfloortime;
   newtime.FloorToFrame( CAMViewerDoc::m_fps );

   if (!m_bskipframes)
      newtime = min(m_time+1.0f/GetDocument()->m_fps, newtime);
      
   if (newtime > GetDocument()->m_scriptlength) {
      newtime=0.0f;
      m_lastnonfloortime = 0.0f;
   }

   if (store && m_time != newtime) {
      m_prevtime = curtime;
      m_time = newtime;   
      m_lastnonfloortime = newnonfloortime;
   }
   
   return newtime;
}
 
void CAMViewerView::OnUpdateIncreaseSub(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_bsubdivided && (m_patchsplitter.m_isvariable || m_patchsplitter.m_maxlevel < MAXSUB ));
}

void CAMViewerView::OnIncreaseSub()
{
   if (m_patchsplitter.m_isvariable) {
      if (m_patchsplitter.m_straight < .1)
         m_patchsplitter.m_straight-=.01f;
      else
         m_patchsplitter.m_straight-=.1f;
      m_patchsplitter.m_straight = max(.01f, m_patchsplitter.m_straight);
   }
   else
      m_patchsplitter.m_maxlevel+=2;
   UpdateSubLevelStats();
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateDecreaseSub(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_bsubdivided && (m_patchsplitter.m_isvariable || m_patchsplitter.m_maxlevel > 0));
}

void CAMViewerView::OnDecreaseSub()
{
   if (m_patchsplitter.m_isvariable) {
      if (m_patchsplitter.m_straight < .1f)
         m_patchsplitter.m_straight+=.01f;
      else
         m_patchsplitter.m_straight+=.1f;
   }
   else
      m_patchsplitter.m_maxlevel-=2;
   UpdateSubLevelStats();
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateMinSub(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_bsubdivided && (m_patchsplitter.m_isvariable || m_patchsplitter.m_maxlevel != 0));
}

void CAMViewerView::OnMinSub()
{
   if (m_patchsplitter.m_isvariable)
      m_patchsplitter.m_straight = 2.0f;
   else
      m_patchsplitter.m_maxlevel = 0;
   UpdateSubLevelStats();
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateMaxSub(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_bsubdivided && (m_patchsplitter.m_isvariable || m_patchsplitter.m_maxlevel != MAXSUB));
}

void CAMViewerView::OnMaxSub()
{
   if (m_patchsplitter.m_isvariable)
      m_patchsplitter.m_straight = .1f;
   else
      m_patchsplitter.m_maxlevel = MAXSUB;
   UpdateSubLevelStats();
   InvalidateRect(NULL, FALSE);
}

void CAMViewerView::OnUpdateFixSub(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_bsubdivided);
   pCmdUI->SetCheck(!m_patchsplitter.m_isvariable);
}

void CAMViewerView::OnFixSub()
{
   m_patchsplitter.m_isvariable = !m_patchsplitter.m_isvariable;
   UpdateSubLevelStats();
   InvalidateRect(NULL, FALSE);
}

BOOL CAMViewerView::MyCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   return OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CAMViewerView::UpdateSubLevelStats()
{
   CString text;
   if (!m_bsubdivided)
      text = " ";
   else if (m_patchsplitter.m_isvariable) {
      CString value;
      value.Format("%g", m_patchsplitter.m_straight);
      AfxFormatString1(text, IDS_STRAIGHT, value);
   }
   else { // fixed
      int num = (int)pow(2,m_patchsplitter.m_maxlevel);
      CString value;
      value.Format("%d", num);
      AfxFormatString1(text, IDS_POLYSPERPATCH, value);
   }
   g_mainframe->SetSubLevel(text);
}

void CAMViewerView::ComputeSpan()
{
   CAMViewerDoc *doc = GetDocument();
   if (doc->m_model==NULL)
      return;
   m_matrix.SetTranslate(Vector(0.0f));
   Vector translate = m_offset;
   translate *= m_matrix;
   translate.z -= m_focaldistance;

   m_frontspan = min(-1, doc->m_model->m_span + translate.z);
   m_backspan = -doc->m_model->m_span + translate.z;
   m_span = m_backspan-m_frontspan;
}

void CAMViewerView::OnShowBones() 
{
  CAMViewerDoc *doc = GetDocument();
 
	if (m_bshowbones)
		m_bshowbones = 0;
	else
		m_bshowbones = 1;
	   InvalidateRect(NULL, FALSE);
}
