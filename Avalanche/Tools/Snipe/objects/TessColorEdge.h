#pragma once

#include "TessEdge.h"

class ColorVertex;

class TessColorEdge : public TessPropertyEdge
{
public:
   TessColorEdge() {}
   ~TessColorEdge() {}

   UINT GetNumColorVertices() const { return GetNumPropertyVertices(); }
   void SetNumColorVertices( UINT nSize ) { SetNumPropertyVertices(nSize); }
   void SetColorVertexAt( UINT nIndex, ColorVertex *pColorVertex ) { SetPropertyVertexAt(nIndex,(PropertyVertex*)pColorVertex); }
   ColorVertex *GetColorVertexAt( UINT nIndex, BOOL bReversed ) const { return (ColorVertex*)GetPropertyVertexAt(nIndex,bReversed); }
};
