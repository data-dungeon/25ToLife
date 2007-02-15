#pragma once

#include "PropertyPatch.h"

class UVPatchEdge;

class UVPatch : public PropertyPatch
{
public:
   DECLARE_COMPONENT_CREATE( UVPatch, PropertyPatch )

            UVPatch() {}
            UVPatch( UINT nID ) {}
   virtual ~UVPatch() {}

   // UVPoly

   // uvs
   UV *GetUVAt( UINT nIndex ) const { return (UV*)GetPropertyVertexAt(nIndex); }
   void SetUVAt( UINT nIndex, UV *pUV ) { SetPropertyVertexAt(nIndex,(PropertyVertex*)pUV); }
   int FindUVFaceRelativeIndex( const UV *pFind ) const { return FindPropertyVertexFaceRelativeIndex((PropertyVertex*)pFind); }

   // UVPatch

   UVPatch *GetNextUVPatch() const { return (UVPatch *)GetNextPropertyPatch(); }

   UVPatchEdge *GetUVPatchEdgeAt( UINT nIndex ) { return (UVPatchEdge *)GetPropertyPatchEdgeAt( nIndex ); }

   void SetUVCVs( Vector2 *pUVCVs, UINT nIndex ) { SetVector2CVs(pUVCVs,nIndex); }
   Vector2 *GetUVCVs( UINT nIndex ) { return GetVector2CVs(nIndex); }
   Vector2 CalculateUV( float fS, float fT, UINT nIndex ) { return CalculateVector2(fS,fT,nIndex); }

   int GetNumTessUVPolys() { return GetNumTessPropertyPolys(); }
   TessUVPoly *GetTessUVPoly( UINT nIndex ) { return (TessUVPoly*)GetTessPropertyPolyAt(nIndex); }

   Vector2 CalculateNPatchCenter();
 
   // PropertyPatch overrides
   virtual void SetTessellation( int nUSubdivisions, int nVSubdivisions );
   virtual void SetNumBasisMatrices(UINT nCount) { PropertyPatch::SetNumBasisMatrices(nCount,VECTOR2BEZIERBASIS); }
   virtual void BuildQuadPatchTangents();
   virtual void BuildNPatchTangents();
   virtual void SetTessellationInternalEdge( int nSub, UINT nSubPatchIndex );

};
