//

#include "stdafx.h"
#include "HColorProperty.h"
#include "Properties/Property.h"

// Info
HColorProperty::HInfo *HColorProperty::HInfo::New( const String &strName, const String &strMatchName, const RGBFloat &defaultvalue, const RGBFloat &minvalue, const RGBFloat &maxvalue)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HColorProperty::HInfo*)SNEW ColorProperty::Info(strName, strMatchName, defaultvalue, minvalue, maxvalue);
}

RGBFloat HColorProperty::HInfo::GetDefaultValue() const
{
   return ((ColorProperty::Info *)this)->GetDefaultValue();
}

void HColorProperty::HInfo::SetDefaultValue( const RGBFloat &value)
{
   ((ColorProperty::Info *)this)->SetDefaultValue( value);
}

// ColorProperty
HColorProperty *HColorProperty::New( HColorProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ColorProperty *pProperty = ColorProperty::Create();
   pProperty->Init( (ColorProperty::Info *)pInfo, (Property *)pParent );
   return (HColorProperty *) pProperty;
}

HColorProperty::HInfo *HColorProperty::GetPropertyInfo() const
{
   return (HColorProperty::HInfo *) ((const ColorProperty *)this)->GetPropertyInfo();
}

void HColorProperty::StoreValue( const RGBFloat &value, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((ColorProperty *)this)->StoreValue( value, time, bStoreUndo );
}

void HColorProperty::StoreValue( const RGBFloat &value, BOOL bStoreUndo /*= TRUE*/ )
{
   ((ColorProperty *)this)->StoreValue( value, GetTime(), bStoreUndo );
}

RGBFloat HColorProperty::GetValue() const
{
   return ((ColorProperty *)this)->GetValue();
}

RGBFloat HColorProperty::GetValue(const Time &time) const
{
   return ((ColorProperty *)this)->GetValue(time);
}