#pragma once

#include "HProperty.h"

class HTranslateProperty;
class HScaleProperty;
class HRotateProperty;

class SNIPEEXPORT HTransformProperty : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HTransformProperty::HInfo *New(const String &strName, const String &strMatchName, const Vector &defaultvalue, const Vector &minvalue, const Vector &maxvalue);
   };
   static HTransformProperty *New( HInfo *pInfo, HProperty *pParent );

   HTranslateProperty *GetTranslate();
   HScaleProperty *GetScale();
   HRotateProperty *GetRotate();
};