#include "stdafx.h"
#include "Snipe.h"
#include "GLTabControl.h"
#include "Resource.h"
#include ".\gltabcontrol.h"

CGLTabControl::Tab::Tab( const String &name, LPARAM lParam )
{
   m_name = name;
   m_lParam = lParam;
   m_rcTab.SetRectEmpty();
}

BOOL CGLTabControl::Tab::HitTest( const CPoint &pt )
{
   return m_rcTab.PtInRect( pt );
}

//////////////////

IMPLEMENT_DYNAMIC(CGLTabControl, CGLControl)

GLImageList CGLTabControl::m_glDefaultImageListBorderTop;
GLImageList CGLTabControl::m_glDefaultImageListBorderBottom;
GLImageList CGLTabControl::m_glDefaultImageListTabsTop;
GLImageList CGLTabControl::m_glDefaultImageListTabsBottom;

CGLTabControl::CGLTabControl( BOOL bTop, BOOL bAutoControlSwap/*=FALSE*/, BOOL bRemovableTabs/*=TRUE*/ )
{
   m_nSelectedTab = -1;
   m_bRemoving = FALSE;
   m_bTop = bTop;
   m_bCenter = FALSE;
   m_bAutoControlSwap = bAutoControlSwap;
   m_bRemovableTabs = bRemovableTabs;
   if (m_bTop)
   {
      m_pglImageListBorder = &m_glDefaultImageListBorderTop;
      m_pglImageListTabs = &m_glDefaultImageListTabsTop;

      m_nTabIndent = 19;
      m_nTabSpacing = -14;
      m_nTabTextMargin = 8;
   }
   else
   {
      m_pglImageListBorder = &m_glDefaultImageListBorderBottom;
      m_pglImageListTabs = &m_glDefaultImageListTabsBottom;

      m_nTabIndent = TAB_INDENT;
      m_nTabSpacing = 0;
      m_nTabTextMargin = 7;
   }
}

CGLTabControl::~CGLTabControl()
{
}

CRect CGLTabControl::GetInsideRect()
{
   CRect rcBorder = m_pglImageListBorder->GetDiceRect();
   CRect rcInside( rcBorder.left, rcBorder.top, m_szControl.cx - (m_pglImageListBorder->GetWidthEach()-rcBorder.right), m_szControl.cy - (m_pglImageListBorder->GetHeight()-rcBorder.bottom) );

   return rcInside;
}

void CGLTabControl::PostInit()
{
   if (m_glDefaultImageListBorderTop.IsEmpty())
   {
      m_glDefaultImageListBorderTop.Create( IDB_TABSTOPBORDER, CRect( 2, 20, 3, 21 ) );
      m_glDefaultImageListTabsTop.Create( IDB_TABSTOP, CRect( 22, 0, 24, 19 ), 31 );
      m_glDefaultImageListBorderBottom.Create( IDB_TABBORDER, CRect( 6, 6, 16, 9 ) );
      m_glDefaultImageListTabsBottom.Create( IDB_TABS, CRect( 13, 0, 16, 16 ), 28 );
   }

   if (m_bTop && m_bRemovableTabs)
   {
      m_glDocListButton.Init( this, NULL, CPoint(20+16,4), CSize(16,m_szControl.cy-8), ID_SPLITTER_WINDOWLIST, CGLControl::JUST_RIGHT );
      m_glDocListButton.m_bHasBackground = FALSE;
      m_glCloseButton.Init( this, NULL, CPoint(20,4), CSize(16,m_szControl.cy-8), ID_SPLITTER_CLOSE, CGLControl::JUST_RIGHT );
      m_glCloseButton.m_bHasBackground = FALSE;
   }
}

void CGLTabControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   if (!m_bTop) // move tab rects for bottom justification
   {
      for (int i=0; i<m_tabarray.GetCount(); i++)
      {
         Tab *pTab = &m_tabarray[i];
         pTab->m_rcTab.top = m_szControl.cy - m_pglImageListTabs->GetHeight();
         pTab->m_rcTab.bottom = m_szControl.cy;
      }
   }

   if (m_bCenter)
   {
      int nLeft = m_tabarray[0].m_rcTab.left;
      int nRight = m_tabarray[GetTabCount()-1].m_rcTab.right;
      int nWidth = nRight - nLeft;
      nLeft = (m_szControl.cx - nWidth) / 2;
      int nDelta = nLeft - m_tabarray[0].m_rcTab.left;
      for (int i=0; i<GetTabCount(); i++)
         m_tabarray[i].m_rcTab.OffsetRect( nDelta, 0 );
   }

   if (m_bAutoControlSwap)
   {
      CRect rcChild = GetInsideRect();
      for (int i=0; i<GetTabCount(); i++)
         m_tabarray[i].GetControlHead()->Move( rcChild.TopLeft(), rcChild.Size() );
   }
}

int CGLTabControl::AddTab( const String &strName, LPARAM lParam )
{
   Tab tab( strName, lParam );
   return AddTab( tab );
}

int CGLTabControl::AddTab( Tab &tab )
{
   int nPrevTabRight;
   if (m_tabarray.IsEmpty())
      nPrevTabRight = m_pglImageListBorder->GetDiceRect().left + m_nTabIndent;
   else
      nPrevTabRight = m_tabarray[m_tabarray.GetCount()-1].m_rcTab.right;

   tab.m_rcTab.left = nPrevTabRight + m_nTabSpacing;
   tab.m_rcTab.right = max(tab.m_rcTab.left, nPrevTabRight) + m_nTabTextMargin + GetTextExtent( tab.m_name ).cx + m_nTabTextMargin;

   if (m_bTop)
   {
      tab.m_rcTab.top = 0;
      tab.m_rcTab.bottom = m_pglImageListTabs->GetHeight();
   }
   else
   {
      tab.m_rcTab.top = m_szControl.cy - m_pglImageListTabs->GetHeight();
      tab.m_rcTab.bottom = m_szControl.cy;
   }

   int nIndex = m_tabarray.Add( tab );

   if (m_bCenter)
   {
      int nLeft = m_tabarray[0].m_rcTab.left;
      int nRight = m_tabarray[GetTabCount()-1].m_rcTab.right;
      int nWidth = nRight - nLeft;
      nLeft = (m_szControl.cx - nWidth) / 2;
      int nDelta = nLeft - m_tabarray[0].m_rcTab.left;
      for (int i=0; i<GetTabCount(); i++)
         m_tabarray[i].m_rcTab.OffsetRect( nDelta, 0 );
   }

   if (m_bAutoControlSwap)
   {
      CGLControl *pControl = tab.GetControlHead();
	   ASSERT( pControl != NULL );
      ASSERT_KINDOF( CGLControl, pControl );
      CRect rcInside = GetInsideRect();
      pControl->Init( this, NULL, rcInside.TopLeft(), rcInside.Size() );
      pControl->SetVisible( FALSE );
   }

   if (m_tabarray.GetCount() == 1)
      SelectTab( nIndex );

   return nIndex;
}

void CGLTabControl::RemoveTab( int nIndex )
{
   if (m_bRemoving || nIndex<0 || nIndex>=m_tabarray.GetCount())
      return;

   m_bRemoving = TRUE;
   m_pParentControl->OnControlNotify( NOTIFYCTRL_TB_REMOVED, MAKEWPARAM(m_nID, nIndex), (LPARAM)this );

   int nTabLeft = m_tabarray[nIndex].m_rcTab.left;

   m_tabarray.RemoveAt( nIndex );

   if (m_nSelectedTab == nIndex)
   {
      if (nIndex)
         SelectTab( nIndex - 1 );
      else if (m_tabarray.GetCount() > 0)
         SelectTab( 0 );
      else
         SelectTab( -1 );
   }
   else if (m_nSelectedTab > nIndex)
      m_nSelectedTab--;

   // if there are tabs to the right of the deleted tab, move them over
   if (nIndex < m_tabarray.GetCount()) {
      int nOffset = m_tabarray[nIndex].m_rcTab.left - nTabLeft;
      for (; nIndex<m_tabarray.GetCount(); nIndex++)
         m_tabarray[nIndex].m_rcTab.OffsetRect( -nOffset, 0 );
   }

   Invalidate();
   m_bRemoving = FALSE;
}

