//

#include "stdafx.h"
#include "GLViewControl.h"
#include "GL2DView.h"
#include "GLRenderWnd.h"
#include "resource.h"

static f_bPositioningSliders;

CGLViewControl::CGLViewControl( CGLBaseView *pBaseView )
{
   m_pBaseView = pBaseView;
   m_pfnCallback = NULL;
   m_pCallbackData = NULL;

   pBaseView->AddViewControl( this );
}

CGLViewControl::~CGLViewControl()
{
   m_pBaseView->RemoveViewControl( this );
}

void ViewControlChangedCallback( CGLViewControl *pViewControl, void *pData )
{
   CGLViewControl *pNotifyControl = (CGLViewControl *)pData;
   pNotifyControl->OnViewControlChanged( pViewControl );
}

/////////////////////////////////
// CGLKnobViewControl

GLImageList CGLKnobViewControl::m_glImageListKnob;

CGLKnobViewControl::CGLKnobViewControl( CGLBaseView *pBaseView, const Vector &vPos, const Matrix44 &matrix/*=Identity44()*/ ) :
   CGLViewControl( pBaseView ),
   m_matrix( matrix ),
   m_vPos( vPos )
{
   m_bPressed = FALSE;

   if (m_glImageListKnob.IsEmpty())
      m_glImageListKnob.Create( IDB_SLIDERKNOB, 14 );
}

void CGLKnobViewControl::SetPos( const Vector &vPos )
{
   m_vPos = vPos;

   if (m_pfnCallback)
      m_pfnCallback( this, m_pCallbackData );
}

BOOL CGLKnobViewControl::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   SetPressed( TRUE );
   m_pBaseView->Invalidate();
   return TRUE;
}

BOOL CGLKnobViewControl::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   SetPressed( FALSE );
   m_pBaseView->Invalidate();
   return TRUE;
}

void CGLKnobViewControl::Draw()
{
   // push 2d projection
   {
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
      if (!m_pBaseView->m_bDoPick)
         glLoadIdentity();
      CRect rcControl = m_pBaseView->GetViewportRect();
      glOrtho( 0, rcControl.Width(), rcControl.Height(), 0, -1, 1 );
   }

   if (m_pBaseView->m_bDoPick)
      glLoadName( (GLuint)this );

   int nWidth = m_glImageListKnob.GetWidthEach();
   int nHeight = m_glImageListKnob.GetHeight();

   Vector vPos = m_vPos;
   //vPos *= m_matrix;
   m_pBaseView->WorldToScreen( vPos );
   m_glImageListKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), m_bPressed, 1.0f );

   if (m_pBaseView->m_bDoPick)
      glLoadName( NULL );

   {
      glMatrixMode( GL_PROJECTION );
      glPopMatrix();
      glMatrixMode( GL_MODELVIEW );
      glPopMatrix();
   }
}

/////////////////////////////////
// CGL1DTranslateViewControl

CGL1DTranslateViewControl::CGL1DTranslateViewControl( CGLBaseView *pBaseView, const Vector &vPos, const Vector &vDir, const Matrix44 &matrix/*=Identity44()*/ ) :
   CGLKnobViewControl( pBaseView, vPos, matrix ),
   m_vDir( vDir )
{
}

BOOL CGL1DTranslateViewControl::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   //CGL2DView *p2DView = (CGL2DView *)m_pBaseView;
   //p2DView->m_objecttoviewmatrix = GetMatrix();
   //p2DView->m_viewtoobjectmatrix = GetMatrix().Inverse();

   m_MouseOnLine.Init( (HCGLBaseView *)m_pBaseView, GetPos(), m_vDir );

   //p2DView->m_objecttoviewmatrix.Identity();
   //p2DView->m_viewtoobjectmatrix.Identity();

   return CGLKnobViewControl::OnLButtonDown( keymodifier, mousestate, ptMouse, pickHit );
}

BOOL CGL1DTranslateViewControl::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   //CGL2DView *p2DView = (CGL2DView *)m_pBaseView;
   //p2DView->m_objecttoviewmatrix = GetMatrix();
   //p2DView->m_viewtoobjectmatrix = GetMatrix().Inverse();

   SetPos( m_MouseOnLine.GetWorldPos( ptMouse ) );

   //p2DView->m_objecttoviewmatrix.Identity();
   //p2DView->m_viewtoobjectmatrix.Identity();

   m_pBaseView->Invalidate();
   return TRUE;
}

