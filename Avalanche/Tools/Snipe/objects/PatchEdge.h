#pragma once

#include "Component.h"
#include "Edge.h"
#include "TessEdge.h"
#include "GUI/GLVertexHandleViewControl.h"

class Vertex;
class VertexNormal;
class Poly;
class Edge;
class Patch;
class UV;

class VertexHandle : public Property
{
   friend class Edge;

public:
   static FloatProperty::Info m_fMagnitudeInfo;
   static FloatProperty::Info m_fAlphaInfo;
   static FloatProperty::Info m_fGammaInfo;

   FloatProperty   m_fMagnitude;
   FloatProperty   m_fAlpha;
   FloatProperty   m_fGamma;

public:
   Vector      m_vTangent;

   VertexHandle();
   VertexHandle(const VertexHandle &other);

   Vector       GetTangent() { return m_vTangent; }
   void         SetTangent(const Vector &vTangent) { m_vTangent = vTangent; }
   FloatProperty *GetMagnitude() { return &m_fMagnitude; }
   FloatProperty *GetAlpha() { return &m_fAlpha; }
   FloatProperty *GetGamma() { return &m_fGamma; }
};

class EdgePatchInfo : public EdgePolyInfo
{
public:
   // tessellation
   TessEdgeNormals m_tessedgenormals;
};

class PatchEdge : public Edge
{
   friend class PatchModel;
protected:
   static VertexHandle::Info m_firstVertexHandleInfo;
   static VertexHandle::Info m_secondVertexHandleInfo;

public:
   // tangents
   VertexHandle     m_firstVertexHandle;
   VertexHandle     m_secondVertexHandle;

   TessEdgeVertices m_alignedTessedgeVertices; // Needs to be here for Dangles
   TessEdgeVertices m_nonAlignedTessedgeVertices;

   PatchEdge       *m_pNextDirtyDangle, *m_pPrevDirtyDangle; // links all dirty dangles together for tessellation, starting from PatchModel::m_pDirtyDangleHead
 
   // cached edge for continuity
   PatchEdge *m_pPrevContEdge;
   PatchEdge *m_pNextContEdge;

   class RedirectInfo
   {
   public:
      PatchEdge *m_pFromEdge;
      PatchEdge *m_pToEdge;
      PatchEdge *m_pOriginalToEdge;
      PatchVertex *m_pSharedVertex;
      BOOL m_bReverseTo;

      RedirectInfo *m_pMirrorInfo;

      RedirectInfo( PatchEdge *pFromEdge, PatchEdge *pToEdge, PatchVertex *pSharedVertex );
      ~RedirectInfo();

      void Undo();
      void Redo();
   };

   DECLARE_COMPONENT_CREATE(PatchEdge, Edge)

	         PatchEdge();
            PatchEdge( UINT nID );
	virtual ~PatchEdge();

   void Init();

   Patch *GetPatch();
   Patch *GetConnectedPatch(Patch *pPatch);

   PatchModel *GetModel() { return (PatchModel *)GetOwner(); }

   PatchEdge *GetNextDirtyDangle() { return m_pNextDirtyDangle; }

   BOOL IsTessellationDirty() { return m_bTessellationDirty; }
   void SetTessellationDirty( BOOL bFlag );

   BOOL IsTangentsDirty() { return m_bTangentsDirty; }
   void SetTangentsDirty( BOOL bFlag );

   VertexHandle *GetVertexHandle(PatchVertex *pPatchVertex);

   VertexHandle *GetFirstVertexHandle(BOOL bBuildTangentIfDirty=TRUE);
   VertexHandle *GetSecondVertexHandle(BOOL bBuildTangentIfDirty=TRUE);
   VertexHandle *GetVertexHandleForVertex(Vertex *pVertex, BOOL bBuildTangentIfDirty=TRUE);

   VertexHandle *GetAlignedFirstVertexHandle( Poly *pPoly );
   VertexHandle *GetAlignedSecondVertexHandle( Poly *pPoly );

   Vector GetFirstTangentPos();
   Vector GetSecondTangentPos();
   Vector GetAlignedFirstTangentPos( Poly *pPoly );
   Vector GetAlignedSecondTangentPos( Poly *pPoly );

   EdgePatchInfo *GetEdgePatchInfo( Poly *pPoly );

