#pragma once

#include "HProperty.h"

class SNIPEEXPORT HVectorProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HVectorProperty::HInfo *New(const String &strName, const String &strMatchName, const Vector &defaultvalue, const Vector &minvalue, const Vector &maxvalue);
      Vector GetDefaultValue() const;
      void SetDefaultValue( const Vector &value );
   };

   static HVectorProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( const Vector &value, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( const Vector &value, BOOL bStoreUndo = TRUE );
   Vector GetValue( const Time &) const;
   Vector GetValue() const;
};