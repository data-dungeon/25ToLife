#pragma once

#include "MapSurfaceDriver.h"

class EnvironmentMapSurfaceDriver : public MapSurfaceDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(EnvironmentMapSurfaceDriver, MapSurfaceDriver)

   EnvironmentMapSurfaceDriver() {}

// SurfaceDriver overrides
   virtual void Bind();
   virtual void UnBind();

// MapSurfaceDriver overrides
   virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam );
   virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pPoly, LPARAM lParam );
};
