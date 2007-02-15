#pragma once
#include "PropertyPoly.h"
#include "TessPoly.h"

#include "..\Utility\DynamicMatrix.h"

class PropertyPatchEdge;
class TessPropertyEdge;

class BezierBasisMatrix
{
public:
   DynamicMatrix m_mat;
public:
            BezierBasisMatrix() {}
   virtual ~BezierBasisMatrix() {}
   DynamicMatrix *GetMatrix();
   virt_base virtual void CalculateInteriorTangents() {}
};

class VectorBezierBasisMatrix : public BezierBasisMatrix
{
protected:
   Vector m_cvs[16];

public:
            VectorBezierBasisMatrix();
            VectorBezierBasisMatrix &operator = ( const VectorBezierBasisMatrix &other );
   virtual ~VectorBezierBasisMatrix() {}

   void SetCVs(Vector *pCVs);
   Vector *GetCVs();
   Vector Calculate(float fS,float fT);
   virtual void CalculateInteriorTangents();
};

class Vector2BezierBasisMatrix : public BezierBasisMatrix
{
protected:
   Vector2 m_cvs[16];
   DynamicMatrix m_mat;

public:
            Vector2BezierBasisMatrix();
            Vector2BezierBasisMatrix &operator = ( const Vector2BezierBasisMatrix &other );
   virtual ~Vector2BezierBasisMatrix()
   {
   }

   void SetCVs(Vector2 *pCVs);
   Vector2 *GetCVs();
   Vector2 Calculate(float fS,float fT);
   virtual void CalculateInteriorTangents();
};

class Vector4BezierBasisMatrix : public BezierBasisMatrix
{
protected:
   Vector4 m_cvs[16];
   DynamicMatrix m_mat;

public:
            Vector4BezierBasisMatrix();
            Vector4BezierBasisMatrix &operator = ( const Vector4BezierBasisMatrix &other );
   virtual ~Vector4BezierBasisMatrix() {}

   void SetCVs(Vector4 *pCVs);
   Vector4 *GetCVs();
   Vector4 Calculate(float fS,float fT);
   virtual void CalculateInteriorTangents();
};

class PropertyPatch : public PropertyPoly
{
protected:
   enum BezierBasisMatrixType { NONE, VECTORBEZIERBASIS, VECTOR2BEZIERBASIS, VECTOR4BEZIERBASIS };

   BezierBasisMatrix **m_pBezierBasisArray;
   TessPropertyPoly *m_pTessPropertyPolyArray;
   TessPropertyEdge *m_pInternalTessPropertyEdges;// internal npatch edges

   UINT        m_nNumTessPropertyPolys              : 16,
               m_bDirtyTangents                     : 1;


public:
   DECLARE_COMPONENT_CREATE( PropertyPatch, PropertyPoly )

   PropertyPatch();
   PropertyPatch( UINT nID );
   virtual ~PropertyPatch();

   void Init();

   Patch *GetPatch() { return (Patch *)GetPoly(); }
   PropertyPatch *GetNextPropertyPatch() const { return (PropertyPatch *)GetNextPropertyPoly(); }

   BOOL IsTangentsDirty() { return m_bDirtyTangents; }
   void SetTangentsDirty( BOOL bDirty );

   PropertyPatchEdge *GetPropertyPatchEdgeAt( UINT nIndex ) { return (PropertyPatchEdge *)GetPropertyEdgeAt( nIndex ); }

   int GetNumTessPropertyPolys() { return m_nNumTessPropertyPolys; }
   TessPropertyPoly *GetTessPropertyPolyAt( UINT nIndex );

   DynamicMatrix *GetMatrix( UINT nIndex ) { return m_pBezierBasisArray[nIndex]->GetMatrix(); }
   void CalculateInteriorTangents( UINT nIndex ) { m_pBezierBasisArray[nIndex]->CalculateInteriorTangents(); }

   // vector
   void SetVectorCVs( Vector *pCVs, UINT nIndex ) { ((VectorBezierBasisMatrix*)m_pBezierBasisArray[nIndex])->SetCVs( pCVs ); }
   Vector *GetVectorCVs( UINT nIndex );
   Vector CalculateVector( float fS, float fT, UINT nIndex );

   // vector2
   void SetVector2CVs( Vector2 *pCVs, UINT nIndex ) { ((Vector2BezierBasisMatrix*)m_pBezierBasisArray[nIndex])->SetCVs( pCVs ); }
   Vector2 *GetVector2CVs( UINT nIndex );
   Vector2 CalculateVector2( float fS, float fT, UINT nIndex );

   // vector4
   void SetVector4CVs( Vector4 *pCVs, UINT nIndex ) { ((Vector4BezierBasisMatrix*)m_pBezierBasisArray[nIndex])->SetCVs( pCVs ); }
   Vector4 *GetVector4CVs( UINT nIndex );
   Vector4 CalculateVector4( float fS, float fT, UINT nIndex );

   TessPropertyPoly *AddTessellatedPropertyPolys( UINT nCount, TessPoly *pTessPolyArray );
   void FillOutTessPropertyPolysPropertyVertices( TessPropertyPoly *pTessPropertyPoly, UINT nSubPatchIndex, int u, int v, int usubdivisions, int vsubdivisions,
   TessPropertyEdge *pLeftTessPropertyEdge,  BOOL bLeftReversed,  TessPropertyEdge *pBottomTessPropertyEdge, BOOL bBottomReversed,
   TessPropertyEdge *pRightTessPropertyEdge, BOOL bRightReversed, TessPropertyEdge *pTopTessPropertyEdge, BOOL bTopReversed );

   void SetNumBasisMatrices( UINT nCount, BezierBasisMatrixType nType );
   BezierBasisMatrix *GetBasisMatrix( UINT nIndex ) { return m_pBezierBasisArray[nIndex]; }

   void BuildTangents();

   void AllocateTessellation( int nColumns,int nRows );
   void SetNumInternalTessPropertyEdges(UINT nSize);
   void ClearInternalTessPropertyEdges();
   void AllocateTessellationInternalEdge( int nSub, PropertyVertex **pCenterPropertyVertex, UINT nSubPatchIndex, UINT nNPatchHookIndex );
   void BuildQuadPatchTessPropertyPoly( TessPoly *pTessPoly, int u, int v, int usubdivisions, int vsubdivisions );
   void BuildNPatchTessPropertyPoly( TessPoly *pTessPoly, UINT nPreviousSubPatchIndex,UINT nSubPatchIndex, int u, int v, int usubdivisions, int vsubdivisions );

   void ClearTessellatedPropertyPolys();
   void ClearBezierBasisMatrices();
   void ClearTessellatedPropertyVertices();

   // Overridables
   virt_base virtual void SetTessellation( int nUSubdivisions, int nVSubdivisions ) {}
   virt_base virtual void SetNumBasisMatrices( UINT nCount ) {}
   virt_base virtual void BuildQuadPatchTangents() {}
   virt_base virtual void BuildNPatchTangents() {}
   virt_base virtual void SetTessellationInternalEdge( int nSub, UINT nSubPatchIndex ) {}

   // PropertyPoly Overrides
   virtual void ReverseWinding();

   // Component overrides
   virtual void Disconnect();
};

#include "PropertyPatch.hpp"