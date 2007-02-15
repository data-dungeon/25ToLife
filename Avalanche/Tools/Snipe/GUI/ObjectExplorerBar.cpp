// CObjectExplorerPane.cpp : implementation file

#include "stdafx.h"
#include "ObjectExplorerBar.h"
#include "SnipeObject.h"
#include "Resource.h"
#include "MainFrm.h"
#include "../Utility/HotKey.h"
#include <afxpriv.h>

GLImageList CGLObjectTreeControl::m_glImageListBackground;
GLImageList CGLObjectTreeControl::m_glImageListTreeIcons;

/////////////////////////////////////////////////////////////////////////////
// CGLExplorerBar

void CGLExplorerBar::PostInit()
{
   CGLControl::PostInit();

   m_glTab.Init( this, NULL, CPoint(0,0), CSize(0,TABHEIGHT) );

   m_glTab.AddTab( "Project", (LPARAM)&m_glProjectTree );
   m_glTab.AddTab( "Images", (LPARAM)&m_glImageTree );
   m_glTab.AddTab( "Models", (LPARAM)&m_glModelTree );
   m_glTab.AddTab( "Motions", (LPARAM)&m_glMotionTree );
   m_glTab.AddTab( "MotionSets", (LPARAM)&m_glMotionSetTree );
}

void CGLExplorerBar::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   m_glTab.Move( CPoint(2,2), CSize( m_szControl.cx-4, m_szControl.cy-4 ));
}

/////////////////////////////////////////////////////////////////////////////
// CGLPanelBar

void CGLPanelBar::PostInit()
{
   CGLControl::PostInit();

   m_glPropertySplitter.Init( this );
   m_glPropertySplitter.SetSize( 1, 2 );
   m_glPropertySplitter.SetHandleSize( 9 );
    m_glExplorerBar.Init( &m_glPropertySplitter );
    m_glPropertyFrame.Init( &m_glPropertySplitter );
}

void CGLPanelBar::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   m_glPropertySplitter.Move( CPoint(0,0), m_szControl );
}

//////////////////////////////////////////////////////////////////////////////
// CGLObjectTreeControl
BEGIN_MESSAGE_MAP(CGLObjectTreeControl, CGLTreeControl)
   ON_UPDATE_COMMAND_UI(ID_OUTPUTCONTROL, OnUpdateOutputControl)
END_MESSAGE_MAP()

CGLObjectTreeControl::CGLObjectTreeControl()
{
   m_pObjectHead = NULL;
}

CGLObjectTreeControl::~CGLObjectTreeControl()
{
   if (m_pObjectHead)
      m_pObjectHead->RemoveNotifyNode( this );
}

void CGLObjectTreeControl::SetObjectHead( SnipeObject *pObjectHead )
{
   CGLTreeControl::Select( NULL );

   if (m_pObjectHead)
      m_pObjectHead->RemoveNotifyNode( this );

   m_pObjectHead = pObjectHead;

   DeleteAllItems();

   if (m_pObjectHead)
   {
      InsertObjectBranch( m_pObjectHead );
      m_pObjectHead->AddNotifyNode( NOTIFYOBJ_DELETE | NOTIFYOBJ_ADDEDDESCENDANT | NOTIFYOBJ_REMOVEDDESCENDANT, this );
   }

   m_bNeedsRecalcLayout = TRUE;
   Invalidate();
}

void CGLObjectTreeControl::InsertObjectBranch( SnipeObject *pObject, CGLTreeItem *pParentItem/*=NULL*/ )
{
   CGLTreeItem *pItem = NULL;
   if (pObject != m_pObjectHead)
      pItem = InsertItem( (LPARAM)pObject, pParentItem );

   for (int i=0; i<pObject->GetNumChildren(); i++) {
      SnipeObject *pChild = pObject->GetChildAt(i);
      InsertObjectBranch( pChild, pItem );
   }
}

CGLTreeItem *CGLObjectTreeControl::AllocItem( CGLTreeControl *pTreeControl, LPARAM lParam )
{
   SnipeObject *pObj = (SnipeObject *)lParam;
   if (pObj)
   {
      CGLObjectTreeItem *pItem = (CGLObjectTreeItem *)pObj->AllocTreeItem( pTreeControl );
      pObj->AddNotifyNode( NOTIFYOBJ_SELECTIONCHANGED | NOTIFYOBJ_DELETE | NOTIFYOBJ_VISIBILITYINTREECHANGED | NOTIFYOBJ_ICONCHANGED, pItem );
      return pItem;
   }
   else
      return SNEW CGLObjectTreeItem( pTreeControl );
}

