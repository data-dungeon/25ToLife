#pragma once

#include "HProperty.h"
#include "../Utility/ATime.h"

class SNIPEEXPORT HBoolProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HBoolProperty::HInfo *New(const String &strName, const String &strMatchName, BOOL defaultvalue);
      BOOL GetDefaultValue() const;
      void SetDefaultValue( BOOL value );
      void SetAutoHideChildren( BOOL bAutoHideChildren );
   };

   static HBoolProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( BOOL value, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( BOOL value, BOOL bStoreUndo = TRUE );
   BOOL GetValue( const Time &) const;
   BOOL GetValue() const;
};