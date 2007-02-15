#include "stdafx.h"
#include "ColorEdge.h"
#include "ColorPoly.h"
#include "ColorVertex.h"

IMPLEMENT_COMPONENT_CREATE( ColorEdge, Edge )

ColorEdge::ColorEdge()
{

}

ColorEdge::ColorEdge( UINT nID )
: PropertyEdge( nID )
{
}

ColorEdge::~ColorEdge()
{
}

Vector4 ColorEdge::GetPosAlongEdge( float fPosition, ColorPoly *pColorPoly )
{
   ASSERT(fPosition >= 0.0 && fPosition <= 1.0f);

   ColorVertex *pFirstColorVertex = GetAlignedFirstColorVertex(pColorPoly);
   ColorVertex *pSecondColorVertex = GetAlignedSecondColorVertex(pColorPoly);

   return (pSecondColorVertex->GetPos() - pFirstColorVertex->GetPos()) * fPosition + pFirstColorVertex->GetPos();
}

void ColorEdge::Draw( BOOL bDrawTess )
{
/*
   glBegin( GL_LINES );
      glVertex2fv( &GetFirstColorVertex()->GetPos().x );
      glVertex2fv( &GetSecondColorVertex()->GetPos().x );
   glEnd();
*/
}
