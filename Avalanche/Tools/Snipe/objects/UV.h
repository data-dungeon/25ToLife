#pragma once

#include "PropertyVertex.h"

class TexCoords;
class UVPoly;
class UVPatch;
class UVEdge;

class UV : public PropertyVertex
{
private:
   Vector2 m_pos;

public:
   DECLARE_COMPONENT_CREATE(UV, PropertyVertex)

            UV();
            UV( UINT nID );
   virtual ~UV();

   // Operations
   TexCoords *GetTexCoords() { return (TexCoords*)GetPropertySet(); }

   const Vector2 &GetPos() const;
   void SetPos( const Vector2 &pos);

   // Neighbors
   UINT GetNumSurroundingUVEdges() const  { return GetNumSurroundingPropertyEdges(); }
   void AddSurroundingUVEdge( UVEdge *pUVEdge ) { AddSurroundingPropertyEdge((PropertyEdge*)pUVEdge); }
   UVEdge *GetSurroundingUVEdgeAt( UINT nIndex ) const { return (UVEdge*)GetSurroundingPropertyEdgeAt(nIndex); }
   void RemoveSurroundingUVEdgeAt( UINT nIndex ) { RemoveSurroundingPropertyEdgeAt(nIndex); }
   void RemoveSurroundingUVEdge(UVEdge *pUVEdge) { RemoveSurroundingPropertyEdge((PropertyEdge*)pUVEdge); }

   UINT GetNumSurroundingUVPolys() const { return GetNumSurroundingPropertyPolys(); }
   void AddSurroundingUVPoly( UVPoly *pUVPoly ) { AddSurroundingPropertyPoly((PropertyPoly*)pUVPoly); }
   UVPoly *GetSurroundingUVPolyAt( UINT nIndex ) const { return (UVPoly*)GetSurroundingPropertyPolyAt(nIndex); }
   void RemoveSurroundingUVPolyAt( const UINT nIndex ) { RemoveSurroundingPropertyPolyAt(nIndex); }
   void RemoveSurroundingUVPoly(UVPoly *pUVPoly) { RemoveSurroundingPropertyPoly((PropertyPoly*)pUVPoly); }

   void GetEdgePair(UVPoly *pUVPoly, UVEdge **pEdge0, UVEdge **pEdge1, UINT *nIndex0=NULL, UINT *nIndex1=NULL)
      { PropertyVertex::GetEdgePair((PropertyPoly*)pUVPoly, (PropertyEdge**)pEdge0, (PropertyEdge**)pEdge1, nIndex0, nIndex1 ); }

// Property Component Overrides
   virtual void SetPosFromPointAlongEdge( float fPosition, PropertyEdge *pEdge, PropertyPoly *pPoly );

// SnipeObject Overrides
   virtual BOOL GetWorldPos( Vector &pos );
   virtual void Transform( const Matrix44 &matrix );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
   virtual void Draw( CGLBaseView *pView );
};


