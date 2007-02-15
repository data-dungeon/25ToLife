//

#include "stdafx.h"
#include "HPropertyPointerProperty.h"
#include "Properties/PropertyPointerProperty.h"

// Info
HPropertyPointerProperty::HInfo *HPropertyPointerProperty::HInfo::New( const String &strName, const String &strMatchName )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPropertyPointerProperty::HInfo*)SNEW PropertyPointerProperty::Info( strName, strMatchName );
}

// HPropertyPointerProperty
HPropertyPointerProperty *HPropertyPointerProperty::New( HPropertyPointerProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   PropertyPointerProperty *pProperty = SNEW PropertyPointerProperty;
   pProperty->Init( (PropertyPointerProperty::Info *)pInfo, (Property *)pParent );
   return (HPropertyPointerProperty *) pProperty;
}

HPropertyPointerProperty::HInfo *HPropertyPointerProperty::GetPropertyInfo() const
{
   return (HPropertyPointerProperty::HInfo *) ((const PropertyPointerProperty *)this)->GetPropertyInfo();
}

void HPropertyPointerProperty::StoreValue( HProperty *pProperty, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((PropertyPointerProperty *)this)->StoreValue( (Property *)pProperty, time, bStoreUndo );
}

void HPropertyPointerProperty::StoreValue( HProperty *pProperty, BOOL bStoreUndo /*= TRUE*/ )
{
   ((PropertyPointerProperty *)this)->StoreValue( (Property *)pProperty, GetTime(), bStoreUndo );
}

HProperty *HPropertyPointerProperty::GetValue() const
{
   return (HProperty *)((PropertyPointerProperty *)this)->GetValue();
}

HProperty *HPropertyPointerProperty::GetValue(const Time &time) const
{
   return (HProperty *)((PropertyPointerProperty *)this)->GetValue(time);
}