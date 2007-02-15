#pragma once

#include "Component.h"
#include "Poly.h"
#include "TessPoly.h"
#include "PatchVertex.h"
#include "PatchEdge.h"
#include "QuadPatch.h"
#include "UVPatch.h"

class PatchVertex;
class PatchEdge;
class VertexHandle;
class UVSubPatchHead;

class SubPatchEdge
{
protected:
   TessEdgeVertices m_tessedgevertices;
   TessEdgeNormals m_tessedgenormals;

public:
   SubPatchEdge() {}
   ~SubPatchEdge() {}
   TessEdgeVertices *GetTessEdgeVertices() { return &m_tessedgevertices; }
   TessEdgeNormals *GetTessEdgeNormals() { return &m_tessedgenormals; }
};

class NPatchData
{
protected:
   SubPatchEdge *m_pSubPatchEdges;

   union {
      ULONG m_nNPatchDataFlags;
      struct {
         ULONG 
               m_nNumSubPatches               : 8;
      };      
   };

public:
   NPatchData() 
   { 
      m_nNPatchDataFlags = 0; 
      m_pSubPatchEdges = NULL;
   }
   ~NPatchData();

   UINT GetNumSubPatchEdges() { return m_nNumSubPatches; }
   void SetNumSubPatchEdges(UINT nNumEdges);
   SubPatchEdge *GetSubPatchEdgeAt(UINT nIndex) { return &m_pSubPatchEdges[nIndex]; }

   TessEdgeVertices *GetTessEdgeVertices(UINT nIndex) { return m_pSubPatchEdges[nIndex].GetTessEdgeVertices(); }
   TessEdgeNormals *GetTessEdgeNormals(UINT nIndex) { return m_pSubPatchEdges[nIndex].GetTessEdgeNormals(); }
};

class Patch : public Poly
{
   friend class PatchModel;
protected:
   CArray<QuadPatch> m_quads;
   Patch    *m_pNextDirtyPatch, *m_pPrevDirtyPatch; // links all dirty patches together for tessellation, start from PatchModel::m_pDirtyPatchHead
   TessPoly *m_tesspolyarray;

   UINT m_nNumTessPolys   : 16,
        m_nMaxSubdivision : 8;

   NPatchData *m_pNPatchData;

public:
   DECLARE_COMPONENT_CREATE(Patch, Poly)

            Patch();
            Patch( UINT nID );
   virtual ~Patch();

   void Init();

   PatchModel *GetModel() { return (PatchModel *)GetOwner(); }

   BOOL IsQuadPatch();
   BOOL IsNPatch();

   void SetNumSubPatches(UINT nSize);
   UINT GetNumSubPatches() const;

   Patch *GetNextDirtyPatch() { return m_pNextDirtyPatch; }

   Vector *GetCVs(UINT nSubPatchIndex);
   Vector *GetVNCVs(UINT nSubPatchIndex);

   void SetMaxSubdivision(BYTE nMaxSubdivision);
   BYTE GetMaxSubdivision();

   BOOL IsTangentsDirty() { return m_bDirtyTangents; }
   void SetTangentsDirty( BOOL bFlag );

   BOOL IsTessellationDirty() { return m_bDirtyTessellation; }
   void SetTessellationDirty( BOOL bFlag );

   BOOL IsTessellationAllocationDirty() { return m_bDirtyTessellationAllocation; }
   void SetTessellationAllocationDirty(BOOL bFlag);

   PatchVertex *GetVertexAt( UINT nIndex ) const { return (PatchVertex *)Poly::GetVertexAt( nIndex ); }
   PatchEdge *GetEdgeAt( UINT nIndex );
 
   // tessellated polys
   TessPoly *AddTessellatedPolys( UINT nCount );
   void ClearTessellatedPolys();
   UINT GetNumTessPolys( BOOL bTessellateIfNeeded=TRUE );
   TessPoly *GetTessPolyAt( UINT nIndex );

   void BuildTangents();
   void BuildQuadPatchTangents();
   void BuildNPatchTangents();
   void BuildInteriorTangents(UINT nSubPatchIndex);

