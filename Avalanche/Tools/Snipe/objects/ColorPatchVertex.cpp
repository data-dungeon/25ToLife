#include "stdafx.h"
#include "ColorPatchVertex.h"
#include "ColorPatchEdge.h"

IMPLEMENT_COMPONENT_CREATE( ColorPatchVertex, ColorVertex )

ColorPatchVertex::ColorPatchVertex()
{
}

ColorPatchVertex::ColorPatchVertex( UINT nID )
: ColorVertex( nID )
{
}

ColorPatchVertex::~ColorPatchVertex()
{
}

void ColorPatchVertex::GetTangents( ColorPatch *pColorPatch, Vector4 &colorVertex0, Vector4 &colorVertex1 )
{
   ColorPatchEdge *pPrevEdge, *pNextEdge;
   GetEdgePair( pColorPatch,(ColorPatchEdge**)&pPrevEdge,(ColorPatchEdge**)&pNextEdge);

   if (pPrevEdge->GetFirstColorVertex() == this)
      colorVertex0 = pPrevEdge->GetFirstColorVertexTangentPos();
   else
      colorVertex0 = pPrevEdge->GetSecondColorVertexTangentPos();

   if (pNextEdge->GetFirstColorVertex() == this)
      colorVertex1 = pNextEdge->GetFirstColorVertexTangentPos();
   else
      colorVertex1 = pNextEdge->GetSecondColorVertexTangentPos();
}

