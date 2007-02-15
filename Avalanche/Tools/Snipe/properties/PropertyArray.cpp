//

#include "stdafx.h"
#include "PropertyArray.h"
#include "Property.h"

Property *PropertyArray::Find( const String &strMatchName, BOOL bRecursive/*=TRUE*/ )
{
   for (int i=0; i < GetSize(); i++ )
   {
      Property *pProperty = GetAt(i);
      if (pProperty->GetMatchName() == strMatchName)
         return pProperty;

      if (bRecursive)
      {
         pProperty = pProperty->GetPropertyArray()->Find( strMatchName, bRecursive );
         if (pProperty)
            return pProperty;
      }
   }

   return NULL;      
}

BOOL PropertyArray::Remove( Property *pProperty )
{
   for (int i=0; i < m_nSize; i++ )
   {
      if (GetAt(i) == pProperty)
      {
         RemoveAt(i);
         return TRUE;
      }
   }

   return FALSE;
}

void PropertyArray::SetCachePointers( const PropertyArray *pCachePropertyArray )
{
   for (int i=0; i < m_nSize; i++ )
   {
      Property *pProperty = GetAt( i );
      pProperty->m_pCache = pCachePropertyArray->GetAt( i );
      ASSERT( pProperty->m_pPropertyInfo == pProperty->m_pCache->m_pPropertyInfo );

      PropertyArray *pPropertyArray = pProperty->GetPropertyArray();
      if (pPropertyArray)
         pPropertyArray->SetCachePointers( pProperty->m_pCache->GetPropertyArray() );
   }
}