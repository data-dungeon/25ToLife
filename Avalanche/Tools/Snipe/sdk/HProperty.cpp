//

#include "stdafx.h"
#include "HProperty.h"
#include "Properties/Property.h"
#include "../gui/Snipe.h"

HProperty::HInfo *HProperty::HInfo::New( const String &strName, const String &strMatchName)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HProperty::HInfo*)SNEW Property::Info(strName, strMatchName);
}

void HProperty::HInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   delete ((Property::Info *)pVoid);
}

void HProperty::HInfo::SetName( const String &strName )
{
   ((Property::Info *)this)->m_strName = strName;
}

String HProperty::HInfo::GetName() const
{
   return ((Property::Info *)this)->m_strName;
}

void HProperty::HInfo::SetMatchName( const String &strMatchName )
{
   ((Property::Info *)this)->m_strMatchName = strMatchName;
}

String HProperty::HInfo::GetMatchName() const
{
   return ((Property::Info *)this)->m_strMatchName;
}

// HProperty
HProperty *HProperty::New( HProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   Property *pProperty = Property::Create();
   pProperty->Init( (Property::Info *)pInfo, (Property *)pParent );
   return (HProperty *) pProperty;
}

BOOL HProperty::IsSet() const
{
   return ((Property *)this)->IsSet();
}

void HProperty::SetSet( BOOL bSet )
{
   ((Property *)this)->SetSet( bSet );
}

HSnipeObject *HProperty::GetObject()
{
   return (HSnipeObject *)((Property *)this)->GetObject();
}

void HProperty::StoreValueFromString( const String &strValue, const Time &consttime/*=TAGGEDTIME*/, BOOL bStoreUndo/*=TRUE*/)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   Time time(consttime);

   if (time.IsTagged())
      time = GetApp().GetTime();
   ((Property *)this)->StoreValueFromString( strValue, time, bStoreUndo );
}

void HProperty::GetValueString( String &strOutValue, const Time &consttime/*TAGGEDTIME*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   Time time(consttime);

   if (time.IsTagged())
      time = GetApp().GetTime();
   ((Property *)this)->GetValueString( strOutValue, time );
}