BOOL CGL1DTranslateViewControl::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   ::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEWE ) );
   return TRUE;
}

/////////////////////////////////
// CGL2DTranslateViewControl

CGL2DTranslateViewControl::CGL2DTranslateViewControl( CGLBaseView *pBaseView, const Vector &vPos, const Vector &vPlane, const Matrix44 &matrix/*=Identity44()*/ ) :
   CGL1DTranslateViewControl( pBaseView, vPos, vPlane, matrix ),
   m_vPlane( vPlane )
{
}

BOOL CGL2DTranslateViewControl::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   CGL2DView *p2DView = (CGL2DView *)m_pBaseView;
   p2DView->m_objecttoviewmatrix = GetMatrix();
   p2DView->m_viewtoobjectmatrix = GetMatrix().Inverse();

   m_MouseOnPlane.Init( (HCGLBaseView *)m_pBaseView, GetPos(), m_vPlane );

   p2DView->m_objecttoviewmatrix.Identity();
   p2DView->m_viewtoobjectmatrix.Identity();

   return CGLKnobViewControl::OnLButtonDown( keymodifier, mousestate, ptMouse, pickHit );
}

BOOL CGL2DTranslateViewControl::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   CGL2DView *p2DView = (CGL2DView *)m_pBaseView;
   p2DView->m_objecttoviewmatrix = GetMatrix();
   p2DView->m_viewtoobjectmatrix = GetMatrix().Inverse();

   SetPos( m_MouseOnPlane.GetWorldPos( ptMouse ) );

   p2DView->m_objecttoviewmatrix.Identity();
   p2DView->m_viewtoobjectmatrix.Identity();

   m_pBaseView->Invalidate();

   return TRUE;
}

BOOL CGL2DTranslateViewControl::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   ::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZENESW ) );
   return TRUE;
}

/////////////////////////////////
// CGL3DTranslateViewControl

CGL3DTranslateViewControl::CGL3DTranslateViewControl( CGLBaseView *pBaseView, const Vector &vPos, const Matrix44 &matrix/*=Identity44()*/ ) :
   CGL2DTranslateViewControl( pBaseView, vPos, Vector(0,1,0), matrix )
{
}

BOOL CGL3DTranslateViewControl::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   CGL2DView *p2DView = (CGL2DView *)m_pBaseView;
   p2DView->m_objecttoviewmatrix = GetMatrix();
   p2DView->m_viewtoobjectmatrix = GetMatrix().Inverse();

   m_MouseOnScreen.Init( (HCGLBaseView *)m_pBaseView, GetPos() );

   p2DView->m_objecttoviewmatrix.Identity();
   p2DView->m_viewtoobjectmatrix.Identity();

   return CGLKnobViewControl::OnLButtonDown( keymodifier, mousestate, ptMouse, pickHit );
}

BOOL CGL3DTranslateViewControl::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   SetPos( m_MouseOnScreen.GetWorldPos( ptMouse ) );
   m_pBaseView->Invalidate();
   return TRUE;
}

BOOL CGL3DTranslateViewControl::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   ::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEALL ) );
   return TRUE;
}

/////////////////////////////////
// CGLSliderViewControl

CGLSliderViewControl::CGLSliderViewControl( CGLBaseView *pBaseView, const Vector &vOrigin, const Vector &vDirection, float fMin/*=0.0f*/, float fMax/*=MAX_FLOAT*/, float fPos/*=0.0f*/, BOOL bDrawTrack/*=TRUE*/, BOOL bDrawFacingScreen/*=TRUE*/, const Matrix44 &matrix/*=Identity44()*/ ) :
   CGL1DTranslateViewControl( pBaseView, vOrigin, vDirection, matrix )
{
   m_vOrigin = vOrigin;
   m_vDirection = vDirection.Normalized();
   m_fMin = fMin;
   m_fMax = fMax;
   m_fPos = fPos;
   m_bDrawTrack = bDrawTrack;
   m_bDrawFacingScreen = bDrawFacingScreen;
   m_fScale = 1.0;
   m_bDrawFacingScreen = FALSE;

   InitMouseOnLine();
   SetPos();
}

