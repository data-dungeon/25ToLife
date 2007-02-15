#pragma once

#include "HBoolProperty.h"

class HImage;
class HTransform2Property;

class SNIPEEXPORT HImagePointerProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
   public:
      static HImagePointerProperty::HInfo *New(const String &strName, const String &strMatchName);
   };

   static HImagePointerProperty *New( HInfo *pInfo, HProperty *pParent );
   HInfo *GetPropertyInfo() const;

   void StoreValue( HImage *pImage, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( HImage *pImage, BOOL bStoreUndo = TRUE );
   HImage *GetValue( const Time &) const;
   HImage *GetValue() const;

   HTransform2Property *GetTransform();
};