#pragma once

#include "HProperty.h"

class HVector2Property;
class HFloatProperty;

class SNIPEEXPORT HTransform2Property : public HProperty
{
public:
   class SNIPEEXPORT HInfo : public HProperty::HInfo
   {
    public:
      static HTransform2Property::HInfo *New(const String &strName, const String &strMatchName, const Vector2 &defaultvalue, const Vector2 &minvalue, const Vector2 &maxvalue);
   };
   static HTransform2Property *New( HInfo *pInfo, HProperty *pParent );

   HVector2Property *GetTranslate();
   HVector2Property *GetScale();
   HFloatProperty *GetRotate();
};