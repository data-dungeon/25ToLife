#include "stdafx.h"
#include "Snipe.h"
#include "MainFrm.h"
#include <afxpriv.h>
#include "Resource.h"

// COLOR CONSTANTS
GLfloat colorWhite[3]		= { 1.0f,1.0f,1.0f };
GLfloat colorBlack[3]		= { 0.0f,0.0f,0.0f };
GLfloat colorGray[3]       = { 0.5f, 0.5f, 0.5f };
GLfloat colorDkGray[3]     = { 0.3f, 0.3f, 0.3f };
GLfloat colorLtGray[3]     = { 0.7f, 0.7f, 0.7f };
GLfloat colorRed[3]        = { 1.0f, 0.0f, 0.0f };
GLfloat colorGreen[3]      = { 0.0f, 1.0f, 0.0f };
GLfloat colorBlue[3]       = { 0.0f, 0.0f, 1.0f };
GLfloat colorLtYellow[3]   = { 1.0f, 1.0f, 0.5f };
GLfloat colorYellow[3]     = { 1.0f, 1.0f, 0.0f };
GLfloat colorOrange[3]     = { 1.0f, 0.5f, 0.0f };
GLfloat colorLtRed[3]      = { 1.0f, 0.7f, 0.7f };
GLfloat colorLtGreen[3]    = { 0.7f, 1.0f, 0.7f };
GLfloat colorLtBlue[3]     = { 0.7f, 0.7f, 1.0f };
GLfloat colorDkRed[3]		= { 0.4f, 0.0f, 0.0f };
GLfloat colorDkGreen[3]		= { 0.0f, 0.2f, 0.0f };
GLfloat colorDkBlue[3]		= { 0.0f, 0.0f, 0.4f };

GLDrawPrepper::GLDrawPrepper( CGLControl *pControl, BOOL bResetProjection/*=TRUE*/ )
{
   pControl->MakeRenderContextCurrent();

   // Get current values for GL states that we are going to change...
   glMatrixMode( GL_MODELVIEW );
   glGetDoublev( GL_MODELVIEW_MATRIX, m_GLModelViewMatrix );
   glLoadIdentity();

   glMatrixMode( GL_PROJECTION );
   glGetDoublev( GL_PROJECTION_MATRIX, m_GLProjectionMatrix );
   if (bResetProjection)
      glLoadIdentity();

   glGetFloatv( GL_SCISSOR_BOX, m_Scissor );

   glGetIntegerv( GL_VIEWPORT, m_Viewport );

   // Change the states
   CRect rcClient;
   pControl->m_pGLRenderWnd->GetClientRect( &rcClient );
   CRect rcControl = pControl->GetViewportRect();
   CRect rcOldScissor( CPoint(m_Scissor[0], m_Scissor[1]), CSize(m_Scissor[2], m_Scissor[3]));
   CRect rcNewScissor( CPoint(rcControl.left, rcClient.bottom-rcControl.bottom), CSize(rcControl.Width(), rcControl.Height()));
   pControl->m_bOutsideScissorArea = FALSE;
   if (rcOldScissor.IsRectNull())
   {
      for (CGLControl *pParent=pControl->GetParent(); pParent; pParent=pParent->GetParent())
      {
         CRect rcParent = pParent->GetViewportRect();
         CRect rcParentScissor( CPoint(rcParent.left, rcClient.bottom-rcParent.bottom), CSize(rcParent.Width(), rcParent.Height()));
         if (!rcNewScissor.IntersectRect( rcNewScissor, rcParentScissor ))
         {
            pControl->m_bOutsideScissorArea = TRUE;
            break;
         }
      }
   }
   else
   {
      if (!rcNewScissor.IntersectRect( rcNewScissor, rcOldScissor ))
         pControl->m_bOutsideScissorArea = TRUE;
   }
   if (pControl->m_bOutsideScissorArea)
   {
      rcNewScissor.right = rcNewScissor.left;
      rcNewScissor.bottom = rcNewScissor.top;
   }

   glViewport( rcControl.left, rcClient.bottom-rcControl.bottom, rcControl.Width(), rcControl.Height());
   glScissor( rcNewScissor.left, rcNewScissor.top, rcNewScissor.Width(), rcNewScissor.Height());
   glOrtho( 0, rcControl.Width(), rcControl.Height(), 0, -1, 1 );

   m_bDepthTest = glIsEnabled( GL_DEPTH_TEST );
   glDisable( GL_DEPTH_TEST );

   m_bLighting = glIsEnabled( GL_LIGHTING );
   glDisable( GL_LIGHTING );
}

GLDrawPrepper::~GLDrawPrepper()
{
   // Put the GL states back the way we found them...
   if (m_bLighting)
      glEnable( GL_LIGHTING );

   if (m_bDepthTest)
      glEnable( GL_DEPTH_TEST );

   glMatrixMode( GL_PROJECTION );
   glLoadMatrixd( m_GLProjectionMatrix );

   glMatrixMode( GL_MODELVIEW );
   glLoadMatrixd( m_GLModelViewMatrix );

   glViewport( m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3] );

   GLfloat scissor[4];
   glGetFloatv( GL_SCISSOR_BOX, scissor );
   if (glAddSwapHintRectWIN)
      glAddSwapHintRectWIN( scissor[0], scissor[1], scissor[2], scissor[3] );

   glScissor( m_Scissor[0], m_Scissor[1], m_Scissor[2], m_Scissor[3] );
}

///////////////// CGLControl

CGLControl *CGLControl::m_pCaptureControl;
CGLControl *CGLControl::m_pContextMenuControl;

IMPLEMENT_DYNAMIC(CGLControl, CCmdTarget)

CGLControl::CGLControl()
{
   m_pGLRenderWnd = NULL;
   m_pParentControl = NULL;
   m_ptControl.SetPoint(0,0);
   m_szControl.SetSize(0,0);
   m_justification = JUST_LEFT;
   m_ptMin.SetPoint(0,0);
   m_nID = -1;
   m_pPrepper = NULL;
   m_bVisible = TRUE;
   m_bEnabled = TRUE;
   m_bInvalid = FALSE;
   m_bAutoDelete = TRUE;
}

