#pragma once

#include "HProperty.h"

class SNIPEEXPORT HFloatProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HFloatProperty::HInfo *New(const String &strName, const String &strMatchName, float defaultvalue, float minvalue, float maxvalue);
      float GetDefaultValue() const;
      void SetDefaultValue( float value );
 
      float GetMinValue() const;
      void SetMinValue( float value );

      float GetMaxValue() const;
      void SetMaxValue( float value );
   };

   static HFloatProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( float value, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( float value, BOOL bStoreUndo = TRUE );
   float GetValue( const Time &) const;
   float GetValue() const;
};