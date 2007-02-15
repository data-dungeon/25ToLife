#pragma once

#include "HProperty.h"
#include "../Utility/RGBColor.h"

class SNIPEEXPORT HColorProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HColorProperty::HInfo *New(const String &strName, const String &strMatchName, const RGBFloat &defaultvalue, const RGBFloat &minvalue, const RGBFloat &maxvalue);
      RGBFloat GetDefaultValue() const;
      void SetDefaultValue( const RGBFloat &value );
   };

   static HColorProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( const RGBFloat &value, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( const RGBFloat &value, BOOL bStoreUndo = TRUE );
   RGBFloat GetValue( const Time &) const;
   RGBFloat GetValue() const;
};