CGLControl::~CGLControl()
{
   DestroyChildren();

   if (m_pParentControl)
      m_pParentControl->OnControlNotify( NOTIFYCTRL_REMOVECHILD, 0, (LPARAM)this );

   if (m_pGLRenderWnd)
   {
      if (m_pGLRenderWnd->m_pMouseOverControl == this)
         m_pGLRenderWnd->m_pMouseOverControl = NULL;
      if (m_pGLRenderWnd->m_pActiveControl == this)
         m_pGLRenderWnd->SetActiveControl( NULL );
      if (m_bInvalid)
         m_pGLRenderWnd->RemoveInvalidControl( this );
      if (!m_pParentControl)
         m_pGLRenderWnd->RemoveControl( this );
   }

   if (m_pCaptureControl == this)
      m_pCaptureControl = NULL;
   if (m_pContextMenuControl == this)
      m_pContextMenuControl = NULL;
}

void CGLControl::DestroyChildren()
{
   for (int i=0; i<m_childcontrolarray.GetCount(); )
   {
      if (m_childcontrolarray[i]->m_bAutoDelete)
         delete m_childcontrolarray[i]; // this will remove the control from m_childcontrolarray, so don't increment i
      else
      {
         m_childcontrolarray[i]->m_pParentControl = NULL;
         m_childcontrolarray[i]->DestroyChildren();
         i++;
      }
   }
}

void CGLControl::SetClip( CRect rcClip )
{
   CRect rcViewport = GetViewportRect();
   rcClip.OffsetRect( rcViewport.TopLeft() );

   float params[4];
   glGetFloatv( GL_SCISSOR_BOX, params );
   CRect rcScissor( CPoint(params[0], params[1]), CSize(params[2], params[3]));

   CRect rcClient;
   m_pGLRenderWnd->GetClientRect( &rcClient );

   CRect rcNewScissor( CPoint(rcClip.left, rcClient.bottom-rcClip.bottom), CSize(rcClip.Width(), rcClip.Height()));
   if (rcNewScissor.IntersectRect( rcNewScissor, rcScissor ))
      glScissor( rcNewScissor.left, rcNewScissor.top, rcNewScissor.Width(), rcNewScissor.Height());
}

#ifdef _DEBUG
void CGLControl::Dump( CDumpContext &dc ) const
{
   if (this)
      dc << (void *)this << " " << GetRuntimeClass()->m_lpszClassName << " " << m_ptControl << " " << GetViewportRect() << " " << (void *)m_pParentControl << "\n";
   else
      dc << (void *)NULL << "\n";
}
#endif

BOOL CGLControl::IsVisible() const
{
   for (const CGLControl *pControl=this; pControl; pControl=pControl->m_pParentControl)
      if (!pControl->IsThisVisible())
         return FALSE;

   return TRUE;
}

BOOL CGLControl::HasVisibleChildControl()
{
   for (int i=0; i<m_childcontrolarray.GetSize(); i++)
      if (m_childcontrolarray[i]->IsThisVisible())
         return TRUE;

   return FALSE;
}

BOOL CGLControl::IsEnabled()
{
   for (CGLControl *pControl=this; pControl; pControl=pControl->m_pParentControl)
      if (!pControl->m_bEnabled)
         return FALSE;
   return TRUE;
}

BOOL CGLControl::IsInvalid()
{
   for (CGLControl *pControl=this; pControl; pControl=pControl->m_pParentControl)
      if (pControl->m_bInvalid)
         return TRUE;
   return FALSE;
}

void CGLControl::Invalidate()
{
   if (m_pGLRenderWnd && IsVisible() && !IsInvalid())
   {
      m_pGLRenderWnd->AddInvalidControl( this );
      ValidateChildren();
   }
}

void CGLControl::ValidateChildren()
{
   for (int i=0; i<m_childcontrolarray.GetCount(); i++ )
   {
      CGLControl *pControl = m_childcontrolarray[i];
      if (pControl->m_bInvalid)
         m_pGLRenderWnd->RemoveInvalidControl( pControl );
      pControl->ValidateChildren();
   }
}

BOOL CGLControl::SetVisible( BOOL bVisible )
{
   if (m_bVisible == bVisible)
      return FALSE;

   m_bVisible = bVisible;
   if (bVisible)
      Invalidate();
   else if (m_pParentControl)
      m_pParentControl->Invalidate();
   else if (m_pGLRenderWnd)
      m_pGLRenderWnd->Invalidate( FALSE );

   return TRUE;
}

void CGLControl::SetEnabled( BOOL bEnabled )
{
   bEnabled = bEnabled ? TRUE : FALSE;
   if (m_bEnabled == bEnabled)
      return;

   m_bEnabled = bEnabled;
   Invalidate();
}

void CGLControl::MakeRenderContextCurrent()
{
	m_pGLRenderWnd->MakeRenderContextCurrent();
}

CRect CGLControl::GetViewportRect() const
{
   CRect rcClient;
   m_pGLRenderWnd->GetClientRect( &rcClient );

   CPoint ptTotalOffset( 0, 0 );
   CSize szControl( m_szControl );
   for (const CGLControl *pControl=this; pControl; pControl=pControl->m_pParentControl) {
      CPoint ptOffset = pControl->m_ptControl;
      if (pControl->m_justification == JUST_RIGHT) {
         if (pControl->m_pParentControl)
            ptOffset.x = pControl->m_pParentControl->m_szControl.cx - pControl->m_ptControl.x;
         else
            ptOffset.x = rcClient.Width() - pControl->m_ptControl.x;
         if (ptOffset.x < pControl->m_ptMin.x)
            ptOffset.x = pControl->m_ptMin.x;
      }
      ptTotalOffset += ptOffset;

      // crop to parent
      if (pControl->m_pParentControl) {
         CPoint ptOverhang = (ptTotalOffset + szControl) - pControl->m_pParentControl->m_szControl;
         if (ptOverhang.x > 0)
            szControl.cx -= ptOverhang.x;
         if (ptOverhang.y > 0)
            szControl.cy -= ptOverhang.y;
      }
   }

   CRect rcViewport( ptTotalOffset, ptTotalOffset + szControl );
   return rcViewport;
}

