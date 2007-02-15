#include "stdafx.h"
#include "HTessPoly.h"
#include "TessPoly.h"

UINT HTessPoly::GetNumSides() const
{
   return ((TessPoly *)this)->GetNumSides();
}

HPatchVertex *HTessPoly::GetVertexAt( UINT nIndex ) const
{
   return (HPatchVertex *)((TessPoly *)this)->GetVertexAt( nIndex );
}

void HTessPoly::SetVertexAt( UINT nIndex, HPatchVertex *pVertex )
{
   ((TessPoly *)this)->SetVertexAt(nIndex,(PatchVertex*)pVertex);
}

HVertexNormal *HTessPoly::GetVertexNormalAt( UINT nIndex ) const
{
   return (HVertexNormal *)((TessPoly *)this)->GetVertexNormalAt( nIndex );
}

Vector *HTessPoly::GetFaceNormal()
{
   return ((TessPoly*)this)->GetFaceNormal();
}

HTessPropertyPoly *HTessPoly::GetHeadTessPropertyPoly() const
{
   return (HTessPropertyPoly *) ((TessPoly *)this)->GetHeadTessPropertyPoly();
}


