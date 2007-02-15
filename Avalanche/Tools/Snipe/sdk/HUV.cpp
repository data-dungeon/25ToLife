#include "stdafx.h"
#include "HUV.h"
#include "UV.h"

const Vector2 &HUV::GetPos() const 
{ 
   return ((UV *)this)->GetPos();
}

void HUV::SetPos( const Vector2 &pos )
{
   ((UV *)this)->SetPos(pos);
}

UINT HUV::GetNumSurroundingUVPolys() const
{
   return ((UV *)this)->GetNumSurroundingUVPolys();
}

HUVPoly *HUV::GetSurroundingUVPolyAt( UINT nIndex ) const
{
   return (HUVPoly *)((UV *)this)->GetSurroundingUVPolyAt( nIndex );
}
