#pragma once

#include "HSurfaceDriver.h"

class HImagePointerProperty;
class HTypeProperty;
class HFloatProperty;

class SNIPEEXPORT HMapSurfaceDriver : public HSurfaceDriver
{
public:
   enum BlendStyle
   {
      REPLACE,
      MULTIPLY,
      ADDITIVE,
      DECAL
   };

   HImagePointerProperty *GetImage() const;
   HTypeProperty *GetBlendStyle();
   HFloatProperty *GetOpacity();
};
