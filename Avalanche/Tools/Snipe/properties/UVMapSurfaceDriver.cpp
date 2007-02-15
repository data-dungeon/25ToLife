#include "stdafx.h"
#include "UVMapSurfaceDriver.h"
#include "Surface.h"
#include "UVPoly.h"
#include "GUI/GLTexCoordView.h"
#include "resource.h."
#include "Poly.h"
#include "TessPoly.h"

/////////////////////////////////////////////////////
// UVMapSurfaceDriver

IMPLEMENT_SNIPEOBJECT_CREATE( UVMapSurfaceDriver )

TexCoordsPointerProperty::Info UVMapSurfaceDriver::m_pTexCoordsInfo( "Tex Coords Set", "TexCoordsSet" );

UVMapSurfaceDriver::UVMapSurfaceDriver()
{
   m_strName = "UV Map";
   m_pTexCoords.Init( &m_pTexCoordsInfo, this );
}

BOOL UVMapSurfaceDriver::IsValid( Poly *pPoly, LPARAM &lParam )
{
   UVPoly *pUVPoly = pPoly->FindUVPoly( m_pTexCoords );
   lParam = (LPARAM)pUVPoly;

   return lParam;
}

BOOL UVMapSurfaceDriver::IsValid( TessPoly *pTessPoly, LPARAM &lParam )
{
   TessUVPoly *pTessUVPoly = pTessPoly->FindTessUVPoly( m_pTexCoords );
   lParam = (LPARAM)pTessUVPoly;

   return lParam;
}

Vector2 UVMapSurfaceDriver::GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam )
{
   UVPoly *pUVPoly = (UVPoly *)lParam;
   ASSERT(pUVPoly);

   UV *pUV = pUVPoly->GetUVAt( nIndex );

   Vector2 uv( pUV->GetPos() );
   //TransformUV( uv );

   return uv;
}

Vector2 UVMapSurfaceDriver::GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pTessPoly, LPARAM lParam )
{ 
   TessUVPoly *pTessUVPoly = (TessUVPoly *)lParam;
   ASSERT(pTessUVPoly);

   UV *pUV = pTessUVPoly->GetUVAt( nIndex );
   if (!pUV)
      return 0.0f;

   Vector2 uv( pUV->GetPos() );
   //TransformUV( uv );

   return uv;
}

float UVMapSurfaceDriver::DrawUVExtras( CGLTexCoordView *pView )
{
   float fAspect = 1.0f;

   if (m_glImageListPivot.IsEmpty())
      m_glImageListPivot.Create( IDB_TARGET );

   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();

   glTranslatef( m_pImage.m_v2Pivot.GetValue().x, m_pImage.m_v2Pivot.GetValue().y, 0.0f );
   float fMatrix[16];
   m_pImage.m_transform.GetMatrix().GetFloatMatrix( fMatrix, TRUE );
   glMultMatrixf( fMatrix );
   glTranslatef( -m_pImage.m_v2Pivot.GetValue().x, -m_pImage.m_v2Pivot.GetValue().y, 0.0f );

   Image *pImage = GetImage();
   if (pImage)
   {
      Vector2 vSize( pImage->GetWidth(), pImage->GetHeight());
      fAspect = vSize.x / vSize.y;
      if (!pView->m_bDoPick)
      {
         glScalef( fAspect, -1.0f, 1.0f );
         glTranslatef( 0.5f, -0.5f, 0.0f );
         pImage->Draw( (CGLBaseView *)pView );
      }
   }

   glPopMatrix();

   if (!pView->m_bDoPick) // draw pivot
   {
      GLDrawPrepper prepper( pView ); // push 2d projection
      int nWidth = m_glImageListPivot.GetWidthEach();
      int nHeight = m_glImageListPivot.GetHeight();
      Vector vPivot = m_pImage.m_v2Pivot;
      vPivot.z = 0.0f;
      pView->WorldToScreen( vPivot );
      m_glImageListPivot.Draw( vPivot.x-(nWidth/2), vPivot.y-(nHeight/2), 0, .5f );
   }

   return fAspect;
}
