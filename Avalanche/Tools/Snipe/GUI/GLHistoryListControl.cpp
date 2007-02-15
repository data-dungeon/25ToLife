//

#include "stdafx.h"
#include "GLHistoryListControl.h"
#include "Interface/CommandList.h"
#include "Interface/SnipeCommand.h"
#include "Resource.h"
#include "HierObject.h"
#include "CommandPlugin.h"
#include "Snipe.h"

////////////////////////////////////////////////////////
// CGLHistoryListControl
BEGIN_MESSAGE_MAP(CGLHistoryListControl, CGLListControl)
   ON_UPDATE_COMMAND_UI(ID_OUTPUTCONTROL, OnUpdateOutputControl)
END_MESSAGE_MAP()

CGLHistoryListControl::CGLHistoryListControl()
{
   m_nID = IDR_HISTORYCONTROL;
   m_pCurrentObject = NULL;
}

void CGLHistoryListControl::PostInit()
{
   CGLScrollableControl::PostInit();

   SetIconBitmap( IDB_HISTORYICONS );
}

CGLListItem *CGLHistoryListControl::AllocItem( CGLListControl *pListControl, LPARAM lParam )
{
   return SNEW CGLHistoryListItem( (CGLHistoryListItem::Info *)lParam );
}

void CGLHistoryListControl::OnUpdateOutputControl( CCmdUI *pCmdUI )
{
   // Don't want default implementation
}

BOOL CGLHistoryListControl::Select( CGLListItem *pItem, UINT nCode/*=GLTC_CARET*/ )
{
   UINT nCurrentIndex = ((CGLHistoryListItem *)pItem)->GetIndex();
   m_pCurrentObject->GetCommandList()->SetCurrentPosition( nCurrentIndex );
   Invalidate();

   BOOL rvalue = CGLListControl::Select( pItem, nCode );
   GetApp().InvalidateAllViews(m_pCurrentObject);
   GetApp().DrawInvalidControls();

   return rvalue;
}

void CGLHistoryListControl::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	CHistoryCmdUI state(this);
   state.DoUpdate(pTarget, bDisableIfNoHndler);
}

void CGLHistoryListControl::SetCommandList( SnipeObject *pObject )
{
   // this could be smarter and check to see what is dirty and only do what is needed like move th current pos
   DeleteAllItems();

   m_pCurrentObject = pObject;
   if (pObject)
   {
      CGLHistoryListItem::Info info( NULL );
      InsertItem( (LPARAM)&info ); // Object in History

      CommandList *pCommandList = pObject->GetCommandList();
      if (pCommandList)
      {
         for (POSITION pos = pCommandList->GetHeadPosition(); pos; ) {
            info.m_pCommand = pCommandList->GetNext(pos);
            InsertItem( (LPARAM)&info );
         }
      }
   }

   Invalidate();
}

////////////////////////////////////////////////////////
// CHistoryCmdUI

void CHistoryCmdUI::SetCommandObject( SnipeObject *pObject )
{
   CGLHistoryListControl *pHistoryList = (CGLHistoryListControl *)m_pOther;

   if (pObject != pHistoryList->m_pCurrentObject)
      pHistoryList->SetCommandList( pObject );
   else
   {
      if (pObject == NULL)
         return;
      
      CommandList *pCommandList = pObject->GetCommandList();
      if (pCommandList)
      {
         if (pCommandList->IsRebuildListControl())
         {
            pHistoryList->SetCommandList( pObject );
            pCommandList->SetRebuildListControl( FALSE );
         }
         if (pCommandList->IsRefreshListControl())
         {
            pHistoryList->Invalidate();
            pCommandList->SetRefreshListControl( FALSE );
         }
      }
   }
}

////////////////////////////////////////////////////////
// CGLHistoryListItem

CGLHistoryListItem::CGLHistoryListItem( const Info *pInfo ) :
 m_info( *pInfo )
{
   m_nIndex = -1;
}

void CGLHistoryListItem::SetIndex( UINT nIndex )
{
   m_nIndex = nIndex;
}

void CGLHistoryListItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   if (m_nIndex % 2)
   {
      glColor4f( 0.0f, 0.0f, 0.0f, 0.06f );
      glBegin( GL_QUADS );
         glVertex2f( 0.0f, 0.0f );
         glVertex2f( m_szControl.cx, 0.0f );
         glVertex2f( m_szControl.cx, m_szControl.cy );
         glVertex2f( 0.0f, m_szControl.cy );
      glEnd();
   }

   SnipeObject *pObject = ((CGLHistoryListControl *)GetListControl())->m_pCurrentObject;
   UINT nCurPos = 0;
   CommandList *pCommandList = pObject->GetCommandList();
   if (pCommandList)
      nCurPos = pCommandList->GetCurrentPosition();

   if (m_nIndex == nCurPos)
   {
      RGBAFloat rgba( 1.0f, 1.0f, 0.5f, 0.8f );
      RGBAFloat c[4] = { rgba, rgba, rgba*0.9f, rgba*0.9f };
      DrawGradient( c, CRect( 0, 0, m_szControl.cx, GetItemHeight() ));
   }

   glColor4f( 0.0f, 0.0f, 0.0f, 0.15f );
   glBegin( GL_LINES );
      glVertex2f( 0, m_szControl.cy );
      glVertex2f( m_szControl.cx, m_szControl.cy );
   glEnd();

   float x = MARGIN;
   if (m_nIndex == 0) // Command Object
      pObject->DrawTreeItem( this, x );
   else
   {
      float fAlpha = (m_nIndex <= nCurPos) ? 1.0f : 0.4f;

      if (!GetInfo()->m_pCommand->m_pPlugin->GetIconImageList()->IsEmpty())
      {
         GetInfo()->m_pCommand->m_pPlugin->GetIconImageList()->Draw( x, 0.0f, 0, fAlpha );
         x += GetInfo()->m_pCommand->m_pPlugin->GetIconImageList()->GetWidthEach() + MARGIN;
      }

      glColor4f( 0.0f, 0.0f, 0.0f, fAlpha );
      DrawText( x, 12, m_info.m_pCommand->GetCommandInfo() );
   }

   if (m_nIndex == nCurPos)
      GetListControl()->m_glImageListIcons.Draw( MARGIN-2.0f, 0.0f, 0 );

   CGLControl::Draw( nDrawFlags ); // Skip CGLListItem
}