void CGLControl::Init( CGLControl *pParent, CGLRenderWnd *pWnd/*=NULL*/, const CPoint &ptControl/*=CPoint(0,0)*/, const CSize &szControl/*=CSize(0,0)*/, UINT nID/*=-1*/, Justify justification/*=JUST_LEFT*/, CGLControl *pInsertAfter/*=GLC_LAST*/ )
{
   m_pGLRenderWnd = pWnd;
   m_ptControl = ptControl;
   m_szControl = szControl;
   m_justification = justification;
   if (nID != -1)
      m_nID = nID;
   m_pParentControl = pParent;
   if (pParent)
   {
      if (pInsertAfter == GLC_LAST)
         pParent->m_childcontrolarray.Add( this );
      else
      {
         int nIndex = pParent->FindChild( pInsertAfter );
         pParent->m_childcontrolarray.InsertAt( nIndex + 1, this );
      }

      if (!pWnd)
         m_pGLRenderWnd = pParent->m_pGLRenderWnd;
   }
   else if (pWnd)
      pWnd->AddControl( this );

   PostInit();
}

BOOL CGLControl::MakeActive( BOOL bUpdateCmdUIs/*=FALSE*/ )
{
   BOOL bMadeActive = m_pGLRenderWnd->SetActiveControl( this ) != this;

   if (bUpdateCmdUIs && bMadeActive && IsWindow( m_pGLRenderWnd->m_hWnd ))
      m_pGLRenderWnd->SendMessage( WM_IDLEUPDATECMDUI, (WPARAM)TRUE );

   return bMadeActive;
}

