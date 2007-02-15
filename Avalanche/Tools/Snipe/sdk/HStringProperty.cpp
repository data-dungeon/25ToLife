//

#include "stdafx.h"
#include "HStringProperty.h"
#include "Properties/Property.h"

// Info
HStringProperty::HInfo *HStringProperty::HInfo::New( const String &strName, const String &strMatchName, const String &defaultvalue, BOOL bMultiLine/*=FALSE*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HStringProperty::HInfo *)SNEW StringProperty::Info( strName, strMatchName, defaultvalue, bMultiLine );
}

const String &HStringProperty::HInfo::GetDefaultValue() const
{
   return ((StringProperty::Info *)this)->GetDefaultValue();
}

void HStringProperty::HInfo::SetDefaultValue( const String &value )
{
   ((StringProperty::Info *)this)->SetDefaultValue( value );
}

BOOL HStringProperty::HInfo::IsMultiLine() const
{
   return ((StringProperty::Info *)this)->IsMultiLine();
}

void HStringProperty::HInfo::SetMultiLine( BOOL bMultiLine )
{
   ((StringProperty::Info *)this)->SetMultiLine( bMultiLine );
}

// StringProperty
HStringProperty *HStringProperty::New( HStringProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   StringProperty *pProperty = StringProperty::Create();
   pProperty->Init( (StringProperty::Info *)pInfo, (Property *)pParent );
   return (HStringProperty *) pProperty;
}

HStringProperty::HInfo *HStringProperty::GetPropertyInfo() const
{
   return (HStringProperty::HInfo *) ((const StringProperty *)this)->GetPropertyInfo();
}

void HStringProperty::StoreValue( const String &value, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((StringProperty *)this)->StoreValue( value, time, bStoreUndo );
}

void HStringProperty::StoreValue( const String &value, BOOL bStoreUndo/*= TRUE*/ )
{
   ((StringProperty *)this)->StoreValue( value, GetTime(), bStoreUndo );
}

const String &HStringProperty::GetValue() const
{
   return ((StringProperty *)this)->GetValue();
}

const String &HStringProperty::GetValue(const Time &time) const
{
   return ((StringProperty *)this)->GetValue(time);
}