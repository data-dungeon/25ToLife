#include "stdafx.h"
#include "GLSplashControl.h"
#include "GLRenderWnd.h"
#include "resource.h"

CGLSplashControl::CGLSplashControl( CGLRenderWnd *pglRenderWnd, float fMinSecondsToDisplay/*=2.0f*/ )
{
#ifdef _DEBUG
   m_fMinSecondsToDisplay = 0.0f;
#else
   m_fMinSecondsToDisplay = fMinSecondsToDisplay;
#endif
   m_fStartTime = (float)GetTickCount() / 1000.0f;

   LoadBitmap( IDB_SPLASH );

   CRect rcClient;
   pglRenderWnd->GetClientRect( &rcClient );

   CPoint pos( (rcClient.Width() - m_glImageListBackground.GetWidthEach()) / 2,
               (rcClient.Height() - m_glImageListBackground.GetHeight()) / 2 - 22 );
   Init( NULL, pglRenderWnd, pos, CSize(m_glImageListBackground.GetWidthEach(), m_glImageListBackground.GetHeight()) );

   for (m_fOpacity=0.0f; m_fOpacity<1.0f; m_fOpacity+=0.1f)
   {
      m_pGLRenderWnd->Invalidate(FALSE);
      m_pGLRenderWnd->UpdateWindow();
   }
}

CGLSplashControl::~CGLSplashControl()
{
   float fCurTime = (float)GetTickCount() / 1000.0f;
   float fSecondsDisplayed = fCurTime - m_fStartTime;
   if (fSecondsDisplayed < m_fMinSecondsToDisplay)
      Sleep( (m_fMinSecondsToDisplay - fSecondsDisplayed) * 1000 );

   for (m_fOpacity=1.0f; m_fOpacity>0.0f; m_fOpacity-=0.1f)
   {
      m_pGLRenderWnd->Invalidate(FALSE);
      m_pGLRenderWnd->UpdateWindow();
   }

   m_pGLRenderWnd->RemoveControl( this );
   m_pGLRenderWnd->Invalidate(FALSE);
   m_pGLRenderWnd->UpdateWindow();
}

void CGLSplashControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   glClear( GL_DEPTH_BUFFER_BIT );
   m_glImageListBackground.Draw( 0.0f, 0.0f, 0, m_fOpacity );

   CGLControl::Draw( nDrawFlags );
}
