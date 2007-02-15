#include "stdafx.h"
#include "Snipe.h"
#include "GLTreeControl.h"
#include "GLRenderWnd.h"
#include "Resource.h"

GLImageList CGLTreeControl::m_glImageListExpandIcons;

CGLTreeControl::CGLTreeControl()
{
   m_pCaretItem = NULL;
   m_bNeedsRecalcLayout = FALSE;
}

CGLTreeControl::~CGLTreeControl()
{
}

CGLTreeItem *CGLTreeControl::InsertItem( LPARAM lParam, CGLTreeItem *pParent/*=NULL*/, CGLTreeItem *pInsertAfter/*=GLTI_LAST*/ )
{
   CGLTreeItem *pItem = AllocItem( this, lParam );
   CGLControl *pPar = pParent;
   if (!pPar)
      pPar = this;
   pItem->Init( pPar, NULL, CPoint(0,0), CSize(m_szControl.cx, -1), lParam, JUST_LEFT, pInsertAfter );

   if (pParent)
   {
      if (pParent->IsExpanded())
      {
         for (CGLControl *pControl=pParent; pControl; pControl=pControl->m_pParentControl)
            if (pControl->IsTreeItem())
               pControl->m_szControl.cy = -1; // this will cause Move to update it's new size
      }
      else
      {
         pParent->m_szControl.cy = -1;
         pItem->SetVisible( FALSE );
      }
   }

   m_bNeedsRecalcLayout = TRUE;
   Invalidate();

   return pItem;
}

void CGLTreeControl::RemoveItem( CGLTreeItem *pItem )
{
   if (!pItem)
      return;

   for (CGLControl *pControl=pItem->m_pParentControl; pControl; pControl=pControl->m_pParentControl)
      if (pControl->IsTreeItem())
         pControl->m_szControl.cy = -1; // this will cause Move to update it's new size

   if (pItem)
      delete pItem;

   m_bNeedsRecalcLayout = TRUE;
   Invalidate();
}

void CGLTreeControl::DeleteAllItems()
{
   while (m_childcontrolarray.GetCount())
      delete m_childcontrolarray[0];
}

BOOL CGLTreeControl::Select( CGLTreeItem *pItem, UINT nCode/*=GLTC_CARET*/ )
{
   if (!(nCode & (GLTC_APPENDTOSELECTION | GLTC_APPENDFROMCARET)))
   {
      if (pItem && pItem->IsSelected() && m_pCaretItem==pItem && m_selecteditemlist.GetCount()==1)
         return TRUE;
      DeselectAll();
   }

   if (nCode & GLTC_APPENDFROMCARET)
   {
      ;
   }

   if (pItem)
      pItem->SetSelected( TRUE );

//TODO   m_pParentControl->OnControlNotify( NOTIFYCTRL_LC_CARETCHANGED, MAKEWPARAM(m_nID, 0), (LPARAM)this );

   return TRUE;
}

BOOL CGLTreeControl::Deselect( CGLTreeItem *pItem )
{
   pItem->SetSelected( FALSE );

   return TRUE;
}

void CGLTreeControl::DeselectAll()
{
   for (POSITION pos=m_selecteditemlist.GetHeadPosition(); pos; )
   {
      CGLTreeItem *pItem = m_selecteditemlist.GetNext( pos );
      pItem->SetSelected( FALSE );
   }
   m_selecteditemlist.RemoveAll();
}

