// Dan  7/7/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\KeyPose\HashTreeBase.cpp

#include "StdAfx.h"
#include "PropertyWnd.h"
#include "Hxt.h"
#include "sdk\HHashObj.h"
#include "ModelExt.h"

extern CHxtApp theApp;

IMPLEMENT_DYNCREATE(CPoseTree, CWnd)

BEGIN_MESSAGE_MAP(CPoseTree, CWnd)
   ON_WM_CREATE()
   ON_WM_SIZE()

   ON_WM_HSCROLL()
   ON_WM_VSCROLL()
   ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
   ON_WM_MBUTTONDOWN()
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONUP()

   ON_WM_ERASEBKGND()
   ON_WM_NCCALCSIZE()
   ON_WM_NCHITTEST()
   ON_WM_NCPAINT()
   ON_WM_NCLBUTTONDOWN()
   ON_WM_NCLBUTTONDBLCLK()

   ON_WM_PAINT()
END_MESSAGE_MAP()

CPoseTree::CPoseTree()
{
   m_xoffset = m_yoffset = 0;
   m_indent = 0;
   m_flags = 0L;
   m_needsrecalcscroll = m_needsadjustprop = TRUE;
   m_startobj = NULL;
}

CPoseTree::~CPoseTree()
{
   RemoveAllPropertyWnd();
}

HProperty *CPoseTree::GetNextObject( HProperty *prev )
{
   return (HProperty *)prev->GetTreeSibling( HTreeObject::SKIPINVISIBLE );
}

BOOL CPoseTree::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style |= WS_CLIPCHILDREN;
   return CWnd::PreCreateWindow(cs);
}

BOOL CPoseTree::Create( const RECT& rect, CWnd* pParentWnd, UINT nID )
{
   DWORD dwstyle = WS_CHILD|WS_VISIBLE;
   return CWnd::Create( NULL, NULL, dwstyle, rect, pParentWnd, nID );
}

int CPoseTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   int rvalue = CWnd::OnCreate( lpCreateStruct );

   static CFont font;
   if (!font.m_hObject) {
      m_font = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
      if (!m_font)
         m_font = CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT));

      LOGFONT logfont;
      m_font->GetLogFont( &logfont );
      logfont.lfHeight = -11;

      font.CreateFontIndirect( &logfont );
   }
   m_font = &font;

   m_textpen.CreatePen( PS_SOLID, 0, GetSysColor(COLOR_WINDOWTEXT));

   return rvalue;
}

BOOL CPoseTree::PreTranslateMessage(MSG* pMsg)
{
   if (pMsg->message == WM_KEYDOWN) {
      if (OnKeyDown(pMsg->wParam))
         return TRUE;
/*         
      else if (GetMainFrame()->PreTranslateMessage( pMsg ))
         return TRUE;
      ::TranslateMessage( pMsg );
      ::DispatchMessage( pMsg );
      return TRUE;
      */
   }   
//   else
      return CWnd::PreTranslateMessage(pMsg);
}

BOOL CPoseTree::OnKeyDown( UINT nChar )
{
   switch(nChar) {
      case VK_ESCAPE:
         if ( GetParent() )
            GetParent()->SetFocus();
         return TRUE;
      default:
         return FALSE;
   }
}

BOOL CPoseTree::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
/*
   if (GetMainFrame()->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ))
      return TRUE;

   if (TreeObject::m_contextmenuwnd && nCode==CN_COMMAND && !pExtra) {
      BOOL rvalue = TreeObject::m_contextmenuwnd->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
      TreeObject::m_contextmenuwnd = NULL;
      return rvalue;
   }
   else
*/   
      return CWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CPoseTree::OnSize(UINT nType, int cx, int cy)
{
   static BOOL isinonsize;

   if (isinonsize)
      return;
   isinonsize = TRUE;

   CWnd::OnSize(nType, cx, cy);

//TODO
//   HTreeObject *obj = GetApp().GetFocusObject();
//   if (obj && TreeObject::VISIBLE == obj->IsVisibleInTree(this))
      //EnsureVisible(obj);

   if (IsWindow(GetSafeHwnd()) && IsWindowVisible()) {
      RecalcScrollBars();
      AdjustAllPropertyWnd();
      Invalidate(FALSE);
   }

   isinonsize = FALSE;
}

