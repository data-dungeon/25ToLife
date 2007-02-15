// KB  6/27/2003  \Ken105\HashApp\ScrollBar.cpp

#include "StdAfx.h"
#include "ColorBuf.h"
#include "ScrollBar.h"

////////////////////

void HashScrollBar::Init()
{
   m_wndParent = NULL;
	m_min = 0;
	m_max = 100;
	m_line = 1;
	m_page = 10;
	m_pos = 0;
	m_isuppushed = FALSE;
	m_isdownpushed = FALSE;
	m_isscrolling = FALSE;
	m_isupdownatbottom = TRUE;
   m_isvisible = FALSE;
   m_ismousecapture = FALSE;
}

HashScrollBar::HashScrollBar()
{
   Init();
   m_ishorizontal = FALSE; // defaults to vertical scrollbar
}

HashScrollBar::HashScrollBar( CWnd *parent, BOOL ishorizontal )
{
   Init();
   m_wndParent = parent;
   m_ishorizontal = ishorizontal;
}

void HashScrollBar::SetRect( const CRect &rect )
{
   m_rcAll = m_rcScroll = m_rcUp = m_rcDown = rect;

   if (m_ishorizontal) {
      if (m_isupdownatbottom) {
         m_rcScroll.right -= SCROLLBARSIZE * 2;
         m_rcUp.left = m_rcScroll.right - 1;
         m_rcUp.right -= SCROLLBARSIZE;
         m_rcDown.left = m_rcUp.right - 1;
      }
      else {
         m_rcScroll.left += SCROLLBARSIZE;
         m_rcScroll.right -= SCROLLBARSIZE;
         m_rcUp.right = m_rcScroll.left + 1;
         m_rcDown.left = m_rcScroll.right - 1;
      }
   }
   else {
      if (m_isupdownatbottom) {
         m_rcScroll.bottom -= SCROLLBARSIZE * 2;
         m_rcUp.top = m_rcScroll.bottom - 1;
         m_rcUp.bottom -= SCROLLBARSIZE;
         m_rcDown.top = m_rcUp.bottom - 1;
      }
      else {
         m_rcScroll.top += SCROLLBARSIZE;
         m_rcScroll.bottom -= SCROLLBARSIZE;
         m_rcUp.bottom = m_rcScroll.top + 1;
         m_rcDown.top = m_rcScroll.bottom - 1;
      }
   }
}

int HashScrollBar::SetMin( int min )
{
   m_min = min;

   if (m_min > m_max)
      m_max = m_min;

   SetPos( m_pos );

   return m_min;
}

int HashScrollBar::SetMax( int max )
{
   m_max = max;

   if (m_max < m_min)
      m_min = m_max;

   SetPos( m_pos );

   return m_max;
}

int HashScrollBar::SetLine( int line )
{
   m_line = line;

   return m_line;
}

int HashScrollBar::SetPage( int page )
{
   m_page = page;

   return m_page;
}

int HashScrollBar::SetPos( int pos )
{
	if (pos < m_min)
		pos = m_min;
	else if (pos > m_max - m_page)
		pos = m_max - m_page;

   m_pos = pos;

	return m_pos;
}

int HashScrollBar::GetPos( int point )
{
   float pct;

   if (m_ishorizontal)
	   pct = (float)(point - m_rcScroll.left) / m_rcScroll.Width();
   else
	   pct = (float)(point - m_rcScroll.top) / m_rcScroll.Height();
	int span = GetMax() - GetMin();
	int pos = (int)(pct * span);

	return pos;
}

BOOL HashScrollBar::SetScrollInfo( LPSCROLLINFO lpScrollInfo, BOOL bRedraw/*=TRUE*/ )
{
   SetMin( lpScrollInfo->nMin );
   SetMax( lpScrollInfo->nMax );
   SetPage( lpScrollInfo->nPage );

// maybe use flag SIF_DISABLENOSCROLL
   Show( (m_max - m_min) > m_page );

   if (m_isvisible)
      SetPos( lpScrollInfo->nPos );

   if (bRedraw)
      Invalidate();

   return TRUE;
}

int HashScrollBar::GetScrollPos() const
{
   return GetPos();
}

