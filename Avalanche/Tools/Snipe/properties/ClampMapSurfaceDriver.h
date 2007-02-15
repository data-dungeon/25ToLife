#pragma once

#include "MapSurfaceDriver.h"

class ClampMapSurfaceDriver : public MapSurfaceDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(ClampMapSurfaceDriver, MapSurfaceDriver)

            ClampMapSurfaceDriver();

// MapSurfaceDriver overrides
   virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam );
   virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pPoly, LPARAM lParam );
};
