#pragma once

#include "PropertyPatchEdge.h"
#include "UV.h"

class PropertyPatch;
class UVPatch;
class PatchUV;

class UVHandle : public PropertyVertexHandle
{
public:
   Vector2		m_tangent;

public:
   UVHandle() {}
   UVHandle(const UVHandle &other) { m_tangent = other.m_tangent; }
   virtual ~UVHandle() {}

   const Vector2 &GetTangent() const { return m_tangent; }
   void           SetTangent(const Vector2 &tangent) { m_tangent = tangent; }
};

/////////////////////////////////////////////////////////////////

class UVPatchEdge : public PropertyPatchEdge
{
public:
   DECLARE_COMPONENT_CREATE(UVPatchEdge, PropertyPatchEdge)

            UVPatchEdge();
            UVPatchEdge( UINT nID );
   virtual ~UVPatchEdge();

   // UVEdge

   UVEdge *GetNextUVEdge() { return (UVEdge*)GetNextPropertyEdge(); }
   void SetNextUVEdge( UVEdge *pNextUVEdge ) { SetNextPropertyEdge((PropertyEdge*)pNextUVEdge); }
   UVEdge **GetNextUVEdgeUplink() { return (UVEdge**)GetNextPropertyEdgeUplink(); }

   void SetFirstUV(PatchUV *pUV) { return SetFirstPropertyVertex((PropertyVertex*)pUV); }
   void SetSecondUV(PatchUV *pUV) { return SetSecondPropertyVertex((PropertyVertex*)pUV); }

   PatchUV *GetFirstUV() const { return (PatchUV*)GetFirstPropertyVertex(); }
   PatchUV *GetSecondUV() const { return (PatchUV*)GetSecondPropertyVertex(); }

   PatchUV *GetSharedUV( UVEdge *pUVEdge ) const { return (PatchUV*)GetSharedPropertyVertex((PropertyEdge*)pUVEdge); }
   PatchUV *GetConnectedUV( UV *pNotThisUV ) { return (PatchUV*)GetConnectedPropertyVertex((PropertyVertex*)pNotThisUV); }
   BOOL IsInEdge(PatchUV *pUV) { return PropertyEdge::IsInEdge((PropertyVertex*)pUV); }

   BOOL IsAligned( UVPoly *pUVPoly ) const { return PropertyEdge::IsAligned((PropertyPoly *)pUVPoly); }
   PatchUV *GetAlignedFirstUV( UVPoly *pUVPoly ) const { return (PatchUV*)GetAlignedFirstPropertyVertex((PropertyPoly*)pUVPoly); }
   PatchUV *GetAlignedSecondUV( UVPoly *pUVPoly ) const { return (PatchUV*)GetAlignedSecondPropertyVertex((PropertyPoly*)pUVPoly); }

   UVPoly *GetFirstUVPoly() { return (UVPoly*)GetFirstPropertyPoly(); }
   void SetFirstUVPoly(UVPoly *pUVPoly) { SetFirstPropertyPoly((PropertyPoly*)pUVPoly); }
   UVPoly *GetSecondUVPoly() { return (UVPoly*)GetSecondPropertyPoly(); }
   void SetSecondUVPoly(UVPoly *pUVPoly) { SetFirstPropertyPoly((PropertyPoly*)pUVPoly); }
   UVPoly *GetUVPoly() { return (UVPoly*)GetPropertyPoly(); }
   UVPoly *GetConnectedUVPoly(UVPoly *pNotThis) { return (UVPoly*)GetConnectedPropertyPoly((PropertyPoly*)pNotThis); }

   TexCoords *GetTexCoords() { return (TexCoords*)GetPropertySet(); }

   static UVEdge *FindEdge( const PatchUV *pUV0, const PatchUV *pUV1 ) { return (UVEdge*)PropertyEdge::FindEdge((PropertyVertex*)pUV0,(PropertyVertex*)pUV1); }

   // UVPatchEdge

   UINT GetNumTessUVs() { return GetNumTessPropertyVertices(); }
   void SetNumTessUVs( UINT nNumUVs ) { SetNumTessPropertyVertices(nNumUVs); }
   void SetTessUVAt( UINT nIndex, UV *pUV ) { SetTessPropertyVertexAt(nIndex,(PropertyVertex*)pUV); }
   UV  *GetTessUVAt( UINT nIndex, BOOL bReversed=FALSE ) { return (UV*)GetTessPropertyVertexAt(nIndex,bReversed); }

   TessEdgeUVs *GetTessEdgeUVs() { return (TessEdgeUVs*)GetTessPropertyEdge(); }

   UVHandle *GetFirstUVHandle() { return (UVHandle*)GetFirstPropertyVertexHandle(); }
   UVHandle *GetSecondUVHandle() { return (UVHandle*)GetSecondPropertyVertexHandle(); }

   UVHandle *GetAlignedFirstUVHandle( UVPoly *pUVPoly ) { return (UVHandle*)GetAlignedFirstPropertyVertexHandle((PropertyPoly*)pUVPoly); }
   UVHandle *GetAlignedSecondUVHandle( UVPoly *pUVPoly ) { return (UVHandle*)GetAlignedFirstPropertyVertexHandle((PropertyPoly*)pUVPoly); }

   UVHandle *GetUVHandle( UV *pUV ) { return (UVHandle*)GetPropertyVertexHandle(pUV); }
   
   Vector2 GetFirstUVTangentPos();
   Vector2 GetSecondUVTangentPos();

   Vector2 GetAlignedFirstUVTangentPos( UVPoly *pUVPoly );
   Vector2 GetAlignedSecondUVTangentPos( UVPoly *pUVPoly );

   UVPatch *GetUVPatch() { return (UVPatch *)GetPropertyPatch(); }
   UVPatch *GetConnectedUVPatch( UVPatch *pNotThis ) { return (UVPatch *)GetConnectedPropertyPatch( (PropertyPatch *)pNotThis ); }

   UVPatchEdge *GetPrevContUVEdge(BOOL bLoop) { return (UVPatchEdge*)GetPrevContPropertyEdge(bLoop); }
   UVPatchEdge *GetNextContUVEdge(BOOL bLoop) { return (UVPatchEdge*)GetNextContPropertyEdge(bLoop); }
   PatchUV *GetPrevContUV() { return (PatchUV*)GetPrevContPropertyVertex(); }
   PatchUV *GetNextContUV() { return (PatchUV*)GetNextContPropertyVertex(); }

   BOOL AddUVPoly(UVPoly *pUVPoly) { return AddPropertyPoly((PropertyPoly*)pUVPoly); }
   void RemoveUVPoly(UVPoly *pUVPoly) { return RemovePropertyPoly((PropertyPoly*)pUVPoly); }

   Vector2 GetPosAlongEdge( float fPosition, UVPoly *pPoly );

// PatchPropertyEdge Overrides
   virtual void BuildTangents();
   virtual void SetTessellation();  
   virtual void Draw( BOOL bDrawTess );
};

#include "UVPatchEdge.hpp"