void CGLObjectTreeControl::PostInit()
{
   CGLTreeControl::PostInit();

   if (m_glImageListTreeIcons.IsEmpty()) {
      MakeRenderContextCurrent();
      m_glImageListTreeIcons.Create( IDB_TREEICONS, 16 );
   }
   if (m_glImageListBackground.IsEmpty()) {
      MakeRenderContextCurrent();
      m_glImageListBackground.Create( IDB_BACKGROUND );
   }
}

void CGLObjectTreeControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   if (m_bNeedsRecalcLayout)
      Move( m_ptControl, GetControlSize() );

   GLDrawPrepper prep(this);

   //if (nDrawFlags & DRAW_PARENT)
   //   m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

//   glClearColor( 1.0f , 1.0f, 1.0f, 1.0f );
//   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   m_glImageListBackground.Draw( CRect(CPoint(0,0), m_szControl), FALSE, TRUE );

   glColor4f( 0.0f, 0.0f, 0.0f, 0.2f );
   glBegin( GL_QUADS );
      glVertex2f( 0.0f, 0.0f );
      glVertex2f( m_szControl.cx, 0.0f );
      glVertex2f( m_szControl.cx, m_szControl.cy );
      glVertex2f( 0.0f, m_szControl.cy );
   glEnd();

   CGLScrollableControl::Draw( nDrawFlags );

   DrawFocusRing();
//   DrawShadow();
}

CGLObjectTreeItem *CGLObjectTreeControl::FindTreeItem( CGLObjectTreeItem *pParentItem, SnipeObject *pChildObject )
{
   CArray<CGLControl *> *pChildControlArray;
   if (pParentItem)
      pChildControlArray = &pParentItem->m_childcontrolarray;
   else
      pChildControlArray = &m_childcontrolarray;

   for (int i=0; i<pChildControlArray->GetCount(); i++)
   {
      CGLControl *pControl = (*pChildControlArray)[i];
      if (pControl->IsTreeItem()) {
         CGLObjectTreeItem *pItem = (CGLObjectTreeItem *)pControl;
         if (pItem->GetObject() == pChildObject)
            return pItem;
      }
   }
   return NULL;
}

CGLObjectTreeItem *CGLObjectTreeControl::FindTreeItem( SnipeObject *pObject )
{
   if (pObject == m_pObjectHead || !pObject)
      return NULL;

   CGLObjectTreeItem *pParentItem = FindTreeItem( pObject->GetParent() );
   return FindTreeItem( pParentItem, pObject );
}

SnipeObject *CGLObjectTreeControl::GetCaretObject()
{
   if (m_pCaretItem==NULL)
      return NULL;

   return ((CGLObjectTreeItem *)m_pCaretItem)->GetObject();
}

BOOL CGLObjectTreeControl::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_DELETE:
      {
         if (m_pObjectHead == (SnipeObject *)lParam)
            SetObjectHead(NULL);
         break;
      }
      case NOTIFYOBJ_ADDEDDESCENDANT: {
         HierObject *pObject = (HierObject *)lParam;
         HierObject *pParent = (HierObject *)pObject->GetParent();
         CGLObjectTreeItem *pParentItem = FindTreeItem( pParent );
         CGLObjectTreeItem *pInsertAfter = NULL;
         if (pParent)
         {
            int nIndex = pParent->FindChildIndex( pObject );
            if (nIndex > 0)
            {
               SnipeObject *pOlderSibling = pParent->GetChildAt( nIndex - 1 );
               pInsertAfter = FindTreeItem( pOlderSibling );
            }
         }
         InsertItem( (LPARAM)pObject, pParentItem, pInsertAfter );
         break;
      }
      case NOTIFYOBJ_REMOVEDDESCENDANT:
         SnipeObject *pObject = (SnipeObject *)lParam;
         CGLObjectTreeItem *pItem = FindTreeItem( pObject );
         RemoveItem( pItem );
         break;
   }
   return SnipeObject::OnObjectNotify( msg, wParam, lParam );
}