   TessEdgeVertices *GetTessEdgeVertices(Patch *pPatch);
   TessEdgeVertices *GetAlignedTessEdgeVertices() { Tessellate(); return &m_alignedTessedgeVertices; }
   TessEdgeVertices *GetNonAlignedTessEdgeVertices() { Tessellate(); return &m_nonAlignedTessedgeVertices; }
   TessEdgeNormals *GetTessEdgeNormals(Patch *pPatch);
   
   UINT CalcEdgeSubdivisions(Patch **pHighOrderPatch=NULL);
   
   void SetTessellationAllocationDirty(BOOL bFlag);
   void SetTessellation();
   void ClearTessellation();

   BOOL IsFirstVertexConstrained() const { return m_bV0Constrained; }
   BOOL IsSecondVertexConstrained() const { return m_bV1Constrained; }
   BOOL IsVertexConstrained(Vertex *pVertex) const;

   void SetFirstVertexConstrainedFlag(BOOL bConstrained) { m_bV0Constrained = bConstrained; } // Don't call directly, call Vertex::SetConstrained
   void SetSecondVertexConstrainedFlag(BOOL bConstrained) { m_bV1Constrained = bConstrained; } // Don't call directly, call Vertex::SetConstrained

   // continuity
   PatchVertex *GetPrevContVertex() const;
   PatchVertex *GetNextContVertex() const;

   PatchEdge *GetPrevContEdge(BOOL bLoop);
   void SetPrevContEdge( PatchEdge *pEdge );
   PatchEdge *GetNextContEdge(BOOL bLoop);
   void SetNextContEdge( PatchEdge *pEdge );
   void SetContEdge( PatchVertex *pVertex, PatchEdge *pEdge );

   void BuildContinuity();
   BOOL IsContinuityLoop() const { return m_bContinuityLoop; }
   void SetContinuityLoop(BOOL bFlag) { m_bContinuityLoop = bFlag; }
   BOOL IsContinuityDirty() const { return m_bPrevContinuityDirty || m_bNextContinuityDirty; }
   void SetPrevContinuityDirty(BOOL bFlag) { m_bPrevContinuityDirty = bFlag; };
   void SetNextContinuityDirty(BOOL bFlag) { m_bNextContinuityDirty = bFlag; };
   BOOL IsAlignedWithContinuity() const;

   RedirectInfo *Redirect( PatchVertex *pTermVertex, String *pOutError=NULL );
   RedirectInfo *Redirect( PatchEdge *pToEdge, String *pOutError=NULL );

   void BuildTangentsFromVertexNormals();

   void DrawEdgeContinuity( CGLBaseView *pBaseView );

   Vector CalculateTangent( PatchVertex *pPatchVertex, Vector &vAlphaPlane, Vector &vGammaPlane );

// Overridables
   virt_base virtual void BuildTangents();
   virt_base virtual void Tessellate();
   virt_base virtual void AllocateTessellation();

// Edge overrides
   virtual void SetFirstVertex(Vertex *pVertex);
   virtual void SetSecondVertex(Vertex *pVertex);
   virtual BOOL AddPoly(Poly *pPoly,UINT nEdgeIndex);
   virtual void RemovePoly(Poly *pPoly, BOOL bRemoveIfNotUsed=TRUE);
   virtual void Draw( BOOL bDrawTess );
   virtual EdgePolyInfo *CreateNewEdgePolyInfo();
   virtual void SetDirty();
   virtual Vector GetPosAlongEdge( float fPosition, Poly *pPoly = NULL );
   virtual void FindClosestIntersection( CGLBaseView *pView, const CPoint &point, float &fOutPos );
   virtual Edge *GetContinuousEdge( Vertex *pVertex, float fAngleTolerance=50.0f );
   virtual void GetDisplayIDText(CGLModelView *pView, String &strText);
   virtual void ReverseSpline();
   virtual void SetFirstVertexPeakedFlag(BOOL bPeaked);// Don't call directly, call Vertex::Peak
   virtual void SetSecondVertexPeakedFlag(BOOL bPeaked);// Don't call directly, call Vertex::Peak
   virtual Vertex::DeleteInfo *SplitEdge( float fPosition=0.5f, Edge **pOutEdge0=NULL, Edge **pOutEdge1=NULL  );
   virtual UINT GetSaveFlags();
   virtual void ApplySaveFlags( UINT nFlags );

// Component overrides
   virtual void Disconnect();
   virtual void Reconnect();

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView );
   virtual void DrawNearestFocused( CGLBaseView *pView, SnipeObject *pFocusObject );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
};

#include "PatchEdge.hpp"