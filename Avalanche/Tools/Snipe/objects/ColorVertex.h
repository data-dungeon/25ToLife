#pragma once

#include "PropertyVertex.h"

class ColorPoly;
class ColorPatch;
class ColorEdge;
class ColorVertexSet;

class ColorVertex : public PropertyVertex
{
private:
   RGBAFloat m_color;

public:
   DECLARE_COMPONENT_CREATE(ColorVertex, PropertyVertex)

            ColorVertex();
            ColorVertex( UINT nID );
   virtual ~ColorVertex();

   // Operations
   ColorVertexSet *GetColorVertexSet() { return (ColorVertexSet*)GetPropertySet(); }

   const RGBAFloat &GetColor() const;
   void SetColor( const RGBAFloat &color);

   Vector4 GetPos() { return Vector4(m_color.GetRed(),m_color.GetGreen(),m_color.GetBlue(),m_color.GetAlpha()); }
   void SetPos(const Vector4 &pos) { m_color.Set(pos.x,pos.y,pos.z,pos.w); }

   // Neighbors
   UINT GetNumSurroundingColorEdges() const  { return GetNumSurroundingPropertyEdges(); }
   void AddSurroundingColorEdge( ColorEdge *pColorEdge ) { AddSurroundingPropertyEdge((PropertyEdge*)pColorEdge); }
   ColorEdge *GetSurroundingColorEdgeAt( UINT nIndex ) const { return (ColorEdge*)GetSurroundingPropertyEdgeAt(nIndex); }
   void RemoveSurroundingColorEdgeAt( UINT nIndex ) { RemoveSurroundingPropertyEdgeAt(nIndex); }
   void RemoveSurroundingColorEdge(ColorEdge *pColorEdge) { RemoveSurroundingPropertyEdge((PropertyEdge*)pColorEdge); }

   UINT GetNumSurroundingColorPolys() const { return GetNumSurroundingPropertyPolys(); }
   void AddSurroundingColorPoly( ColorPoly *pColorPoly ) { AddSurroundingPropertyPoly((PropertyPoly*)pColorPoly); }
   ColorPoly *GetSurroundingColorPolyAt( UINT nIndex ) const { return (ColorPoly*)GetSurroundingPropertyPolyAt(nIndex); }
   void RemoveSurroundingColorPolyAt( const UINT nIndex ) { RemoveSurroundingPropertyPolyAt(nIndex); }
   void RemoveSurroundingColorPoly(ColorPoly *pColorPoly) { RemoveSurroundingPropertyPoly((PropertyPoly*)pColorPoly); }

   void GetEdgePair(ColorPoly *pColorPoly, ColorEdge **pEdge0, ColorEdge **pEdge1, UINT *nIndex0=NULL, UINT *nIndex1=NULL)
      { PropertyVertex::GetEdgePair((PropertyPoly*)pColorPoly, (PropertyEdge**)pEdge0, (PropertyEdge**)pEdge1, nIndex0, nIndex1 ); }

// Property Component Overrides
   virtual void SetPosFromPointAlongEdge( float fPosition, PropertyEdge *pEdge, PropertyPoly *pPoly );

// SnipeObject Overrides
//   virtual BOOL GetWorldPos( Vector &pos );
//   virtual void Transform( const Matrix44 &matrix );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
};