CRect CPoseTree::GetScrollRect()
{
   CRect rcScroll;
   GetClientRect( &rcScroll );
   rcScroll.OffsetRect( 2, 2 );
   rcScroll.left = rcScroll.right;
   rcScroll.right += SCROLLBARSIZE;

   return rcScroll;
}

void CPoseTree::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
   // We override WM_NCCALCSIZE to increase the non-client area to include space for a border.
   CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

   lpncsp->rgrc[0].left += 2;
   lpncsp->rgrc[0].right -= 2;
   lpncsp->rgrc[0].top += 2;
   lpncsp->rgrc[0].bottom -= 2;
}

void CPoseTree::OnNcPaint()
{
   CWindowDC dc(this);

   CRect rc;
   GetWindowRect(&rc);
   rc.OffsetRect( -rc.left, -rc.top );

   ::DrawEdge(dc.m_hDC, rc, BDR_SUNKENINNER|BDR_SUNKENOUTER, BF_RECT);

   CWnd::OnNcPaint();
}

BOOL CPoseTree::OnEraseBkgnd(CDC* pDC) 
{
   CRect rcClient;
   GetClientRect( &rcClient );

   ColorBuf color = GetSysColor(COLOR_BTNFACE);
   color.values.red   = min(255, color.values.red  +20);
   color.values.green = min(255, color.values.green+20);
   color.values.blue  = min(255, color.values.blue +20);
   pDC->FillSolidRect( rcClient, color );

   return TRUE;
}

void CPoseTree::OnPaint()
{
   if (m_needsrecalcscroll) 
      RecalcScrollBars();
   if (m_needsadjustprop)
      AdjustAllPropertyWnd();

   CPaintDC paintDC(this);
}

BOOL CPoseTree::GetTreeItemRect( HProperty *property, CRect &rcItem, HProperty *find/*=NULL*/, HProperty *caller/*=NULL*/)
{
   if (!caller)
      caller = property;

   PropertyExt *ext = GetPropertyExt(caller);

   CRect rcClient;
   GetClientRect( &rcClient );

   rcItem.SetRect( 0, 0, rcClient.right, ext->GetDefaultHeight());

   if (ext->IsExpanded()) {
      CRect rcSubItem;
      for (HProperty *child=GetFirstTreeProperty(caller); child; child=(HProperty *)child->GetTreeSibling(HTreeObject::SKIPINVISIBLE)) {
         BOOL found = GetTreeItemRect( child, rcSubItem, find );
         rcSubItem.OffsetRect( rcItem.left, rcItem.bottom );
         if (found) {
            rcItem = rcSubItem;
            return TRUE;
         }
         rcItem.UnionRect( rcItem, rcSubItem );
      }
      rcItem.bottom += 6; // ENDCAP FOR EXPANDED PROPERTIES
   }
   return find == property;
}

CPropertyWnd *CPoseTree::CreatePropertyWnd(HProperty *property, CWnd *parent, const CRect &rc)
{
   UINT hot = property->GetObjectType();
   switch (property->GetObjectType()) {
   case HOT_PERCENTPROPERTY:
      return new CPercentWnd(property, this, parent, rc);
   case HOT_BOOLPROPERTY:
      return new CBoolWnd(property, this, parent, rc);
   case HOT_USERCATEGORYPROPERTY:
      return new CUserCategoryWnd(property, this, parent, rc);
   default:
      return new CPropertyWnd(property, this, parent, rc);
   }
}

