#pragma once

#include "HMapSurfaceDriver.h"

class HTexCoordsPointerProperty;

class SNIPEEXPORT HUVMapSurfaceDriver : public HMapSurfaceDriver
{
public:
   HTexCoordsPointerProperty *GetTexCoords();
};
