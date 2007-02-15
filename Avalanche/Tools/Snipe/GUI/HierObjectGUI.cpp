
#include "stdafx.h"
#include "MainFrm.h"
#include "HierObject.h"
#include "SnipeDocTemplate.h"
#include "BaseDoc.h"
#include "GLBaseFrame.h"
#include "Selection.h"
#include "Interface/SelectionList.hpp"
#include "Resource.h"

BEGIN_MESSAGE_MAP(HierObject, ParentableObject)
   SNIPEOBJECT_ON_COMMAND(ID_OBJECT_EDIT, OnObjectEdit)
END_MESSAGE_MAP()

BOOL SnipeObject::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   NotifyNodes( msg, wParam, lParam );

   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED: // Don't want this on Interpolation
         GetApp().InvalidateAllViews( this );
         break;
   }

   return FALSE;
}

void SnipeObject::OnFocusChanged( BOOL bFocus )
{
   if (bFocus)
      GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glObjectPropertyBar.SetObject( this );
   else if (GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glObjectPropertyBar.m_glTree.m_pObjectHead == this)
      GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glObjectPropertyBar.SetObject( NULL );
}

BOOL HierObject::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   if (m_pDocument)
      m_pDocument->OnObjectNotify( msg, wParam, lParam );

   switch (msg)
   {
      case NOTIFYOBJ_ADDEDCHILD:
         {
            SnipeObject *pChild = (SnipeObject *)lParam;
            if (pChild->GetSOT() == SOT_SelectionContainer)
               SetSelectionContainer( (SelectionContainer *)pChild );
         }
         break;
      case NOTIFYOBJ_REMOVEDCHILD:
         {
            SnipeObject *pChild = (SnipeObject *)lParam;
            if (pChild == GetSelectionContainer())
               SetSelectionContainer( NULL );
         }
         break;
      case NOTIFYOBJ_REMOVEDFROMPROJECT:
         m_bSelectedAtDisconnect = IsSelected();
         if (m_bSelectedAtDisconnect)
         {
            SelectionList *pList = GetCommandObject()->GetSelectionList();
            if (pList)
               pList->Remove( this );
         }
         break;
      case NOTIFYOBJ_ADDEDTOPROJECT:
         if (m_bSelectedAtDisconnect)
         {
            SelectionList *pList = GetCommandObject()->GetSelectionList();
            pList->Add( this );
         }
         break;
   }

   return ParentableObject::OnObjectNotify( msg, wParam, lParam );
}

void HierObject::OnEditDelete()
{
   Destroy();
   GetApp().DrawInvalidControls();
}

void HierObject::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
   pCmdUI->Enable( IsDeletable());
}

void HierObject::OnObjectEdit()
{
   CSnipeDocTemplate *pTemplate = GetDocTemplate();
   if (!pTemplate)
      return;

   CGLBaseDocFrame *pBaseDocFrame = GetMainFrame()->GetMainRenderWnd()->m_glDocumentBar.m_pCurrentFrame;
   CGLTabControl *pTabControl = &pBaseDocFrame->m_glTabControl;

   if (m_pDocument)
   {
      int nTabIndex = pTabControl->FindTab( (LPARAM)m_pDocument );
      pTabControl->SelectTab( nTabIndex );
   }
   else
   {
      m_pDocument = (CBaseDoc *)GetDocTemplate()->CreateNewDocument();
      m_pDocument->SetObjectHead( this );
      if (GetFileName().Get())
         m_pDocument->SetPathName( GetFileName().Get(), FALSE );

      CGLTabControl::Tab tab( (LPCTSTR)m_pDocument->GetTitle(), (LPARAM)m_pDocument );
      int nTabIndex = pTabControl->AddTab( tab );
      pBaseDocFrame->Move( pBaseDocFrame->m_ptControl, pBaseDocFrame->GetControlSize() );
      pTabControl->SelectTab( nTabIndex );
   }

   Select();
   GetApp().DrawInvalidControls();

   return;
}