void CGLSliderViewControl::Draw()
{
   if (m_bDrawTrack && !m_pBaseView->m_bDoPick && m_fMax!=MAX_FLOAT && m_fMin!=-MAX_FLOAT)
   {
      Vector vStart,vEnd;
      if (m_bDrawFacingScreen)
      {
         // push 2d projection
         {
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glLoadIdentity();
            glMatrixMode( GL_PROJECTION );
            glPushMatrix();
            if (!m_pBaseView->m_bDoPick)
               glLoadIdentity();
            CRect rcControl = m_pBaseView->GetViewportRect();
            glOrtho( 0, rcControl.Width(), rcControl.Height(), 0, -1, 1 );
         }

         Vector vNewOrigin = m_vOrigin;
         m_pBaseView->WorldToScreen(vNewOrigin);
         vNewOrigin.z = 0.0f;

         float fFactor = ((CGL2DView*)m_pBaseView)->GetCamera()->GetCameraDistanceFactor();

         vStart = vNewOrigin + m_vDirection * m_fMin * m_fScale * fFactor;
         vEnd = vNewOrigin + m_vDirection * m_fMax * m_fScale * fFactor;

         glBegin( GL_LINES );
         glColor3f( 1,1,1 );
         glVertex2f( vStart.x, vStart.y );
         glVertex2f( vEnd.x, vEnd.y );
         glEnd();

         {
            glMatrixMode( GL_PROJECTION );
            glPopMatrix();
            glMatrixMode( GL_MODELVIEW );
            glPopMatrix();
         }
      }
      else
      {
         vStart = m_vOrigin + m_vDirection * m_fMin * m_fScale;
         vEnd = m_vOrigin + m_vDirection * m_fMax * m_fScale;
         glBegin( GL_LINES );
         glColor3f( 1,1,1 );
         glVertex3fv( &vStart.x );
         glVertex3fv( &vEnd.x );
         glEnd();
      }
  
   }

   CGLKnobViewControl::Draw();
}

void CGLSliderViewControl::InitMouseOnLine()
{
   float fFactor = 1.0f;
   Vector vOrigin = m_vOrigin;
   if (m_bDrawFacingScreen)
   {
      m_pBaseView->WorldToScreen(vOrigin);
      vOrigin.z = 0.0f;
      fFactor = ((CGL2DView*)m_pBaseView)->GetCamera()->GetCameraDistanceFactor();
   }

   m_MouseOnLine.Init( (HCGLBaseView *)m_pBaseView, vOrigin + m_vDirection * m_fMin * fFactor * m_fScale, m_vDirection);
}

void CGLSliderViewControl::SetPos()
{
   float fFactor = 1.0f;
   Vector vOrigin = m_vOrigin;
   if (m_bDrawFacingScreen)
   {
      m_pBaseView->WorldToScreen(vOrigin);
      vOrigin.z = 0.0f;
      fFactor = ((CGL2DView*)m_pBaseView)->GetCamera()->GetCameraDistanceFactor();
   }
   Vector vPos = vOrigin + m_vDirection * m_fPos * fFactor * m_fScale;
   if (m_bDrawFacingScreen)
   {
      Ray pRay = ((HCGLBaseView*)m_pBaseView)->GetScreenMouseRay(CPoint(vPos.x,vPos.y));
      MouseOnPlane mouseOnPlane;
      mouseOnPlane.Init((HCGLBaseView*)m_pBaseView,m_vOrigin,pRay.m_dir.Negate());
      vPos = mouseOnPlane.GetWorldPos(CPoint(vPos.x,vPos.y));
   }

   CGLKnobViewControl::SetPos( vPos );
}

void CGLSliderViewControl::SetOrigin(const Vector &vOrigin)
{
   m_vOrigin = vOrigin;
   SetPos();
   InitMouseOnLine();
}

BOOL CGLSliderViewControl::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   InitMouseOnLine();
   return CGLKnobViewControl::OnLButtonDown( keymodifier, mousestate, ptMouse, pickHit );
}

