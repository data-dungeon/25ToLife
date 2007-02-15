#include "stdafx.h"
#include "InstanceObject.h"
#include "GUI/GLBaseView.h"
#include "../Interface/MainInterface.h"

IMPLEMENT_SNIPEOBJECT_PURE( InstanceObject )

InstanceObject::InstanceObject()
{
   m_pCache = NULL;
   m_nInstanceNumber = 0;
}

InstanceObject::~InstanceObject()
{
   if (m_pCache)
      m_pCache->RemoveInstance( this );

   for (POSITION pos=m_instancelist.GetHeadPosition(); pos; )
   {
      InstanceObject *pInstance = m_instancelist.GetNext( pos );
      pInstance->Destroy();
   }
}

BOOL InstanceObject::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   DeleteInfo *&pInstanceObjectDeleteInfo = (InstanceObject::DeleteInfo *&)pDeleteInfo;
   if (pInstanceObjectDeleteInfo==NULL)
      pInstanceObjectDeleteInfo = SNEW DeleteInfo( this );

   pInstanceObjectDeleteInfo->m_pCache = m_pCache;

   if (m_pCache)
      pInstanceObjectDeleteInfo->m_nInstanceIndex = m_pCache->RemoveInstance( this );

   return PropertyObject::Delete( pDeleteInfo, strOutError );
}

BOOL InstanceObject::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   if (!PropertyObject::UnDelete( pDeleteInfo ))
      return FALSE;

   DeleteInfo *pInstanceObjectDeleteInfo = (InstanceObject::DeleteInfo *)pDeleteInfo;
   m_pCache = pInstanceObjectDeleteInfo->m_pCache;
   if (m_pCache)
      m_pCache->m_instancelist.InsertAfter( m_instancelist.FindIndex( pInstanceObjectDeleteInfo->m_nInstanceIndex ), this );

   return TRUE;
}

InstanceObject *InstanceObject::CreateInstance()
{
   InstanceObject *pNewInstance = (InstanceObject *)GetSnipeObjectInfo()->CreateObject();

   BOOL bFound;
   do
   {
      bFound = FALSE;
      for (POSITION pos=m_instancelist.GetHeadPosition(); pos; )
      {
         InstanceObject *pInstanceObject = m_instancelist.GetNext( pos );
         if (pInstanceObject->m_nInstanceNumber == pNewInstance->m_nInstanceNumber)
         {
            bFound = TRUE;
            pNewInstance->m_nInstanceNumber++;
            break;
         }
      }
   } while (bFound);

   m_instancelist.AddTail( pNewInstance );
   pNewInstance->m_pCache = this;
   pNewInstance->GetPropertyArray()->SetCachePointers( GetPropertyArray() );

   return pNewInstance;
}

int InstanceObject::RemoveInstance( InstanceObject *pInstance )
{
   if (pInstance->m_pCache != this)
      return -1;

   POSITION pos = m_instancelist.Find( pInstance );
   if (!pos)
      return -1;

   m_instancelist.RemoveAt( pos );
   pInstance->m_pCache = NULL;

   return 0; // TODO: return index
}

InstanceObject *InstanceObject::FindInstance( const String &name ) const
{
   for (POSITION pos=m_instancelist.GetHeadPosition(); pos; )
   {
      InstanceObject *pInstance = m_instancelist.GetNext( pos );
      if (pInstance->GetMatchName() == name)
         return pInstance;
   }

   return NULL;
}

void InstanceObject::MarkSaveDependents()
{
   if (m_bSaveDependent)
      return;

   PropertyObject::MarkSaveDependents();

   if (m_pCache) // Remove when cache becomes a PointerProperty
      m_pCache->MarkSaveDependents();
}
