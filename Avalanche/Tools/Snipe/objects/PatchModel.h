#pragma once
#include "PolyModel.h"
#include "Patch.h"
#include "Interface/MainInterface.h"

class PatchModel : public PolyModel
{
friend class Patch;
protected:
   // Statistics
      static StatisticProperty::Info m_nTessPolysStatInfo;
      static StatisticProperty::Info m_nQuadsStatInfo;
   static Property::Info        m_categorySubdivisionInfo;
      static BoolProperty::Info    m_bFixedSubdivisionsInfo;
      static IntProperty::Info     m_nPatchSubdivisionsInfo;
      static FloatProperty::Info   m_fPatchToleranceInfo;
   static Property::Info        m_categoryContinuityInfo;
      static FloatProperty::Info   m_fContinuityToleranceInfo;

public:
   DECLARE_SNIPEOBJECT_CREATE(PatchModel, PolyModel)

   // Statistics
      StatisticProperty m_nTessPolysStat;
      StatisticProperty m_nQuadsStat;
   Property       m_categorySubdivision;
      BoolProperty   m_bFixedSubdivisions;
      IntProperty    m_nPatchSubdivisions;
      FloatProperty  m_fPatchTolerance;
   Property       m_categoryContinuity;
      FloatProperty  m_fContinuityTolerance;

   Patch         *m_pDirtyPatchHead; // linked list of patches that need tessellated
   PatchEdge     *m_pDirtyDangleHead; // linked list of DANGLES that need tessellated

   UINT m_nNumTessPolys;
   UINT *m_pConstrainedRTVertices;
   UINT  m_nNumConstrainedRTVertices;

            PatchModel();
   virtual ~PatchModel();

   void Clear();

   PatchModel *GetRootCache() { return (PatchModel *)SceneObject::GetRootCache(); } // Note: returns this if it is the cache
   PatchModel *GetCache() { return (PatchModel *)SceneObject::GetCache(); } // Note: returns this if it is the cache

   UINT GetNumPatches();
   Patch *AddPatch();
   Patch *AddPatch(UINT nID);
   void RemovePatch(Patch *pPatch);
   Patch *GetNextFace( POSITION &pos ) { return (Patch *)PolyModel::GetNextFace( pos ); }

   PatchEdge *GetNextEdge( POSITION &pos ) { return (PatchEdge *)PolyModel::GetNextEdge( pos ); }

   // tessellation access functions
   
   PatchVertex *AddTessellatedVertex();
   PatchVertex *AddTessellatedVertex( const Vector &pos );
   PatchVertex *AddTessellatedVertices( UINT nSize );
   VertexNormal *AddTessellatedVertexNormal();
 //  VertexNormal *AddTessellatedVertexNormal( const Vector &direction );
   VertexNormal *AddTessellatedVertexNormals( UINT nSize );

   static void DrawBezierSplineSegment(UINT nMaxLevel, UINT nLevel, const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3);

   // draw
   void DrawVertexNormals( CGLModelView *pModelView );
   void DrawHiddenEdges( CGLModelView *pModelView );

   void DirtyTessellationAllocation();

   UINT GetNumTessPolys() { return m_nNumTessPolys; }

   void DrawContinuity( CGLModelView *pModelView );
   void CreateConstrainedRTVertices();
   void FreeConstrainedRTVertices();

   void BuildContinuity();

// Overridables
   virt_base virtual void DrawControlVertices();

// PolyModel overrides
   virtual Vertex *CreateVertex( UINT nID=-1 ) const;
   virtual Edge *CreateEdge( UINT nID=-1 ) const;
   virtual Poly *CreatePoly( UINT nID=-1 ) const;
   virtual void DrawVertexIndices( CGLModelView *pModelView );
   virtual void ConvertFrom(HierObject *pHierObject);
   virtual void FreeRTVertices();
   virtual void CreateRTVertices( BOOL bTessellated );
   virtual BOOL CreateRTQuads( CompositedSurface *pCompositedSurface, BOOL bTessellated );
   virtual void DrawRetainedMode( CGLModelView *pModelView );
   virtual Edge *AddEdgeBetween( Vertex *pV0, Vertex *pV1, Edge **pOutReversedEdge=NULL, Edge *pPrevEdge=EDGE_FINDCONTINUOUS, Edge *pNextEdge=EDGE_FINDCONTINUOUS );

// InstanceObject overrides
   virtual void CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount );
   virtual void Draw( CGLBaseView *pBaseView );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
};

#include "PatchModel.hpp"