void CPoseTree::AdjustPropertyWnd( HProperty *property, CWnd *parent, const CRect &rcItem )
{
   PropertyExt *ext = GetPropertyExt(property);
   CRect rcWnd( rcItem.left, rcItem.top, rcItem.right, rcItem.bottom );
   if (!ext->m_wnd) {
      ext->m_wnd = CreatePropertyWnd(property, parent, rcWnd );
      ext->m_wnd->Invalidate(FALSE);
   }
   else {
      CRect rcNow;
      ext->m_wnd->GetWindowRect( &rcNow );
      ScreenToClient( &rcNow );
      if (rcNow != rcWnd) {
         ext->m_wnd->MoveWindow( rcWnd, TRUE );
         ext->m_wnd->Invalidate(FALSE);
         ext->m_wnd->UpdateWindow();
      }
   }

   if (ext->IsExpanded()) {
      int top = ext->m_wnd->GetDefaultHeight() + 1;
      CRect rcVar, rcTreeClient, rcItemClient;
      GetClientRect( &rcTreeClient );
      ext->m_wnd->GetClientRect( &rcItemClient );

      int leftmargin, rightmargin;
      leftmargin = 11;
      rightmargin = 6;

      for (HProperty *child=GetFirstTreeProperty(property); child; child=(HProperty *)child->GetTreeSibling(HTreeObject::SKIPINVISIBLE)) {
         GetTreeItemRect( child, rcVar );
         rcVar.OffsetRect( 0, top );
         rcVar.left = rcItemClient.left+leftmargin;
         rcVar.right = rcItemClient.right-rightmargin;

         if (rcVar.bottom>=rcItemClient.top && rcVar.top<rcItemClient.bottom)
            AdjustPropertyWnd( child, ext->m_wnd, rcVar );
         else
            RemovePropertyWnd( child );

         top = rcVar.bottom;
      }
   }
}

void CPoseTree::AdjustAllPropertyWnd( HProperty *obj )
{
   static CRect rcClient;
   static int left, top;

   if (!obj) {
      obj = GetStartObject();
      GetClientRect( &rcClient );
      left = rcClient.left - m_xoffset;
      top = rcClient.top - m_yoffset;
      m_needsadjustprop = FALSE;
   }

   PropertyExt *propext = GetPropertyExt(obj);

   CRect rcItem;
   for (; obj; obj=GetNextObject(obj)) {
      GetTreeItemRect( obj, rcItem );
      rcItem.OffsetRect( left, top );
      rcItem.right = rcClient.right;

      if (rcItem.bottom>=rcClient.top && rcItem.top<rcClient.bottom)
         AdjustPropertyWnd( obj, this, rcItem );

      top = rcItem.bottom;

      if (propext->IsExpanded()) {
         HProperty *progeny = (HProperty *)obj->GetTreeProgeny();
         if (progeny) {
            left += SIZE_INDENT;
            AdjustAllPropertyWnd( progeny );
            left -= SIZE_INDENT;
         }
      }
   }
}

void CPoseTree::RemovePropertyWnd(HProperty *obj)
{
   PropertyExt *ext = GetPropertyExt(obj);
   if (ext->m_wnd) {
      if (ext->m_wnd->GetSafeHwnd())
         ext->m_wnd->DestroyWindow();
      delete ext->m_wnd;
      ext->m_wnd = NULL;
   }

   if (ext->IsExpanded()) {
      HProperty *child = (HProperty *)obj->GetTreeProgeny();
      if (child)
         RemoveAllPropertyWnd( child );
   }
}

void CPoseTree::RemoveAllPropertyWnd( HProperty *obj/*=NULL*/ )
{
   if (!obj)
      obj = GetStartObject();

   for (; obj; obj=GetNextObject(obj))
         RemovePropertyWnd(obj);
}

void CPoseTree::RecalcExtents( HProperty *prop )
{
   static int left, top;

   if (prop == NULL) {
      prop = GetStartObject();
      left = 0;
      top = 0;
   }
   
   CRect rcItem;
   for (; prop; prop=GetNextObject(prop)) {
      GetTreeItemRect( prop, rcItem );
      rcItem.OffsetRect( left, top );
      if (rcItem.right > m_extents.cx)
         m_extents.cx = 0;
      top = rcItem.bottom;

      PropertyExt *ext = GetPropertyExt(prop);

      if (ext->IsExpanded()) {
         HProperty *progeny = (HProperty *)prop->GetTreeProgeny();
         if (progeny) {
            left += SIZE_INDENT;
            RecalcExtents( progeny );
            left -= SIZE_INDENT;
         }
      }
   }

   m_extents.cy = top;
}

