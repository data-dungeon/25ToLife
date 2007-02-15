// Dan  7/6/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\KeyPose\HashTab.cpp

#include "StdAfx.h"
#include "ColorBuf.h"
#include "HashTab.h"

BEGIN_MESSAGE_MAP(CHashTabCtrl, CWnd)
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CHashTabCtrl::CHashTabCtrl()
{
   m_curtab = NULL;
}

CHashTabCtrl::~CHashTabCtrl()
{
}

BOOL CHashTabCtrl::Create( const RECT& rect, CWnd* pParentWnd, UINT nID )
{
   return CWnd::Create( NULL, NULL, WS_CHILD|WS_VISIBLE, rect, pParentWnd, nID );
}

int CHashTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   m_font = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
   if (!m_font)
      m_font = CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT));

   return CWnd::OnCreate( lpCreateStruct );   
}

void CHashTabCtrl::OnPaint()
{
   CPaintDC dc(this);

   CRect rcClient;
   GetClientRect( &rcClient );

   if (!dc.RectVisible(rcClient))
      return;  // Early return if header isn't in need of repaint

   ColorBuf bkcolor = GetSysColor(COLOR_BTNFACE);
   bkcolor.values.red   = fast_ftol_unsigned(bkcolor.values.red * .75f);
   bkcolor.values.green = fast_ftol_unsigned(bkcolor.values.green * .75f);
   bkcolor.values.blue  = fast_ftol_unsigned(bkcolor.values.blue * .75f);
   dc.FillSolidRect( rcClient, bkcolor );

   CFont *pOldFont = dc.SelectObject( m_font );
   CPen pen(PS_SOLID, 0, GetSysColor(COLOR_WINDOWFRAME));
   CPen *pOldPen = dc.SelectObject( &pen );
   CBrush br( GetSysColor(COLOR_BTNFACE));
   CBrush *pOldBrush = dc.SelectObject( &br );
   dc.SetBkMode( TRANSPARENT );

   CRect rcTab = rcClient;
   rcTab.OffsetRect( 1, 1 );
   for (int i=0; i<m_tabs.GetSize(); i++) {
      DrawTab( &dc, rcTab, &m_tabs[i] );
      rcTab.left = rcTab.right + 4;
   }

   dc.MoveTo( rcClient.left, rcClient.bottom-1 );
   dc.LineTo( rcClient.right, rcClient.bottom-1 );

   if (m_curtab) {
      CBrush br2( GetSysColor(COLOR_WINDOW));
      dc.SelectObject( &br2 );
      DrawTab( &dc, m_curtab->m_rect, m_curtab );
   }

   dc.SelectObject(pOldPen);
   dc.SelectObject(pOldBrush);
   dc.SelectObject(pOldFont);
}

void CHashTabCtrl::DrawTab( CDC *dc, CRect &rcTab, HashTab *tab )
{
   CRect rcText(0,0,0,0);
   dc->DrawText( tab->m_label.Get(), &rcText, DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_CENTER|DT_CALCRECT);
   rcTab.right = rcTab.left + rcText.Width() + 8;
   tab->m_rect = rcTab;

   CPoint pts[4];
   pts[0].x = rcTab.left;                   pts[0].y = rcTab.bottom;
   pts[1].x = rcTab.left;                   pts[1].y = rcTab.top;
   pts[2].x = rcTab.right;                  pts[2].y = rcTab.top;
   pts[3].x = rcTab.right+rcTab.Height()/2; pts[3].y = rcTab.bottom;
   dc->Polygon(pts, 4);

   rcText.SetRect( rcTab.left+4, rcTab.top, rcTab.right-4, rcTab.bottom );
   dc->DrawText( tab->m_label.Get(), &rcText, DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_CENTER);
}

int CHashTabCtrl::AddTab( const String &label, void *data )
{
   HashTab tab( label, data );

   m_tabs.Add( tab );
   SetActiveTab( m_tabs.GetSize()-1 );

   Invalidate( FALSE );

   return m_tabs.GetSize();
}

void CHashTabCtrl::RemoveTab( const String &label )
{
   for(int i=0;i<m_tabs.GetSize();i++){
	  if( m_tabs[i].m_label == label ){
         m_tabs.RemoveAt(i);
         SetActiveTab( m_tabs.GetSize()-1 );
         Invalidate( FALSE );
         break;
      }
   }
}

void CHashTabCtrl::SetActiveTab( int index )
{
   if (index >= 0 && index < m_tabs.GetSize()){
      m_curtab = &m_tabs[index];
	  m_curtab->m_id = index;
   }
   else
      m_curtab = NULL;

   HASHTAB_NMHDR hdr;
   hdr.nmhdr.hwndFrom = GetSafeHwnd();
   hdr.nmhdr.idFrom = GetDlgCtrlID();
   hdr.nmhdr.code = TCN_SELCHANGE;
   hdr.tab = m_curtab;
   if (GetParent())
      GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&hdr );

   Invalidate( FALSE );
}

void CHashTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
   CRect rcClient;
   GetClientRect( &rcClient );

   for (int i=0; i<m_tabs.GetSize(); i++) {
      if (point.x < m_tabs[i].m_rect.right) {
         SetActiveTab( i );
         return;
      }
   }
}

int CHashTabCtrl::GetActiveTab()
{
   for(int index = 0; index < m_tabs.GetSize(); index++){
      if(m_curtab == &m_tabs[index])
		  return index;
   }
   return 0;
}