#pragma once

#include "Properties/TexCoordsPointerProperty.h"
#include "Properties/MapSurfaceDriver.h"

//class Poly;
//class TessPoly;

class UVMapSurfaceDriver : public MapSurfaceDriver
{
public:
   static TexCoordsPointerProperty::Info m_pTexCoordsInfo;

   TexCoordsPointerProperty m_pTexCoords;

   DECLARE_SNIPEOBJECT_CREATE( UVMapSurfaceDriver, MapSurfaceDriver )

   UVMapSurfaceDriver();

   TexCoordsPointerProperty *GetTexCoords() { return &m_pTexCoords; }

// SurfaceDriver overrides
   virtual BOOL IsValid( Poly *pPoly, LPARAM &lParam );
   virtual BOOL IsValid( TessPoly *pTessPoly, LPARAM &lParam );
   virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam );
   virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pPoly, LPARAM lParam );

// InstanceObject overrides
   virtual float DrawUVExtras( CGLTexCoordView *pView );
};
