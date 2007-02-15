#include "stdafx.h"
#include "Snipe.h"
#include "GLComboControl.h"
#include "GLRenderWnd.h"

CGLListControl::CGLListControl()
{
   m_pCaretItem = NULL;
   m_pDropTarget = NULL;
   m_pComboControl = NULL;
   m_bNeedsRecalcLayout = FALSE;
}

CGLListControl::~CGLListControl()
{
   if (m_pComboControl)
   {
      if (GetParent())
         GetParent()->Invalidate();
      else
         m_pGLRenderWnd->Invalidate( FALSE );

      m_pComboControl->SetListControl( NULL );
   }
}

BOOL CGLListControl::SetIconBitmap( UINT nBitmapID, UINT nIconWidth/*=16*/ )
{
   return m_glImageListIcons.Create( nBitmapID, nIconWidth );
}

CGLListItem *CGLListControl::InsertItem( LPARAM lParam, CGLListItem *pInsertAfter/*=GLLI_LAST*/ )
{
   CGLListItem *pItem = AllocItem( this, lParam );
   pItem->Init( this, NULL, CPoint(0,0), CSize(m_szControl.cx, -1), -1, JUST_LEFT, pInsertAfter );

   if (pInsertAfter == GLLI_LAST)
      pItem->SetIndex( m_childcontrolarray.GetCount()-1 );
   else
   {
      for (int i=0; i<m_childcontrolarray.GetCount(); i++)
      {
         CGLListItem *pItem = (CGLListItem *)m_childcontrolarray[i];
         pItem->SetIndex( i );
      }
   }

   m_bNeedsRecalcLayout = TRUE;
   Invalidate();

   return pItem;
}

void CGLListControl::RemoveItem( CGLListItem *pItem )
{
   for (CGLControl *pControl=pItem->m_pParentControl; pControl; pControl=pControl->m_pParentControl)
      if (pControl->IsListItem())
         pControl->m_szControl.cy = -1; // this will cause Move to update it's new size

   if (pItem)
      delete pItem;

   m_bNeedsRecalcLayout = TRUE;
}

void CGLListControl::DeleteAllItems()
{
   while (m_childcontrolarray.GetCount())
      delete m_childcontrolarray[0];

   m_bNeedsRecalcLayout = TRUE;
   m_glVScroll.SetPos(0);
   Invalidate();
}

BOOL CGLListControl::Select( CGLListItem *pItem, UINT nCode/*=GLTC_CARET*/ )
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

   if (nCode & GLTC_CARET)
   {
      m_pCaretItem = pItem;

      if (pItem)
      {
         m_selecteditemlist.AddTail( pItem );
         pItem->SetSelected( TRUE );
         EnsureVisible( pItem );
      }

      CGLControl *pNotifyControl = m_pComboControl;
      if (!pNotifyControl)
         pNotifyControl = m_pParentControl;
      if (pNotifyControl)
         pNotifyControl->OnControlNotify( NOTIFYCTRL_LC_CARETCHANGED, MAKEWPARAM(m_nID, 0), (LPARAM)this );
   }

   if (pItem && (nCode & GLTC_DROPHILITE))
      pItem->SetDropHilited( TRUE );

   return TRUE;
}

BOOL CGLListControl::Deselect( CGLListItem *pItem )
{
   if (m_pCaretItem == pItem)
      m_pCaretItem = NULL;

   POSITION pos = m_selecteditemlist.Find( pItem );
   if (!pos)
      return FALSE;

   m_selecteditemlist.RemoveAt( pos );

   pItem->SetSelected( FALSE );
   return TRUE;
}

void CGLListControl::DeselectAll()
{
   m_pCaretItem = NULL;
   for (POSITION pos=m_selecteditemlist.GetHeadPosition(); pos; ) {
      CGLListItem *pItem = m_selecteditemlist.GetNext( pos );
      pItem->SetSelected( FALSE );
   }
   m_selecteditemlist.RemoveAll();
}

void CGLListControl::EnsureVisible( CGLListItem *pItem )
{
   if (!pItem)
      return;

   if (pItem->m_ptControl.y < 0)
      m_glVScroll.SetPos( m_glVScroll.GetPos() + pItem->m_ptControl.y - 2 );
   else if (pItem->m_ptControl.y + pItem->GetControlSize().cy > m_szControl.cy)
      m_glVScroll.SetPos( m_glVScroll.GetPos() + ((pItem->m_ptControl.y + pItem->GetControlSize().cy) - m_szControl.cy) + 2 );
}

int CGLListControl::GetCurSel() const
{
   for (int i=0; i<m_childcontrolarray.GetCount(); i++)
   {
      CGLListItem *pItem = (CGLListItem *)m_childcontrolarray[i];
      if (pItem == m_pCaretItem)
         return i;
   }
   return -1;
}

