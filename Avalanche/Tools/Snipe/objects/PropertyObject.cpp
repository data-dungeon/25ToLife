#include "stdafx.h"
#include "PropertyObject.h"
#include "Properties/Property.h"

IMPLEMENT_SNIPEOBJECT_PURE( PropertyObject )

PropertyObject::PropertyObject()
{
}

PropertyObject::~PropertyObject()
{
}

void PropertyObject::PreDestruct()
{
   for (int i=0; i < m_propertyarray.GetCount(); i++)
   {
      Property *pProperty = m_propertyarray[i];
      if (!pProperty->m_bIsPreDestructed)// this handles non allocated properties
         pProperty->PreDestruct();
   }

   SnipeObject::PreDestruct();
}

void PropertyObject::AddProperty( Property *pProperty ) 
{ 
   ASSERT(pProperty); 

   if (!pProperty->m_bIsPostConstructed) // this handles non allocated properties
      pProperty->PostConstruct();

   if (pProperty->GetParent() == NULL) // We do this because some manipulators share properties across ManipulatorPlugins and ManipulatorInstances, thus we let the ManipulatorPlugins keep the parent pointer for notifications
      pProperty->SetParent( this ); 

   m_propertyarray.Add( pProperty ); 

   OnObjectNotify( NOTIFYOBJ_ADDPROPERTY, 0, (LPARAM)pProperty );
}

void PropertyObject::ShowProperties( BOOL bVisible )
{
   for (int i=0; i < m_propertyarray.GetCount(); i++)
      m_propertyarray[i]->SetVisible( bVisible );
}

BOOL PropertyObject::HasVisibleProperty()
{
   for (int i=0; i < m_propertyarray.GetCount(); i++)
      if (m_propertyarray[i]->IsVisible())
         return TRUE;

   return FALSE;
}

void PropertyObject::MarkSaveDependents()
{
   if (m_bSaveDependent)
      return;

   SnipeObject::MarkSaveDependents();

   for ( int i=0; i<m_propertyarray.GetCount(); i++ )
      m_propertyarray[i]->MarkSaveDependents();
}

void PropertyObject::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   SnipeObject::Save( bs );

   for ( int i=0; i<m_propertyarray.GetCount(); i++ )
      m_propertyarray[i]->Save( bs, TRUE );
}