int HashScrollBar::SetScrollPos( int nPos, BOOL bRedraw/*=TRUE*/ )
{
   int prevpos = m_pos;

   SetPos( nPos );

   if (bRedraw)
      Invalidate();

   return prevpos;
}

void HashScrollBar::GetScrollRange( LPINT lpMinPos, LPINT lpMaxPos ) const
{
   *lpMinPos = m_min;
   *lpMaxPos = m_max;
}

void HashScrollBar::Show( BOOL show )
{
   if ((BOOL)m_isvisible == show)
      return;

   m_isvisible = show;
   m_wndParent->SetWindowPos( NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE );
}

CRect HashScrollBar::GetKnobRect()
{
	int span = GetMax() - GetMin();
	float fPos = (float)GetPos() / span;
	float fPage = (float)GetPage() / span;
	float fEndPos = fPos + fPage;

	CRect rcKnob = m_rcScroll;

   if (m_ishorizontal) {
	   rcKnob.left = m_rcScroll.left + (int)(m_rcScroll.Width() * fPos);
	   rcKnob.right = m_rcScroll.left + (int)(m_rcScroll.Width() * fEndPos);

	   if (rcKnob.Width() < MINKNOBSIZE) {
		   rcKnob.right = rcKnob.left + MINKNOBSIZE;
		   if (rcKnob.right > m_rcScroll.right)
			   rcKnob.OffsetRect(0, m_rcScroll.right - rcKnob.right);
	   }
   }
   else {
	   rcKnob.top = m_rcScroll.top + (int)(m_rcScroll.Height() * fPos);
	   rcKnob.bottom = m_rcScroll.top + (int)(m_rcScroll.Height() * fEndPos);

	   if (rcKnob.Height() < MINKNOBSIZE) {
		   rcKnob.bottom = rcKnob.top + MINKNOBSIZE;
		   if (rcKnob.bottom > m_rcScroll.bottom)
			   rcKnob.OffsetRect(0, m_rcScroll.bottom - rcKnob.bottom);
	   }
   }
	return rcKnob;
}

void HashScrollBar::Invalidate()
{
   m_wndParent->SetWindowPos( NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOREPOSITION | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE );
}

