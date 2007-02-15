#pragma once

#include "PropertyPoly.h"

class ColorVertex;
class ColorEdge;
class TexCoords;

class ColorPoly : public PropertyPoly
{
public:
   DECLARE_COMPONENT_CREATE( ColorPoly, PropertyPoly )

            ColorPoly() {}
            ColorPoly( UINT nID ) {}
   virtual ~ColorPoly() {}

   ColorPoly *GetNextColorPoly() const { return (ColorPoly*)GetNextPropertyPoly(); }
   ColorPoly **GetNextColorPolyUplink() { return (ColorPoly**)GetNextPropertyPolyUplink(); }

   void SetNextColorPoly( ColorPoly *pColorPoly ) { SetNextPropertyPoly((PropertyPoly*)pColorPoly); }

   // uvs
   ColorVertex *GetColorVertexAt( UINT nIndex ) const { return (ColorVertex*)GetPropertyVertexAt(nIndex); }
   void SetColorVertexAt( UINT nIndex, ColorVertex *pColorVertex ) { SetPropertyVertexAt(nIndex,(PropertyVertex*)pColorVertex); }
   int FindColorFaceRelativeIndex( const ColorVertex *pFind ) const { return FindPropertyVertexFaceRelativeIndex((PropertyVertex*)pFind); }

   // edges
   void SetColorEdgeAt( UINT nIndex, ColorEdge *pColorEdge ) { SetPropertyEdgeAt(nIndex,(PropertyEdge*)pColorEdge); }
   ColorEdge *GetColorEdgeAt( UINT nIndex ) const { return (ColorEdge*)GetPropertyEdgeAt(nIndex); }
   int FindUVFaceRelativeIndex( const ColorEdge *pFind ) const { return FindPropertyVertexFaceRelativeIndex((PropertyEdge*)pFind); }

   TexCoords *GetTexCoords() { return (TexCoords*)GetPropertySet(); }
 
   ColorPoly *FindConnectedColorPoly(ColorVertex *pColorVertex0, ColorVertex *pColorVertex1) { return (ColorPoly*)FindConnectedPropertyPoly((PropertyVertex*)pColorVertex0,(PropertyVertex*)pColorVertex1); }

// SnipeObject Overrides
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
};