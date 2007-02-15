#pragma once

#include "HProperty.h"
class HTexCoords;

class SNIPEEXPORT HTexCoordsPointerProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
   public:
      static HTexCoordsPointerProperty::HInfo *New(const String &strName, const String &strMatchName);
   };

   static HTexCoordsPointerProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( HTexCoords *pTexCoords, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( HTexCoords *pTexCoords, BOOL bStoreUndo = TRUE );
   HTexCoords *GetValue( const Time &) const;
   HTexCoords *GetValue() const;
};