BOOL CGLSliderViewControl::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  )
{
   SetPos(ptMouse);
   return CGLKnobViewControl::OnLButtonUp( keymodifier, mousestate, ptMouse, pickHit );
}

BOOL CGLSliderViewControl::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   SetPos(ptMouse);
   m_pBaseView->Invalidate();
   return TRUE;
}

void CGLSliderViewControl::SetPos( const CPoint &ptMouse )
{
   float fFactor = 1.0f;
   Vector vOrigin = m_vOrigin;
   if (m_bDrawFacingScreen)
   {
      m_pBaseView->WorldToScreen(vOrigin);
      vOrigin.z = 0.0f;
      fFactor = ((CGL2DView*)m_pBaseView)->GetCamera()->GetCameraDistanceFactor();
   }

   Vector vPos = m_MouseOnLine.GetWorldPos(ptMouse);
   Vector vDelta = vPos - vOrigin;
//   m_fPos = vDelta.Dot( m_vDirection * m_fMax * fFactor * m_fScale );
//   vDelta.Normalize();
   m_fPos = vDelta.Dot( m_vDirection );

//   g_outputWindow << String(m_fPos) << "\n";

   if (m_fPos < m_fMin)
      m_fPos = m_fMin;
   if (m_fPos > m_fMax)
      m_fPos = m_fMax;

   SetPos();
}

void CGLSliderViewControl::PositionSlider()
{
   f_bPositioningSliders = TRUE;
   SetPos();
   f_bPositioningSliders = FALSE;
}

BOOL CGLSliderViewControl::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   ::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEWE ) );
   return TRUE;
}

/////////////////////////////////
// CGLRotateViewControl

CGLRotateViewControl::CGLRotateViewControl( CGLBaseView *pBaseView, const Vector &vOrigin, const Vector &vDirection, float fRadius/*=1.0f*/ ) :
   CGLKnobViewControl( pBaseView, vOrigin )
{
   m_MouseOnCircle.Init( (HCGLBaseView *)m_pBaseView, vOrigin, vDirection, fRadius, TRUE );

   //SetPos( m_vOrigin + m_vDirection * m_fPos );
}

BOOL CGLRotateViewControl::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   SetPos( m_MouseOnCircle.GetWorldPos( ptMouse ) );
   m_pBaseView->Invalidate();
   return TRUE;
}

BOOL CGLRotateViewControl::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   ::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_UPARROW ) );
   return TRUE;
}

/////////////////////////////////
// CGL2DScaleViewControl

CGL2DScaleViewControl::CGL2DScaleViewControl( CGLBaseView *pBaseView, const Vector &vOrigin, const Vector2 &v2Size, const Matrix44 &matrix ) :
   CGLViewControl( pBaseView ),
   m_SliderN(  pBaseView, vOrigin, Vector(0,1,0), matrix ),
   m_SliderNE( pBaseView, vOrigin, Vector(0,0,1), matrix ),
   m_SliderE(  pBaseView, vOrigin, Vector(1,0,0), matrix ),
   m_SliderSE( pBaseView, vOrigin, Vector(0,0,1), matrix ),
   m_SliderS(  pBaseView, vOrigin, Vector(0,-1,0), matrix ),
   m_SliderSW( pBaseView, vOrigin, Vector(0,0,1), matrix ),
   m_SliderW(  pBaseView, vOrigin, Vector(-1,0,0), matrix ),
   m_SliderNW( pBaseView, vOrigin, Vector(0,0,1), matrix )
{
   m_v2Size = v2Size;

   m_SliderN.SetCallback( this );
   m_SliderNE.SetCallback( this );
   m_SliderE.SetCallback( this );
   m_SliderSE.SetCallback( this );
   m_SliderS.SetCallback( this );
   m_SliderSW.SetCallback( this );
   m_SliderW.SetCallback( this );
   m_SliderNW.SetCallback( this );

   PositionSliders();
}

void CGL2DScaleViewControl::SetMatrix( const Matrix44 &matrix )
{
   m_SliderN.SetMatrix( matrix );
   m_SliderNE.SetMatrix( matrix );
   m_SliderE.SetMatrix( matrix );
   m_SliderSE.SetMatrix( matrix );
   m_SliderS.SetMatrix( matrix );
   m_SliderSW.SetMatrix( matrix );
   m_SliderW.SetMatrix( matrix );
   m_SliderNW.SetMatrix( matrix );
}