CSize CGLControl::GetTextExtent( const String &text )
{
   CDC *pDC = m_pGLRenderWnd->GetDC();
   CFont font;
   font.CreateFont( -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
   CFont *pOldFont = pDC->SelectObject( &font );
   CSize szText = pDC->GetTextExtent( text.Get());
   pDC->SelectObject( pOldFont );
   m_pGLRenderWnd->ReleaseDC( pDC );
   return szText;
}

String CGLControl::GetAbbreviated(const String &strSrc) const
{
   int nMaxWidth=m_szControl.cx;

   CDC *pDC = m_pGLRenderWnd->GetDC();
   CFont font;
   font.CreateFont( -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
   CFont *pOldFont = pDC->SelectObject( &font );
   String strString = strSrc;

   while (pDC->GetTextExtent(strString.Get()).cx > nMaxWidth && strString != strSrc.GetNameOnly())
      strString = strSrc.GetAbbreviated( strString.Length()-1 );

   pDC->SelectObject( pOldFont );
   m_pGLRenderWnd->ReleaseDC( pDC );

   return strString;
}

void CGLControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   if (nDrawFlags & DRAW_CHILDREN)
   {
      for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      {
         CGLControl *pChild = m_childcontrolarray[i];
         if (pChild && pChild->IsThisVisible())
            pChild->Draw( nDrawFlags & DRAW_CHILDREN );
      }
   }
}

void CGLControl::DrawGradient( RGBAFloat c[4], CRect &rect )
{
   SetWindowsProjectionMatrix();

   if (rect.IsRectEmpty())
      rect.SetRect( 0, 0, m_szControl.cx, m_szControl.cy );

   GLboolean bOldDepthTest = glSet( GL_DEPTH_TEST, FALSE );

	glBegin( GL_QUADS );
	glColor4f( c[0].GetRed(), c[0].GetGreen(), c[0].GetBlue(), c[0].GetAlpha() );
	glVertex3f( rect.left, rect.top, 0 );
	glColor4f( c[1].GetRed(), c[1].GetGreen(), c[1].GetBlue(), c[1].GetAlpha() );
	glVertex3f( rect.right, rect.top, 0 );
	glColor4f( c[2].GetRed(), c[2].GetGreen(), c[2].GetBlue(), c[2].GetAlpha() );
	glVertex3f( rect.right, rect.bottom, 0 );
	glColor4f( c[3].GetRed(), c[3].GetGreen(), c[3].GetBlue(), c[3].GetAlpha() );
	glVertex3f( rect.left, rect.bottom, 0 );
	glEnd();

   glSet( GL_DEPTH_TEST, bOldDepthTest );
}

void CGLControl::DrawBorder()
{
   glColor3f( 0.4f, 0.4f, 0.4f );
   glBegin( GL_LINE_LOOP );
      glVertex2f( 1.5f, 1.5f );
      glVertex2f( m_szControl.cx-1.5f, 1.5f );
      glVertex2f( m_szControl.cx-1.5f, m_szControl.cy-1.5f );
      glVertex2f( 1.0f, m_szControl.cy-1.5f );
   glEnd();

   glColor4f( 1.0f, 1.0f, 1.0f, 0.4f );
   glBegin( GL_LINES );
      glVertex2f( 1.5f, m_szControl.cy-0.5f );
      glVertex2f( m_szControl.cx-1.5f, m_szControl.cy-0.5f );
   glEnd();
   glBegin( GL_LINES );
      glVertex2f( m_szControl.cx-0.5f, 1.5f );
      glVertex2f( m_szControl.cx-0.5f, m_szControl.cy-1.5f );
   glEnd();

   glColor4f( 0.0f, 0.0f, 0.0f, 0.2f );
   glBegin( GL_LINES );
      glVertex2f( 1.5f, 0.5f );
      glVertex2f( m_szControl.cx-1.5f, 0.5f );
   glEnd();
   glBegin( GL_LINES );
      glVertex2f( 0.5f, 1.5f );
      glVertex2f( 0.5f, m_szControl.cy-1.5f );
   glEnd();

   float params[4];
   glGetFloatv( GL_SCISSOR_BOX, params );
   CRect rcScissor( CPoint(params[0], params[1]), CSize(params[2], params[3]));
   CRect rcInside = GetViewportRect();
   rcInside.DeflateRect( 2, 2 );
   CRect rcClient;
   m_pGLRenderWnd->GetClientRect( &rcClient );
   CRect rcNewScissor( CPoint(rcInside.left, rcClient.bottom-rcInside.bottom), CSize(rcInside.Width(), rcInside.Height()));
   if (rcNewScissor.IntersectRect( rcNewScissor, rcScissor ))
      glScissor( rcNewScissor.left, rcNewScissor.top, rcNewScissor.Width(), rcNewScissor.Height());
}

void CGLControl::DrawFocusRing()
{
   if (/*CWnd::GetFocus()!=m_pGLRenderWnd ||*/ m_pGLRenderWnd->m_pActiveControl!=this)
      return;

   SetWindowsProjectionMatrix();

   GLboolean bOldLighting = glSet( GL_LIGHTING, FALSE );
   GLboolean bOldDepthtest = glSet( GL_DEPTH_TEST, FALSE );

   float r = 255.0f/255.0f;
   float g = 255.0f/255.0f;
   float b = 0.0f/255.0f;

	glBegin( GL_QUADS );
	glColor4f( r, g, b, 1.0f );
	glVertex3f( 0, 0, 0 );
	glColor4f( r, g, b, 1.0f );
	glVertex3f( m_szControl.cx, 0, 0 );
	glColor4f( r, g, b, 0.0f );
   glVertex3f( m_szControl.cx-FOCUSRINGWIDTH, 0+FOCUSRINGWIDTH, 0 );
	glColor4f( r, g, b, 0.0f );
	glVertex3f( 0+FOCUSRINGWIDTH, 0+FOCUSRINGWIDTH, 0 );

	glColor4f( r, g, b, 0.0f );
   glVertex3f( m_szControl.cx-FOCUSRINGWIDTH, 0+FOCUSRINGWIDTH, 0 );
	glColor4f( r, g, b, 1.0f );
	glVertex3f( m_szControl.cx, 0, 0 );
	glColor4f( r, g, b, 1.0f );
	glVertex3f( m_szControl.cx, m_szControl.cy, 0 );
	glColor4f( r, g, b, 0.0f );
	glVertex3f( m_szControl.cx-FOCUSRINGWIDTH, m_szControl.cy-FOCUSRINGWIDTH, 0 );

	glColor4f( r, g, b, 0.0f );
	glVertex3f( 0+FOCUSRINGWIDTH, m_szControl.cy-FOCUSRINGWIDTH, 0 );
	glColor4f( r, g, b, 0.0f );
	glVertex3f( m_szControl.cx-FOCUSRINGWIDTH, m_szControl.cy-FOCUSRINGWIDTH, 0 );
	glColor4f( r, g, b, 1.0f );
   glVertex3f( m_szControl.cx, m_szControl.cy, 0 );
	glColor4f( r, g, b, 1.0f );
	glVertex3f( 0, m_szControl.cy, 0 );

  	glColor4f( r, g, b, 1.0f );
	glVertex3f( 0, 0, 0 );
	glColor4f( r, g, b, 0.0f );
	glVertex3f( 0+FOCUSRINGWIDTH, 0+FOCUSRINGWIDTH, 0 );
	glColor4f( r, g, b, 0.0f );
	glVertex3f( 0+FOCUSRINGWIDTH, m_szControl.cy-FOCUSRINGWIDTH, 0 );
	glColor4f( r, g, b, 1.0f );
	glVertex3f( 0, m_szControl.cy, 0 );

   glEnd();

   glSet( GL_DEPTH_TEST, bOldDepthtest );
   glSet( GL_LIGHTING, bOldLighting );
}

void CGLControl::DrawShadow()
{
   SetWindowsProjectionMatrix();

   GLboolean bOldLighting = glSet( GL_LIGHTING, FALSE );
   GLboolean bOldDepthtest = glSet( GL_DEPTH_TEST, FALSE );

   glBegin( GL_QUADS );
	   glColor4f( 0, 0, 0, .5 );
	   glVertex3f( 0, 0, 0 );
	   glVertex3f( m_szControl.cx, 0, 0 );
	   glColor4f( 0, 0, 0, 0 );
      glVertex3f( m_szControl.cx, 4, 0 );
	   glVertex3f( 4, 4, 0 );

  	   glColor4f( 0, 0, 0, .5 );
	   glVertex3f( 0, 0, 0 );
	   glColor4f( 0, 0, 0, 0 );
	   glVertex3f( 4, 4, 0 );
      glVertex3f( 4, m_szControl.cy, 0 );
	   glColor4f( 0, 0, 0, .5 );
	   glVertex3f( 0, m_szControl.cy, 0 );
   glEnd();

   glSet( GL_DEPTH_TEST, bOldDepthtest );
   glSet( GL_LIGHTING, bOldLighting );
}

float CGLControl::DrawText( float x, float y, const String &text, float scale/*=1.0f*/ )
{
   if (text.IsEmpty())
      return 0.0f;

#ifdef USEFREETYPE
   return freetype::print( GetMainFrame()->m_glSharedRenderWnd.m_font, fast_round(x), fast_round(y), scale, scale, text.Get());
#endif
#ifdef USEFTGL
   glRasterPos2f( x, y );
   GetMainFrame()->m_glSharedRenderWnd.m_pFont->Render( text.Get() );
   return 0.0f;
#endif
}

void CGLControl::DrawText( float x, float y, float z, const String &text, float scale/*=1.0f*/ )
{
#if 0
   BOOL bCenter=TRUE;
   if (bCenter)
   {
      GLdouble glmodelview_matrix[16], glprojection_matrix[16];
      GLint glviewport[4];
      GLdouble winx, winy, winz;

      glGetDoublev( GL_MODELVIEW_MATRIX, glmodelview_matrix );
      glGetDoublev( GL_PROJECTION_MATRIX, glprojection_matrix );
      glGetIntegerv( GL_VIEWPORT,glviewport );

      glMatrixMode( GL_PROJECTION );
      gluProject( x, y, z, glmodelview_matrix, glprojection_matrix, glviewport, &winx, &winy, &winz );
      glMatrixMode( GL_MODELVIEW );
      glLoadIdentity();

      SetWindowsProjectionMatrix();

      CRect rcClient;
      m_pGLRenderWnd->GetClientRect( &rcClient );
      winx -= glviewport[0] - rcClient.left;
      winy -= rcClient.bottom - (rcClient.bottom - glviewport[1]);

      GLint oldlistbase;
      glGetIntegerv( GL_LIST_BASE, &oldlistbase );
      glListBase( CGLSharedRenderWnd::ALPHADISPLAYLISTBASE );

      glEnable( GL_DEPTH_TEST );
      glRasterPos3f( winx, glviewport[3] - winy, 0.9f );
      glCallLists( text.Length(), GL_UNSIGNED_BYTE, text.Get() );

      glListBase( oldlistbase );

      glMatrixMode( GL_PROJECTION );
      glLoadMatrixd( glprojection_matrix );
      glMatrixMode( GL_MODELVIEW );
      glLoadMatrixd( glmodelview_matrix );
   }
   else
#else
   {
      GLint oldlistbase;
      glGetIntegerv( GL_LIST_BASE, &oldlistbase );
      glListBase( CGLSharedRenderWnd::ALPHADISPLAYLISTBASE );

      glRasterPos3f( x, y, z );
      glCallLists( text.Length(), GL_UNSIGNED_BYTE, text.Get() );

      glListBase( oldlistbase );
   }
#endif

#ifdef USEFREETYPEx
   // project point from 3dspace to 2d space
   GLdouble glmodelview_matrix[16], glprojection_matrix[16];
   GLint glviewport[4];
   GLdouble winx, winy, winz;

   glGetDoublev( GL_MODELVIEW_MATRIX, glmodelview_matrix );
   glGetDoublev( GL_PROJECTION_MATRIX, glprojection_matrix );
   glGetIntegerv( GL_VIEWPORT,glviewport );

   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   gluProject( x, y, z, glmodelview_matrix, glprojection_matrix, glviewport, &winx, &winy, &winz );
   glMatrixMode( GL_PROJECTION );
   glPopMatrix();
   glMatrixMode( GL_MODELVIEW );
   glPopMatrix();

   CRect rcClient;
   m_pGLRenderWnd->GetClientRect( &rcClient );
   winx -= glviewport[0] - rcClient.left;
   winy -= rcClient.bottom - (rcClient.bottom - glviewport[1]);

   freetype::print( GetMainFrame()->m_glSharedRenderWnd.m_font, winx, winy, scalex, scaley, text.Get());
#endif
}

void CGLControl::DrawSpline( Vector2 p[4], int nNumSteps/*=NUMSPLINESTEPS*/ )
{
   glBegin(GL_LINE_STRIP);

   Vector2 v;
   double inc = 1.0 / nNumSteps;
   for (double t=0; t<=1; t+=inc)
   {
      v = (( 2*t*t*t)-(3*t*t)+1)*p[0]
        + ((-2*t*t*t)+(3*t*t)  )*p[3]
        + ((   t*t*t)-(2*t*t)+t)*p[1]
        + ((   t*t*t)-(  t*t)  )*p[2];
      glVertex2fv( &v.x );
   }

   glEnd();
}

void CGLControl::SetWindowsProjectionMatrix()
{
   // Set up projection to be window coords
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   CRect rcViewport = GetViewportRect();

   glOrtho( 0, rcViewport.Width(), rcViewport.Height(), 0, -1, 1 );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void CGLControl::SetProjectionMatrix()
{
   SetWindowsProjectionMatrix();
}

void CGLControl::OnActivate( ActivationMsg msg, CGLControl *pOther )
{
   Invalidate();
}

BOOL CGLControl::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYCTRL_SWAPCHILD: {
         NotifySwapChild *pSwapChild = (NotifySwapChild *)lParam;
         int nIndex = FindChild( pSwapChild->m_pOldChild );
         if (nIndex == -1) {
            if (pSwapChild->m_pNewChild)
               nIndex = m_childcontrolarray.Add( pSwapChild->m_pNewChild );
         }
         else if (pSwapChild->m_pNewChild)
            m_childcontrolarray.SetAt( nIndex, pSwapChild->m_pNewChild );
         else
            m_childcontrolarray.RemoveAt( nIndex );
         if (pSwapChild->m_pOldChild)
            pSwapChild->m_pOldChild->m_pParentControl = NULL;
         if (pSwapChild->m_pNewChild)
            pSwapChild->m_pNewChild->m_pParentControl = this;
         return TRUE;
      }
      case NOTIFYCTRL_REMOVECHILD: {
         CGLControl *pRemove = (CGLControl *)lParam;
         int nIndex = FindChild( pRemove );
         if (nIndex != -1)
            m_childcontrolarray.RemoveAt( nIndex );
         pRemove->m_pParentControl->Invalidate();
         pRemove->m_pParentControl = NULL;
         return TRUE;
      }
      default: {
         if (m_pParentControl && m_pParentControl->OnControlNotify( msg, wParam, lParam ))
            return TRUE;
      }
   }
   return FALSE;
}

void CGLControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   m_ptControl = ptControl;
   m_szControl = szControl;
   if (justification != JUST_NOCHANGE)
      m_justification = justification;
   Invalidate();
}

void CGLControl::ClientToControl( CPoint &pt ) const
{
   CRect rcViewport = GetViewportRect();
   pt -= rcViewport.TopLeft();
}

void CGLControl::ClientToControl( CRect &rc ) const
{
   CRect rcViewport = GetViewportRect();
   rc -= rcViewport.TopLeft();
}

void CGLControl::ControlToClient( CPoint &pt ) const
{
   CRect rcViewport = GetViewportRect();
   pt += rcViewport.TopLeft();
}

void CGLControl::ControlToClient( CRect &rc ) const
{
   CRect rcViewport = GetViewportRect();
   rc += rcViewport.TopLeft();
}

CGLControl *CGLControl::SetCapture()
{
   CGLControl *pOldCapture = m_pCaptureControl;
   m_pCaptureControl = this;
   return pOldCapture;
}

BOOL CGLControl::IsDescendentOf( CGLControl *pParent ) const
{
   for (CGLControl *pControl=GetParent(); pControl; pControl=pControl->GetParent())
   {
      if (pControl == pParent)
         return TRUE;
   }
   return FALSE;
}

int CGLControl::FindChild( CGLControl *pChild )
{
   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      if (m_childcontrolarray[i] == pChild)
         return i;
   return -1;
}

