#pragma once

#include "HHierObject.h"

class HImagePointerProperty;
class HFloatProperty;
class HTransform2Property;

class SNIPEEXPORT HRotoscope : public HHierObject
{
public:
   HImagePointerProperty *GetImage() const;
   HTransform2Property *GetTransform();
   HFloatProperty *GetOpacity();
};