#include "stdafx.h"
#include "GLScrollBarControl.h"
#include "Snipe.h"
#include "GLRenderWnd.h"
#include "Resource.h"

/////////////////////////////////////////////////////////
// CGLScrollBarControl

GLImageList CGLScrollBarControl::m_glImageListBackground;
GLImageList CGLScrollBarControl::m_glImageListKnob;

CGLScrollBarControl::CGLScrollBarControl()
{
   m_pOwnerControl = NULL;
   m_nPos = 0;
   m_nPage = 1;
   m_nPage = 10;
   m_nMin = 0;
   m_nMax = 100;
   m_bUpPressed = FALSE;
   m_bDownPressed = FALSE;
   m_bKnobPressed = FALSE;
   m_bAutoHide = TRUE;
}

CGLScrollBarControl::~CGLScrollBarControl()
{
}

void CGLScrollBarControl::PostInit()
{
   CGLControl::PostInit();

   if (m_glImageListBackground.IsEmpty())
   {
      m_glImageListBackground.Create( IDB_VSCROLL, CRect( 0, 29, 15, 31 ) );
      m_glImageListKnob.Create( IDB_SCROLLKNOBV, CRect( 0, 11, 15, 13 ) );
   }

   m_szControl.cx = m_glImageListBackground.GetWidth();
}

void CGLScrollBarControl::SetPage( int nPage )
{
   nPage = min(nPage, m_nMax-m_nMin);
   if (nPage == m_nPage)
      return;

   m_nPage = nPage;
   SetPos( m_nPos );

   if (m_bAutoHide)
   {
      CSize szControl;
      if (m_pOwnerControl)
         szControl = m_pOwnerControl->GetControlSize();
      if (SetVisible(m_nPage < m_nMax-m_nMin))
         if (m_pOwnerControl)
            m_pOwnerControl->Move( m_pOwnerControl->m_ptControl, szControl );
   }
   Invalidate(); 
}

void CGLScrollBarControl::SetMin( int nMin )
{
   int nMax = max(nMin, m_nMax);

   if (nMin==m_nMin && nMax==m_nMax)
      return;

   m_nMin = nMin;
   m_nMax = nMax;
   SetPage( m_nPage );
   Invalidate();
}

void CGLScrollBarControl::SetMax( int nMax )
{
   int nMin = min(m_nMin, nMax);

   if (nMin==m_nMin && nMax==m_nMax)
      return;

   m_nMin = nMin;
   m_nMax = nMax;
   SetPage( m_nPage );
   Invalidate();
}

void CGLScrollBarControl::SetPos( int nPos )
{
   int nOldPos = m_nPos;
   m_nPos = min( m_nMax-m_nPage, nPos );
   m_nPos = max( m_nMin, m_nPos );

   if (m_nPos != nOldPos)
   {
      CGLControl *pNotifyControl;
      if (m_pOwnerControl)
         pNotifyControl = m_pOwnerControl;
      else
         pNotifyControl = m_pParentControl;
      pNotifyControl->OnControlNotify( NOTIFYCTRL_SB_CHANGED, MAKEWPARAM(m_nID, m_nPos), (LPARAM)this );
      Invalidate();
   }
}

CRect CGLScrollBarControl::GetUpRect() const
{
   CRect rcDice = m_glImageListBackground.GetDiceRect();
   CRect rcUp( 0, 0, m_szControl.cx, rcDice.top-7 );
   return rcUp;
}

CRect CGLScrollBarControl::GetDownRect() const
{
   CRect rcDice = m_glImageListBackground.GetDiceRect();
   CRect rcDown( 0, m_szControl.cy-(m_glImageListBackground.GetHeight() - rcDice.bottom)+7, m_szControl.cx, m_szControl.cy );
   return rcDown;
}

CRect CGLScrollBarControl::GetWellRect() const
{
   CRect rcDice = m_glImageListBackground.GetDiceRect();
   CRect rcWell( rcDice.left, rcDice.top-14, m_szControl.cx-(m_glImageListBackground.GetWidth() - rcDice.right), m_szControl.cy-(m_glImageListBackground.GetHeight() - rcDice.bottom)+15);
   return rcWell;
}

CRect CGLScrollBarControl::GetKnobRect() const
{
   float fPos = (float)m_nPos / (float)((m_nMax-m_nPage) - m_nMin);
   float fPage = (float)m_nPage / (float)(m_nMax - m_nMin);

   CRect rcWell = GetWellRect();
   int nKnobHeight = max( rcWell.Height() * fPage, m_glImageListKnob.GetHeight() );
   int nMin = rcWell.top;
   int nMax = rcWell.bottom - nKnobHeight;

   CRect rcKnob( rcWell.left, nMin + (nMax-nMin) * fPos, rcWell.right, 0 );
   rcKnob.bottom = rcKnob.top + nKnobHeight;
   return rcKnob;
}

void CGLScrollBarControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   m_glImageListBackground.DrawDiced( CRect( 0, 0, m_szControl.cx, m_szControl.cy ) );

   if (m_nPage < m_nMax-m_nMin)
      m_glImageListKnob.DrawDiced( GetKnobRect() );
}

static CPoint f_ptStart;
static int    f_nStartPos;