void CGL2DScaleViewControl::PositionSliders()
{
   f_bPositioningSliders = TRUE;
   
   m_SliderN.SetPos(  Vector2(            0.0f,  m_v2Size.y/2.0f) );
   m_SliderNE.SetPos( Vector2( m_v2Size.x/2.0f,  m_v2Size.y/2.0f) );
   m_SliderE.SetPos(  Vector2( m_v2Size.x/2.0f,             0.0f) );
   m_SliderSE.SetPos( Vector2( m_v2Size.x/2.0f, -m_v2Size.y/2.0f) );
   m_SliderS.SetPos(  Vector2(            0.0f, -m_v2Size.y/2.0f) );
   m_SliderSW.SetPos( Vector2(-m_v2Size.x/2.0f, -m_v2Size.y/2.0f) );
   m_SliderW.SetPos(  Vector2(-m_v2Size.x/2.0f,             0.0f) );
   m_SliderNW.SetPos( Vector2(-m_v2Size.x/2.0f,  m_v2Size.y/2.0f) );

   if (m_pfnCallback)
      m_pfnCallback( this, m_pCallbackData );

   f_bPositioningSliders = FALSE;
}

void CGL2DScaleViewControl::Draw()
{
return;
}

void CGL2DScaleViewControl::OnViewControlChanged( CGLViewControl *pViewControl )
{
   if (f_bPositioningSliders)
      return;

   if (pViewControl == &m_SliderN)
      m_v2Size.y = m_SliderN.GetPos().Length() * 2.0f;
   else if (pViewControl == &m_SliderNE)
   {
      m_v2Size.x = m_SliderNE.GetPos().x * 2.0f;
      m_v2Size.y = m_SliderNE.GetPos().y * 2.0f;
   }
   else if (pViewControl == &m_SliderE)
      m_v2Size.x = m_SliderE.GetPos().Length() * 2.0f;
   else if (pViewControl == &m_SliderSE)
   {
      m_v2Size.x = m_SliderSE.GetPos().x * 2.0f;
      m_v2Size.y = -m_SliderSE.GetPos().y * 2.0f;
   }
   else if (pViewControl == &m_SliderS)
      m_v2Size.y = m_SliderS.GetPos().Length() * 2.0f;
   else if (pViewControl == &m_SliderSW)
   {
      m_v2Size.x = -m_SliderSW.GetPos().x * 2.0f;
      m_v2Size.y = -m_SliderSW.GetPos().y * 2.0f;
   }
   else if (pViewControl == &m_SliderW)
      m_v2Size.x = m_SliderW.GetPos().Length() * 2.0f;
   else if (pViewControl == &m_SliderNW)
   {
      m_v2Size.x = -m_SliderNW.GetPos().x * 2.0f;
      m_v2Size.y = m_SliderNW.GetPos().y * 2.0f;
   }

   PositionSliders();
}

////////////////////////////////////
// CGLTransformPropertyViewControl

CGLTransformPropertyViewControl::CGLTransformPropertyViewControl( CGLBaseView *pBaseView, TransformProperty *pTransformProperty ) :
   CGLViewControl( pBaseView ),
   m_Translate(  pBaseView, Vector(0,0,0) ),
   m_SliderN(    pBaseView, Vector(0,0,0), Vector(0,1,0)  ),
   m_SliderNE(   pBaseView, Vector(0,0,0), Vector(0,0,1)  ),
   m_SliderE(    pBaseView, Vector(0,0,0), Vector(1,0,0)  ),
   m_SliderSE(   pBaseView, Vector(0,0,0), Vector(0,0,1)  ),
   m_SliderS(    pBaseView, Vector(0,0,0), Vector(0,-1,0) ),
   m_SliderSW(   pBaseView, Vector(0,0,0), Vector(0,0,1)  ),
   m_SliderW(    pBaseView, Vector(0,0,0), Vector(-1,0,0) ),
   m_SliderNW(   pBaseView, Vector(0,0,0), Vector(0,0,1)  ),
   m_RollHandle( pBaseView, Vector(0,0,0), Vector(0,0,1), 1.0f )
{
   m_pTransformProperty = pTransformProperty;
   m_pTransformProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );

   m_Translate.SetCallback( this );
   m_SliderN.SetCallback( this );
   m_SliderNE.SetCallback( this );
   m_SliderE.SetCallback( this );
   m_SliderSE.SetCallback( this );
   m_SliderS.SetCallback( this );
   m_SliderSW.SetCallback( this );
   m_SliderW.SetCallback( this );
   m_SliderNW.SetCallback( this );
   m_RollHandle.SetCallback( this );

   PositionSliders();
}