   DynamicMatrix *GetVertexMatrix(UINT nSubPatchIndex) { return &m_quads[nSubPatchIndex].m_vmat; }
   DynamicMatrix *GetVertexNormalMatrix(UINT nSubPatchIndex) { return &m_quads[nSubPatchIndex].m_vnmat; }

   Vector CalculatePosition(UINT nSubPatchIndex, float fS, float fT);

   void Tessellate();
   void TessellateNoSubdivisions();
   void TessellateWithSurrounding();

   void BuildQuadPatchTessPolys(int nUSubdivisions,int nVSubdivisions);
   void BuildNPatchTessPolys(int nUSubdivisions,int nVSubdivisions);
   void AllocateTessellation(int nColumns,int nRows);
   void SetTessellation(int nUSubdivisions, int nVSubdivisions);
   
   void CalcPatchSubdivisions(UINT &nSubU,UINT &nSubV);
   UINT CalcUSubdivisions();
   UINT CalcVSubdivisions();

   void BuildQuadTessPoly(
      TessPoly *pPoly,
      int u, int v,
      int usubdivisions, int vsubdivisions);

   void BuildNPatchTessPoly(
      TessPoly *pPoly,
      UINT nSubPatchIndex,
      int u, int v,
      int usubdivisions, int vsubdivisions);

   void FillOutVerticesAndVertexNormalsTessPoly(
      TessPoly *pPoly,
      UINT nSubPatchIndex,
      int u, int v,
      int usubdivisions, int vsubdivisions,
      TessEdgeVertices *pLeftTessEdgeVertices,
      TessEdgeVertices *pBottomTessEdgeVertices,
      TessEdgeVertices *pRightTessEdgeVertices,
      TessEdgeVertices *pTopTessEdgeVertices,
      TessEdgeNormals *pLeftTessEdgeNormals,
      TessEdgeNormals *pBottomTessEdgeNormals,
      TessEdgeNormals *pRightTessEdgeNormals,
      TessEdgeNormals *pTopTessEdgeNormals);

   void BuildSharedEdgeInfo(int nNumSteps,
      PatchVertex *pFirstVertex, PatchVertex *pSecondVertex,
      VertexNormal *pFirstVertexNormal, VertexNormal *pSecondVertexNormal,
      CArray<UV *> &firstuvs,CArray<UV *> &seconduvs,  
      TessEdgeVertices **pTessEdgeVertices,
      TessEdgeNormals **pTessEdgeNormals);

   NPatchData *GetNPatchData();
   void AllocateTessellationInternalEdge( int nSub,UINT nSubPatchIndex, PatchVertex *pEdgeCenterVertex, PatchVertex *pCenterVertex,
         VertexNormal *pEdgeCenterVertexNormal, VertexNormal *pCenterVertexNormal );
   void SetTessellationInternalEdge( int nSub,UINT nSubPatchIndex );
   void CalculateNPatchCenter( Vector &center,Vector &centernormal );
   void DrawInternalEdges();
   void AllocateTessellationInternalEdges( int nSub );
   void SetTessellationInternalEdges( int nSub );
   void DirtyTessellationAllocation();
   void ClearTessellatedComponents();

   UINT GetNextSubPatchIndex( UINT nIndex ) const;
   UINT GetPrevSubPatchIndex( UINT nIndex ) const;

// Poly overrides
   virtual void SetNumSides( UINT nNumSides );
   virtual void Draw( const Matrix44 &viewmatrix, BOOL bDrawTess, SurfaceDriver *pSurfaceDriver=NULL );
   virtual Edge *FindEdgeClosestToPoint( const Vector &point );
   virtual void ReverseWinding();
   virtual void SetVertexAt( UINT nIndex, Vertex *pVertex );
   virtual BOOL SetEdgeAt( UINT nIndex, Edge *pEdge );
   virtual void SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal );
   virtual void FlipNormal();
   virtual void DrawPickNearest( Edge *pInEdge );

   // property poly
   virtual void AddPropertyPoly( PropertyPoly *pPropertyPoly );
   virtual BOOL RemovePropertyPoly( PropertyPoly *pUVPoly );

// Component overrides
   virtual void Reconnect();
   virtual void Disconnect();

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView );
};

#include "Patch.hpp"
