#pragma once

#include "HProperty.h"

class SNIPEEXPORT HVector2Property : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HVector2Property::HInfo *New(const String &strName, const String &strMatchName, const Vector2 &defaultvalue, const Vector2 &minvalue, const Vector2 &maxvalue);
      Vector2 GetDefaultValue() const;
      void SetDefaultValue( const Vector2 &value );
   };

   static HVector2Property *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( const Vector2 &value, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( const Vector2 &value, BOOL bStoreUndo = TRUE );
   Vector2 GetValue( const Time &) const;
   Vector2 GetValue() const;
};