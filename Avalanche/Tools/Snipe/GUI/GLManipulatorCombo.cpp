#include "stdafx.h"
#include "GLManipulatorCombo.h"
#include "ManipulatorPlugin.h"
#include "MainFrm.h"
#include "BaseDoc.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////
// CGLManipulatorCombo

CGLManipulatorCombo::CGLManipulatorCombo()
{
   m_nID = IDR_MANIPULATOR_COMBO;
   m_bEnabled = FALSE;
   m_pSnipeManipulatorList = NULL;
}

void CGLManipulatorCombo::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   int nOldIndex = m_nIndex;
   m_nIndex = -1;
   CGLComboControl::Draw( nDrawFlags );
   m_nIndex = nOldIndex;

   if (m_nIndex>=0 && m_nIndex<m_entries.GetCount())
   {
      int nOffset = 0;
      if (m_bPressed)
         nOffset = 1;

      float x = 5.0f;
      SnipeManipulator *pManipulator = (SnipeManipulator *)GetItemData( m_nIndex );

      GLImageList *pImageList = pManipulator->m_pPlugin->GetImageList();

      if (!pImageList->IsEmpty())
      {
         pImageList->Draw( x, (m_szControl.cy - pImageList->GetHeight()) / 2 + nOffset );
         x += pImageList->GetWidthEach() + 2;
      }

      glColor3f( 0.0f, 0.0f, 0.0f );
      DrawText( x, (m_szControl.cy / 2) + 4 + nOffset, m_entries[m_nIndex].m_strText );
   }
}

CGLListControl *CGLManipulatorCombo::AllocListControl()
{
   return SNEW CGLManipulatorComboListControl;
}

void CGLManipulatorCombo::SetCurSel( int nIndex, BOOL bNotify/*=TRUE*/ )
{
   if (m_nIndex==nIndex || nIndex>=m_entries.GetCount())
      return;

   CGLComboControl::SetCurSel( nIndex, bNotify );

   if (bNotify)
      m_pGLRenderWnd->SendMessage( WM_COMMAND, m_nID );
}

void CGLManipulatorCombo::SetAvailableManipulators( SnipeManipulatorList *pSnipeManipulatorList )
{
   m_pSnipeManipulatorList = NULL;

   SnipeManipulator *pPrevManipulator = NULL;
   if (pSnipeManipulatorList)
      pPrevManipulator = pSnipeManipulatorList->GetCurrentManipulatorInstance()?pSnipeManipulatorList->GetCurrentManipulatorInstance()->m_pSnipeManipulator:NULL;

   m_entries.RemoveAll();
   CGLComboControl::SetCurSel( -1 );

   if (pSnipeManipulatorList)
   {
      int i = 0, nCurSel = -1;
      for (POSITION pos=pSnipeManipulatorList->GetHeadPosition(); pos; i++)
      {
         SnipeManipulator *pManipulator = pSnipeManipulatorList->GetNext( pos );
         AddString( pManipulator->m_pPlugin->GetTitleName(), (LPARAM)pManipulator );
         if (pPrevManipulator && pManipulator == pPrevManipulator)
            nCurSel = i;
      }
      if (nCurSel == -1 && GetCount())
         nCurSel = 0;
      SetCurSel( nCurSel );
   }
   else
      SetCurSel( -1 );

   if (pSnipeManipulatorList)
      pSnipeManipulatorList->m_bDirty = FALSE;

   m_pSnipeManipulatorList = pSnipeManipulatorList;
}

SnipeManipulator *CGLManipulatorCombo::GetManipulator()
{
   if (GetCurSel() == -1)
      return NULL;
   else
      return (SnipeManipulator *)GetItemData( GetCurSel() );
}

void CGLManipulatorCombo::SetManipulator( SnipeManipulator *pSnipeManipulator )
{
   int nIndex = Find( (LPARAM)pSnipeManipulator );
   SetCurSel( nIndex );
}

void CGLManipulatorCombo::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	CManipulatorComboCmdUI state(this);
   state.DoUpdate( pTarget, bDisableIfNoHndler );
}

CGLListItem *CGLManipulatorComboListControl::AllocItem( CGLListControl *pListControl, LPARAM lParam )
{
   CGLComboControl::ComboEntry *pComboEntry = (CGLComboControl::ComboEntry *)lParam;
   return SNEW CGLManipulatorListItem( (SnipeManipulator *)pComboEntry->m_lParam );
}

int CGLManipulatorListItem::GetItemHeight()
{
   return m_pSnipeManipulator->m_pPlugin->GetImageList()->GetHeight();
}

void CGLManipulatorListItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   //if (m_nIndex % 2)
   //{
   //   glColor4f( 0.0f, 0.0f, 0.0f, 0.06f );
   //   glBegin( GL_QUADS );
   //   glVertex2f( 0.0f, 0.0f );
   //   glVertex2f( m_szControl.cx, 0.0f );
   //   glVertex2f( m_szControl.cx, m_szControl.cy );
   //   glVertex2f( 0.0f, m_szControl.cy );
   //   glEnd();
   //}

   if (IsSelected() || IsDropHilited())
   {
      RGBAFloat rgba( 1.0f, 1.0f, 0.5f, 0.8f );
      RGBAFloat c[4] = { rgba, rgba, rgba*0.9f, rgba*0.9f };
      DrawGradient( c, CRect( 0, 0, m_szControl.cx, GetItemHeight() ));
   }

   float x = MARGIN;
   GLImageList *pImageList = m_pSnipeManipulator->m_pPlugin->GetImageList();
   if (!pImageList->IsEmpty())
   {
      pImageList->Draw( x, (m_szControl.cy - (int)pImageList->GetHeight()) / 2 );
      x += pImageList->GetWidthEach() + 2;
   }

   glColor3f( 0.0f, 0.0f, 0.0f );
   DrawText( x, (m_szControl.cy / 2) + 4, m_pSnipeManipulator->m_pPlugin->GetTitle() );

   CGLControl::Draw( nDrawFlags ); // Skip CGLListItem
}

////////////////////////////////////////////////////////
// CManipulatorComboCmdUI

void CManipulatorComboCmdUI::SetAvailableManipulators( SnipeManipulatorList *pSnipeManipulatorList )
{
   CGLManipulatorCombo *pCombo = (CGLManipulatorCombo *)m_pOther;
   if (!pCombo)
      return;

   if (pSnipeManipulatorList != pCombo->m_pSnipeManipulatorList || (pSnipeManipulatorList && pSnipeManipulatorList->m_bDirty))
      pCombo->SetAvailableManipulators( pSnipeManipulatorList );
}

void CManipulatorComboCmdUI::SetManipulator( SnipeManipulator *pSnipeManipulator )
{
   CGLManipulatorCombo *pCombo = (CGLManipulatorCombo *)m_pOther;
   if (!pCombo)
      return;

   if (pSnipeManipulator && pSnipeManipulator->IsComboInvalid())
      pCombo->Invalidate();

   if (pSnipeManipulator != pCombo->GetManipulator())
      pCombo->SetManipulator( pSnipeManipulator );
}

