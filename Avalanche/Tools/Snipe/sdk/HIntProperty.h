#pragma once

#include "HProperty.h"
class Time;

class SNIPEEXPORT HIntProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HIntProperty::HInfo *New(const String &strName, const String &strMatchName, int defaultvalue, int minvalue, int maxvalue);
      int GetDefaultValue() const;
      void SetDefaultValue( int value );
 
      int GetMinValue() const;
      void SetMinValue( int value );

      int GetMaxValue() const;
      void SetMaxValue( int value );
   };

   static HIntProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( int value, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( int value, BOOL bStoreUndo = TRUE );
   int GetValue( const Time &) const;
   int GetValue() const;
};