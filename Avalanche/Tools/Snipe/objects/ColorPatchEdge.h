#pragma once

#include "PropertyPatchEdge.h"
#include "ColorVertex.h"

class PropertyPatch;
class ColorPatch;
class ColorPatchVertex;
class TessColorEdge;

class ColorVertexHandle : public PropertyVertexHandle
{
public:
   Vector4		m_tangent;

public:
   ColorVertexHandle() {}
   ColorVertexHandle(const ColorVertexHandle &other) { m_tangent = other.m_tangent; }

   const Vector4   &GetTangent() const { return m_tangent; }
   void           SetTangent(const Vector4 &tangent) { m_tangent = tangent; }
};

/////////////////////////////////////////////////////////////////

class ColorPatchEdge : public PropertyPatchEdge
{
public:
   DECLARE_COMPONENT_CREATE(ColorPatchEdge, PropertyPatchEdge)

   ColorPatchEdge();
   ColorPatchEdge( UINT nID );
   virtual ~ColorPatchEdge();

   // ColorEdge

   ColorEdge *GetNextColorEdge() { return (ColorEdge*)GetNextPropertyEdge(); }
   void SetNextColorEdge( ColorEdge *pNextColorEdge ) { SetNextPropertyEdge((PropertyEdge*)pNextColorEdge); }
   ColorEdge **GetNextColorEdgeUplink() { return (ColorEdge**)GetNextPropertyEdgeUplink(); }

   void SetFirstColorVertex(ColorPatchVertex *pColorVertex) { return SetFirstPropertyVertex((PropertyVertex*)pColorVertex); }
   void SetSecondColorVertex(ColorPatchVertex *pColorVertex) { return SetSecondPropertyVertex((PropertyVertex*)pColorVertex); }

   ColorPatchVertex *GetFirstColorVertex() const { return (ColorPatchVertex*)GetFirstPropertyVertex(); }
   ColorPatchVertex *GetSecondColorVertex() const { return (ColorPatchVertex*)GetSecondPropertyVertex(); }

   ColorPatchVertex *GetSharedColorVertex( ColorEdge *pColorEdge ) const { return (ColorPatchVertex*)GetSharedPropertyVertex((PropertyEdge*)pColorEdge); }
   ColorPatchVertex *GetConnectedColorVertex( ColorVertex *pNotThisColorVertex ) { return (ColorPatchVertex*)GetConnectedPropertyVertex((PropertyVertex*)pNotThisColorVertex); }
   BOOL IsInEdge(ColorPatchVertex *pColorVertex) { return PropertyEdge::IsInEdge((PropertyVertex*)pColorVertex); }

   BOOL IsAligned( ColorPoly *pColorPoly ) const { return PropertyEdge::IsAligned((PropertyPoly *)pColorPoly); }
   ColorPatchVertex *GetAlignedFirstColorVertex( ColorPoly *pColorPoly ) const { return (ColorPatchVertex*)GetAlignedFirstPropertyVertex((PropertyPoly*)pColorPoly); }
   ColorPatchVertex *GetAlignedSecondColorVertex( ColorPoly *pColorPoly ) const { return (ColorPatchVertex*)GetAlignedSecondPropertyVertex((PropertyPoly*)pColorPoly); }

   ColorPoly *GetFirstColorPoly() { return (ColorPoly*)GetFirstPropertyPoly(); }
   void SetFirstColorPoly(ColorPoly *pColorPoly) { SetFirstPropertyPoly((PropertyPoly*)pColorPoly); }
   ColorPoly *GetSecondColorPoly() { return (ColorPoly*)GetSecondPropertyPoly(); }
   void SetSecondColorPoly(ColorPoly *pColorPoly) { SetFirstPropertyPoly((PropertyPoly*)pColorPoly); }
   ColorPoly *GetColorPoly() { return (ColorPoly*)GetPropertyPoly(); }
   ColorPoly *GetConnectedColorPoly(ColorPoly *pNotThis) { return (ColorPoly*)GetConnectedPropertyPoly((PropertyPoly*)pNotThis); }