void CGLTreeControl::EnsureVisible( CGLTreeItem *pItem )
{
   if (!pItem)
      return;

   for (CGLControl *pParent=pItem->m_pParentControl; pParent; pParent=pParent->m_pParentControl)
   {
      if (pParent->IsTreeItem())
         ((CGLTreeItem *)pParent)->Expand();
      else
         break;
   }

   CRect rcControl = pItem->GetViewportRect();
   rcControl.BottomRight() = rcControl.TopLeft() + pItem->GetControlSize();
   rcControl.bottom = rcControl.top + pItem->GetItemHeight();
   CRect rcTree = GetViewportRect();
   rcControl.OffsetRect( -rcTree.TopLeft());

   if (rcControl.top < 0)
      m_glVScroll.SetPos( m_glVScroll.GetPos() + rcControl.top - 2 );
   else if (rcControl.bottom > rcTree.Height())
      m_glVScroll.SetPos( m_glVScroll.GetPos() + (rcControl.bottom - rcTree.Height()) + 2 );
}

CGLTreeItem *CGLTreeControl::AllocItem( CGLTreeControl *pTreeControl, LPARAM lParam )
{
   return SNEW CGLTreeItem( pTreeControl );
}

void CGLTreeControl::PostInit()
{
   CGLScrollableControl::PostInit();

   m_glVScroll.SetLine( CGLTreeItem::HEIGHT );

   if (m_glImageListExpandIcons.IsEmpty()) {
      MakeRenderContextCurrent();
      m_glImageListExpandIcons.Create( IDB_TREEEXPAND, 9 );
   }
}

void CGLTreeControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification )
{
   CGLScrollableControl::Move( ptControl, szControl, justification );

   CPoint ptChild = CPoint( 3, 3 - m_glVScroll.GetPos() );
   for (int i=0; i<m_childcontrolarray.GetCount(); i++) {
      CGLControl *pItem = m_childcontrolarray[i];
      if (!pItem->IsVisible())
         continue;
      CSize szChildControl( m_szControl.cx-ptChild.x, pItem->m_szControl.cy==-1?0:pItem->m_szControl.cy );
      if (pItem->m_szControl == szChildControl)
         pItem->m_ptControl = ptChild;
      else
         pItem->Move( ptChild, szChildControl );
      ptChild.y += pItem->m_szControl.cy;
   }

   m_glVScroll.SetMax( ptChild.y + m_glVScroll.GetPos() + 6);
   m_glVScroll.SetPage( m_szControl.cy );

   m_bNeedsRecalcLayout = FALSE;
}

void CGLTreeControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   if (m_bNeedsRecalcLayout)
   {
      Move( m_ptControl, GetControlSize() );
      if (m_pCaretItem)
         EnsureVisible( m_pCaretItem );
   }

   GLDrawPrepper prep(this);

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   RGBAFloat c[4] = { RGBAFloat(1,1,1,1),
                      RGBAFloat(1,1,1,1),
                      RGBAFloat(0.5f,0.5f,0.5f,1.0f),
                      RGBAFloat(0.5f,0.5f,0.5f,1.0f)};
   DrawGradient( c );

   CGLScrollableControl::Draw( nDrawFlags );

   DrawFocusRing();
//   DrawShadow();
}

BOOL CGLTreeControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   if (MakeActive())
      GetApp().DrawInvalidControls();

   return TRUE;// don't want it to go thru parents CGLControl::OnLButtonDown( nFlags, point );
}

BOOL CGLTreeControl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
   if (m_pCaretItem && m_pCaretItem->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo ))
      return TRUE;

   return CGLScrollableControl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

////////////////////////////////////////
// CGLTreeItem

CGLTreeItem::CGLTreeItem( CGLTreeControl *pTreeControl )
{
   m_pTreeControl = pTreeControl;
   m_nTIFlags = 0L;
}

CGLTreeItem::~CGLTreeItem()
{
   if (m_bSelected)
      GetTreeControl()->Deselect( this );
}

