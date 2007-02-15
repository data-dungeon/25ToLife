#include "stdafx.h"
#include "UVEdge.h"
#include "UVPoly.h"
#include "UV.h"

IMPLEMENT_COMPONENT_CREATE( UVEdge, Edge )

UVEdge::UVEdge()
{

}

UVEdge::UVEdge( UINT nID )
: PropertyEdge( nID )
{
}

UVEdge::~UVEdge()
{
}

void UVEdge::GetVector(Vector2 &dir)
{
   dir = GetSecondUV()->GetPos() - GetFirstUV()->GetPos();
}

Vector2 UVEdge::GetPosAlongEdge( float fPosition, UVPoly *pUVPoly )
{
   ASSERT(fPosition >= 0.0 && fPosition <= 1.0f);

   UV *pFirstUV = GetAlignedFirstUV(pUVPoly);
   UV *pSecondUV = GetAlignedSecondUV(pUVPoly);

   return (pSecondUV->GetPos() - pFirstUV->GetPos()) * fPosition + pFirstUV->GetPos();
}

void UVEdge::Draw( BOOL bDrawTess )
{
   glBegin( GL_LINES );
      glVertex2fv( &GetFirstUV()->GetPos().x );
      glVertex2fv( &GetSecondUV()->GetPos().x );
   glEnd();
}
