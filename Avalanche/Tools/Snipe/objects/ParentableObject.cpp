#include "stdafx.h"
#include "ParentableObject.h"
#include "GUI/GLRenderWnd.h"

IMPLEMENT_SNIPEOBJECT_PURE( ParentableObject )

ParentableObject::ParentableObject()
{
   m_pParent = NULL;
}

ParentableObject::~ParentableObject()
{
}

BOOL ParentableObject::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   DeleteInfo *&pParentableObjectDeleteInfo = (ParentableObject::DeleteInfo *&)pDeleteInfo;

   if (pParentableObjectDeleteInfo == NULL)
      pParentableObjectDeleteInfo = SNEW DeleteInfo( this );

   if (m_pParent)
   {
      pParentableObjectDeleteInfo->m_pParent = m_pParent;
      m_pParent = NULL;
   }

   return InstanceObject::Delete( pDeleteInfo, strOutError );
}

BOOL ParentableObject::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   if (!InstanceObject::UnDelete( pDeleteInfo ))
      return FALSE;

   DeleteInfo *pParentableObjectDeleteInfo = (ParentableObject::DeleteInfo *)pDeleteInfo;
   m_pParent = pParentableObjectDeleteInfo->m_pParent;

   return TRUE;
}

BOOL ParentableObject::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   return InstanceObject::OnObjectNotify( msg, wParam, lParam );
}

void ParentableObject::MarkSaveDependents()
{
   if (m_bSaveDependent)
      return;

   SnipeObject::MarkSaveDependents();

   for (SnipeObject *pParent=GetParent(); pParent; pParent=pParent->GetParent())
      m_pParent->m_bSaveDependent = TRUE;
}