CGLControl *CGLControl::FindChild( UINT nID )
{
   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      if (m_childcontrolarray[i]->m_nID == nID)
         return m_childcontrolarray[i];
   return NULL;
}

BOOL CGLControl::HitTest( const CPoint &pt )
{
   CRect rcViewport = GetViewportRect();
   return rcViewport.PtInRect( pt );
}

CGLControl *CGLControl::HitTestAll( const CPoint &pt )
{
   if (HitTest( pt )) // don't even check children if we didn't hit us
   {
      for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      {
         CGLControl *pChild = m_childcontrolarray[i];
         if (pChild && pChild->IsThisVisible()/* && pChild->m_bEnabled*/) // didn't want disables skipped for tooltips (when would you?)
         {
            CGLControl *pHit = pChild->HitTestAll( pt );
            if (pHit)
               return pHit;
         }
      }
      return this;
   }
   return NULL;
}

void CGLControl::PostInit()
{
}

void CGLControl::Update()
{
   m_pGLRenderWnd->Update();
}

void CGLControl::OnMouseEnter()
{
   if (m_nID != -1)
   {
      CString strFullText;
      if (strFullText.LoadString( m_nID ))
      {
         static CString strDescription;
         if (AfxExtractSubString( strDescription, strFullText, 0, '\n' ))
            GetMainFrame()->m_glMainRenderWnd.m_glStatusPaneControl.SetText( strDescription );
      }
   }
}

void CGLControl::OnMouseLeave()
{
   if (m_nID != -1)
      GetMainFrame()->m_glMainRenderWnd.m_glStatusPaneControl.SetText( NULL );
}

UINT CGLControl::GetToolTipText( CPoint point, String &strText ) const
{
   if (m_nID != -1)
   {
      CString strFullText;
      if (strFullText.LoadString( m_nID ))
      {
         CString strName;
         if (AfxExtractSubString( strName, strFullText, 1, '\n' ))
            strText = strName;
      }
   }
   return m_nID;
}

UINT CGLControl::GetContextMenuID( const CPoint &ptControl, UINT &nDefaultItemID )
{
   return m_nID;
}

BOOL CGLControl::GetContextMenu( const CPoint &ptControl, CMenu &menu )
{
   UINT nDefaultItemID = -1;
   UINT nMenuID = GetContextMenuID( ptControl, nDefaultItemID );
   if (nMenuID == -1)
      return FALSE;

   if (!menu.LoadMenu( nMenuID ))
      return FALSE;

   CMenu *pSubMenu = menu.GetSubMenu(0);
   if (!pSubMenu)
      return FALSE;

   if (nDefaultItemID != -1)
      pSubMenu->SetDefaultItem( nDefaultItemID );

   return TRUE;
}

BOOL CGLControl::OnContextMenu( CPoint ptScreen )
{
   CPoint ptControl = ptScreen;
   m_pGLRenderWnd->ScreenToClient( &ptControl );
   ClientToControl( ptControl );

   CMenu menu;
   if (!GetContextMenu( ptControl, menu ))
      return FALSE;

   CMenu *pSubMenu = menu.GetSubMenu(0);
   if (!pSubMenu)
      return FALSE;

   m_pContextMenuControl = this;
   GetMainFrame()->SendMessage( WM_INITMENUPOPUP, (WPARAM)pSubMenu->GetSafeHmenu(), MAKELPARAM(0, FALSE) );

   UINT nCmdID = pSubMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, ptScreen.x, ptScreen.y, m_pGLRenderWnd );
   m_pGLRenderWnd->SendMessage( WM_COMMAND, nCmdID );
   m_pContextMenuControl = NULL;

   return TRUE;
}

BOOL CGLControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   if (m_pParentControl)
      return m_pParentControl->OnLButtonDown( nFlags, point );
   else
      return FALSE;
}

BOOL CGLControl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
   if (m_pParentControl)
      return m_pParentControl->OnMouseWheel( nFlags, zDelta, pt );
   else
      return FALSE;
}

BOOL CGLControl::OnSetCursor( UINT message, CPoint point )
{
   if (m_pParentControl)
      return m_pParentControl->OnSetCursor( message, point );
   else
      return FALSE;
}

void CGLControl::OnDragEnter( CPoint point, CGLControl *pDragControl )
{
   SetCursor( GetApp().LoadStandardCursor( IDC_CROSS ));
}

void CGLControl::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   CGLControlHeader header( this, bWriteHeader, bs );

   for (int i=0; i<m_childcontrolarray.GetCount(); i++ ) {
      CGLControl *pChild = m_childcontrolarray[i];
      pChild->Save( bs, TRUE );
   }
}

BOOL CGLControl::Load( BaseStream &bs )
{
   static MemoryStream ms( 1024 );
   static CGLControlHeader controlheader;
   static DataHeader dataheader;
   static SnipeObjectHeader snipeobjectheader;

   while (bs.ReadLine( ms ))
   {
      if (CGLControlHeader::IsHeader(ms))
      {
         ms >> controlheader;

         if (!controlheader.m_bBeginHeader)
            return TRUE;

         CGLControl *pControl = (CGLControl *)CRuntimeClass::CreateObject( controlheader.m_strKey.Get() );
         if (!pControl)
         {
            // snipeobjectheader.SkipToEnd( bs );
            continue;
         }

         pControl->Init( this );

         if (controlheader.m_bEndHeader)
            continue;

         pControl->Load( bs );
      }
      else if (DataHeader::IsHeader(ms))
      {
         ms >> dataheader;

         if (!dataheader.m_bBeginHeader)
            continue;

         if (!ParseData( bs, dataheader ))
         {
            // dataheader.SkipToEnd( bs );
         }
      }
      if (SnipeObjectHeader::IsHeader(ms))
      {
         ms >> snipeobjectheader;

         if (!snipeobjectheader.m_bBeginHeader)
            continue;

         SnipeObject *pObject = GetObject( snipeobjectheader.m_strKey );
         if (!pObject)
            continue;

         pObject->Load( bs );
      }
   }

   return TRUE;
}