void CPoseTree::RecalcScrollBarsGuts()
{
   CRect rcClient, rcItem;
   GetClientRect( &rcClient );
   int width = rcClient.right - rcClient.left + 1;
   int height = rcClient.bottom - rcClient.top + 1;

   m_xoffset = max(0,min(m_xoffset, m_extents.cx-width+1));
   m_yoffset = max(0,min(m_yoffset, m_extents.cy-height+1));

   SCROLLINFO sbinfo;
   sbinfo.cbSize = sizeof( SCROLLINFO );
   sbinfo.fMask = SIF_ALL;//|SIF_DISABLENOSCROLL;
   sbinfo.nMin  = 0;
   sbinfo.fMask = SIF_ALL;
   sbinfo.nMax  = m_extents.cy;
   sbinfo.nPage = height;
   sbinfo.nPos  = m_yoffset;

   SetScrollInfo( SB_VERT, &sbinfo, TRUE );
}

void CPoseTree::RecalcScrollBars()
{
   RecalcExtents();

   RecalcScrollBarsGuts(); // call it twice because the first time could have turned a scroll bar,
   RecalcScrollBarsGuts(); // possibly causing the other to be needed

   m_needsrecalcscroll = FALSE;
}

void CPoseTree::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar )
{
   CRect rcClient;
   GetClientRect(rcClient);

   int oldoffset = m_xoffset;
   m_xoffset = GetScrollPos(SB_HORZ);
   switch (nSBCode) {
      case SB_LEFT:
         m_xoffset = 0;
         break;
      case SB_RIGHT:
         m_xoffset = m_extents.cx-rcClient.Width();
         break;
      case SB_LINEUP:
         m_xoffset -= 8;
         break;
      case SB_LINEDOWN:
         m_xoffset += 8;
         break;
      case SB_PAGERIGHT:
         m_xoffset -= rcClient.Width();
         break;
      case SB_PAGELEFT:
         m_xoffset += rcClient.Width();
         break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
         m_xoffset = nPos;
         break;
      default:
         break;
   }
   int min, max;
   GetScrollRange( SB_HORZ, &min, &max );
   m_xoffset = min( max(m_xoffset, min), max-rcClient.Width()+1 );
   int scrollamount = oldoffset-m_xoffset;
   ScrollWindow( scrollamount, 0, NULL, NULL );
   SetScrollPos( SB_HORZ, m_xoffset, TRUE );
   UpdateWindow();
}

static CPoint ptPrev;

void CPoseTree::OnMButtonDown(UINT nFlags, CPoint point) 
{
   if (GetFocus() != this/* && !CExpressionEdit::m_active*/)
      SetFocus();

   SetCapture();

   ptPrev = point;
   m_ismmoving = TRUE;
}

void CPoseTree::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (m_ismmoving) {
      ScrollVert( ptPrev.y - point.y );
      ptPrev = point;
   }
}

void CPoseTree::OnMButtonUp(UINT nFlags, CPoint point) 
{
   m_ismmoving = FALSE;
   ::ReleaseCapture();
}

BOOL CPoseTree::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
   ScrollVert( fast_ftol_signed(-(float)zDelta/WHEEL_DELTA * 18 ));

   return TRUE;
}

void CPoseTree::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar )
{
   CRect rcClient;
   GetClientRect(rcClient);

   int yoffset = GetScrollPos(SB_VERT);
   switch (nSBCode) {
      case SB_TOP:
         yoffset = 0;
         break;
      case SB_BOTTOM:
         yoffset = m_extents.cy-rcClient.Height();
         break;
      case SB_LINEUP:
         yoffset -= 18;
         break;
      case SB_LINEDOWN:
         yoffset += 18;
         break;
      case SB_PAGEUP:
         yoffset -= rcClient.Height();
         break;
      case SB_PAGEDOWN:
         yoffset += rcClient.Height();
         break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
         yoffset = nPos;
         break;
      default:
         break;
   }
   ScrollVert( yoffset-m_yoffset );
}

void CPoseTree::OnTimer( UINT timerid )
{
   if (timerid == VSCROLLTIMERID)
      m_vscroll->OnTimer();
}

void CPoseTree::ScrollVert( int amount )
{
   int min, max;
   GetScrollRange( SB_VERT, &min, &max );

   int oldoffset = m_yoffset;
   m_yoffset += amount;

   CRect rcClient;
   GetClientRect(rcClient);
   max = max( min, max-rcClient.Height() );

   m_yoffset = min( max(m_yoffset, min), max );

   int scrollamount = oldoffset-m_yoffset;
   ScrollWindow( 0, scrollamount, NULL, NULL );
   SetScrollPos( SB_VERT, m_yoffset, TRUE );
   AdjustAllPropertyWnd();
   UpdateWindow();
}