CGLTransformPropertyViewControl::~CGLTransformPropertyViewControl()
{
   m_pTransformProperty->RemoveNotifyNode( this );
}

void CGLTransformPropertyViewControl::PositionSliders()
{
   f_bPositioningSliders = TRUE;

   Matrix44 matrix = m_pTransformProperty->GetMatrix();

   m_Translate.SetPos(  matrix * Vector( 0.0f,  0.0f,  0.0f) );
   m_SliderN.SetPos(    matrix * Vector( 0.0f,  0.5f, 20.0f) );
   m_SliderNE.SetPos(   matrix * Vector( 0.5f,  0.5f, 20.0f) );
   m_SliderE.SetPos(    matrix * Vector( 0.5f,  0.0f, 20.0f) );
   m_SliderSE.SetPos(   matrix * Vector( 0.5f, -0.5f, 20.0f) );
   m_SliderS.SetPos(    matrix * Vector( 0.0f, -0.5f, 20.0f) );
   m_SliderSW.SetPos(   matrix * Vector(-0.5f, -0.5f, 20.0f) );
   m_SliderW.SetPos(    matrix * Vector(-0.5f,  0.0f, 20.0f) );
   m_SliderNW.SetPos(   matrix * Vector(-0.5f,  0.5f, 20.0f) );
   m_RollHandle.SetPos( matrix * Vector( 0.0f,  1.0f, 20.0f) );

   if (m_pfnCallback)
      m_pfnCallback( this, m_pCallbackData );

   //m_Translate.SetMatrix( matrix );
   //m_SliderN.SetMatrix( matrix );
   //m_SliderNE.SetMatrix( matrix );
   //m_SliderE.SetMatrix( matrix );
   //m_SliderSE.SetMatrix( matrix );
   //m_SliderS.SetMatrix( matrix );
   //m_SliderSW.SetMatrix( matrix );
   //m_SliderW.SetMatrix( matrix );
   //m_SliderNW.SetMatrix( matrix );
   //m_RollHandle.SetMatrix( matrix );

   f_bPositioningSliders = FALSE;
}

void CGLTransformPropertyViewControl::OnViewControlChanged( CGLViewControl *pViewControl )
{
   if (f_bPositioningSliders)
      return;

   Vector vScale = m_pTransformProperty->m_vScale.GetValue();

   if (pViewControl == &m_Translate)
   {
      m_pTransformProperty->m_vTranslate.StoreValue( m_Translate.GetPos() );
      return;
   }
   else if (pViewControl == &m_SliderN)
      vScale.y = m_SliderN.GetPos().y * 2.0f;
   else if (pViewControl == &m_SliderNE)
   {
      vScale.x = m_SliderNE.GetPos().x * 2.0f;
      vScale.y = m_SliderNE.GetPos().y * 2.0f;
   }
   else if (pViewControl == &m_SliderE)
      vScale.x = m_SliderE.GetPos().x * 2.0f;
   else if (pViewControl == &m_SliderSE)
   {
      vScale.x = m_SliderSE.GetPos().x * 2.0f;
      vScale.y = -m_SliderSE.GetPos().y * 2.0f;
   }
   else if (pViewControl == &m_SliderS)
      vScale.y = -m_SliderS.GetPos().y * 2.0f;
   else if (pViewControl == &m_SliderSW)
   {
      vScale.x = -m_SliderSW.GetPos().x * 2.0f;
      vScale.y = -m_SliderSW.GetPos().y * 2.0f;
   }
   else if (pViewControl == &m_SliderW)
      vScale.x = -m_SliderW.GetPos().x * 2.0f;
   else if (pViewControl == &m_SliderNW)
   {
      vScale.x = -m_SliderNW.GetPos().x * 2.0f;
      vScale.y = m_SliderNW.GetPos().y * 2.0f;
   }
   else if (pViewControl == &m_RollHandle)
   {
      Vector vRotate = m_pTransformProperty->m_vRotate.GetValue();
      Vector vDelta = m_RollHandle.GetPos() - Vector(0,0,20);
      vRotate.z = -atan2( vDelta.x, vDelta.y ) * RADIANS;
      m_pTransformProperty->m_vRotate.StoreValue( vRotate );
      return;
   }

   m_pTransformProperty->m_vScale.StoreValue( vScale );
}