CGLListItem *CGLListControl::AllocItem( CGLListControl *pListControl, LPARAM lParam )
{
   CGLBasicListItem::Info *pInfo = (CGLBasicListItem::Info *)lParam;
//   ASSERT_KINDOF( CGLBasicListItem::Info, pInfo );

   return SNEW CGLBasicListItem( pInfo );
}

void CGLListControl::PostInit()
{
   CGLScrollableControl::PostInit();

   m_glVScroll.SetLine( CGLListItem::HEIGHT );
}

void CGLListControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification )
{
   CGLScrollableControl::Move( ptControl, szControl, justification );

   CPoint ptChild = CPoint( 3, 3 - m_glVScroll.GetPos() );
   for (int i=0; i<m_childcontrolarray.GetCount(); i++) {
      CGLListItem *pItem = (CGLListItem *)m_childcontrolarray[i];
      CSize szChildControl( m_szControl.cx-ptChild.x-3, pItem->GetItemHeight() );
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

void CGLListControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   if (m_bNeedsRecalcLayout)
   {
      Move( m_ptControl, GetControlSize() );
      if (m_pCaretItem)
         EnsureVisible( m_pCaretItem );
   }

   GLDrawPrepper prep(this);

   if (m_pParentControl && (nDrawFlags & DRAW_PARENT))
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   DrawBorder();

   glColor3f( 1,1,1 );
   glBegin( GL_QUADS );
      glVertex2f( 0, 0 );
      glVertex2f( m_szControl.cx, 0 );
      glVertex2f( m_szControl.cx, m_szControl.cy );
      glVertex2f( 0, m_szControl.cy );
   glEnd();

   CGLScrollableControl::Draw( nDrawFlags );

   DrawFocusRing();
//   DrawShadow();
}

BOOL CGLListControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   if (MakeActive( TRUE ))
      GetApp().DrawInvalidControls();

   if (m_pComboControl)
   {
      ControlToClient( point );
      CGLControl *pControlClicked = m_pGLRenderWnd->HitTestControls( point );
      if (pControlClicked != this)
      {
         if (!pControlClicked || (pControlClicked!=m_pComboControl && pControlClicked!=&m_glVScroll && !pControlClicked->IsDescendentOf( this )))
         {
            delete this;

            GetApp().DrawInvalidControls();
         }

         if (pControlClicked)
         {
            pControlClicked->SetCapture();
            pControlClicked->ClientToControl( point );
            return pControlClicked->OnLButtonDown( nFlags, point );
         }

         return TRUE;
      }
   }

   return CGLScrollableControl::OnLButtonDown( nFlags, point );
}

BOOL CGLListControl::OnMButtonUp( UINT nFlags, CPoint point )
{
   if (m_pComboControl)
      SetCapture();

   return CGLScrollableControl::OnMButtonUp( nFlags, point );
}

BOOL CGLListControl::OnRButtonUp( UINT nFlags, CPoint point )
{
   if (m_pComboControl)
      SetCapture();

   return CGLScrollableControl::OnRButtonUp( nFlags, point );
}

BOOL CGLListControl::OnMouseMove( UINT nFlags, CPoint point )
{
   if (m_pComboControl)
   {
      CPoint ptClient = point;
      ControlToClient( ptClient );
      CGLControl *pHitControl = m_pGLRenderWnd->HitTestControls( ptClient );
      if (pHitControl && pHitControl->IsListItem())
         Select( (CGLListItem *)pHitControl, GLTC_DROPHILITE );
   }

   return CGLScrollableControl::OnMouseMove( nFlags, point );
}

BOOL CGLListControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch (nChar)
   {
      case VK_UP:
         for (int i=0; i<m_childcontrolarray.GetCount(); i++)
         {
            if (m_childcontrolarray[i] == m_pCaretItem)
            {
               if (i > 0)
               {
                  Select( (CGLListItem *)m_childcontrolarray[i-1] );
                  GetApp().DrawInvalidControls();
               }
               break;
            }
         }
         return TRUE;
      case VK_DOWN:
         for (int i=0; i<m_childcontrolarray.GetCount(); i++)
         {
            if (m_childcontrolarray[i] == m_pCaretItem)
            {
               if (i < m_childcontrolarray.GetCount()-1)
               {
                  Select( (CGLListItem *)m_childcontrolarray[i+1] );
                  GetApp().DrawInvalidControls();
               }
               break;
            }
         }
         return TRUE;
      case VK_ESCAPE:
         if (m_pComboControl)
         {
            delete this;
            GetApp().DrawInvalidControls();
            return TRUE;
         }
         break;
   }

   return FALSE;
}

