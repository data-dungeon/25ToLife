#pragma once

#include "PropertyEdge.h"

class ColorVertexSet;
class ColorVertex;
class ColorPoly;

class ColorEdge : public PropertyEdge
{

public:
   DECLARE_COMPONENT_CREATE(ColorEdge, PropertyEdge)

   ColorEdge();
   ColorEdge( UINT nID );
   virtual ~ColorEdge();

   ColorEdge *GetNextColorEdge() { return (ColorEdge*)GetNextPropertyEdge(); }
   void SetNextColorEdge( ColorEdge *pNextColorEdge ) { SetNextPropertyEdge((PropertyEdge*)pNextColorEdge); }
   ColorEdge **GetNextColorEdgeUplink() { return (ColorEdge**)GetNextPropertyEdgeUplink(); }

   void SetFirstColorVertex(ColorVertex *pColorVertex) { return SetFirstPropertyVertex((PropertyVertex*)pColorVertex); }
   void SetSecondColorVertex(ColorVertex *pColorVertex) { return SetSecondPropertyVertex((PropertyVertex*)pColorVertex); }

   ColorVertex *GetFirstColorVertex() const { return (ColorVertex*)GetFirstPropertyVertex(); }
   ColorVertex *GetSecondColorVertex() const { return (ColorVertex*)GetSecondPropertyVertex(); }

   ColorVertex *GetSharedColorVertex( ColorEdge *pColorEdge ) const { return (ColorVertex*)GetSharedPropertyVertex((PropertyEdge*)pColorEdge); }
   ColorVertex *GetConnectedColorVertex( ColorVertex *pNotThisColorVertex ) { return (ColorVertex*)GetConnectedPropertyVertex((PropertyVertex*)pNotThisColorVertex); }
   BOOL IsInEdge(ColorVertex *pColorVertex) { return PropertyEdge::IsInEdge((PropertyVertex*)pColorVertex); }

   BOOL IsAligned( ColorPoly *pColorPoly ) const { return PropertyEdge::IsAligned((PropertyPoly *)pColorPoly); }
   ColorVertex *GetAlignedFirstColorVertex( ColorPoly *pColorPoly ) const { return (ColorVertex*)GetAlignedFirstPropertyVertex((PropertyPoly*)pColorPoly); }
   ColorVertex *GetAlignedSecondColorVertex( ColorPoly *pColorPoly ) const { return (ColorVertex*)GetAlignedSecondPropertyVertex((PropertyPoly*)pColorPoly); }

   ColorPoly *GetFirstColorPoly() { return (ColorPoly*)GetFirstPropertyPoly(); }
   void SetFirstColorPoly(ColorPoly *pColorPoly) { SetFirstPropertyPoly((PropertyPoly*)pColorPoly); }
   ColorPoly *GetSecondColorPoly() { return (ColorPoly*)GetSecondPropertyPoly(); }
   void SetSecondColorPoly(ColorPoly *pColorPoly) { SetFirstPropertyPoly((PropertyPoly*)pColorPoly); }
   ColorPoly *GetColorPoly() { return (ColorPoly*)GetPropertyPoly(); }
   ColorPoly *GetConnectedColorPoly(ColorPoly *pNotThis) { return (ColorPoly*)GetConnectedPropertyPoly((PropertyPoly*)pNotThis); }

   ColorVertexSet *GetColorVertexSet() { return (ColorVertexSet*)GetPropertySet(); }

   static ColorEdge *FindEdge( const ColorVertex *pColorVertex0, const ColorVertex *pColorVertex1 ) { return (ColorEdge*)PropertyEdge::FindEdge((PropertyVertex*)pColorVertex0,(PropertyVertex*)pColorVertex1); }

   BOOL AddColorPoly(ColorPoly *pColorPoly) { return AddPropertyPoly((PropertyPoly*)pColorPoly); }
   void RemoveColorPoly(ColorPoly *pColorPoly) { return RemovePropertyPoly((PropertyPoly*)pColorPoly); }
  
   Vector4 GetPosAlongEdge( float fPosition, ColorPoly *pPoly );

   void Draw( BOOL bDrawTess );
};
