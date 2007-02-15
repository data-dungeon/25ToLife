#include "stdafx.h"
#include "HUVPoly.h"
#include "UVPoly.h"

UINT HUVPoly::GetNumSides()
{
   return ((UVPoly*)this)->GetNumSides();
}

HUV *HUVPoly::GetUVAt(UINT nIndex)
{
   return (HUV*)((UVPoly*)this)->GetUVAt(nIndex);
}

void HUVPoly::SetUVAt(UINT nIndex,HUV *pUV)
{
   ((UVPoly*)this)->SetUVAt(nIndex,(UV*)pUV);
}

int HUVPoly::FindUVFaceRelativeIndex( HUV *pUV )
{
   return ((UVPoly *)this)->FindUVFaceRelativeIndex( (UV *)pUV );
}

HTexCoords *HUVPoly::GetTexCoords()
{
   return (HTexCoords*)((UVPoly*)this)->GetTexCoords();
}