BOOL CGLScrollBarControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   BOOL bUpPressed = FALSE, bDownPressed = FALSE;
   if (GetUpRect().PtInRect( point ))
   {
      m_bUpPressed = bUpPressed = TRUE;
      SetPos( GetPos() - GetLine() );
   }
   else if (GetDownRect().PtInRect( point ))
   {
      m_bDownPressed = bDownPressed = TRUE;
      SetPos( GetPos() + GetLine() );
   }
   else
   { // in the well
      CRect rcKnob = GetKnobRect();
      if (rcKnob.PtInRect( point ))
      {
         f_nStartPos = GetPos();
         m_bKnobPressed = TRUE;
      }
      else
      {
         if (point.y < rcKnob.top)
            SetPos( GetPos() - GetPage() );
         else
            SetPos( GetPos() + GetPage() );
      }
   }

   f_ptStart = point;

   Invalidate();
   GetApp().DrawInvalidControls();

   if (!m_bUpPressed && !m_bDownPressed)
      return TRUE;

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
      BOOL bOutside = FALSE;
      point = msgLoop.pt;
      m_pGLRenderWnd->ScreenToClient( &point );
      ClientToControl( point );

      if (bUpPressed)
      {
         m_bUpPressed = GetUpRect().PtInRect( point );
         if (m_bUpPressed)
            SetPos( GetPos() - GetLine() );
      }

      if (bDownPressed)
      {
         m_bDownPressed = GetDownRect().PtInRect( point );
         if (m_bDownPressed)
            SetPos( GetPos() + GetLine() );
      }

      GetApp().DrawInvalidControls();
   }

   return TRUE;
}

BOOL CGLScrollBarControl::OnMouseMove( UINT nFlags, CPoint point )
{
   if (m_bKnobPressed)
   {
      int nDelta = point.y - f_ptStart.y;
      CRect rcWell = GetWellRect();
      float fUnitsPerPixel = (float)(m_nMax - m_nMin) / rcWell.Height();
      SetPos( f_nStartPos + nDelta * fUnitsPerPixel );
      GetApp().DrawInvalidControls();
      return TRUE;
   }
   return CGLControl::OnMouseMove( nFlags, point );
}

BOOL CGLScrollBarControl::OnLButtonUp( UINT nFlags, CPoint point )
{
   m_bUpPressed = FALSE;
   m_bDownPressed = FALSE;
   m_bKnobPressed = FALSE;

   CGLControl *pNotifyControl;
   if (m_pOwnerControl)
      pNotifyControl = m_pOwnerControl;
   else
      pNotifyControl = m_pParentControl;
   pNotifyControl->OnControlNotify( NOTIFYCTRL_SB_ENDSCROLL, MAKEWPARAM(m_nID, GetPos()), (LPARAM)this );

   Invalidate();
   GetApp().DrawInvalidControls();

   return TRUE;
}

/////////////////////////////////////////////////////////
// CGLScrollableControl

CGLScrollableControl::CGLScrollableControl()
{
   m_bPanning = FALSE;
}

CGLScrollableControl::~CGLScrollableControl()
{
}

void CGLScrollableControl::PostInit()
{
   CGLControl::PostInit();

   m_glVScroll.Init( m_pParentControl );
   m_glVScroll.SetOwnerControl( this );
   if (!m_pParentControl)
      m_pGLRenderWnd->AddControl( &m_glVScroll );

   Move( m_ptControl, m_szControl );
}

void CGLScrollableControl::Move( const CPoint &ptControl, const CSize &cszControl, Justify justification )
{
   CSize szControl = cszControl;
   if (m_glVScroll.IsVisible())
   {
      szControl.cx -= m_glVScroll.m_szControl.cx;
      m_glVScroll.Move( CPoint(ptControl.x + szControl.cx, ptControl.y), CSize(m_glVScroll.m_szControl.cx, szControl.cy) );
   }

   CGLControl::Move( ptControl, szControl, justification );
}

CSize CGLScrollableControl::GetControlSize()
{
   CSize szControl = m_szControl;
   if (m_glHScroll.IsVisible())
      szControl.cy += m_glHScroll.m_szControl.cy;
   if (m_glVScroll.IsVisible())
      szControl.cx += m_glVScroll.m_szControl.cx;
   return szControl;
}

BOOL CGLScrollableControl::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_SB_CHANGED:
      {
         UINT nID = LOWORD(wParam);
         int nPos = HIWORD(wParam);
         CGLScrollBarControl *pScrollBar = (CGLScrollBarControl *)lParam;

         if (pScrollBar == &m_glVScroll)
         {
            Move( m_ptControl, GetControlSize() );
            return TRUE;
         }
      }
   }

   return CGLControl::OnControlNotify( msg, wParam, lParam );
}

BOOL CGLScrollableControl::OnSetCursor( UINT message, CPoint point )
{
   if (m_bPanning)
      ::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZENS ));

   return FALSE;
}

static CPoint f_ptPrev;

BOOL CGLScrollableControl::OnMButtonDown( UINT nFlags, CPoint point )
{
   f_ptPrev = point;
   m_bPanning = TRUE;

   return CGLControl::OnMButtonDown( nFlags, point );
}

BOOL CGLScrollableControl::OnMouseMove( UINT nFlags, CPoint point )
{
   if (m_bPanning)
   {
      int nDelta = point.y - f_ptPrev.y;
      f_ptPrev = point;
      m_glVScroll.SetPos( m_glVScroll.GetPos() - nDelta );
      GetApp().DrawInvalidControls();
   }

   return CGLControl::OnMouseMove( nFlags, point );
}

BOOL CGLScrollableControl::OnMButtonUp( UINT nFlags, CPoint point )
{
   m_bPanning = FALSE;

   OnControlNotify( NOTIFYCTRL_SB_ENDSCROLL, MAKEWPARAM(m_glVScroll.m_nID, m_glVScroll.GetPos()), (LPARAM)&m_glVScroll );

   return CGLControl::OnMButtonUp( nFlags, point );
}

BOOL CGLScrollableControl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
   int nLines = -zDelta / WHEEL_DELTA;

   m_glVScroll.SetPos( m_glVScroll.GetPos() + nLines * m_glVScroll.GetLine() );
   GetApp().DrawInvalidControls();

   return TRUE;
}
