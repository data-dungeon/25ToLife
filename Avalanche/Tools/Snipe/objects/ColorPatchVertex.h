#pragma once

#include "ColorVertex.h"

class ColorPatch;
class ColorPatchEdge;

class ColorPatchVertex : public ColorVertex
{
public:
   DECLARE_COMPONENT_CREATE(ColorPatchVertex, ColorVertex)

            ColorPatchVertex();
            ColorPatchVertex( UINT nID );
   virtual ~ColorPatchVertex();

   void GetTangents( ColorPatch *pColorPatch, Vector4 &colorVertex0, Vector4 &colorVertex1 );

   void GetEdgePair(ColorPatch *pColorPatch, ColorPatchEdge **pEdge0, ColorPatchEdge **pEdge1, UINT *nIndex0=NULL, UINT *nIndex1=NULL)
      { PropertyVertex::GetEdgePair((PropertyPoly*)pColorPatch, (PropertyEdge**)pEdge0, (PropertyEdge**)pEdge1, nIndex0, nIndex1 ); }

};


