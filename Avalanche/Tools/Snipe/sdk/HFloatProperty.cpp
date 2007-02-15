//

#include "stdafx.h"
#include "HFloatProperty.h"
#include "Properties/Property.h"

// Info
HFloatProperty::HInfo *HFloatProperty::HInfo::New( const String &strName, const String &strMatchName, float defaultvalue, float minvalue, float maxvalue)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HFloatProperty::HInfo*)SNEW FloatProperty::Info(strName, strMatchName, defaultvalue, minvalue, maxvalue);
}

float HFloatProperty::HInfo::GetDefaultValue() const
{
   return ((FloatProperty::Info *)this)->GetDefaultValue();
}

void HFloatProperty::HInfo::SetDefaultValue( float value )
{
   ((FloatProperty::Info *)this)->SetDefaultValue( value );
}

float HFloatProperty::HInfo::GetMinValue() const
{
   return ((FloatProperty::Info *)this)->GetMinValue();
}

void HFloatProperty::HInfo::SetMinValue( float value )
{
   ((FloatProperty::Info *)this)->SetMinValue( value );
}

float HFloatProperty::HInfo::GetMaxValue() const
{
   return ((FloatProperty::Info *)this)->GetMaxValue();
}

void HFloatProperty::HInfo::SetMaxValue( float value )
{
   ((FloatProperty::Info *)this)->SetMaxValue( value );
}

// FloatProperty
HFloatProperty *HFloatProperty::New( HFloatProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   FloatProperty *pProperty = FloatProperty::Create();
   pProperty->Init( (FloatProperty::Info *)pInfo, (Property *)pParent );
   return (HFloatProperty *) pProperty;
}

HFloatProperty::HInfo *HFloatProperty::GetPropertyInfo() const
{
   return (HFloatProperty::HInfo *) ((const FloatProperty *)this)->GetPropertyInfo();
}

void HFloatProperty::StoreValue( float value, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((FloatProperty *)this)->StoreValue( value, time, bStoreUndo );
}

void HFloatProperty::StoreValue( float value, BOOL bStoreUndo /*= TRUE*/ )
{
   ((FloatProperty *)this)->StoreValue( value, GetTime(), bStoreUndo );
}

float HFloatProperty::GetValue() const
{
   return ((FloatProperty *)this)->GetValue();
}

float HFloatProperty::GetValue(const Time &time) const
{
   return ((FloatProperty *)this)->GetValue(time);
}