   ColorVertexSet *GetColorVertexSet() { return (ColorVertexSet*)GetPropertySet(); }

   static ColorEdge *FindEdge( const ColorPatchVertex *pColorVertex0, const ColorPatchVertex *pColorVertex1 ) { return (ColorEdge*)PropertyEdge::FindEdge((PropertyVertex*)pColorVertex0,(PropertyVertex*)pColorVertex1); }

   // ColorPatchEdge

   UINT GetNumTessColorVertexs() { return GetNumTessPropertyVertices(); }
   void SetNumTessColorVertexs( UINT nNumColorVertexs ) { SetNumTessPropertyVertices(nNumColorVertexs); }
   void SetTessColorVertexAt( UINT nIndex, ColorVertex *pColorVertex ) { SetTessPropertyVertexAt(nIndex,(PropertyVertex*)pColorVertex); }
   ColorVertex  *GetTessColorVertexAt( UINT nIndex, BOOL bReversed=FALSE ) { return (ColorVertex*)GetTessPropertyVertexAt(nIndex,bReversed); }

   TessColorEdge *GetTessColorEdge() { return (TessColorEdge*)GetTessPropertyEdge(); }

   ColorVertexHandle *GetFirstColorVertexHandle() { return (ColorVertexHandle*)GetFirstPropertyVertexHandle(); }
   ColorVertexHandle *GetSecondColorVertexHandle() { return (ColorVertexHandle*)GetSecondPropertyVertexHandle(); }

   ColorVertexHandle *GetAlignedFirstColorVertexHandle( ColorPoly *pColorPoly ) { return (ColorVertexHandle*)GetAlignedFirstPropertyVertexHandle((PropertyPoly*)pColorPoly); }
   ColorVertexHandle *GetAlignedSecondColorVertexHandle( ColorPoly *pColorPoly ) { return (ColorVertexHandle*)GetAlignedFirstPropertyVertexHandle((PropertyPoly*)pColorPoly); }

   Vector4 GetFirstColorVertexTangentPos();
   Vector4 GetSecondColorVertexTangentPos();

   Vector4 GetAlignedFirstColorVertexTangentPos( ColorPoly *pColorPoly );
   Vector4 GetAlignedSecondColorVertexTangentPos( ColorPoly *pColorPoly );

   ColorPatch *GetColorVertexPatch() { return (ColorPatch *)GetPropertyPatch(); }
   ColorPatch *GetConnectedColorVertexPatch( ColorPatch *pNotThis ) { return (ColorPatch *)GetConnectedPropertyPatch( (PropertyPatch *)pNotThis ); }

   ColorPatchEdge *GetPrevContColorEdge(BOOL bLoop) { return (ColorPatchEdge*)GetPrevContPropertyEdge(bLoop); }
   ColorPatchEdge *GetNextContColorEdge(BOOL bLoop) { return (ColorPatchEdge*)GetNextContPropertyEdge(bLoop); }
   ColorPatchVertex *GetPrevContColorVertex() { return (ColorPatchVertex*)GetPrevContPropertyVertex(); }
   ColorPatchVertex *GetNextContColorVertex() { return (ColorPatchVertex*)GetNextContPropertyVertex(); }

   BOOL AddColorPoly(ColorPoly *pColorPoly) { return AddPropertyPoly((PropertyPoly*)pColorPoly); }
   void RemoveColorPoly(ColorPoly *pColorPoly) { return RemovePropertyPoly((PropertyPoly*)pColorPoly); }

   Vector4 GetPosAlongEdge( float fPosition, ColorPoly *pPoly );

// PatchPropertyEdge Overridables
   virtual void BuildTangents();
   virtual void SetTessellation();
};

#include "ColorPatchEdge.hpp"