void CGLObjectTreeControl::OnUpdateOutputControl( CCmdUI *pCmdUI )
{
   CGLOutputControlCmdUI *pOutputControlCmdUI = (CGLOutputControlCmdUI *)pCmdUI;
   SnipeObject *pCommandObject = NULL;
   SnipeObject *pCaretObject = GetCaretObject();
   if (pCaretObject)
   {
      pCommandObject = pCaretObject->GetCommandObject();
      pOutputControlCmdUI->SetCurrentWorkingObject( pCommandObject );
      pOutputControlCmdUI->SetCurrentCommandObject( pCommandObject );
   } // If delete occurs, then the COmmandObject is set to NULL in CGLObjectTreeItem::OnObjectNotify first
}

////////////////////////////////////////
// CGLObjectTreeItem

CGLObjectTreeItem::CGLObjectTreeItem( CGLTreeControl *pTreeControl )
 : CGLTreeItem( pTreeControl )
{
   m_bOdd = FALSE;
}

CGLObjectTreeItem::~CGLObjectTreeItem()
{
   if (IsSelected())
      GetTreeControl()->Deselect( this );

   SnipeObject *pObject = GetObject();
   if (pObject)
      pObject->RemoveNotifyNode( this );
}

void CGLObjectTreeItem::PostInit()
{
   CGLTreeItem::PostInit();

   SnipeObject *pObject = GetObject();
   if (pObject)
      pObject->OnTreeItemCreated( this );
}

BOOL CGLObjectTreeItem::IsSelected()
{
   return GetObject() && GetObject()->IsSelected();
}

void CGLObjectTreeItem::SetSelected( BOOL bSelected )
{
   CGLTreeItem::SetSelected( bSelected );

   if (bSelected)
   {
      SnipeObject *pObject = GetObject();
      pObject->Select();
   }
}

int CGLObjectTreeItem::GetItemHeight()
{
   return GetObject()->GetItemHeight();
}

//BOOL CGLControl::IsOnScreen()
//{
//}

void CGLObjectTreeItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   //if (nDrawFlags & DRAW_PARENT)
   //{
   //   if (!IsOnScreen())
   //      return;
   //}

   GLDrawPrepper prep(this);

   if (m_bOutsideScissorArea) // this is only set if your parent asked you to draw
      return;

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

	glClear( GL_DEPTH_BUFFER_BIT );

   if (IsSelected())
   {
      RGBAFloat rgba( 1.0f, 1.0f, 0.5f, 0.8f );
      RGBAFloat c[4] = { rgba, rgba, rgba*0.9f, rgba*0.9f };
      DrawGradient( c, CRect( 2.0f+CGLTreeControl::m_glImageListExpandIcons.GetWidthEach()+2.0f, 0, m_szControl.cx, GetItemHeight() ));
   }

   SnipeObject *pObject = GetObject();

   if (pObject)
   {
      if (pObject->HasVisibleChild())
         CGLTreeControl::m_glImageListExpandIcons.Draw( 2.0f, 4.0f, IsExpanded() );
      pObject->DrawTreeItem( this, 2.0f+CGLTreeControl::m_glImageListExpandIcons.GetWidthEach()+4.0f );
   }

   CGLTreeItem::Draw( nDrawFlags & ~DRAW_PARENT );
}

