//

#include "stdafx.h"
#include "HVectorProperty.h"
#include "Properties/Property.h"

// Info
HVectorProperty::HInfo *HVectorProperty::HInfo::New( const String &strName, const String &strMatchName, const Vector &defaultvalue, const Vector &minvalue, const Vector &maxvalue)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVectorProperty::HInfo*)SNEW VectorProperty::Info(strName, strMatchName, defaultvalue, minvalue, maxvalue);
}

Vector HVectorProperty::HInfo::GetDefaultValue() const
{
   return ((VectorProperty::Info *)this)->GetDefaultValue();
}

void HVectorProperty::HInfo::SetDefaultValue( const Vector &value)
{
   ((VectorProperty::Info *)this)->SetDefaultValue( value);
}

// VectorProperty
HVectorProperty *HVectorProperty::New( HVectorProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   VectorProperty *pProperty = VectorProperty::Create();
   pProperty->Init( (VectorProperty::Info *)pInfo, (Property *)pParent );
   return (HVectorProperty *) pProperty;
}

HVectorProperty::HInfo *HVectorProperty::GetPropertyInfo() const
{
   return (HVectorProperty::HInfo *) (( const VectorProperty *)this)->GetPropertyInfo();
}

void HVectorProperty::StoreValue( const Vector &value, const Time &time, BOOL bStoreUndo /*= TRUE*/ )
{
   ((VectorProperty *)this)->StoreValue( value, time, bStoreUndo );
}

void HVectorProperty::StoreValue( const Vector &value, BOOL bStoreUndo /*= TRUE*/ )
{
   ((VectorProperty *)this)->StoreValue( value, GetTime(), bStoreUndo );
}

Vector HVectorProperty::GetValue() const
{
   return ((VectorProperty *)this)->GetValue();
}

Vector HVectorProperty::GetValue(const Time &time) const
{
   return ((VectorProperty *)this)->GetValue(time);
}