HProperty *CPoseTree::HitTestBranch( HProperty *obj, CPoint pt, UINT &nFlags )
{
   static CRect rcClient;
   static int left, top;

   if (!obj) {
      obj = GetStartObject();
      GetClientRect( &rcClient );
      left = rcClient.left - m_xoffset;
      top = rcClient.top - m_yoffset;
   }

   CRect rcItem;
   for (; obj; obj=GetNextObject(obj)) {
      GetTreeItemRect( obj, rcItem );
      rcItem.top = 0;
      rcItem.OffsetRect( left, top );
      rcItem.right = rcClient.right;

      if (pt.y>=rcItem.top && pt.y<rcItem.bottom) { // FOUND OBJECT
         rcItem.left += SIZE_GAP;
         if (!obj->GetTreeProgeny())
            rcItem.left += SIZE_EXPAND;
         if (pt.x < rcItem.left) {
            nFlags |= TVHT_ONITEMINDENT;
            return obj;
         }

         if (obj->GetTreeProgeny())
            rcItem.left += SIZE_EXPAND;
         if (pt.x < rcItem.left) {
            nFlags |= TVHT_ONITEMBUTTON;
            return obj;
         }
      }

      top = rcItem.bottom;

      PropertyExt *ext = GetPropertyExt(obj);
      if (ext->IsExpanded()) {
         HProperty *progeny = (HProperty *)obj->GetTreeProgeny();
         if (progeny) {
            left += SIZE_INDENT;
            HProperty *hit = HitTestBranch( progeny, pt, nFlags );
            if (hit)
               return hit;
            left -= SIZE_INDENT;
         }
      }
   }
   nFlags = TVHT_NOWHERE;
   return NULL;
}

HProperty *CPoseTree::HitTest( CPoint point, UINT &nFlags )
{
   nFlags = 0;

   CRect rcClient;
   GetClientRect( &rcClient );

   if (!rcClient.PtInRect(point)) {
      if (point.x < rcClient.left)
         nFlags |= TVHT_TOLEFT;
      else if (point.x > rcClient.right)
         nFlags |= TVHT_TORIGHT;
      if (point.y < rcClient.top)
         nFlags |= TVHT_ABOVE;
      else if (point.y > rcClient.bottom)
         nFlags |= TVHT_BELOW;
      return NULL;
   }

   return HitTestBranch( NULL, point, nFlags );
}

BOOL CPoseTree::GetItemRectBranch( HProperty *find, CRect &rcItem, HProperty *obj )
{
   static int top;
   if (!obj) {
      obj = GetStartObject();
      top = 0;
   }

   for (; obj; obj=GetNextObject(obj)) {
      BOOL found = GetTreeItemRect( obj, rcItem, find );
      if (found) {
         rcItem.OffsetRect( 0, top );
         return TRUE;
      }

      top += rcItem.bottom;

      PropertyExt *ext = GetPropertyExt(obj);

      if (ext->IsExpanded()) {
         HProperty *progeny = (HProperty *)obj->GetTreeProgeny();
         if (progeny) {
            if (GetItemRectBranch( find, rcItem, progeny ))
               return TRUE;
         }
      }
   }
   return FALSE;
}

BOOL CPoseTree::GetItemRect( HProperty *find, CRect &rcItem )
{
   if (!GetItemRectBranch( find, rcItem, NULL ))
      return FALSE;

   rcItem.OffsetRect( -m_xoffset, -m_yoffset );
   return TRUE;
}

static HProperty *GetArrowDownObjectBranch( CPoseTree *tree, HProperty *obj, HProperty *find, BOOL &found )
{
   for (; obj; obj=tree->GetNextObject(obj)) {
      if (found)
         return obj;
      if (obj == find)
         found = TRUE;

      PropertyExt *ext = GetPropertyExt(obj);
      if (ext->IsExpanded()) {
         HProperty *var = GetFirstTreeProperty(obj);
         if (var) {
            HProperty *next = GetArrowDownObjectBranch( tree, var, find, found );
            if (next)
               return next;
         }
      }

      if (ext->IsExpanded()) {
         HProperty *progeny = (HProperty *)obj->GetTreeProgeny();
         if (progeny) {
            HProperty *next = GetArrowDownObjectBranch( tree, progeny, find, found );
            if (next)
               return next;
         }
      }
   }
   return NULL;
}

