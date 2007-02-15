//

#include "stdafx.h"
#include "HVector2Property.h"
#include "Properties/Property.h"

// Info
HVector2Property::HInfo *HVector2Property::HInfo::New( const String &strName, const String &strMatchName, const Vector2 &defaultvalue, const Vector2 &minvalue, const Vector2 &maxvalue)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVector2Property::HInfo*)SNEW Vector2Property::Info(strName, strMatchName, defaultvalue, minvalue, maxvalue);
}

Vector2 HVector2Property::HInfo::GetDefaultValue() const
{
   return ((const Vector2Property::Info *)this)->GetDefaultValue();
}

void HVector2Property::HInfo::SetDefaultValue( const Vector2 &value)
{
   ((Vector2Property::Info *)this)->SetDefaultValue( value);
}

// Vector2Property
HVector2Property *HVector2Property::New( HVector2Property::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   Vector2Property *pProperty = Vector2Property::Create();
   pProperty->Init( (Vector2Property::Info *)pInfo, (Property *)pParent );
   return (HVector2Property *) pProperty;
}

HVector2Property::HInfo *HVector2Property::GetPropertyInfo() const
{
   return (HVector2Property::HInfo *) ((const Vector2Property *)this)->GetPropertyInfo();
}

void HVector2Property::StoreValue( const Vector2 &value, const Time &time, BOOL bStoreUndo /*= TRUE*/ )
{
   ((Vector2Property *)this)->StoreValue( value, time, bStoreUndo );
}

void HVector2Property::StoreValue( const Vector2 &value, BOOL bStoreUndo /*= TRUE*/ )
{
   ((Vector2Property *)this)->StoreValue( value, GetTime(), bStoreUndo );
}

Vector2 HVector2Property::GetValue() const
{
   return ((Vector2Property *)this)->GetValue();
}

Vector2 HVector2Property::GetValue(const Time &time) const
{
   return ((Vector2Property *)this)->GetValue(time);
}