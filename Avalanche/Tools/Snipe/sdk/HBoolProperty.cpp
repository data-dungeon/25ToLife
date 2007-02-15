//

#include "stdafx.h"
#include "HBoolProperty.h"
#include "Properties/Property.h"

// Info
HBoolProperty::HInfo *HBoolProperty::HInfo::New( const String &strName, const String &strMatchName, BOOL defaultvalue)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HBoolProperty::HInfo*)SNEW BoolProperty::Info( strName, strMatchName, defaultvalue );
}

BOOL HBoolProperty::HInfo::GetDefaultValue() const
{
   return ((BoolProperty::Info *)this)->GetDefaultValue();
}

void HBoolProperty::HInfo::SetDefaultValue( BOOL value )
{
   ((BoolProperty::Info *)this)->SetDefaultValue( value );
}

void HBoolProperty::HInfo::SetAutoHideChildren( BOOL bAutoHideChildren )
{
   ((BoolProperty::Info *)this)->SetAutoHideChildren( bAutoHideChildren );
}

// BoolProperty
HBoolProperty *HBoolProperty::New( HBoolProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   BoolProperty *pProperty = BoolProperty::Create();
   pProperty->Init( (BoolProperty::Info *)pInfo, (Property *)pParent );
   return (HBoolProperty *)pProperty;
}

HBoolProperty::HInfo *HBoolProperty::GetPropertyInfo() const
{
   return (HBoolProperty::HInfo *)((const BoolProperty *)this)->GetPropertyInfo();
}

void HBoolProperty::StoreValue( BOOL value, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((BoolProperty *)this)->StoreValue( value, time, bStoreUndo );
}

void HBoolProperty::StoreValue( BOOL value, BOOL bStoreUndo /*= TRUE*/ )
{
   ((BoolProperty *)this)->StoreValue( value, GetTime(), bStoreUndo );
}

BOOL HBoolProperty::GetValue() const
{
   return ((BoolProperty *)this)->GetValue();
}

BOOL HBoolProperty::GetValue( const Time &time ) const
{
   return ((BoolProperty *)this)->GetValue( time );
}