BOOL SnipeObject::GetContextMenu( CMenu &menu )
{
   CMenu popup;
   popup.CreatePopupMenu();

   menu.CreateMenu();
   menu.AppendMenu( MF_POPUP, (UINT)popup.GetSafeHmenu() );

   CList< SnipeObjectType > list;
   GetSOTList( list );
   BOOL bAppendSeparator = FALSE;
   for (POSITION sotpos=list.GetHeadPosition(); sotpos; )
   {
      SnipeObjectType sot = list.GetNext( sotpos );
      CommandUIPluginList *pCommandUIPluginList = GetMainInterface()->GetCommandUIPluginManager()->GetCommandUIPluginListObject( sot );
      for (POSITION cmdpos=pCommandUIPluginList->GetHeadPosition(); cmdpos; )
      {
         CommandUIPlugin *pCommandUIPlugin = pCommandUIPluginList->GetNext( cmdpos );
         String strMenu = pCommandUIPlugin->GetTitleName();
         if (!pCommandUIPlugin->m_actualaccellist.IsEmpty())
         {
            strMenu += '\t';

            CommandUIAccel &commanduiaccel = pCommandUIPlugin->m_actualaccellist.GetHead();
            strMenu += ::GetHotKeyDescription( commanduiaccel.m_key, commanduiaccel.m_virt );
         }
         if (bAppendSeparator)
         {
            popup.AppendMenu( MF_SEPARATOR );
            bAppendSeparator = FALSE;
         }
         popup.AppendMenu( 0, pCommandUIPlugin->m_nCmdID, strMenu.Get() );
      }
      bAppendSeparator = TRUE;
   }

   UINT nDefaultItemID = GetDefaultMenuItemID();
   if (nDefaultItemID != -1)
      popup.SetDefaultItem( nDefaultItemID );

   popup.Detach(); // we don't want popup to go out of scope and destroy the hmenu that it wraps

   return TRUE;
}

BOOL CGLObjectTreeItem::GetContextMenu( const CPoint &ptControl, CMenu &menu )
{
   return GetObject()->GetContextMenu( menu );
}

BOOL CGLObjectTreeItem::OnLButtonDblClk( UINT nFlags, CPoint point )
{
   UINT nDefaultItemID = GetObject()->GetDefaultMenuItemID();
   if (nDefaultItemID != -1)
   {
      // TODO: determine if the id is enabled via sending a cmdui
      AfxGetMainWnd()->SendMessage( WM_COMMAND, MAKEWPARAM(nDefaultItemID, 0), NULL );
      return TRUE;
   }
   return CGLTreeItem::OnLButtonDblClk( nFlags, point );
}

BOOL CGLObjectTreeItem::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
   if (GetObject()->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ))
      return TRUE;

   return CGLTreeItem::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CGLObjectTreeItem::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_DELETE:
      {
         if (GetMainInterface()->GetCurrentCommandObject() == GetObject())
         {
            GetMainInterface()->SetCurrentWorkingObject( NULL );
            GetMainInterface()->SetCurrentCommandObject( NULL );

         }
         delete this;
         return TRUE;
      }
      case NOTIFYOBJ_SELECTIONCHANGED:
      {
         BOOL bSelected = (BOOL)wParam;
         CGLTreeItem::SetSelected( bSelected );
         break;
      }
      case NOTIFYOBJ_VISIBILITYINTREECHANGED:
      {
         BOOL bVisible = (BOOL)wParam;
         SetHidden( !bVisible );
         break;
      }
      case NOTIFYOBJ_ICONCHANGED:
      {
         Invalidate();
         break;
      }
   }
   return SnipeObject::OnObjectNotify( msg, wParam, lParam );
}

void CGLObjectTreeItem::BeginDrag( CPoint point )
{
   CGLTreeItem::BeginDrag( point );
   SnipeObject *pObject = GetObject();
   if (pObject)
      pObject->OnBeginDrag( point, this );
}

void CGLObjectTreeItem::DoDrag( CPoint point )
{
   CGLTreeItem::DoDrag( point ); 
   SnipeObject *pObject = GetObject();
   if (pObject)
   {
      ControlToClient(point);
      CGLObjectTreeItem *pHitObjectTreeItem = (CGLObjectTreeItem*)GetTreeControl()->HitTestAll( point );
      if (pHitObjectTreeItem)
         pObject->OnDoDrag( point, this, pHitObjectTreeItem );
   }
}

void CGLObjectTreeItem::EndDrag( CPoint point )
{
   CGLTreeItem::EndDrag( point );
   SnipeObject *pObject = GetObject();
   if (pObject)
   {
      ControlToClient(point);
      CGLObjectTreeItem *pHitObjectTreeItem = (CGLObjectTreeItem*)GetTreeControl()->HitTestAll( point );
      if (pHitObjectTreeItem)
      {
         pObject->OnEndDrag( point, this, pHitObjectTreeItem );
 //        GetApp().InvalidateAllViews(pHitObjectTreeItem->GetObject());
      }
   }
//   GetApp().InvalidateAllViews(pObject);
   GetApp().DrawInvalidControls();
}