//

#include "stdafx.h"
#include "GLComponentCombo.h"
#include "BaseDoc.h"
#include "GLRenderWnd.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////
// CGLComponentCombo

CGLComponentCombo::CGLComponentCombo()
{
   m_nID = IDR_COMPONENTTYPE_COMBO;
   m_bEnabled = FALSE;
   m_pSOTArray = NULL;
}

void CGLComponentCombo::PostInit()
{
   CGLControl::PostInit();
}

void CGLComponentCombo::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   m_glImageListCombo.DrawDiced( CRect( CPoint(0,0), m_szControl ));
   if (!IsEnabled())
      m_glImageListCombo.DrawDiced( CRect( CPoint(0,0), m_szControl ), 1 );

   if (m_nIndex>=0 && m_nIndex<m_entries.GetCount())
   {
      int nOffset = 0;
      if (m_bPressed)
         nOffset = 1;

      float x = 5.0f;
      SnipeObjectInfo *pInfo = (SnipeObjectInfo *)GetItemData( m_nIndex );
      if (!pInfo->GetImageListIcon().IsEmpty())
      {
         pInfo->GetImageListIcon().Draw( x, (m_szControl.cy - pInfo->GetImageListIcon().GetHeight()) / 2 + nOffset );
         x += pInfo->GetImageListIcon().GetWidthEach() + 2;
      }

      glColor3f( 0.0f, 0.0f, 0.0f );
      DrawText( x, (m_szControl.cy / 2) + 4 + nOffset, m_entries[m_nIndex].m_strText );
   }
}

CGLListControl *CGLComponentCombo::AllocListControl()
{
   return SNEW CGLComponentComboListControl;
}

void CGLComponentCombo::SetCurSel( int nIndex, BOOL bNotify/*=TRUE*/ )
{
   if (m_nIndex==nIndex || nIndex>=m_entries.GetCount())
      return;

   CGLComboControl::SetCurSel( nIndex, bNotify );
   SnipeObjectInfo *pInfo = (SnipeObjectInfo *)GetItemData( m_nIndex );
   GetMainInterface()->SetCurrentSelectionSOT(pInfo?pInfo->m_nSOT:SOT_Unknown);

   if (bNotify)
      m_pGLRenderWnd->SendMessage( WM_COMMAND, m_nID );
}

void CGLComponentCombo::SetAvailableComponentTypes( SnipeObjectTypeArray *pSOTArray )
{
   m_pSOTArray = NULL;

   m_entries.RemoveAll();
   CGLComboControl::SetCurSel( -1 );

   SetEnabled( pSOTArray!=NULL );

   if (pSOTArray)
   {
      for (UINT i=0; i<pSOTArray->GetNumInfos(); i++)
      {
         SnipeObjectInfo *pInfo = pSOTArray->GetInfoAt(i);
         ASSERT(pInfo);
         AddString( pInfo->GetLocalizedName(), (LPARAM)pInfo );
      }
      SetCurSel( pSOTArray->GetCurrentIndex(), FALSE);
   }
   else
      SetCurSel( -1 );

   m_pSOTArray = pSOTArray;
}

void CGLComponentCombo::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	CComponentComboCmdUI state(this);
   state.DoUpdate(pTarget, bDisableIfNoHndler);
}

////////////////////////////////////////////////////////
// CComponentComboCmdUI

void CComponentComboCmdUI::SetAvailableComponentTypes( SnipeObjectTypeArray *pSOTArray )
{
   CGLComponentCombo *pCombo = (CGLComponentCombo *)m_pOther;

   if (pCombo)
   {
      if (pSOTArray != pCombo->m_pSOTArray)
         pCombo->SetAvailableComponentTypes( pSOTArray );
      if (pSOTArray)
         pCombo->SetCurSel( pSOTArray->GetCurrentIndex(), FALSE );
   }
}

CGLListItem *CGLComponentComboListControl::AllocItem( CGLListControl *pListControl, LPARAM lParam )
{
   CGLComboControl::ComboEntry *pComboEntry = (CGLComboControl::ComboEntry *)lParam;
   return SNEW CGLComponentListItem( (SnipeObjectInfo *)pComboEntry->m_lParam );
}

void CGLComponentListItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
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
   SnipeObjectInfo *pInfo = m_pSnipeObjectInfo;
   if (!pInfo->GetImageListIcon().IsEmpty())
   {
      pInfo->GetImageListIcon().Draw( x, (m_szControl.cy - pInfo->GetImageListIcon().GetHeight()) / 2 );
      x += pInfo->GetImageListIcon().GetWidthEach() + 2;
   }

   glColor3f( 0.0f, 0.0f, 0.0f );
   DrawText( x, (m_szControl.cy / 2) + 4, pInfo->GetLocalizedName() );

   CGLControl::Draw( nDrawFlags ); // Skip CGLListItem
}
