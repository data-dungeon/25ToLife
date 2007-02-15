#pragma once

#include "Vertex.h"

class TessPoly;

class PatchVertex : public Vertex
{
public:
   class WeldInfo : public Vertex::WeldInfo
   {
   public:
      virtual void ReWeld();
      virtual void UnWeld();
   };

   TessPoly **m_pSecondarySurroundingPolys;
   float m_fUWeight,m_fVWeight;
   char m_nNumSecondarySurroundingFaces, m_nNumMaxSecondarySurroundingFaces;

   DECLARE_COMPONENT_CREATE(PatchVertex, Vertex)

            PatchVertex();
            PatchVertex( UINT nID );
   virtual ~PatchVertex();

   BOOL CanConstrain( PatchEdge *pEdge );
   BOOL SetConstrained( PatchEdge *pEdge );
   BOOL IsConstrained() const { return m_bContrained; }
   BOOL GetConstrainedBetween( PatchEdge *&pEdge0, PatchEdge *&pEdge1);
   BOOL GetConstrainedBetween( PatchVertex *&pVertex0, PatchVertex *&pVertex1);
   BOOL ComputeConstrainedPos();
   BOOL GetTwoContinuousUnConstrainedVertices( PatchEdge *pEdge, PatchVertex *&pOutPrevVertex, PatchVertex *&pOutNextVertex, PatchEdge *&pOutPrevEdge, PatchEdge *&pOutNextEdge, float &pOutConstrainedPos );

   BOOL IsNPatchHook() { return m_bNPatchHook; }
   void SetNPatchHook(BOOL bFlag) { m_bNPatchHook = bFlag; }

   float GetUWeight() { return m_fUWeight; }
   float GetVWeight() { return m_fUWeight; }
   void SetUVWeight(float fUWeight,float fVWeight) { m_fUWeight = fUWeight; m_fVWeight = fVWeight; }

   // secondary neighbors
   UINT GetNumSecondarySurroundingPolys() const;
   void AddSecondarySurroundingPoly( TessPoly *pPoly );
   TessPoly *GetSecondarySurroundingPolyAt( UINT nIndex ) const;
   void RemoveSecondarySurroundingPolyAt( UINT nIndex );
   void RemoveSecondarySurroundingPoly( TessPoly *pPoly );
   void CalcSecondaryNormal(VertexNormal *pVertexNormal);

   // Vertex Overrides
   virtual void SetDirty();
   virtual BOOL GetTangents(Poly *pPoly,Vector &v0,Vector &v1);
   virtual void Compact();
   virtual const Vector &GetPos();
   virtual void DirtyDependentEdges(Edge *pNotThis=NULL);
   virtual Vertex::WeldInfo *AllocWeldInfo();

   // SnipeObject overrides
   virtual void OnFocusChanged( BOOL bFocus );
   virtual void Draw( CGLBaseView *pView );
   BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
};

#include "PatchVertex.hpp"
