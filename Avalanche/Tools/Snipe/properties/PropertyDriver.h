#pragma once

#include "HierObject.h"
#include "PropertyPointerProperty.h"

class PropertyDriver : public HierObject // When you derive off this class, you must write a GetPossiblePointerPropertyChoices
{
public:
   DECLARE_SNIPEOBJECT_PURE(PropertyDriver, HierObject)

   static PropertyPointerProperty::Info m_pPropertyInfo;

   PropertyPointerProperty m_pProperty;

   PropertyDriver *m_pNextPropertyDriver;

   PropertyDriver();

   Property *GetProperty() { return (Property *)m_pProperty.GetValue(); }
   SnipeObject *GetObjectDriven();
   PropertyDriver *GetNextPropertyDriver() { return m_pNextPropertyDriver; }

// Overridables
   virt_base virtual void OnPropertyValueStored( const Time &time, BOOL bStoreUndo ) {}
   virt_base virtual BOOL IsApplyDirty( Time time ) { return FALSE; }
   virt_base virtual void Apply( Time time ) {}
   virt_base virtual void Apply( Property *pProperty, Time time ) {}

// SnipeObject Overrides
   virtual String GetTitle();
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
