#include "stdafx.h"
#include "Property.h"

/////////////////////////////////////////
// Property

Property::Property()
{
}

Property::~Property()
{
}

void Property::Init( Property::Info *pInfo, ParentableObject *pParent )
{
   m_pPropertyInfo = pInfo;

   m_pParent = pParent;
   if (m_pParent)
      m_pParent->m_propertyarray.Add( this );
}

String Property::GetTitle()
{
   if (m_pPropertyInfo)
      return m_pPropertyInfo->m_strName;
   else
      return "No Property Info!";
}

void Property::SendNotificationToDependents( NotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   for (ParentableObject *pParent=m_pParent; pParent; pParent=pParent->m_pParent) {
      pParent->OnObjectNotify( msg, wParam, lParam );
      if (!pParent->IsProperty())
         break;
   }
   NotifyNodes( msg, wParam, lParam );
}

void Property::OnValueStored()
{
   Validate();
   SendNotificationToDependents( NOTIFY_VALUECHANGED, 0, (LPARAM)this );
}

/////////////////////////////////////////
// IntProperty

BOOL IntProperty::GetValueString( String &strValue, int nComponentIndex/*=0*/ )
{
   strValue = String( GetValue());
   return TRUE;
}

BOOL IntProperty::StoreValueFromString( const String &strValue, int nComponentIndex/*=0*/, const Time &time )
{
   int value;
   sscanf( strValue.Get(), "%d", &value );
   StoreValue( value, time );
   return TRUE;
}

void IntProperty::OnScrub( int nPixels, int nComponentIndex/*=0*/ )
{
   StoreValue( GetValue() + nPixels, GetTime());
}

/////////////////////////////////////////
// TypeProperty

BOOL TypeProperty::GetValueString( String &strValue, int nComponentIndex/*=0*/ )
{
   strValue = GetPropertyInfo()->m_typenodearray[ GetValue() ].m_strName;
   return TRUE;
}

BOOL TypeProperty::StoreValueFromString( const String &strValue, int nComponentIndex/*=0*/, const Time &time )
{
//   int value;
//   sscanf( strValue.Get(), "%d", &value );
//   StoreValue( value, time );
   return TRUE;
}

/////////////////////////////////////////
// FloatProperty

BOOL FloatProperty::GetValueString( String &strValue, int nComponentIndex/*=0*/ )
{
   float value = GetValue();
   Round( value, 0.01f );
   char temp[30];
   sprintf( temp, "%2g", value );
   strValue.Set( temp, strlen( temp ));
   return TRUE;
}

BOOL FloatProperty::StoreValueFromString( const String &strValue, int nComponentIndex/*=0*/, const Time &time )
{
   float value;
   sscanf( strValue.Get(), "%f", &value );
   StoreValue( value, time );
   return TRUE;
}

void FloatProperty::OnScrub( int nPixels, int nComponentIndex/*=0*/ )
{
   StoreValue( GetValue() + nPixels, GetTime());
}

/////////////////////////////////////////
// Vector2Property

BOOL Vector2Property::GetValueString( String &strValue, int nComponentIndex/*=0*/ )
{
   float value = GetValue()[nComponentIndex];
   Round( value, 0.01f );
   char temp[30];
   sprintf( temp, "%2g", value );
   strValue.Set( temp, strlen( temp ));
   return TRUE;
}

BOOL Vector2Property::StoreValueFromString( const String &strValue, int nComponentIndex/*=0*/, const Time &time )
{
   float fValue;
   sscanf( strValue.Get(), "%f", &fValue );
   Vector2 vValue = GetValue();
   vValue[nComponentIndex] = fValue;
   StoreValue( vValue, time );
   return TRUE;
}

void Vector2Property::OnScrub( int nPixels, int nComponentIndex/*=0*/ )
{
   Vector2 vValue = GetValue();
   vValue[nComponentIndex] += nPixels;

   StoreValue( vValue );
}

/////////////////////////////////////////
// VectorProperty

BOOL VectorProperty::GetValueString( String &strValue, int nComponentIndex/*=0*/ )
{
   float value = GetValue()[nComponentIndex];
   Round( value, 0.01f );
   char temp[30];
   sprintf( temp, "%2g", value );
   strValue.Set( temp, strlen( temp ));
   return TRUE;
}

BOOL VectorProperty::StoreValueFromString( const String &strValue, int nComponentIndex/*=0*/, const Time &time )
{
   float fValue;
   sscanf( strValue.Get(), "%f", &fValue );
   Vector vValue = GetValue();
   vValue[nComponentIndex] = fValue;
   StoreValue( vValue, time );
   return TRUE;
}

void VectorProperty::OnScrub( int nPixels, int nComponentIndex/*=0*/ )
{
   Vector vValue = GetValue();
   vValue[nComponentIndex] += nPixels;

   StoreValue( vValue );
}

/////////////////////////////////////////
// TransformProperty

VectorProperty::Info TransformProperty::m_vTranslateInfo( "Translate", Vector(0), Vector(-MAX_FLOAT), Vector(MAX_FLOAT) );
VectorProperty::Info TransformProperty::m_vScaleInfo( "Scale", Vector(1), Vector(-MAX_FLOAT), Vector(MAX_FLOAT) );
RotateProperty::Info TransformProperty::m_vRotateInfo( "Rotate", Vector(0), Vector(-MAX_FLOAT), Vector(MAX_FLOAT) );

TransformProperty::TransformProperty()
{
   m_vTranslate.Init( &m_vTranslateInfo, this );
   m_vScale.Init( &m_vScaleInfo, this );
   m_vRotate.Init( &m_vRotateInfo, this );
}