HProperty *CPoseTree::GetArrowDownObject( HProperty *obj )
{
   BOOL found = FALSE;
   return GetArrowDownObjectBranch( this, GetStartObject(), obj, found );
}

static HProperty *GetArrowUpObjectBranch( CPoseTree *tree, HProperty *obj, HProperty *find, HProperty *&prev )
{
   for (; obj; obj=tree->GetNextObject(obj)) {
      if (obj == find)
         return prev;
      prev = obj;

      PropertyExt *ext = GetPropertyExt(obj);

      if (ext->IsExpanded()) {
         HProperty *var = GetFirstTreeProperty(obj);
         if (var)
            if (GetArrowUpObjectBranch( tree, var, find, prev ))
               return prev;
      }

      if (ext->IsExpanded()) {
         HProperty *progeny = (HProperty *)obj->GetTreeProgeny();
         if (progeny)
            if (GetArrowUpObjectBranch( tree, progeny, find, prev ))
               return prev;
      }
   }
   return NULL;
}

HProperty *CPoseTree::GetArrowUpObject( HProperty *obj )
{
   HProperty *prev = (HProperty *)obj->GetTreeParent();
   return GetArrowUpObjectBranch( this, (HProperty *)obj->GetTreeParent(), obj, prev );
}

void CPoseTree::ExpandProperties(HProperty *obj, BOOL expand)
{
  if (!obj->GetPropertyAt(0))
      return;

   PropertyExt *ext = GetPropertyExt(obj);

   ext->SetExpanded( expand );
   RecalcScrollBars();

   if (expand) {
      CRect rcClient, rcItem;
      GetClientRect( rcClient );
      GetItemRect( obj, rcItem );
      if (rcItem.bottom > rcClient.bottom) {
         int maxamount = rcItem.top - rcClient.top;
         int scrollamount = min(rcItem.bottom - rcClient.bottom, maxamount);
         ScrollVert( scrollamount );
      }
      Invalidate(FALSE);
   }
   else {
      Invalidate(TRUE);
   }

   if (!expand)
      RemovePropertyWnd( GetFirstTreeProperty(obj));
   AdjustAllPropertyWnd();
}

void CPoseTree::TotalRefresh()
{
   m_needsrecalcscroll = m_needsadjustprop = TRUE;
   if (IsWindowVisible())
      Invalidate( FALSE );
}

void CPoseTree::EnsureVisible(HProperty *obj)
{
   BOOL repaint = FALSE;
   HProperty *prevobj = obj;

   for (HProperty *parent=(HProperty *)obj->GetTreeParent(); parent; parent=(HProperty *)parent->GetTreeParent()) {
      PropertyExt *ext = GetPropertyExt(parent);
      if (!ext->IsExpanded()) {
         ext->SetExpanded(TRUE);
         repaint = TRUE;
      }
      prevobj = parent;
   }

   CRect rcClient;
   GetClientRect( rcClient );
   CRect rcItem;
   GetItemRect( obj, rcItem );
   if (rcItem.top < rcClient.top) {
      m_yoffset += rcItem.top;
      repaint = TRUE;
   }
   else if (rcItem.bottom > rcClient.bottom) {
      int maxamount = rcItem.top - rcClient.top;
      m_yoffset += min(rcItem.bottom - rcClient.bottom, maxamount);
      repaint = TRUE;
   }

   if (repaint) 
      TotalRefresh();
}

void CPoseTree::Select(HProperty *property, BOOL select/*=TRUE*/, BOOL setfocus/*=TRUE*/)
{
//TODO
//   GetApp().Select( obj, select, setfocus );
}

/*
BOOL CPoseTree::OnPtrMessage( PtrMessage ptrmessage, HashPtrObject **ptruplink, LONG lparam, Time time )
{
   if (ptruplink == (HashPtrObject **)&m_startobj) {
      switch ( ptrmessage ) {
         case PTM_DELETED:
            m_startobj = NULL;
            return TRUE;
         default:
            return FALSE;
      }
   }
   return FALSE;
}
*/
