//

#include "stdafx.h"
#include "GLButtonControl.h"
#include "Snipe.h"
#include "GLRenderWnd.h"
#include "Resource.h"

///////////////////////////////////////
// CGLButtonControl

GLImageList CGLButtonControl::m_glImageListButtonDefault;

IMPLEMENT_DYNAMIC(CGLButtonControl, CGLControl)

CGLButtonControl::CGLButtonControl( BOOL bCheckBox/*=FALSE*/, BOOL bChecked/*=FALSE*/ )
{
   m_pglImageListButton = &m_glImageListButtonDefault;
   m_bDeleteButtonImageList = FALSE;
   m_bCheckBox = bCheckBox;
   m_bChecked = bChecked;
   m_bPressed = FALSE;
   m_bHasBackground = TRUE;
}

CGLButtonControl::~CGLButtonControl()
{
   if (m_bDeleteButtonImageList)
      delete m_pglImageListButton;
}

void CGLButtonControl::PostInit()
{
   if (m_glImageListButtonDefault.IsEmpty())
      m_glImageListButtonDefault.Create( IDB_BUTTON, CRect(3, 4, 13, 14), 16 );

   if (m_nID != -1)
   {
      if (m_strText.LoadString( m_nID ) == 0)
         m_strText.Empty();
      MakeRenderContextCurrent();
      m_glImageListIcon.Create( m_nID );
   }
}

BOOL CGLButtonControl::LoadButtonImageList(  UINT nBitmapID, const CRect &rcDice, int cx/*=0*/, HINSTANCE hInstance/*=NULL*/ )
{
   m_pglImageListButton = SNEW GLImageList;
   m_bDeleteButtonImageList = TRUE;
   return m_pglImageListButton->Create( nBitmapID, rcDice, cx, hInstance );
}

void CGLButtonControl::SetPressed( BOOL bPressed )
{
   if (m_bPressed == bPressed)
      return;

   m_bPressed = bPressed;

   Invalidate();
}

BOOL CGLButtonControl::SetChecked( BOOL bChecked )
{
   BOOL bWasChecked = m_bChecked;

   if (m_bChecked != bChecked) {
      m_bChecked = bChecked;
      Invalidate();
   }

   return bWasChecked;
}

void CGLButtonControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   float fOffset = 0.0f;
   if (m_bPressed)
      fOffset = 1.0f;

   RGBFloat cTint;
   if (IsEnabled())
      cTint.Set( 1.0f );
   else
      cTint.Set( 0.8f );

   // Background
   if (m_bHasBackground)
   {
      UINT nIndex;
      if (m_bPressed || m_bChecked)
         nIndex = 1;
      else
         nIndex = 0;

      m_pglImageListButton->SetTint( cTint );

      m_pglImageListButton->DrawDiced( CRect(CPoint(0,0), m_szControl), nIndex );

      if (m_bCheckBox && m_bChecked)
         m_pglImageListButton->Draw( CRect(CPoint(0,0), m_szControl), FALSE, FALSE, 2 );
   }

   // Icon
   if (!m_glImageListIcon.IsEmpty())
   {
      m_glImageListIcon.SetTint( cTint );
      m_glImageListIcon.Draw( (m_szControl.cx-(int)m_glImageListIcon.GetWidthEach())/2, (m_szControl.cy-(int)m_glImageListIcon.GetHeight())/2+fOffset, 0, 1 );
   }
   else if (!m_strText.IsEmpty())
   {
      glColor3f( 0,0,0 );
      DrawText( (m_szControl.cx - GetTextExtent( m_strText ).cx) / 2, m_szControl.cy/2 + 5.0f, m_strText );
   }
}

BOOL CGLButtonControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   SetPressed( TRUE );
   GetApp().DrawInvalidControls();

   MSG msgLoop;
   msgLoop.message = 0;
   while (msgLoop.message != WM_LBUTTONUP)	{
		if (PeekMessage(&msgLoop, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msgLoop);
			DispatchMessage(&msgLoop);
			continue;
		}

      GetCursorPos( &point );
      m_pGLRenderWnd->ScreenToClient( &point );
      if (m_bPressed != HitTest( point )) {
         SetPressed( !m_bPressed );
         GetApp().DrawInvalidControls();
      }
   }

   if (m_bPressed) {
      if (m_bCheckBox)
         SetChecked( !m_bChecked );

      if (!m_pParentControl->OnControlNotify( NOTIFYCTRL_BN_CLICKED, MAKEWPARAM(m_nID, 0), (LPARAM)this ))
         AfxGetMainWnd()->SendMessage( WM_COMMAND, m_nID );

      SetPressed( FALSE );
      GetApp().DrawInvalidControls();
   }
   return TRUE;
}