BOOL CGLListControl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
   if (m_pCaretItem && m_pCaretItem->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo ))
      return TRUE;

   return CGLScrollableControl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CGLListControl::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_SB_ENDSCROLL:
      {
         if (m_pComboControl)
         {
            SetCapture();
            return TRUE;
         }
      }
   }

   return CGLScrollableControl::OnControlNotify( msg, wParam, lParam );
}

////////////////////////////////////////
// CGLListItem

CGLListItem::CGLListItem()
{
   m_bSelected = FALSE;
   m_bDropHilited = FALSE;
}

CGLListItem::~CGLListItem()
{
   if (m_bSelected)
      GetListControl()->Deselect( this );
}

void CGLListItem::SetSelected( BOOL bSelected )
{
   if (m_bSelected == bSelected)
      return;

   m_bSelected = bSelected;

   Invalidate();
}

void CGLListItem::SetDropHilited( BOOL bDropHilited )
{
   CGLListItem *pOldDropTarget = GetListControl()->m_pDropTarget;
   CGLListItem *pNewDropTarget = NULL;
   if (bDropHilited)
      pNewDropTarget = this;

   if (pNewDropTarget == pOldDropTarget)
      return;

   GetListControl()->m_pDropTarget = pNewDropTarget;

   if (pOldDropTarget)
      pOldDropTarget->m_bDropHilited = FALSE;
   if (pNewDropTarget)
      pNewDropTarget->m_bDropHilited = TRUE;

   Invalidate();
}

BOOL CGLListItem::OnLButtonDown( UINT nFlags, CPoint point )
{
   GetListControl()->MakeActive( TRUE );

   UINT nCode = CGLListControl::GLTC_CARET;
   if (IsDownShift())
      nCode |= CGLListControl::GLTC_APPENDFROMCARET;
   if (IsDownControl())
      nCode |= CGLListControl::GLTC_APPENDTOSELECTION;
   GetListControl()->Select( this, nCode );

   GetApp().DrawInvalidControls();

   return TRUE;
}

BOOL CGLListItem::OnLButtonUp( UINT nFlags, CPoint point )
{
   if (GetListControl()->m_pComboControl)
   {
      if (GetListControl()->GetParent())
         GetListControl()->GetParent()->Invalidate();
      else
         GetListControl()->m_pGLRenderWnd->Invalidate( FALSE );
      delete GetListControl();
      GetApp().DrawInvalidControls();
   }

   return TRUE;
}

BOOL CGLListItem::OnRButtonDown( UINT nFlags, CPoint point )
{
   OnLButtonDown( nFlags, point );

   return CGLControl::OnRButtonDown( nFlags, point );
}

BOOL CGLListItem::OnMButtonDown( UINT nFlags, CPoint point )
{
   ControlToClient( point );
   m_pParentControl->ClientToControl( point );
   return m_pParentControl->OnMButtonDown( nFlags, point );
}

BOOL CGLListItem::OnMouseMove( UINT nFlags, CPoint point )
{
   if (nFlags & MK_MBUTTON)
   {
      ControlToClient( point );
      m_pParentControl->ClientToControl( point );
      return m_pParentControl->OnMouseMove( nFlags, point );
   }
   else if (GetListControl()->m_pComboControl)
   {
      GetListControl()->Select( this, CGLListControl::GLTC_CARET );
      return TRUE;
   }
   else
      return CGLControl::OnMouseMove( nFlags, point );
}

BOOL CGLListItem::OnMButtonUp( UINT nFlags, CPoint point )
{
   ControlToClient( point );
   m_pParentControl->ClientToControl( point );
   return m_pParentControl->OnMButtonUp( nFlags, point );
}

//////////////////////////////////////
// GLBasicListItem

CGLBasicListItem::CGLBasicListItem( CGLBasicListItem::Info *pInfo ) :
 m_info( *pInfo )
{
}

void CGLBasicListItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   if (IsSelected() || IsDropHilited())
   {
      RGBAFloat rgba( 1.0f, 1.0f, 0.5f, 0.8f );
      RGBAFloat c[4] = { rgba, rgba, rgba*0.9f, rgba*0.9f };
      DrawGradient( c, CRect( 0, 0, m_szControl.cx, GetItemHeight() ));
   }

   float x = MARGIN;
   if (!GetListControl()->m_glImageListIcons.IsEmpty())
   {
      GetListControl()->m_glImageListIcons.Draw( x, 0 );
      x += GetListControl()->m_glImageListIcons.GetWidthEach() + MARGIN;
   }

   glColor3f( 0.0f, 0.0f, 0.0f );
   DrawText( x, 12, m_info.m_strName );

   CGLControl::Draw( nDrawFlags );
}