///////////////////////////////////////////////////////////// 
// CGLControlHeader

CGLControlHeader::CGLControlHeader( CGLControl *pControl, BOOL bWriteHeader, BaseStream &bs ) :
 HeaderBase( bWriteHeader, bs )
{
   m_pControl = pControl;

   if (m_bWriteHeader)
   {
      *m_pBS << mar << '(' << m_pControl->GetRuntimeClass()->m_lpszClassName << ')' << nl;
      m_nFilePos = m_pBS->GetPos();

      m_pBS->m_nMargin++;
   }
}

CGLControlHeader::~CGLControlHeader()
{
   if (m_bWriteHeader) 
   {
      m_pBS->m_nMargin--;

      if (m_pBS->GetPos() != m_nFilePos)
         *m_pBS << mar << "(/" << m_pControl->GetRuntimeClass()->m_lpszClassName << ')' << nl;
      else
      {
         m_pBS->Seek( -m_pBS->GetEOLSize()-1, BaseStream::ST_OFFSET );
         *m_pBS << "/)" << nl;
      }
   }
}

BOOL CGLControlHeader::IsHeader( MemoryStream &ms )
{
   ms.ReadMargin();
   return (ms.GetBuffer()[ms.GetPos()] == '(');
}

BaseStream &operator >> ( BaseStream &bs, CGLControlHeader &value )
{
   value.m_bBeginHeader = FALSE;
   value.m_bEndHeader = FALSE;

   bs.ReadMargin();

   char cTemp;
   bs >> cTemp; // '('
   ASSERT( cTemp == '(' );

   int nCount = bs.ParseToken(')');

   if (bs.m_tokenarray[0] == '/') // end header (/FOO)
   {
      value.m_bEndHeader = TRUE;
      value.m_strKey = &bs.m_tokenarray[1];
      return bs;
   }

   value.m_bBeginHeader = TRUE;

   if (bs.m_tokenarray[nCount-1] == '/') // begin and end header (FOO/)
   {
      bs.m_tokenarray[nCount-1] = '\0';
      value.m_bEndHeader = TRUE;
   }

   value.m_strKey = bs.m_tokenarray;

   return bs;
}

////////////////////////////////////////////////////////////
// CGLTransparentControl

void CGLTransparentControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   CGLControl::Draw( nDrawFlags );
}

////////////////////////////////////////////////////////////
// CGLStaticTextControl

void CGLStaticTextControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   glColor3f( 0,0,0 );
   DrawText( 0, m_szControl.cy/2 + 5.0f, m_strText );

   CGLControl::Draw( nDrawFlags );
}

////////////////////////////////////////////////////////////
// CGLBorderedControl

void CGLBorderedControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   DrawBorder();

   CGLControl::Draw( nDrawFlags );
}

////////////////////////////////////////////////////////////////////
// CGLHeaderControl

IMPLEMENT_DYNAMIC(CGLHeaderControl, CGLControl)

CGLHeaderControl::CGLHeaderControl()
{
}

CGLHeaderControl::~CGLHeaderControl()
{
}

int CGLHeaderControl::AddColumn( const String &strName, int nWidth )
{
   return m_columnarray.Add( Column( strName, nWidth ));
}

void CGLHeaderControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   // Background
	glBegin( GL_QUADS );
	glColor3f( 1, 1, 1 );
	glVertex3f( 0, 0, 0 );
	glColor3f( 1, 1, 1 );
	glVertex3f( m_szControl.cx, 0, 0 );
	glColor3f( .5, .5, .5 );
   glVertex3f( m_szControl.cx, m_szControl.cy, 0 );
	glColor3f( .5, .5, .5 );
	glVertex3f( 0, m_szControl.cy, 0 );
   glEnd();
/*
   // Bottom border
   glColor3f( 0.3f, 0.3f, 0.3f );
   glBegin( GL_LINES );
   glVertex3f( 0, m_szControl.cy-0.5f, 0 );
   glVertex3f( m_szControl.cx, m_szControl.cy-0.5f, 0 );
   glEnd();
*/
   // Columns
   int nPos = 0;
   for (int i=0; i<m_columnarray.GetCount(); i++) {
      glColor3f( 0.0f, 0.0f, 0.0f );
      DrawText( nPos+3, 11, m_columnarray[i].m_strName );

      if (m_columnarray[i].m_nWidth == -1)
         break;
      nPos += m_columnarray[i].m_nWidth;

      glColor3f( 0.3f, 0.3f, 0.3f );
      glBegin( GL_LINES );
      glVertex3f( nPos+0.5f, 0, 0 );
      glVertex3f( nPos+0.5f, m_szControl.cy, 0 );
      glEnd();
   }

   CGLControl::Draw( nDrawFlags );
}

///////////////////////////////////////
// CGLSliderControl

GLImageList CGLSliderControl::m_glImageListSliderBar;
GLImageList CGLSliderControl::m_glImageListSliderKnob;

CGLSliderControl::CGLSliderControl( int nPos/*=0*/, int nMin/*=0*/, int nMax/*=100*/ )
{
   m_nPos = nPos;
   m_nMin = nMin;
   m_nMax = nMax;

   m_nCGLSliderControlFlags = 0;
}

CGLSliderControl::~CGLSliderControl()
{
}

void CGLSliderControl::PostInit()
{
   CGLControl::PostInit();

   if (m_glImageListSliderBar.IsEmpty()) {
      m_glImageListSliderBar.Create( IDB_SLIDERBAR );
      m_glImageListSliderKnob.Create( IDB_SLIDERKNOB, 14 );
   }
}

