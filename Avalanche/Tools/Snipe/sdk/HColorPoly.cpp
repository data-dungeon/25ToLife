#include "stdafx.h"
#include "HColorPoly.h"
#include "ColorPoly.h"

HColorVertex *HColorPoly::GetColorVertexAt(UINT nIndex)
{
   return (HColorVertex*)((ColorPoly*)this)->GetColorVertexAt(nIndex);
}

void HColorPoly::SetColorVertexAt(UINT nIndex,HColorVertex *pColorVertex)
{
   ((ColorPoly*)this)->SetColorVertexAt(nIndex,(ColorVertex*)pColorVertex);
}

