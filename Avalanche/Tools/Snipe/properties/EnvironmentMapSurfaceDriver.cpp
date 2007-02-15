//

#include "stdafx.h"
#include "EnvironmentMapSurfaceDriver.h"
#include "Poly.h."
#include "TessPoly.h."
#include "VertexNormal.h"

IMPLEMENT_SNIPEOBJECT_CREATE( EnvironmentMapSurfaceDriver )

Vector2 EnvironmentMapSurfaceDriver::GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam )
{
   return 0;
   static Vector vXUnitVector( 1.0f, 0.0f, 0.0f );
   static Vector vYUnitVector( 0.0f, 1.0f, 0.0f );

   VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt( nIndex );
   Vector vNormal = pVertexNormal->GetDir();

   TransformVectorNoTranslate( viewmatrix, vNormal );

   Vertex *pVertex = pPoly->GetVertexAt( nIndex );
   Vector vPos = pVertex->GetPos();
   vPos *= viewmatrix;
   Vector vEyeDir = vPos.Normalized();

   Vector vReflectDir = vEyeDir.Reflect( vNormal );

   Vector2 uv;
   uv.y = vReflectDir.Dot( vYUnitVector );

   vReflectDir.y = 0.0f;

   uv.x = vReflectDir.Dot( vXUnitVector ) * 0.5f;

   if (vReflectDir.z >= 0.0f)
      uv = (uv + 1.0f) * 0.5f;
   else
   {
      uv.y = (uv.y + 1.0f) * 0.5f;
      uv.x = (-uv.y) * 0.5f + 1.0f;
   }

   //TransformUV( uv );

   return uv;
}

void EnvironmentMapSurfaceDriver::Bind()
{
   MapSurfaceDriver::Bind();

   glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
   glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
   glEnable( GL_TEXTURE_GEN_S );
   glEnable( GL_TEXTURE_GEN_T );
}

void EnvironmentMapSurfaceDriver::UnBind()
{
   glDisable( GL_TEXTURE_GEN_S );
   glDisable( GL_TEXTURE_GEN_T );

   MapSurfaceDriver::UnBind();
}

Vector2 EnvironmentMapSurfaceDriver::GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pPoly, LPARAM lParam )
{
   return 0;
   static Vector vXUnitVector( 1.0f, 0.0f, 0.0f );
   static Vector vYUnitVector( 0.0f, 1.0f, 0.0f );

   VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt( nIndex );
   Vector vNormal = pVertexNormal->GetDir();

   TransformVectorNoTranslate( viewmatrix, vNormal );

   Vertex *pVertex = pPoly->GetVertexAt( nIndex );
   Vector vPos = pVertex->GetPos();
   vPos *= viewmatrix;
   Vector vEyeDir = vPos.Normalized();

   Vector vReflectDir = vEyeDir.Reflect( vNormal );

   Vector2 uv;
   uv.y = vReflectDir.Dot( vYUnitVector );

   vReflectDir.y = 0.0f;

   uv.x = vReflectDir.Dot( vXUnitVector ) * 0.5f;

   if (vReflectDir.z >= 0.0f)
      uv = (uv + 1.0f) * 0.5f;
   else
   {
      uv.y = (uv.y + 1.0f) * 0.5f;
      uv.x = (-uv.x) * 0.5f + 1.0f;
      if (uv.x > 1.0f)
         uv.x -= 1.0f;
   }

   ASSERT( uv.x>=0.0f && uv.x<=1.0f );
   ASSERT( uv.y>=0.0f && uv.y<=1.0f );

   //TransformUV( uv );

   return uv;
}