void HashScrollBar::Draw( CDC *pdc )
{
   if (!m_isvisible)
      return;

   CDC memDC;
   if (!memDC.CreateCompatibleDC( pdc ))
      return;
   CBitmap bm;
   CRect rcWindow;
   m_wndParent->GetWindowRect( &rcWindow );
   if (!bm.CreateCompatibleBitmap( pdc, rcWindow.Width(), rcWindow.Height()))
      return;

   CDC *dc = &memDC;
   CBitmap *oldbitmap = dc->SelectObject( &bm );

   CPoint triangle[3];
   CPen blackpen( PS_SOLID, 0, GetSysColor(COLOR_WINDOWFRAME));
   CBrush blackbrush( GetSysColor(COLOR_WINDOWFRAME));
   CPen *oldpen = dc->SelectObject( &blackpen );
   CBrush *oldbrush = dc->SelectObject( &blackbrush );

// GAPS
   ColorBuf colorbtnface = GetSysColor(COLOR_BTNFACE);
   CPen penbtnface( PS_SOLID, 0, colorbtnface );
   ColorBuf colorbk( colorbtnface.Red()*5/6, colorbtnface.Green()*5/6, colorbtnface.Blue()*5/6 );
   ColorBuf colorshadowlight( colorbtnface.Red()*2/3, colorbtnface.Green()*2/3, colorbtnface.Blue()*2/3 );
   CPen penshadowlight( PS_SOLID, 0, colorshadowlight );
   CPen penshadowdark( PS_SOLID, 0, RGB(colorshadowlight.Red()*7/8, colorshadowlight.Green()*7/8, colorshadowlight.Blue()*7/8 ));
   CPen penhighlight( PS_SOLID, 0, RGB(colorbtnface.Red()*11/12, colorbtnface.Green()*11/12, colorbtnface.Blue()*11/12 ));
   CRect rcKnob = GetKnobRect();

   CRect rcGap = m_rcScroll;
   if (m_ishorizontal)
      rcGap.right = rcKnob.left + 1;
   else
      rcGap.bottom = rcKnob.top + 1;
   dc->Rectangle( rcGap );
   rcGap.DeflateRect( 1, 1 );
   if (m_ishorizontal)
      rcGap.bottom--;
   else
      rcGap.right--;
   dc->SelectObject( &penshadowdark );
   dc->MoveTo( rcGap.left, rcGap.bottom );
   dc->LineTo( rcGap.left, rcGap.top );
   dc->LineTo( rcGap.right, rcGap.top );
   dc->SelectObject( &penbtnface );
   dc->MoveTo( rcGap.right, rcGap.top );
   dc->LineTo( rcGap.right, rcGap.bottom );
   rcGap.left++, rcGap.top++, rcGap.right--;
   dc->SelectObject( &penshadowlight );
   dc->MoveTo( rcGap.left, rcGap.bottom );
   dc->LineTo( rcGap.left, rcGap.top );
   dc->LineTo( rcGap.right, rcGap.top );
   dc->SelectObject( &penhighlight );
   dc->MoveTo( rcGap.right, rcGap.top );
   dc->LineTo( rcGap.right, rcGap.bottom );
   rcGap.left++, rcGap.top++;
   dc->FillSolidRect( rcGap, colorbk );

   rcGap = m_rcScroll;
   if (m_ishorizontal) 
      rcGap.left = rcKnob.right - 1;
   else
      rcGap.top = rcKnob.bottom - 1;
   dc->SelectObject( &blackpen );
   dc->Rectangle( rcGap );
   rcGap.DeflateRect( 1, 1 );
   if (m_ishorizontal) 
      rcGap.bottom--;
   else
      rcGap.right--;
   dc->SelectObject( &penshadowdark );
   dc->MoveTo( rcGap.left, rcGap.bottom-1 );
   dc->LineTo( rcGap.left, rcGap.top );
   dc->LineTo( rcGap.right, rcGap.top );
   dc->SelectObject( &penbtnface );
   dc->MoveTo( rcGap.right, rcGap.top );
   dc->LineTo( rcGap.right, rcGap.bottom-1 );
   rcGap.left++, rcGap.top++, rcGap.right--;
   dc->SelectObject( &penshadowlight );
   dc->MoveTo( rcGap.left, rcGap.bottom-1 );
   dc->LineTo( rcGap.left, rcGap.top );
   dc->LineTo( rcGap.right, rcGap.top );
   dc->SelectObject( &penhighlight );
   dc->MoveTo( rcGap.right, rcGap.top );
   dc->LineTo( rcGap.right, rcGap.bottom-1 );
   rcGap.left++, rcGap.top++;
   dc->FillSolidRect( rcGap, colorbk );

// KNOB
   dc->SelectObject( &blackpen );
   dc->Rectangle( rcKnob );
   rcKnob.DeflateRect( 1, 1 );
   ColorBuf color = GetSysColor(COLOR_BTNSHADOW);
   if (m_isscrolling)
      color.Set( color.Red()*2/3, color.Green()*2/3, color.Blue()*2/3, color.Alpha());
   ColorBuf colorhighlight( (color.Red()+MAXCOLOR)/2, (color.Green()+MAXCOLOR)/2, (color.Blue()+MAXCOLOR)/2 );
   ColorBuf colorshadow( color.Red()*2/3, color.Green()*2/3, color.Blue()*2/3 );
   ColorBuf colordarkshadow( color.Red()*1/3, color.Green()*1/3, color.Blue()*1/3 );
   dc->Draw3dRect( rcKnob, colorhighlight, colorshadow );
   rcKnob.DeflateRect( 1, 1 );
   dc->FillSolidRect( rcKnob, color );
   CPoint mid;
   mid.x = (rcKnob.left + rcKnob.right) / 2;
   mid.y = (rcKnob.top + rcKnob.bottom) / 2;
   if (m_ishorizontal) {
      for (int i=0; i<4; i++) {
         int x = mid.x-4 + i*2;
         dc->FillSolidRect( CRect( x, mid.y-4, x+1, mid.y+3 ), colorhighlight );
         dc->FillSolidRect( CRect( x+1, mid.y-3, x+2, mid.y+4 ), colordarkshadow );
      }
   }
   else {
      for (int i=0; i<4; i++) {
         int y = mid.y-4 + i*2;
         dc->FillSolidRect( CRect( mid.x-4, y, mid.x+3, y+1 ), colorhighlight );
         dc->FillSolidRect( CRect( mid.x-3, y+1, mid.x+4, y+2 ), colordarkshadow );
      }
   }

// UP BUTTON
   CRect rcUp = m_rcUp;
   dc->Rectangle( rcUp );
   rcUp.DeflateRect( 1, 1 );
   if (m_isuppushed) {
      ColorBuf colorbtnface = GetSysColor(COLOR_BTNSHADOW);
      ColorBuf colorshadow( colorbtnface.Red()*2/3, colorbtnface.Green()*2/3, colorbtnface.Blue()*2/3 );
      ColorBuf colorhighlight = GetSysColor(COLOR_BTNFACE);
      dc->Draw3dRect( rcUp, colorshadow, colorhighlight);
      rcUp.DeflateRect( 1, 1 );
      dc->FillSolidRect( rcUp, colorbtnface);
   }
   else {
      dc->Draw3dRect( rcUp, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
      rcUp.DeflateRect( 1, 1 );
      dc->FillSolidRect( rcUp, GetSysColor(COLOR_BTNFACE));
   }
   CPoint midup( (rcUp.left + rcUp.right) / 2, (rcUp.top + rcUp.bottom) / 2 );
   if (m_ishorizontal) {
      triangle[0] = CPoint(midup.x-2, midup.y-1);
      triangle[1] = CPoint(midup.x+1, midup.y+2);
      triangle[2] = CPoint(midup.x+1, midup.y-4);
   }
   else {
      triangle[0] = CPoint(midup.x-1, midup.y-2);
      triangle[1] = CPoint(midup.x+2, midup.y+1);
      triangle[2] = CPoint(midup.x-4, midup.y+1);
   }
   dc->Polygon( triangle, 3 );

// DOWN BUTTON
   CRect rcDown = m_rcDown;
   dc->Rectangle( rcDown );
   rcDown.DeflateRect( 1, 1 );
   if (m_isdownpushed) {
      ColorBuf colorbtnface = GetSysColor(COLOR_BTNSHADOW);
      ColorBuf colorshadow( colorbtnface.Red()*2/3, colorbtnface.Green()*2/3, colorbtnface.Blue()*2/3 );
      ColorBuf colorhighlight = GetSysColor(COLOR_BTNFACE);
      dc->Draw3dRect( rcDown, colorshadow, colorhighlight);
      rcDown.DeflateRect( 1, 1 );
      dc->FillSolidRect( rcDown, colorbtnface);
   }
   else {
      dc->Draw3dRect( rcDown, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
      rcDown.DeflateRect( 1, 1 );
      dc->FillSolidRect( rcDown, GetSysColor(COLOR_BTNFACE));
   }
   CPoint middown( (rcDown.left + rcDown.right) / 2, (rcDown.top + rcDown.bottom) / 2 );
   if (m_ishorizontal) {
      triangle[0] = CPoint(middown.x+1, middown.y-1);
      triangle[1] = CPoint(middown.x-2, middown.y+2);
      triangle[2] = CPoint(middown.x-2, middown.y-4);
   }
   else {
      triangle[0] = CPoint(middown.x-1, middown.y+1);
      triangle[1] = CPoint(middown.x+2, middown.y-2);
      triangle[2] = CPoint(middown.x-4, middown.y-2);
   }
   dc->Polygon( triangle, 3 );

// BLIT IT

   pdc->BitBlt( m_rcAll.left, m_rcAll.top, m_rcAll.Width(), m_rcAll.Height(), dc, m_rcAll.left, m_rcAll.top, SRCCOPY );

   dc->SelectObject( oldbitmap );
   dc->SelectObject( oldpen );
   dc->SelectObject( oldbrush );
}

void HashScrollBar::OnLButtonDown( CPoint point )
{
   UINT msg = m_ishorizontal ? WM_HSCROLL : WM_VSCROLL;

   m_wndParent->SetCapture();
   m_ismousecapture = TRUE;

	CRect rcKnob = GetKnobRect();
   BOOL settimer = FALSE;

   if (m_rcUp.PtInRect( point )) {
      m_isuppushed = TRUE;
      Invalidate();
      m_wndParent->SendMessage( msg, MAKEWPARAM(SB_LINEUP, 0), NULL );
      settimer = TRUE;
   }
   else if (m_rcDown.PtInRect( point )) {
      m_isdownpushed = TRUE;
      Invalidate();
      m_wndParent->SendMessage( msg, MAKEWPARAM(SB_LINEDOWN, 0), NULL );
      settimer = TRUE;
   }
   else {
      if (m_ishorizontal)
	      m_prevpos = GetPos( point.x );
      else
	      m_prevpos = GetPos( point.y );
	   m_clickoffset = m_prevpos - GetPos();
	   m_isscrolling = rcKnob.PtInRect( point );

	   if (m_isscrolling)
		   Invalidate();
	   else {
         if (m_ishorizontal) {
		      if (point.x < rcKnob.left)
               m_wndParent->SendMessage( WM_HSCROLL, MAKEWPARAM(SB_PAGEUP, 0), NULL );
		      else
               m_wndParent->SendMessage( WM_HSCROLL, MAKEWPARAM(SB_PAGEDOWN, 0), NULL );
         }
         else {
		      if (point.y < rcKnob.top)
               m_wndParent->SendMessage( WM_VSCROLL, MAKEWPARAM(SB_PAGEUP, 0), NULL );
		      else
               m_wndParent->SendMessage( WM_VSCROLL, MAKEWPARAM(SB_PAGEDOWN, 0), NULL );
         }
         settimer = TRUE;
	   }
   }

   if (settimer) {
      m_isfirsttimer = TRUE;
      m_wndParent->SetTimer( VSCROLLTIMERID, 80, NULL );
   }
}

void HashScrollBar::OnMouseMove( CPoint point )
{
	int trypos;

   if (m_ishorizontal)
   	trypos = GetPos( point.x );
   else
   	trypos = GetPos( point.y );

	if (m_isscrolling && trypos!=m_prevpos) {
      float oldpos = (float)GetPos();
      float newpos = (float)SetPos( trypos - m_clickoffset );
      if (oldpos != newpos)
         m_wndParent->SendMessage( m_ishorizontal ? WM_HSCROLL : WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, newpos), NULL );

		m_prevpos = trypos;
	}
}

void HashScrollBar::OnLButtonUp( CPoint point )
{
   if (m_isscrolling) {
      m_wndParent->SendMessage( m_ishorizontal ? WM_HSCROLL : WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, m_pos), NULL );
   	m_isscrolling = FALSE;
	   Invalidate();
   }
   else if (m_isuppushed || m_isdownpushed) {
   	m_isuppushed = m_isdownpushed = FALSE;
	   Invalidate();
   }

   ::ReleaseCapture();
   m_wndParent->KillTimer( VSCROLLTIMERID );
   m_ismousecapture = FALSE;
}

