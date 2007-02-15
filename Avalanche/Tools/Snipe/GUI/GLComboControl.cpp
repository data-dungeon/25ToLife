//

#include "stdafx.h"
#include "GLComboControl.h"
#include "GLRenderWnd.h"
#include "Snipe.h"
#include "resource.h"

/////////////////////////////////////////////////////
// CGLComboListControlItem

CGLComboListItem::CGLComboListItem( const CGLComboControl::ComboEntry *pComboEntry )
{
   m_pComboEntry = pComboEntry;
   GetInfo()->m_strName = pComboEntry->m_strText;
}

void CGLComboListItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   CGLBasicListItem::Draw( nDrawFlags );
}

/////////////////////////////////////////////////////
// CGLComboListControl

CGLListItem *CGLComboListControl::AllocItem( CGLListControl *pListControl, LPARAM lParam )
{
   CGLComboControl::ComboEntry *pComboEntry = (CGLComboControl::ComboEntry *)lParam;

   return SNEW CGLComboListItem( pComboEntry );
}

/////////////////////////////////////////////////////
// GLComboControl

GLImageList CGLComboControl::m_glImageListCombo;

IMPLEMENT_DYNAMIC(CGLComboControl, CGLControl)

CGLComboControl::CGLComboControl()
{
   m_nIndex = -1;
   m_pGLListControl = NULL;
   m_bPressed = FALSE;
   m_bWindowsLike = FALSE;
   m_bFlatLook = FALSE;
}

CGLComboControl::~CGLComboControl()
{
}

void CGLComboControl::SetCurSel( int nIndex, BOOL bNotify/*=TRUE*/ )
{
   if (m_nIndex==nIndex || nIndex>=m_entries.GetCount())
      return;

   m_nIndex = nIndex;

   Invalidate();

   if (bNotify && m_pParentControl)
      m_pParentControl->OnControlNotify( NOTIFYCTRL_CB_CHANGED, MAKEWPARAM(m_nID, m_nIndex), (LPARAM)this );
}

int CGLComboControl::GetCurSel() const
{
   return m_nIndex;
}

int CGLComboControl::Find( LPARAM lParam )
{
   for (int i=0; i<GetCount(); i++)
   {
      if (lParam == GetItemData( i ))
         return i;
   }
   return -1;
}

LPARAM CGLComboControl::GetItemData( UINT nIndex ) const
{
   if (nIndex < 0 || nIndex > (UINT)m_entries.GetCount())
      return NULL;
   return m_entries[nIndex].m_lParam;
}

void CGLComboControl::ResetContent( BOOL bNotify/*=TRUE*/ )
{
   for (int i=0; i<m_entries.GetCount(); i++)
      OnDeleteItem( i );
   m_entries.RemoveAll();
   SetCurSel( -1, bNotify );
}

int CGLComboControl::AddString( const String &strText, LPARAM lParam/*=0L*/ )
{
   ComboEntry entry( strText, lParam );
   int nIndex = m_entries.Add( entry );
   OnAddItem( nIndex );
   return nIndex;
}

int CGLComboControl::DeleteString( UINT nIndex )
{
   ASSERT( nIndex < (UINT)GetCount() );

   OnDeleteItem( nIndex );
   m_entries.RemoveAt( nIndex );

   if (m_nIndex == nIndex)
      SetCurSel( -1, TRUE );

   return GetCount();
}

void CGLComboControl::PostInit()
{
   CGLControl::PostInit();

   if (m_glImageListCombo.IsEmpty())
      m_glImageListCombo.Create( IDB_COMBOBOX, CRect(5, 0, 10, 22), 33 );
}

void CGLComboControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   if (m_bFlatLook)
   {
      glClear( GL_DEPTH_BUFFER_BIT );

      RGBAFloat c[4] = { 
         RGBAFloat( 1.0f, 1.0f, 1.0f, 1.0f ),
            RGBAFloat( 1.0f, 1.0f, 1.0f, 1.0f ),
            RGBAFloat( 0.5f, 0.5f, 0.5f, 1.0f ),
            RGBAFloat( 0.5f, 0.5f, 0.5f, 1.0f ),
      };
      DrawGradient( c, CRect( 0, 0, m_szControl.cx, m_szControl.cy ));

      float fOffset = m_bPressed ? 1.0f : 0.0f;
      GLboolean bOldPolygonSmooth = glSet( GL_POLYGON_SMOOTH, TRUE );
      glColor4f( 0,0,0,1 );
      glBegin( GL_TRIANGLES );
      glVertex3f( m_szControl.cx - 10.0f, m_szControl.cy/2.0f - 2.5f + fOffset, 0 );
      glVertex3f( m_szControl.cx - 4.0f, m_szControl.cy/2.0f - 2.5f + fOffset, 0 );
      glVertex3f( m_szControl.cx - 7.0f, m_szControl.cy/2.0f + 2.5f + fOffset, 0 );
      glEnd();
      glSet( GL_POLYGON_SMOOTH, bOldPolygonSmooth );
   }
   else
   {
      m_glImageListCombo.DrawDiced( CRect( CPoint(0,0), m_szControl ));
      if (!IsEnabled())
         m_glImageListCombo.DrawDiced( CRect( CPoint(0,0), m_szControl ), 1 );
   }

   if (m_nIndex>=0 && m_nIndex<m_entries.GetCount())
   {
      int nOffset = 0;
      if (m_bPressed)
         nOffset = 1;
      glColor3f( 0.0f, 0.0f, 0.0f );
      String strText;
      GetText( strText );
      SetClip( CRect( 0, 0, m_szControl.cx - 23, m_szControl.cy ));
      DrawText( 5.0f, (m_szControl.cy / 2) + 4 + nOffset, strText );
   }
}

void CGLComboControl::GetText( String &strValue ) const
{
   if (m_nIndex>=0 && m_nIndex<m_entries.GetCount())
      strValue = m_entries[m_nIndex].m_strText;
}

CGLListControl *CGLComboControl::AllocListControl()
{
   return SNEW CGLComboListControl;
}

BOOL CGLComboControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   if (m_pGLListControl)
   {
      delete m_pGLListControl;
      m_pGLListControl = NULL;
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   CRect rcCombo = GetViewportRect();
   CRect rcListBox = rcCombo;
   rcListBox.top = rcCombo.bottom - 3;
   rcListBox.bottom = rcListBox.top + 16;
   rcListBox.DeflateRect( 1, 1 );

   m_pGLListControl = AllocListControl();
   m_pGLListControl->m_pComboControl = this;
   m_pGLListControl->Init( NULL, m_pGLRenderWnd, rcListBox.TopLeft(), rcListBox.Size() );

   CSize szExtents( rcListBox.Width(), 0 );
   int nCount = m_entries.GetCount();
   for (int i=0; i<nCount; i++)
   {
      CGLListItem *pListItem = m_pGLListControl->InsertItem( (LPARAM)&m_entries[i] );
      if (i == GetCurSel())
         pListItem->SetDropHilited( TRUE );
      CSize sz = m_pGLListControl->GetTextExtent( m_entries[i].m_strText.Get() );
      if (sz.cx > szExtents.cx)
         szExtents.cx = sz.cx;
      szExtents.cy += pListItem->GetItemHeight();
   }

   if (nCount)
   {
      szExtents.cy += 9;

      CRect rcClient;
      m_pGLRenderWnd->GetClientRect( &rcClient );
      if (m_pGLListControl->m_ptControl.y + szExtents.cy > rcClient.bottom) // hanging off the bottom of Snipe
      {
         if (rcCombo.top - szExtents.cy >= rcClient.top) // see if it will fit upwards
            m_pGLListControl->m_ptControl.y = rcCombo.top - szExtents.cy;
         else
            szExtents.cy = rcClient.bottom - rcListBox.top - 2;
      }

      m_pGLListControl->Move( m_pGLListControl->m_ptControl, szExtents );
   }

   return TRUE;
}

BOOL CGLComboControl::OnLButtonUp( UINT nFlags, CPoint point )
{
   if (m_pGLListControl)
   {
      m_pGLListControl->MakeActive( FALSE );
      m_pGLListControl->SetCapture();
      return TRUE;
   }
   else
      return CGLControl::OnLButtonUp( nFlags, point );
}

BOOL CGLComboControl::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_LC_CARETCHANGED:
      {
         ASSERT( m_pGLListControl == (CGLControl *)lParam );
         SetCurSel( m_pGLListControl->GetCurSel() );
         return TRUE;
      }
   }

   return CGLControl::OnControlNotify( msg, wParam, lParam );
}
