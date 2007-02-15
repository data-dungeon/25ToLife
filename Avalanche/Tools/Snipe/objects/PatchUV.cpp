#include "stdafx.h"
#include "PatchUV.h"
#include "UVPatchEdge.h"

IMPLEMENT_COMPONENT_CREATE( PatchUV, UV )

PatchUV::PatchUV()
{
}

PatchUV::PatchUV( UINT nID )
: UV( nID )
{
}

PatchUV::~PatchUV()
{
}

void PatchUV::GetTangents( UVPatch *pUVPatch, Vector2 &uv0, Vector2 &uv1 )
{
   UVPatchEdge *pPrevEdge, *pNextEdge;
   GetEdgePair( pUVPatch,(UVPatchEdge**)&pPrevEdge,(UVPatchEdge**)&pNextEdge);

   if (pPrevEdge->GetFirstUV() == this)
      uv0 = pPrevEdge->GetFirstUVTangentPos();
   else
      uv0 = pPrevEdge->GetSecondUVTangentPos();

   if (pNextEdge->GetFirstUV() == this)
      uv1 = pNextEdge->GetFirstUVTangentPos();
   else
      uv1 = pNextEdge->GetSecondUVTangentPos();
}

