#pragma once

#include "PropertyPoly.h"

class UV;
class UVEdge;
class TexCoords;

class UVPoly : public PropertyPoly
{
public:
   DECLARE_COMPONENT_CREATE( UVPoly, PropertyPoly )

            UVPoly() {}
            UVPoly( UINT nID ) {}
   virtual ~UVPoly() {}

   UVPoly *GetNextUVPoly() const { return (UVPoly*)GetNextPropertyPoly(); }
   UVPoly **GetNextUVPolyUplink() { return (UVPoly**)GetNextPropertyPolyUplink(); }

   void SetNextUVPoly( UVPoly *pUVPoly ) { SetNextPropertyPoly((PropertyPoly*)pUVPoly); }

   // uvs
   UV *GetUVAt( UINT nIndex ) const { return (UV*)GetPropertyVertexAt(nIndex); }
   void SetUVAt( UINT nIndex, UV *pUV ) { SetPropertyVertexAt(nIndex,(PropertyVertex*)pUV); }
   int FindUVFaceRelativeIndex( const UV *pFind ) const { return FindPropertyVertexFaceRelativeIndex((PropertyVertex*)pFind); }

   // edges
   void SetUVEdgeAt( UINT nIndex, UVEdge *pUVEdge ) { SetPropertyEdgeAt(nIndex,(PropertyEdge*)pUVEdge); }
   UVEdge *GetUVEdgeAt( UINT nIndex ) const { return (UVEdge*)GetPropertyEdgeAt(nIndex); }
   int FindUVFaceRelativeIndex( const UVEdge *pFind ) const { return FindPropertyVertexFaceRelativeIndex((PropertyEdge*)pFind); }

   TexCoords *GetTexCoords() { return (TexCoords*)GetPropertySet(); }
 
   UVPoly *FindConnectedUVPoly(UV *pUV0, UV *pUV1) { return (UVPoly*)FindConnectedPropertyPoly((PropertyVertex*)pUV0,(PropertyVertex*)pUV1); }

// SnipeObject Overrides
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
};
