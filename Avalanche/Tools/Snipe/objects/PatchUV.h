#pragma once

#include "UV.h"

class UVPatch;
class UVPatchEdge;

class PatchUV : public UV
{
public:
   DECLARE_COMPONENT_CREATE(PatchUV, UV)

            PatchUV();
            PatchUV( UINT nID );
   virtual ~PatchUV();

   void GetTangents( UVPatch *pUVPatch, Vector2 &uv0, Vector2 &uv1 );

   void GetEdgePair(UVPatch *pUVPatch, UVPatchEdge **pEdge0, UVPatchEdge **pEdge1, UINT *nIndex0=NULL, UINT *nIndex1=NULL)
      { PropertyVertex::GetEdgePair((PropertyPoly*)pUVPatch, (PropertyEdge**)pEdge0, (PropertyEdge**)pEdge1, nIndex0, nIndex1 ); }

};


