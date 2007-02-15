// BC  6/23/2003  \Bob105\HashApp\TreeObWnd.cpp

#include "StdAfx.h"
#include "Hxt.h"
#include "F_Protos.h"
#include "sdk\HPropert.h"
#include "PropertyWnd.h"
#include "Resource.h"
#include "ModelExt.h"

#define ID_EDIT_REFTREE				(1)        //any old ID will do

#define ITEMHEIGHT       16
#define HALFITEMHEIGHT   ITEMHEIGHT/2

UINT           CPropertyWnd::m_hitflags;
UINT           CPropertyWnd::m_hitid;
UINT           CPropertyWnd::m_timerid;
BOOL           CPropertyWnd::m_isnoupdate;
CPoint         CPropertyWnd::m_ptHotSpot;
CPoint         CPropertyWnd::m_ptDown;

#define SCROLLTIMERID        1
#define SLOWSCROLL_FREQUENCY 3

extern CHxtApp theApp;

BEGIN_MESSAGE_MAP(CPropertyWnd, CWnd)
   ON_WM_PAINT()

   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_MBUTTONDOWN()
   ON_WM_RBUTTONDOWN()
   ON_WM_RBUTTONUP()
   ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CPropertyWnd::CPropertyWnd( HProperty *property, CPoseTree *tree, CWnd *parent, const CRect &rect )
{
   m_property = property;
   m_propertyext = GetPropertyExt(m_property);
   m_tree = tree;

   DWORD style = WS_VISIBLE|WS_CLIPCHILDREN;

   if (!parent) {
      parent = AfxGetMainWnd();
      style |= WS_POPUP;
   }
   else
      style |= WS_CHILD;

   CWnd::Create( NULL, NULL, WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN, rect, parent, 0xFFFF );

   EnableToolTips();
}

CPropertyWnd::~CPropertyWnd()
{
   if (m_tree)
      m_tree->TotalRefresh();

   if (m_propertyext)
      m_propertyext->m_wnd = NULL;
}

HProperty *GetParentProperty(HProperty *property)
{
   HTreeObject *parent = property->GetParent();
   if (parent)
      return parent->IsProperty();//Doesn't exist
   else
      return NULL;
}

/*
HashPtrObject **CTreeObjectWnd::GetPtrAt( int index )
{
   if (index == 0)
      return (HashPtrObject **)&m_object;
   else
      return NULL;
}

BOOL CTreeObjectWnd::OnPtrMessage( PtrMessage ptrmessage, HashPtrObject **ptruplink, LONG lparam, Time time )
{
   switch ( ptrmessage ) {
      case PTM_DELETED:
         delete this;
         return TRUE;
      case PTM_RENAMED:
      case PTM_SELECTIONCHANGED:
         Invalidate( FALSE );
         return TRUE;
      case PTM_NULLCHANGED:
      case PTM_VALUESTORED:
      case PTM_VALUECHANGED:
         if (!m_isnoupdate && m_object->IsProperty()) {// colors need to update when rgbs are changed
            Invalidate( FALSE );
            UpdateWindow();
         }
         return TRUE;
   }
   return FALSE;
}
*/

