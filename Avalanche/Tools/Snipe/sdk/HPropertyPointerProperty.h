#pragma once

#include "HProperty.h"

class SNIPEEXPORT HPropertyPointerProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
   public:
      static HPropertyPointerProperty::HInfo *New(const String &strName, const String &strMatchName);
   };

   static HPropertyPointerProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( HProperty *pProperty, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( HProperty *pProperty, BOOL bStoreUndo = TRUE );
   HProperty *GetValue( const Time &) const;
   HProperty *GetValue() const;
};