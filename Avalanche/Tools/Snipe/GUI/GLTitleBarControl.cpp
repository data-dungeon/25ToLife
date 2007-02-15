#include "stdafx.h"
#include "GLTitleBarControl.h"
#include "GLRenderWnd.h"
#include "Resource.h"
#include ".\gltitlebarcontrol.h"

GLImageList CGLTitleBarControl::m_glImageListBackground;
GLImageList CGLTitleBarControl::m_glImageListButton;

CGLTitleBarControl::CGLTitleBarControl()
{
}

CGLTitleBarControl::~CGLTitleBarControl()
{
}

void CGLTitleBarControl::PostInit()
{
   CGLControl::PostInit();

   if (m_glImageListBackground.IsEmpty())
   {
      m_glImageListBackground.Create( IDB_TITLEBAR, CRect( 3, 0, 4, 25 ));
      m_glImageListButton.Create( IDB_TITLEBARBUTTON, 18 );
   }

   m_glCloseButton.Init( this, NULL, CPoint(22, 4), CSize(18, 18), ID_CLOSE, JUST_RIGHT );
   m_glCloseButton.m_strText.Empty();
   m_glCloseButton.SetButtonImageList( &m_glImageListButton );
}

void CGLTitleBarControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   glClear( GL_DEPTH_BUFFER_BIT );

   m_glImageListBackground.DrawDiced( CRect(CPoint(0,0), m_szControl) );

   CString strTitle;
   m_pGLRenderWnd->GetWindowText( strTitle );
   if (!strTitle.IsEmpty())
   {
      glColor3f( 0.0f, 0.0f, 0.0f );
      CSize szText = GetTextExtent( (LPCTSTR)strTitle );
      DrawText( (m_szControl.cx - szText.cx) / 2, 17, (LPCTSTR)strTitle );
   }

   CGLControl::Draw( nDrawFlags );
}

void CGLTitleBarControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification )
{
   CGLControl::Move( ptControl, CSize(szControl.cx, m_glImageListBackground.GetHeight()), justification );
}

BOOL CGLTitleBarControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   DWORD pos = GetMessagePos();
   CPoint ptStart( pos );

   CRect rcWindowStart;
   m_pGLRenderWnd->GetWindowRect( &rcWindowStart );
   if (GetWindowLong( *m_pGLRenderWnd, GWL_STYLE ) & WS_CHILD)
      m_pGLRenderWnd->GetParent()->ScreenToClient( &rcWindowStart );

   MSG msgLoop;
   msgLoop.message = 0;
   while (msgLoop.message != WM_LBUTTONUP)
   {
		if (PeekMessage(&msgLoop, 0, 0, 0, PM_REMOVE))
      {
			TranslateMessage(&msgLoop);
			DispatchMessage(&msgLoop);
			continue;
		}

      if (msgLoop.message == WM_MOUSEMOVE)
      {
         CPoint ptDelta = CPoint(msgLoop.pt) - ptStart;
         m_pGLRenderWnd->SetWindowPos( NULL, rcWindowStart.left + ptDelta.x, rcWindowStart.top + ptDelta.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
      }
   }
   m_pGLRenderWnd->Invalidate( FALSE );
   return TRUE;
}

BOOL CGLTitleBarControl::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYCTRL_BN_CLICKED: {
         UINT nID = LOWORD(wParam);
         switch (nID) {
            case ID_CLOSE:
               OnClose();
               return TRUE;
         }
         return FALSE;
      }
   }

   return CGLControl::OnControlNotify( msg, wParam, lParam );
}

void CGLTitleBarControl::OnClose()
{
   m_pGLRenderWnd->EndModalLoop( ID_CLOSE );
//   m_pGLRenderWnd->DestroyWindow();
}