BOOL CGLTransformPropertyViewControl::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
         PositionSliders();
         break;
   }

   return SnipeObject::OnObjectNotify( msg, wParam, lParam );
}

////////////////////////////////////
// CGL3DFloatPropertyViewControl

CGL3DFloatPropertyViewControl::CGL3DFloatPropertyViewControl( CGLBaseView *pBaseView, FloatProperty *pFloatProperty ) :
CGLViewControl( pBaseView ),
   m_Slider( pBaseView, Vector(0,0,0), Vector(1,0,0) )
{
   m_pFloatProperty = pFloatProperty;
   m_pFloatProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );

   m_Slider.SetCallback( this );
   
   m_Slider.m_fMin = m_pFloatProperty->GetPropertyInfo()->m_minvalue;
   m_Slider.m_fMax = m_pFloatProperty->GetPropertyInfo()->m_maxvalue;

   m_Slider.m_fPos = m_pFloatProperty->GetValue();

   m_Slider.PositionSlider();
}

CGL3DFloatPropertyViewControl::~CGL3DFloatPropertyViewControl()
{
   m_pFloatProperty->RemoveNotifyNode( this );
}

void CGL3DFloatPropertyViewControl::OnViewControlChanged( CGLViewControl *pViewControl )
{
   if (f_bPositioningSliders)
      return;

   float vFloat = m_pFloatProperty->GetValue();

   if (pViewControl == &m_Slider)
   {
      m_pFloatProperty->StoreValue( m_Slider.m_fPos );
      return;
   }

   m_pFloatProperty->StoreValue( vFloat );
}

BOOL CGL3DFloatPropertyViewControl::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
   case NOTIFYOBJ_VALUECHANGED:
      {
         m_Slider.PositionSlider();
      }

      break;
   }

   return SnipeObject::OnObjectNotify( msg, wParam, lParam );
}

void CGL3DFloatPropertyViewControl::Draw()
{
   String strText = m_pFloatProperty->GetMatchName();

   if (!m_pBaseView->m_bDoPick)
   {
      // push 2d projection
      {
         glMatrixMode( GL_MODELVIEW );
         glPushMatrix();
         glLoadIdentity();
         glMatrixMode( GL_PROJECTION );
         glPushMatrix();
         if (!m_pBaseView->m_bDoPick)
            glLoadIdentity();
         CRect rcControl = m_pBaseView->GetViewportRect();
         glOrtho( 0, rcControl.Width(), rcControl.Height(), 0, -1, 1 );
      }


      Vector vPos = m_Slider.GetPos();
      m_pBaseView->WorldToScreen(vPos);

 //     glDisable(GL_CULL_FACE);
 //     glColor4f(0.0f,0.0f,0.0f,0.4f);
 //     glBegin(GL_QUADS);
 //     glVertex2f(vPos.x-2.0f, vPos.y-2.0f);
 //     glVertex2f(vPos.x-2.0f, vPos.y+2.0f);
 //     glVertex2f(vPos.x+strText.Length()+2.0f, vPos.y+2.0f);
 //     glVertex2f(vPos.x+strText.Length()+2.0f, vPos.y-2.0f);
 //     glEnd();

      glColor3f(1.0f,1.0f,1.0f);
      m_pBaseView->DrawText(vPos.x+4.0f,vPos.y+2.0f,strText);

      {
         glMatrixMode( GL_PROJECTION );
         glPopMatrix();
         glMatrixMode( GL_MODELVIEW );
         glPopMatrix();
      }
   }
   m_Slider.Draw();
}