HTreeObject *CPropertyWnd::HitTestTreeItem(const CPoint &pt, const CRect &rcItem, UINT &nFlags, UINT &nID)
{
   CRect rcThis( rcItem.left, rcItem.top, rcItem.right, rcItem.top+m_propertyext->GetDefaultHeight());

   if (rcThis.PtInRect(pt)) { // FOUND OBJECT
      nFlags = HTVHT_PROPERTY;

      if (pt.x < rcThis.left) {
         nFlags |= TVHT_ONITEMINDENT;
         return m_property;
      }

      BOOL dragspot=FALSE;
      HProperty *parent = GetParentProperty(m_property);
      if (parent) {
         CRect rc(rcThis.left, rcThis.top+2, rcThis.left+13, rcThis.top+15);
         if (rc.PtInRect(pt)) {
            dragspot = TRUE;
            nFlags |= HTVHT_DRAGSPOT;   
         }
         rcThis.left += 18;
      }

      if (!dragspot) {
         if (pt.x < rcThis.left+SIZE_VARBK+SIZE_VAREXPAND && GetFirstTreeProperty(m_property))
            nFlags |= HTVHT_ONITEMPROPERTYBUTTON;
         else {
            CRect rcText = rcThis;
            rcText.left += SIZE_VARBK;
            if (GetFirstTreeProperty(m_property))
               rcText.left += SIZE_VAREXPAND;
            CDC *dc = m_tree->GetDC();
            CFont *oldfont = dc->SelectObject( m_tree->m_font );
            String stemp = (String)m_property->GetName() + " *";
            dc->DrawText( stemp.Get(), rcText, DT_CALCRECT|DT_SINGLELINE|DT_END_ELLIPSIS );
            dc->SelectObject( oldfont );
            m_tree->ReleaseDC( dc );
            if (pt.x <= rcText.right)
               nFlags |= TVHT_ONITEMLABEL;
            else
               nFlags |= TVHT_ONITEMRIGHT;
         }
      }
      return m_property;
   }
   else if (m_propertyext->IsExpanded()) {
      int top = rcThis.bottom;
      CRect rcVar;

      for (HProperty *property=(HProperty *)GetFirstTreeProperty(m_property); property; property=(HProperty *)property->GetTreeSibling(HTreeObject::SKIPINVISIBLE|HTreeObject::SKIPFILTERED|HTreeObject::SKIPADVANCED)) {
         m_tree->GetTreeItemRect( property, rcVar );
         rcVar.OffsetRect( rcItem.left+SIZE_INDENT, top );
         rcVar.right = rcItem.right-6;

         if (rcVar.PtInRect(pt)) { // FOUND OBJECT
            HTreeObject *hit = HitTestTreeItem( pt, rcVar, nFlags, nID );
            if (hit)
               return hit;
         }
         if (pt.y < rcVar.bottom)
            break;

         top = rcVar.bottom;
      }
   }
   return NULL;
}

UINT CPropertyWnd::HitTest( CPoint pt, UINT &nFlags )
{
   CRect rcClient;
   GetClientRect( &rcClient );
   UINT nID = 0;
   nFlags = 0;

   HitTestTreeItem( pt, rcClient, nFlags, nID );

   return nID;
}

int CPropertyWnd::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
   static String text, prevtext;

   String name;
   if (m_hitflags & TVHT_ONITEMLABEL)
      name = m_property->GetName();

   if (m_hitid) { //|| !name.IsEmpty()) {
      text = GetResourceString( m_hitid );

      pTI->lpszText = _tcsdup(text.Get());
      if (text != prevtext) {
         prevtext = text;
         return -1;
      }

      pTI->hwnd = m_hWnd;
      pTI->uId = 0;
      pTI->uFlags = 0;
 
      CRect rcClient;
      GetClientRect( rcClient );
      pTI->rect = rcClient;

   	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
   	CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;

#if _MFC_VER >= 0x0600
      pToolTip->SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
#endif
      pToolTip->SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
      pToolTip->SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 600);
      pToolTip->SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 600);

      return 1;
   }
   else
      return -1;
}

void CPropertyWnd::OnPaint()
{
   CPaintDC paintDC(this);
   CDC *pDC = &paintDC;

   CRect rcClient;
   GetClientRect( &rcClient );

   CBitmap bmp, *pBmpOld = NULL;
   CDC memDC;
   if (memDC.CreateCompatibleDC(pDC) && bmp.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height())) {
      pBmpOld = memDC.SelectObject(&bmp);
      pDC = &memDC;
   }

   CFont *pOldFont = pDC->SelectObject( m_tree->m_font );
   pDC->SetBkMode( TRANSPARENT );

   DrawProperty(pDC, rcClient );

   pDC->SelectObject( pOldFont );

   if (pDC == &memDC) {
      paintDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
      memDC.SelectObject(pBmpOld);
   }
}