void CGLTreeItem::SetSelected( BOOL bSelected )
{
   if (m_bSelected == bSelected)
      return;

   m_bSelected = bSelected;

   if (bSelected)
   {
      GetTreeControl()->m_pCaretItem = this;
      GetTreeControl()->m_selecteditemlist.AddTail( this );
      GetTreeControl()->EnsureVisible( this );
   }
   else
   {
      if (GetTreeControl()->m_pCaretItem == this)
         GetTreeControl()->m_pCaretItem = NULL;
      POSITION pos = GetTreeControl()->m_selecteditemlist.Find( this );
      if (pos)
         GetTreeControl()->m_selecteditemlist.RemoveAt( pos );
   }

   Invalidate();
}

BOOL CGLTreeItem::SetVisible( BOOL bVisible )
{
   if (m_bHidden)
      bVisible = FALSE;

   if (CGLControl::SetVisible( bVisible )) // Changed
   {
      for (CGLControl *pControl=GetParent(); pControl; pControl=pControl->GetParent())
      {
         if (pControl->IsTreeItem())
            pControl->m_szControl.cy = -1; // this will cause Move to update it's new size
         else
            break;
      }
      GetTreeControl()->m_bNeedsRecalcLayout = TRUE;
      GetTreeControl()->Invalidate();
      return TRUE;
   }

   if (GetParent() && GetParent()->IsTreeItem())
      GetParent()->Invalidate();

   return FALSE;
}

void CGLTreeItem::Expand( BOOL bExpand/*=TRUE*/ )
{
   if (m_bExpanded == bExpand)
      return;

   m_bExpanded = bExpand;
   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
   {
      CGLControl *pItem = m_childcontrolarray[i];
      if (pItem->IsTreeItem())
         pItem->SetVisible( m_bExpanded );
   }

   for (CGLControl *pControl=this; pControl; pControl=pControl->m_pParentControl)
   {
      if (pControl->IsTreeItem())
         pControl->m_szControl.cy = -1; // this will cause Move to update it's new size
      else
         break;
   }
   GetTreeControl()->Move( GetTreeControl()->m_ptControl, GetTreeControl()->GetControlSize() );
}

void CGLTreeItem::Move( const CPoint &ptControl, const CSize &szControl, Justify justification )
{
   CPoint ptChild( INDENT, GetItemHeight() );
   if (m_bExpanded)
   {
      BOOL bHasVisibleTreeItem = FALSE;
      for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      {
         CGLControl *pItem = m_childcontrolarray[i];
         if (pItem->IsTreeItem())
         {
            if (!pItem->IsVisible())
               continue;
            bHasVisibleTreeItem = TRUE;
            CSize szChildControl( szControl.cx-ptChild.x, pItem->m_szControl.cy==-1?0:pItem->m_szControl.cy );
            if (pItem->m_szControl == szChildControl)
               pItem->m_ptControl = ptChild;
            else
               pItem->Move( ptChild, szChildControl );
            ptChild.y += pItem->m_szControl.cy;
         }
      }
      if (bHasVisibleTreeItem)
         ptChild.y += GetExpandedMargin();
   }

   CGLControl::Move( ptControl, CSize( szControl.cx, ptChild.y ), justification );
}

void CGLTreeItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   if (IsExpanded() && m_childcontrolarray.GetCount())
   {
      float y = 0.0f;
      glColor4f( 0.0f, 0.0f, 0.0f, 0.3f );
      glLineWidth( 1.4f );
      glBegin( GL_LINES );

//CRect rcViewport = GetViewportRect();
//for (; y<rcViewport.Height(); y+=18)
//{
//   glVertex2f( 6.5f, y );
//   glVertex2f( 6.5f + INDENT + 3.0f, y );
//}
         for (int i=0; i<m_childcontrolarray.GetCount(); i++)
         {
            y = m_childcontrolarray[i]->m_ptControl.y;
            y += 8.5f;
            glVertex2f( 6.5f, y );
            glVertex2f( 6.5f + INDENT + 3.0f, y );
         }

         glVertex2f( 6.5f, 13 );
         glVertex2f( 6.5f, y );
      glEnd();
   }

   CGLControl::Draw( nDrawFlags );
}

static CPoint f_ptStart;

