#pragma once

#include "PropertyPatch.h"

class ColorVertex;
class ColorPatchEdge;
class TessColorPoly;

class ColorPatch : public PropertyPatch
{
public:
   DECLARE_COMPONENT_CREATE( ColorPatch, PropertyPatch )

            ColorPatch() {}
            ColorPatch( UINT nID ) {}
   virtual ~ColorPatch() {}

   // ColorVertexPoly

   // ColorVertexs
   ColorVertex *GetColorVertexAt( UINT nIndex ) const { return (ColorVertex*)GetPropertyVertexAt(nIndex); }
   void SetColorVertexAt( UINT nIndex, ColorVertex *pColorVertex ) { SetPropertyVertexAt(nIndex,(PropertyVertex*)pColorVertex); }
   int FindColorVertexFaceRelativeIndex( const ColorVertex *pFind ) const { return FindPropertyVertexFaceRelativeIndex((PropertyVertex*)pFind); }

   // ColorPatch

   ColorPatch *GetNextColorPatch() const { return (ColorPatch *)GetNextPropertyPatch(); }

   ColorPatchEdge *GetColorPatchEdgeAt( UINT nIndex ) { return (ColorPatchEdge *)GetPropertyPatchEdgeAt( nIndex ); }

   void SetCVs( Vector4 *pCVs, UINT nIndex ) { SetVector4CVs(pCVs,nIndex); }
   Vector4 *GetCVs( UINT nIndex ) { return GetVector4CVs(nIndex); }
   Vector4 CalculateColor( float fS, float fT, UINT nIndex ) { return CalculateVector4(fS,fT,nIndex); }

   int GetNumTessColorVertexPolys() { return GetNumTessPropertyPolys(); }
   TessColorPoly *GetTessColorVertexPoly( UINT nIndex ) { return (TessColorPoly*)GetTessPropertyPolyAt(nIndex); }

   Vector4 CalculateNPatchCenter();
 
   // PropertyPatch overrides
   virtual void SetTessellation( int nUSubdivisions, int nVSubdivisions );
   virtual void SetNumBasisMatrices(UINT nCount) { PropertyPatch::SetNumBasisMatrices(nCount,VECTOR4BEZIERBASIS); }
   virtual void BuildQuadPatchTangents();
   virtual void BuildNPatchTangents();
   virtual void SetTessellationInternalEdge( int nSub, UINT nSubPatchIndex );

};
