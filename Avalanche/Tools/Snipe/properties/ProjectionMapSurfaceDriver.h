#pragma once

#include "MapSurfaceDriver.h"

class ProjectionMapSurfaceDriver : public MapSurfaceDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(ProjectionMapSurfaceDriver, MapSurfaceDriver)

   static TypeProperty::Info      m_nProjectionMethodInfo;
   static TransformProperty::Info m_transformInfo;

   TypeProperty       m_nProjectionMethod; enum { PM_PLANAR, PM_CYLINDRICAL, PM_SPHERICAL };
   TransformProperty  m_transform;

   ProjectionMapSurfaceDriver();

// SurfaceDriver overrides
   virtual void Bind();
   virtual void UnBind();

// MapSurfaceDriver overrides
   virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam );
   virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pPoly, LPARAM lParam );

// SnipeObject overrides
   virtual void OnFocusChanged( BOOL bFocus );
   virtual void DrawFocused( CGLBaseView *pView );
};