void CPropertyWnd::DrawProperty(CDC *pDC, const CRect &rcItem)
{
   int left = rcItem.left + SIZE_VARBK;
   int itemheight = GetDefaultHeight();

   ColorBuf bkcolor = GetSysColor(COLOR_BTNFACE);
   bkcolor.values.red   = min(255, bkcolor.values.red  +20);
   bkcolor.values.green = min(255, bkcolor.values.green+20);
   bkcolor.values.blue  = min(255, bkcolor.values.blue +20);
   pDC->FillSolidRect( &rcItem, bkcolor );
   ColorBuf color = bkcolor;
   color.values.red   -= 8;
   color.values.green -= 8;
   color.values.blue  -= 8;
   CRect rcStripe( rcItem.left, rcItem.top+4, rcItem.right, rcItem.top+itemheight-5 );
   pDC->FillSolidRect( &rcStripe, color );

//TODO
//   if (m_property->m_isselected)
//      dc->FillSolidRect( CRect(left-1, rcItem.top+2, rcItem.right-1, rcItem.top+itemheight-2), RenderInfo::appearance[HC_MANIPULATORS].color);

   // DRAW DRAG ICON IF USER PROPERTY
   HProperty *parentproperty = m_property->GetParentProperty();
   if (m_property->GetObjectType() != HOT_VIRTUALPROPERTY && m_property->IsUserProperty() && parentproperty && parentproperty->IsUserProperty()) {
      CRect rc(left, rcItem.top+2, left+13, rcItem.top+15);
      pDC->Draw3dRect( rc, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
      rc.DeflateRect( 1, 1 );
      pDC->FillSolidRect( rc, bkcolor );
      static HICON iconDrag;
      if (!iconDrag)
         iconDrag = theApp.LoadIcon( ID_DRAG );
      pDC->DrawIcon( left, rcItem.top, iconDrag );
      left += 18;\
   }
   // DRAW DATA TRIANGLE IF NECESSARY
   if (GetFirstTreeProperty(m_property)) {
      CPoint triangle[3];
      if (m_propertyext->IsExpanded()) {
         triangle[0] = CPoint(left+0, rcItem.top+7);
         triangle[1] = CPoint(left+8, rcItem.top+7);
         triangle[2] = CPoint(left+4, rcItem.top+11);
      }
      else {
         triangle[0] = CPoint(left+2, rcItem.top+4);
         triangle[1] = CPoint(left+2, rcItem.top+12);
         triangle[2] = CPoint(left+6, rcItem.top+8);
      }
      pDC->SelectObject( &m_tree->m_textpen );
      pDC->Polygon( triangle, 3 );
      left += SIZE_VAREXPAND;
   }

   // DRAW THE TEXT
   CRect rcText( left, rcItem.top+2, rcItem.right-GetTreeItemControlWidth(pDC), rcItem.top+itemheight-1 );
   DrawTextLabel( pDC, rcText );

   // DRAW SUB-PROPERTIES IF EXPANDED
   if (m_propertyext->IsExpanded()) {
      left -= SIZE_VAREXPAND;
      rcText.left = left;

      pDC->ExcludeClipRect( rcText );
      CRect rcFrame( left+4, rcItem.top+8, rcItem.right-2, rcItem.bottom-2 );
      pDC->Draw3dRect( &rcFrame, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
      rcFrame.DeflateRect(1,1);
      pDC->Draw3dRect( &rcFrame, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
   }
   else if (m_property->IsNull()) {
      rcText.SetRect( rcItem.left, rcItem.top+2, rcItem.right-4, rcItem.bottom-1 );
//      COLORREF textcolor = GetEditBoxTextColor();
      COLORREF textcolor = GetSysColor(COLOR_WINDOWTEXT);
      COLORREF windowcolor = GetSysColor(COLOR_BTNFACE);
      textcolor = RGB((GetRValue(windowcolor)*2+GetRValue(textcolor))/3,
                      (GetGValue(windowcolor)*2+GetGValue(textcolor))/3,
                      (GetBValue(windowcolor)*2+GetBValue(textcolor))/3);
      pDC->SetTextColor( textcolor );
      CString  notset;
      notset.LoadString(IDS_NOTSET);

      pDC->DrawText( notset, rcText, DT_SINGLELINE|DT_RIGHT );
   }
}

void CPropertyWnd::DrawTextLabel( CDC *dc, CRect &rcText, BOOL setcolor/*=TRUE*/, BOOL fullname/*=FALSE*/ )
{
   if (setcolor)
      dc->SetTextColor( GetSysColor(COLOR_WINDOWTEXT));

   String stemp;
   if (fullname)
      stemp = m_property->GetFullName();
   else
      stemp = m_property->GetName();

   if (m_property->IsModified())
      stemp += " *";

   rcText.right -= 3;
   if (dc->DrawText( stemp.Get(), rcText, DT_CALCRECT|DT_SINGLELINE|DT_END_ELLIPSIS )) {
      rcText.right += 3;
      rcText.bottom++;
      dc->DrawText( stemp.Get(), rcText, DT_SINGLELINE|DT_END_ELLIPSIS );
   }
   else
      rcText.right = rcText.left;
}

void CPropertyWnd::DrawValue( CDC *dc, const CRect &rcItem, const String &value )
{
   CRect rcText( rcItem.left, rcItem.top+2, rcItem.right-6, rcItem.bottom-1 );

   if (m_propertyext->IsExpanded()) {
      ColorBuf color;
      color = GetSysColor(COLOR_BTNFACE);
      color.values.red   = min(255, color.values.red  +20)-8;
      color.values.green = min(255, color.values.green+20)-8;
      color.values.blue  = min(255, color.values.blue +20)-8;

      CRect rcTemp = rcText;
      dc->DrawText( value.Get(), rcTemp, DT_CALCRECT|DT_SINGLELINE|DT_RIGHT|DT_NOPREFIX );
      int itemheight = m_propertyext->GetDefaultHeight();
      CRect rcStripe( rcText.right-rcTemp.Width(), fast_ftol_signed(rcItem.top+itemheight*.25f), rcText.right+1, fast_ftol_signed(rcItem.top+itemheight*.75f) );
      dc->FillSolidRect( &rcStripe, color );
   }
   dc->SetTextColor( GetSysColor(COLOR_WINDOWTEXT) );
   dc->DrawText( value.Get(), rcText, DT_SINGLELINE|DT_RIGHT|DT_NOPREFIX );
//TODO
//   if (m_iskey/* || (IsCache() && !IsDefaultValue())*/) { // don't know if I like showing nondefault values bold
//      rcText.OffsetRect( -1, 0 );
//      dc->DrawText( value.Get(), rcText, DT_SINGLELINE|DT_RIGHT|DT_NOPREFIX );
//   }
}


static float  m_mousedowntime;
static BOOL   m_isobjecthandlingmouse, m_editlabelonup, m_isdragable, m_isdragging, m_startedmoving, m_isallowdblclick;
static HProperty *m_prevlbuttondownproperty;

void CPropertyWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
 	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   m_ptDown = point;

   SetCapture();
   if (GetFocus()!=m_tree)
      m_tree->SetFocus();

   float time = GetClockTicks();
   float timesincelastclick = time-m_mousedowntime;
   float dblclktime = (float)GetDoubleClickTime()/1000.0f;
   m_mousedowntime = time;
   m_startedmoving = FALSE;
   m_isallowdblclick = FALSE;

   m_prevlbuttondownproperty = m_property;
   if (m_hitflags & HTVHT_ONITEMPROPERTYBUTTON) {
      m_tree->ExpandProperties(m_property, !m_propertyext->IsExpanded());
   }
}


void CPropertyWnd::OnMouseMove(UINT nFlags, CPoint point)
{
   if (!(nFlags & MK_LBUTTON || nFlags & MK_MBUTTON || nFlags & MK_RBUTTON)) {
      m_hitid = HitTest(point, m_hitflags);
      return;
   }
      
//   if (m_isobjecthandlingmouse)
      //m_object->OnTreeMouseMove( this, nFlags, point );
}

void CPropertyWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
   ReleaseCapture();

   //m_object->OnTreeLButtonUp( this, nFlags, point );
//      m_isobjecthandlingmouse = FALSE;
}

void CPropertyWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
/*
   if (!m_isallowdblclick) {
      OnLButtonDown( nFlags, point );
      return;
   }
*/
}

void CPropertyWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
   ClientToScreen( &point );
   m_tree->ScreenToClient( &point );

   m_tree->OnMButtonDown( nFlags, point );
}

void CPropertyWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
   if (GetFocus() != m_tree)
      m_tree->SetFocus();

/*
   if (m_object->m_isselected)
      GetApp().SetFocusObject( m_object );
   else
      GetApp().Select( m_object, TRUE, TRUE ); //for context menu not set in pose window with rotate or other property selected

   if (m_object)
      m_object->ShowContextMenu( this, point );
*/
}

//*************************************************************************************
//***  CPercentWnd
//*************************************************************************************
BEGIN_MESSAGE_MAP(CPercentWnd, CPropertyWnd)
END_MESSAGE_MAP()

void CPercentWnd::DrawProperty(CDC *dc, const CRect &rcItem)
{
   CPropertyWnd::DrawProperty( dc, rcItem );

   if (!m_property->IsNull()) 
      DrawValue( dc, rcItem, m_property->ToString() );

   int left = rcItem.left + SIZE_VARBK;
   int right = rcItem.right - SIZE_VARBK;

   CRect rcBar( left, rcItem.bottom-10, right, rcItem.bottom-2 );
   dc->Draw3dRect( &rcBar, GetSysColor(COLOR_WINDOWFRAME), GetSysColor(COLOR_WINDOWFRAME));
   rcBar.DeflateRect(1,1);
   dc->Draw3dRect( &rcBar, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
   rcBar.DeflateRect(1,1);
   dc->FillSolidRect( rcBar, GetSysColor(COLOR_BTNFACE));

   if (m_property->IsNull())
      return;

   rcBar.right--;

   HFloatPropertyInfo *floatinfo = (HFloatPropertyInfo *)m_property->GetPropertyInfo();
   float minvalue = floatinfo->GetMinValue();
   float maxvalue = floatinfo->GetMaxValue();
   float span = maxvalue - minvalue;
   int zeropos = max(rcBar.left, min(rcBar.right, rcBar.left + fast_round(rcBar.Width() * (-minvalue/span))));
   int valuepos = rcBar.left + fast_round(rcBar.Width() * (((HPercentProperty *)m_property)->GetValue()-minvalue)/span);

   int red, green, blue;
   red = 255, green = 255, blue = 255;
   if (valuepos > zeropos) {
      float pixelspan = (float)(rcBar.right - zeropos);
      for (int x=zeropos; x<valuepos; x+=3) {
         float value = (x-zeropos)/pixelspan;
         dc->FillSolidRect( CRect( x, rcBar.top, min(valuepos,x+3), rcBar.bottom ), RGB(fast_round(red*value), fast_round(green*value), fast_round(blue*value)));
      }
   }
   else
      dc->FillSolidRect( CRect( zeropos, rcBar.top, valuepos, rcBar.bottom ), RGB(0,0,0));

   CRect rcKnob( valuepos-1, rcBar.top-2, valuepos+2, rcBar.bottom+2 );
   dc->Draw3dRect( &rcKnob, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
   dc->FillSolidRect( valuepos,   rcKnob.top+1, 1, rcKnob.Height()-1, GetSysColor(COLOR_BTNFACE));
}

int CPercentWnd::GetTreeItemControlWidth(CDC *dc)
{
   CRect rect;
   dc->DrawText( m_property->ToString().Get(), rect, DT_CALCRECT|DT_SINGLELINE|DT_RIGHT|DT_NOPREFIX );
   return rect.Width() + 3;
}

//*************************************************************************************
//***  CBoolWnd
//*************************************************************************************
BEGIN_MESSAGE_MAP(CBoolWnd, CPropertyWnd)
END_MESSAGE_MAP()

void CBoolWnd::DrawProperty(CDC *dc, const CRect &rcItem)
{
   CPropertyWnd::DrawProperty( dc, rcItem );

   if (m_property->IsNull())
      return;

   DrawValue( dc, rcItem, m_property->ToString() );
}

int CBoolWnd::GetTreeItemControlWidth(CDC *dc)
{
   CRect rect;
   dc->DrawText( m_property->ToString().Get(), rect, DT_CALCRECT|DT_SINGLELINE|DT_RIGHT|DT_NOPREFIX );
   return rect.Width() + 3;
}

//*************************************************************************************
//***  CUserCategoryWnd
//*************************************************************************************
BEGIN_MESSAGE_MAP(CUserCategoryWnd, CPropertyWnd)
END_MESSAGE_MAP()

