#pragma once

#include "HProperty.h"
class Time;

class SNIPEEXPORT HStringProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HStringProperty::HInfo *New( const String &strName, const String &strMatchName, const String &defaultvalue, BOOL bMultiLine=FALSE );

      const String &GetDefaultValue() const;
      void SetDefaultValue( const String &value );

      BOOL IsMultiLine() const;
      void SetMultiLine( BOOL bMultiLine );
   };

   static HStringProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( const String &value, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( const String &value, BOOL bStoreUndo = TRUE );
   const String &GetValue( const Time &) const;
   const String &GetValue() const;
};