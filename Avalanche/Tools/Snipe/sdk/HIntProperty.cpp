//

#include "stdafx.h"
#include "HIntProperty.h"
#include "Properties/Property.h"

// Info
HIntProperty::HInfo *HIntProperty::HInfo::New( const String &strName, const String &strMatchName, int defaultvalue, int minvalue, int maxvalue)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HIntProperty::HInfo*)SNEW IntProperty::Info(strName, strMatchName, defaultvalue, minvalue, maxvalue);
}

int HIntProperty::HInfo::GetDefaultValue() const
{
   return ((IntProperty::Info *)this)->GetDefaultValue();
}

void HIntProperty::HInfo::SetDefaultValue( int value )
{
   ((IntProperty::Info *)this)->SetDefaultValue( value );
}

int HIntProperty::HInfo::GetMinValue() const
{
   return ((IntProperty::Info *)this)->GetMinValue();
}

void HIntProperty::HInfo::SetMinValue( int value )
{
   ((IntProperty::Info *)this)->SetMinValue( value );
}

int HIntProperty::HInfo::GetMaxValue() const
{
   return ((IntProperty::Info *)this)->GetMaxValue();
}

void HIntProperty::HInfo::SetMaxValue( int value )
{
   ((IntProperty::Info *)this)->SetMaxValue( value );
}

// IntProperty
HIntProperty *HIntProperty::New( HIntProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   IntProperty *pProperty = IntProperty::Create();
   pProperty->Init( (IntProperty::Info *)pInfo, (Property *)pParent );
   return (HIntProperty *) pProperty;
}

HIntProperty::HInfo *HIntProperty::GetPropertyInfo() const
{
   return (HIntProperty::HInfo *) ((const IntProperty *)this)->GetPropertyInfo();
}

void HIntProperty::StoreValue( int value, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((IntProperty *)this)->StoreValue( value, time, bStoreUndo );
}

void HIntProperty::StoreValue( int value, BOOL bStoreUndo /*= TRUE*/ )
{
   ((IntProperty *)this)->StoreValue( value, GetTime(), bStoreUndo );
}

int HIntProperty::GetValue() const
{
   return ((IntProperty *)this)->GetValue();
}

int HIntProperty::GetValue(const Time &time) const
{
   return ((IntProperty *)this)->GetValue(time);
}