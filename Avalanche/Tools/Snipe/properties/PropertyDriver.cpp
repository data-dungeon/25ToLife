#include "stdafx.h"
#include "PropertyDriver.h"
#include "GUI/PropertyBar.h"
#include "..\Interface\MainInterface.h"

IMPLEMENT_SNIPEOBJECT_PURE( PropertyDriver )

PropertyPointerProperty::Info PropertyDriver::m_pPropertyInfo( "Property", "Property" );

PropertyDriver::PropertyDriver()
{
   m_pProperty.Init( &m_pPropertyInfo, this );
   m_pNextPropertyDriver = NULL;
   m_bCurrent = TRUE;
}

String PropertyDriver::GetTitle()
{
   if ( m_pProperty )
      return m_strName + " (" + ((PropertyPointerProperty *)&m_pProperty)->GetValue()->GetTitle() + ")";
   else
      return m_strName + " (No Property)";
}

SnipeObject *PropertyDriver::GetObjectDriven()
{
   Property *pProperty = GetProperty();
   if (pProperty)
      return pProperty->GetObject();
   else
      return NULL;
}

BOOL PropertyDriver::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_pProperty)
         {
            if (m_pProperty.GetPrevValue())
               m_pProperty.GetPrevValue()->RemovePropertyDriver( this );
            if (m_pProperty.GetValue())
               m_pProperty.GetValue()->AddPropertyDriver( this );
         }
      }
   }

   return HierObject::OnObjectNotify( msg, wParam, lParam );
}