int CGLSliderControl::SetPos( int nPos )
{
   int nOldPos = m_nPos;
   nPos = min( m_nMax, max( m_nMin, nPos ));
   if (nOldPos != nPos) {
      m_nPos = nPos;
      Invalidate();
   }
   return nOldPos;
}

int CGLSliderControl::SetMin( int nMin )
{
   int nOldMin = m_nMin;
   if (nOldMin != nMin) {
      m_nMin = nMin;
      m_nPos = min( m_nMax, max( m_nMin, m_nPos ));
      Invalidate();
   }
   return nOldMin;
}

int CGLSliderControl::SetMax( int nMax )
{
   int nOldMax = m_nMax;
   if (nOldMax != nMax) {
      m_nMax = nMax;
      m_nPos = min( m_nMax, max( m_nMin, m_nPos ));
      Invalidate();
   }
   return nOldMax;
}

void CGLSliderControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   m_glImageListSliderBar.Draw( 2.0f, 5.0f );

   float fRatio = (float)(m_nPos - m_nMin) / (float)(m_nMax - m_nMin);
   float x = 6.0f + (m_szControl.cx - 12.0f) * fRatio;
   m_glImageListSliderKnob.Draw( x - (float)m_glImageListSliderKnob.GetWidthEach()/2.0f, 1.0f, m_bScrubbing );
}

BOOL CGLSliderControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   m_bScrubbing = TRUE;
   m_bScrubbed = FALSE;

   Invalidate();
   GetApp().DrawInvalidControls();
   return TRUE;
}

BOOL CGLSliderControl::OnMouseMove( UINT nFlags, CPoint point )
{
   if (m_bScrubbing) {
      float fRatio = (float)(point.x - 6) / (float)(m_szControl.cx-12);
      int nPos = m_nMin + (m_nMax - m_nMin) * fRatio;
      SetPos( nPos );
      m_bScrubbed = TRUE;
   }
   return TRUE;
}

BOOL CGLSliderControl::OnLButtonUp( UINT nFlags, CPoint point )
{
   m_bScrubbing = FALSE;
   if (m_bScrubbed)
      OnEndScrub();
   Invalidate();
   GetApp().DrawInvalidControls();
   return TRUE;
}

///////////////////////////////////////
// CGLColorControl

GLImageList CGLColorControl::m_glImageListColorChip;

CGLColorControl::CGLColorControl( RGBFloat rgb )
{
   m_rgb = rgb;
   m_bPressed = FALSE;
}

CGLColorControl::~CGLColorControl()
{
}

RGBFloat CGLColorControl::SetColor( const RGBFloat &rgb )
{
   RGBFloat oldrgb = m_rgb;

   if (m_rgb != rgb) {
      m_rgb = rgb;
      Invalidate();
   }
   return oldrgb;
}

// CGLControl overrides
void CGLColorControl::PostInit()
{
   CGLControl::PostInit();

   if (m_glImageListColorChip.IsEmpty())
      m_glImageListColorChip.Create( IDB_COLORCHIP, 66 );
}

void CGLColorControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   FillColorChip( CRect( MARGIN, MARGIN, m_glImageListColorChip.GetWidthEach() - MARGIN, m_glImageListColorChip.GetHeight() - MARGIN ) );

   m_glImageListColorChip.Draw( 0.0f, 0.0f, m_bPressed );
}

void CGLColorControl::FillColorChip( const CRect &rcFill )
{
   glColor3f( m_rgb.GetRed(), m_rgb.GetGreen(), m_rgb.GetBlue() );
   glBegin( GL_QUADS );
      glVertex2i( MARGIN, MARGIN );
      glVertex2i( m_glImageListColorChip.GetWidthEach() - MARGIN, MARGIN );
      glVertex2i( m_glImageListColorChip.GetWidthEach() - MARGIN, m_glImageListColorChip.GetHeight() - MARGIN );
      glVertex2i( MARGIN, m_glImageListColorChip.GetHeight() - MARGIN );
   glEnd();
}

BOOL CGLColorControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   m_bPressed = TRUE;
   Invalidate();
   GetApp().DrawInvalidControls();
   return TRUE;
}

BOOL CGLColorControl::OnMouseMove( UINT nFlags, CPoint point )
{
   return TRUE;
}

BOOL CGLColorControl::OnLButtonUp( UINT nFlags, CPoint point )
{
   if (!m_bPressed)
      return TRUE;

   CColorDialog dlg;
   if (dlg.DoModal() == IDOK) {
      SetColor( RGBFloat( (float)GetRValue(dlg.m_cc.rgbResult)/255.0f, (float)GetGValue(dlg.m_cc.rgbResult)/255.0f, (float)GetBValue(dlg.m_cc.rgbResult)/255.0f) );
   }

   m_bPressed = FALSE;
   Invalidate();
   GetApp().DrawInvalidControls();
   return TRUE;
}

/////////////////////////////////////////////////////////
// CGLImageControl

CGLImageControl::CGLImageControl()
{
}

CGLImageControl::~CGLImageControl()
{
}

void CGLImageControl::LoadBitmap( UINT nBitmapID )
{
   m_glImageListBackground.Create( nBitmapID );
}

void CGLImageControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   glClear( GL_DEPTH_BUFFER_BIT );

   m_glImageListBackground.Draw( CRect(CPoint(0,0), m_szControl), FALSE, TRUE );

   CGLControl::Draw( nDrawFlags );
}

void CGLControl::GetGLViewportInfo(GLint glViewportInfo[4]) const
{
   CRect rcWindow;
   m_pGLRenderWnd->GetClientRect( &rcWindow );
   CRect rcViewport = GetViewportRect();
   glViewportInfo[0] = rcViewport.left;
   glViewportInfo[1] = rcWindow.bottom - rcViewport.bottom;
   glViewportInfo[2] = rcViewport.Width();
   glViewportInfo[3] = rcViewport.Height();

}