int CGLTabControl::GetTabCount() const
{
   return m_tabarray.GetCount();
}

LPARAM CGLTabControl::GetItemData( int nIndex ) const
{
   if (nIndex>=0 && nIndex<m_tabarray.GetCount())
      return m_tabarray[nIndex].m_lParam;
   else
      return NULL;
}

void CGLTabControl::SetItemData( int nIndex, LPARAM lParam )
{
   if (nIndex>=0 && nIndex<m_tabarray.GetCount()) {
      m_tabarray[nIndex].m_lParam = lParam;
      Invalidate();
   }
}

int CGLTabControl::FindTab( Tab *pTab )
{
   for (int i=0; i<m_tabarray.GetCount(); i++)
      if (&m_tabarray[i] == pTab)
         return i;
   return -1;
}

int CGLTabControl::FindTab( LPARAM lParam )
{
   for (int i=0; i<m_tabarray.GetCount(); i++)
      if (m_tabarray[i].m_lParam == lParam)
         return i;
   return -1;
}

void CGLTabControl::SelectTab( int nIndex )
{
   if (nIndex < 0 || nIndex >= m_tabarray.GetCount())
      nIndex = -1;

   if (nIndex == m_nSelectedTab)
      return;

   int nOldIndex = m_nSelectedTab;
   m_nSelectedTab = nIndex;

   if (m_bAutoControlSwap)
   {
      if (nOldIndex >= 0 && nOldIndex < GetTabCount())
         m_tabarray[ nOldIndex ].GetControlHead()->SetVisible( FALSE );
      if (m_nSelectedTab >= 0)
         m_tabarray[ m_nSelectedTab ].GetControlHead()->SetVisible( TRUE );
   }

   m_pParentControl->OnControlNotify( NOTIFYCTRL_TB_CHANGED, MAKEWPARAM(m_nID, nOldIndex), (LPARAM)this );

   Invalidate();
}

CGLTabControl::Tab *CGLTabControl::HitTestTabs( const CPoint &pt )
{
//   if (m_nSelectedTab!=-1 && m_tabarray[m_nSelectedTab].HitTest( pt ))
//      return &m_tabarray[m_nSelectedTab];

   for (int i=0; i<m_tabarray.GetCount(); i++) {
      if (m_tabarray[i].HitTest( pt ))
         return &m_tabarray[i];
   }
   return NULL;
}

void CGLTabControl::DrawTab( int nIndex )
{
   Tab *pTab = &m_tabarray[nIndex];
   int nTextTopMargin = 4;

   int nBtnIndex;
   if (m_pglImageListTabs == &m_glDefaultImageListTabsTop)
   {
      nBtnIndex = m_nSelectedTab==nIndex ? 0 : 1;
      nTextTopMargin++;
   }
   else
   {
      if (m_tabarray.GetCount()==1) // standalone
         nBtnIndex = 0;
      else if (nIndex==0) // start
         nBtnIndex = 1;
      else if (nIndex+1==m_tabarray.GetCount()) // end
         nBtnIndex = 3;
      else // middle
         nBtnIndex = 2;

      if (m_nSelectedTab==nIndex)
         nBtnIndex += 4;
   }

   m_pglImageListTabs->DrawDiced( pTab->m_rcTab, nBtnIndex );

   glColor3f( 0.0f, 0.0f, 0.0f );
   DrawText( pTab->m_rcTab.left - m_nTabSpacing + m_nTabTextMargin - 1, pTab->m_rcTab.top + pTab->m_rcTab.Height()/2 + nTextTopMargin, pTab->m_name.Get() );
}

void CGLTabControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   m_pglImageListBorder->DrawDiced( CRect( CPoint(0,0), GetControlSize()) );

   // All unselected tabs
   for (int i=m_tabarray.GetCount()-1; i>=0; i--) {
      if (i != m_nSelectedTab)
         DrawTab( i );
   }

   // Selected tab
   if (m_nSelectedTab != -1)
      DrawTab( m_nSelectedTab );

   CGLControl::Draw( nDrawFlags );
}

BOOL CGLTabControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   Tab *pHit = HitTestTabs( point );
   if (pHit) {
      SelectTab( FindTab( pHit ) );
      GetApp().DrawInvalidControls();
   }
   return TRUE;
}

UINT CGLTabControl::GetTabMenuID()
{
   return IDR_TABCONTROL;
}

UINT CGLTabControl::GetContextMenuID( const CPoint &point, UINT &nDefaultItemID )
{
   Tab *pHit = HitTestTabs( point );
   if (pHit) {
      SelectTab( FindTab( pHit ) );
      GetApp().DrawInvalidControls();
      return GetTabMenuID();
   }
   else
      return -1;
}

BOOL CGLTabControl::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYCTRL_BN_CLICKED:
      {
         UINT nID = (UINT)wParam;
         switch (nID) {
            case ID_SPLITTER_WINDOWLIST:
            {
               CPoint point;
               GetCursorPos( &point );
               CMenu menu, popup;
               menu.CreateMenu();
               popup.CreatePopupMenu();
               menu.AppendMenu( MF_POPUP, (UINT)popup.GetSafeHmenu() );
               for (int i=0; i<m_tabarray.GetCount(); i++)
                  popup.AppendMenu( 0, ID_TAB_ITEM_FIRST+i, m_tabarray[i].m_name.Get() );
               UINT nID = popup.TrackPopupMenu( TPM_LEFTALIGN|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, (CWnd *)m_pGLRenderWnd );
               if (nID)
                  OnSelectTab( nID );
               break;
            }
            case ID_SPLITTER_CLOSE:
               RemoveTab( m_nSelectedTab );
               break;
         }
         return TRUE;
      }
   }
   return CGLControl::OnControlNotify( msg, wParam, lParam );
}

BEGIN_MESSAGE_MAP(CGLTabControl, CGLControl)
   ON_COMMAND_RANGE(ID_TAB_ITEM_FIRST, ID_TAB_ITEM_LAST, OnSelectTab)
   ON_COMMAND(ID_TABCONTROL_CLOSE, OnTabControlClose)
   ON_UPDATE_COMMAND_UI(ID_TABCONTROL_CLOSEALLBUTTHIS, OnUpdateTabControlCloseAllButThis)
   ON_COMMAND(ID_TABCONTROL_CLOSEALLBUTTHIS, OnTabControlCloseAllButThis)
END_MESSAGE_MAP()

void CGLTabControl::OnSelectTab( UINT nID )
{
   SelectTab( nID - ID_TAB_ITEM_FIRST );
   GetApp().DrawInvalidControls();
}

void CGLTabControl::OnTabControlClose()
{
   RemoveTab( m_nSelectedTab );
   GetApp().DrawInvalidControls();
}

void CGLTabControl::OnUpdateTabControlCloseAllButThis(CCmdUI *pCmdUI)
{
   pCmdUI->Enable( m_nSelectedTab!=-1 && m_tabarray.GetCount()>1);
}

void CGLTabControl::OnTabControlCloseAllButThis()
{
   for (int i=m_tabarray.GetCount()-1; i>=0; i--)
   {
      if (i != m_nSelectedTab)
         RemoveTab( i );
   }
   GetApp().DrawInvalidControls();
}