void HashScrollBar::OnTimer()
{
   if (m_isfirsttimer) {
      m_isfirsttimer = FALSE;
      return;
   }

   if (m_isuppushed)
      m_wndParent->SendMessage( m_ishorizontal ? WM_HSCROLL : WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), NULL );
   else if (m_isdownpushed)
      m_wndParent->SendMessage( m_ishorizontal ? WM_HSCROLL : WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), NULL );
   else {
   	CRect rcKnob = GetKnobRect();
      CPoint point;
      GetCursorPos( &point );
      m_wndParent->ScreenToClient( &point );

      if (m_rcScroll.PtInRect( point )) {
         if (m_ishorizontal) {
		      if (point.x < rcKnob.left)
               m_wndParent->SendMessage( WM_HSCROLL, MAKEWPARAM(SB_PAGEUP, 0), NULL );
		      else if (point.x > rcKnob.right)
               m_wndParent->SendMessage( WM_HSCROLL, MAKEWPARAM(SB_PAGEDOWN, 0), NULL );
         }
         else {
		      if (point.y < rcKnob.top)
               m_wndParent->SendMessage( WM_VSCROLL, MAKEWPARAM(SB_PAGEUP, 0), NULL );
		      else if (point.y > rcKnob.bottom)
               m_wndParent->SendMessage( WM_VSCROLL, MAKEWPARAM(SB_PAGEDOWN, 0), NULL );
         }
      }   
   }   
}

void HashScrollBar::SetHorizontal( BOOL ishorizontal )
{
   m_ishorizontal = ishorizontal;
}
