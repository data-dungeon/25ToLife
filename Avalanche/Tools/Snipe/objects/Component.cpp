#include "stdafx.h"
#include "Component.h"
#include "ComponentList.h"
#include "Interface/MainInterface.h"

IMPLEMENT_COMPONENT_PURE( Component )

Component::Component()
{
   m_nID = -1;
   m_pList = NULL;
   m_bComponent = TRUE;
}

Component::Component( UINT nID )
{
   SetID( nID );
   m_pList = NULL;
   m_bComponent = TRUE;
}

Component::~Component()
{
}

SnipeObject *Component::GetOwner()
{ 
   if (!m_pList)
      return NULL;
   return m_pList->GetOwner(); 
}

SnipeObject *Component::GetParent() const
{
   if (!m_pList)
      return NULL;
   return m_pList->GetOwner(); 
}

void Component::Disconnect()
{
   m_bSelectedAtDisconnect = IsSelected();
   m_bFocusAtDisconnect = SelectionList::GetFocusObject()==this;
   m_bNearestFocusAtDisconnect = SelectionList::GetNearestFocusObject()==this;

   if (SelectionList::GetMouseOverObject()==this)
      SelectionList::SetMouseOverObject(NULL);

   if (m_bSelectedAtDisconnect)
   {
      SnipeObject *pOwner = GetOwner();
      ASSERT(pOwner);
      
      SelectionList *pList = pOwner->GetCommandObject(FALSE)->GetSelectionList();
      ASSERT( pList );

      pList->Remove( this );
   }

   if (m_bFocusAtDisconnect)
      SelectionList::SetFocusObjectOnlyNoCheck( NULL );

   if (m_bNearestFocusAtDisconnect)
      SelectionList::SetNearestFocusObjectOnlyNoCheck( NULL );

   SnipeObject *pMirror = GetMirrorObject();
   if ( pMirror )
      pMirror->SetMirrorObject(NULL);
}

void Component::Reconnect()
{
   if (m_bSelectedAtDisconnect)
   {
      SelectionList *pList = GetMainInterface()->GetCurrentSelectionList();
      pList->Add( this );
   }

   if (m_bFocusAtDisconnect)
      SelectionList::SetFocusObjectOnlyNoCheck(this);

   if (m_bNearestFocusAtDisconnect)
      SelectionList::SetNearestFocusObjectOnlyNoCheck(this);

   if (m_pMirror)
      m_pMirror->SetMirrorObject(this);
}

BOOL Component::SafeSetID( UINT nID )
{
   ASSERT(m_pList);

   if (nID == m_nID)
      return TRUE;

   if (m_pList->Find(nID))
      return FALSE;

   m_pList->ChangeComponentID( m_nID, nID);
   m_nID = nID;

   return TRUE;
}

BOOL Component::RemoveFromList()
{
   return m_pList->Remove( this );
}

void Component::ReInsertIntoList()
{
   if (m_bAllowReinsert)
      m_pList->ReInsert( this );
}

void Component::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObject::Save( bs ); // don't call PropertyObject, it save properties

   bs << mar << m_nID; // no nl

   if (bWriteHeader)
      bs << nl;
}

BOOL Component::Load( BaseStream &bs )
{
   bs >> m_nID;

   return TRUE;
}

BOOL Component::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   if (msg == NOTIFYOBJ_VISIBILITYCHANGED || msg == NOTIFYOBJ_LOCKEDCHANGED)
      GetOwner()->OnObjectNotify( msg, wParam, lParam );

   return PropertyObject::OnObjectNotify( msg, wParam, lParam );
}

void Component::DrawSelected( CGLBaseView *pView )
{
   RGBAFloat color;
   float fAlpha;
   if (IsLocked())
   {
      color = g_selectedcolorlocked;
      fAlpha = 0.05f;
   }
   else
   {
      color = g_selectedcolor;
      fAlpha = 0.15f;
   }

   glDisable( GL_DEPTH_TEST );
   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha()*fAlpha );
   Draw( pView );
   glEnable( GL_DEPTH_TEST );

   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
   Draw( pView );
}

void Component::DrawMirrored( CGLBaseView *pView )
{
   RGBAFloat color;
   float fAlpha;
   if (IsLocked())
   {
      color = g_mirrorcolorlocked;
      fAlpha = 0.05f;
   }
   else
   {
      color = g_mirrorcolor;
      fAlpha = 0.15f;
   }

   glDisable( GL_DEPTH_TEST );
   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha()*fAlpha );
   Draw( pView );
   glEnable( GL_DEPTH_TEST );

   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
   Draw( pView );
}

void Component::DrawFocused( CGLBaseView *pView )
{
   RGBAFloat color;
   if (IsLocked())
      color = g_focuscolorlocked;
   else
      color = g_focuscolor;

   glDisable( GL_DEPTH_TEST );
   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha()*0.3f );
   Draw( pView );
   glEnable( GL_DEPTH_TEST );

   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
   Draw( pView );
}