BOOL CGLTreeItem::OnLButtonDown( UINT nFlags, CPoint point )
{
   f_ptStart = point;

   GetTreeControl()->MakeActive();

   if (IsExpandable() && point.x<(4 + (int)CGLTreeControl::m_glImageListExpandIcons.GetWidthEach())) {
      Expand( !IsExpanded() );
   }
   else {
      CGLTreeControl *pTree = GetTreeControl();
      UINT nCode = CGLTreeControl::GLTC_CARET;
      if (IsDownShift())
         nCode |= CGLTreeControl::GLTC_APPENDFROMCARET;
      if (IsDownControl())
         nCode |= CGLTreeControl::GLTC_APPENDTOSELECTION;
      pTree->Select( this, nCode );
   }
   GetApp().DrawInvalidControls();

   return TRUE;
}

BOOL CGLTreeItem::OnRButtonDown( UINT nFlags, CPoint point )
{
   f_ptStart = point;

   OnLButtonDown( nFlags, point );
   return CGLControl::OnRButtonDown( nFlags, point );
}

BOOL CGLTreeItem::OnMButtonDown( UINT nFlags, CPoint point )
{
   f_ptStart = point;

   ControlToClient( point );
   m_pParentControl->ClientToControl( point );
   return m_pParentControl->OnMButtonDown( nFlags, point );
}

BOOL CGLTreeItem::OnMouseMove( UINT nFlags, CPoint point )
{
   UINT nButtons = nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON);

   if (nButtons == MK_LBUTTON || m_bDragging)
   {
      if (!m_bDragging)
      {
         CPoint delta = point - f_ptStart;
         if (abs(delta.x) > 4 || abs( delta.y) > 4)
         {
            m_bDragging = TRUE;
            BeginDrag( point );
         }
      }

      if (m_bDragging)
      {
         DoDrag( point );
         return TRUE;
      }
   }
   else if (nButtons == MK_MBUTTON)
   {
      ControlToClient( point );
      m_pParentControl->ClientToControl( point );
      return m_pParentControl->OnMouseMove( nFlags, point );
   }

   return CGLControl::OnMouseMove( nFlags, point );
}

BOOL CGLTreeItem::OnLButtonUp( UINT nFlags, CPoint point )
{
   if (m_bDragging)
   {
      EndDrag( point );
      m_bDragging = FALSE;
   }
   return CGLControl::OnLButtonUp( nFlags, point );
}

BOOL CGLTreeItem::OnMButtonUp( UINT nFlags, CPoint point )
{
   if (m_bDragging)
   {
      EndDrag( point );
      m_bDragging = FALSE;
   }
   ControlToClient( point );
   m_pParentControl->ClientToControl( point );
   return m_pParentControl->OnMButtonUp( nFlags, point );
}

static CGLControl *f_pPrevHitControl;

void CGLTreeItem::BeginDrag( CPoint point )
{
   f_pPrevHitControl = this;
   ControlToClient( point );
   OnDragEnter( point, this );
}

void CGLTreeItem::DoDrag( CPoint point )
{
   ControlToClient( point );
   CGLControl *pHitControl = m_pGLRenderWnd->HitTestControls( point );
   if (!pHitControl->IsEnabled())
      pHitControl = NULL;

   if (pHitControl != f_pPrevHitControl)
   {
      if (f_pPrevHitControl)
         f_pPrevHitControl->OnDragLeave( point, this );
      if (pHitControl)
         pHitControl->OnDragEnter( point, this );
      f_pPrevHitControl = pHitControl;
   }

   if (pHitControl)
      pHitControl->OnDragMove( point, this );
}

void CGLTreeItem::EndDrag( CPoint point )
{
   if (f_pPrevHitControl)
   {
      ControlToClient( point );
      f_pPrevHitControl->OnDragDrop( point, this );
      f_pPrevHitControl = NULL;
   }
}
