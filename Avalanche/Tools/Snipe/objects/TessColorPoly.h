#pragma once

#include "TessPoly.h"

class ColorVertex;
class ColorPatch;

class TessColorPoly : public TessPropertyPoly
{
public:
   TessColorPoly() {}

   TessColorPoly *GetNextTessColorPoly() const { return (TessColorPoly*)GetNextTessPropertyPoly(); }
   TessColorPoly **GetNextTessColorPolyUplink() { return (TessColorPoly**)GetNextTessPropertyPolyUplink(); }
   void SetNextTessColorPoly( TessColorPoly *pTessColorPoly ) { SetNextTessPropertyPoly((TessPropertyPoly*)pTessColorPoly); }

   ColorVertex *GetColorAt(UINT nIndex) const { return (ColorVertex*)GetPropertyVertexAt(nIndex); }
   void SetColorAt(UINT nIndex, ColorVertex *pColorVertex) { SetPropertyVertexAt(nIndex,(PropertyVertex*)pColorVertex); }

   ColorPatch *GetColorPatch() const { return (ColorPatch*)GetPropertyPatch(); }
};
