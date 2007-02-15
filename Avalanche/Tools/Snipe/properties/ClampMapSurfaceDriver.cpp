#include "stdafx.h"
#include "ClampMapSurfaceDriver.h"

IMPLEMENT_SNIPEOBJECT_CREATE( ClampMapSurfaceDriver )

ClampMapSurfaceDriver::ClampMapSurfaceDriver()
{
   m_strName = "Clamp Map";
}

Vector2 ClampMapSurfaceDriver::GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam )
{
   //double clampeduvs[] = { 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 
   //   0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
   //   0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
   //   0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0 };
   //int grouprotflag = 0;
   //pModel->m_patchrotmap.FindKey((int)pPoly,grouprotflag);
   //   Vector2 uv((float)clampeduvs[(nIndex+grouprotflag+3)*2], (float)clampeduvs[((nIndex+grouprotflag+3)*2)+1]);

   Vector2 uv;

   switch (nIndex)
   {
      case 0:
         uv.Set( 0.0f, 0.0f );
         break;
      case 1:
         uv.Set( 1.0f, 0.0f );
         break;
      case 2:
         uv.Set( 1.0f, 1.0f );
         break;
      case 3:
         uv.Set( 0.0f, 1.0f );
         break;
   }

   //TransformUV( uv );

   return uv;
}

Vector2 ClampMapSurfaceDriver::GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pPoly, LPARAM lParam )
{
   return GetUVAt( viewmatrix, nIndex, (Poly *)NULL, lParam ); // TODO
}
